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


#ifndef INCL_ORBSUPPORT_TESTORBSUPPORTSERVERPROCESS_HPP
#define INCL_ORBSUPPORT_TESTORBSUPPORTSERVERPROCESS_HPP


#include "testorbsupport/TestOrbSupportServer_impl.hpp"

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

        class TestOrbSupportServerProcess
        {

            public:

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TestOrbSupportServerProcess();


                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~TestOrbSupportServerProcess()
                throw();


                /**
                * Purpose:
                * <p> Create and run the server
                * 
                */
                void
                start( int& argc, char** argv );


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


                static const int CDMW_PROCESS_PORT = 42536;
                static const int CDMW_LOCALISATION_PORT = 42537;

                // Multi-threaded server is on rootPOA. Its POAManager use CDMW_LOCALISATION_PORT.
                static const int SERVER_MT_PORT = CDMW_LOCALISATION_PORT;

                // Single-threaded server is on CdmwRootPOA. Its POAManager use CDMW_PROCESS_PORT.
                static const int SERVER_ST_PORT = CDMW_PROCESS_PORT;

                static const char* SERVER_MT_CORBALOC_NAME;
                static const char* SERVER_ST_CORBALOC_NAME;



            protected:


            private:

                /**
                * Purpose:
                * <p>  [Copy constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TestOrbSupportServerProcess(const TestOrbSupportServerProcess& rhs)
                throw();


                /**
                * Purpose:
                * <p> [ Assignement operator description if necessary]
                * 
                *@param
                *@return
                *@exception
                */
                TestOrbSupportServerProcess&
                operator=(const TestOrbSupportServerProcess& rhs)
                throw();



        }

        ; // End class TestOrbSupportServerProcess

    } // End namespace OrbSupport
} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_TESTORBSUPPORTSERVERPROCESS_HPP

