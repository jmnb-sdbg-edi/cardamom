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


#include <iostream>
#include <ccm_ft/CCM_ServerWithFacet_impl.hpp>

#include <Foundation/ossupport/OS.hpp>


//
// IDL:thalesgroup.com/CcmFtTest/CCM_ServerWithFacet:1.0
//
CcmFtTest::CCM_ServerWithFacet_impl::CCM_ServerWithFacet_impl()
{
   m_expectedClientHostname = CORBA::string_dup("");
}

CcmFtTest::CCM_ServerWithFacet_impl::~CCM_ServerWithFacet_impl()
{
    std::cout << "   ... CCM_ServerWithFacet_impl::~CCM_ServerWithFacet_impl called!" << std::endl;
}



//
// attribute expectedClientHostname
//
char*
CcmFtTest::CCM_ServerWithFacet_impl::expectedClientHostname()
    throw(CORBA::SystemException)
{
    return CORBA::string_dup(m_expectedClientHostname);
}

void
CcmFtTest::CCM_ServerWithFacet_impl::expectedClientHostname(const char* a)
    throw(CORBA::SystemException)
{
    m_expectedClientHostname =  CORBA::string_dup(a);
}


//
// facet serverHostInfo
//
CcmFtTest::CCM_HostInfo_ptr 
CcmFtTest::CCM_ServerWithFacet_impl::get_serverHostInfo ()
    throw (CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithFacet_impl::get_serverHostInfo()" << std::endl; 
    return CcmFtTest::CCM_HostInfo::_duplicate(this);
}

char*
CcmFtTest::CCM_ServerWithFacet_impl::get_hostname()
    throw (CORBA::SystemException)
{
    std::string hostname = Cdmw::OsSupport::OS::get_hostname();
    std::cout << "   ... CCM_ServerWithFacet_impl::get_hostname()  " << hostname << std::endl; 
    return CORBA::string_dup(hostname.c_str());
}    


//
// set_session_context function : it fixes the session context of the component (call by the container)
//
void
CcmFtTest::CCM_ServerWithFacet_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    std::cout << "   ... CCM_ServerWithFacet_impl::Set_session_context called!" << std::endl;
   
    m_session_context = ::CcmFtTest::CCM_ServerWithFacet_Context::_narrow(ctx); 
}


//
// ccm_activate function : it actives the component (call by the container)
//
void
CcmFtTest::CCM_ServerWithFacet_impl::ccm_activate()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithFacet_impl::Ccm_activate called!" << std::endl;
}


//
// ccm_passivate function : it makes stop the component (call by the container)
//

void
CcmFtTest::CCM_ServerWithFacet_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithFacet_impl::Ccm_passivate called!" << std::endl;
} 



//
// ccm_remove function : it destroys the component (call by the container)
void
CcmFtTest::CCM_ServerWithFacet_impl::ccm_remove()
   throw(CORBA::SystemException)
{
   std::cout << "   ... CCM_ServerWithFacet_impl::Ccm_remove called!" << std::endl;
}


