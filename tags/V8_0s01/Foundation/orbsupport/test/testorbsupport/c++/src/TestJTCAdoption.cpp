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


#if CDMW_ORB_VDR == orbacus

#include "Foundation/testutils/Testable.hpp"
#include "testorbsupport/TestJTCAdoption.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"
#include <JTC/JTC.h>


namespace Cdmw
{

    namespace OrbSupport
    {

        //----------------------------------------------------------------------------
        TestJTCAdoption::TestJTCAdoption()
        throw ()
                : m_registration_ok(false),
                m_unregistration_ok(false)
        {}

        //----------------------------------------------------------------------------
        TestJTCAdoption::~TestJTCAdoption() throw()
        {}

        //----------------------------------------------------------------------------
        void TestJTCAdoption::run() throw()
        {

            int timescale = Cdmw::TestUtils::Testable::get_timescale();

            // I. Is current thread registered?

            try
            {
                //
                // Get a pointer to the current thread.
                //
                JTCThreadHandle thr = JTCThread::currentThread();
                // PB!
                return ;
            }
            catch (const JTCUnknownThreadException &)
            {
                // OK.
            }

            {
                // This may throw OutOfMemoryException or InternalErrorException
                // but since this is only a test program ...
                ThreadAdoptionToken_var token = OrbSupport::adopt_thread();
                // II. Is current thread registered?

                try
                {
                    //
                    // Get a pointer to the current thread.
                    //
                    JTCThreadHandle thr = JTCThread::currentThread();
                    // OK
                    m_registration_ok = true;
                }
                catch (const JTCUnknownThreadException &)
                {
                    // PB!
                    return ;
                }

                // Sleep for half a second
                Cdmw::OsSupport::OS::sleep(timescale*500);
            } // This will destroy token

            // Sleep for half a second
            Cdmw::OsSupport::OS::sleep(timescale*500);

            // III. Is current thread registered?
            try
            {
                //
                // Get a pointer to the current thread.
                //
                JTCThreadHandle thr = JTCThread::currentThread();
                // PB!
                return ;
            }
            catch (const JTCUnknownThreadException &)
            {
                // OK.
                m_unregistration_ok = true;
            }
        }

    } // namespace OrbSupport
} // namespace Cdmw



#endif // CDMW_ORB_VDR == orbacus

