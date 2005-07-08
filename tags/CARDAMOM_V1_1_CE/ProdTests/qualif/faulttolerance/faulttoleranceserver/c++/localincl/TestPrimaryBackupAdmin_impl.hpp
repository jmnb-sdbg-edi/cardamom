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

#ifndef INCL_CDMW_TEST_PRIMARY_BACKUP_ADMIN_IMPL_HPP
#define INCL_CDMW_TEST_PRIMARY_BACKUP_ADMIN_IMPL_HPP

// Cdmw Files
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <FaultTolerance/idllib/CdmwFTMembersAdmin.skel.hpp>
#include "faulttoleranceserver/TestMembersAdmin_impl.hpp"

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
class TestPrimaryBackupAdmin_impl : virtual public POA_CdmwFT::Location::PrimaryBackupAdmin,
                                virtual public TestMembersAdmin_impl,
                                virtual public Cdmw::OrbSupport::RefCountServantBase 
{

public:

    TestPrimaryBackupAdmin_impl(PortableServer::POA_ptr   poa, 
                      const ::FT::Location&     the_location);
    ~TestPrimaryBackupAdmin_impl();

    //
    // IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupAdmin/update_primary:1.0
    //
    virtual void update_primary(const CdmwFT::GroupInfo& group_info,
                                const ::FT::Location& primary_location)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);


private:

    PortableServer::POA_var m_poa;

    // location where members admin is running
    std::string             m_location;




};

} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

#endif  // INCL_CDMW_TEST_PRIMARY_BACKUP_ADMIN_IMPL_HPP

