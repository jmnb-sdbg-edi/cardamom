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


#ifndef INCL_CDMW_FT_LOCATION_PRIMARY_BACKUP_GROUP_REPOSITORY_IMPL_HPP
#define INCL_CDMW_FT_LOCATION_PRIMARY_BACKUP_GROUP_REPOSITORY_IMPL_HPP

// Cdmw Files
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <FaultTolerance/ftlocationmanager/GroupRepository_impl.hpp>

#include <idllib/CdmwFTGroupRepository.skel.hpp>

//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{

namespace FT
{
// Forward declaration
class FTManager_impl;
//
// IDL:thalesgroup.com/CdmwFT/Location:1.0
//
namespace Location
{

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository:1.0
//
class PrimaryBackupGroupRepository_impl : virtual public CdmwFT::Location::PrimaryBackupGroupRepository, 
                                          virtual public GroupRepository_impl,
                                          virtual public Cdmw::OrbSupport::RefCountLocalObject
{

public:
    friend class PrimaryBackupAdmin_impl;
    friend class Cdmw::FT::FTManager_impl;
    /**
    * Purpose:
    * <p>  
    *  Return the single instance of the PrimaryBackupGroupRepository_impl 
    *
    *@param  none
    *@exception none
    */ 
    static PrimaryBackupGroupRepository_impl* Get_instance()
        throw();

    ~PrimaryBackupGroupRepository_impl();

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
    PrimaryBackupGroupRepository_impl();

    /**
    * Purpose:
    * <p> This operation adds the new primary member reference with its group id in the map.
    * 
    *@param object_group_id the new primary group_id.
    *@param new_primary_member the new primary member reference.
    *
    *@return void.
    *
    *@exception none
    */     
    void add_primary(::FT::ObjectGroupId object_group_id, 
        CORBA::Object_ptr new_primary_member)
        throw();
    
    /**
    * Purpose:
    * <p> This operation removes the old primary member reference from the map.
    * 
    *@param object_group_id the primary group_id to be removed.
    *
    *@return void.
    *
    *@exception ::FT::MemberNotFound is raised if the member is not in the map.
    */     
    void remove_primary(::FT::ObjectGroupId object_group_id)
        throw(::FT::MemberNotFound);


private:
    PrimaryBackupGroupRepository_impl(const PrimaryBackupGroupRepository_impl&);
    void operator=(const PrimaryBackupGroupRepository_impl&);

    // list of primary group member reference ordered by their membership group_id 
    typedef std::map< ::FT::ObjectGroupId, CORBA::Object_var, std::less< ::FT::ObjectGroupId> > PrimaryMap;
    PrimaryMap  m_primary_members;  

    // singleton instance
    static PrimaryBackupGroupRepository_impl* mp_primary_backup_instance;  

    // Mutex protecting m_primary_members manipulation
	 Cdmw::OsSupport::Mutex                   m_mutex;

};

} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_LOCATION_PRIMARY_BACKUP_GROUP_REPOSITORY_IMPL_HPP

