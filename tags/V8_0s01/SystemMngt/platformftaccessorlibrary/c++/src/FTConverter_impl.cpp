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

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"

#include "platformsystemlibrary/Host_impl.hpp"

#include "platformftaccessorlibrary/FTConverter_impl.hpp"

#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include "FaultTolerance/idllib/FT.stub.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// FTConverter_impl class.
// ----------------------------------------------------------------------
FTConverter_impl::FTConverter_impl (HostContainer_impl* hostContainer)
{
    m_hostContainer = hostContainer;    
}



// register the process_observer object which receive the notify_ending for all processes
void FTConverter_impl::register_process_observer(
                    const char* observer_name,
                    CdmwPlatformMngt::ProcessObserver_ptr observer)
    throw (CORBA::SystemException)
{
    // std::cout<<"FTConverter_impl::register_process_observer"<<std::endl;
    // std::cout<<"register_observer : " <<observer_name<<std::endl;
    
    m_processObservers[observer_name] = 
             CdmwPlatformMngt::ProcessObserver::_duplicate(observer);
}

void FTConverter_impl::unregister_process_observer(const char* observer_name)
    throw( CORBA::SystemException )
{
    // std::cout<<"FTConverter_impl::unregister_process_observer "<<observer_name<<std::endl; 
    ProcessObservers::iterator it = m_processObservers.find(observer_name);
    
    if (it != m_processObservers.end())
    {
        m_processObservers.erase(observer_name);                        
    }
}

void FTConverter_impl::offer_change(const CosNotification::EventTypeSeq & added,
                                    const CosNotification::EventTypeSeq & removed)
    throw (CosNotifyComm::InvalidEventType,
           CORBA::SystemException)
{
    // std::cout<<"FTConverter_impl::offer_change"<<std::endl;
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


void FTConverter_impl::push_structured_event(const CosNotification::StructuredEvent & notification)
    throw (CosEventComm::Disconnected,
           CORBA::SystemException)
{
    // std::cout << "FTConverter_impl::push_structured_event() enter" << std::endl;  

    if ((notification.filterable_data.length() >= 2) &&
        (strcmp(notification.filterable_data[1].name.in(),"Location") == 0)) 
    {
        const ::FT::Location * location;
        notification.filterable_data[1].value >>= location;
        
        const std::string s_location = 
            Cdmw::CommonSvcs::Naming::NamingInterface::to_string(*location);
        
        // std::cout<< "FTConverter_impl::push_structured_event(): Location is '" << s_location << "'." << std::endl;
        
        // Handle host failure, application failure, and process failure
        
        // get location size
        const CORBA::ULong lg = location->length();
        
        // there are two type of message 
        // 1 : the "ObjectCrashFault" message to notify if an object is crashed
        // 2 : the "ObjectResponding" message to notify if an object responding (for the moment, 
        //     it's just used for daemon)
        std::string type_name_message = 
              CORBA::string_dup(notification.header.fixed_header.event_type.type_name);
        
        // if the timestamp is specified into the notification, use it
        // else take the current time
        CdmwPlatformMngtBase::TimeStamp timeStamp;
        
        if ((notification.filterable_data.length() >= 3) &&
            (strcmp(notification.filterable_data[2].name.in(),"TimeStamp") == 0)) 
        {
            const CdmwPlatformMngtBase::TimeStamp *_timeStamp;
            if(notification.filterable_data[2].value >>= _timeStamp)
            {
                timeStamp = (*_timeStamp);
            }
            else
            {
                // if the any failed, take the current time
                timeStamp = EventHeaderFactory::createTimeStamp();
            }
            
        } else {
            timeStamp = EventHeaderFactory::createTimeStamp();
        }
        
        
        if (type_name_message == "ObjectCrashFault")
        {
            switch (lg) 
            {
            	// process ending (host/application/process)
                case 3: 
                {
                    const std::string process_name((*location)[2].id);
                    const std::string application_name((*location)[1].id);
                    const std::string host_name((*location)[0].id);
                        
                    // the process exit status (for future use)
                    CORBA::Long exit_status = 0;
                    
                    // search process observer for the application      
                    ProcessObservers::iterator it = m_processObservers.find(application_name);
                    
                    // if found       
                    if (it != m_processObservers.end())
                    {   
                        try
                        {
                            // notify the process ending               
                            (it->second)->notify_ending (application_name.c_str(),
                                                         process_name.c_str(), 
                                                         host_name.c_str(),
                                                         timeStamp, exit_status);
                        }
                        catch (CdmwPlatformMngt::ProcessNotFound &)
                        {
                            std::cout << "FTConverter_impl::push_structured_event(): application '" 
                                      << application_name.c_str() 
                                      << "' process '" << process_name.c_str()
                                      << "'host '" << host_name.c_str() 
                                      << "' Process not found" << std::endl;
                        }                                            
                    }
                            
                }
                break;
                
                // process started by daemon ending    
                case 2:
                {   
                    // do nothing                      
                }
                break;
                    
                // host not responding
                case 1:
                {
                    const std::string host_name((*location)[0].id);
                    
                    // std::cout << "FTConverter_impl::push_structured_event(): Host '" 
                    //           << host_name.c_str() << "' not responding" << std::endl;
                            
                    Host_impl* p_host_impl;
                    PlatformMngt::HostContainer* actualContainer = m_hostContainer->getActualContainer();
                    
                    bool result = false;
                    
                    try
                    {      
                        result = actualContainer->findElement(host_name.c_str(), p_host_impl);
                    }
                    catch (...)
                    {
                    }                   
                        
                    if (result == false)
                    {
                        std::cout << "FTConverter_impl::push_structured_event(): host '" 
                                  << host_name.c_str() 
                                  << "' NotFound" << std::endl;
                    }
                    else
                    {
                        p_host_impl->setStatus (CdmwPlatformMngt::HOST_UNREACHABLE, timeStamp);
                    }
                            
                }
                break;
                    
                default:
                    // Do nothing for the moment
                    break;
            }

        } 
    
        else 
        {                
            if (type_name_message == "ObjectResponding")
            {
                switch (lg) 
                { 
                	// host responding                       
                    case 1:
                    {
                        const std::string host_name((*location)[0].id);
                            
                        // std::cout << "FTConverter_impl::push_structured_event(): Host '" 
                        //           << host_name.c_str() << "' responding" << std::endl;
                                
                        Host_impl* p_host_impl;
                        PlatformMngt::HostContainer* actualContainer = m_hostContainer->getActualContainer();
                        
                        bool result = false;
                    
                        try
                        {      
                            result = actualContainer->findElement(host_name.c_str(), p_host_impl);
                        }
                        catch (...)
                        {
                        }    
                          
                        if (result == false)
                        {
                            std::cout << "FTConverter_impl::push_structured_event(): host '" 
                                      << host_name.c_str() 
                                      << "' NotFound" << std::endl;
                        }
                        else
                        {
                            p_host_impl->setStatus (CdmwPlatformMngt::HOST_RESPONDING, timeStamp);
                        }                                
                    }
                    break;
                        
                    default:
                        // Do nothing for the moment
                        break;
                }                
            }                
        }        
    }
}


void FTConverter_impl::disconnect_structured_push_consumer()
    throw (CORBA::SystemException)
{
    // std::cout<<"FTConverter_impl::disconnect_structured_push_consumer"<<std::endl; 
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


} // End namespace PlatformMngt
} // End namespace Cdmw

