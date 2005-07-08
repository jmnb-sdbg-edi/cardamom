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

#include <iostream>
#include <sstream>

#include <Foundation/common/System.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <Repository/naminginterface/NamingUtil.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>

#include <CCMContainer/idllib/CdmwCcmComponentServer.stub.hpp>
#include <CCMContainer/ccmcontainer/ConfigValue_impl.hpp>


#include "MyComponentInstallation.hpp"

#include "Hello_cif.stub.hpp"

// TAO Specific Load balancing
#include <orbsvcs/orbsvcs/CosLoadBalancingC.h>
#include <orbsvcs/orbsvcs/PortableGroupC.h>


namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;    

    const char* SPLICE_PORT_OPTION="-Splice-port=";
    const char* LIBRARY_DIRECTORY_OPTION="--lib_dir";


    static void
    usage(std::ostream & os, const std::string & program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                    Display this information.\n"
           << LIBRARY_DIRECTORY_OPTION << "=<directory>    Directory containing user implementation dynamic library.\n"
           << std:: endl;
    }
    
    CosLoadBalancing::LoadManager_ptr
    get_load_manager(CORBA::ORB_ptr orb)
        throw(CORBA::TRANSIENT)
    {
        CDMW_ASSERT(!CORBA::is_nil(orb));
        
        CORBA::Object_var obj = CORBA::Object::_nil();
        // Get reference to the repository interface
        CosLoadBalancing::LoadManager_var lm = CosLoadBalancing::LoadManager::_nil();
        
        try {
            // Get the Load Manager
            obj = orb->resolve_initial_references("LoadManager");
            
            lm = CosLoadBalancing::LoadManager::_narrow(obj.in() );
        } catch (const CORBA::Exception & ex) {
            std::cerr << "Couldn't retrieve reference of the LoadMonitor"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ") :" 
                      << ex << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                                   CORBA::COMPLETED_NO);
        } catch (...) {
            std::cerr << "Couldn't retrieve reference of the LoadMonitor"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                                   CORBA::COMPLETED_NO);
        }
        return lm._retn();
    }
    
    Components::ConfigValues*
    create_Homes_ConfigValues(const char * repid,
                              const char * classname,
                              CdmwDeployment::ServantLifetimeValue servant_lifetime,
                              CdmwDeployment::ThreadingPolicyValue threading_policy,
                              CdmwDeployment::ValuetypeFactoryDescriptionSeq* factory_seq = 0)
        throw ()
    {
        CORBA::ULong nb_values = 5L;
        if (factory_seq)
            nb_values = 6L;
        Components::ConfigValues_var cfg = new Components::ConfigValues(nb_values);
        cfg->length(nb_values); 
        CORBA::ULong idx = 0L;
        using namespace Cdmw::CCM::Container;
        // COMPONENT_KIND
        {
            CORBA::Any value;
            value <<= CdmwDeployment::SESSION;
            cfg[idx++] = new ConfigValue_impl(CdmwDeployment::COMPONENT_KIND,value);
        }
        // HOME_REPOSITORY_ID
        {
            CORBA::Any value;
            value <<= repid;
            cfg[idx++] = new ConfigValue_impl(CdmwDeployment::HOME_REPOSITORY_ID,value);
        }
        // HOME_SERVANT_CLASSNAME
        {
            CORBA::Any value;
            value <<= classname;
            cfg[idx++] = new ConfigValue_impl(CdmwDeployment::HOME_SERVANT_CLASSNAME,value);
        }
        // SERVANT_LIFETIME
        {
            CORBA::Any value;
            value <<= servant_lifetime;
            cfg[idx++] = new ConfigValue_impl(CdmwDeployment::SERVANT_LIFETIME,value);
        }
        // THREADING_POLICY
        {
            CORBA::Any value;
            value <<= threading_policy;
            cfg[idx++] = new ConfigValue_impl(CdmwDeployment::THREADING_POLICY,value);
        }
        // VALUETYPE_FACTORY_DEPENDENCIES </TD><TD>sequence< ValuetypeFactoryDescription ></TD>
        {
            if (factory_seq) {
                CORBA::Any value;
                value <<= *factory_seq;
                cfg[idx++] = new ConfigValue_impl(CdmwDeployment::VALUETYPE_FACTORY_DEPENDENCIES,value);
            }
        }
        
        return cfg._retn();
    }
    
    class OrbEventLoopThread : public Cdmw::OsSupport::Thread
    {
    public:
        
        OrbEventLoopThread(CORBA::ORB_ptr orb) throw (CORBA::SystemException)
            : m_orb(CORBA::ORB::_duplicate(orb))
        {
        }
        OrbEventLoopThread() throw (CORBA::SystemException)
            : m_orb(CORBA::ORB::_nil())
        {
        }

        ~OrbEventLoopThread() throw()
        {
        };


        void shutdown(bool wait_for_completion)
            throw (CORBA::SystemException)
        {
            if (!CORBA::is_nil(m_orb.in()))
                m_orb->shutdown( wait_for_completion );
        }


        CORBA::ORB_ptr orb()
        {
            return CORBA::ORB::_duplicate( m_orb.in() );
        }
        void orb(CORBA::ORB_ptr the_orb)
        {
            m_orb = CORBA::ORB::_duplicate( the_orb );
        }

    protected:

        void run() throw()
        {
            if (!CORBA::is_nil(m_orb.in()))
                m_orb->run();
        }

    private:

        CORBA::ORB_var  m_orb;
    };

    CORBA::Object_ptr 
    add_member(CosLoadBalancing::LoadManager_ptr lm,
               CORBA::Object_ptr group,
               const char* processname,
               CORBA::Object_ptr member)
        throw (CORBA::Exception)
    {

      PortableGroup::Location location (1);
      location.length (1);

      location[0].id = CORBA::string_dup (processname);
      location[0].kind = CORBA::string_dup ("processname");
      
      CORBA::Object_var obj_group;
      try {
          obj_group = lm->add_member(group, location, member);
      } catch (const CORBA::SystemException & ex) {
          std::cerr << __FILE__ << "  " << __LINE__ << "\n"
                    << ex << std::endl;
          throw;
      } catch (const CORBA::Exception & ex) {
          std::cerr << __FILE__ << "  " << __LINE__ << "\n"
                    << ex << std::endl;
          throw CORBA::INTERNAL();
      }
      return obj_group._retn();
    }
    
    Hello::Server_ptr
    create_configured_server_component
    (const char * repid,
     const char * classname,
     const char * uuid,
     const char * processname,
     const char * entrypoint,
     const char * componentname,
     const char * bindingname,
     Cdmw::NamingAndRepository::NamingInterface & ni,
     CdmwDeployment::ComponentInstallation_ptr component_installation)
    {
        std::cout << "[A] Create the component server"  << std::endl;
        std::cout << "   - Get '"  << processname << "' component server from \n"
                  << "     the CARDAMOM NamingAndRepository service" << std::endl;
        std::ostringstream ostr;
        ostr << "CDMW/SERVICES/ASSEMBLYANDDEPLOYMENT/"
             << processname
             << "@LBApplication.ComponentServer";
        
        const std::string process_location = ostr.str();
        CdmwCcmComponentServer::ComponentServer_var process =
            Cdmw::NamingAndRepository::NamingUtil<CdmwCcmComponentServer::ComponentServer>::resolve_name (ni,process_location);            
        process->set_component_installation(component_installation);
        std::cout << "[B] Create the container on the component server" << std::endl;        
        // Create a set of ConfigValues.
        Components::ConfigValues_var cfg_compserver = new Components::ConfigValues(1L);
        cfg_compserver->length(1L);
        // COMPONENT_KIND
        {
            CORBA::Any value;
            value <<= CdmwDeployment::SESSION;
            cfg_compserver[0L] =
                new Cdmw::CCM::Container::ConfigValue_impl(CdmwDeployment::COMPONENT_KIND,value);
        }    
        std::cout << "   - Create the container" << std::endl;
        Components::Deployment::Container_var container =
            process->create_container(cfg_compserver.in());
        
        // **************************************************************************
        // [4] Create the component homes
        // **************************************************************************
        std::cout << "[C] Create the component home" << std::endl;
        Components::ConfigValues_var home_cfg =
            create_Homes_ConfigValues(repid,
                                      classname,
                                      CdmwDeployment::CONTAINER_LIFETIME,
                                      CdmwDeployment::MULTITHREAD);
        std::cout << "   - Installing the Home on the container" << std::endl;
        Components::CCMHome_var a_home = container->install_home(uuid,
                                                                 entrypoint,
                                                                 home_cfg.in());
        
        Hello::ServerHome_var home = Hello::ServerHome::_narrow(a_home.in());
		  
        // **************************************************************************
        // [5] Create the components
        // **************************************************************************
        std::cout << "[D] Create the component" << std::endl;
        // - Create one instance of a server component
        std::cout << "   - Creating one instance of the component" << std::endl;
        Hello::Server_var server = home->create();
        // **************************************************************************
        // [6] Assemble and configure the components
        // **************************************************************************
        std::cout << "[E] Assemble and configure the component" << std::endl;
        std::cout << "   - Configuring '" << componentname << "' ...";
        // - Configure the component instance
        server->name(componentname);
        // Register the component on the name server as "MyNaming/Tutorial/HelloServer1"
        ni.bind(bindingname,server.in(),true);
        server->configuration_complete();
        std::cout << "done." << std::endl;
        
        return server._retn();
        
    }

        
}; // End anonymous namespace



