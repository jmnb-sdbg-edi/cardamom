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


#include "LoggingMechanism.hpp"

#include <iostream>
    
void LoggingMechanism::add_replica(FT::Checkpointable_ptr replica) {
    FT::Checkpointable_var replica_to_add(FT::Checkpointable::_duplicate(replica));    
    m_replicas.push_back(replica_to_add);
}
    
    
void LoggingMechanism::log_state(const FT::State& state) 
{
    
    std::cout << "           . LoggingMechanism::log_state " << (unsigned long) state[0] << std::endl;
    
    // For each replica of the list...
    for (std::list<FT::Checkpointable_var>::iterator it_replica = m_replicas.begin();
         it_replica != m_replicas.end();
         ++it_replica) 
    {
        try {
            // ... send it the new state
            std::cout << "           .    send state... ";
            (*it_replica)->set_state(state);
            std::cout << "OK" << std::endl;
        
        } catch (const CORBA::SystemException&) {
            // The replica could be dead !
            std::cout << "SystemException !! (replica is dead ?)" << std::endl;
        }
    }
}


