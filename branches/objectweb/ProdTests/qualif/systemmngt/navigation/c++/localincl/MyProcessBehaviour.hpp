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


#ifndef INCL_MYPROCESSBEHAVIOUR_HPP 
#define INCL_MYPROCESSBEHAVIOUR_HPP 

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"
#include "navigation/TestNavigation.hpp"

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
                       PortableServer::POA_ptr rootPOA,
                       const std::string& name,
                       const std::string& supervision,
                       const std::string& host1,
                       const std::string& host2)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_poa = PortableServer::POA::_duplicate(rootPOA);
        m_name = name;
        m_supervision = supervision;
        m_host1 = host1;
        m_host2 = host2;
    }


    /**
    * Purpose:
    * <p> The destructor.
    */ 
    virtual ~MyProcessBehaviour() throw ()
    {
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
        return 1;
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
        // get application and process names                    
        m_applicationName = PlatformInterface::getApplicationName();
        m_processName = PlatformInterface::getProcessName();
        
        // example of using the PlatformInterface for notifying a message
        PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                                         m_processName.c_str(), "Initialisation requested");
        
        
        // init current step    
        m_currentInitStep = 0;

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

        if (!strcmp("NavigationProcess1",m_processName.c_str()))
        {
            PlatformInterface::setSystemEntityStatus ("SYSTEM_ENTITY",
                                                      CdmwPlatformMngtEntity::ENTITY_FUNCTIONING,
                                                      "system_functioning");
            
            PlatformInterface::setApplicationEntityStatus ("APPLICATION_ENTITY1", 
                                                           CdmwPlatformMngtEntity::ENTITY_FUNCTIONING,
                                                           "application1_functioning");
            
            PlatformInterface::setProcessEntityStatus ("PROCESS_ENTITY1",
                                                       CdmwPlatformMngtEntity::ENTITY_FUNCTIONING,
                                                       "process1_functioning");
        } else {
            PlatformInterface::setApplicationEntityStatus ("APPLICATION_ENTITY2", 
                                                           CdmwPlatformMngtEntity::ENTITY_DYSFUNCTION,
                                                           "application2_dysfunction");
            
            PlatformInterface::setProcessEntityStatus ("PROCESS_ENTITY2",
                                                       CdmwPlatformMngtEntity::ENTITY_FAILED_NO_RESPONSE,
                                                       "process2_failed_no_response");
        }
        
        if (m_supervision != "no")
        {
            Cdmw::TestNavigation test(m_orb.in(), m_poa.in(), m_name, m_supervision, m_host1, m_host2);
            test.start();
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

    }

private:

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;

    PortableServer::POA_var m_poa;
    
    std::string m_name;
    
    std::string m_host1;
    std::string m_host2;

    std::string m_supervision;
    
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

};


#endif // INCL_MYPROCESSBEHAVIOUR_HPP

