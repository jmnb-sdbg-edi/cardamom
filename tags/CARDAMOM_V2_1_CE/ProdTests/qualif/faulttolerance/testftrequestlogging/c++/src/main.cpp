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


#include <string>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/osthreads/Thread.hpp>
#include <Foundation/ossupport/OS.hpp>
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"
#include "testftrequestlogging/TestHello_impl.hpp"
#include "testftrequestlogging/MyProcessBehaviour.hpp"
#include "testftrequestlogging/TestRequestLogging.hpp"
#include "testftrequestlogging/TestORBInitializer.hpp"
#include "FaultTolerance/ftinit/FTServiceInit.hpp"
#include "FaultTolerance/ftcommon/FTConfiguration.hpp"
#include <ConfAndPlug/cdmwinit/CdmwInterface.hpp>
#include <ConfAndPlug/cdmwinit/InitUtils.hpp>
#include <ConfAndPlug/cdmwinit/CdmwInitConfiguration.hpp>
#include <iostream>

#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
#include <tao/ORBInitializer_Registry.h>
#endif

using namespace Cdmw;
using Cdmw::OsSupport::OS;
using namespace std;

class ORBThread : public OsSupport::Thread {

public:
        
    ORBThread(int argc, char* argv[], CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA,const std::string&  application_name )
    {
        
        m_orb = CORBA::ORB::_duplicate(orb);
        m_rootPOA =  PortableServer::POA::_duplicate(rootPOA);
        PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
        poaMgr->activate();

    }
        
    ~ORBThread() throw() {} ;


    void
    shutdown() 
    {
        m_orb->shutdown(false);
    }

protected:

    void
    run() throw() 
    {
        m_orb->run();   
    }

        
public:

