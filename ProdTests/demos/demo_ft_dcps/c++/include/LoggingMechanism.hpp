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

#include <TestHelloData.h>
//DemoFTDCPS.h>

#include <dds_dcps.h> 


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
		  //DDS::SpliceDomainParticipantFactory_var m_factory;

		  // its domainParticipant
		  DDS::DomainParticipant_var m_participant;

		  // myTopic
		  DDS::Topic_var m_topic;
		  
		  // DCPS publisher
		  DDS::Publisher_var m_publisher;

		  // DCPS data writer
		  DDS::DataWriter_ptr                  m_data_writer;
		  DemoFTDCPS::BasicTypesDataWriter_var  m_typed_data_writer;
};

#endif // INCL_DEMO_FT_LOGGING_MECHANISM_HPP

