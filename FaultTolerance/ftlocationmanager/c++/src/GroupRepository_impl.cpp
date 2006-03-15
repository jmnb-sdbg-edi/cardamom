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
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/GroupRepository_impl.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreBase.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>

// Local Files
#include <ftlocationmanager/LocationGroupInfo.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>
 
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

GroupRepository_impl* GroupRepository_impl::mp_instance = 0;

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository:1.0
//
GroupRepository_impl* 
GroupRepository_impl::Get_instance()
    throw()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if(mp_instance == 0)
        {
            // acquire a mutex
            CDMW_MUTEX_GUARD(M_mutex);
            
            // double check
            // some thread can to have already initialized it
            if (mp_instance == 0)
                {
                    mp_instance = new GroupRepository_impl();
                }     
        }
    
    return mp_instance;
    
}
  
GroupRepository_impl::GroupRepository_impl()
  : m_is_initialised(false),
    m_is_ftmanager(false)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}
  
GroupRepository_impl::~GroupRepository_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    for (LocationGroupInfoMap::iterator iter = m_group_infos.begin() ; 
         iter != m_group_infos.end() ; iter++)
        {
            LocationGroupInfo* loc_group_info = (*iter).second;
            
            m_group_infos.erase(iter);
            delete loc_group_info;
        }
}
    
