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
#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"
#include "TraceAndPerf/tracelibrary/Trace.hpp"
#include <FaultTolerance/idllib/CdmwFTActivation.skel.hpp>
#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include "faulttoleranceserver/TestHello_impl.hpp"
#include <FaultTolerance/ftinit/FTServiceInit.hpp>


#include <FaultTolerance/idllib/CdmwFTActivation.skel.hpp>

#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

class HelloActivationHandler : virtual public CdmwFT::Location::ActivationHandler, 
                               virtual public Cdmw::OrbSupport::RefCountLocalObject
{
public:
    HelloActivationHandler(const std::string& name)
        :m_name(name)
    {};
    
    virtual void activate() throw(CORBA::SystemException)
    {
        std::cout << m_name <<" <####### HELLO ACTIVATION ######>" << std::endl;
    };
    
    
    virtual void deactivate() throw(CORBA::SystemException)
    {
        std::cout << m_name << " <####### HELLO DEACTIVATION ######>" << std::endl;
    };

private:
    /**
    * The application name.
    */
    std::string m_name;
};

class GroupRepositoryThread : public Cdmw::OsSupport::Thread {

public:
        
    GroupRepositoryThread(CORBA::ORB_ptr orb, CORBA::Object_ptr member,
                          std::string full_name)
        :m_orb (CORBA::ORB::_duplicate(orb)),
         m_member (CORBA::Object::_duplicate(member)),
         m_full_name (full_name)
    {            
    };
    
     
        
    ~GroupRepositoryThread() throw()
    {};
    


    void
    shutdown();
	 
protected:

    void
    run() throw()
    {
        // Get object group ref
        ::FT::ObjectGroup_var group_ref = ::FT::ObjectGroup::_nil();
        CdmwFT::Location::GroupRepository_var group_repository;
        try
        {
            CORBA::Object_var obj = m_orb->resolve_initial_references("FTGroupRepository");
            group_repository = CdmwFT::Location::PrimaryBackupGroupRepository::_narrow(obj.in());
        }
        catch (const CORBA::ORB::InvalidName& )
        {
            std::cerr << "ERROR: Impossible to resolve FTGroupRepository! Name is invalid!"
                      << std::endl;
        }
        int i=0;
        while (i < 4000)
        {
            try 
            {
                group_ref = group_repository->get_object_group(m_member.in());
                
                // result ok, get out from the loop
                break;
            }
            catch (const FT::MemberNotFound&)
            {
                // nothing to do, wait for group_rep init!
            }
            Cdmw::OsSupport::OS::sleep(1000);
            
            i++;
        }
        
        if (i == 4000)
        {
            std::cerr << "ERROR: Impossible to get the group! "
                      << std::endl;
            
        }
        
        // Create activation handler and register it
        std::cout<< m_full_name << " test_server: create activation handler and register it"<<std::endl;
        CdmwFT::Location::ActivationHandler_var activation_handler 
        = new HelloActivationHandler(m_full_name);
        
        try
        {
            CORBA::Object_var obj = m_orb->resolve_initial_references("FTActivationManager");
            CdmwFT::Location::ActivationManager_var  activation_manager 
            = CdmwFT::Location::ActivationManager::_narrow(obj.in());
            
            activation_manager->register_activation_handler(group_ref.in(), activation_handler.in()); 
        }
        catch (const FT::ObjectGroupNotFound&)
        {
            std::cerr << "ERROR: Impossible to register FTActivationHandler! Object Group not found!"
                      << std::endl;
        }
        catch (const CORBA::ORB::InvalidName& )
        {
            std::cerr << "ERROR: Impossible to register FTActivationManager! Name is invalid!"
                      << std::endl;
        }
        catch (const CORBA::SystemException& e)
        {
            
            std::cerr << "ERROR: Impossible to register FTActivationHandler!"
                      << std::endl;
            std::cerr << e << std::endl;
        }
    };
    
public:

