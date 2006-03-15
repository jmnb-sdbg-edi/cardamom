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


#include "RecoveryMechanism.hpp"

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <iostream>


RecoveryMechanism::RecoveryMechanism() {
    // set initial state
    m_state.length(1);
    m_state[0] = 0;
}
        
void 
RecoveryMechanism::set_state(const FT::State& s) 
    throw (CORBA::SystemException, 
           FT::InvalidState)
{
    std::cout << "     . . . . RecoveryMechanism: new state received: " << (unsigned long) s[0] << std::endl;

    // store new state
    m_state = s;
};
        
        
FT::State * 
RecoveryMechanism::get_state() 
    throw (CORBA::SystemException, 
           FT::NoStateAvailable)
{
    // Not implemented.
    // The RecoveryMechanism only need to receive 
    // the state from primary via set_state().
    throw CORBA::NO_IMPLEMENT(Cdmw::OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
};
        
        
void 
RecoveryMechanism::update_state(HelloInterface_impl* hello) 
    throw (CORBA::SystemException,
           FT::InvalidState)
{
    std::cout << "     . . . . RecoveryMechanism: update state: " << (unsigned long) m_state[0] << std::endl;

    // set current state to Hello servant
    hello->set_state(m_state);
};



