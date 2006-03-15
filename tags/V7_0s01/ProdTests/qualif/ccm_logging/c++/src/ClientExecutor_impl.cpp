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
#include <Foundation/logging/LogManager.hpp>

#include "ccm_logging/ClientExecutor_impl.hpp"


namespace Hello
{

/**
 * IDL:thalesgroup.com/Hello/CCM_Client:1.0
 */


/**
 * Constructor.
 */
ClientExecutor_impl::ClientExecutor_impl()
	: m_session_context(CCM_Client_Context::_nil()),
      m_logger(Cdmw::Logging::Logger::Get_Logger("CCM_Client",
                                                 "QUALIF_CCM_LOGGING"))
{
}


/**
 * Destructor.
 */
ClientExecutor_impl::~ClientExecutor_impl()
{
    CDMW_INFO(m_logger, "ClientExecutor_impl::~ClientExecutor_impl() called.");

    delete m_logger;
}


/**
 * Set the session context of the component (called by the container).
 */
void
ClientExecutor_impl::set_session_context(Components::SessionContext_ptr ctx)
    throw(CORBA::SystemException)
{
    CDMW_INFO(m_logger, "ClientExecutor_impl::set_session_context() called.");

    m_session_context = CCM_Client_Context::_narrow(ctx);
}


/**
 * Activate the component (called by the container).
 */
void
ClientExecutor_impl::ccm_activate()
   throw(CORBA::SystemException)
{
    CDMW_INFO(m_logger, "ClientExecutor_impl::ccm_activate() called.");

    try {
        // get the display connection of the server.
        CDMW_INFO(m_logger, "connect to the display facet...");

        Display_var display =
            m_session_context->get_connection_client_receptacle();

        // call the server display.
        CDMW_INFO(m_logger, "call the server display...");

        display->display_hello();
    }
	catch (...) {
        CDMW_ERROR(m_logger, "unexpected exception raised.");
    }

}


/**
 * Stop the component (called by the container).
 */
void
ClientExecutor_impl::ccm_passivate()
   throw(CORBA::SystemException)
{
    CDMW_INFO(m_logger, "ClientExecutor_impl::ccm_passivate() called.");
}


/**
 * Destroy the component (called by the container).
 */
void
ClientExecutor_impl::ccm_remove()
   throw(CORBA::SystemException)
{
    CDMW_INFO(m_logger, "ClientExecutor_impl::ccm_remove() called.");
}


}; // End of namespace Hello
