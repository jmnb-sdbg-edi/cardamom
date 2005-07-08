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


#ifndef INCL_ORBSUPPORT_TEST_JTC_ADOPTION_HPP
#define INCL_ORBSUPPORT_TEST_JTC_ADOPTION_HPP

#include "testorbsupport/Thread.hpp"

namespace Cdmw
{

    namespace OrbSupport
    {


        class TestJTCAdoption : public TestOrbSupport::Thread
        {

            public:
                TestJTCAdoption() throw ();

                ~TestJTCAdoption() throw();

                bool registration_ok() const throw()
                {
                    return m_registration_ok;
                }

                bool unregistration_ok() const throw()
                {
                    return m_unregistration_ok;
                }

            protected:
                void
                run() throw();

            private:
                bool m_registration_ok;
                bool m_unregistration_ok;
        };

    } // End namespace OrbSupport

} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_TEST_JTC_ADOPTION_HPP

