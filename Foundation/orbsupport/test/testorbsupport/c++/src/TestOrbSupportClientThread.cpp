/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include "testorbsupport/TestOrbSupportClientThread.hpp"


namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        TestOrbSupportClientThread::TestOrbSupportClientThread(
            CdmwTest::TestOrbSupportServer_var server,
            const std::string& request_name )
                : m_server( server ),
                m_result(0),
                m_request_name(request_name)
        {
        }


        TestOrbSupportClientThread::~TestOrbSupportClientThread()
        throw()
        {
        }

        void
        TestOrbSupportClientThread::run()
        throw()
        {
            if ( m_request_name == "slow" )
            {
                m_result = m_server->compute_square_slowly(5);
            }
            else
            {
                m_result = m_server->compute_square(5);
            }

        }

        CORBA::Long
        TestOrbSupportClientThread::get_result()
        {
            return m_result;
        }


    } // End namespace OrbSupport
} // End namespace Cdmw

