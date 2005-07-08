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


#include "SystemMngt/platformlibrary/TaskSequencer.hpp"
#include "SystemMngt/platformlibrary/SynchronisableTaskDef.hpp"

#include "testplatformlibrary/SynchronisableTaskChecker.hpp"
#include "testplatformlibrary/MySynchronisableTaskFactory.hpp"
#include "testplatformlibrary/MyCallback.hpp"

#include "testplatformlibrary/TestSequencer.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/testutils/Testable.hpp"

#include <iostream>
#include <string>
#include <memory>

using namespace Cdmw::PlatformMngt;
using Cdmw::PlatformMngt::SynchronisableTaskDef;
using Cdmw::PlatformMngt::TaskIdList;


TestSequencer::TestSequencer(const std::string& name)
    : Testable(name)
{
}


TestSequencer::~TestSequencer()
{
}


bool TestSequencer::checkTaskSuccessors(const TaskIdList& resultList,
    const std::string& taskId,
    const Cdmw::PlatformMngt::TaskIdList& successors)
{
    bool ret = true;


    TaskIdList::const_iterator task_it;
    bool found = false;
    
    for (task_it = resultList.begin() ;
         task_it != resultList.end() ;
         task_it++)
    {
        if (*task_it == taskId)
        {
            found = true;
            break;
        }
    }
    
    
    if (!found)

    {
        ret = false;
        std::cout << "ERROR: Task " << taskId << " has not been executed" << std::endl;
    }
    else
    {
        std::cout << "Task " << taskId << " has been executed" << std::endl;
    }

    if (ret && !successors.empty())
    {
        std::cout << "Checks successors of Task " << taskId << ":" << std::endl;

        for (TaskIdList::const_iterator it = successors.begin();
            it != successors.end(); ++it)
        {

           TaskIdList::const_iterator task_it2;
            bool found = false;
    
            for (task_it2 = resultList.begin() ;
                 task_it2 != resultList.end() ;
                 task_it2++)
            {
                if (*task_it2 == *it)
                {
                    found = true;
                    break;
                }
            }

            
            if (!found)


            {
                ret = false;        
                std::cout << "Error: Task " << *it <<
                    " has not been executed after Task " << taskId;
                break;
            }

            std::cout << *it << " ";

        }

        std::cout << std::endl;

    }

    return ret;

}


bool TestSequencer::checkDependencyGraph(const TaskIdList& resultList,
    const SynchronisableTaskDefMap& taskDefMap)
{

    bool ret = true;

    std::cout << "Tasks execution order : ";

    for (TaskIdList::const_iterator it = resultList.begin();
        it != resultList.end(); ++it)
    {

        std::cout << *it << " ";

    }
    std::cout << std::endl;

    for (SynchronisableTaskDefMap::const_iterator it = taskDefMap.begin();
        it != taskDefMap.end(); ++it)
    {

        ret = checkTaskSuccessors(resultList, it->first, (it->second).m_successors_id);

        if (!ret)
            break;

    }

    return ret;

}


void TestSequencer::checkExecution(const SynchronisableTaskChecker& taskChecker,
    const SynchronisableTaskDefMap& taskDefMap)
{
    // set number of requested successfull tests
    add_nbOfRequestedTestOK (2);
    
    TEST_INFO("Checks the concurrency of tasks");
    size_t maxNumberConcurrentTasks = taskChecker.getMaxNbConcurrentTasks();

    std::cout << "Maximum number of concurrent tasks : "
        << maxNumberConcurrentTasks << std::endl;

    if (maxNumberConcurrentTasks > 1)
        TEST_SUCCEED();
    else
        TEST_FAILED();

    TEST_INFO("Checks the graph of tasks dependency");
    bool check = checkDependencyGraph(taskChecker.getTaskIdList(), taskDefMap); 
    if (check)
        TEST_SUCCEED();
    else
        TEST_FAILED();

}


