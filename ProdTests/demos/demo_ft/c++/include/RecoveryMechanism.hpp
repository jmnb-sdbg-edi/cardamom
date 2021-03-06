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

#include "HelloInterface_impl.hpp"


// This Recovery Mechanism implements the FT::Checkpointable interface
// to receive new state from the primary's Logging Mechanism.
// It only stores the state to update a HelloInterface servant when requested. 

class RecoveryMechanism: public virtual POA_FT::Checkpointable,
                         public virtual Cdmw::OrbSupport::RefCountServantBase
{

    public:
    
        // Constructor
        RecoveryMechanism();
        
        // IDL:omg.org/FT/Checkpointable/set_state()
        virtual void set_state(const FT::State& s) 
            throw (CORBA::SystemException, 
                   FT::InvalidState);
        
        // IDL:omg.org/FT/Checkpointable/get_state()
        virtual FT::State * get_state() 
            throw (CORBA::SystemException, 
                   FT::NoStateAvailable);
        
        // update the state of a HelloInterface servant
        virtual void update_state(HelloInterface_impl* hello) 
            throw (CORBA::SystemException,
                   FT::InvalidState);

        
    private:
        
        // the last receveived state
        FT::State m_state;
        
};


#endif // INCL_DEMO_FT_RECOVERY_MECHANISM_HPP


