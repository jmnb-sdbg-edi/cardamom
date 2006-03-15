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

#ifndef INCL_CDMW_FT_LOCATION_SIM_PRIMARY_BACKUP_GROUP_REPOSITORY_IMPL_HPP
#define INCL_CDMW_FT_LOCATION_SIM_PRIMARY_BACKUP_GROUP_REPOSITORY_IMPL_HPP

// Cdmw Files
#include <FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp>
//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{

namespace FT
{

//
// IDL:thalesgroup.com/CdmwFT/Location:1.0
//
namespace Location
{

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository:1.0
//
class SIM_PrimaryBackupGroupRepository_impl : public virtual PrimaryBackupGroupRepository_impl
{

public:
    SIM_PrimaryBackupGroupRepository_impl();
    ~SIM_PrimaryBackupGroupRepository_impl();

    //
    // IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group:1.0
    //
    virtual ::FT::ObjectGroup_ptr get_object_group(CORBA::Object_ptr member)
        throw(::FT::MemberNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_other_group_members:1.0
    //
    virtual CdmwFT::MemberInfos* get_other_group_members(CORBA::Object_ptr member)
        throw(::FT::MemberNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group_version:1.0
    //
    virtual ::FT::ObjectGroupRefVersion get_object_group_version(CORBA::Object_ptr member)
        throw(::FT::MemberNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group_from_gid:1.0
    //
    virtual ::FT::ObjectGroup_ptr get_object_group_from_gid(::FT::ObjectGroupId object_group_id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_other_group_members_from_gid:1.0
    //
    virtual CdmwFT::MemberInfos* get_other_group_members_from_gid(::FT::ObjectGroupId object_group_id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group_version_from_gid:1.0
    //
    virtual ::FT::ObjectGroupRefVersion get_object_group_version_from_gid(::FT::ObjectGroupId object_group_id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository/is_primary:1.0
    //
    virtual CORBA::Boolean is_primary(CORBA::Object_ptr member)
        throw(::FT::MemberNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository/is_primary_from_gid:1.0
    //
    virtual CORBA::Boolean is_primary_from_gid(::FT::ObjectGroupId object_group_id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);

    virtual CdmwFT::ReplicationManager* the_replication_manager()
        throw(CORBA::SystemException);
    
    virtual ::FT::Location* the_location()
          throw(CORBA::SystemException);
  
        
};

} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_LOCATION_SIM_PRIMARY_BACKUP_GROUP_REPOSITORY_IMPL_HPP
