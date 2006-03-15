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
#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"



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
    MyProcessBehaviour(CORBA::ORB_ptr orb,int argc, char* argv[])
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        message = OS::get_option_value (argc, argv, "-message");
        
        std::ostringstream SystemCorbaloc;

        SystemCorbaloc
        << "corbaloc::" << Cdmw::OsSupport::OS::get_hostname()
        <<  ":" << 21869
        <<  "/" <<"CdmwPlatformMngtSupervision";
        system = SystemCorbaloc.str();
        
    }


    /**
    * Purpose:
    * <p> The destructor.
    */ 
    virtual ~MyProcessBehaviour() throw ()
    {

    }

    CORBA::Short get_counter()
    {
        return 0;
    }
    
    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/nb_steps:1.0
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
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
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
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
	* operation
	*/
    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException)
    {
        std::cout<<"(argc, argv) message : "<<message<<std::endl;

        
        // retrieve System object from Supervision
        CORBA::Object_var obj = m_orb->string_to_object(system.c_str());
        CdmwPlatformMngt::System_var system = 
            CdmwPlatformMngt::System::_narrow(obj.in());

        if (strcmp(PlatformInterface::Get_application_name(),"APPL1") == 0)
        {

            // get Application
            CdmwPlatformMngt::Application_var appl1 = 
            system->get_application("APPL1");
            
            // get Process proxy
            CdmwPlatformMngt::Process_var process1 =
            appl1->get_process("P11", Cdmw::OsSupport::OS::get_hostname().c_str());
            
            CdmwPlatformMngt::ProcessInfo_var info1 =  process1->get_process_info();
            std::cout<<"(ProcessInfo) P11 message : "<<info1->process_args<<std::endl;
            
            // get Application
            CdmwPlatformMngt::Application_var appl3 = 
            system->get_application("APPL3");
            // get Process proxy
            CdmwPlatformMngt::Process_var process3 =
            appl3->get_process("P31", Cdmw::OsSupport::OS::get_hostname().c_str());
            
            CdmwPlatformMngt::ProcessInfo_var info3 =  process3->get_process_info();
            std::cout<<"(ProcessInfo) P31 message : "<<info3->process_args<<std::endl;
        } else {

            // get Application
            CdmwPlatformMngt::Application_var appl2 = 
            system->get_application("APPL2");
            
            // get Process proxy
            CdmwPlatformMngt::Process_var process2 =
            appl2->get_process("P21", Cdmw::OsSupport::OS::get_hostname().c_str());
            
            CdmwPlatformMngt::ProcessInfo_var info2 =  process2->get_process_info();
            std::cout<<"(ProcessInfo) P21 message : "<<info2->process_args<<std::endl;
            
            // get Application
            CdmwPlatformMngt::Application_var appl4 = 
            system->get_application("APPL4");

            // get Process proxy
            CdmwPlatformMngt::Process_var process4 =
            appl4->get_process("P41", Cdmw::OsSupport::OS::get_hostname().c_str());
            
            CdmwPlatformMngt::ProcessInfo_var info4 =  process4->get_process_info();
            std::cout<<"(ProcessInfo) P41 message : "<<info4->process_args<<std::endl;
        }
        
    }


    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
	* operation
	*/
    virtual void next_step()
        throw(CdmwPlatformMngt::ProcessDelegate::InvalidStep, CORBA::SystemException)
    {

    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
	* operation
	*/
    virtual void run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
              CORBA::SystemException)
    {

    }

    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
	* operation
	*/
    virtual void stop() throw(CORBA::SystemException)
    {
        m_orb->shutdown(false);
    }

    /**
        * Purpose:
        * <p>
        * the behaviour for the
        * IDL:FT/PullMonitorable/is_alive:1.0
        * operation
        */
    virtual bool is_alive()
        throw(CORBA::SystemException)
    {
        return true;
    }

    /**
	* Purpose:
	* <p> auto ending the process
	*/
    virtual void autoEnd()
    {

    }

    
    
private:

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;

    std::string message;

    std::string system;

};


#endif // INCL_MYPROCESSBEHAVIOUR_HPP

