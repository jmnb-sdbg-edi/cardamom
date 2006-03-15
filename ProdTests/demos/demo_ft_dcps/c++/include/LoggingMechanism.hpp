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


#ifndef INCL_DEMO_FT_LOGGING_MECHANISM_HPP
#define INCL_DEMO_FT_LOGGING_MECHANISM_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>

//#include <TestHelloData.h>
//#include <DemoFTDCPS.h>

#include <ccpp_Basic.h>
//DemoFTDCPS.h>

//#include <dds_dcps.h> 

using namespace Basic;
using namespace DDS;
using namespace CORBA;

// This Logging Mechanism dispatch a new state to registered replicas

class LoggingMechanism {
    
    public:
    
        // Constructor
        LoggingMechanism(int argc, char *argv[]);

        // Destructor
	~LoggingMechanism();
            
        // To dispatch a new state to registered replicas 
        void log_state(const FT::State& state);
    
    
    private:
 
		  // Splice DomainParticipantFactory 
                  DomainId_t myDomain;
                  DomainParticipantFactory_ptr dpf;

		  // its domainParticipant
                  DomainParticipant_ptr dp;

		  // myTopic
                  Topic_ptr t;
		  
		  // DCPS publisher
                  Publisher_ptr p;

		  // DCPS data writer
                  DataWriter_ptr dw;
                  BasicTypeDataWriter_ptr fdw;
                  BasicTypeTypeSupport dt;
};


#endif // INCL_DEMO_FT_LOGGING_MECHANISM_HPP

