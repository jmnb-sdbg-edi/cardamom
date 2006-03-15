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

      try {
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

         CdmwFT::ReplicationManager_var replication_manager =
            CdmwFT::ReplicationManager::_nil();
         try {
             CORBA::Object_var obj = 
                orb->resolve_initial_references("ReplicationManager");
             replication_manager =
                CdmwFT::ReplicationManager::_narrow(obj.in());
         } catch (const CORBA::ORB::InvalidName& e) {
             std::cerr << "ReplicationManager not registered in ORB !!"
                      << std::endl;
             throw CORBA::INTERNAL();
         } catch (const CORBA::SystemException& e) {
             std::cerr << "SystemException while resolving ReplicationManager:\n"
                      << e << std::endl;
             throw CORBA::INTERNAL();
         }
                
         if (CORBA::is_nil(replication_manager.in())) {
             std::cerr << "ReplicationManager reference is null !!" << std::endl;
             throw CORBA::INTERNAL();
         }


         // Location information shall be obtain from supervision
         // but here this is for test purpose!
         ::FT::Location loc;
         loc.length(3);

         loc[0].id = Cdmw::OsSupport::OS::get_hostname().c_str();
         loc[0].kind = "hostname";
         loc[1].id = "APPL";
         loc[1].kind = "application";
         loc[2].id = "PROC";
         loc[2].kind = "process";
         
         // create a reference for the PrimaryBackupGroupRepository
         ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primary_backup_repository
             = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance();
         
         ::Cdmw::FT::Location::PrimaryBackupAdmin_impl * primary_backup_admin
             = new ::Cdmw::FT::Location::PrimaryBackupAdmin_impl(orb, 
                                                                 CDMW_rootPOA.in(), 
                                                                 loc, 
                                                                 primary_backup_repository);
         
         ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin_ref 
             = primary_backup_admin->_this();

         replication_manager->register_location(loc, primary_backup_admin_ref.in());

     
      } catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;
            ex.raise_SystemException();
      } catch (const CORBA::SystemException & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;           
            throw;
      }

   }

      // CDMW Factory objects will be deactivated at ORB shutdown.
      void
      CDMW_cleanup(CORBA::ORB_ptr orb)
         throw ()
      {
         CDMW_ASSERT(!CORBA::is_nil(orb));
      }

   
} // end of namespace CdmwInit

} // end of namespace Cdmw

