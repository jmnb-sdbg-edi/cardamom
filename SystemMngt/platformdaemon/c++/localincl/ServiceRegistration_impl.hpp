/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
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

#ifndef __CDMW_SERVICE_LOCATOR_IMPL_HPP__
#define __CDMW_SERVICE_LOCATOR_IMPL_HPP__

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/idllib/CdmwServiceLocator.skel.hpp"

#include <iostream>
#include <map>

namespace Cdmw
{
namespace PlatformMngt
{

class  ServiceRegistration_impl :
        public virtual POA_CdmwServiceLocator::ServiceRegistration,
        public virtual PortableServer::RefCountServantBase
{

public :
    
    ServiceRegistration_impl(CORBA::ORB_ptr orb,
                             PortableServer::POA_ptr poa,
                             std::string service_locator_address,
                             std::string registration_service_address,
                             int nb_retries,
                             int time_interval);
    
    ~ServiceRegistration_impl();

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/Foundation/CdmwServiceLocator/register_service:1.0
    * operation
    */  
    virtual void register_service(const char * name, CORBA::Object_ptr ref)
        throw(CORBA::SystemException);
    
    /**
    * Purpose:
    * <p> register locally the service.
    * 
    */    
    void register_local_service(const char * name, CORBA::Object_ptr ref);

    /**
    * Purpose:
    * <p> Get the service refrence from its name.
    * 
    */
    CORBA::Object_ptr get_service(const char * name);


    typedef std::map<std::string, CORBA::Object_var, std::less<std::string> > ServiceMap;
    
    /**
    * Purpose:
    * <p> Returns a copy of the underlying service map.
    * 
    */
    ServiceMap get_known_services();
    

private:

    /**
    * The global services.
    */
    ServiceMap m_services;
    
    /**
    * The mutex protecting concurrent access to m_globalServices.
    * (associated to the receiving registration condition)
    */
    OsSupport::Mutex m_servicesMutex;
    
    /**
    * The condition for receiving the registration.
    */
    OsSupport::Condition m_servicesCondition;
    
    /**
    * The orb reference.
    */
    CORBA::ORB_var m_orb;

    /**
    * The poa reference.
    */
    PortableServer::POA_var m_poa;
    
    /**
    * The max number of retries to interogate the service locator.
    */
    int m_nb_retries;
    
    /**
    * The time interval between two interogations.
    */
    int m_time_interval;

    /**
    * The service locator to interrogate.
    */
    CdmwServiceLocator::ServiceLocator_var m_service_locator;
    
    /**
    * The string reference of the registration service receiving the answers..
    */
    std::string m_registration_service_address;
};
    
}
}

#endif
