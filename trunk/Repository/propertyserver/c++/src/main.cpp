/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


// Standard include files
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>


// Cdmw include files
#include <Foundation/common/System.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/ThreadHandle.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/tools/RepositoryAdmin.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>
#include <SystemMngt/platforminterface/ServiceNames.hpp>
#include <Foundation/testutils/Testable.hpp>

#if CDMW_ORB_VDR == orbacus	    
#include <Foundation/idllib/CosProperty.stub.hpp>
#elif CDMW_ORB_VDR == tao
#include <orbsvcs/Property/CosPropertyService_i.h>
#endif

namespace
{
    const int SUCCESS = 0;
    const int FAILURE = 1;  
    
    const char* PROP_SERVICE_ID               = "PropertyService";
    const std::string REPOSITORY_PORT_OPTION  = "--RepPort";
    const std::string PROPERTY_PORT_OPTION    = "--PropPort";
    const std::string PROPERTY_SET_OPTION     = "--CreatePropertySet";
    const int POA_THREAD_POOL_SIZE = 5;
    
    static void
        usage(std::ostream & os, const std::string &program_name)
    {
        os << "Usage:\n"
            << program_name << " options\n"
            << "\n"
            << "Options:\n"
            << "--help                     Display this information.\n"
            << "\n"
            << "[--RepPort=<nbr>]          Port of Name and Repository.\n"
            << "[--PropPort=<nbr>]         Port of Property Service.\n"
            << "[--CreatePropertySet]      Create a PropertySet for LifeCycle test usage"
            << std:: endl;
    }

