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


#include "DatabaseServantLocator.hpp"

#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>



DatabaseServantLocator::DatabaseServantLocator(
    const std::string& oid,
    DatabaseActivationHandler* activation_handler)
{
    m_oid_str = oid;
    
    CDMW_ASSERT(activation_handler != NULL);
    m_activation_handler = activation_handler;
    m_activation_handler->_add_ref();
    
}              


DatabaseServantLocator::~DatabaseServantLocator() {
    m_activation_handler->_remove_ref();
}

        

PortableServer::Servant
DatabaseServantLocator::preinvoke(
        const PortableServer::ObjectId& oid,
        PortableServer::POA_ptr poa,
        const char* operation,
        PortableServer::ServantLocator::Cookie& the_cookie)
   throw (CORBA::SystemException)
{
    // Check if oid corresponds to ObjectId of Database servant.
    CORBA::String_var oid_string = PortableServer::ObjectId_to_string(oid);
    std::string oid_str(oid_string);
    if (oid_str != m_oid_str) {
        std::cerr << "    ERROR in DatabaseServantLocator: " 
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
            std::cerr << "    ERROR in DatabaseServantLocator: "
                      << "Object Database is not active (object is not primary)"
                      << std::endl;
            // throws TRANSIENT exception. 
            // The FT infrastructure will catch it and retry on another member
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
        }
    }
    
    // return Database servant.
    return m_activation_handler->getDatabaseServant();        
}



void 
DatabaseServantLocator::postinvoke(
        const PortableServer::ObjectId& oid,
        PortableServer::POA_ptr poa,
        const char* operation,
        PortableServer::ServantLocator::Cookie the_cookie,
        PortableServer::Servant the_servant)
    throw(CORBA::SystemException)
{
    // nothing to do
}




