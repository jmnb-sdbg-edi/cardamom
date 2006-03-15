/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */

// Standard Files
#include <iostream>

// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <faulttoleranceserver/TestMembersAdmin_impl.hpp>


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
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin:1.0
//
TestMembersAdmin_impl::TestMembersAdmin_impl(PortableServer::POA_ptr poa,
                                     const ::FT::Location&   the_location)
    : m_poa(PortableServer::POA::_duplicate(poa)),
      m_location(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location))
{
}

TestMembersAdmin_impl::~TestMembersAdmin_impl()
{
}


//
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/update_group_view:1.0
//
void
TestMembersAdmin_impl::update_group_view(const CdmwFT::GroupInfo& group_info)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{

    std::cout<<"******** TestMembersAdmin_impl::update_group_view ******** "<<std::endl;
    

}

//
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/the_location:1.0
//
::FT::Location*
TestMembersAdmin_impl::the_location()
    throw(CORBA::SystemException)
{
    ::FT::Location* location = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(m_location);
    return location;
}

//
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/the_slocation:1.0
//
char*
TestMembersAdmin_impl::the_slocation()
    throw(CORBA::SystemException)
{
    return CORBA::string_dup(m_location.c_str());
}


//
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/get_data_store_infos:1.0
//
CdmwFT::StateTransfer::DataStoreInfos* 
TestMembersAdmin_impl::get_data_store_infos(const CdmwFT::StateTransfer::DataStoreIDs& dsids)
    throw(CdmwFT::Location::UnknownDataStoreID,
          CORBA::SystemException)
{        
    return NULL;
}              


} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

