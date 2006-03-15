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


#ifndef INCL_GROUP_CREATOR_TESTER_HPP
#define INCL_GROUP_CREATOR_TESTER_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "idllib/FT.stub.hpp"

namespace Cdmw {
namespace FT {
 
class TestGroupCreator : virtual public Cdmw::TestUtils::Testable
{


public:
    TestGroupCreator(CORBA::ORB_ptr                  orb,
                     PortableServer::POA_ptr         rootPOA,
                     const std::string& name);
    
    virtual ~TestGroupCreator();

protected:
    virtual void do_tests();
    
private:
    CORBA::ORB_var                  m_orb;
    PortableServer::POA_var         m_poa;
        

};

}; // namespace FT
}; // namespace Cdmw

#endif //INCL_GROUP_CREATOR_TESTER_HPP

