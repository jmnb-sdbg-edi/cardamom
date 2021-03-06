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


#ifndef INCL_TEST_CDMW_CCM_TEST_SESSION_CONTAINER_HPP
#define INCL_TEST_CDMW_CCM_TEST_SESSION_CONTAINER_HPP


#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "CCMContainer/idllib/Components.stub.hpp"
#include "CCMContainer/idllib/CdmwDeployment.stub.hpp"

namespace TestCdmwCCM
{

class TestSessionContainer : public Cdmw::TestUtils::Testable
{

public:
    
    /**
     * Purpose:
     * <p> [Constructor description if necessary]
     * 
     *@param
     *@exception
     */ 
    TestSessionContainer(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, std::string process_name)
        throw(CORBA::SystemException);
    
    /**
     * Purpose:
     * <p>  [Copy constructor description if necessary]
     * 
     *@param
     *@exception
     */ 
    TestSessionContainer(const TestSessionContainer& rhs)
        throw(CORBA::SystemException);
    
    
    /**
     * Purpose:
     * <p> [ Destructor description if necessary]
     * 
     */ 
    virtual 
    ~TestSessionContainer()
        throw();
    
    
    /**
     * Purpose:
     * <p> [ Assignement operator description if necessary]
     * 
     *@param
     *@return
     *@exception
     */ 
    TestSessionContainer&
    operator=(const TestSessionContainer& rhs)
        throw(CORBA::SystemException);
    
    
    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    
    
    /**
     *[Attribute description]
     */ 
    
    
protected:
    
    virtual void do_tests();
private:
    Components::ConfigValues*
    create_ConfigValues(CdmwDeployment::ComponentKindValue   component_kind,
                        CdmwDeployment::ServantLifetimeValue servant_lifetime,
                        CdmwDeployment::ThreadingPolicyValue threading_policy,
                        bool                                  with_TraceMessage) const
        throw ();

    
    TestSessionContainer(PortableServer::POA_ptr poa)
        throw();

    CORBA::ORB_var          m_orb;
    PortableServer::POA_var m_poa;
    std::string             m_process_name;
    

}; // End class TestSessionContainer

}; // End namespace TestCdmwCCM
#endif // INCL_TEST_CDMW_CCM_TEST_SESSION_CONTAINER_HPP

