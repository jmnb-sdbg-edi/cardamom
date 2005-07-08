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


#ifndef INCL_DEMO_FT_RECOVERY_MECHANISM_HPP
#define INCL_DEMO_FT_RECOVERY_MECHANISM_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>

#include <FaultTolerance/idllib/FT.skel.hpp>

#include <TestHelloData.h>

#include "HelloInterface_impl.hpp"


// This Recovery Mechanism implements the FT::Checkpointable interface
// to receive new state from the primary's Logging Mechanism.
// It only stores the state to update a HelloInterface servant when requested. 

class RecoveryMechanism 
{

    public:
    
        // Constructor
        RecoveryMechanism(int argc, char *argv[]);

		  // Destructor
		  virtual ~RecoveryMechanism();
        
        // update the state of a HelloInterface servant
        virtual void update_state(HelloInterface_impl* hello) 
            throw (CORBA::SystemException,
                   FT::InvalidState);

        
    private:
        
		  // Splice DomainParticipantFactory
		  //DDS::SpliceDomainParticipantFactory_var m_factory;

		  // Splice domainParticipant
		  DDS::DomainParticipant_var m_participant;

		  // Splice Topic
		  DDS::Topic_var m_topic;
        
		  // DCPS Subscriber
		  DDS::Subscriber_var m_subscriber;

		  // DCPS Data Reader
		  DDS::DataReader_var                  m_data_reader;
		  DemoFTDCPS::BasicTypesDataReader_ptr  m_typed_data_reader;
};


#endif // INCL_DEMO_FT_RECOVERY_MECHANISM_HPP


