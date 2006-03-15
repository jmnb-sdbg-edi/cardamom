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

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/common/Exception.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>
#include <TraceAndPerf/tracelibrary/Trace.hpp>



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
        HelloProcessBehaviour  (CORBA::ORB_ptr orb) : m_countIsAlive(0)
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
	* IDL:thalesgroup.com/CdmwSystemMngt/platformlibrary/Process/initialise:1.0
	* operation
	*/
    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException)
    {
        // ==============================================================
        // get application and process names   
        // ==============================================================                 
        m_applicationName = Cdmw::PlatformMngt::PlatformInterface::Get_application_name();
        m_processName = Cdmw::PlatformMngt::PlatformInterface::Get_process_name();
                    
        // ==============================================================
        // example of using the PlatformInterface for notifying a message
        // ==============================================================
        Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
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
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
              CORBA::SystemException)
    {    
        // ==============================================================        
        // example of using the PlatformInterface for notifying a message
        // ==============================================================
        Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
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
        Cdmw::PlatformMngt::PlatformInterface::Notify_message(CdmwPlatformMngtBase::INF,
                m_processName.c_str(), 
                ">>>>>>>>>>>>>> Stop requested by supervision");

        // ==============================================================
        // Shut down the orb
        // ==============================================================
        m_orb->shutdown(false);
    }

    /**
	* Purpose:
	* <p>
	* the behaviour for the IDL:FT/PullMonitorable/is_alive:1.0 operation
	*/
    virtual bool is_alive() throw(CORBA::SystemException)
    {    
        m_countIsAlive++;
        if ( m_countIsAlive < 10)
        {
            std::cout << "Pull monitoring active : is_alive called" << std::endl;
        }
        else if ( m_countIsAlive == 10)
        {
            std::cout << "is_alive tracing will switch off now (to avoid too much traces)" << std::endl;
        }
        return true;
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

    int m_countIsAlive;
};


#endif // INCL_HELLO_PROCESS_BEHAVIOUR_HPP
