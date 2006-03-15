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

//#include <TestHelloData.h>
#include <ccpp_Basic.h>

#include "HelloInterface_impl.hpp"

//#include <dds_dcps.h> 

using namespace std;
using namespace DDS;
using namespace CORBA;
using namespace Basic;


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
                  DomainId_t myDomain;
		  // Splice DomainParticipantFactory
                  DomainParticipantFactory_ptr dpf;
		  // Splice domainParticipant
                  DomainParticipant_ptr dp;
		  // Splice Topic
                  Topic_ptr t;
		  // DCPS Subscriber
                  Subscriber_ptr s;
		  // DCPS Data Reader
                  DataReader_ptr dr;
                  BasicTypeDataReader_ptr fdr;
                  BasicTypeTypeSupport dt;
                  BasicTypeSeq_var dataList;
                  SampleInfoSeq_var infoList;
};


#endif // INCL_DEMO_FT_RECOVERY_MECHANISM_HPP