int main(int argc, char* argv[])
{    
    using namespace Cdmw::OsSupport;
    int status = SUCCESS;

    CORBA::ORB_var orb;    
    if(argc == 1) {
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    //
    // Execute command
    //
    if(strcmp(argv[1], "--help") == 0) {
        usage(std::cout, argv[0]);
        return SUCCESS;
    }

	 // Get Repository host
    std::string lib_dir;
    try {
        lib_dir = OS::get_option_value(argc, argv, 
                                       LIBRARY_DIRECTORY_OPTION);
    } catch (const Cdmw::OutOfMemoryException & ) {
		std::cerr << "Not enought memory to get option value!" << std::endl;
        return FAILURE;
    } catch (const Cdmw::InternalErrorException & ) {
        usage(std::cout,argv[0]);
        return FAILURE;
    }
    
    if ((lib_dir == "no") ||
        (lib_dir == "yes")) {
        usage(std::cout,argv[0]);
        return FAILURE;
    }
    
    
    std::cout << "************************************************************\n"
              << "===> Manual Assembly Program for the CARDAMOM demo CCM 320    *\n"
              << "************************************************************" 
              << std::endl;
    
    OrbEventLoopThread orb_thread;
    try 
    {
        // ===================================================
        // Initialize the ORB
        // ===================================================
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
        
        // ===================================================
        // Get the root POA 
        // ===================================================
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

        // ===================================================
        // Activate the root POA manager
        // ===================================================
        PortableServer::POAManager_var poaManager = rootPOA -> the_POAManager();
        poaManager -> activate();

        // ===================================================
        // run the ORB event loop in a separate thread
        // ===================================================
        orb_thread.orb(orb.in());
        orb_thread.start();        

        // init Cardamom
        Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv );
        
        // ===================================================
        // Get the process name and application name
        // ===================================================
		
        // **************************************************************************
        // [0] Get the CARDAMOM NamingAndRepository object reference
        // **************************************************************************
        using namespace Cdmw::NamingAndRepository;
        CdmwNamingAndRepository::Repository_var repository =
            RepositoryInterface::get_repository(); 
        
        CosNaming::NamingContext_var root =
            repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
        NamingInterface ni(root.in());
        // **************************************************************************
        // [1] Create the component installation object
        // **************************************************************************        
        std::cout << "===> Create the component installation object" << std::endl;
        std::cout << "[1] Create the component installation object" << std::endl;
        // Create a map containing <UUID, path to the component shared library path> pairs
        std::map<std::string,std::string> comp_implementations;// A map of <uuid,implementation>
        
        const char* server_UUID = "DCE:700dc518-0110-11ce-ac8f-0800090b5d3e";
        std::string server_library = lib_dir + "/libServerHello.so";
        comp_implementations[server_UUID] = server_library.c_str();
        std::map<std::string,std::string> valuefactory_locations;// A map of <repid,location>
        //		const char* rep_id = "IDL:acme.com/Hello/StatusInfo:1.0";
        //		std::string valuetype_location = lib_dir + "/libServerHello.so";
        //		valuefactory_locations[rep_id] =  valuetype_location.c_str();
        
        TestCardamomCCM::MyComponentInstallation * comp_install =
            new TestCardamomCCM::MyComponentInstallation(rootPOA.in(),
                                                      comp_implementations,
                                                      valuefactory_locations);
        PortableServer::ServantBase_var servant = comp_install;
        CdmwDeployment::ComponentInstallation_var component_installation
            = comp_install->_this();

        // **************************************************************************
        // [2] Create and configure the Server components
        // **************************************************************************
        std::cout << "[2] Create and configure the Server components" << std::endl;
        Hello::Server_var server1 = create_configured_server_component 
            ("IDL:acme.com/Hello/ServerHome:1.0",
             "Cdmw.CCM.CIF.CdmwHello.SessionServerHome_impl",
             server_UUID,
             "ServerProcess1",
             "create_ServerHomeExecutor",
             "ServerNo1",
             "MyNaming/Tutorial/HelloServer1",
             ni,
             component_installation.in());
        Hello::Server_var server2 = create_configured_server_component 
            ("IDL:acme.com/Hello/ServerHome:1.0",
             "Cdmw.CCM.CIF.CdmwHello.SessionServerHome_impl",
             server_UUID,
             "ServerProcess2",
             "create_ServerHomeExecutor",
             "ServerNo2",
             "MyNaming/Tutorial/HelloServer2",
             ni,
             component_installation.in());
        Hello::Server_var server3 = create_configured_server_component 
            ("IDL:acme.com/Hello/ServerHome:1.0",
             "Cdmw.CCM.CIF.CdmwHello.SessionServerHome_impl",
             server_UUID,
             "ServerProcess3",
             "create_ServerHomeExecutor",
             "ServerNo3",
             "MyNaming/Tutorial/HelloServer3",
             ni,
             component_installation.in());
        

        // **************************************************************************
        // [3] Create the components
        // **************************************************************************
    
        
        std::cout << "   - Add server components and component facets to LB group references" 
                  << std::endl;
        // Get the Component's IOGR
        CORBA::Object_var component_iogr =
              ni.resolve("demo_ccm_lb/hello_servers/hello_server.group");
        // Get the Facet's IOGR
        CORBA::Object_var facet_iogr =
              ni.resolve("demo_ccm_lb/hello_servers/hello_server_facet.group");
        
        // Get the TAO LoadManager's IOR
        CosLoadBalancing::LoadManager_var load_manager = get_load_manager(orb.in());
        
        Hello::Display_var facet1 = server1->provide_server_facet();
        Hello::Display_var facet2 = server2->provide_server_facet();
        Hello::Display_var facet3 = server3->provide_server_facet();
        
        
        component_iogr = add_member(load_manager.in(),
                                    component_iogr.in(),
                                    "ServerProcess1",
                                    server1.in());
        component_iogr = add_member(load_manager.in(),
                                    component_iogr.in(),
                                    "ServerProcess2",
                                    server2.in());
        component_iogr = add_member(load_manager.in(),
                                    component_iogr.in(),
                                    "ServerProcess3",
                                    server3.in());
        // We do the same with facets
        
        facet_iogr = add_member(load_manager.in(),
                                facet_iogr.in(),
                                "ServerProcess1",
                                facet1.in());
        facet_iogr = add_member(load_manager.in(),
                                facet_iogr.in(),
                                "ServerProcess2",
                                facet2.in());
        facet_iogr = add_member(load_manager.in(),
                                facet_iogr.in(),
                                "ServerProcess3",
                                facet3.in());
        try {

            CORBA::String_var ior =
                orb->object_to_string (facet_iogr.in());
            std::cout << "Facet's LB Iogr :\n"
                      << ior.in() << std::endl;
        } catch (const CORBA::Exception & ex) {
            std::cerr << __FILE__ << "  " << __LINE__ << "\n"
                      << ex << std::endl;
        }
        std::cout << "done." << std::endl;
        // ===================================================
        // wait for CORBA message
        // ===================================================
//        orb->run();
    } 
    catch (const Cdmw::Exception & ex) 
    {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    } 
    catch(const CORBA::Exception& ex)  
    {        
        std::cerr << ex << std::endl;
        status = FAILURE;
    }

    std::cout << "===> Exiting Manual Assembly Program" << std::endl;
    // ========================================================
    // program stopping
    // ========================================================
    orb_thread.shutdown(true);
    orb_thread.join();
    // ===================================================
    // Call generated Cardamom cleanup
    // ===================================================
    Cdmw::CdmwInit::CDMW_cleanup(orb.in());
            
    // ===================================================
    // Call ORB cleanup
    // ===================================================
    Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
       
    // =====================================================
    // Destroy orb
    // =====================================================
    if(!CORBA::is_nil(orb.in()))
    {
        try 
        {
            orb -> destroy();
        }
        catch(const CORBA::Exception& ex)
        {           
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        

    return status;    
}

