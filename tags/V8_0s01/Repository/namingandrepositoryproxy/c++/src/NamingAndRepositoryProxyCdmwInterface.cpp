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

/* =========================================================================== * 
 * This code has been generated by CDMW Code Generator 1.0
 * on Wed Mar 23 18:23:31 CET 2005
 * DO NOT EDIT !!!
 * =========================================================================== */


   

   
   #include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
   #include "ConfAndPlug/cdmwinit/CdmwInitConfiguration.hpp"
   #include "ConfAndPlug/cdmwinit/InitUtils.hpp"
   #include "SystemMngt/platforminterface/PlatformInterface.hpp"
   #include "Foundation/common/Assert.hpp"

   #include "Foundation/orbsupport/StrategyList.hpp"

   #include "Foundation/osthreads/Mutex.hpp"
   #include "Foundation/osthreads/Condition.hpp"

   #include "Foundation/common/Exception.hpp"
   #include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

   #include "Foundation/orbsupport/OrbSupport.hpp"
   #include "Repository/repositoryinterface/RepositoryInterface.hpp"
   

   
      
      #include "Foundation/ossupport/OS.hpp"
      #include "FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp"
      #include "FaultTolerance/ftlocationmanager/PrimaryBackupAdmin_impl.hpp"
      #include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"
      #include "FaultTolerance/ftcommon/FTConfiguration.hpp"
      #include "Foundation/commonsvcs/datastore/StorageHome.hpp"
      #include "FaultTolerance/ftstatemanager/DataStore.hpp"
	  
      
   



   
      #include <memory> // for std::auto_ptr
      #include <string>
      #include <vector>
   

   
   
   
   #define CDMW_TIME_SERVICE_ENABLED

   
      namespace
      {
         using namespace Cdmw::CdmwInit;

         static void ADVISE_EXCEPTION_RAISED(CdmwProcess_impl *process_impl)
         {
             if (process_impl != 0) {
                 process_impl->set_cdmw_init_exception_raised();
                 process_impl->get_condition().broadcast();
             }
         }
      } // end anonymous namespace
   

   //--------------------------------------------------------------------------

   
