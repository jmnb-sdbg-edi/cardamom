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

#ifndef INCL_TEST_FT_REPOSITORY_HPP
#define INCL_TEST_FT_REPOSITORY_HPP

#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/testutils/Testable.hpp>




namespace Cdmw {
namespace NamingAndRepository {

class TestFTRepository : virtual public Cdmw::TestUtils::Testable
{
public:
    // constructor
    TestFTRepository(CORBA::ORB_ptr                  orb,
                     PortableServer::POA_ptr         rootPOA,
                     const std::string&              name);
    

    // destructor
    virtual ~TestFTRepository();

protected:
    // do_tests
    virtual void do_tests();

private:
    TestFTRepository();
    TestFTRepository(const TestFTRepository&);
    void operator=(const TestFTRepository&);

    CORBA::ORB_var                  m_orb;
    PortableServer::POA_var         m_rootPOA;
    

}; // end class 

}; // namespace NamingAndRepository

}; // namespace Cdmw

#endif // INCL_TEST_FT_REPOSITORY_HPP

