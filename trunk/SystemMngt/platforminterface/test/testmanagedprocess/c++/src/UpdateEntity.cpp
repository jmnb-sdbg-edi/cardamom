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


#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp"

#include "testmanagedprocess/UpdateEntity.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>


using namespace Cdmw::PlatformMngt;

namespace
{  
    const unsigned long ENTITY_UPDATE_MAX_COUNTER = 10;
}


/**
*Implementation notes: 
*<p>
*Portability issues:  none 
*<p>
*/


// HostEntityToUpdate.

HostEntityToUpdate::HostEntityToUpdate(std::string& entityName,
                                       std::string& hostName,
                                       std::string& applicationName,
                                       std::string& processName)
          : EntityToUpdate (entityName, hostName, applicationName, processName)
{ 
}
        
        

HostEntityToUpdate::~HostEntityToUpdate() 
{
}


void HostEntityToUpdate::update() throw()
{
    if (!m_entityName.empty() && !m_errorSet)
    {
                
        std::string infoMsg;
            
        try
        {
            m_updatingCounter++;
                    
            if (m_updatingCounter == ENTITY_UPDATE_MAX_COUNTER)
            {
                std::cout<<"HostEntityToUpdate:  Notify_fatal_error "<<m_processName.c_str()<<" "<<m_entityName.c_str()<<std::endl;
                PlatformInterface::Notify_fatal_error (
                               m_processName.c_str(), "Fatal Error");
            }
            else if (m_updatingCounter == 1)
            {
                // initialise the entity
                std::cout<<"Init host entity "<<m_processName.c_str()<<" "<<m_entityName.c_str()<<std::endl;
                PlatformInterface::Set_host_entity_status(m_hostName.c_str(),
                                                          m_entityName.c_str(), 
                                                          "functioning",
                                                          "the host entity status is : functioning");

                char* status;
                // get the status of the entity
                
                char* info;
                CORBA::String_out info_out (info);
                status = PlatformInterface::Get_host_entity_status(m_hostName.c_str(),
                                                                   m_entityName.c_str(),
                                                                   info_out);

            }
            else if (m_updatingCounter < ENTITY_UPDATE_MAX_COUNTER)
            {
                char* status;
                // get the status of the entity
                
                char* info;
                CORBA::String_out info_out (info);
                status = PlatformInterface::Get_host_entity_status(m_entityName.c_str(),
                                                                   info_out);
                
                    
                if (strcmp(status, "disfunction") == 0)
                {
                    // entity to be updated
                    PlatformInterface::Set_host_entity_status (m_entityName.c_str(), 
                                                               "functioning",
                                                               "the host entity status is : functioning");
                }
                else
                {
                    // entity to be updated
                    PlatformInterface::Set_host_entity_status (m_entityName.c_str(), 
                                                               "disfunction",
                                                               "the host entity status is : disfunction");
                }
            }
        }
        catch (const CdmwPlatformMngt::EntityNotFound& e)
        {
            m_errorSet = true;
            infoMsg = "Entity not found : ";
            infoMsg += m_entityName;
        }
        catch (const CORBA::SystemException& e)
        {
            m_errorSet = true;
                
            std::ostringstream exceptionInfo;
            exceptionInfo << e;
                
            infoMsg = "setHostEntityStatus system exception : ";
            infoMsg += m_entityName;
            infoMsg += "\n";
            infoMsg += exceptionInfo.str();            
        }
        catch (const Cdmw::BadOrderException& e)
        {
            m_errorSet = true;
            infoMsg = "setHostEntityStatus bad order exception";
        }
        catch (const Cdmw::BadParameterException& e)
        {
            m_errorSet = true;
            infoMsg = "setHostEntityStatus bad parameter exception";
        }
                                    
        if (m_errorSet)
        {
            PlatformInterface::Notify_message(CdmwPlatformMngtBase::ERR,
                               m_processName.c_str(), infoMsg.c_str());
        }  
    }         
}

/**
*Implementation notes: 
*<p>
*Portability issues:  none 
*<p>
*/


// SystemEntityToUpdate.

