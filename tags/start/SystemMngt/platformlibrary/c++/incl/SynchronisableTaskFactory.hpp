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


#ifndef INCL_PLATFORMMNGT_SYNCHRONISABLETASKFACTORY_HPP 
#define INCL_PLATFORMMNGT_SYNCHRONISABLETASKFACTORY_HPP 

#include "SystemMngt/platformlibrary/TaskFactory.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskDef.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

class Sequencer;

/**
*Purpose:
* The abstract simple task.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class SynchronisableTask : public Task
{

    public:

        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~SynchronisableTask() throw();


        /**
         *Purpose:
         *<p> Decrements the number of predecessors to wait.
         * Starts actually the task if there is no more predecessors
         * to wait.
         */
        virtual void startTask() throw();


    protected:

        /**
        * Purpose:
        * <p> Constructor.
        */ 
        SynchronisableTask(const std::string& id, Sequencer* sequencer,
            TaskObserver* observer, size_t nbPredecessors);


        /**
         *Purpose:
         *<p>
         */
        virtual void run() throw();


    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        SynchronisableTask(const SynchronisableTask& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        SynchronisableTask& operator=(const SynchronisableTask& rhs);



    private:

        /**
         * The number of predecessors to wait.
         */
        size_t m_nbPredecessorsToWait;
       
        /**
         * The sequencer.
         */
        Sequencer* m_sequencer;


}; // End Task


/**
*Purpose:
* The synchronisable task factory.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class SynchronisableTaskFactory
{

    public:

        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~SynchronisableTaskFactory();

        /**
         *Purpose:
         *<p> Returns the number of tasks of this factory.
         */
        size_t getNumberOfTasks();

        /**
         *Purpose:
         *<p> Creates the specified task.
         *
         * @param taskId the identifier of the task.
         * @param sequencer the sequencer of the created task.
         * @param taskObserver the observer of the created task.
         */
        virtual SynchronisableTask* createTask(const std::string& taskId,
            Sequencer* sequencer, TaskObserver* taskObserver)
            throw(InvalidTaskIdException, OutOfMemoryException) = 0;

        /**
         *Purpose:
         *<p> Indicates whether the specified task has successors.
         *
         * @param taskId the identifier of the task.
         */
        bool hasSuccessors(const std::string& taskId)
            throw(InvalidTaskIdException);


        /**
         *Purpose:
         *<p> Returns the identifier of the specified task's successors.
         *
         * @param taskId the identifier of the task.
         */
        TaskIdList getSuccessorsId(const std::string& taskId)
            throw(InvalidTaskIdException);


        /**
         *Purpose:
         *<p> Returns the identifier of the root tasks.
         */
        TaskIdList getRootTasksId();


    protected:

        /**
        * Purpose:
        * <p> Constructor.
        */ 
        SynchronisableTaskFactory(const SynchronisableTaskDefMap& taskDefMap,
            const TaskIdList& rootTasksId);

        /**
         *Purpose:
         *<p> Returns the definition of the task.
         *
         * @param taskId the identifier of the task.
         */
        SynchronisableTaskDef getTaskDef(const std::string& taskId)
            throw(InvalidTaskIdException);


    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        SynchronisableTaskFactory(const SynchronisableTaskFactory& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        SynchronisableTaskFactory& operator=(const SynchronisableTaskFactory& rhs);


        /**
        * Purpose:
        * <p>
        */ 
        SynchronisableTaskDef& find(const std::string& taskId)
            throw(InvalidTaskIdException);


    private:

        /**
         * The map of tasks' definition
         */
        SynchronisableTaskDefMap m_taskDefMap;

        /**
         * The list of the root tasks' id.
         */
        TaskIdList m_rootTasksId;

}; // End class SynchronisableTaskFactory 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_SYNCHRONISABLETASKFACTORY_HPP

