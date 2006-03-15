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


#ifndef INCL_TEST_MEMBER_ADMIN_HPP
#define INCL_TEST_MEMBER_ADMIN_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/RefCountServantBase.hpp"
#include <idllib/CdmwFTMembersAdmin.skel.hpp>

namespace Cdmw {
namespace FT {

    class TestMemberAdmin : virtual public POA_CdmwFT::Location::MembersAdmin,
                            public virtual Cdmw::OrbSupport::RefCountServantBase
{
public:
    // constructor
    TestMemberAdmin(PortableServer::POA_ptr   poa, 
                    const ::FT::Location&     the_location);
    
    // destructor
    virtual ~TestMemberAdmin();
    
    
    virtual PortableServer::POA_ptr _default_POA();

    //
    // IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/update_group_view:1.0
    //
    virtual void update_group_view(const CdmwFT::GroupInfo& group_info)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/MembersAdmin/the_location:1.0
    //
    virtual ::FT::Location* the_location()
        throw(CORBA::SystemException);
    
protected:
    
private:
    PortableServer::POA_var m_poa;
    
    // location where members admin is running
    std::string             m_location;
};
}; // namespace FT
}; // namespace Cdmw

#endif //INCL_TEST_FAULT_NOTIFIER_HPP

