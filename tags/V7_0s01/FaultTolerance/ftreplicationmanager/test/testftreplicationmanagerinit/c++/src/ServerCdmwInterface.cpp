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


#include <memory> // for std::auto_ptr
#include <string>
#include <vector>
#include <fstream>


#include <common/Assert.hpp>
#include <common/Exception.hpp>
#include <ossupport/OS.hpp>
#include <osthreads/Mutex.hpp>
#include <osthreads/Condition.hpp>
#include <orbsupport/ExceptionMinorCodes.hpp>
#include <orbsupport/OrbSupport.hpp>
#include <orbsupport/CORBASystemExceptionWrapper.hpp>
#include <orbsupport/StrategyList.hpp>
#include <orbsupport/POAThreadingPolicy.hpp>


#include <ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp>
#include <ftlocationmanager/PrimaryBackupAdmin_impl.hpp>

#include <idllib/CdmwFTReplicationManager.stub.hpp>


   

namespace
{
/*    using namespace Cdmw::CdmwInit;
    
    static void ADVISE_EXCEPTION_RAISED(CdmwProcessDelegate_impl *process_delegate_impl)
    {
        if (process_delegate_impl != 0) {
            process_delegate_impl->set_cdmw_init_exception_raised();
            process_delegate_impl->get_condition().broadcast();
        }
    }
*/

    CdmwFT::ReplicationManager_ptr
    get_replication_manager_ref_from_file(CORBA::ORB_ptr orb)
        throw (CORBA::SystemException)
    {
        CORBA::Object_var objref;
        CdmwFT::ReplicationManager_var replication_manager_ref;
        
        // import the object reference from the file
        std::ifstream is("replication_manager.ior");
        if (is.good())
        {
            std::string objref_str;
            is >> objref_str;
            try
            {
                std::cout << "string_to_object on " << objref_str << std::endl;
                objref = orb->string_to_object(objref_str.c_str());

                std::cout << "narrow replication manager ref" << std::endl;
                replication_manager_ref = CdmwFT::ReplicationManager::_narrow(objref.in());
                std::cout << "end of narrow" << std::endl;
            }
            catch( CORBA::SystemException& e )
            {
                std::cerr << " CORBA System Exception raised: " << e << std::endl;
                std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
                    << " IN FILE: " << __FILE__ << std::endl; 
                throw;
            }
            catch (...)
            {
                throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            }
            
            is.close();
        }
        else
        {
            std::cerr << "Error opening reference filename: replication_manager.ior" << std::endl;
            throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        }
        
        return replication_manager_ref._retn();
    }

    class InitUtils
    {
        InitUtils();

