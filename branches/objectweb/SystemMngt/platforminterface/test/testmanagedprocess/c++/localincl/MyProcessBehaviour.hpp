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


#ifndef INCL_MYPROCESSBEHAVIOUR_HPP 
#define INCL_MYPROCESSBEHAVIOUR_HPP 

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"

#include "testmanagedprocess/AutoEnding.hpp"
#include "testmanagedprocess/UpdateEntity.hpp"


#include <string>
#include <sstream>
#include <iostream>
#include <fstream>



using namespace Cdmw::PlatformMngt;
using namespace Cdmw::OsSupport;



/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
{

public:

    /**
    * Purpose:
    * <p> The constructor.
    */ 
    MyProcessBehaviour(CORBA::ORB_ptr orb, 
                       bool platformManaged,
                       char *p_processExe,
                       AutoEnding* pAutoEnding,
                       long autoEndTimeOut = -1, 
                       unsigned long entityUpdateInterval = 5000,
                       int nbrOfInitSteps=1)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_platformManaged = platformManaged;
        m_processName = p_processExe;
        m_autoEndTimeOut = autoEndTimeOut;
        m_entityUpdateInterval = entityUpdateInterval;
        m_nbrOfInitSteps = nbrOfInitSteps;
        m_currentInitStep = 0;
        m_startup_mode = 0;
        
        m_pAutoEnding = pAutoEnding;
        if (m_pAutoEnding != NULL)
        {
            m_pAutoEnding->set_ProcessBehaviour(this);
        }
        
        m_pUpdateSystemEntity = NULL;
        m_pUpdateApplicationEntity = NULL;
        m_pUpdateProcessEntity = NULL;
    }


    /**
    * Purpose:
    * <p> The destructor.
    */ 
    virtual ~MyProcessBehaviour() throw ()
    {
        if (m_pUpdateSystemEntity != NULL)
        {
            delete m_pUpdateSystemEntity;
            m_pUpdateSystemEntity = NULL;
        }
        
        if (m_pUpdateApplicationEntity != NULL)
        {
            delete m_pUpdateApplicationEntity;
            m_pUpdateApplicationEntity = NULL;
        }
        
        if (m_pUpdateProcessEntity != NULL)
        {
            delete m_pUpdateProcessEntity;
            m_pUpdateProcessEntity = NULL;
        }        
    }

    
    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
    * attribute
    */
    virtual CORBA::ULong nb_steps() throw(CORBA::SystemException)
    {
        return m_nbrOfInitSteps;
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
	* operation
	*/
    virtual CORBA::Object_ptr get_service() throw(CORBA::SystemException)
    {
        // example of using the PlatformInterface for notifying a message
        PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
            m_processName.c_str(), "Service requested->NIL returned");
            
        return CORBA::Object::_nil();
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
	* operation
	*/
    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::Process::BadOrder, CORBA::SystemException)
    {
        if (m_platformManaged)
        {
            // get application and process names                    
            m_applicationName = PlatformInterface::getApplicationName();
            m_processName = PlatformInterface::getProcessName();
                    
            // example of using the PlatformInterface for notifying a message
            PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                m_processName.c_str(), "Initialisation requested");
        }
        
        else
        {
            std::cout << m_processName.c_str() << "Initialisation requested"
                      << std::endl;
        }
        
        // init current step    
        m_currentInitStep = 0;
        
        
        // set startup mode
        m_startup_mode = startup_kind.startup_mode;
        
        // if timeout for init phase
        if (m_startup_mode > 0)
        {
            if (m_platformManaged)
            { 
                std::ostringstream msgInfo;
                msgInfo << "Init Timeout requested :";
                msgInfo << m_startup_mode;
                msgInfo << " msec";
                     
                PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                    m_processName.c_str(), msgInfo.str().c_str());
            }
        
            else
            {
                std::cout << m_processName.c_str() << "Init Timeout requested :"
                          << m_startup_mode << " msec"
                          << std::endl;
            }
        
            OS::sleep (m_startup_mode);
        }
        
        
        
        // creates the EntityToUpdate classes
        std::auto_ptr<SystemEntityToUpdate> pSystemEntityToUpdate (
                    new SystemEntityToUpdate (m_sysEntityName, m_processName));
                                
        std::auto_ptr<ApplicationEntityToUpdate> pApplicationEntityToUpdate (
                    new ApplicationEntityToUpdate (m_appEntityName, m_processName));
                                
        std::auto_ptr<ProcessEntityToUpdate> pProcessEntityToUpdate (
                    new ProcessEntityToUpdate (m_procEntityName, m_processName));
                                
      
        // creates the UpdateEntity classes
        std::auto_ptr<UpdateEntity> pUpdateSystemEntity (
                    new UpdateEntity (pSystemEntityToUpdate.get(),
                                      m_entityUpdateInterval));
        pSystemEntityToUpdate.release();
                                
        std::auto_ptr<UpdateEntity> pUpdateApplicationEntity (
                    new UpdateEntity (pApplicationEntityToUpdate.get(),
                                      m_entityUpdateInterval));
        pApplicationEntityToUpdate.release();
                                
        std::auto_ptr<UpdateEntity> pUpdateProcessEntity (
                    new UpdateEntity (pProcessEntityToUpdate.get(),
                                      m_entityUpdateInterval));
        pProcessEntityToUpdate.release();
                                
        // transfer auto pointers
        m_pUpdateSystemEntity = pUpdateSystemEntity.get();
        m_pUpdateApplicationEntity = pUpdateApplicationEntity.get();
        m_pUpdateProcessEntity = pUpdateProcessEntity.get();
                         
        pUpdateSystemEntity.release(); 
        pUpdateApplicationEntity.release(); 
        pUpdateProcessEntity.release();                
    }


    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
	* operation
	*/
    virtual void next_step()
        throw(CdmwPlatformMngt::Process::InvalidStep, CORBA::SystemException)
    {
        // increment init step
        if (m_currentInitStep < m_nbrOfInitSteps)
        {    
          m_currentInitStep ++;
        }
        
        std::ostringstream msgInfo;
        msgInfo << "Step requested :";
        msgInfo << m_currentInitStep;
        
        if (m_platformManaged)
        {              
            // example of using the PlatformInterface for notifying a message
            PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                m_processName.c_str(), msgInfo.str().c_str());
        }
        
        else
        {
            std::cout << m_processName.c_str() << msgInfo.str().c_str()
                      << std::endl;
        }
               
        // if invalid init step
        if (m_currentInitStep >= m_nbrOfInitSteps)
        {
            throw CdmwPlatformMngt::Process::InvalidStep();
        }            
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
	* operation
	*/
    virtual void run()
        throw(CdmwPlatformMngt::Process::NotReadyToRun,
              CdmwPlatformMngt::Process::AlreadyDone,
              CORBA::SystemException)
    {
        if (m_platformManaged)
        {              
            // example of using the PlatformInterface for notifying a message
            PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                m_processName.c_str(), "Run requested");
        }
        
        else
        {
            std::cout << m_processName.c_str() << "Run requested"
                      << std::endl;
        }
       
        
        if (!m_serviceName.empty())
        {
            bool errorSet = false;
            std::string infoMsg;
            
            try
            {
                CORBA::Object_var serviceObject = PlatformInterface::getService (m_serviceName.c_str());
            }
            catch (const CdmwPlatformMngtService::ServiceNotFound& e)
            {
                errorSet = true;
                infoMsg = "Service not found : ";
                infoMsg += m_serviceName;
            }
            catch (const CdmwPlatformMngtService::ServiceUnavailable& e)
            {
                errorSet = true;
                infoMsg = "Service not available : ";
                infoMsg += m_serviceName;
            }
            catch (const CORBA::SystemException& e)
            {
                errorSet = true;
                
                std::ostringstream exceptionInfo;
                exceptionInfo << e;
                
                infoMsg = "getService system exception : ";
                infoMsg += m_serviceName;
                infoMsg += "\n";
                infoMsg += exceptionInfo.str();
                
            }
            catch (const Cdmw::BadOrderException& e)
            {
                errorSet = true;
                infoMsg = "getService bad order exception";
            }
            catch (const Cdmw::BadParameterException& e)
            {
                errorSet = true;
                infoMsg = "getService bad parameter exception";
            }
            
            if (errorSet)
            {
                PlatformInterface::notifyMessage(CdmwPlatformMngtBase::ERR,
                               m_processName.c_str(), infoMsg.c_str());
            }
            else
            {
                infoMsg = "Service has been found : ";
                infoMsg += m_serviceName;
                
                PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                               m_processName.c_str(), infoMsg.c_str());
            }
        }
        
        
        // if system entity to update
        if (!m_sysEntityName.empty())
        {
            try
            {
                // start thread
                m_pUpdateSystemEntity->start();
            }
            catch (...)
            {
                PlatformInterface::notifyMessage(CdmwPlatformMngtBase::ERR,
                      m_processName.c_str(), "System Entity Update cannot be started");
            }
        }
    
        // if application entity to update
        if (!m_appEntityName.empty())
        {
            try
            {
                // start thread
                m_pUpdateApplicationEntity->start();
            }
            catch (...)
            {
                PlatformInterface::notifyMessage(CdmwPlatformMngtBase::ERR,
                      m_processName.c_str(), "Application Entity Update cannot be started");
            }
        }
        
        // if process entity to update
        if (!m_procEntityName.empty())
        {
            try
            {
                // start thread
                m_pUpdateProcessEntity->start();
            }
            catch (...)
            {
                PlatformInterface::notifyMessage(CdmwPlatformMngtBase::ERR,
                      m_processName.c_str(), "Process Entity Update cannot be started");
            }
        }
        
        // if process auto ending
        if (m_pAutoEnding != NULL && m_autoEndTimeOut > 0)
        {
            try
            {
                // start thread
                m_pAutoEnding->start();
            }
            catch (...)
            {
                PlatformInterface::notifyMessage(CdmwPlatformMngtBase::ERR,
                      m_processName.c_str(), "AutoEnding cannot be started");
            }
        }
              
        // if timeout for running phase
        if (m_startup_mode > 0)
        {
            if (m_platformManaged)
            { 
                std::ostringstream msgInfo;
                msgInfo << "Running Timeout requested :";
                msgInfo << m_startup_mode;
                msgInfo << " msec";
                     
                PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                    m_processName.c_str(), msgInfo.str().c_str());
            }
        
            else
            {
                std::cout << m_processName.c_str() << "Running Timeout requested :"
                          << m_startup_mode << " msec"
                          << std::endl;
            }
        
            OS::sleep (m_startup_mode);
        }
        
    }

    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
	* operation
	*/
    virtual void stop() throw(CORBA::SystemException)
    {
        if (m_platformManaged)
        {              
            // example of using the PlatformInterface for notifying a message
            PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                m_processName.c_str(), "Stop requested");
        }
        
        else
        {
            std::cout << m_processName.c_str() << "Stop requested"
                      << std::endl;
        }
        
        try
        {   
            // if process auto ending request to stop thread 
            //    if not already stopped
            if (m_pAutoEnding != NULL && m_autoEndTimeOut > 0)
            {
                m_pAutoEnding->stop();
            }
            
            // if system entity updating to stop
            if (!m_sysEntityName.empty())
            {
                // start thread
                m_pUpdateSystemEntity->stop();
            }
            
            // if application entity updating to stop
            if (!m_appEntityName.empty())
            {
                // start thread
                m_pUpdateApplicationEntity->stop();
            }
            
            // if process entity updating to stop
            if (!m_procEntityName.empty())
            {
                // start thread
                m_pUpdateProcessEntity->stop();
            }
        }
        catch (...)
        {
            // nothing to do
        }
        
        
        
        // stop timeout using startup mode
        
        if (m_startup_mode > 0)
        {
            if (m_platformManaged)
            { 
                std::ostringstream msgInfo;
                msgInfo << "Stop Timeout requested :";
                msgInfo << m_startup_mode;
                msgInfo << " msec";
                     
                PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                    m_processName.c_str(), msgInfo.str().c_str());
            }
        
            else
            {
                std::cout << m_processName.c_str() << "Stop Timeout requested :"
                          << m_startup_mode << " msec"
                          << std::endl;
            }

            OS::sleep (m_startup_mode);
        }
        
        

        m_orb->shutdown(false);
    }


    /**
	* Purpose:
	* <p> auto ending the process
	*/
    virtual void autoEnd()
    {
        if (m_platformManaged)
        {              
            // example of using the PlatformInterface for notifying a message
            PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                m_processName.c_str(), "Process auto ending");
        }
        
        else
        {
            std::cout << m_processName.c_str() << "Process auto ending"
                      << std::endl;
        }
        
        m_orb->shutdown(false);
    }

    
    /**
    * Purpose:
    * <p> Set service name to request.
    */ 
    void setServiceToRequest (const char* p_serviceName)
    {
        if (p_serviceName != NULL)
        {
            m_serviceName = p_serviceName;
        }
    }
    
    
    /**
    * Purpose:
    * <p> Set system entity name to set.
    */ 
    void setSysEntityToSet (const char* p_entityName)
    {
        if (p_entityName != NULL)
        {
            m_sysEntityName = p_entityName;
        }
    }
    
    /**
    * Purpose:
    * <p> Set application entity name to set.
    */ 
    void setAppEntityToSet (const char* p_entityName)
    {
        if (p_entityName != NULL)
        {
            m_appEntityName = p_entityName;
        }
    }
    
    /**
    * Purpose:
    * <p> Set process entity name to set.
    */ 
    void setProcEntityToSet (const char* p_entityName)
    {
        if (p_entityName != NULL)
        {
            m_procEntityName = p_entityName;
        }
    }
    
    
