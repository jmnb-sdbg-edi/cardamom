/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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

#ifndef INCL_TEST_LB_INIT_HPP
#define INCL_TEST_LB_INIT_HPP

#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/testutils/Testable.hpp>

#include <LoadBalancing/idllib/PortableGroup.stub.hpp>
#include <LoadBalancing/idllib/CdmwLBGroupManager.stub.hpp>
#include <loadbalancingclient4/TestHello.stub.hpp>


namespace Cdmw {
namespace LB {

class TestLBInit: virtual public Cdmw::TestUtils::Testable
{
public:
    // constructor
    TestLBInit(CORBA::ORB_ptr                  orb,
               PortableServer::POA_ptr         rootPOA,
               CdmwLB::LBGroupManager_ptr  group_manager,
               const std::string&              name,
               std::string host_name1,
               std::string host_name2,
	       std::string host_name3);
    

    // destructor
    virtual ~TestLBInit();

protected:
    // do_tests
    virtual void do_tests();

private:
    TestLBInit();
    TestLBInit(const TestLBInit&);
    void operator=(const TestLBInit&);

    CORBA::ORB_var                  m_orb;
    PortableServer::POA_var         m_rootPOA;
    CdmwLB::LBGroupManager_var m_group_manager;
    
    std::string m_host_name1;
    std::string m_host_name2;
    std::string m_host_name3;
    
}; // end class 

}; // namespace LB

}; // namespace Cdmw

#endif // INCL_TEST_LB_INIT_HPP

