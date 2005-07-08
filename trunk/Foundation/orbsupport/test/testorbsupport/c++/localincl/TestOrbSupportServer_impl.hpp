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


#ifndef INCL_ORBSUPPORT_TESTORBSUPPORTSERVER_IMPL_HPP
#define INCL_ORBSUPPORT_TESTORBSUPPORTSERVER_IMPL_HPP

#include "Foundation/orbsupport/CORBA.hpp"

#include "testorbsupport/TestOrbSupportServer.skel.hpp"
#include "Foundation/osthreads/Mutex.hpp"

namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Purpose:
        *<p>    [class description]
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class TestOrbSupportServer_impl :
                    public virtual POA_CdmwTest::TestOrbSupportServer
        {

            public:

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TestOrbSupportServer_impl()
                throw();

                /**
                * Purpose:
                * <p>  [Copy constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TestOrbSupportServer_impl(const TestOrbSupportServer_impl& rhs)
                throw();


                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~TestOrbSupportServer_impl()
                throw();


                /**
                * Purpose:
                * <p> [ Assignement operator description if necessary]
                * 
                *@param
                *@return
                *@exception
                */
                TestOrbSupportServer_impl&
                operator=(const TestOrbSupportServer_impl& rhs)
                throw();



                virtual
                CORBA::Long
                compute_square(CORBA::Short value)
                throw(CORBA::SystemException);


                virtual
                CORBA::Long
                compute_square_slowly(CORBA::Short value)
                throw(CORBA::SystemException);


                virtual
                CORBA::Long
                get_number_of_threads()
                throw(CORBA::SystemException);


                /**
                * Purpose:
                * <p>
                * 
                *@param
                *@return
                *@exception
                */


                /**
                *[Attribute description]
                */


            protected:


            private:

                OsSupport::Mutex m_mutex;
                int m_nbThreads;

        }

        ; // End class TestOrbSupportServer_impl

    } // End namespace OrbSupport
} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_TESTORBSUPPORTSERVER_IMPL_HPP

