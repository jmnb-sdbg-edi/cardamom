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

#ifndef INCL_REPOSITORY_INTERFACE_TEST_HPP 
#define INCL_REPOSITORY_INTERFACE_TEST_HPP 

#include <Foundation/testutils/Testable.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <LifeCycle/idllib/CdmwLifeCycle.stub.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

namespace Cdmw
{
namespace NamingAndRepository
{

class RepositoryInterfaceTest: public virtual Cdmw::TestUtils::Testable
{
public:
    
    RepositoryInterfaceTest(int argc, char* argv[])
        throw ();
    
    void init(const char* namingReposURL)
        throw (CORBA::SystemException);
    
    void destroyORB()
        throw ();
    
protected:

    virtual void do_tests();    
    
private:
    
    CORBA::ORB_var m_orb;
    CdmwNamingAndRepository::Repository_var m_repository; 
    
};
 
};  // namespace NamingAndRepository

};  // namespace Cdmw
 
#endif   // INCL_REPOSITORY_INTERFACE_TEST_HPP

