/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */


#include "testtracelibrary/PILOT_Trace.hpp"

#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"
#include "testtracelibrary/SIM_Collector.hpp"
#include "testtracelibrary/SIM_OrbThread.hpp"

namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/


PILOT_Trace::PILOT_Trace()
    throw()
    : Testable("Cdmw::TraceMacro")
{

}


PILOT_Trace::~PILOT_Trace()
    throw()
{

}




void
PILOT_Trace::do_tests()
        throw()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (9);

    TEST_INFO("Starting the ORB in its own thread");
    OrbThread orbThread;
    orbThread.start();

	CORBA::ORB_var orb = orbThread.get_orb();
	PortableServer::POA_var rootPOA = orbThread.get_poa();  

	// create the initial collector list set to empty
	std::list<CdmwTrace::TraceProducer::CollectorData> collectorList;
 		
	
    size_t flushingTime = 2000; // 2 seconds

    TEST_INFO("Trying to initialise the TraceLibrary");
    CdmwTrace::TraceProducer_var traceProducer =
            FlushAreaMngr::init (rootPOA.in(),
							     collectorList,
							     CdmwTrace::ALL_DOMAINS, 
							     CdmwTrace::ALL_VALUES,
							     "application_name",
								 "process_name",
                                 flushingTime,   // We flush each 5 seconds
                                 2,      // 2 FlushArea
                                 200     // Each FlushArea has 200 octets
                                );
    TEST_SUCCEED();
    TEST_CHECK (! CORBA::is_nil (traceProducer.in()));

	TEST_INFO ("Retrieving the FlushAreaMngr singleton");
	FlushAreaMngr& flushAreaMngr=FlushAreaMngr::instance();
	flushAreaMngr.activate_flushing();      // activate flushing to perform test
	TEST_SUCCEED();

    TEST_INFO("Creating a fake collector");
	SIM_Collector* pSim_CollectorServant = new SIM_Collector; 
	PortableServer::ServantBase_var servant_var = pSim_CollectorServant;

    CdmwTrace::Collector_var collector = pSim_CollectorServant->_this(); 

    TEST_INFO("Trying to register the collector into the TraceProducer object");
    traceProducer->register_collector (collector.in(), "collector_name", "collector_mnemonic");
    TEST_SUCCEED();

    TEST_INFO ("Using the TraceProducer interface to activate the domain/level");
    CORBA::String_var strDomain = CORBA::string_dup(CDMW_NREP);
    traceProducer->activate_level (strDomain.in(), INF);

    TEST_INFO ("Registering the tracing thread");
    CDMW_TRACE_REGISTER();

    TEST_INFO ("Tracing a first message");
    CDMW_TRACE (CDMW_NREP,INF, "foo" << 25);

    TEST_INFO ("Unregistering the tracing thread");
    CDMW_TRACE_UNREGISTER();

    TEST_INFO ("Creating a stream on the stack");
    Stream stream;
    TEST_SUCCEED();
    
    TEST_INFO ("Tracing a second message using the local stream");
    CDMW_TRACE_ST (stream, CDMW_NREP, INF, "bar" << 25);

    TEST_INFO ("Waiting a while to let the AreaFlusher does its work");
	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OsSupport::OS::sleep(timescale*5*flushingTime);

    TEST_INFO ("Checking if the Collector has received message");
    const CdmwTrace::FormattedMessageSeq& formattedMessagesSeq = 
			             pSim_CollectorServant->get_FormattedMessageSeq();
	CORBA::ULong nbMessages = formattedMessagesSeq.length();
	TEST_INFO ("Nb received messages : " << nbMessages);
	TEST_INFO ("Received messages : ");
	for (CORBA::ULong i = 0 ; i < nbMessages ; i++)
	{
	  TEST_INFO ("  " << formattedMessagesSeq[i].message.in());
	}

    TEST_CHECK (nbMessages == 2);

    TEST_INFO ("Checking the content of the first message");
    TEST_CHECK (::strcmp(formattedMessagesSeq[0].message.in(),"foo25") == 0);

    TEST_INFO ("Checking the content of the second message");
    TEST_CHECK (::strcmp(formattedMessagesSeq[1].message.in(),"bar25") == 0);
    
    TEST_INFO ("Cleaning the TraceLibrary");
    FlushAreaMngr::cleanup();
    TEST_SUCCEED();

    TEST_INFO ("Stopping the orb");
    orbThread.shutdown();
    orbThread.join();
}        



}; // End namespace Trace
}; // End namespace Cdmw

