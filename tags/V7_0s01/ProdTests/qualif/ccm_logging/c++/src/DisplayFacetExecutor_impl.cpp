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

#include "ccm_logging/DisplayFacetExecutor_impl.hpp"


namespace Hello
{
    /**
     * IDL:thalesgroup.com/Hello/CCM_Display:1.0
     */


    /**
     * Default constructor.
     */
    DisplayFacetExecutor_impl::DisplayFacetExecutor_impl()
            : m_logger(Cdmw::Logging::Logger::Get_Logger("CCM_Display",
                                                         "QUALIF_CCM_LOGGING"))
    {
    }


    /**
     * Destructor.
     */
    DisplayFacetExecutor_impl::~DisplayFacetExecutor_impl()
    {
        CDMW_INFO(m_logger, "DisplayFacetExecutor_impl::~DisplayFacetExecutor_impl() called.");

        delete m_logger;
    }


    /**
     * IDL:thalesgroup.com/Hello/Display/print:1.0
     */
    void
    DisplayFacetExecutor_impl::display_hello()
        throw(CORBA::SystemException)
    {
        CDMW_INFO(m_logger, "DisplayFacetExecutor_impl::display_hello() called.");

        using namespace std;

        cout << endl << "<------------------------------>"
             << endl << "<############ HELLO ###########>"
             << endl << "<------------------------------>" << endl << endl;
    }
}; // namespace Hello
