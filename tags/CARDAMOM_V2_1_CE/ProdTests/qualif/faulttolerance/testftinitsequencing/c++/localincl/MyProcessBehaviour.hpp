/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * This file and the information it contains are confidential and proprietary. 
 * They shall not be reproduced nor disclosed to any person except to those 
 * having a need to know them without prior written consent of the owner.
*/
/* ===================================================================== */

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
#include "testftinitsequencing/TestHello_impl.hpp"
#include "testftinitsequencing/TestInterceptors.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
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
    MyProcessBehaviour  (CORBA::ORB_ptr orb,
                         PortableServer::POA_ptr rootPOA, 
                         std::string the_host,
                         Cdmw::TestServerInterceptor* server_interceptor_impl)
            : m_hostname(the_host)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_rootPOA =  PortableServer::POA::_duplicate(rootPOA);
        m_server_interceptor_impl = server_interceptor_impl;        
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
        m_applicationname =  Cdmw::PlatformMngt::PlatformInterface::Get_application_name();
        
        // get process name
        m_processname = Cdmw::PlatformMngt::PlatformInterface::Get_process_name();

        ::FT::Location loc;
        loc.length(3);
        loc[0].id = CORBA::string_dup(m_hostname.c_str());
        loc[0].kind = "hostname";
        loc[1].id = CORBA::string_dup(m_applicationname.c_str());
        loc[1].kind = "applicationname";
        loc[2].id = CORBA::string_dup(m_processname.c_str());
        loc[2].kind = "processname";

        m_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc);
        

        CdmwNamingAndRepository::Repository_var repository
            = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();

        // ======================================================
        // Get HELLO name domain from repository
        // ======================================================
        CdmwNamingAndRepository::NameDomain_var helloDomain =
            repository->resolve_name_domain ("dom1/dom2/dom3");
        std::string full_name = "P31HelloGroup";
        std::string time_collector_name = "TimeCollector";

        // Get the Replication Manager
        CORBA::Object_var objref = m_orb->resolve_initial_references("ReplicationManager");
        m_rm = CdmwFT::ReplicationManager::_narrow(objref.in());

        std::cout << m_name << " test_server: create HelloInterface_impl"<<std::endl;
        // create object hellointerface
        Cdmw::HelloInterface_impl * helloInterface_impl
            = new ::Cdmw::HelloInterface_impl (
                m_orb.in(), m_rootPOA.in(), m_name.c_str());
        m_helloInterface
            =  helloInterface_impl->_this();

        Cdmw::CommonSvcs::Naming::NamingInterface ni =
            Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2/dom3");

        // ========================================================
        // Add the HelloInterface object to the Object Group
        // ========================================================
        
        try
        {
            m_obj = ni.resolve (full_name);
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout << e << std::endl;
            abort();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout << e << std::endl;
            abort();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout << e << std::endl;
            abort();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout << e << std::endl;
            abort();
        }

        // ========================================================
        // Add the HelloInterface object to the Object Group
        // ========================================================
        
        
        try
        {
            CORBA::Object_var obj = ni.resolve (time_collector_name);
            CdmwReplicationManager::time_collector_var time_collector = CdmwReplicationManager::time_collector::_narrow(obj.in());
            if (!CORBA::is_nil(time_collector.in()))
                {
                    std::cout<<"the time_collector is not nil"<<std::endl;
                    m_server_interceptor_impl->init(time_collector.in());
                }
                
        }
        catch(const CosNaming::NamingContext::NotFound& e)
        {
            std::cout << e << std::endl;
            abort();
        }
        catch(const CosNaming::NamingContext::CannotProceed& e)
        {
            std::cout << e << std::endl;
            abort();
        }
        catch(const CosNaming::NamingContext::InvalidName& e)
        {
            std::cout << e << std::endl;
            abort();
        }
        catch (const CORBA::SystemException& e)
        {
            std::cout << e << std::endl;
            abort();
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
        try
        {
            std::cout<<"**** before  add_member"<<std::endl;
            ::FT::Location loc;
            loc.length(3);
            loc[0].id = CORBA::string_dup(m_hostname.c_str());
            loc[0].kind = "hostname";
            loc[1].id = CORBA::string_dup(m_applicationname.c_str());
            loc[1].kind = "applicationname";
            loc[2].id = CORBA::string_dup(m_processname.c_str());
            loc[2].kind = "processname";
            m_obj = m_rm->add_member(m_obj.in(),
                                   loc,
                                   m_helloInterface.in());
            std::cout<<"**** after  add_member"<<std::endl;
        }
        catch( CORBA::Exception& e )
        {
            std::cerr << e._name() << std::endl;
            std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                      << " IN FILE: " << __FILE__ << std::endl; 
            abort();
        }
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
    std::string m_applicationname;
    std::string m_processname;
    CdmwReplicationManager::HelloInterface_var m_helloInterface;
    Cdmw::TestServerInterceptor* m_server_interceptor_impl;

    bool m_creator;
    CdmwFT::ReplicationManager_var m_rm;
    CORBA::Object_var m_obj;
};


#endif // INCL_MY_PROCESS_BEHAVIOUR_HPP

