/* ===================================================================== */
/*
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
*/
/* ===================================================================== */

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "platformsystem/FTConverter_impl.hpp"
#include <Repository/naminginterface/NamingInterface.hpp>
#include "FaultTolerance/idllib/FT.stub.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include "platformsystem/HostProxy_impl.hpp"
namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// FTConverter_impl class.
// ----------------------------------------------------------------------
FTConverter_impl::FTConverter_impl (HostContainer_impl* hostContainer)
{
//    std::cout<<"FTConverter_impl::FTConverter_impl"<<std::endl;
    m_hostContainer = hostContainer;
    
}

// register the application_agent_observer object which receive the notify_ending for all agent processes
void FTConverter_impl::register_application_agent_observer(const char* observer_name,
                                         CdmwPlatformMngt::MonitorableProcessObserver_ptr observer )
    throw( CORBA::SystemException )
{
//    std::cout<<"FTConverter_impl::register_application_agent_observer"<<std::endl;
//    std::cout<<"register_observer : " <<observer_name<<std::endl;
    
    m_applicationAgentObservers[observer_name] = CdmwPlatformMngt::MonitorableProcessObserver::_duplicate(observer);
}

void FTConverter_impl::unregister_application_agent_observer(const char* observer_name)
    throw( CORBA::SystemException )
{
//    std::cout<<"FTConverter_impl::unregister_application_agent_observer "<<observer_name<<std::endl; 
    ApplicationAgentObservers::iterator it = m_applicationAgentObservers.find(observer_name);
    
    if (it != m_applicationAgentObservers.end())
    {
        m_applicationAgentObservers.erase(observer_name);
    }
}

// register the process_observer object which receive the notify_ending for all processes
void FTConverter_impl::register_process_observer(const char* observer_name,
                                         CdmwPlatformMngt::ProcessObserver_ptr observer )
    throw( CORBA::SystemException )
{
//    std::cout<<"FTConverter_impl::register_process_observer"<<std::endl;
//    std::cout<<"register_observer : " <<observer_name<<std::endl;
    
    m_processObservers[observer_name] = CdmwPlatformMngt::ProcessObserver::_duplicate(observer);
}

void FTConverter_impl::unregister_process_observer(const char* observer_name)
    throw( CORBA::SystemException )
{
//    std::cout<<"FTConverter_impl::unregister_process_observer "<<observer_name<<std::endl; 
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
//    std::cout<<"FTConverter_impl::offer_change"<<std::endl;
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


void FTConverter_impl::push_structured_event(const CosNotification::StructuredEvent & notification)
    throw (CosEventComm::Disconnected,
           CORBA::SystemException)
{
//    std::cout<<"FTConverter_impl::push_structured_event"<<std::endl;  

    if ((notification.filterable_data.length() >= 2) &&
        (strcmp(notification.filterable_data[1].name.in(),"Location") == 0)) {
        const ::FT::Location * location;
        notification.filterable_data[1].value >>= location;
        const std::string s_location = 
        Cdmw::NamingAndRepository::NamingInterface::to_string(*location);
//        std::cout<< "push_structured_event(): Location is '" << s_location << "'." << std::endl;
        
        // Handle host failure, application failure, and process failure
        std::string hostname(""), applicationname(""), processname("");
        const CORBA::ULong lg = location->length();
        
        // there are two type of message 
        // 1 : the "ObjectCrashFault" message to notify if an object is crashed
        // 2 : the "ObjectResponding" message to notify if an object responding (for the moment, 
        //     it's just used for daemon)
        std::string type_name_message = CORBA::string_dup(notification.header.fixed_header.event_type.type_name);
        if ( type_name_message == "ObjectCrashFault")
        {
            switch (lg) {
                case 3: 
                    {
                        const std::string processname((*location)[2].id);
                        const std::string applicationname((*location)[1].id);
                        const std::string hostname((*location)[0].id);
//                         std::cout<<"processname : "<<processname
//                                  <<" -- applicationname : "<<applicationname
//                                  <<" -- hostname : "<<hostname<<std::endl;
                            
                        std::string appliProcName = applicationname + "-" + processname;
                            
                        ProcessObservers::iterator it = m_processObservers.find(appliProcName);
                            
                        if (it != m_processObservers.end())
                        {
                            CdmwPlatformMngtBase::TimeStamp timeStamp =
                            EventHeaderFactory::createTimeStamp();
                            (it->second)->notify_ending(processname.c_str() ,timeStamp);
                            //unregister_process_observer(appliProcName.c_str());                       
                        }
                            
                    }
                    break;
                case 2:
                    {
                        const std::string applicationname((*location)[1].id);
                        const std::string hostname((*location)[0].id);
//                         std::cout<<"applicationname : "<<applicationname
//                                  <<" -- hostname : "<<hostname<<std::endl;
                            
                        ApplicationAgentObservers::iterator it = m_applicationAgentObservers.find(applicationname);
                            
                        if (it != m_applicationAgentObservers.end())
                        {
                            CdmwPlatformMngtBase::TimeStamp timeStamp =
                            EventHeaderFactory::createTimeStamp();
                            (it->second)->notify_ending(hostname.c_str() ,timeStamp);
                            //unregister_application_agent_observer(applicationname.c_str());                        
                        }
                            
                    }
                    break;
                case 1:
                    {
                        const std::string hostname((*location)[0].id);
//                         std::cout<<" hostname : "<<hostname<<std::endl;
                            
                        HostProxy_impl* p_hostProxy_impl;
                        PlatformMngt::HostContainer* actualContainer = m_hostContainer->getActualContainer();
                            
                        bool result = actualContainer->findElement(hostname.c_str(), p_hostProxy_impl);
                        if (result == false)
                        {
                            throw CdmwPlatformMngt::HostNotFound();
                        }
                        else
                        {
                            p_hostProxy_impl->setStatus (CdmwPlatformMngt::HOST_NOT_RESPONDING);
                        }
                            
                    }
                    break;
                default:
                    // Do nothing for the moment
                    break;
            }

        } else {
                
            if (type_name_message == "ObjectResponding")
            {
                switch (lg) {                        
                    case 1:
                        {
                            const std::string hostname((*location)[0].id);
//                             std::cout<<" hostname : "<<hostname<<std::endl;
                                
                            HostProxy_impl* p_hostProxy_impl;
                            PlatformMngt::HostContainer* actualContainer = m_hostContainer->getActualContainer();
                                
                            bool result = actualContainer->findElement(hostname.c_str(), p_hostProxy_impl);
                            if (result == false)
                            {
                                throw CdmwPlatformMngt::HostNotFound();
                            }
                            else
                            {
                                p_hostProxy_impl->setStatus (CdmwPlatformMngt::HOST_RESPONDING);
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
//     std::cout<<"FTConverter_impl::disconnect_structured_push_consumer"<<std::endl; 
    throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
}


} // End namespace PlatformMngt
} // End namespace Cdmw

