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


// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>

#include <Foundation/logging/FunctionLogger.hpp>

// Local Files
#include <ftreplicationmanager/FTORBInitializer.hpp>
#include <ftreplicationmanager/FTServerRequestInterceptorFTManager_impl.hpp>
#include <ftreplicationmanager/ReplicationManagerRepository_impl.hpp>
#include <ftreplicationmanager/PrimaryBackupGroupRepositoryFTManager_impl.hpp>


namespace 
{
    const CORBA::ORB::ObjectId FT_CURRENT_ID = "FTCurrent";
    const CORBA::ORB::ObjectId FT_GROUP_REPOSITORY = "FTGroupRepository";
    const PortableInterceptor::ORBInitInfo::ObjectId FT_REPLICATION_MANAGER_REPOSITORY 
        = "FTReplicationManagerRepository";

} // end of anonymous namespace


namespace Cdmw
{
    
namespace FT
{

namespace ReplicationManager
{

// constructor
ORBInitializer::ORBInitializer()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

//destructor
ORBInitializer::~ORBInitializer()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

void
ORBInitializer::pre_init(PortableInterceptor::ORBInitInfo_ptr info) 
  throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

    
//
// Note: the interceptors are registered in post_init()
//       if their constructors require
//       ORBInitInfo::resolve_initial_reference(), which 
//       cannot be called in pre_init().
//
void
ORBInitializer::post_init(PortableInterceptor::ORBInitInfo_ptr info) 
   throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    //
    // allocate needed slot_id
    //
    CDMW_DEBUG(FTLogger::GetLogger(),"post_init => allocate needed slot_id");
    PortableInterceptor::SlotId  ft_domain_id_slot_id = info->allocate_slot_id();
    PortableInterceptor::SlotId  object_group_id_slot_id = info->allocate_slot_id();
    PortableInterceptor::SlotId  object_group_ref_version_slot_id = info->allocate_slot_id();
    PortableInterceptor::SlotId  client_id_slot_id = info->allocate_slot_id();
    PortableInterceptor::SlotId  retention_id_slot_id = info->allocate_slot_id();
    PortableInterceptor::SlotId  expiration_time_slot_id = info->allocate_slot_id();
    PortableInterceptor::SlotId  is_a_ft_request_slot_id = info->allocate_slot_id();

    //
    //  create PrimaryBackupGroupRepository
    //
    CDMW_DEBUG(FTLogger::GetLogger(),"post_init => Create PrimaryBackupGroupRepository");
    CdmwFT::Location::PrimaryBackupGroupRepository_var primary_backup_rep 
    = FT::ReplicationManager::PrimaryBackupGroupRepositoryFTManager_impl::Get_instance();

    try
    {
        info->register_initial_reference(FT_GROUP_REPOSITORY, primary_backup_rep.in());
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        CDMW_FATAL(FTLogger::GetLogger(),
                   "ERROR: Impossible to register FT_GROUP_REPOSITORY! Name is invalid!");
        CDMW_ASSERT(false);
    }
    catch (const CORBA::SystemException& e)
    {
        CDMW_FATAL(FTLogger::GetLogger(),
                   "ERROR: Impossible to register FT_GROUP_REPOSITORY! "
                   << e );
        CDMW_ASSERT(false);
    }


    //
    //  create ReplicationManagerRepository
    //
    CdmwFT::ReplicationManagerRepository_var repl_mng_rep 
    = Cdmw::FT::ReplicationManagerRepository_impl::Get_instance();
    
    try
    {
        CDMW_DEBUG(FTLogger::GetLogger(),"post_init => Create ReplicationManagerRepository");
        info->register_initial_reference(FT_REPLICATION_MANAGER_REPOSITORY, 
                                         repl_mng_rep.in());
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: Impossible to register FT_REPLICATION_MANAGER_REPOSITORY! Name is invalid!");
        CORBA::INTERNAL ex(Cdmw::OrbSupport::INTERNALFaultToleranceError, 
                           CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    catch (const CORBA::SystemException& e)
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: Impossible to register FT_REPLICATION_MANAGER_REPOSITORY! " << e);
        CDMW_LOG_FUNCTION_EXCEPTION(e);
        throw;
    }
        //
    // Create and register the interceptors
    //
    CDMW_DEBUG(FTLogger::GetLogger(),"post_init => Create the interceptors");
    PortableInterceptor::ServerRequestInterceptor_var serverInterceptor;    


    serverInterceptor 
        = new ServerRequestInterceptorFTManager_impl(info, 
                                                     ft_domain_id_slot_id,
                                                     object_group_id_slot_id,
                                                     object_group_ref_version_slot_id,
                                                     client_id_slot_id,
                                                     retention_id_slot_id,
                                                     expiration_time_slot_id,
                                                     is_a_ft_request_slot_id);
    

    try
    {
        info->add_server_request_interceptor(serverInterceptor.in());
    }
    catch(const PortableInterceptor::ORBInitInfo::DuplicateName&)
    {
        // can not happened while interceptor is not named!
        CDMW_ASSERT(false);
    }
    

}


} // End of namespace ReplicationManager

} // End of namespace FT

} // End of namespace Cdmw



