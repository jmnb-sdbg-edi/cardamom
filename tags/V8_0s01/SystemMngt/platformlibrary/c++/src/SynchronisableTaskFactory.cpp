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


#include "SystemMngt/platformlibrary/TaskSequencer.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskFactory.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


//=========================================================================
// SynchronisableTask
//=========================================================================


SynchronisableTask::SynchronisableTask(const std::string& id,
    Sequencer* sequencer, TaskObserver* observer, size_t nbPredecessors)
    : Task(id, observer), m_nbPredecessorsToWait(nbPredecessors)
{

    if (sequencer == NULL)
    {
        CDMW_THROW2(BadParameterException, "sequencer", "NULL");
    }

    m_sequencer = sequencer;

}


SynchronisableTask::~SynchronisableTask()
  throw()
{

}


void SynchronisableTask::startTask() throw()
{

    if (m_nbPredecessorsToWait > 1)
    {
        --m_nbPredecessorsToWait;
    }
    else
    {
        start();
    }

}


void SynchronisableTask::run()
  throw()
{

    bool ret = execute();

    if (!ret)
        m_sequencer->suspend();

    end();

}


//=========================================================================
// SynchronisableTaskFactory
//=========================================================================

SynchronisableTaskFactory::SynchronisableTaskFactory(
    const SynchronisableTaskDefMap& taskDefMap,
    const TaskIdList& rootTasksId)
    : m_taskDefMap(taskDefMap), m_rootTasksId(rootTasksId)
{
}


SynchronisableTaskFactory::~SynchronisableTaskFactory()
{
}


size_t SynchronisableTaskFactory::getNumberOfTasks()
{
    return m_taskDefMap.size();
}


SynchronisableTaskDef& SynchronisableTaskFactory::find(
    const std::string& taskId)
    throw(InvalidTaskIdException)
{

    SynchronisableTaskDefMap::iterator it =
        m_taskDefMap.find(taskId);

    if (it == m_taskDefMap.end())
    {
        CDMW_THROW1(InvalidTaskIdException, taskId);
    }

    return it->second;

}


bool SynchronisableTaskFactory::hasSuccessors(const std::string& taskId)
    throw(InvalidTaskIdException)
{
    bool ret = false;

    SynchronisableTaskDef& taskDef = find(taskId);

    if (!(taskDef.m_successors_id).empty())
        ret = true;

    return ret;

}

TaskIdList SynchronisableTaskFactory::getSuccessorsId(const std::string& taskId)
    throw(InvalidTaskIdException)
{

    SynchronisableTaskDef& taskDef = find(taskId);

    return taskDef.m_successors_id;

}


TaskIdList SynchronisableTaskFactory::getRootTasksId()
{
    return m_rootTasksId;
}


SynchronisableTaskDef SynchronisableTaskFactory::getTaskDef(const std::string& taskId)
    throw(InvalidTaskIdException)
{
    SynchronisableTaskDef& taskDef = find(taskId);

    return taskDef;
}


} // End namespace PlatformMngt
} // End namespace Cdmw