SystemEntityToUpdate::SystemEntityToUpdate(std::string& entityName,
                                           std::string& hostName,
                                           std::string& applicationName,
                                           std::string& processName)
          : EntityToUpdate (entityName, hostName, applicationName, processName)
{ 
}
        
        

SystemEntityToUpdate::~SystemEntityToUpdate() 
{
}


void SystemEntityToUpdate::update() throw()
{
    if (!m_entityName.empty() && !m_errorSet)
    {
                
        std::string infoMsg;
            
        try
        {
            m_updatingCounter++;
                    
            if (m_updatingCounter == ENTITY_UPDATE_MAX_COUNTER)
            {
                std::cout<<"SystemEntityToUpdate:  Notify_fatal_error "<<m_processName.c_str()<<" "<<m_entityName.c_str()<<std::endl;
                PlatformInterface::Notify_fatal_error (
                               m_processName.c_str(), "Fatal Error");
            }
            else if (m_updatingCounter == 1)
            {
                // initialise the entity
                std::cout<<"Init system entity "<<m_processName.c_str()<<" "<<m_entityName.c_str()<<std::endl;
                PlatformInterface::Set_system_entity_status(m_entityName.c_str(), 
                                                            "functioning",
                                                            "the system entity status is : functioning");
            }
            else if (m_updatingCounter < ENTITY_UPDATE_MAX_COUNTER)
            {
                char* status;
                // get the status of the entity
                char* info;
                CORBA::String_out info_out (info);
                status = PlatformInterface::Get_system_entity_status(m_entityName.c_str(),
                                                                     info_out);
                    
                if (strcmp(status, "disfunction") == 0)
                {
                    // entity to be updated
                    PlatformInterface::Set_system_entity_status (m_entityName.c_str(), 
                                                                 "functioning",
                                                                 "the system entity status is : functioning");
                }
                else
                {
                    // entity to be updated
                    PlatformInterface::Set_system_entity_status (m_entityName.c_str(), 
                                                                 "disfunction",
                                                                 "the system entity status is : disfunction");
                }
            }     
        }
        catch (const CdmwPlatformMngt::EntityNotFound& e)
        {
            m_errorSet = true;
            infoMsg = "Entity not found : ";
            infoMsg += m_entityName;
        }
        catch (const CORBA::SystemException& e)
        {
            m_errorSet = true;
                
            std::ostringstream exceptionInfo;
            exceptionInfo << e;
                
            infoMsg = "setSystemEntityStatus system exception : ";
            infoMsg += m_entityName;
            infoMsg += "\n";
            infoMsg += exceptionInfo.str();            
        }
        catch (const Cdmw::BadOrderException& e)
        {
            m_errorSet = true;
            infoMsg = "setSystemEntityStatus bad order exception";
        }
        catch (const Cdmw::BadParameterException& e)
        {
            m_errorSet = true;
            infoMsg = "setSystemEntityStatus bad parameter exception";
        }
                                    
        if (m_errorSet)
        {
            PlatformInterface::Notify_message(CdmwPlatformMngtBase::ERR,
                               m_processName.c_str(), infoMsg.c_str());
        }  
    }         
}

// ApplicationEntityToUpdate.

ApplicationEntityToUpdate::ApplicationEntityToUpdate(std::string& entityName,
                                                     std::string& hostName,
                                                     std::string& applicationName,
                                                     std::string& processName)
          : EntityToUpdate (entityName, hostName, applicationName, processName)
{ 
}
        

ApplicationEntityToUpdate::~ApplicationEntityToUpdate()
{ 
}