    public:
        static PortableServer::POA_ptr
        get_root_POA(CORBA::ORB_ptr orb)
            throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
        {
            CDMW_ASSERT(!CORBA::is_nil(orb));
            
            PortableServer::POA_var root = PortableServer::POA::_nil();
            try {  
                //
                // Resolve the Root POA
                //            
                CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
                root = PortableServer::POA::_narrow(obj.in());
            } catch (const CORBA::ORB::InvalidName & ) {
                // Raised by resolve_initial_references("RootPOA") (shouldn't happen)
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                    CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                    CORBA::COMPLETED_NO) );
            } catch (const CORBA::SystemException & ex) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
            }
            return root._retn();
        } 

        static PortableServer::POA_ptr
        create_cdmw_root_POA(CORBA::ORB_ptr orb)
            throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
        {
            //
            // Create the CDMW root POA with its own POA manager.
            // Use Default POA policies:
            //
            // Lifespan policy            = TRANSIENT
            // Id Assignment policy       = SYSTEM_ID
            // Id Uniqueness policy       = UNIQUE_ID
            // Implicit Activation policy = IMPLICIT_ACTIVATION
            // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY
            // Servant Retention policy   = RETAIN;
            // Thread Policy              = ORB_CTRL_MODEL;
            //
            
            using namespace PortableServer;
            
            // Resolve the Root POA. May raise a CORBA::SystemException.
            POA_var root = get_root_POA(orb);
            
            CORBA::PolicyList policies;
            policies.length(7);
            policies[0] = root -> create_id_assignment_policy       (SYSTEM_ID);
            policies[1] = root -> create_lifespan_policy            (TRANSIENT);
            policies[2] = root -> create_servant_retention_policy   (RETAIN);
            policies[3] = root -> create_id_uniqueness_policy       (UNIQUE_ID);
            policies[4] = root -> create_request_processing_policy  (USE_ACTIVE_OBJECT_MAP_ONLY);
            policies[5] = root -> create_implicit_activation_policy (IMPLICIT_ACTIVATION);
            policies[6] = root -> create_thread_policy              (ORB_CTRL_MODEL);
            
            Cdmw::OrbSupport::StrategyList poaStrategies;
#if CDMW_ORB_VDR == tao
            Cdmw::OrbSupport::ThreadPerConnectionPolicy threading;
#else
            Cdmw::OrbSupport::ThreadPoolPolicy threading(CDMW_ROOT_POA_THREAD_POOL_SIZE);
#endif
            threading.append_to_StrategyList(poaStrategies);
            
            POA_var CDMW_rootPOA = POA::_nil();
            
            try {
                CDMW_rootPOA 
                    = Cdmw::OrbSupport::OrbSupport::create_POA(root.in(),
                    Cdmw::OrbSupport::CDMW_ROOT_POA_NAME,
                    POAManager::_nil(),
                    policies,
                    poaStrategies);  
                
                
            } catch (const POA::AdapterAlreadyExists & ) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                    CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMCdmwRootPOAExists,
                    CORBA::COMPLETED_NO) );
            } catch (const POA::InvalidPolicy & ) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                    CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                    CORBA::COMPLETED_NO) );
            } catch (const CORBA::SystemException & ex) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
                // TODO:?
            }
            
            return CDMW_rootPOA._retn();    
        }

    };

} // end anonymous namespace
   

   //--------------------------------------------------------------------------

   
