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


#include "testtracelibrary/PILOT_FlushAreaMngr.hpp"

#include "TraceAndPerf/tracelibrary/FlushAreaMngr.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
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


PILOT_FlushAreaMngr::PILOT_FlushAreaMngr()
    : Testable("Cdmw::FlushAreaMngr")
{

}


PILOT_FlushAreaMngr::~PILOT_FlushAreaMngr()
    throw()
{

}



void
PILOT_FlushAreaMngr::do_tests()
    throw()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (21);
    
    TEST_INFO( "Starting the Orb in it's own thread" );
    OrbThread orbThread;
    orbThread.start();

	CORBA::ORB_var orb = orbThread.get_orb();
    PortableServer::POA_var rootPOA = orbThread.get_poa();  

	// create messages
    std::string message1 = "First Message";
    std::string message2 = "Second Message";
    std::string message3 = "Third Message";

    // constructs a message header
    Message::MessageHeader header;
    header.m_fileLine = __LINE__;
    header.m_fileName = __FILE__;
    header.m_userLevel = 1;
    header.m_userDomain = "TEST";
    header.m_tid = OsSupport::Thread::self();
    header.m_timestamping = OsSupport::OS::get_time();

	size_t headerStringSize = header.m_fileName.length() + 1;
	headerStringSize += header.m_userDomain.length() + 1;

	// create the initial collector list set to empty
	std::list<CdmwTrace::TraceProducer::CollectorData> collectorList;
	
    size_t flushingTime = 2000; // 2 seconds

    TEST_INFO("Initializing the TraceLibrary");
    CdmwTrace::TraceProducer_var traceProducer = 
        FlushAreaMngr::init (rootPOA.in(),
						     collectorList,
						     CdmwTrace::ALL_DOMAINS, 
						     CdmwTrace::ALL_VALUES,
						     "application_name",
							 "process_name",
                             flushingTime,
                             2,                    // 2 FlushArea
                             20 + headerStringSize // Each FlushArea has 20 octets
                             );
    
    TEST_CHECK (!CORBA::is_nil(traceProducer.in()));
    TEST_SUCCEED();
    
    TEST_INFO ("Retrieving the FlushAreaMngr singleton");
    FlushAreaMngr& flushAreaMngr=FlushAreaMngr::instance();
	flushAreaMngr.deactivate_flushing();      // deactivate flushing to perform test
    TEST_SUCCEED();


           

    TEST_INFO ("Creating a fake collector");
	SIM_Collector* pSim_CollectorServant = new SIM_Collector; 
	PortableServer::ServantBase_var servant_var = pSim_CollectorServant;

    CdmwTrace::Collector_var collector = pSim_CollectorServant->_this(); 

    TEST_INFO ("Registering the collector into the TraceProducer object");
    traceProducer->register_collector (collector.in(), "collector_name", "collector_mnemonic");
    TEST_SUCCEED();


    bool ret;

    
    TEST_INFO ("Adding a first message");    
    ret = flushAreaMngr.add_message (header, message1);
    TEST_CHECK (ret == true);

    TEST_INFO ("Adding a second message");    
    ret = flushAreaMngr.add_message (header, message2);
    TEST_CHECK (ret == true);

    // as the flush area manager contains only 2 flush areas,
    // the third message cannot be added => error message is generated
    TEST_INFO ("Adding a third message");    
    ret = flushAreaMngr.add_message (header, message3);
    TEST_CHECK (ret == false);
    
    {
        TEST_INFO ("Checking if the Collector has not received messages");
        const CdmwTrace::FormattedMessageSeq& formattedMessagesSeq = 
			                   pSim_CollectorServant->get_FormattedMessageSeq();
	    CORBA::ULong nbMessages = formattedMessagesSeq.length();
     
        TEST_CHECK (nbMessages == 0);
    }
    
    flushAreaMngr.activate_flushing();      // activate flushing

	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    TEST_INFO ("Waiting for a free flush area to add a third message");    
    do
    {
        OsSupport::OS::sleep(timescale*flushingTime);
        ret = flushAreaMngr.add_message (header, message3);
        // as the flush area manager contains only 2 flush areas,
        // the third message will be added only after a flush area 
        // had been released by the area flusher
    }
    while (!ret);

    TEST_INFO ("Third message added to the flush area manager");    


    TEST_INFO ("Waiting a while to let the AreaFlusher does its work");
    OsSupport::OS::sleep(timescale*5*flushingTime);

    
    TEST_INFO ("Checking if the Collector has received messages");
    const CdmwTrace::FormattedMessageSeq& formattedMessagesSeq = 
			                   pSim_CollectorServant->get_FormattedMessageSeq();
	CORBA::ULong nbMessages = formattedMessagesSeq.length();
    TEST_INFO ("Nb received messages : " << nbMessages);
    TEST_INFO ("Received messages : ");
	for (CORBA::ULong i = 0 ; i < nbMessages ; i++)
	{
	  TEST_INFO ("  " << formattedMessagesSeq[i].message.in());
	}
	// note : third message is "running out of flush areas"
    TEST_CHECK (nbMessages == 4);


    TEST_INFO("Checking if an unexisting domain/level is to be traced");
	traceProducer->deactivate_level (CdmwTrace::ALL_DOMAINS, CdmwTrace::ALL_VALUES);
	traceProducer->activate_level ("domain1", 1);
    TEST_CHECK (FlushAreaMngr::is_to_be_traced("foo", 25) == false);

    TEST_INFO ("Trying to activate a domain/level using the TraceProducer interface");
    traceProducer->activate_level ("foo", 25);
    TEST_SUCCEED();

    TEST_INFO("Checking if the level is returned activated by");
    TEST_INFO("     the FilterMngr");
    TEST_CHECK (FlushAreaMngr::is_to_be_traced("foo", 25) == true);

    
    TEST_INFO ("Trying to deactivate a domain/level using the TraceProducer interface");
    traceProducer->deactivate_level ("foo", 25);
    TEST_SUCCEED();
    
    TEST_INFO("Checking if the level is returned deactivated by");
    TEST_INFO("     the FilterMngr");
    TEST_CHECK(FlushAreaMngr::is_to_be_traced("foo", 25) == false);

    TEST_INFO("Doing the test for different Domain/level");

    traceProducer->activate_level("foo", 2);
    traceProducer->activate_level("foo", 5);
    TEST_CHECK (FlushAreaMngr::is_to_be_traced("foo", 2) == true);
    TEST_CHECK (FlushAreaMngr::is_to_be_traced("foo", 5) == true);

    traceProducer->deactivate_level ("foo", 2);
    TEST_CHECK (FlushAreaMngr::is_to_be_traced("foo", 2) == false);

    traceProducer->activate_level ("bar", 5);
    TEST_CHECK (FlushAreaMngr::is_to_be_traced("bar", 5) == true);

    traceProducer->activate_level ("bar", 4);
    TEST_CHECK (FlushAreaMngr::is_to_be_traced("bar", 4) == true);

    traceProducer->deactivate_level ("bar", 5);
    TEST_CHECK (FlushAreaMngr::is_to_be_traced("bar", 5) == false);

    TEST_INFO ("Trying to cleanup the TraceLibrary");
    FlushAreaMngr::cleanup();
    TEST_SUCCEED();
    
    TEST_INFO ("Stopping the ORB");
    orbThread.shutdown();
    orbThread.join();
}



}; // End namespace Trace
}; // End namespace Cdmw

