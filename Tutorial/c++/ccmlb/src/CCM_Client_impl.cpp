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


#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <CCM_Client_impl.hpp>

//
// IDL:thalesgroup.com/CcmLbTest/CCM_Client:1.0
//
CcmLbTest::CCM_Client_impl::CCM_Client_impl()
    : m_session_context(CcmLbTest::CCM_Client_Context::_nil()),
      m_testThread()
{
    DEBUG_DUMP( "   ... CCM_Client_impl constructor called  ");
}


CcmLbTest::CCM_Client_impl::~CCM_Client_impl()
{
    DEBUG_DUMP( "   ... CCM_Client_impl destructor called  " );
}



//
// set_session_context() sets the session context of the component
// (called by the container)
//
void
CcmLbTest::CCM_Client_impl::set_session_context(
        Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    DEBUG_DUMP( "   ... CCM_Client_impl::set_session_context() called" );

    m_session_context = ::CcmLbTest::CCM_Client_Context::_narrow(ctx);
    m_testThread.set_context(m_session_context.in());
}


//
// ccm_activate() activates the component (called by the container)
//
void
CcmLbTest::CCM_Client_impl::ccm_activate()
   throw(CORBA::SystemException)
{
    DEBUG_DUMP("   ... CCM_Client_impl::ccm_activate() called" );
    m_testThread.start();
}


//
// ccm_passivate() stops the component (called by the container)
//
void
CcmLbTest::CCM_Client_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
    DEBUG_DUMP("   ... CCM_Client_impl::ccm_passivate() called");
}


//
// ccm_remove() destroys the component (called by the container)
//
void
CcmLbTest::CCM_Client_impl::ccm_remove()
   throw(CORBA::SystemException)
{
    DEBUG_DUMP("   ... CCM_Client_impl::ccm_remove() called");
}



