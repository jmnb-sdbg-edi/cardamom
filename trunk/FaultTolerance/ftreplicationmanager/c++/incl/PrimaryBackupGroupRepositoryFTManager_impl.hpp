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


#ifndef INCL_CDMW_FT_LOCATION_PRIMARY_BACKUP_GROUP_REPOSITORY_FTMANAGER_IMPL_HPP
#define INCL_CDMW_FT_LOCATION_PRIMARY_BACKUP_GROUP_REPOSITORY_FTMANAGER_IMPL_HPP

// Cdmw Files
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <ftreplicationmanager/GroupRepositoryFTManager_impl.hpp>

#include <FaultTolerance/idllib/CdmwFTGroupRepository.skel.hpp>

//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{

namespace FT
{

namespace ReplicationManager
{

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository:1.0
//
class PrimaryBackupGroupRepositoryFTManager_impl : virtual public CdmwFT::Location::PrimaryBackupGroupRepository, 
                                                   virtual public GroupRepositoryFTManager_impl,
                                                   virtual public Cdmw::OrbSupport::RefCountLocalObject
{

public:
    /**
    * Purpose:
    * <p>  
    *  Return the single instance of the PrimaryBackupGroupRepository_impl 
    *
    *@param  none
    *@exception none
    */ 
    static PrimaryBackupGroupRepositoryFTManager_impl* Get_instance()
        throw();

    ~PrimaryBackupGroupRepositoryFTManager_impl();

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

protected:
    PrimaryBackupGroupRepositoryFTManager_impl();




private:
    PrimaryBackupGroupRepositoryFTManager_impl(const PrimaryBackupGroupRepositoryFTManager_impl&);
    void operator=(const PrimaryBackupGroupRepositoryFTManager_impl&);

    // singleton instance
    static PrimaryBackupGroupRepositoryFTManager_impl* mp_primary_backup_instance;  


};

} // End of namespace ReplicationManager

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_LOCATION_PRIMARY_BACKUP_GROUP_REPOSITORY_FTMANAGER_IMPL_HPP

