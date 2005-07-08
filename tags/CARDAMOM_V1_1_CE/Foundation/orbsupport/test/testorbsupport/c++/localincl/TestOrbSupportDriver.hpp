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


#ifndef INCL_ORBSUPPORT_TESTORBSUPPORTDRIVER_HPP
#define INCL_ORBSUPPORT_TESTORBSUPPORTDRIVER_HPP

#include <string>

#include "Foundation/testutils/Testable.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "testorbsupport/TestOrbSupportServer.stub.hpp"

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

        class TestOrbSupportDriver : public TestUtils::Testable
        {

            public:

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TestOrbSupportDriver( const std::string& process_name );


                /**
                * Purpose:
                * <p>  [Copy constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TestOrbSupportDriver(const TestOrbSupportDriver& rhs);



                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~TestOrbSupportDriver()
                throw();


                /**
                * Purpose:
                * <p> [ Assignement operator description if necessary]
                * 
                *@param
                *@return
                *@exception
                */
                TestOrbSupportDriver&
                operator=(const TestOrbSupportDriver& rhs)
                throw();




                /**
                * Purpose:
                * <p>
                * 
                *@param
                *@return
                *@exception
                */
                virtual
                void
                do_tests()
                throw();


                virtual
                void
                do_port_number_test( CORBA::ORB_ptr orb );


                virtual
                void
                do_poa_creation_tests( CdmwTest::TestOrbSupportServer_var server );


                virtual
                void
                do_orb_thread_pool_limit_tests( CdmwTest::TestOrbSupportServer_var server );


                virtual
                void
                do_orb_multithread_tests( CdmwTest::TestOrbSupportServer_var server );




                virtual
                void
                do_orb_tests()
                throw();

                /**
                *[Attribute description]
                */


            protected:


            private:
                const std::string m_process_name;

        }

        ; // End class TestOrbSupportDriver

    } // End namespace OrbSupport
} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_TESTORBSUPPORTDRIVER_HPP