private:

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;


    /**
    * To indicate if supervised by platform management.
    */
    bool m_platformManaged;
    
    
    /**
    * The time out for process auto ending after run.
    * -1 if no auto ending
    */
    long m_autoEndTimeOut;
    
    
    /**
    * The time interval to update entity
    */
    unsigned long m_entityUpdateInterval;
    
    /**
    * The number of init step defined for this process.
    */
    int m_nbrOfInitSteps;
    
    /**
    * The current init step.
    */
    int m_currentInitStep;
    
    /**
    * The application name.
    */
    std::string m_applicationName;
      
    /**
    * The process name.
    */
    std::string m_processName;
     
    /**
    * The service name to request.
    */
    std::string m_serviceName;
    
    /**
    * The system entity name to set.
    */
    std::string m_sysEntityName;
    
    /**
    * The application entity name to set.
    */
    std::string m_appEntityName;
    
    /**
    * The process entity name to set.
    */
    std::string m_procEntityName;
    
    /**
    * The system entity to update.
    */
    UpdateEntity* m_pUpdateSystemEntity;
    
    /**
    * The application entity to update.
    */
    UpdateEntity* m_pUpdateApplicationEntity;
    
    /**
    * The process entity to update.
    */
    UpdateEntity* m_pUpdateProcessEntity;
    
    /**
    * The startup mode.
    *     < 0 if initialisation time out in msec
    *     > 0 if running time out in msec
    */
    CdmwPlatformMngtBase::StartupMode m_startup_mode;
    
    
    /**
    * The AutoEnding thread class.
    */
    AutoEnding* m_pAutoEnding;

};


#endif // INCL_MYPROCESSBEHAVIOUR_HPP