    CORBA::ORB_var  m_orb;
    CORBA::Object_var m_member;
    std::string     m_full_name;

};



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
    MyProcessBehaviour  (CORBA::ORB_ptr orb, const std::string& the_host, int val, const std::string& the_host_file_loc)
        : m_orb(CORBA::ORB::_duplicate(orb)),
          m_hostname(the_host),
          m_hostname_file_location(the_host_file_loc),
          m_val(val),
          m_group_repository_thread1(NULL),
          m_group_repository_thread2(NULL),
          m_group_repository_thread3(NULL)
    {
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
        
        int val = m_val;
        
//         // Get the Naming Context reference
//         CORBA::Object_var naming_context_object = m_orb->resolve_initial_references ("NameService");
//         CosNaming::NamingContext_var naming_context = CosNaming::NamingContext::_narrow (naming_context_object.in ());

    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

        // ======================================================
        // Get HELLO name domain from repository
        // ======================================================
        CdmwNamingAndRepository::NameDomain_var helloDomain =
            repository->resolve_name_domain ("dom1/dom2/dom3");
       

        if (val > 3)
        {
            std::cout << m_name << " test_server: create HelloInterface3_impl"<<std::endl;
            Cdmw::Test::HelloInterface3_impl * hello_interface 
            = new Cdmw::Test::HelloInterface3_impl(m_orb.in(), m_name);
            Cdmw::Test::HelloInterface3_var hello_interface_ref 
            =  hello_interface->_this();
            val = val - 4;

            std::string full_name = process_name + "HelloInterface3";


            // ========================================================
            // Register the HelloInterface object in the HELLO name domain
            // ========================================================
            // reserve the name "HelloInterface" in the domain
            CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (full_name.c_str());
            // register the object
            helloDomain->register_object (regId.in(), hello_interface_ref.in ());

            m_group_repository_thread3 = new GroupRepositoryThread(m_orb.in(), hello_interface_ref.in(),  full_name);
            m_group_repository_thread3->start();
        }
        
        if (val > 1)
        {
            std::cout << m_name << " test_server: create HelloInterface2_impl"<<std::endl;
            Cdmw::Test::HelloInterface2_impl * hello_interface 
            = new Cdmw::Test::HelloInterface2_impl(m_orb.in(), m_name);
            Cdmw::Test::HelloInterface2_var hello_interface_ref 
            =  hello_interface->_this();
            val = val - 2;

            std::string full_name = process_name + "HelloInterface2";

            // ========================================================
            // Register the HelloInterface object in the HELLO name domain
            // ========================================================
            // reserve the name "HelloInterface" in the domain
            CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (full_name.c_str());
            // register the object
            helloDomain->register_object (regId.in(), hello_interface_ref.in ());

            m_group_repository_thread2 = new GroupRepositoryThread(m_orb.in(), hello_interface_ref.in (),  full_name);
            m_group_repository_thread2->start();


        }

        if (val > 0)
        {
            std::cout << m_name << " test_server: create HelloInterface1_impl"<<std::endl;
            Cdmw::Test::HelloInterface1_impl * hello_interface = 
               new Cdmw::Test::HelloInterface1_impl(m_orb.in(), 
                                                    m_name, 
                                                    m_hostname_file_location);
            Cdmw::Test::HelloInterface1_var hello_interface_ref =  
               hello_interface->_this();
            val = val - 1;

            std::string full_name = process_name + "HelloInterface1";

            // ========================================================
            // Register the HelloInterface object in the HELLO name domain
            // ========================================================
            // reserve the name "HelloInterface" in the domain
            for (int i=0; i<2 ; ++i)
            {
                try
                {
                    CdmwNamingAndRepository::NameDomain::RegistrationId_var regId = helloDomain->new_name (full_name.c_str());
                    // register the object
                    helloDomain->register_object (regId.in(), hello_interface_ref.in ());
                    std::cout << "register_object succeeded" << std::endl; 
                    break;
                }
                catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists& )
                {
                    std::cerr << "register_object raises AlreadyExists exception" << std::endl;
                    try
                    {
                        helloDomain->release_name(full_name.c_str());
                    }
                    catch (const CdmwNamingAndRepository::NameDomain::NotRegistered& )
                    {
                         std::cerr << "ERROR: ------------------------" << std::endl;
                         std::cerr << "release_name raises NotRegistered " << std::endl;
                         break;
                    }
                    catch (const CdmwNamingAndRepository::InvalidName& )
                    {
                         std::cerr << "ERROR: ------------------------" << std::endl;
                         std::cerr << "release_name raises InvalidName " << std::endl;
                         break;
                    }
                    catch (const CORBA::SystemException& e)
                    {
                         std::cerr << "ERROR: ------------------------" << std::endl;
                         std::cerr << "register_object raises " << e << std::endl;
                         break;
                    }
                }
                catch (const CdmwNamingAndRepository::NameDomain::InvalidRegistration&)
                {
                    std::cerr << "ERROR: ------------------------" << std::endl;
                    std::cerr << "register_object raises InvalidRegistration " << std::endl;
                    break;
                }
                catch (const CORBA::SystemException& e)
                {
                    
                    std::cerr << "ERROR: ------------------------" << std::endl;
                    std::cerr << "register_object raises " << e << std::endl;
                    break;
                }
            }

            m_group_repository_thread1 = new GroupRepositoryThread(m_orb.in(), hello_interface_ref.in (),  full_name);
            m_group_repository_thread1->start();


         }
        std::cout<< m_name << " test_server: orb run"<<std::endl;
        Cdmw::OsSupport::OS::sleep(500);
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
    * The application name.
    */
    std::string m_name;
    std::string m_hostname;
    std::string m_hostname_file_location;
    int m_val;
    GroupRepositoryThread* m_group_repository_thread1;
    GroupRepositoryThread* m_group_repository_thread2;
    GroupRepositoryThread* m_group_repository_thread3;
};


#endif // INCL_MY_PROCESS_BEHAVIOUR_HPP