namespace Cdmw {

namespace CdmwInit {


   
    void 
    CDMW_init(CORBA::ORB_ptr orb, int & argc, char** argv) 
   	throw (CORBA::SystemException)
    {

            CDMW_ASSERT(!CORBA::is_nil(orb));
//            CdmwProcessDelegate_impl * process_delegate_impl = 0;

      try {
/*         // Get the XML init filename. This may raise CORBA::BAD_PARAM, CORBA::NO_MEMORY,
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
*/
         // *****************************************************************
         // *****************  CDMW root POA and servant manager ***********
         // *****************************************************************
         //
         // Create the CDMW root POA. It has its own POA Manager
         // May raise a CORBA::SystemException.
         //
            PortableServer::POA_var CDMW_rootPOA = ::InitUtils::create_cdmw_root_POA(orb);
        
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
         // *************  setup FaultTolerance LocationManager  ************
         // *****************************************************************
         // Location information shall be obtain from supervision
         // but here this is for test purpose!
         ::FT::Location loc;
         loc.length(3);

         // get ReplicationManager ref and register ref
         std::cout<<"get the ReplicationManager ref and register it"<<std::endl;
         CdmwFT::ReplicationManager_var rm
             = get_replication_manager_ref_from_file(orb);

         orb->register_initial_reference("ReplicationManager", rm.in());

         // get host name
         std::string host_name = Cdmw::OsSupport::OS::get_option_value (argc, argv, "--HostName");
         
         if (host_name  == "no") 
         {
             host_name = "HOST1";
         } 
         loc[0].id = host_name.c_str();
         loc[0].kind = "hostname";
         
         loc[1].id = "APPL";
         loc[1].kind = "application";
         loc[2].id = "PROC";
         loc[2].kind = "process";
         
         // create a reference for the PrimaryBackupGroupRepository
         ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primary_backup_repository
             = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();
         
         
         std::cout<<"create the PrimaryBackupAdmin"<<std::endl;
         ::Cdmw::FT::Location::PrimaryBackupAdmin_impl * primary_backup_admin
             = new ::Cdmw::FT::Location::PrimaryBackupAdmin_impl(orb, 
                                                                  CDMW_rootPOA.in(), 
                                                                  loc, 
                                                                  primary_backup_repository);
         
         std::cout<<" : create a reference for the PrimaryBackupAdmin"<<std::endl;
         ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin_ref 
             = primary_backup_admin->_this();

         // export the object reference to a file
         std::string file_name = host_name +"_primary_backup_admin.ior";
         CORBA::String_var ref_string = orb->object_to_string(primary_backup_admin_ref.in());
         std::ofstream os(file_name.c_str());
         os << ref_string.in();
         os.close();
         rm->register_location(loc, primary_backup_admin_ref.in());

/*
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
            process_delegate_impl = new CdmwProcessDelegate_impl(orb,
                                                 rootPOA.in(),
                                                 CDMW_rootPOA.in(),
                                                 process_ctrl_var._retn());
            PortableServer::ServantBase_var process_delegate_servant = process_delegate_impl;
            
            CdmwPlatformMngt::ProcessDelegate_var process = process_delegate_impl->_this();
            
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
        
         // Retreive Repository object reference. This may raise CORBA::BAD_PARAM
         // or CORBA::BAD_INV_ORDER
         CdmwNamingAndRepository::Repository_var repository 
            = InitUtils::get_cdmw_repository();
   

   
      
        // *****************************************************************
         // *****************  INIT of RepositoryInterface ******************
         // *****************************************************************
         std::string default_domaine_name("");
         try {
            default_domaine_name  
               = xml_data->getServiceAttributeValue("repository-interface", "default-domain-name");
	      } catch (const Cdmw::BadParameterException& ex) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                           CORBA::COMPLETED_NO),
                          ex.what() );       
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

         try {
            Cdmw::NamingAndRepository::RepositoryInterface::init
               (default_domaine_name, repository.in());
         } catch (const CdmwNamingAndRepository::NoNameDomain & ) {
            // Default NameDomain is not a NameDomain! 
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMNameDomainNotFound,
                                           CORBA::COMPLETED_NO),
                          default_domaine_name + " is not a NameDomain!" );
         } catch (const CdmwNamingAndRepository::InvalidName &  ex) {
            // Default NameDomain name is ill-formed! 
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidDefaultDomainName,
                                           CORBA::COMPLETED_NO),
                          default_domaine_name + " is ill-formed :" + ex.reason.in() );
         } catch (const CORBA::SystemException & ex) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                          ex,
                          "Cannot initialise RepositoryInterface." );
         }
      
   
      
         // *****************************************************************
         // *********  Create CDMW Factories POA (persistent POA)  *********
         // ***************************************************************** 
         // Raises CORBA::SystemException
         PortableServer::POA_var factories_poa 
            = InitUtils::create_cdmw_LifeCycle_POA(CDMW_rootPOA.in());                 

            

   
         // Discard requests until run order comes from the platform management
         poaManager->discard_requests(false);

         // advise process_delegate_impl that cdmw_init is done
         process_delegate_impl->set_cdmw_init_done();
         process_delegate_impl->get_condition().broadcast();
    
*/
     
   
      
     
      } catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;
//            ADVISE_EXCEPTION_RAISED(process_delegate_impl);   
            ex.raise_SystemException();
      } catch (const CORBA::SystemException & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;           
//            ADVISE_EXCEPTION_RAISED(process_delegate_impl);
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
         //Cdmw::NamingAndRepository::RepositoryInterface::finish();
     
      }

   
} // end of namespace CdmwInit

} // end of namespace Cdmw

