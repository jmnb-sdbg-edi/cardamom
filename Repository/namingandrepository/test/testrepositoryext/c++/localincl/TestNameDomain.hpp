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


#ifndef INCL_TESTNAMEDOMAIN_HPP
#define INCL_TESTNAMEDOMAIN_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Repository/idllib/CdmwRepositoryExt.stub.hpp"
#include "Foundation/testutils/Testable.hpp"

#include "Tester_impl.hpp"
#include "TesterFactory_impl.hpp"
#include "ObjectFactory_impl.hpp"

class TestNameDomain : virtual public Cdmw::TestUtils::Testable
{

protected:
    PortableServer::POA_ptr m_rootPOA;
    CORBA::ORB_ptr m_ORB;
    CosNaming::NamingContext_var m_orbRootCtx;
    CdmwNamingAndRepository::RepositoryExt_var m_repository;

public:

    Tester_ptr create_Tester(const std::string& name);

    TesterFactory_ptr create_TesterFactory(const std::string& name);

    ObjectFactory_ptr create_ObjectFactory(const std::string& name);

    TestNameDomain(const std::string& name,
        CORBA::ORB_ptr orb,
        CdmwNamingAndRepository::RepositoryExt_ptr repository);

    virtual ~TestNameDomain();

    void display_factories(const CosLifeCycle::Factories& factories);

protected:

	virtual void do_tests();

};

#endif // INCL_TESTNAMEDOMAIN_HPP