//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group:1.0
//
::FT::ObjectGroup_ptr
GroupRepository_impl::get_object_group(CORBA::Object_ptr member)
    throw(::FT::MemberNotFound,
          CORBA::SystemException)
{
    
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    ::FT::ObjectGroup_var object_group = ::FT::ObjectGroup::_nil();

    {
        CDMW_MUTEX_GUARD(m_mutex);
        for (LocationGroupInfoMap::const_iterator iter = m_group_infos.begin() ; 
        iter != m_group_infos.end() ; iter++)
        {
            CORBA::Object_var stored_local_object = (*iter).second->get_local_object_ref();
            if (member->_is_equivalent(stored_local_object.in()))
            {
                // initialise group_ref
                object_group = (*iter).second->get_object_group_ref();
                
                // get out from the loop
                break;
            }
        }
    }


    if (CORBA::is_nil(object_group.in()))
    {
        ::FT::MemberNotFound ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    return object_group._retn();
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_other_group_members:1.0
//
CdmwFT::MemberInfos*
GroupRepository_impl::get_other_group_members(CORBA::Object_ptr member)
    throw(::FT::MemberNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CdmwFT::MemberInfos_var other_members = 0;
    
    {
        CDMW_MUTEX_GUARD(m_mutex);

        for (LocationGroupInfoMap::const_iterator iter = m_group_infos.begin() ; 
             iter != m_group_infos.end() ; iter++)
        {
            CORBA::Object_var stored_local_object = (*iter).second->get_local_object_ref();
            if (member->_is_equivalent(stored_local_object.in()))
            {
                // initialise other_members
                other_members = (*iter).second->get_other_group_members();
                
                // get out from the loop
                break;
            }
        }
    }

    if (other_members == 0)
    {
        ::FT::MemberNotFound ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    return other_members._retn();
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group_version:1.0
//
::FT::ObjectGroupRefVersion
GroupRepository_impl::get_object_group_version(CORBA::Object_ptr member)
    throw(::FT::MemberNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    ::FT::ObjectGroupRefVersion group_version = 0;

    {
        CDMW_MUTEX_GUARD(m_mutex);

        for (LocationGroupInfoMap::const_iterator iter = m_group_infos.begin() ; 
             iter != m_group_infos.end() ; iter++)
        {
            CORBA::Object_var stored_local_object = (*iter).second->get_local_object_ref();
            if (member->_is_equivalent(stored_local_object.in()))
            {
                // initialise other_members
                group_version = (*iter).second->get_group_ref_version();
                
                // get out from the loop
                break;
            }
        }
    }

    if (group_version == 0)
    {
        ::FT::MemberNotFound ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    return group_version;
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group_from_gid:1.0
//
::FT::ObjectGroup_ptr
GroupRepository_impl::get_object_group_from_gid(::FT::ObjectGroupId object_group_id)
throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    ::FT::ObjectGroup_var object_group = ::FT::ObjectGroup::_nil();

    {
        CDMW_MUTEX_GUARD(m_mutex);

        LocationGroupInfoMap::const_iterator iter = m_group_infos.find(object_group_id) ; 
        
        if (iter == m_group_infos.end())
        {
            throw ::FT::ObjectGroupNotFound();
        }
        else
        {
            object_group = (*iter).second->get_object_group_ref();
        }
    }

    std::string object_group_str = m_orb->object_to_string(object_group.in());

    return object_group._retn();
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_other_group_members_from_gid:1.0
//
CdmwFT::MemberInfos*
GroupRepository_impl::get_other_group_members_from_gid(::FT::ObjectGroupId object_group_id)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    CdmwFT::MemberInfos_var other_members = 0;
    
    {
        CDMW_MUTEX_GUARD(m_mutex);

        LocationGroupInfoMap::const_iterator iter = m_group_infos.find(object_group_id) ; 
        
        if (iter == m_group_infos.end())
        {
            throw ::FT::ObjectGroupNotFound();
        }
        else
        {
            other_members = (*iter).second->get_other_group_members();
        }
    }

    if (other_members == 0)
    {
        ::FT::ObjectGroupNotFound ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    return other_members._retn();
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group_version_from_gid:1.0
//
::FT::ObjectGroupRefVersion
GroupRepository_impl::get_object_group_version_from_gid(::FT::ObjectGroupId object_group_id)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    ::FT::ObjectGroupRefVersion group_version = 0;

    {
        CDMW_MUTEX_GUARD(m_mutex);
        
        LocationGroupInfoMap::const_iterator iter = m_group_infos.find(object_group_id) ; 
        
        if (iter == m_group_infos.end())
        {
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        else
        {
            group_version = (*iter).second->get_group_ref_version();
        }
    }

    if (group_version == 0)
    {
        ::FT::ObjectGroupNotFound ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    return group_version;
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/the_replication_manager:1.0
//
CdmwFT::ReplicationManager_ptr
GroupRepository_impl::the_replication_manager()
    throw(CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (m_is_ftmanager == true) {
        CORBA::OBJECT_NOT_EXIST ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    if (m_is_initialised == true)
    {
        CdmwFT::ReplicationManager_var replication_manager 
            = CdmwFT::ReplicationManager::_duplicate(m_replication_manager.in());
        return replication_manager._retn();
    }
    else
    {
        CORBA::BAD_INV_ORDER ex(Cdmw::OrbSupport::BAD_INV_ORDERInitNotCalled,
                                CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/the_location:1.0
//
::FT::Location*
GroupRepository_impl::the_location()
    throw(CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (m_is_initialised == true)
    {
        ::FT::Location* location 
            = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(m_location);
        return location;
    }
    else
    {
        CORBA::BAD_INV_ORDER ex(Cdmw::OrbSupport::BAD_INV_ORDERInitNotCalled,
                                CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
}
//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/the_slocation:1.0
//
char*
GroupRepository_impl::the_slocation()
    throw(CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (m_is_initialised == true)
    {
        CDMW_LOG_FUNCTION_RETURN(m_location);
        return CORBA::string_dup(m_location.c_str());
    }
    else
    {
        CORBA::BAD_INV_ORDER ex(Cdmw::OrbSupport::BAD_INV_ORDERInitNotCalled,
                                CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
}

void
GroupRepository_impl::update_group_view(const CdmwFT::GroupInfo& group_info)
    throw(::FT::ObjectGroupNotFound, CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    ::FT::ObjectGroupId group_id = group_info.object_group_id;

    {
        CDMW_MUTEX_GUARD(m_mutex);

        
        LocationGroupInfoMap::iterator iter = m_group_infos.find(group_id);
        bool group_found = false; // indicate this group is already registered
        
        LocationGroupInfo* old_loc_group_info = NULL;
        
        if (iter != m_group_infos.end())
        {
            // this group is already registered in the map, erase it!
            // it would be replaced if it is an update, 
            // otherwise it is a remove operation.
            old_loc_group_info = (*iter).second;
            
            group_found = true;
            m_group_infos.erase(iter);
        }
        
        // read Group info, extract info and save it in the map
        try
        {
            ::FT::Location* location = this->the_location();
            LocationGroupInfo* loc_group_info 
                = new LocationGroupInfo(group_info, *location);
            
            // add group to the map
            m_group_infos.insert(LocationGroupInfoMap::value_type(group_id, loc_group_info));

            // get locations which have been added or removed (for DataStores update)
            std::list<std::string> removed_locations;
            std::list<std::string> added_locations;

            if (old_loc_group_info == NULL) {
                // all locations are new locations
                //added_locations = loc_group_info->get_locations();
	        loc_group_info->get_locations(added_locations);
            } else {
                // removed locations are the locations that are in surplus
                // in old_loc_group_info compared to loc_group_info
                removed_locations = old_loc_group_info->get_surplus_locations(*loc_group_info);
                
                // added locations are the locations that are in surplus
                // in loc_group_info compared to old_loc_group_info
                added_locations = loc_group_info->get_surplus_locations(*old_loc_group_info);
            }
        }
        catch (const ::FT::ObjectGroupNotFound& ex)
        {
            // this is not an update but a remove order

            if (!group_found)
            {
                // This group has already been deleted or no member
                // has never existed in this location
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            }
        }    

        if (old_loc_group_info != NULL)
            delete old_loc_group_info;
    }
}

void 
GroupRepository_impl::init(CORBA::ORB_ptr orb,
                           const ::FT::Location& the_location,
                           bool is_ftmanager)
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"is_ftmanager="<<is_ftmanager);
    if (m_is_initialised == true)
    {
        CORBA::BAD_INV_ORDER ex(Cdmw::OrbSupport::BAD_INV_ORDERInitAlreadyDone,
                                CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    m_orb = CORBA::ORB::_duplicate(orb);

    try
    {
        m_location = CommonSvcs::Naming::NamingInterface::to_string(the_location);
    }
    catch (const CommonSvcs::Naming::InvalidNameException& )
    {
        CORBA::BAD_PARAM ex(Cdmw::OrbSupport::BAD_PARAMInvalidLocation,
                            CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    if (is_ftmanager != true)
    {        
        try
        {
            CORBA::Object_var obj = orb->resolve_initial_references("ReplicationManager");
            
            m_replication_manager = CdmwFT::ReplicationManager::_narrow(obj.in());
        }
        catch (const CommonSvcs::Naming::InvalidNameException& )
        {
            CORBA::BAD_PARAM ex(Cdmw::OrbSupport::BAD_PARAMInvalidLocation,
                                CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    } 
    else 
    {
        m_replication_manager = CdmwFT::ReplicationManager::_nil();        
        m_is_ftmanager = true;        
    }

    // set current location name to DataStores
    DataStoreBase::Set_Location_Name(m_location);

    // no error during initialisation
    m_is_initialised = true;
}
  

} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

