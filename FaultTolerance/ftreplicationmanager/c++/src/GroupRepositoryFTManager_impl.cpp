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
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <ftreplicationmanager/GroupRepositoryFTManager_impl.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>

#include <Foundation/logging/FunctionLogger.hpp>

//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{

namespace FT
{

namespace ReplicationManager
{

GroupRepositoryFTManager_impl* GroupRepositoryFTManager_impl::mp_instance = 0;

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository:1.0
//
GroupRepositoryFTManager_impl* 
GroupRepositoryFTManager_impl::Get_instance()
    throw()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (!mp_instance)
    {
        // create instance
        mp_instance = new GroupRepositoryFTManager_impl();
    }

    return mp_instance;
}

GroupRepositoryFTManager_impl::GroupRepositoryFTManager_impl()
    : m_is_initialised(false)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

GroupRepositoryFTManager_impl::~GroupRepositoryFTManager_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group:1.0
//
::FT::ObjectGroup_ptr
GroupRepositoryFTManager_impl::get_object_group(CORBA::Object_ptr member)
    throw(::FT::MemberNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CORBA::NO_IMPLEMENT ex(Cdmw::OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    CDMW_LOG_FUNCTION_EXCEPTION(ex);
    throw ex;
    ::FT::ObjectGroup_var object_group;
    return object_group._retn();
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_other_group_members:1.0
//
CdmwFT::MemberInfos*
GroupRepositoryFTManager_impl::get_other_group_members(CORBA::Object_ptr member)
    throw(::FT::MemberNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CORBA::NO_IMPLEMENT ex(Cdmw::OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    CDMW_LOG_FUNCTION_EXCEPTION(ex);
    throw ex;
    CdmwFT::MemberInfos_var other_members = 0;
    return other_members._retn();
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group_version:1.0
//
::FT::ObjectGroupRefVersion
GroupRepositoryFTManager_impl::get_object_group_version(CORBA::Object_ptr member)
    throw(::FT::MemberNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CORBA::NO_IMPLEMENT ex(Cdmw::OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    CDMW_LOG_FUNCTION_EXCEPTION(ex);
    throw ex;
    ::FT::ObjectGroupRefVersion group_version = 0;
    return group_version;
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group_from_gid:1.0
//
::FT::ObjectGroup_ptr
GroupRepositoryFTManager_impl::get_object_group_from_gid(::FT::ObjectGroupId object_group_id)
throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    ::FT::ObjectGroup_var object_group = m_ft_manager->get_object_group(object_group_id);
    return object_group._retn();
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_other_group_members_from_gid:1.0
//
CdmwFT::MemberInfos*
GroupRepositoryFTManager_impl::get_other_group_members_from_gid(::FT::ObjectGroupId object_group_id)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    CORBA::NO_IMPLEMENT ex(Cdmw::OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    CDMW_LOG_FUNCTION_EXCEPTION(ex);
    throw ex;
    CdmwFT::MemberInfos_var other_members = 0;
    return other_members._retn();
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/get_object_group_version_from_gid:1.0
//
::FT::ObjectGroupRefVersion
GroupRepositoryFTManager_impl::get_object_group_version_from_gid(::FT::ObjectGroupId object_group_id)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    ::FT::ObjectGroupRefVersion group_version = m_ft_manager->get_object_group_version(object_group_id);
    return group_version;
}

//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/the_location:1.0
//
::FT::Location*
GroupRepositoryFTManager_impl::the_location()
    throw(CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (m_is_initialised == true)
    {
        ::FT::Location* location 
            = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(m_location);
        CDMW_LOG_FUNCTION_RETURN(m_location);
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
GroupRepositoryFTManager_impl::the_slocation()
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
//
// IDL:thalesgroup.com/CdmwFT/Location/GroupRepository/the_replication_manager:1.0
//
CdmwFT::ReplicationManager_ptr
GroupRepositoryFTManager_impl::the_replication_manager()
    throw(CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CORBA::NO_IMPLEMENT ex(Cdmw::OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    CDMW_LOG_FUNCTION_EXCEPTION(ex);
    throw ex;
    CdmwFT::ReplicationManager_var replication_manager;
    return replication_manager._retn();
}

void 
GroupRepositoryFTManager_impl::init(CORBA::ORB_ptr orb,
                           const ::FT::Location& the_location,
                           ::CdmwFT::FTManager_ptr ft_manager)
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
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
    
    m_ft_manager = ::CdmwFT::FTManager::_duplicate(ft_manager);
    // no error during initialisation
    m_is_initialised = true;
}
  
} // End of namespace ReplicationManager

} // End of namespace FT

} // End of namespace Cdmw

