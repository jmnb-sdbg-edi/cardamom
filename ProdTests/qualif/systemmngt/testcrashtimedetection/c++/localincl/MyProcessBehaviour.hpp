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
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"
#include "testcrashtimedetection/TestCrashProcessMeasureThread.hpp"
#include "testcrashtimedetection/TestManage_impl.hpp"
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
class MyProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    /**
    * Purpose:
    * <p> The constructor.
    */ 
    MyProcessBehaviour(CORBA::ORB_ptr orb,
                       PortableServer::POA_ptr rootPOA,
                       const std::string& name,
                       const std::string& observer,
                       const std::string& killer,
                       const std::string& nbobserver,
                       const std::string& nbtest,
                       const std::string& supervision,
                       const std::string& limittime,
                       const std::string& percentcase)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_poa = PortableServer::POA::_duplicate(rootPOA);
        m_name = name;
        m_observer = observer;
        m_nbobserver = nbobserver;
        m_killer = killer;
        m_nbtest = nbtest;
        m_supervision = supervision;
        m_limittime = limittime;
        m_percentcase = percentcase;

        std::cout<<"m_observer: "<<m_observer<<" m_killer: "<<m_killer<<std::endl;
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
        return CORBA::Object::_nil();
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
	* operation
	*/
    virtual void on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CORBA::SystemException)
    {

        std::cout<< "on_initialise"<<std::endl;
        CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

        if (m_observer == "yes")
        {
            // ======================================================
            // Get HELLO name domain from repository
            // ======================================================
            CdmwNamingAndRepository::NameDomain_var helloDomain =
            repository->resolve_name_domain ("dom1/dom2/dom3");
            
            
            std::cout << m_name << " test_server: create killer_impl"<<std::endl;
            // create object hellointerface
            

            Cdmw::Test::observer_impl* observer_obj = new Cdmw::Test::observer_impl(m_orb.in());
            Cdmw::Test::observer_var observer = observer_obj->_this();
            
            // Get the CORBA reference of the system of the CDMW Supervision
            CORBA::Object_var systemObj = m_orb->string_to_object(m_supervision.c_str());
            
            CdmwPlatformMngt::System_var system = CdmwPlatformMngt::System::_narrow(systemObj.in());
            if (CORBA::is_nil(system.in())) {
                std::cout<<"can't resolve the supervision corbaloc"<<std::endl;
            }

            std::string processName = PlatformInterface::Get_process_name();
            system->register_observer( processName.c_str(), observer.in() );


            // ========================================================
            // Register the HelloInterface object in the HELLO name domain
            // ========================================================
            // reserve the name "HelloInterface" in the domain
            try
            {
                std::cout<<"Register dom1/dom2/dom3/"<<processName.c_str()<<std::endl;
                CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (processName.c_str());
                // register the object
                helloDomain->register_object (regId.in(), observer.in ());
            }
            catch (const CORBA::SystemException& e)
            {
                
                std::cerr << "ERROR: ------------------------"
                          << std::endl;
                std::cerr << e << std::endl;
            }
        }
        
    }

    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
	* operation
	*/
    virtual void on_run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CORBA::SystemException)
    {   
        
        if (m_killer == "yes")
        {
            m_test = new TestCrashProcessMeasureThread(m_orb.in(),
                                                       m_poa.in(),
                                                       m_name,
                                                       m_nbobserver,
                                                       m_nbtest,
                                                       m_supervision,
                                                       m_limittime,
                                                       m_percentcase);
            m_test->start();
        }
    }

    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
	* operation
	*/
    virtual void on_stop() throw(CORBA::SystemException)
    {
        if (m_observer == "yes")
        {
            
            // Get the CORBA reference of the system of the CDMW Supervision
            CORBA::Object_var systemObj = m_orb->string_to_object(m_supervision.c_str());
            
            CdmwPlatformMngt::System_var system = CdmwPlatformMngt::System::_narrow(systemObj.in());
            if (CORBA::is_nil(system.in())) {
                std::cout<<"can't resolve the supervision corbaloc"<<std::endl;
            }
            std::string processName = PlatformInterface::Get_process_name();
            system->unregister_observer(processName.c_str());
        }
        

        if (m_killer == "yes")
        {
            m_test->join();
        }
    }

private:

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;

    PortableServer::POA_var m_poa;
    
    std::string m_name;

    std::string m_observer;

    std::string m_killer;

    std::string m_nbobserver;

    std::string m_nbtest;

    std::string m_supervision;

    std::string m_limittime;
    
    std::string m_percentcase;

    TestCrashProcessMeasureThread * m_test;
};

    

#endif // INCL_MYPROCESSBEHAVIOUR_HPP