void ApplicationEntityToUpdate::update() throw()
{
    if (!m_entityName.empty() && !m_errorSet)
    {
                
        std::string infoMsg;
            
        try
        {
            m_updatingCounter++;
                    
            if (m_updatingCounter == ENTITY_UPDATE_MAX_COUNTER)
            {
                std::cout<<"ApplicationEntityToUpdate:  Notify_fatal_error "<<m_processName.c_str()<<" "<<m_entityName.c_str()<<std::endl;
                PlatformInterface::Notify_fatal_error (
                               m_processName.c_str(), "Fatal Error");
            }
            else if (m_updatingCounter == 1)
            {
                // initialise the entity
                std::cout<<"Init application entity "<<m_processName.c_str()<<" "<<m_entityName.c_str()<<std::endl;
                PlatformInterface::Set_application_entity_status(m_applicationName.c_str(),
                                                                 m_entityName.c_str(), 
                                                                 "functioning",
                                                                 "the application entity status is : functioning");
                char* status;
                // get the status of the entity
                char* info;
                CORBA::String_out info_out (info);
                status = PlatformInterface::Get_application_entity_status(m_applicationName.c_str(),
                                                                          m_entityName.c_str(),
                                                                          info_out);
            }
            else if (m_updatingCounter < ENTITY_UPDATE_MAX_COUNTER)
            {
                char* status;
                // get the status of the entity
                char* info;
                CORBA::String_out info_out (info);
                status = PlatformInterface::Get_application_entity_status(m_entityName.c_str(),
                                                                          info_out);
                    
                if (strcmp(status, "disfunction") == 0)
                {
                    // entity to be updated
                    PlatformInterface::Set_application_entity_status (m_entityName.c_str(), 
                                                                      "functioning",
                                                                      "the application entity status is : functioning");
                }
                else
                {
                    // entity to be updated
                    PlatformInterface::Set_application_entity_status (m_entityName.c_str(), 
                                                                      "disfunction",
                                                                      "the application entity status is : disfunction");
                }
            }     
        }
        catch (const CdmwPlatformMngt::EntityNotFound& e)
        {
            m_errorSet = true;
            infoMsg = "Entity not found : ";
            infoMsg += m_entityName;
        }
        catch (const CORBA::SystemException& e)
        {
            m_errorSet = true;
                
            std::ostringstream exceptionInfo;
            exceptionInfo << e;
                
            infoMsg = "setApplicationEntityStatus system exception : ";
            infoMsg += m_entityName;
            infoMsg += "\n";
            infoMsg += exceptionInfo.str();                
        }
        catch (const Cdmw::BadOrderException& e)
        {
            m_errorSet = true;
            infoMsg = "setApplicationEntityStatus bad order exception";
        }
        catch (const Cdmw::BadParameterException& e)
        {
            m_errorSet = true;
            infoMsg = "setApplicationEntityStatus bad parameter exception";
        }
                                    
        if (m_errorSet)
        {
            PlatformInterface::Notify_message(CdmwPlatformMngtBase::ERR,
                               m_processName.c_str(), infoMsg.c_str());
        }  
    }         
}
        



// ProcessEntityToUpdate.

ProcessEntityToUpdate::ProcessEntityToUpdate(std::string& entityName,
                                             std::string& hostName,
                                             std::string& applicationName,
                                             std::string& processName)
          : EntityToUpdate (entityName, hostName, applicationName, processName)
{ 
}
        

ProcessEntityToUpdate::~ProcessEntityToUpdate() 
{ 
}


