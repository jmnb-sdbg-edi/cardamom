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


#include "platformdaemon/ServiceRegistration_impl.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

    ServiceRegistration_impl::ServiceRegistration_impl(CORBA::ORB_ptr orb,
                                                       PortableServer::POA_ptr poa,
                                                       std::string service_locator_address,
                                                       std::string registration_service_address,
                                                       int nb_retries,
                                                       int time_interval)
        : m_servicesCondition(m_servicesMutex),
          m_orb(CORBA::ORB::_duplicate(orb)),
          m_poa(PortableServer::POA::_duplicate(poa)),
          m_nb_retries(nb_retries),
          m_time_interval(time_interval),
          m_registration_service_address(registration_service_address)
    {
        // get multicast object group of service locator to interogate
        CORBA::Object_var service_locator_group = 
                m_orb->string_to_object(service_locator_address.c_str());
        
        m_service_locator = 
            CdmwServiceLocator::ServiceLocator::_unchecked_narrow (service_locator_group.in());
        
        // get multicast object group of registration service 
        CORBA::Object_var registration_service_group = 
                m_orb->string_to_object(registration_service_address.c_str());
        
        // get an object id on this multicast object group
        PortableServer::ObjectId_var registration_id = 
                m_poa->create_id_for_reference(registration_service_group.in());
        
        // associate this servant to the multicast object id
        m_poa->activate_object_with_id (registration_id.in(), this);         
    }
    
    
    ServiceRegistration_impl::~ServiceRegistration_impl()
    {
    }
    

    //
    // register the service. Called by the service locator
    //
    void ServiceRegistration_impl::register_service(const char* name, CORBA::Object_ptr ref)
        throw(CORBA::SystemException)
    {
        CDMW_MUTEX_GUARD(m_servicesMutex);
                
        // first search the service in map
        ServiceMap::iterator iter = m_services.find(name);
        
        // if not found
        if (iter == m_services.end())
        {
             // store the service reference in the map
             CORBA::Object_var obj = CORBA::Object::_duplicate(ref);
             m_services[name] = obj;
             
             // broadcast the condition
             m_servicesCondition.broadcast();
        } 
    }
    
    //
    // register the service. Called locally
    //
    void ServiceRegistration_impl::register_local_service(const char* name, CORBA::Object_ptr ref)
    {
        CDMW_MUTEX_GUARD(m_servicesMutex);
                
        // first search the service in map
        ServiceMap::iterator iter = m_services.find(name);
        
        // if not found
        if (iter == m_services.end())
        {
             // store the service reference in the map
             CORBA::Object_var obj = CORBA::Object::_duplicate(ref);
             m_services[name] = obj;
        } 
    }
    

    //
    // get the registered service
    // if not already registered, discover by interogating the service locator
    //
    CORBA::Object_ptr ServiceRegistration_impl::get_service(const char * name)
    {
        CORBA::Object_var service_ref = CORBA::Object::_nil();
        
        // lock mutex of service map
        CDMW_MUTEX_GUARD(m_servicesMutex);
        
        // first search the servive in map
        ServiceMap::iterator iter = m_services.find(name);
        
        // if found
        if (iter != m_services.end())
        {
            service_ref = (*iter).second;
        } 
        
        // the service is not in the map, interogate the service locator
        // in multicast
        else 
        {
            // calculate the max time out to wait response
            unsigned long max_timeout = (unsigned long) (m_nb_retries * m_time_interval);
            
            // first get the start time
            OsSupport::OS::Timeval start_time = OsSupport::OS::get_time();
            
            // call the service locator in multicast
            m_service_locator->locate_service(name, m_registration_service_address.c_str());
            
            bool loop_flag = false;
            
            bool timeout = false;
            
            unsigned long allowed_timeout = (unsigned long) m_time_interval;
            
            // wait the response
            while (!loop_flag && !timeout)
            {
                // get the wait start time
                OsSupport::OS::Timeval wait_start_time = OsSupport::OS::get_time();
            
                // wait the condition
                OsSupport::Condition::WaitReturn statusWait =
                    m_servicesCondition.wait((size_t)allowed_timeout);
                
                // condition wakeup
                
                // search the service in map
                ServiceMap::iterator iter = m_services.find(name);
        
                // if service reference received
                if (iter != m_services.end())
                {
                    service_ref = (*iter).second;
                    
                    loop_flag = true;
                }
                
                // else if time out
                else if (statusWait == OsSupport::Condition::TIMEDOUT)
                {       
                    // get the current time
                    OsSupport::OS::Timeval time = OsSupport::OS::get_time();
                         
                    // Compute the remaining timeout (in ms)
                    unsigned long elapsed_time =
                       (time.seconds - start_time.seconds) * 1000000;
                    elapsed_time += (time.microseconds - start_time.microseconds);
                    elapsed_time = elapsed_time / 1000;
                    
                    // timeout has not been reached, retry a new interogation
                    if (elapsed_time < max_timeout)
                    {
                        // calculate the allowed timeout to wait a new response
                        // limited to time interval
                        allowed_timeout = max_timeout - elapsed_time;
                        
                        if (allowed_timeout > (unsigned long) m_time_interval)
                        {
                            allowed_timeout = (unsigned long) m_time_interval;
                        }
                        
                        // recall the service locator in multicast
                        m_service_locator->locate_service(
                                 name, m_registration_service_address.c_str());
                    }
                    
                    // else time out has been reached, service is not found
                    else
                    {
                        timeout = true;
                    }                    
                }  
                
                // condition is wakeup but not by broadcast or timeout
                // condition must return to wait
                else
                {
                    // get the current time
                    OsSupport::OS::Timeval time = OsSupport::OS::get_time();
                    
                    // Compute the remaining timeout for waiting (in ms)
                    unsigned long elapsed_time =
                       (time.seconds - wait_start_time.seconds) * 1000000;
                    elapsed_time += (time.microseconds - wait_start_time.microseconds);
                    elapsed_time = elapsed_time / 1000;
                    
                    // calculate the remaining allowed timeout to wait response
                    if (elapsed_time >= allowed_timeout)
                    {
                        allowed_timeout = 0;
                    }
                    else
                    {
                        allowed_timeout = allowed_timeout - elapsed_time;
                    }
                }                        
            }
        }
        
        return service_ref._retn();
    }

   
    //
    // Returns a copy of the underlying service map.
    //
    ServiceRegistration_impl::ServiceMap ServiceRegistration_impl::get_known_services()
    {
        ServiceMap result;
        result.insert(m_services.begin(),m_services.end());
        return result;
    }
    
}
}
