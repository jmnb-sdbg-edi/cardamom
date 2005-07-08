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


#include "testorbsupport/TestOrbSupportServer_impl.hpp"

#include <iostream>

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

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


        TestOrbSupportServer_impl::TestOrbSupportServer_impl()
        throw()
                : m_nbThreads(0)
        {
        }



        TestOrbSupportServer_impl::TestOrbSupportServer_impl(const TestOrbSupportServer_impl& rhs)
        throw()
        {
        }



        TestOrbSupportServer_impl&
        TestOrbSupportServer_impl::operator=(const TestOrbSupportServer_impl& rhs)
        throw()
        {

            return * this;
        }



        TestOrbSupportServer_impl::~TestOrbSupportServer_impl()
        throw()
        {
        }


        //
        // Compute the squared value
        //
        CORBA::Long
        TestOrbSupportServer_impl::compute_square(CORBA::Short value)
        throw( CORBA::SystemException )
        {
            return value*value;
        }



        //
        // Compute the squared value slowly
        //
        CORBA::Long
        TestOrbSupportServer_impl::compute_square_slowly(CORBA::Short value)
        throw( CORBA::SystemException )
        {
            {
                CDMW_MUTEX_GUARD(m_mutex);
                m_nbThreads++;
            }

            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            OsSupport::OS::sleep(timescale*10000);

            {
                CDMW_MUTEX_GUARD(m_mutex);
                m_nbThreads--;

            }

            return value*value;
        }


        //
        // Compute the squared value slowly
        //
        CORBA::Long
        TestOrbSupportServer_impl::get_number_of_threads()
        throw( CORBA::SystemException )
        {

            return m_nbThreads;
        }


    } // End namespace OrbSupport
} // End namespace Cdmw

