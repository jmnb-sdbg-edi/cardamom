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

#ifndef INCL_TEST_NAVIGATION_HPP
#define INCL_TEST_NAVIGATION_HPP

#include <string>

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/testutils/Testable.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
namespace Cdmw {

class TestNavigation : virtual public Cdmw::TestUtils::Testable
{
public:
    // constructor
    TestNavigation(CORBA::ORB_ptr                  orb,
                   PortableServer::POA_ptr         rootPOA,
                   const std::string&              name,
                   const std::string&              supervision_corbaloc,
                   const std::string&              host1,
                   const std::string&              host2);
    

    // destructor
    virtual ~TestNavigation();

protected:
    // do_tests
    virtual void do_tests();

private:
    TestNavigation();
    TestNavigation(const TestNavigation&);
    void operator=(const TestNavigation&);

    CORBA::ORB_var                  m_orb;
    PortableServer::POA_var         m_poa;
    std::string                     m_supervision;
    std::string                     m_hostname1;
    std::string                     m_hostname2;
    
    CdmwPlatformMngt::System_var m_system;
    CdmwPlatformMngt::Application_var m_application1;
    CdmwPlatformMngt::Application_var m_application2;
    CdmwPlatformMngt::ProcessProxy_var m_process1;
    CdmwPlatformMngt::ProcessProxy_var m_process2;
    CdmwPlatformMngt::HostProxy_var m_host1;
    CdmwPlatformMngt::HostProxy_var m_host2;
    

}; // end class 


}; // namespace Cdmw

#endif // INCL_TEST_NAVIGATION_HPP