void TestSequencer::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (18);

    std::cout << std::endl << std::endl;
    std::cout << "Creates the following sequencer :            " << std::endl;
    std::cout << std::endl;
    std::cout << "          +--> T4 --|                        " << std::endl;
    std::cout << " +--> T1 -|         |                        " << std::endl;
    std::cout << " |        +--> T5 --|--> T6 -|               " << std::endl;
    std::cout << " |                  |        |               " << std::endl;
    std::cout << " +--> T2 -----------|        |               " << std::endl;
    std::cout << " |                           |--> T10        " << std::endl;
    std::cout << " |        +--> T7            |               " << std::endl;
    std::cout << " |        |                  |               " << std::endl;
    std::cout << " +--> T3 -+--> T8 --------> -|               " << std::endl;
    std::cout << "          |                                  " << std::endl;
    std::cout << "          +--> T9                            " << std::endl;
    std::cout << std::endl;

    SynchronisableTaskDef T1;
    T1.m_nbPredecessors = 0;
    T1.m_successors_id.push_back("T4");
    T1.m_successors_id.push_back("T5");

    SynchronisableTaskDef T2;
    T2.m_nbPredecessors = 0;
    T2.m_successors_id.push_back("T6");

    SynchronisableTaskDef T3;
    T3.m_nbPredecessors = 0;
    T3.m_successors_id.push_back("T7");
    T3.m_successors_id.push_back("T8");
    T3.m_successors_id.push_back("T9");

    SynchronisableTaskDef T4;
    T4.m_nbPredecessors = 1;
    T4.m_successors_id.push_back("T6");

    SynchronisableTaskDef T5;
    T5.m_nbPredecessors = 1;
    T5.m_successors_id.push_back("T6");

    SynchronisableTaskDef T6;
    T6.m_nbPredecessors = 3;
    T6.m_successors_id.push_back("T10");

    SynchronisableTaskDef T7;
    T7.m_nbPredecessors = 1;

    SynchronisableTaskDef T8;
    T8.m_nbPredecessors = 1;
    T8.m_successors_id.push_back("T10");

    SynchronisableTaskDef T9;
    T9.m_nbPredecessors = 1;

    SynchronisableTaskDef T10;
    T10.m_nbPredecessors = 2;

    SynchronisableTaskDefMap taskDefs;
    taskDefs["T1"] = T1;
    taskDefs["T2"] = T2;
    taskDefs["T3"] = T3;
    taskDefs["T4"] = T4;
    taskDefs["T5"] = T5;
    taskDefs["T6"] = T6;
    taskDefs["T7"] = T7;
    taskDefs["T8"] = T8;
    taskDefs["T9"] = T9;
    taskDefs["T10"] = T10;

    TaskIdList rootTasksId;
    rootTasksId.push_back("T1");
    rootTasksId.push_back("T2");
    rootTasksId.push_back("T3");
 
    ////   
    TaskIdList::iterator it;
    for (it=rootTasksId.begin() ; it != rootTasksId.end() ; it++)
    {
       std::cout << "Root Task " << (*it).c_str() << std::endl; 
    }
    ////


    SynchronisableTaskChecker taskChecker;

    MySynchronisableTaskFactory taskFactory(taskChecker, taskDefs, rootTasksId);
    MyCallback *completionCallback = new MyCallback("Sequencer completed");
    MyCallback *suspensionCallback = new MyCallback("Sequencer suspended");

    Sequencer *sequencer = new Sequencer(&taskFactory);
    sequencer->setCompletionCallback(completionCallback);
    sequencer->setSuspensionCallback(suspensionCallback);

    TEST_INFO("Starts the sequencer");
    sequencer->start();
    TEST_SUCCEED();

    TEST_INFO("Waiting a while to let the tasks complete their work");
	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OsSupport::OS::sleep(timescale*5000);

    TEST_INFO("Stops the sequencer");
    sequencer->stop();
    TEST_SUCCEED();

    TEST_INFO("Destroys the sequencer");
    delete sequencer;
    TEST_SUCCEED();

    checkExecution(taskChecker, taskDefs);


    std::cout << std::endl << std::endl;
    std::cout << "Creates the following sequencer with faulty task F5 :" << std::endl;
    std::cout << std::endl;
    std::cout << "          +--> T4 --|                        " << std::endl;
    std::cout << " +--> T1 -|         |                        " << std::endl;
    std::cout << " |        +--> F5 --|--> T6 -|               " << std::endl;
    std::cout << " |                  |        |               " << std::endl;
    std::cout << " +--> T2 -----------|        |               " << std::endl;
    std::cout << " |                           |--> T10        " << std::endl;
    std::cout << " |        +--> T7            |               " << std::endl;
    std::cout << " |        |                  |               " << std::endl;
    std::cout << " +--> T3 -+--> T8 --------> -|               " << std::endl;
    std::cout << "          |                                  " << std::endl;
    std::cout << "          +--> T9                            " << std::endl;
    std::cout << std::endl;

    T1.m_nbPredecessors = 0;
    T1.m_successors_id.clear();
    T1.m_successors_id.push_back("T4");
    T1.m_successors_id.push_back("F5");


    taskDefs.clear();
    taskDefs["T1"] = T1;
    taskDefs["T2"] = T2;
    taskDefs["T3"] = T3;
    taskDefs["T4"] = T4;
    taskDefs["F5"] = T5;
    taskDefs["T6"] = T6;
    taskDefs["T7"] = T7;
    taskDefs["T8"] = T8;
    taskDefs["T9"] = T9;
    taskDefs["T10"] = T10;

    taskChecker.reset();

    MySynchronisableTaskFactory taskFactory2(taskChecker, taskDefs, rootTasksId);

    sequencer = new Sequencer(&taskFactory2);
    completionCallback = new MyCallback("Sequencer completed");
    suspensionCallback = new MyCallback("Sequencer suspended");

    sequencer->setCompletionCallback(completionCallback);
    sequencer->setSuspensionCallback(suspensionCallback);

    TEST_INFO("Starts the sequencer");
    sequencer->start();
    TEST_SUCCEED();

    TEST_INFO("Waiting a while to let the tasks complete their work");
    OsSupport::OS::sleep(timescale*5000);

    TEST_INFO("Resumes the sequencer");
    sequencer->resume();
    TEST_SUCCEED();

    TEST_INFO("Waiting a while to let the remaining tasks complete their work");
    OsSupport::OS::sleep(timescale*5000);

    TEST_INFO("Stops the sequencer");
    sequencer->stop();
    TEST_SUCCEED();

    TEST_INFO("Destroys the sequencer");
    delete sequencer;
    TEST_SUCCEED();

    checkExecution(taskChecker, taskDefs);



    std::cout << std::endl << std::endl;
    std::cout << "Creates the following sequencer with faulty tasks F5 and F10 :" << std::endl;
    std::cout << std::endl;
    std::cout << "          +--> T4 --|                        " << std::endl;
    std::cout << " +--> T1 -|         |                        " << std::endl;
    std::cout << " |        +--> F5 --|--> T6 -|               " << std::endl;
    std::cout << " |                  |        |               " << std::endl;
    std::cout << " +--> T2 -----------|        |               " << std::endl;
    std::cout << " |                           |--> F10        " << std::endl;
    std::cout << " |        +--> T7            |               " << std::endl;
    std::cout << " |        |                  |               " << std::endl;
    std::cout << " +--> T3 -+--> T8 --------> -|               " << std::endl;
    std::cout << "          |                                  " << std::endl;
    std::cout << "          +--> T9                            " << std::endl;
    std::cout << std::endl;

    T6.m_nbPredecessors = 3;
    T6.m_successors_id.clear();
    T6.m_successors_id.push_back("F10");

    T8.m_nbPredecessors = 1;
    T8.m_successors_id.clear();
    T8.m_successors_id.push_back("F10");

    taskDefs.clear();
    taskDefs["T1"] = T1;
    taskDefs["T2"] = T2;
    taskDefs["T3"] = T3;
    taskDefs["T4"] = T4;
    taskDefs["F5"] = T5;
    taskDefs["T6"] = T6;
    taskDefs["T7"] = T7;
    taskDefs["T8"] = T8;
    taskDefs["T9"] = T9;
    taskDefs["F10"] = T10;

    taskChecker.reset();
    
    ////   
    TaskIdList::iterator it2;
    for (it2=rootTasksId.begin() ; it2 != rootTasksId.end() ; it2++)
    {
       std::cout << "Root Task " << (*it2).c_str() << std::endl; 
    }
    ////

    MySynchronisableTaskFactory taskFactory3(taskChecker, taskDefs, rootTasksId);

    sequencer = new Sequencer(&taskFactory3);
    completionCallback = new MyCallback("Sequencer completed");
    suspensionCallback = new MyCallback("Sequencer suspended");

    sequencer->setCompletionCallback(completionCallback);
    sequencer->setSuspensionCallback(suspensionCallback);

    TEST_INFO("Starts the sequencer");
    sequencer->start();
    TEST_SUCCEED();

    TEST_INFO("Waiting a while to let the tasks complete their work");
    OsSupport::OS::sleep(timescale*5000);

    TEST_INFO("Resumes the sequencer");
    sequencer->resume();
    TEST_SUCCEED();

    TEST_INFO("Waiting a while to let the remaining tasks complete their work");
    OsSupport::OS::sleep(timescale*5000);

    TEST_INFO("Resumes the sequencer");
    sequencer->resume();
    TEST_SUCCEED();

    TEST_INFO("Waiting a while to let the remaining tasks complete their work");
    OsSupport::OS::sleep(timescale*5000);

    TEST_INFO("Stops the sequencer");
    sequencer->stop();
    TEST_SUCCEED();

    TEST_INFO("Destroys the sequencer");
    delete sequencer;
    TEST_SUCCEED();

    checkExecution(taskChecker, taskDefs);


    std::cout << std::endl << std::endl;
    std::cout << "Creates a sequencer with faulty tasks F5 and F10 :" << std::endl;

    taskChecker.reset();

    MySynchronisableTaskFactory taskFactory4(taskChecker, taskDefs, rootTasksId);

    sequencer = new Sequencer(&taskFactory4);
    completionCallback = new MyCallback("Sequencer completed");
    suspensionCallback = new MyCallback("Sequencer suspended");

    sequencer->setCompletionCallback(completionCallback);
    sequencer->setSuspensionCallback(suspensionCallback);

    TEST_INFO("Starts the sequencer");
    sequencer->start();
    TEST_SUCCEED();

    TEST_INFO("Waiting a while to let the tasks complete their work");
    OsSupport::OS::sleep(timescale*5000);

    TEST_INFO("Stops the sequencer after the suspension");
    sequencer->stop();
    TEST_SUCCEED();

    TEST_INFO("Destroys the sequencer");
    delete sequencer;
    TEST_SUCCEED();


    std::cout << std::endl << std::endl;
    std::cout << "Creates, starts and stops prematurely a sequencer :" << std::endl;

    taskChecker.reset();

    MySynchronisableTaskFactory taskFactory5(taskChecker, taskDefs, rootTasksId);

    sequencer = new Sequencer(&taskFactory5);

    TEST_INFO("Starts the sequencer");
    sequencer->start();
    TEST_SUCCEED();

    TEST_INFO("Stops the sequencer immediately");
    sequencer->stop();
    TEST_SUCCEED();

    TEST_INFO("Destroys the sequencer");
    delete sequencer;
    TEST_SUCCEED();


}



