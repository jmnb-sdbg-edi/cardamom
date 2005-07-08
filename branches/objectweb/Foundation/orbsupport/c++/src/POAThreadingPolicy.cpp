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


#include "Foundation/orbsupport/POAThreadingPolicy.hpp"
#include "Foundation/common/Assert.hpp"

namespace
{
    const char* SINGLE_THREAD_DESIGNATION = "Single Thread Policy";
    const char* THREAD_POOL_DESIGNATION = "Thread Pool Policy";
    const char* THREAD_PER_REQUEST_DESIGNATION = "Thread Per Request Policy";
    const char* THREAD_PER_CONNECTION_DESIGNATION = "Thread Per Connection Policy";
}

; // End anonymous namespace

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


        void
        SingleThreadPolicy::print(std::ostream & os) const
        throw()
        {
            try
            {
                os << SINGLE_THREAD_DESIGNATION << std::endl;
            }
            catch (...)
            {
                // do nothing
            }
        }

        void
        SingleThreadPolicy::append_to_StrategyList(StrategyList & s) const
        throw()
        {
            // Do nothing !
        }

        ThreadPoolPolicy::ThreadPoolPolicy(size_t t)
        throw()
                : m_thread_pool_size(t)
        {}

        ThreadPoolPolicy::ThreadPoolPolicy(const ThreadPoolPolicy & other)
        throw()
                : m_thread_pool_size(other.m_thread_pool_size)
        {}

        ThreadPoolPolicy&
        ThreadPoolPolicy::operator=(const ThreadPoolPolicy& rhs)
        throw()
        {
            if (this != &rhs)
                m_thread_pool_size = rhs.m_thread_pool_size;

            return *this;
        }

        size_t
        ThreadPoolPolicy::get_pool_size() const
        throw()
        {
            return m_thread_pool_size;
        }

        void
        ThreadPoolPolicy::print(std::ostream & os) const
        throw()
        {
            try
            {
                os << THREAD_POOL_DESIGNATION
                << " <size =" << m_thread_pool_size << ">"
                << std::endl;
            }
            catch (...)
            {
                // do nothing
            }
        }

        void
        ThreadPoolPolicy::append_to_StrategyList(StrategyList & s) const
        throw()
        {
            s.add_PoaThreadPool(m_thread_pool_size);
        }

        void
        ThreadPerRequestPolicy::print(std::ostream & os) const
        throw()
        {
            try
            {
                os << THREAD_PER_REQUEST_DESIGNATION << std::endl;
            }
            catch (...)
            {
                // do nothing
            }
        }

        void
        ThreadPerRequestPolicy::append_to_StrategyList(StrategyList & s) const
        throw()
        {
            s.add_PoaThreadPerRequest();
        }

        void
        ThreadPerConnectionPolicy::print(std::ostream & os) const
        throw()
        {
            try
            {
                os << THREAD_PER_CONNECTION_DESIGNATION << std::endl;
            }
            catch (...)
            {
                // do nothing
            }
        }

        void
        ThreadPerConnectionPolicy::append_to_StrategyList(StrategyList & s) const
        throw()
        {
            s.add_PoaThreadPerConnection();
        }


    }

    ; // End namespace OrbSupport
}

; // End namespace Cdmw

