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
#include <Foundation/orbsupport/CORBA.hpp>
#include "ftreplicationmanager/PrimaryBackupGroupRepositoryFTManager_impl.hpp"

#include <FaultTolerance/ftcommon/FTLogger.hpp>

#include <Foundation/logging/FunctionLogger.hpp>
namespace Cdmw
{
namespace FT
{
namespace ReplicationManager
{

PrimaryBackupGroupRepositoryFTManager_impl* PrimaryBackupGroupRepositoryFTManager_impl::mp_primary_backup_instance = 0;

PrimaryBackupGroupRepositoryFTManager_impl* 
PrimaryBackupGroupRepositoryFTManager_impl::Get_instance()
    throw()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (!mp_primary_backup_instance)
    {
        // create instance
        mp_primary_backup_instance = new PrimaryBackupGroupRepositoryFTManager_impl();
        mp_instance = mp_primary_backup_instance;
    }

    return mp_primary_backup_instance;
}

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository:1.0
//
PrimaryBackupGroupRepositoryFTManager_impl::PrimaryBackupGroupRepositoryFTManager_impl()
: GroupRepositoryFTManager_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

PrimaryBackupGroupRepositoryFTManager_impl::~PrimaryBackupGroupRepositoryFTManager_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository/is_primary:1.0
//
CORBA::Boolean
PrimaryBackupGroupRepositoryFTManager_impl::is_primary(CORBA::Object_ptr member)
    throw(::FT::MemberNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CORBA::Boolean result = m_ft_manager->is_activated();

    CDMW_LOG_FUNCTION_RETURN(result);
    return result;
}

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository/is_primary_from_gid:1.0
//
CORBA::Boolean
PrimaryBackupGroupRepositoryFTManager_impl::is_primary_from_gid(::FT::ObjectGroupId object_group_id)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    CORBA::Boolean result = m_ft_manager->is_activated();

    CDMW_LOG_FUNCTION_RETURN(result);
    return result;
}

} // End of namespace ReplicationManager

} // End of namespace FT

} // End of namespace Cdmw