    //-----------------------------------------------------------------------------------
    CosPropertyService::PropertySetDefFactory_ptr
    get_PropertySetDefFactory(CORBA::ORB_ptr orb, const std::string & url)
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
            std::cerr << "Invalid PropertyService URL" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }
        
        if(CORBA::is_nil(obj.in())) {
            std::cerr << "PropertyService URL denotes a nil object reference" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                   CORBA::COMPLETED_NO);
        }
        
        // Get reference to the property service interface
        CosPropertyService::PropertySetDefFactory_var prop
            = CosPropertyService::PropertySetDefFactory::_nil();
        try {
            prop =  CosPropertyService::PropertySetDefFactory::_narrow(obj.in());
            if (CORBA::is_nil(prop.in())) {
                std::cerr << "Couldn't retrieve reference of the PropertyService"
                          << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
                throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENT,
                                       CORBA::COMPLETED_NO);
            }
        } catch (...) {
            std::cerr << "Couldn't retrieve reference of the PropertyService"
                      << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENT,
                                   CORBA::COMPLETED_NO);
        }
        return prop._retn();

    }


    //-----------------------------------------------------------------------------------
    void
    install_PropertySetDef(CORBA::ORB_ptr orb, 
                           CosPropertyService::PropertySetDefFactory_ptr propfac,
                           CdmwNamingAndRepository::Repository_ptr rep)
    {
        // Create a Propertyset
        CosPropertyService::PropertySetDef_var set = propfac->create_propertysetdef();
        
        // Command is "ln ior CDMW/SERVICESPropertySet.object\n x \n");
        std::string command("ln ");
        
        // Get set's ior
        CORBA::String_var str = orb->object_to_string(set.in());
        
        command += str.in();
        
        command += " CDMW/SERVICES/PropertySet.object\n x \n";
        
        Cdmw::Tools::RepositoryAdmin adm(orb, rep);
        
        std::istringstream is(command);
        adm.run(is, std::cout);
    }

    //-----------------------------------------------------------------------------------

    class RunThread : public  Cdmw::OsSupport::Thread 
    {
    public:
        RunThread(CORBA::ORB_ptr orb, 
                  CosPropertyService::PropertySetDefFactory_ptr propsetdef_factory,
                  CdmwNamingAndRepository::Repository_ptr repository_ref) 
            throw (CORBA::SystemException)
            : m_orb(CORBA::ORB::_duplicate(orb)),
              m_propsetdef_factory(CosPropertyService::PropertySetDefFactory::_duplicate(propsetdef_factory)),
              m_repository_ref(CdmwNamingAndRepository::Repository::_duplicate(repository_ref))
        {
        }
        
        ~RunThread() throw()
        {
        }
        
    protected:
        void
            run() throw()
        {
            install_PropertySetDef(m_orb.in(), 
                                   m_propsetdef_factory.in(),
                                   m_repository_ref.in());
        }
        
    private:
        
        RunThread();
        RunThread(const RunThread&);
        RunThread& operator = (const RunThread&);
        
        CORBA::ORB_var     m_orb;
        CosPropertyService::PropertySetDefFactory_var m_propsetdef_factory;
        CdmwNamingAndRepository::Repository_var      m_repository_ref;   
    };

    //-----------------------------------------------------------------------------------

    // Default Process Behaviour
    class MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
    {
        
      public:
        MyProcessBehaviour(CORBA::ORB_ptr orb,
                  CosPropertyService::PropertySetDefFactory_ptr propsetdef_factory,
                  bool create_prop_set = false)
            throw(CORBA::SystemException)
            : m_orb(CORBA::ORB::_duplicate(orb)),
              m_create_prop_set(create_prop_set),
              m_propsetdef_factory(CosPropertyService::PropertySetDefFactory::_duplicate(propsetdef_factory))
        {
        }

         ~MyProcessBehaviour()
            throw()
        {
        };
      
        virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
            throw(CORBA::SystemException)
        {
            std::cout << "INFO: cdmw_property_server init" << std::endl;

            if (m_create_prop_set)
            {
                using namespace Cdmw::PlatformMngt;

                CdmwNamingAndRepository::Repository_var repository_ref =
                    CdmwNamingAndRepository::Repository::_nil();
                
                // get the repository service
                try 
                {
                    CORBA::Object_var nar_obj =
                        PlatformInterface::Get_service(ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
                    
                    repository_ref = CdmwNamingAndRepository::Repository::_narrow(nar_obj.in());
                } 
                catch (const CdmwPlatformMngtService::ServiceNotFound &) 
                {
                    // Cannot Proceed with no CDMW Repository Ior
                    std::cerr << "Couldn't retrieve reference of the Repository : Service not found" << std::endl;
                    throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMRepositoryNotFound,
                        CORBA::COMPLETED_NO);
                } 
                catch (const CdmwPlatformMngtService::ServiceUnavailable &) 
                {
                    // Cannot Proceed with no CDMW Repository Ior
                    std::cerr << "Couldn't retrieve reference of the Repository : Service not available" << std::endl;
                    throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMCannotGetRepositoryReference,
                        CORBA::COMPLETED_NO);
                } 
                catch (const CORBA::SystemException & ex) 
                {
                    std::cerr << "Couldn't retrieve reference of the Repository : System Exception raised"
                        << "\n" << ex
                        << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
                    throw;
                } 
                catch (...)
                {
                    std::cerr << "Couldn't retrieve reference of the Repository"
                        << "\n(file:" << __FILE__ << " line:" << __LINE__ << ")" << std::endl;
                    throw;
                }
                
                RunThread* run_thread = new RunThread(m_orb.in(), m_propsetdef_factory.in(), repository_ref.in());
                run_thread->start();
            }
       }
        
        virtual void run()
            throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, CORBA::SystemException)
        {
            std::cout << "INFO: cdmw_property_server run" << std::endl;

        }
        
        virtual void stop()
            throw(CORBA::SystemException)
        {
            std::cout << "INFO: cdmw_property_server stop" << std::endl;
            m_orb->shutdown(false);
        }
        
      private:
      
        CORBA::ORB_var                                m_orb;
        bool                                          m_create_prop_set;
        CosPropertyService::PropertySetDefFactory_var m_propsetdef_factory;
    };
};  // end anonymous namespace