void ProcessEntityToUpdate::update() throw()
{
    if (!m_entityName.empty() && !m_errorSet)
    {
                
        std::string infoMsg;
            
        try
        {
            m_updatingCounter++;
                    
            if (m_updatingCounter == ENTITY_UPDATE_MAX_COUNTER)
            {
                std::cout<<"ProcessEntityToUpdate:  Notify_fatal_error "<<m_processName.c_str()<<" "<<m_entityName.c_str()<<std::endl;
                PlatformInterface::Notify_fatal_error (
                               m_processName.c_str(), "Fatal Error");
            }
            else if (m_updatingCounter == 1)
            {
                std::cout<<"Init process entity "<<m_processName.c_str()<<" "<<m_entityName.c_str()<<std::endl;
                // initialise the entity
                PlatformInterface::Set_process_entity_status(m_applicationName.c_str(),
                                                             m_processName.c_str(),
                                                             m_hostName.c_str(),
                                                             m_entityName.c_str(), 
                                                             "functioning",
                                                             "the process entity status is : functioning");
                char* status;
                // get the status of the entity
                char* info;
                CORBA::String_out info_out (info);
                status = PlatformInterface::Get_process_entity_status(m_applicationName.c_str(),
                                                                      m_processName.c_str(),
                                                                      m_hostName.c_str(),
                                                                      m_entityName.c_str(),
                                                                      info_out);
            }
            else if (m_updatingCounter < ENTITY_UPDATE_MAX_COUNTER)
            {
                char* status;
                // get the status of the entity
                char* info;
                CORBA::String_out info_out (info);
                status = PlatformInterface::Get_process_entity_status(m_entityName.c_str(),
                                                                      info_out);
                    
                if (strcmp(status, "disfunction") == 0)
                {
                    // entity to be updated
                    PlatformInterface::Set_process_entity_status (m_entityName.c_str(), 
                                                                  "functioning",
                                                                  "the process entity status is : functioning");
                }
                else
                {
                    // entity to be updated
                    PlatformInterface::Set_process_entity_status (m_entityName.c_str(), 
                                                                  "disfunction",
                                                                  "the process entity status is : disfunction");
                }
            }
                  
        }
        catch (const CdmwPlatformMngt::EntityNotFound& e)
        {
            m_errorSet = true;
            infoMsg = "Entity not found : ";
            infoMsg += m_entityName;
        }
        catch (const CORBA::SystemException& e)
        {
            m_errorSet = true;
                
            std::ostringstream exceptionInfo;
            exceptionInfo << e;
                
            infoMsg = "setProcessEntityStatus system exception : ";
            infoMsg += m_entityName;
            infoMsg += "\n";
            infoMsg += exceptionInfo.str();                
        }
        catch (const Cdmw::BadOrderException& e)
        {
            m_errorSet = true;
            infoMsg = "setProcessEntityStatus bad order exception";
        }
        catch (const Cdmw::BadParameterException& e)
        {
            m_errorSet = true;
            infoMsg = "setProcessEntityStatus bad parameter exception";
        }
                                    
        if (m_errorSet)
        {
            PlatformInterface::Notify_message(CdmwPlatformMngtBase::ERR,
                               m_processName.c_str(), infoMsg.c_str());
        }  
    }         
}
        


// Update Entity

//
// Ctor
//    
UpdateEntity::UpdateEntity (EntityToUpdate* p_entityToUpdate,
                            unsigned long updatingInterval)
    : m_stopped (false),
      m_timerCondition (m_mutex),
      m_updatingInterval (updatingInterval),      
      m_pEntityToUpdate (p_entityToUpdate)      
{

}




//
// Dtor
//
UpdateEntity::~UpdateEntity()
    throw()
{
    if (m_pEntityToUpdate != NULL)
    {
        delete m_pEntityToUpdate;
    }
}



//
// Ask to the internal thread for stopping
//
void UpdateEntity::stop()
    throw (InternalErrorException)
{
	if (m_stopped == false)
	{
		try 
	    {
	        // Lock the mutex
	        m_mutex.lock();

            // We require the thread to terminate
            m_stopped = true;

            // We awake the thread, if it is sleeping
            m_timerCondition.broadcast();

            // Unlock the mutex
	        m_mutex.unlock();
	
            // Wait the termination of the internal thread
            this->join();
        } 
        
        // No explanation, how is it possible : corrupted
	    catch (const OsSupport::DeadlockException&)
	    {
            CDMW_THROW (InternalErrorException);
        }
        catch (...)
	    {
            CDMW_THROW (InternalErrorException);
        }
    }
}




//
// Call in a new thread by the Thread system
//
void UpdateEntity::run()
    throw()
{
    try 
	{
		OsSupport::Condition::WaitReturn status;
		
		// lock mutex
		CDMW_MUTEX_GUARD (m_mutex);
		
        // Infinite loop, we stop when requested
        while (m_stopped == false) 
	    {
            // rearm timer and wait condition
            status = m_timerCondition.wait(m_updatingInterval);  
            
            // The time has expired, start updating
            if (status == OsSupport::Condition::TIMEDOUT)
            {
            	try
            	{
                    if (m_pEntityToUpdate != NULL)
                    {
                        m_pEntityToUpdate->update();
                    }
                }
                catch(...)
                {
 
                }
            }           
        }
    }
    
	// catch any exception and assert it
	catch (...)
	{
	  CDMW_ASSERT(false);
	}
}



