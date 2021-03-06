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


#ifndef INCL_OSSUPPORT_TESTCONDITIONTHREAD_HPP
#define INCL_OSSUPPORT_TESTCONDITIONTHREAD_HPP

#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/osthreads/Condition.hpp"

namespace Cdmw
{

    namespace OsSupport
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

        class TestConditionThread : public Thread
        {

            public:

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TestConditionThread(
                    Condition& condition,
                    int& valueToBeChanged );


                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~TestConditionThread()
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
                run()
                throw();



                /**
                *[Attribute description]
                */


            protected:


            private:

                /**
                * Purpose:
                * <p>  [Copy constructor description if necessary]
                * 
                *@param
                *@exception
                */
                TestConditionThread(const TestConditionThread& rhs);

                /**
                * Purpose:
                * <p> [ Assignement operator description if necessary]
                * 
                *@param
                *@return
                *@exception
                */
                TestConditionThread&
                operator=(const TestConditionThread& rhs)
                throw();


                Condition& m_condition;

                int& m_valueToBeChanged;





        }

        ; // End class TestConditionThread

    } // End namespace osthreads
} // End namespace Cdmw

#endif // INCL_OSSUPPORT_TESTCONDITIONTHREAD_HPP

