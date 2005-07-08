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


#ifndef INCL_DATABASE_SERVANT_LOCATOR_HPP
#define INCL_DATABASE_SERVANT_LOCATOR_HPP


// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>

// Demo Files
#include <DatabaseActivationHandler.hpp>



class DatabaseServantLocator:
    public virtual PortableServer::ServantLocator,
    public virtual Cdmw::OrbSupport::RefCountLocalObject
{
    public: 
    
        // Constructor
        DatabaseServantLocator(const std::string& oid,
                               DatabaseActivationHandler* activation_handler);
       
        // Destructor
        virtual ~DatabaseServantLocator();
    
        // IDL:omg.org/PortableServer/ServantLocator/preinvoke()
        virtual PortableServer::Servant 
        preinvoke(const PortableServer::ObjectId& oid,
                  PortableServer::POA_ptr poa,
                  const char* operation,
                  PortableServer::ServantLocator::Cookie& the_cookie)
            throw (CORBA::SystemException);
        
        
        // IDL:omg.org/PortableServer/ServantLocator/postinvoke()
        virtual void 
        postinvoke(const PortableServer::ObjectId& oid,
                   PortableServer::POA_ptr poa,
                   const char* operation,
                   PortableServer::ServantLocator::Cookie the_cookie,
                   PortableServer::Servant the_servant)
            throw (CORBA::SystemException);
    
    
    private: 
    
        // ObjectId of the Database object
        std::string m_oid_str;
        
        // The Activation Handler
        DatabaseActivationHandler* m_activation_handler;
};



#endif // INCL_DATABASE_SERVANT_LOCATOR_HPP

