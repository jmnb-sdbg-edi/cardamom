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


#include "testosthreads/TestConditionBroadcastThread.hpp"

#include "Foundation/testutils/Testable.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/osthreads/Condition.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        /**
        *Implementation notes: [if no pertinent write none ]
        *<p>
        *Portability issues: [if no pertinent write none ]
        *<p>
        */


        TestConditionBroadcastThread::TestConditionBroadcastThread( Condition& condition )
                : m_condition(condition)
        {
        }


        TestConditionBroadcastThread::~TestConditionBroadcastThread()
        throw()
        {
            // Nothing to do
        }

        void
        TestConditionBroadcastThread::run()
        throw()
        {
            // We sleep a while before signalling the condition
            int timescale = Cdmw::TestUtils::Testable::get_timescale();
            Thread::sleep(timescale*2000);
            m_condition.get_mutex().lock();
            m_condition.broadcast();
            m_condition.get_mutex().unlock();

        }

    } // End namespace OsSupport
} // End namespace Cdmw