    CORBA::ORB_var  m_orb;
    PortableServer::POA_var m_rootPOA;

};


                               
int main( int argc, char* argv[] )
{
    // Are we started as the main test driver or as a server ?
    std::string server = OS::get_option_value( argc, argv, "--server");
    std::string fault_manager_corbaloc = OS::get_option_value( argc, argv, "--fault_manager");
    std::string host_name = OS::get_hostname();

    if (server == "no") 
    {
        // get host name
        std::string host_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host1");
        // get host name
        std::string host_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host2");
        // get host name
        std::string host_name3 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host3");

        // No option --process, we are the main test driver 
        try
        {
            Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
            strategyList.add_PoaThreadPerConnection();
            strategyList.add_multicast();
            // Initialises the ORB
            CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList, "main_orb");
    
            // Get the root POA
            CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
            PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
            std::cout<<"Get the fault manager reference"<<std::endl;
            CORBA::Object_var fm_obj
            = orb->string_to_object(fault_manager_corbaloc.c_str());
        
            if ( CORBA::is_nil( fm_obj.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid corbaloc" );
        
            ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
        
            CdmwFT::ReplicationManager_var rm =  fault_manager->get_replication_manager();
        
            if ( CORBA::is_nil( rm.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid replication manager reference" );
        
            orb->register_initial_reference("ReplicationManager", rm.in());

            // Start the ORBThread 
            ORBThread orbThread(argc, argv, orb.in(), rootPOA.in(), "main");
            orbThread.start();


            // Initialize XML library
            Cdmw::CdmwInit::InitUtils::init_xml_library();
   
            // Get the XML init filename. This may raise CORBA::BAD_PARAM, 
            // CORBA::NO_MEMORY, or CORBA::INTERNAL    
            std::string xml_file = 
               Cdmw::CdmwInit::InitUtils::get_xml_initialisation_file(argc, 
                                                                      argv);
           
            // Get the XML documents validation option. This may raise 
            // CORBA::NO_MEMORY, or CORBA::INTERNAL
            bool validate_xml = 
               Cdmw::CdmwInit::InitUtils::get_xml_validation_option(argc, 
                                                                    argv);
           
            // Parsing xml
            std::auto_ptr<Cdmw::CdmwInit::CdmwInitConfiguration> xml_data(0);
            try 
            {
               std::auto_ptr<Cdmw::CdmwInit::CdmwInitConfiguration> init
                  (Cdmw::CdmwInit::CdmwInitConfiguration::ReadConfiguration(
                      xml_file,
                      validate_xml));
               
               xml_data = init;
            } 
            catch (const Cdmw::OutOfMemoryException &) 
            {
                std::cerr << __FILE__ << ": line "
                          << __LINE__
                          <<"NO_MEMORY"
                          << std::endl;
                throw  CORBA::BAD_PARAM(Cdmw::OrbSupport::NO_MEMORY,
                                        CORBA::COMPLETED_NO);
            } 
            catch (const Cdmw::CdmwInit::ConfigurationException & ex) 
            {
                std::cerr << "XMLErrorException raised: "
                          << ex.what()
                          << std::endl;
                throw  CORBA::BAD_PARAM(
                      Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                      CORBA::COMPLETED_NO);
            } 
            catch(const Cdmw::InternalErrorException & ex)
            {
                std::cerr << "InternalErrorException raised:"
                          << ex.what()
                          << std::endl;
                throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALXMLLibraryError,
                                      CORBA::COMPLETED_NO);
            }


            // get FT duration_time
            size_t duration_time;
            try 
            {
                  std::string res;
                  try 
                  {
                     res = xml_data->getServiceAttributeValue(
                           "fault-tolerance", 
                           "request-duration-policy"); 
                     duration_time = atoi(res.c_str());
                  } 
                  catch (const Cdmw::BadParameterException& ex) 
                  {
                      std::cerr << "BadParameterException raised: "
                                << ex.what()
                                << std::endl;;
                      throw  CORBA::BAD_PARAM(
                            Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                            CORBA::COMPLETED_NO);
                  }
            } 
            catch (const Cdmw::CdmwInit::XMLErrorException & ex) 
            {
                std::cerr << "XMLErrorException raised: "
                          << ex.what()
                          << std::endl;
                throw  CORBA::BAD_PARAM(
                      Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                      CORBA::COMPLETED_NO);
            }
            catch (const OutOfMemoryException & ex) 
            {
                std::cerr << "NO_MEMORY raised: "
                          << ex.what();
                throw  CORBA::BAD_PARAM(Cdmw::OrbSupport::NO_MEMORY,
                                        CORBA::COMPLETED_NO);
            } 


            // Do test
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            OsSupport::OS::sleep(timescale*1000);

            // Do test
            std::string test_name("TestFTRequestLogging");
            std::string proc_name(argv[0]);
            Cdmw::FT::TestRequestLogging test(orb.in(), 
                                              rootPOA.in(), 
                                              test_name,
                                              duration_time, 
                                              host_name1, 
                                              host_name2, 
                                              host_name3);
            test.start();
        
            // Stop the ORBThread 
            orbThread.shutdown();
            orbThread.join();

            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
        }
        catch( const CORBA::Exception &e )
        {
            cerr << e._name() << endl;
        }
        catch( const Cdmw::Exception &e )
        {
            cerr << e.what() << endl;
        }
        catch(...)
        {
            cerr << "Uncaught exception" << endl;
        }	    
    }
    // Else, we are in a "server" process
    else {
        // get host name
        std::string host_name1 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host1");
        // get host name
        std::string host_name2 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host2");
        // get host name
        std::string host_name3 = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--host3");

        try
        {
            // Initialise FT service
            PortableInterceptor::ORBInitializer_var test_initializer 
               = new Cdmw::TestORBInitializer(host_name1, host_name2, host_name3);
        
            PortableInterceptor::register_orb_initializer(test_initializer.in());

            Cdmw::FT::FTServiceInit::Init( argc, argv );

            std::cout << "[**** " << host_name << "::main] starting with args:" << std::endl;
            for (int i=1; i<argc; ++i) {
                std::cout << "  -> argv[" << i << "] \"" << argv[i] << "\"" << std::endl;
            }
        
            Cdmw::OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded(); // Mandatory in OrbSupport::ORB_init
            strategyList.add_PoaThreadPerConnection();
            strategyList.add_multicast();
        
            // Initialises the ORB
            CORBA::ORB_var orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, 
                                                           argv, strategyList, host_name.c_str());
    
            // Get the root POA
            CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
            PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
    

            PortableServer::POAManager_var poaMgr = rootPOA->the_POAManager();
            poaMgr->activate();
        
            // creates the process behaviour
            std::auto_ptr<MyProcessBehaviour> 
            pProcessBehaviour(new MyProcessBehaviour(orb.in(),rootPOA.in(), host_name));
        
            // init Cdmw
            std::cout << host_name << " test_server: init Cdmw"<<std::endl;
            Cdmw::CdmwInit::CDMW_init(orb.in(), argc, argv,  pProcessBehaviour.get());


            std::cout<< "[" << host_name << "main ]"  << " Get the fault manager reference"<<std::endl;
            CORBA::Object_var fm_obj
            = orb->string_to_object(fault_manager_corbaloc.c_str());

            if ( CORBA::is_nil( fm_obj.in() ) )
                CDMW_THROW2(Exception,
                            Exception::FATAL,
                            "Invalid corbaloc" );
        
            ::CdmwFT::FTManager_var fault_manager = ::CdmwFT::FTManager::_narrow( fm_obj.in() );
        

            // Start the ORBThread 
            ORBThread orbThread(argc, argv, orb.in(), rootPOA.in(), host_name);
            orbThread.start();
            // orbThread will be stopped by orb->shutdown performed during
            // supervision stop.
            orbThread.join();

            Cdmw::CdmwInit::CDMW_cleanup(orb.in());
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
        }
        catch( const CORBA::Exception &e )
        {
            cerr << e._name() << endl;
        }
        catch( const Cdmw::Exception &e )
        {
            cerr << e.what() << endl;
        }
        catch(...)
        {
            cerr << "Uncaught exception" << endl;
        }
    }
}


