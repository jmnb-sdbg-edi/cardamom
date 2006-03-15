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


#ifndef INCL_ORBSUPPORT_POA_THREADING_POLICY_HPP
#define INCL_ORBSUPPORT_POA_THREADING_POLICY_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include <iostream>

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{

    namespace OrbSupport
    {

        /**
         *Purpose:
         *<p>    Pure base class for all Threading Policies
         *
         *Features:
         *<p> [Thread safety, exception safety, performance]
         *    [if no pertinent write none ]
         *
         *@see  [if necessary] 
         *@link [if necessary] 
         *
         */

        class POAThreadingPolicy
        {

            public:
                virtual ~POAThreadingPolicy() throw()
                {}

                ;

                /**
                * Purpose:
                * <p>Appends self to the CDMW Strategy list..
                *
                *@param sl CDMW Strategy List.
                */
                virtual void append_to_StrategyList(StrategyList & sl) const
                throw() = 0;

                /**
                * Purpose:
                * <p>Dumps self to an output stream.
                *
                *@param os Output stream.
                */
                virtual void print(std::ostream & os) const throw() = 0;
        };

        /**
         *Purpose:
         *<p> Single Thread Strategy.
         *
         */

        class SingleThreadPolicy : public POAThreadingPolicy
        {

            public:
                void append_to_StrategyList(StrategyList &) const
                throw();
                void print(std::ostream &) const throw();
        };


        /**
         *Purpose:
         *<p> Base class for Multithread strategies.
         *
         */

        class MultiThreadPolicy : public POAThreadingPolicy
            {}

        ;

        /**
         *Purpose:
         *<p> Multithread strategies supporting ThreadPool policy.
         *
         */

        class ThreadPoolPolicy : public MultiThreadPolicy
        {

            public:
                ThreadPoolPolicy(size_t) throw();

                ThreadPoolPolicy(const ThreadPoolPolicy &) throw();

                ThreadPoolPolicy&
                operator=(const ThreadPoolPolicy& rhs)
                throw();

                /**
                 * Purpose:
                 * <p> Returns the size of the Thread Pool
                 *
                 *@return the size
                 */
                size_t get_pool_size() const throw();
                void print(std::ostream &) const throw();
                void append_to_StrategyList(StrategyList &) const
                throw();

            private:
                ThreadPoolPolicy() throw();

                size_t m_thread_pool_size;
        };

        /**
         *Purpose:
         *<p> Multithread strategies supporting ThreadPerRequest policy.
         *
         */

        class ThreadPerRequestPolicy : public MultiThreadPolicy
        {

            public:
                void print(std::ostream &) const throw();
                void append_to_StrategyList(StrategyList &) const
                throw();
        };

        /**
         *Purpose:
         *<p> Multithread strategies supporting ThreadPerConnection policy.
         *
         */

        class ThreadPerConnectionPolicy : public MultiThreadPolicy
        {

            public:
                void print(std::ostream &) const throw();
                void append_to_StrategyList(StrategyList &) const
                throw();
        };




    }

    ; // End namespace OrbSupport
}

; // End namespace Cdmw
#endif // INCL_ORBSUPPORT_POA_THREADING_POLICY_HPP

