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


// Standard Files
#include <iostream>

// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/MembersAdmin_impl.hpp>
#include <FaultTolerance/ftlocationmanager/GroupRepository_impl.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>

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

//
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin:1.0
//
MembersAdmin_impl::MembersAdmin_impl(CORBA::ORB_ptr orb,
                                     PortableServer::POA_ptr poa,
                                     const ::FT::Location&   the_location,
                                     Cdmw::FT::Location::GroupRepository_impl* group_repository,
                                     bool is_ftmanager)
    : m_poa(PortableServer::POA::_duplicate(poa)),
      m_location(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location)),
      m_group_repository(group_repository)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"is_ftmanager="<<is_ftmanager);
    try
    {
        m_group_repository->init(orb, the_location, is_ftmanager);
    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_FATAL(FTLogger::GetLogger(),
                   "[MembersAdmin] ERROR: CORBA System exception raised while initialising GroupRepository! " 
                   << ex );
        CDMW_ASSERT(false);
    }
}

MembersAdmin_impl::~MembersAdmin_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

PortableServer::POA_ptr
MembersAdmin_impl::_default_POA()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    return PortableServer::POA::_duplicate(m_poa.in());
}

//
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/update_group_view:1.0
//
void
MembersAdmin_impl::update_group_view(const CdmwFT::GroupInfo& group_info)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    // advise group_repository to store this information
    m_group_repository->update_group_view(group_info);

}

//
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/the_location:1.0
//
::FT::Location*
MembersAdmin_impl::the_location()
    throw(CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    ::FT::Location* location = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(m_location);
    return location;
}
//
// IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/the_slocation:1.0
//
char*
MembersAdmin_impl::the_slocation()
    throw(CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_LOG_FUNCTION_RETURN(m_location);
    return CORBA::string_dup(m_location.c_str());
}

} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

