/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#include <Foundation/ossupport/OS.hpp>

#include <ccm_lb/CCM_ServerWithFacet_impl.hpp>
#include <SystemMngt/platforminterface/PlatformInterface.hpp>

//
// IDL:thalesgroup.com/CcmLbTest/CCM_ServerWithFacet:1.0
//
CcmLbTest::CCM_ServerWithFacet_impl::CCM_ServerWithFacet_impl()
{
}


CcmLbTest::CCM_ServerWithFacet_impl::~CCM_ServerWithFacet_impl()
{
    std::cout << "   ... CCM_ServerWithFacet_impl destructor called"
              << std::endl;
}


//
// facet serverHostInfo
//
CcmLbTest::CCM_HostInfo_ptr
CcmLbTest::CCM_ServerWithFacet_impl::get_serverHostInfo()
    throw (CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithFacet_impl::get_serverHostInfo()"
              << std::endl;

    return CcmLbTest::CCM_HostInfo::_duplicate(this);
}


char*
CcmLbTest::CCM_ServerWithFacet_impl::get_process_info()
    throw (CORBA::SystemException)
{
  //    std::string hostname = Cdmw::OsSupport::OS::get_hostname();
    // std::string processID = Cdmw::OsSupport::OS::get_processId();
    std::string process_name = Cdmw::PlatformMngt::PlatformInterface::Get_process_name();
    //std::string result = std::string("==>HOST: ") + hostname + std::string("\n==>PROCESS NAME: ") + processID + std::string("\n");
    std::cout << "   ... CCM_ServerWithFacet_impl::get_process_info() returns "
              << process_name << std::endl;

    return CORBA::string_dup(process_name.c_str());
}


//
// set_session_context() sets the session context of the component
// (called by the container)
//
void
CcmLbTest::CCM_ServerWithFacet_impl::set_session_context(
        Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithFacet_impl::set_session_context() called"
              << std::endl;

    m_session_context = ::CcmLbTest::CCM_ServerWithFacet_Context::_narrow(ctx);
}


//
// ccm_activate() activates the component (called by the container)
//
void
CcmLbTest::CCM_ServerWithFacet_impl::ccm_activate()
   throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithFacet_impl::ccm_activate() called"
              << std::endl;
}


//
// ccm_passivate() stops the component (called by the container)
//
void
CcmLbTest::CCM_ServerWithFacet_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithFacet_impl::ccm_passivate() called"
              << std::endl;
}


//
// ccm_remove function : it destroys the component (called by the container)
void
CcmLbTest::CCM_ServerWithFacet_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithFacet_impl::ccm_remove() called"
             << std::endl;
}
