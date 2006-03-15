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

#ifndef INCL_MY_PROCESS_BEHAVIOUR_HPP 
#define INCL_MY_PROCESS_BEHAVIOUR_HPP 

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include <FaultTolerance/idllib/FT.stub.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "testftwritesequencing/TestHello_impl.hpp"
#include "testftwritesequencing/TestSequenceRecorder_impl.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
//class MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
class MyProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    /**
    * Purpose:
    * <p> The constructor.
    */ 
    MyProcessBehaviour  (CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA, std::string the_host, bool recorder)
        : m_hostname(the_host),
          m_recorder(recorder)

    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_rootPOA =  PortableServer::POA::_duplicate(rootPOA);
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
    virtual void on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw( CORBA::SystemException)
    {

        Cdmw::OsSupport::OS::sleep(100);
        // get application name
        std::string application_name =  Cdmw::PlatformMngt::PlatformInterface::Get_application_name();
        
        // get process name
        std::string process_name = Cdmw::PlatformMngt::PlatformInterface::Get_process_name();

        ::FT::Location loc;
        loc.length(3);
        loc[0].id = CORBA::string_dup(m_hostname.c_str());
        loc[0].kind = "hostname";
        loc[1].id = CORBA::string_dup(application_name.c_str());
        loc[1].kind = "applicationname";
        loc[2].id = CORBA::string_dup(process_name.c_str());
        loc[2].kind = "processname";

        m_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
        

        CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

        // ======================================================
        // Get HELLO name domain from repository
        // ======================================================
        CdmwNamingAndRepository::NameDomain_var helloDomain =
            repository->resolve_name_domain ("dom1/dom2/dom3");

        if (m_recorder)
        {
            std::cout << m_name << " test_server: create TestSequenceRecorder_impl"<<std::endl;

            Cdmw::SequenceRecorder_impl * recorder_impl
                = new ::Cdmw::SequenceRecorder_impl (
                    m_orb.in(), m_rootPOA.in(), m_name.c_str());
            Test::SequenceRecorder_var recorderInterface
                =  recorder_impl->_this();
        
            std::string full_name = process_name + "Recorder";
        
            try
            {
                CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (full_name.c_str());
                // register the object
                helloDomain->register_object (regId.in() , recorderInterface.in ());
            }
            catch (const CORBA::SystemException& e)
            {
            
                std::cerr << "ERROR: ------------------------"
                          << std::endl;
                std::cerr << e << std::endl;
            }
        }
        else
        {
            std::cout << m_name << " test_server: create HelloInterface_impl"<<std::endl;
            // create object hellointerface
            Cdmw::HelloInterface_impl * helloInterface_impl
                = new ::Cdmw::HelloInterface_impl (
                    m_orb.in(),
                    m_rootPOA.in(),
                    m_name.c_str(),
                    m_hostname.c_str());
            CdmwReplicationManager::HelloInterface_var helloInterface
                =  helloInterface_impl->_this();
        
            std::string full_name = process_name + "HelloInterface";
        
            // ========================================================
            // Register the HelloInterface object in the HELLO name domain
            // ========================================================
            // reserve the name "HelloInterface" in the domain
            try
            {
                CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (full_name.c_str());
                // register the object
                helloDomain->register_object (regId.in() , helloInterface.in ());
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
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
	* operation
	*/
    virtual void on_run()
        throw(CORBA::SystemException,
              CdmwPlatformMngt::ProcessDelegate::NotReadyToRun)
    {    

    }

    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
	* operation
	*/
    virtual void on_stop() throw(CORBA::SystemException)
    {    

    }


    
private:

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;
    /**
     * The poa
     */
    PortableServer::POA_var m_rootPOA;
    
    /**
    * The application name.
    */
    std::string m_name;
    std::string m_hostname;
    bool m_recorder;
};


#endif // INCL_MY_PROCESS_BEHAVIOUR_HPP

