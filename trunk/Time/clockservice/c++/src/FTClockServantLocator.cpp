/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */

#include "clockservice/FTClockActivationHandler.hpp"
#include "clockservice/FTClockServantLocator.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

Cdmw::clock::FTClockServantLocator::FTClockServantLocator(
    const std::string& oid,
    Cdmw::clock::FTClockActivationHandler* activation_handler)
    : m_oid_str(oid),
      m_activation_handler(activation_handler)
{
    m_activation_handler->_add_ref();
}

Cdmw::clock::FTClockServantLocator::~FTClockServantLocator()
{
    m_activation_handler->_remove_ref();
}

PortableServer::Servant
Cdmw::clock::FTClockServantLocator::preinvoke(
    const PortableServer::ObjectId& oid,
    PortableServer::POA_ptr/* poa */,
    const char* operation,
    PortableServer::ServantLocator::Cookie& /* the_cookie */)
    throw (CORBA::SystemException)
{
    // Check if oid corresponds to ObjectId of Executor servant.
    CORBA::String_var oid_string = PortableServer::ObjectId_to_string(oid);
    std::string oid_str(oid_string);
    if (oid_str != m_oid_str) {
        std::cerr << "    ERROR in Cdmw::clock::FTClockServantLocator: " 
                  << "No servant found for oid " << oid_str
                  << std::endl;
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST, CORBA::COMPLETED_NO);
    } 

    // Check if this replica is active (i.e. is primary)
    if (! m_activation_handler->is_active()) {
        std::string op(operation);
        // authorize only operations of the Object base interface
        // (_is_a(), _interface() and _non_existent())
        if (op != "_is_a" && op != "_interface" && op != "_non_existent") {
            std::cerr << "    ERROR in Cdmw::clock::FTClockServantLocator: "
                      << "Object Executor is not active (object is not primary)"
                      << std::endl;
            // throws TRANSIENT exception. 
            // The FT infrastructure will catch it and retry on another member
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
        }
    }
    
    // return Executor servant.
    return m_activation_handler->getServant();        
}



void 
Cdmw::clock::FTClockServantLocator::postinvoke(
    const PortableServer::ObjectId&,
    PortableServer::POA_ptr,
    const char*,
    PortableServer::ServantLocator::Cookie,
    PortableServer::Servant)
    throw(CORBA::SystemException)
{
    // No-Op
}

