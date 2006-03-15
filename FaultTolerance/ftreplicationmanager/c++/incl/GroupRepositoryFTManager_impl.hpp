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


#ifndef INCL_CDMW_FT_LOCATION_GROUP_REPOSITORY_FTMANAGER_IMPL_HPP
#define INCL_CDMW_FT_LOCATION_GROUP_REPOSITORY_FTMANAGER_IMPL_HPP

// Standard Files
#include <map>

// Cdmw Files
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>


#include <FaultTolerance/idllib/CdmwFTGroupRepository.skel.hpp>
#include <FaultTolerance/idllib/CdmwFTManager.stub.hpp>
//#include "ftmanager/FTManager_impl.hpp"

namespace Cdmw
{
namespace FT
{
namespace ReplicationManager
{

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository:1.0
//
class GroupRepositoryFTManager_impl : virtual public CdmwFT::Location::GroupRepository, 
                                      virtual public Cdmw::OrbSupport::RefCountLocalObject
{

public:
    /**
    * Purpose:
    * <p>  
    *  Return the single instance of the GroupRepository_impl 
    *
    *@param  none
    *@exception none
    */ 
    static GroupRepositoryFTManager_impl* Get_instance()
        throw();

    ~GroupRepositoryFTManager_impl();

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
    // IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/the_location:1.0
    //
    virtual ::FT::Location* the_location()
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/the_slocation:1.0
    //
    virtual char* the_slocation()
        throw(CORBA::SystemException);


    //
    // IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/the_replication_manager:1.0
    //
    CdmwFT::ReplicationManager_ptr  the_replication_manager()
        throw(CORBA::SystemException);

    // init the GroupRepository, only available to friend class MembersAdmin_impl
    // and PrimaryBackupAdmin_impl
    /**
    * Purpose:
    * <p> This operation accepts a ::FT::Location and init the GroupRepository.
    * 
    *@param the_location the ::FT::Location where the GroupRepository is instanciated.
    *
    *@return void.
    *
    *@exception CORBA::BAD_PARAM with BAD_PARAMInvalidLocation Indicates that the location
    * is invalid (length 0).
    */ 
    void init(CORBA::ORB_ptr orb, const ::FT::Location& the_location, ::CdmwFT::FTManager_ptr ft_manager)
        throw (CORBA::SystemException);


protected:
    // Default constructor
    GroupRepositoryFTManager_impl();



    // singleton instance
    static GroupRepositoryFTManager_impl* mp_instance;  


private:
    // hide copy constructor
    GroupRepositoryFTManager_impl(const GroupRepositoryFTManager_impl&);
    void operator=(const GroupRepositoryFTManager_impl&);


    /*
     * Attributes
     */
    // the orb
    CORBA::ORB_var m_orb;

    // location where group_repository is instanciated
    std::string   m_location;

    // set to true when init operation has been called with success
    bool          m_is_initialised;

protected:
    // the fault tolerance manager
	::CdmwFT::FTManager_var m_ft_manager;
};


} // End of namespace ReplicationManager

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_LOCATION_GROUP_REPOSITORY_FTMANAGER_IMPL_HPP

