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

#include <Foundation/common/System.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/common/Assert.hpp>

#include <ConfAndPlug/cdmwinit/InitUtils.hpp>
#include <Foundation/commonsvcs/naming/NamingUtil.hpp>

#include <CCMContainer/idllib/CdmwCcmComponentServer.stub.hpp>
#include <CCMContainer/ccmcontainer/ConfigValue_impl.hpp>
#include <CCMContainer/ccmcommon/Cookie_impl.hpp>


#include "Hello_cif.stub.hpp"

#include "MyComponentInstallation.hpp"

namespace 
{
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;    

	 const char* REPOSITORY_PORT_OPTION="--rep_port";
	 const char* REPOSITORY_HOST_OPTION="--rep_host";
	 const char* LIBRARY_DIRECTORY_OPTION="--lib_dir";

    static void
    usage(std::ostream & os, const std::string & program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                    Display this information.\n"
           << REPOSITORY_PORT_OPTION << "=<port_number>  Port number of the CARDAMOM Naming and Repository service.\n"
           << "[" << REPOSITORY_HOST_OPTION << "=<host_name>]  Host where the CARDAMOM Naming and Repository is running. Default value is localhost\n"
           << LIBRARY_DIRECTORY_OPTION << "=<directory>    Directory containing user implementation dynamic library.\n"
           << std:: endl;
    }

    CosNaming::NamingContext_ptr
    get_naming_service(CORBA::ORB_ptr orb, const std::string & url)
        throw(CORBA::TRANSIENT)
    {
        CDMW_ASSERT(!CORBA::is_nil(orb));
    
        CORBA::Object_var obj = CORBA::Object::_nil();
        //
        // Get naming service
        //
        try {
            obj = orb -> string_to_object(url.c_str());
        } catch(...) {
            std::cerr << "Invalid Naming URL" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                                   CORBA::COMPLETED_NO);
        }
        
