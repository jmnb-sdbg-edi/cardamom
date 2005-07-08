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

#ifndef INCL_HELLO_PROCESS_BEHAVIOUR_HPP 
#define INCL_HELLO_PROCESS_BEHAVIOUR_HPP 

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/common/Exception.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>


/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class HelloProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
{

public:

    /**
    * Purpose:
    * <p> The constructor.
    */ 
    HelloProcessBehaviour  (CORBA::ORB_ptr orb) 
    {
        m_orb = CORBA::ORB::_duplicate(orb);
    }


    /**
    * Purpose:
    * <p> The destructor.
    */ 
    virtual ~HelloProcessBehaviour() throw ()
    {
     
    }

    
    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/nb_steps:1.0
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
	* IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/get_service:1.0
	* operation
	*/
    virtual CORBA::Object_ptr get_service() throw(CORBA::SystemException)
    {
        return CORBA::Object::_nil();
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/initialise:1.0
	* operation
	*/
    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::Process::BadOrder, CORBA::SystemException)
    {
        // ==============================================================
        // get application and process names   
        // ==============================================================                 
        m_applicationName = Cdmw::PlatformMngt::PlatformInterface::getApplicationName();
        m_processName = Cdmw::PlatformMngt::PlatformInterface::getProcessName();
                    
        // ==============================================================
        // example of using the PlatformInterface for notifying a message
        // ==============================================================
        Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                m_processName.c_str(), 
                ">>>>>>>>>>>>>> Initialisation requested by supervision");                            
    }



    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/run:1.0
	* operation
	*/
    virtual void run()
        throw(CdmwPlatformMngt::Process::NotReadyToRun,
              CdmwPlatformMngt::Process::AlreadyDone,
              CORBA::SystemException)
    {    
        // ==============================================================        
        // example of using the PlatformInterface for notifying a message
        // ==============================================================
        Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                m_processName.c_str(), 
                ">>>>>>>>>>>>>> Run requested by supervision");                
    }

    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/stop:1.0
	* operation
	*/
    virtual void stop() throw(CORBA::SystemException)
    {    
        // ==============================================================         
        // example of using the PlatformInterface for notifying a message
        // ==============================================================
        Cdmw::PlatformMngt::PlatformInterface::notifyMessage(CdmwPlatformMngtBase::INF,
                m_processName.c_str(), 
                ">>>>>>>>>>>>>> Stop requested by supervision");

        // ==============================================================
        // Shut down the orb
        // ==============================================================
        m_orb->shutdown(false);
    }


    
private:

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;
    
    /**
    * The application name.
    */
    std::string m_applicationName;
      
    /**
    * The process name.
    */
    std::string m_processName;
};


#endif // INCL_HELLO_PROCESS_BEHAVIOUR_HPP
