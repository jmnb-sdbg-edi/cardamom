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

#include <iostream>

// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <faulttoleranceserver/TestPrimaryBackupAdmin_impl.hpp>


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
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupAdmin:1.0
//
TestPrimaryBackupAdmin_impl::TestPrimaryBackupAdmin_impl(PortableServer::POA_ptr   poa, 
                                                         const ::FT::Location&     the_location)
    :  TestMembersAdmin_impl( poa, the_location),
       m_poa(PortableServer::POA::_duplicate(poa)),
       m_location(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location))

{
}

TestPrimaryBackupAdmin_impl::~TestPrimaryBackupAdmin_impl()
{
}


//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupAdmin/update_primary:1.0
//
void
TestPrimaryBackupAdmin_impl::update_primary(const CdmwFT::GroupInfo& group_info,
                                        const ::FT::Location& primary_location)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    std::cout<<"******** TestPrimaryBackupAdmin_impl::update_primary ******** "<<std::endl;

}


} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

