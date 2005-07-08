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


#ifndef INCL_TASKCOUNTER_HPP
#define INCL_TASKCOUNTER_HPP

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/common/Assert.hpp"

#include <iostream>

using namespace Cdmw;


/**
 * Counts the max number of concurrent tasks.
 */
class TaskCounter
{

public:

    TaskCounter() : m_nbConcurrentTasks(0), m_maxNbConcurrentTasks(0)
    {
    }

    virtual ~TaskCounter() {}

    void enter(const std::string& message)
    {

        CDMW_MUTEX_GUARD(m_mutex);

        std::cout << message << std::endl;

        ++m_nbConcurrentTasks;

        if (m_nbConcurrentTasks > m_maxNbConcurrentTasks)
        {
            m_maxNbConcurrentTasks = m_nbConcurrentTasks;
        }

    }

    void exit(const std::string& message)
    {

        CDMW_MUTEX_GUARD(m_mutex);

        std::cout << message << std::endl;

        CDMW_ASSERT(m_nbConcurrentTasks > 0);

        --m_nbConcurrentTasks;

    }


    size_t getMaxNbConcurrentTasks() const
    {
        return m_maxNbConcurrentTasks;
    }


    void reset()
    {
        m_nbConcurrentTasks = 0;
        m_maxNbConcurrentTasks = 0;
    }


protected:

    size_t m_nbConcurrentTasks;

    size_t m_maxNbConcurrentTasks;

    OsSupport::Mutex m_mutex;


};


#endif // INCL_TASKCOUNTER_HPP