        if(CORBA::is_nil(obj.in())) {
            std::cerr << "URL denotes a nil object reference" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                                   CORBA::COMPLETED_NO);
        }
        
        // Get reference to the repository interface
        CosNaming::NamingContext_var naming = CosNaming::NamingContext::_nil();
        try {
            naming =  CosNaming::NamingContext::_narrow(obj.in());
            if (CORBA::is_nil(naming.in())) {
                std::cerr << "Couldn't retrieve reference of the Name Service"
                          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
                throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                                       CORBA::COMPLETED_NO);
            }
        } catch (...) {
            std::cerr << "Couldn't retrieve reference of the Name Service"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachNameService,
                                   CORBA::COMPLETED_NO);
        }
        return naming._retn();
    }

    CdmwNamingAndRepository::Repository_ptr
    get_cdmw_repository(CORBA::ORB_ptr orb, const std::string & url)
        throw(CORBA::TRANSIENT)
    {
        CDMW_ASSERT(!CORBA::is_nil(orb));
    
        CORBA::Object_var obj = CORBA::Object::_nil();
        //
        // Get naming service
        //
        try {
            obj = orb -> string_to_object(url.c_str());
        } catch(...) {
            std::cerr << "Invalid URL" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }
        
        if(CORBA::is_nil(obj.in())) {
            std::cerr << "URL denotes a nil object reference" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }
        
        // Get reference to the repository interface
        CdmwNamingAndRepository::Repository_var rep
            = CdmwNamingAndRepository::Repository::_nil();
        try {
            rep =  CdmwNamingAndRepository::Repository::_narrow(obj.in());
            if (CORBA::is_nil(rep.in())) {
                std::cerr << "Couldn't retrieve reference of the Repository"
                          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
                throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                       CORBA::COMPLETED_NO);
            }
        } catch (...) {
            std::cerr << "Couldn't retrieve reference of the Repository"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }
        return rep._retn();
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
			  if (factory_seq)
			  {
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

    std::string nar_url;    // CARDAMOM Naming and Repository URL
    std::string naming_url; // Naming service URL
    //
    // Execute command
    //
    if(strcmp(argv[1], "--help") == 0) {
        usage(std::cout, argv[0]);
        return SUCCESS;
    }

	 // Get Repository port
    std::string rep_port;
    try 
	 {
        rep_port = OS::get_option_value(argc, argv, 
                                        REPOSITORY_PORT_OPTION);
    } catch (const Cdmw::OutOfMemoryException & ) 
	 {
		  std::cerr << "Not enought memory to get option value!" << std::endl;
        return FAILURE;
    } catch (const Cdmw::InternalErrorException & ) 
	 {
        usage(std::cout,argv[0]);
        return FAILURE;
    }
    
    if ((rep_port == "no") ||
        (rep_port == "yes")) 
	 {
           usage(std::cout,argv[0]);
           return FAILURE;
    }

	 // Get Repository host
    std::string rep_host;
    try 
	 {
        rep_host = OS::get_option_value(argc, argv, 
                                        REPOSITORY_HOST_OPTION);
    } catch (const Cdmw::OutOfMemoryException & ) 
	 {
		  std::cerr << "Not enought memory to get option value!" << std::endl;
        return FAILURE;
    } catch (const Cdmw::InternalErrorException & ) 
	 {
        usage(std::cout,argv[0]);
        return FAILURE;
    }
    
    if ((rep_host == "no") ||
        (rep_host == "yes")) 
	 {
	   nar_url="corbaloc::localhost:" + rep_port + "/CdmwRepository";
	   naming_url = "corbaloc::localhost:" + rep_port + "/NameService";
	 }
    else
         {
	   nar_url="corbaloc::" + rep_host + ":" + rep_port + "/CdmwRepository";
	   naming_url = "corbaloc::" + rep_host + ":" + rep_port + "/NameService";
	 }

	 // Get Repository host
    std::string lib_dir;
    try 
	 {
        lib_dir = OS::get_option_value(argc, argv, 
                                       LIBRARY_DIRECTORY_OPTION);
    } catch (const Cdmw::OutOfMemoryException & ) 
	 {
		  std::cerr << "Not enought memory to get option value!" << std::endl;
        return FAILURE;
    } catch (const Cdmw::InternalErrorException & ) 
	 {
        usage(std::cout,argv[0]);
        return FAILURE;
    }
    
    if ((lib_dir == "no") ||
        (lib_dir == "yes")) 
	 {
        usage(std::cout,argv[0]);
        return FAILURE;
	 }

    std::cout << "************************************************************" << std::endl;
    std::cout << "===> Manual Assembly Program for the Cdmw.Tutorial.ccm310 *" << std::endl;
    std::cout << "************************************************************" << std::endl;
    
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

		  // ===================================================
        // Register ValueType factory
		  // ===================================================
#if CDMW_ORB_VDR == tao
        CORBA::ValueFactoryBase_var vf, old_vf;
#else
        CORBA::ValueFactoryBase_var vf, old_vf;
#endif

		  vf = new Cdmw::CCM::Common::CookieFactory();
        old_vf = orb->register_value_factory ("IDL:omg.org/Components/Cookie:1.0", vf.in());
        
        // ===================================================
        // Get the process name and application name
        // ===================================================

        // **************************************************************************
        // [0] Get the CARDAMOM NamingAndRepository object reference
        // **************************************************************************
        CdmwNamingAndRepository::Repository_var repository =
            get_cdmw_repository(orb.in(),nar_url);
        
        CosNaming::NamingContext_var root =
            repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
        using namespace CdmwNamingAndRepository;
       Cdmw::CommonSvcs::Naming::NamingInterface ni(root.in());
        // **************************************************************************
        // [1] Create the component installation object
        // **************************************************************************        
        std::cout << "===> Create the component installation object" << std::endl;
        std::cout << "[1] Create the component installation object" << std::endl;
        // Create a map containing <UUID, path to the component shared library path> pairs
        std::map<std::string,std::string> comp_implementations;// A map of <uuid,implementation>
        
        const char* server_UUID = "DCE:700dc518-0110-11ce-ac8f-0800090b5d3e";
		  std::string server_library = lib_dir + "/libServerHello.so";
        const char* client_UUID = "DCE:700dc518-0110-11ce-ac8f-0800090531ac";
		  std::string client_library = lib_dir + "/libClientHello.so";
        comp_implementations[server_UUID] = server_library.c_str();
        comp_implementations[client_UUID] = client_library.c_str();
        std::map<std::string,std::string> valuefactory_locations;// A map of <repid,location>
		  const char* rep_id = "IDL:acme.com/Hello/StatusInfo:1.0";
		  std::string valuetype_location = lib_dir + "/libServerHello.so";
		  valuefactory_locations[rep_id] =  valuetype_location.c_str();
		 
        TestCardamomCCM::MyComponentInstallation * comp_install =
            new TestCardamomCCM::MyComponentInstallation(rootPOA.in(),
                                                      comp_implementations,
                                                      valuefactory_locations);
        PortableServer::ServantBase_var servant = comp_install;
        CdmwDeployment::ComponentInstallation_var component_installation
            = comp_install->_this();
        // **************************************************************************
        // [2] Create the component servers
        //    - Get the first component server 'ClientProcess' from
        //      the CARDAMOM NamingAndRepository service
        // **************************************************************************
        std::cout << "[2] Create the component servers" << std::endl;
        std::cout << "   - Get the first component server 'ClientProcess' from \n"
                  << "     the CARDAMOM NamingAndRepository service" << std::endl;
        const std::string client_process_location =
            "CDMW/SERVICES/ASSEMBLYANDDEPLOYMENT/ClientProcess@HelloApplication.ComponentServer";
        CdmwCcmComponentServer::ComponentServer_var client_process = 
	  Cdmw::CommonSvcs::Naming::NamingUtil<CdmwCcmComponentServer::ComponentServer>::resolve_name
            (ni,client_process_location);            
        client_process->set_component_installation(component_installation.in());
            
        //    - Get the second component server 'ServerProcess' from
        //      the CARDAMOM NamingAndRepository service
        std::cout << "   - Get the second component server 'ServerProcess' from \n"
                  << "     the CARDAMOM NamingAndRepository service" << std::endl;
        const std::string server_process_location =
            "CDMW/SERVICES/ASSEMBLYANDDEPLOYMENT/ServerProcess@HelloApplication.ComponentServer";
        CdmwCcmComponentServer::ComponentServer_var server_process =
            Cdmw::CommonSvcs::Naming::NamingUtil<CdmwCcmComponentServer::ComponentServer>::resolve_name
            (ni,server_process_location);
        server_process->set_component_installation(component_installation.in());

        // **************************************************************************
        // [3] Create the containers on the component servers
        // **************************************************************************
        std::cout << "[3] Create the containers on the component servers" << std::endl;        
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
        std::cout << "   - Create the client container" << std::endl;
        Components::Deployment::Container_var client_container =
            client_process->create_container(cfg_compserver.in());
        std::cout << "   - Create the server container" << std::endl;
        Components::Deployment::Container_var server_container =
            server_process->create_container(cfg_compserver.in());
        
        // **************************************************************************
        // [4] Create the component homes
        // **************************************************************************
        std::cout << "[4] Create the component homes" << std::endl;
        Components::ConfigValues_var home_cfg =
            create_Homes_ConfigValues("IDL:acme.com/Hello/ClientHome:1.0",
                                      "Cdmw.CCM.CIF.CdmwHello.SessionClientHome_impl",
                                      CdmwDeployment::CONTAINER_LIFETIME,
                                      CdmwDeployment::MULTITHREAD);
        std::cout << "   - Installing ClientHome on the client container" << std::endl;
        Components::CCMHome_var a_home =
            client_container->install_home(client_UUID,
                                           "create_ClientHomeExecutor",
                                           home_cfg.in());
        
        Hello::ClientHome_var client_home = Hello::ClientHome::_narrow(a_home.in());
		  
        std::cout << "   - Installing ServerHome on the server container" << std::endl;
		  CdmwDeployment::ValuetypeFactoryDescriptionSeq seq(1L);
        seq.length(1L);
		  
		  CdmwDeployment::ValuetypeFactoryDescription desc;
        desc.repid = "IDL:acme.com/Hello/StatusInfo:1.0";
        desc.factory_entrypoint = "createStatusInfoFactory";
        seq[0]=desc;

        home_cfg = create_Homes_ConfigValues("IDL:acme.com/Hello/ServerHome:1.0",
                                             "Cdmw.CCM.CIF.CdmwHello.SessionServerHome_impl",
                                             CdmwDeployment::CONTAINER_LIFETIME,
                                             CdmwDeployment::MULTITHREAD,
															&seq);
        a_home = server_container->install_home(server_UUID,
                                                "create_ServerHomeExecutor",
                                                home_cfg.in());
        Hello::ServerHome_var server_home = Hello::ServerHome::_narrow(a_home.in());
        // **************************************************************************
        // [5] Create the components
        // **************************************************************************
        std::cout << "[5] Create the components" << std::endl;
        // - Create two instances of server components
        std::cout << "   - Creating 1st instance of server component" << std::endl;
        Hello::Server_var server1 = server_home->create();
        std::cout << "   - Creating 2nd instance of server component" << std::endl;
        Hello::Server_var server2 = server_home->create();
        // **************************************************************************
        // [6] Assemble and configure the components
        // **************************************************************************
        std::cout << "[6] Assemble and configure the components" << std::endl;
        // Get the Name Service object reference
        CosNaming::NamingContext_var naming_ctxt =
            get_naming_service(orb.in(),naming_url);
        Cdmw::CommonSvcs::Naming::NamingInterface naming_ni(naming_ctxt.in());
        std::cout << "   - Configuring ServerNo1 ...";
        // - Configure the two instances
        server1->name("ServerNo1");
        // Register the component on the name server as "MyNaming/Tutorial/HelloServer1"
        naming_ni.bind("MyNaming/Tutorial/HelloServer1",server1.in(),true);
        server1->configuration_complete();
        std::cout << "done." << std::endl;
        std::cout << "   - Configuring ServerNo2 ...";        
        server2->name("ServerNo2");
        // Register the component on the name server as "MyNaming/Tutorial/HelloServer2"
        naming_ni.bind("MyNaming/Tutorial/HelloServer2",server2.in(),true);
        server2->configuration_complete();
        std::cout << "done." << std::endl;
        // - Create one instance of the client component
        std::cout << "   - Creating one instance of the client component" << std::endl;
        Hello::Client_var client = client_home->create();
        std::cout << "   - Configuring HelloClient ...";   
        Hello::Display_var server_facet = server1->provide_server_facet();
        client->connect_client_receptacle(server_facet.in());
        client->connect_to_naming(naming_ctxt.in());
		  
		  Hello::StatusInfoConsumer_var consumer1 = server1->get_consumer_info();
		  Hello::StatusInfoConsumer_var consumer2 = server2->get_consumer_info();
		  Components::Cookie_var cookie1 = client->subscribe_info (consumer1.in());
		  Components::Cookie_var cookie2 = client->subscribe_info (consumer2.in());

        // Register the component on the name server as "MyNaming/Tutorial/HelloClient"
        naming_ni.bind("MyNaming/Tutorial/HelloClient",client.in(),true);
        client->configuration_complete();
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
//    Cdmw::CdmwInit::CDMW_cleanup(orb.in());
            
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