int main(int argc, char* argv[])
{
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::Common;

    int status = SUCCESS;
    CORBA::ORB_var orb;

#if CDMW_ORB_VDR == orbacus	    
    OS::ProcessId idPropServer;
#endif

    std::string help = OS::get_option_value (argc, argv, "--help");
    if(help == "yes") 
    {
        usage(std::cout, argv[0]);
        return SUCCESS;
    }

    // get Repository port
    std::string repositoryPort = OS::get_option_value (argc, argv, REPOSITORY_PORT_OPTION);
    
    if (repositoryPort  == "no") 
    {
        repositoryPort = "5020";
    } 
    
    // get Property service port
    std::string propServPort = OS::get_option_value (argc, argv, PROPERTY_PORT_OPTION);
    
    if (propServPort  == "no") 
    {
        propServPort = "5015";
    } 

    // get Property service port
    std::string createPropSet = OS::get_option_value (argc, argv, PROPERTY_SET_OPTION);
    

    std::string root_poa_port_option;
    root_poa_port_option 
        = OS::get_option_value(argc, argv, Options::LOCALISATION_SERVICE_OPTION);

    
    // We must ensure to be listening on a host and a port known to the client
    std::string str_opt( "" );
    char** argv_tmp = NULL;
    int argc_tmp = argc;
    
#if CDMW_ORB_VDR == orbacus	 
    if (root_poa_port_option == "no")
    {
        // no localisation port defined
        // Nothing special to do! Just copy argc argv
        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
        
    }
    else
    {
        // a localisation port is defined ! It is becomes the propserv port
        propServPort = root_poa_port_option;
        
        // and we should remove the option from the args!
        argc_tmp = 0;
        for (int i=0; i<argc; i++)
        {
            std::string arg(argv[i]);
            
            // We check if in the current argument we have, the option
            if (arg.find(Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION) 
                == std::string::npos ) 
            {
                // no localisation port, copy argument
                str_opt += std::string( argv[i] ) + " ";
                argc_tmp ++;
            }
        }
    }
    argv_tmp = Cdmw::Common::String::to_char_array(
        Cdmw::Common::String::to_strings( str_opt ) );


#elif CDMW_ORB_VDR == tao	
    if (root_poa_port_option == "no")
    {
        // no localisation port defined
        // add propServPort as LOCALISATION_SERVICE_OPTION
        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";

        str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
        str_opt += "=";
        str_opt += propServPort + " ";
        argc_tmp++;
    }
    else
    {
        // a localisation port is defined ! It is becomes the propserv port
        // Nothing special to do! Just copy argc argv
        propServPort = root_poa_port_option;

        for (int i=0; i<argc; i++)
            str_opt += std::string( argv[i] ) + " ";
    }
    argv_tmp = Cdmw::Common::String::to_char_array(
        Cdmw::Common::String::to_strings( str_opt ) );

#endif

    // Initialize the ORB
    Cdmw::OrbSupport::StrategyList orb_strategies;
    orb_strategies.add_OrbThreaded();
    orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
    
    
    orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc_tmp, argv_tmp, orb_strategies);
    
    CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj.in());
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    
#if CDMW_ORB_VDR == orbacus	    
    // Starting Orbacus Propserver
    std::cout << "Starting Orbacus propserv on port " << propServPort << std::endl;
    idPropServer = OS::create_process("propserv", " -OAport " + propServPort);

    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OS::sleep(timescale*2000);

    std::cout << "Propserv started" << std::endl;
#else  // TAO
    // Create PropertySet factory and then register.
    std::cout << "Create Property Set Def Factory" << std::endl;
    TAO_PropertySetDefFactory *propsetdef_factory_impl = new TAO_PropertySetDefFactory();
    
    CosPropertyService::PropertySetDefFactory_var propsetdef_factory 
        = propsetdef_factory_impl->_this();

    std::cout << "bind_object_to_corbaloc Property Set Def Factory" << std::endl;
    Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc (orb.in(),
                                                            PROP_SERVICE_ID,
                                                            propsetdef_factory.in());
