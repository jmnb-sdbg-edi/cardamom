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


#ifndef INCL_CDMW_FT_LOCATION_GROUP_INFO_HPP 
#define INCL_CDMW_FT_LOCATION_GROUP_INFO_HPP 


// Standard Files
#include <map>

// Cdmw Files
#include <idllib/CdmwFTCommon.stub.hpp>
#include <idllib/FT.stub.hpp>

//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{

namespace FT
{


namespace Location
{

class LocationNotFound {};    
    
    
//
// This class contains information related to a group at a given Location
//
class LocationGroupInfo
{
public:

    LocationGroupInfo(const CdmwFT::GroupInfo& group_info,
                      const ::FT::Location&     the_location)
                      throw (::FT::ObjectGroupNotFound);

    ~LocationGroupInfo();

    ::FT::ObjectGroup_ptr get_object_group_ref() const;

    CORBA::Object_ptr get_local_object_ref() const;

    ::FT::ObjectGroupRefVersion get_group_ref_version() const;

	 /* FNO_JE added code
	 CdmwFT::StateTransfer::DataStoreInfos* get_data_store_infos() const;
	 */

    CdmwFT::MemberInfos* get_other_group_members() const;

    CdmwFT::MemberInfo* get_member_info(const std::string& location) const
        throw (LocationNotFound);  
    
  //std::list<std::string> get_locations() const;
    void get_locations(std::list<std::string>& par) const;

    // compare this LocationGroupInfo with the group_info argument,
    // and return the list of locations which are this LocationGroupInfo and
    // not into the group_info argument
    std::list<std::string> get_surplus_locations(const LocationGroupInfo& group_info) const;
    
    bool contains_location(const std::string location) const;
    
private:
    // hide default constructor and copy constructor
    LocationGroupInfo();
    LocationGroupInfo(const LocationGroupInfo&);
    void operator=(const LocationGroupInfo&);
   
    std::string                   m_the_location;      // the location of this group info
    ::FT::ObjectGroup_var         m_object_group_ref;  // group reference
    CORBA::Object_var             m_local_object_ref;  // local member reference
    ::FT::ObjectGroupRefVersion   m_group_ref_version; // group version
	 /* FNO_JE added code
	::CdmwFT::StateTransfer::DataStoreInfos      m_data_store_infos;  // list of data store info
	*/

    typedef std::map< std::string, ::CdmwFT::MemberInfo > MemberInfoMap;
    MemberInfoMap m_other_group_members;  // map of other member indexed by their location
    
};


} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_LOCATION_GROUP_INFO_HPP 

