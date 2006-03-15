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


#if CDMW_ORB_VDR == tao && CDMW_ORB_VER > 13
#include <tao/ORBInitializer_Registry.h>
#endif
#include <FaultTolerance/ftreplicationmanager/InitUtils.hpp>
#include <FaultTolerance/ftcommon/FTConfiguration.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include "ftreplicationmanager/ReplicationManager_impl.hpp"
#include "ftreplicationmanager/FTORBInitializer.hpp"
#include "ftreplicationmanager/ReplicationManagerRepository_impl.hpp"

#include <Foundation/logging/LogManager.hpp>
namespace
{    
    /**
     * Name of the POA for the CDMW ReplicationManager object
     */
    static const char*  REPLICATION_MANAGER_POA_NAME = "ReplicationManagerPOA";

}; // End anonymous namespace


namespace Cdmw {
namespace FT {
namespace ReplicationManager {

/**
 * Purpose:
 * <p> pre_init the ORB where a CDMW ReplicationManager object will be created.
 * 
 *@exception CORBA::SystemException
 */        
    
void
InitUtils::Pre_init(int& argc, char** argv)
    throw(CORBA::SystemException)
{

    // Init the Logging library
    Cdmw::Logging::LogManager::Init(argc, argv);

    FTConfiguration::Activate_FT_service(argc, argv);

    // register_interceptors();
    PortableInterceptor::ORBInitializer_var initializer 
    = new Cdmw::FT::ReplicationManager::ORBInitializer();
    
    PortableInterceptor::register_orb_initializer(initializer.in());
}
    
    
    
/** Purpose:
 * <p> post_init the context where a CDMW ReplicationManager object has been created.
 * 
 *@exception CORBA::SystemException
 */    
    
void
InitUtils::Post_init (CdmwFT::ReplicationManager_ptr the_replication_manager)
    throw(CORBA::SystemException)
{
    ReplicationManagerRepository_impl* repl_mng_rep 
    = ReplicationManagerRepository_impl::Get_instance();
    
    repl_mng_rep->init(the_replication_manager);
}

    
    
/**
 * Purpose:
 * <p> Creates a CDMW ReplicationManager object.
 * <b>NOTA</b>: The current version of the CDMW ReplicationManager
 * serialises processing of all request invocations.
 * 
 *@param orb  A reference to the ORB pseudo-object
 *@param parent  A reference to the CdmwRootPOA pseudo-object
 *@param ft_domain_id  The Fault Tolerance domain identifier
 *@param echo_stream An output stream for use by the ReplicationManager
 * whenever there is a need to output a debug/warning/info message.
 *
 *@return A reference to a CDMW ReplicationManager object
 *
 *@exception CORBA::SystemException
 */        
CdmwFT::ReplicationManager_ptr
InitUtils::Create_replication_manager(int argc, 
                                      char* argv[],
                                      CORBA::ORB_ptr          orb,
                                      PortableServer::POA_ptr parent,
                                      const char*             ft_domain_id,
                                      const char*             security_level,
                                      const char*             replication_level,
                                      std::ostream &          echo_stream)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
   
    try {
        
        // I) Create a single thread POA for the ReplicationManager's object reference
        // (with Retain and Single-thread policicies)
        // to be used to activate the CDMW replication manager object.
        // Lifespan policy            = PERSISTENT
        // Id Assignment policy       = SYSTEM_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY
        // Servant Retention policy   = RETAIN;
        // Thread Policy              = SINGLE_THREAD_MODEL
        //        
        using namespace PortableServer;
        PortableServer::POAManager_var poaManager = parent -> the_POAManager();
        CORBA::PolicyList policies;
        policies.length(7);
        policies[0] = parent -> create_id_assignment_policy       (SYSTEM_ID);
        policies[1] = parent -> create_lifespan_policy            (PERSISTENT);
        policies[2] = parent -> create_servant_retention_policy   (RETAIN);
        policies[3] = parent -> create_id_uniqueness_policy       (UNIQUE_ID);
        policies[4] = parent -> create_request_processing_policy  (USE_ACTIVE_OBJECT_MAP_ONLY);
        policies[5] = parent -> create_implicit_activation_policy (IMPLICIT_ACTIVATION);
//        policies[6] = parent -> create_thread_policy              (SINGLE_THREAD_MODEL);
        policies[6] = parent -> create_thread_policy              (ORB_CTRL_MODEL);
        
        Cdmw::OrbSupport::StrategyList poaStrategies;
        
        PortableServer::POA_var rep_poa =
            Cdmw::OrbSupport::OrbSupport::create_POA(parent,
                                                      REPLICATION_MANAGER_POA_NAME,
                                                      poaManager.in(), 
                                                      policies,
                                                      poaStrategies);

        const ::FT::FTDomainId  ft_domain = CORBA::string_dup(ft_domain_id); // TODO: memory leak!
        ReplicationManager_impl * repmngr =
            new ReplicationManager_impl(argc,
                                        argv,
                                        orb,
                                        rep_poa.in(),
                                        ft_domain,
                                        security_level,
                                        replication_level,
                                        echo_stream);
        
        PortableServer::ServantBase_var serv  = repmngr;
        
        // II) Activate the Replication Manager
        CdmwFT::ReplicationManager_var the_replication_manager = repmngr->_this();
        
        return the_replication_manager._retn();
    } catch (const PortableServer::POA::AdapterAlreadyExists &) {
        // A Replication Manager is a singleton!
        // TODO: Add exception minot codes
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                      CORBA::COMPLETED_NO) );
    } catch (const PortableServer::POA::InvalidPolicy &) {
        // TODO: Add exception minot codes
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                      CORBA::COMPLETED_NO) );
    } catch (const CORBA::SystemException & ex) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
    }
     
};
    
}; // namespace ReplicationManager    
}; // namespace FT
}; // namespace Cdmw

