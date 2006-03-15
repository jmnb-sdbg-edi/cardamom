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


#ifndef INCL_MYSYNCHRONISABLETASKFACTORY_HPP
#define INCL_MYSYNCHRONISABLETASKFACTORY_HPP

#include "SystemMngt/platformlibrary/SynchronisableTaskFactory.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/testutils/TestManager.hpp"

#include "testplatformlibrary/SynchronisableTaskChecker.hpp"

#include <iostream>

using namespace Cdmw;

class MySynchronisableTask : virtual public PlatformMngt::SynchronisableTask
{

public:

    MySynchronisableTask(SynchronisableTaskChecker& checker,
        const std::string& id, PlatformMngt::Sequencer* sequencer,
        PlatformMngt::TaskObserver* observer, size_t nbPredecessors)
        : PlatformMngt::SynchronisableTask(id, sequencer, observer, nbPredecessors)
        , m_checker(checker)
    {
        int result = Common::string_compare (id,0,1,"F");

        if (result == 0)
            m_faulty = true;
        else
            m_faulty = false;

    }

    virtual ~MySynchronisableTask() throw() {}

protected:

    virtual bool execute() throw()
    {
        std::string messageHeader("Task ");
        messageHeader += m_id;
        
        std::string entryMessage = messageHeader + " executing";
        m_checker.enter(m_id, entryMessage);
		int timescale = Cdmw::TestUtils::get_timescale();
        OsSupport::OS::sleep(timescale*500);

        std::string exitMessage = messageHeader;
        if (m_faulty)
        {
            exitMessage += " reported an error";
        }
        else
        {
            exitMessage += " executed";
        }

        m_checker.exit(exitMessage);

        return !m_faulty;
    }


    SynchronisableTaskChecker& m_checker;

    bool m_faulty;

};


class MySynchronisableTaskFactory : virtual public PlatformMngt::SynchronisableTaskFactory
{

public:

    MySynchronisableTaskFactory(SynchronisableTaskChecker& checker,
        const PlatformMngt::SynchronisableTaskDefMap& taskDefMap,
        const PlatformMngt::TaskIdList& rootTasksId)
        : PlatformMngt::SynchronisableTaskFactory(taskDefMap, rootTasksId)
        , m_checker(checker)
    {
    }

    virtual ~MySynchronisableTaskFactory() {}


    virtual PlatformMngt::SynchronisableTask* createTask(
        const std::string& taskId,
        PlatformMngt::Sequencer* sequencer,
        PlatformMngt::TaskObserver* taskObserver)
        throw(PlatformMngt::InvalidTaskIdException, OutOfMemoryException)
    {

        PlatformMngt::SynchronisableTaskDef taskDef = getTaskDef(taskId);
        
        return new MySynchronisableTask(m_checker,
            taskId, sequencer, taskObserver, taskDef.m_nbPredecessors);
    }

protected:

    SynchronisableTaskChecker& m_checker;

};


#endif // INCL_MYSYNCHRONISABLETASKFACTORY_HPP