#endif

    std::cout << "Property Set Factory's corbaloc name: " << PROP_SERVICE_ID << std::endl;
    std::cout << "Property Set Factory's corbaloc port: " << propServPort << std::endl;
    
    try
    {
        using namespace Cdmw::PlatformMngt;
        bool platformManaged 
            = PlatformInterface::Is_launched_by_PlatformManagement(argc, argv);
      
        if (platformManaged) 
        {
            // This has been launched by CDMW Plateform Management
            poa_manager->activate();
            
            // initialise the platform interface
            PlatformInterface::Setup(orb.in(), argc, argv);

#if CDMW_ORB_VDR == orbacus	    
            std::string prop_url = "corbaloc::localhost:" + propServPort + "/PropertyService";
            CosPropertyService::PropertySetDefFactory_var propsetdef_factory 
                = get_PropertySetDefFactory(orb.in(), prop_url);
#endif        
            // Create a Process Behaviour
            // acknowledge the creation of the process
            if (createPropSet == "yes")
            {
                PlatformInterface::Acknowledge_creation(new MyProcessBehaviour(orb.in(), 
                                                         propsetdef_factory.in(), true)); 
            }
            else
            {
                PlatformInterface::Acknowledge_creation(new MyProcessBehaviour(orb.in(), 
                                                         propsetdef_factory.in())); 
            }

            orb->run();

            // cleanup the platform interface
            PlatformInterface::Cleanup();

        }
        else
        {
            // This is launched for unit test with Simrepository

            std::string repositoryCorbaloc 
                = "corbaloc:iiop:localhost:" + repositoryPort + "/SimRepository";

            try
            {
                if (createPropSet == "yes")
                {
                    CdmwNamingAndRepository::Repository_var repository_ref =
                        CdmwNamingAndRepository::Repository::_nil();
                    
                    // get repository from url
                    
                    // Get reference to the repository interface
                    try 
                    {
                        CORBA::Object_var obj 
                            = orb->string_to_object(repositoryCorbaloc.c_str());
                        
                        if(CORBA::is_nil(obj.in())) 
                        {
                            std::cerr << "URL denotes a nil object reference" << std::endl;
                            throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                CORBA::COMPLETED_NO);
                        }
                        repository_ref = CdmwNamingAndRepository::Repository::_narrow(obj.in());
                        
                        if (CORBA::is_nil(repository_ref.in())) 
                        {
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
                    
#if CDMW_ORB_VDR == orbacus	    
                    std::string prop_url = "corbaloc::localhost:" + propServPort + "/PropertyService";
                    CosPropertyService::PropertySetDefFactory_var propsetdef_factory 
                        = get_PropertySetDefFactory(orb.in(), prop_url);

                    install_PropertySetDef(orb.in(), 
                                           propsetdef_factory.in(),
                                           repository_ref.in());
#else  // TAO                   
                    RunThread* run_thread = new RunThread(orb.in(), propsetdef_factory.in(), repository_ref.in());                
                    
                    if (run_thread)
                    {
                        // create Property Set for Cdmw internal use.
                        run_thread->start();
                    }      
#endif
                }

                orb->run();
            }
            catch(const CORBA::Exception& ex)
            {
                std::cerr << ex << std::endl;
                status = FAILURE;
            }
        }
    }
    catch(const CORBA::ORB::InvalidName&)
    {
        std::cerr << "Can't resolve `NameService'" << std::endl;
        status = FAILURE;
    }
    catch(const CORBA::Exception& ex)
    {
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
    catch (...) 
    {
        std::cerr << "unknown exception in main" << std::endl;
        status = FAILURE;
    }           

    if(!CORBA::is_nil(orb.in()))
    {
        try {
            orb -> destroy();
#if CDMW_ORB_VDR == orbacus	    
            OS::kill_process(idPropServer);
#endif
        }
        catch(const CORBA::Exception& ex)
        {
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }        
    
    return status;    
}

