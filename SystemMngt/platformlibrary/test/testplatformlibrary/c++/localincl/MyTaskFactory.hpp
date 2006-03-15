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


#ifndef INCL_MYTASKFACTORY_HPP
#define INCL_MYTASKFACTORY_HPP

#include "SystemMngt/platformlibrary/TaskFactory.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/TestManager.hpp"

#include "testplatformlibrary/TaskCounter.hpp"

#include <iostream>

using namespace Cdmw;

class MyTask : virtual public PlatformMngt::Task
{

public:

    MyTask(TaskCounter& counter, const std::string& id, PlatformMngt::TaskObserver* observer)
        : PlatformMngt::Task(id, observer), m_counter(counter)
    {
    }

    virtual ~MyTask() throw() {}

protected:

    virtual bool execute() throw()
    {
        std::string messageHeader("Task ");
        messageHeader += m_id;
        
        std::string entryMessage = messageHeader + " executing";
        m_counter.enter(entryMessage);
		
		int timescale = Cdmw::TestUtils::get_timescale();
        OsSupport::OS::sleep(timescale*500);

        std::string exitMessage = messageHeader + " executed";
        m_counter.exit(exitMessage);

        return true;
    }

    TaskCounter& m_counter;

};


class MyTaskFactory : virtual public PlatformMngt::TaskFactory
{

public:

    MyTaskFactory(TaskCounter& counter)
        : PlatformMngt::TaskFactory(), m_counter(counter)
    {
    }


    virtual ~MyTaskFactory() {}


    virtual PlatformMngt::Task* createTask(const std::string& taskId,
        PlatformMngt::TaskObserver* taskObserver)
        throw(PlatformMngt::InvalidTaskIdException, OutOfMemoryException)
    {
        return new MyTask(m_counter, taskId, taskObserver);
    }

protected:

    TaskCounter& m_counter;

};


#endif // INCL_MYTASKFACTORY_HPP