namespace Cdmw {

namespace CdmwInit {




   
    void 
    CDMW_init(CORBA::ORB_ptr orb, int & argc, char** argv, ProcessControl* ctrl) 
   	throw (CORBA::SystemException)
    {

            CDMW_ASSERT(!CORBA::is_nil(orb));
            CdmwProcess_impl * process_impl = 0;

      try {
         // Initialize XML library
         Cdmw::CdmwInit::InitUtils::init_xml_library();

         // Get the XML init filename. This may raise CORBA::BAD_PARAM, CORBA::NO_MEMORY,
         // or CORBA::INTERNAL    
         std::string xml_file = InitUtils::get_xml_initialisation_file(argc, argv);
        
         // Get the XML documents validation option. This may raise CORBA::NO_MEMORY,
         // or CORBA::INTERNAL
         bool validate_xml = InitUtils::get_xml_validation_option(argc, argv);
        
         // Parsing xml
         std::auto_ptr<Cdmw::CdmwInit::CdmwInitConfiguration> xml_data(0);
         try {
            std::auto_ptr<Cdmw::CdmwInit::CdmwInitConfiguration> init
               (Cdmw::CdmwInit::CdmwInitConfiguration::ReadConfiguration(xml_file,
                                                                            validate_xml));
            
            xml_data = init;
         } catch (const Cdmw::OutOfMemoryException &) {
            CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                          CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY, 
                                           CORBA::COMPLETED_NO) );
         } catch (const Cdmw::CdmwInit::ConfigurationException & ex) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                           CORBA::COMPLETED_NO),
                          ex.what());
         } catch(const Cdmw::InternalErrorException & ex){
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALXMLLibraryError,
                                          CORBA::COMPLETED_NO),
                          ex.what() );
         }

         // *****************************************************************
         // *****************  CDMW root POA and servant manager ***********
         // *****************************************************************
         //
         // Create the CDMW root POA. It has its own POA Manager
         // May raise a CORBA::SystemException.
         //
         PortableServer::POA_var CDMW_rootPOA = InitUtils::create_cdmw_root_POA(orb);
        
         PortableServer::POAManager_var poaManager = CDMW_rootPOA -> the_POAManager();
         //
         // Initialize the server so that incoming requests are
         // handled from this point on
         //
         // May raise PortableServer::AdapterInactive or CORBA::SystemException
         try {
            poaManager -> activate(); // TODO: maybe move it?
         } catch (const PortableServer::POAManager::AdapterInactive & ) {
            CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALORBAdapterInactive,
                                          CORBA::COMPLETED_NO) );
         } catch (const CORBA::SystemException & ex ) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                          ex, "Could not activate the CDMW POA Manager!" );
         }
   

    
      
         // *****************************************************************
         // *************  setup FaultTolerance Policies   ******************
         // *****************************************************************

         Cdmw::OrbSupport::OrbSupport::set_endpoint_selector_factory(::Cdmw::FT::FTConfiguration::Get_FT_endpoint_selector_factory());

         size_t duration_time;
         std::string client_id_str;
            
         try {
               std::string res;
               try {
                  res = xml_data->getServiceAttributeValue("fault-tolerance", "request-duration-policy"); 
                  duration_time = atoi(res.c_str());
               } catch (const Cdmw::BadParameterException& ex) {
                  CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                 CORBA::COMPLETED_NO),
                                ex.what() );
               }

               try  {
                  client_id_str = xml_data->getServiceAttributeValue("fault-tolerance", "client-id");
				  ::Cdmw::FT:: FTConfiguration::Set_client_id(orb, client_id_str.c_str());
               } catch (const Cdmw::BadParameterException& ) {
                  // do nothing : parameter is optional
               }
          } catch (const Cdmw::CdmwInit::XMLErrorException & ex) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                             CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                              CORBA::COMPLETED_NO),
                             ex.what() );
          } catch (const OutOfMemoryException & ex) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                             CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                              CORBA::COMPLETED_NO),
                             ex.what() );
          } 

         CORBA::Object_var obj;
         obj = orb->resolve_initial_references("ORBPolicyManager");
        
         CORBA::PolicyManager_var policy_manager 
            = CORBA::PolicyManager::_narrow(obj.in());
        
         // Create and apply an ORB-wide Routed policy
         CORBA::Any any;
         TimeBase::TimeT duration = duration_time;

         any <<= duration;
         CORBA::PolicyList policies(1);
         policies.length(1);
         policies[0] =
            orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);
        
         policy_manager->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
      
   



      
         // *****************************************************************
         // *****************  setup platforminterface object  **************
         // *****************************************************************

         // Did we receive a ProcessControl callback object?
         {
            ProcessControl_var process_ctrl_var;
            if (ctrl != 0)
                process_ctrl_var = ctrl;
            else
                process_ctrl_var = new DefaultProcessControl();
            
            // CdmwProcess_impl() will acquire a reference on ProcessControl object and
            // will automatically release it at object's destruction.
            PortableServer::POA_var rootPOA = InitUtils::get_root_POA(orb);
            process_impl = new CdmwProcess_impl(orb,
                                                 rootPOA.in(),
                                                 CDMW_rootPOA.in(),
                                                 process_ctrl_var._retn());
            PortableServer::ServantBase_var process_impl_servant = process_impl;
            
            CdmwPlatformMngt::ProcessDelegate_var process = process_impl->_this();
            
            InitUtils::init_platform_interface(orb, argc, argv, process.in());
            //  From now on, a platform supervisor may initiate a call to 
            //  process->init() method. 
            //  process->init() call is blocked with a condition until CDMW
            //  init has completed?
         }

         //
         // Get the process name. This may raise CORBA::BAD_INV_ORDER
         std::string process_name = InitUtils::get_cdmw_process_name();
         //
         // Get the application name. This may raise CORBA::BAD_INV_ORDER
         std::string application_name = InitUtils::get_cdmw_application_name();  
           
   
   
         // *****************************************************************
         // ***********  setup FaultTolerance ReplicationManager  ***********
         // *****************************************************************

         // Get the reference of the Replication Manager and register it in the ORB
         CORBA::Object_var objref;
         const char* replication_manager_name = "ReplicationManager";
			objref =  InitUtils::get_service(replication_manager_name);  

         CdmwFT::ReplicationManager_var replication_manager = CdmwFT::ReplicationManager::_narrow(objref.in());

	      try 
         {
             orb->register_initial_reference("ReplicationManager", replication_manager.in());      
         }
         catch (const CORBA::SystemException& ex)
         {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                          ex,
                          "Cannot register ReplicationManager." );
          }
          catch (const CORBA::ORB::InvalidName& )
          {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO),
								  "InvalidName => Cannot register ReplicationManager.");
          }
	 
	

   
         // Discard requests until run order comes from the platform management
         poaManager->discard_requests(false);

         // advise process_impl that cdmw_init is done
         process_impl->set_cdmw_init_done();
         process_impl->get_condition().broadcast();
    
     
   
      
     
      } catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;
            ADVISE_EXCEPTION_RAISED(process_impl);   
            ex.raise_SystemException();
      } catch (const CORBA::SystemException & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;           
            ADVISE_EXCEPTION_RAISED(process_impl);
            throw;
      }
      
      
   

   }

      // CDMW Factory objects will be deactivated at ORB shutdown.
      void
      CDMW_cleanup(CORBA::ORB_ptr orb)
         throw ()
      {
         CDMW_ASSERT(!CORBA::is_nil(orb));
         
         // Release all static object references held by RepositoryInterface
         Cdmw::NamingAndRepository::RepositoryInterface::finish();
   
         // Release all static object references held by PlatformInterface
         Cdmw::PlatformMngt::PlatformInterface::Cleanup();
   
         try {
             // Cleanup XML library
             Cdmw::CdmwInit::InitUtils::cleanup_xml_library();
         } catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper& ex) {
             std::cerr << "CDMW_cleanup Warning at XML library cleanup: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;
         }	 
      }

   
} // end of namespace CdmwInit

} // end of namespace Cdmw
