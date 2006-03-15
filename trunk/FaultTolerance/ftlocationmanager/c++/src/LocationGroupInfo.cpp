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
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include <ftlocationmanager/LocationGroupInfo.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>


namespace Cdmw
{

namespace FT
{

namespace Location
{

LocationGroupInfo::LocationGroupInfo(const CdmwFT::GroupInfo& group_info,
                                     const ::FT::Location&     the_location)
    throw (::FT::ObjectGroupNotFound)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    using namespace Cdmw::CommonSvcs::Naming;
    // read Group info, extract info and save it 
    m_object_group_ref = group_info.object_group_ref;
    m_group_ref_version = group_info.object_group_ref_version;
    m_local_object_ref = CORBA::Object::_nil();
    m_the_location = NamingInterface::to_string(the_location);
    
    // find local object ref and build other_members map
    for (unsigned long i=0; i<group_info.members.length(); i++)
    {

        if (NamingInterface::to_string(group_info.members[i].the_location) == m_the_location)
        {
            // current member is the local member
            m_local_object_ref = group_info.members[i].the_reference;
				/* FNO_JE added code
            m_data_store_infos = group_info.members[i].data_store_infos;
				*/
        }
        else
        {
            // current member is an other object group
            CdmwFT::MemberInfo info;
            info.the_reference = group_info.members[i].the_reference;
            info.the_location = group_info.members[i].the_location;
				/* FNO_JE added code
            info.data_store_infos = group_info.members[i].data_store_infos;
				*/
            std::string location_str = NamingInterface::to_string(info.the_location);
            m_other_group_members[location_str] = info;
        }
    }

    if (CORBA::is_nil(m_local_object_ref.in()))
    {
        // no local object found, throw ObjectGroupNotFound
        ::FT::ObjectGroupNotFound ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
}

LocationGroupInfo::~LocationGroupInfo()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

::FT::ObjectGroup_ptr 
LocationGroupInfo::get_object_group_ref() const
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    return ::FT::ObjectGroup::_duplicate(m_object_group_ref.in());
}

CORBA::Object_ptr 
LocationGroupInfo::get_local_object_ref() const 
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    return CORBA::Object::_duplicate(m_local_object_ref.in());
}

::FT::ObjectGroupRefVersion 
LocationGroupInfo::get_group_ref_version() const
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_LOG_FUNCTION_RETURN(m_group_ref_version);
    return m_group_ref_version;
}

CdmwFT::MemberInfos* 
LocationGroupInfo::get_other_group_members() const
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CdmwFT::MemberInfos_var other_members 
        = new CdmwFT::MemberInfos();
    other_members->length(m_other_group_members.size());

    CORBA::ULong i = 0;
    for (MemberInfoMap::const_iterator iter = m_other_group_members.begin(); 
         iter != m_other_group_members.end(); 
         iter++)
    {
        other_members[i].the_reference = iter->second.the_reference;
        other_members[i].the_location = iter->second.the_location;
		  /* FNO_JE added code
        other_members[i].data_store_infos = iter->second.data_store_infos;
		  */
        i++;
    }
    return other_members._retn();
}

CdmwFT::MemberInfo*
LocationGroupInfo::get_member_info(const std::string& location) const
    throw (LocationNotFound)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location="<<location);
    CdmwFT::MemberInfo_var member_info;
    if (location == m_the_location) {
        member_info = new CdmwFT::MemberInfo();
        member_info->the_reference = CORBA::Object::_duplicate(m_local_object_ref.in());
        ::FT::Location_var loc = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(m_the_location);
        member_info->the_location = loc.in();
		  /* FNO_JE added code
        member_info->data_store_infos = m_data_store_infos;
		  */
    } else {
        MemberInfoMap::const_iterator iter = m_other_group_members.find(location);
        if (iter == m_other_group_members.end()) {
            throw LocationNotFound();
        }
        member_info = new CdmwFT::MemberInfo(iter->second);
    }
    return member_info._retn();
}

bool
LocationGroupInfo::contains_location(const std::string location) const
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location="<<location);
    MemberInfoMap::const_iterator iter = m_other_group_members.find(location);
    bool result = (iter != m_other_group_members.end());
    CDMW_LOG_FUNCTION_RETURN(result);
    return result;
}


void
LocationGroupInfo::get_locations(std::list<std::string>& locations) const  
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
  //  std::list<std::string> locations;
    
    MemberInfoMap::const_iterator iter;
//    MemberInfoMap::iterator iter;
    for (iter = m_other_group_members.begin();
         iter != m_other_group_members.end();
         ++iter) 
    {
        locations.push_back(iter->first);
    }

    return;
}

std::list<std::string> 
LocationGroupInfo::get_surplus_locations(const LocationGroupInfo& group_info) const
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    std::list<std::string> surplus_locations;
    
    MemberInfoMap::const_iterator iter;
//    MemberInfoMap::iterator iter;
    for (iter = m_other_group_members.begin();
         iter != m_other_group_members.end();
         ++iter) 
    {
        if (! group_info.contains_location(iter->first)) {
            surplus_locations.push_back(iter->first);
        }
    }

    return surplus_locations;
}






} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

