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


#ifndef INCL_OSSUPPORT_THREADOBSERVER_HPP
#define INCL_OSSUPPORT_THREADOBSERVER_HPP

namespace Cdmw
{

    namespace OsSupport
    {


        /**
        *Purpose:
        *<p> This class allows to be notified at each step
        * of the life of a Thread
        */

        class ThreadObserver
        {

            public:

                /**
                * Purpose:
                * <p> Call when a new thread is started
                * 
                */
                virtual
                void
                onEnterRun()
                {}




                /**
                * Purpose:
                * <p> Call when a new thread exits
                * 
                */
                virtual
                void
                onLeaveRun()
                {}

                virtual
                ~ThreadObserver()
                {}

            protected:



            private:


        };




    } // End namespace osthreads

} // End namespace Cdmw



#endif // INCL_OSSUPPORT_THREADOBSERVER_HPP

