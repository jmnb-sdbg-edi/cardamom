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


#include <sstream>

#include <LifeCycle/lifecycle/LifeCycleObjectBase.hpp>

#include "HelloUserServantProvider.hpp"
#include "HelloInterface_impl.hpp"


namespace 
{
    const char* HELLOINTERFACE_REP_ID = "IDL:thalesgroup.com/Hello/HelloInterface:1.0";
};



namespace Cdmw
{
namespace Tutorial
{



HelloUserServantProvider::~HelloUserServantProvider()
    throw()
{
}



HelloUserServantProvider::HelloUserServantProvider (
                   CORBA::ORB_ptr orb,
                   PortableServer::POA_ptr poa,
                   const Cdmw::LifeCycle::ObjectDeactivator & deactivator)
   throw(CORBA::SystemException)
  
  : m_orb(CORBA::ORB::_duplicate(orb)),
    m_poa(PortableServer::POA::_duplicate(poa)),
    m_object_deactivator(deactivator)
{
}



PortableServer::ObjectId* HelloUserServantProvider::create_HelloInterface(const char* object_oid)
    throw ()
{
    //
    // create the object id
    //
	PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(object_oid);
	
   	std::cout << "<<HelloUserServantProvider>> Hello '" 
   	          << object_oid <<"' OID created" << std::endl;
	
	return oid._retn();
}



PortableServer::Servant 
HelloUserServantProvider::lookup_servant(const PortableServer::ObjectId& oid) 
    throw(PortableServer::ForwardRequest,
          CORBA::SystemException)
{

 	//
    // Create implementation object 
    //
    HelloInterface_impl* hello_impl = new HelloInterface_impl();
    
    PortableServer::Servant servant = hello_impl;
    
    std::cout << "<<HelloUserServantProvider>> HelloInterface servant created" 
              << std::endl;
    
    // return the servant reference of object
    return servant;
}


void HelloUserServantProvider::release_servant(
                     const PortableServer::ObjectId& oid,
                     PortableServer::POA_ptr poa,
                     PortableServer::Servant servant) 
    throw(CORBA::SystemException)
{
    // convert servant to LifeCycleObjectBase
    Cdmw::LifeCycle::LifeCycleObjectBase* removable =
            dynamic_cast<Cdmw::LifeCycle::LifeCycleObjectBase *>(servant);
            
    if ((removable) && removable->is_removed(oid)) 
    {
        cleanup_object(oid);            
        servant->_remove_ref();
        
        std::cout << "<<HelloUserServantProvider>> HelloInterface servant reference removed." 
                  << std::endl;
    }
}



std::string HelloUserServantProvider::get_supported_interface(
                         const PortableServer::ObjectId& oid) const
    throw(CORBA::OBJECT_NOT_EXIST)
{
	std::string result = HELLOINTERFACE_REP_ID;
            
    return result;    
}

        
}; // End namespace Hello
}; // End namespace Cdmw
