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


#include "SystemMngt/platformlibrary/TaskStarter.hpp"

#include "testplatformlibrary/TaskCounter.hpp"
#include "testplatformlibrary/MyTaskFactory.hpp"
#include "testplatformlibrary/MyCallback.hpp"

#include "testplatformlibrary/TestStarter.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/testutils/Testable.hpp"

#include <iostream>
#include <string>
#include <memory>

using namespace Cdmw::PlatformMngt;


TestStarter::TestStarter(const std::string& name)
    : Testable(name)
{
}


TestStarter::~TestStarter()
{
}


void TestStarter::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (8);
    
    
    TaskCounter taskCounter;
    MyTaskFactory taskFactory(taskCounter);

    MyCallback *callback = new MyCallback("Starter completed");

    TEST_INFO("Creates the starter");
    Starter *starter = new Starter(&taskFactory);
    starter->setCompletionCallback(callback);

    std::list<std::string> tasksList;
    tasksList.push_back("T1");
    tasksList.push_back("T2");
    tasksList.push_back("T3");
    tasksList.push_back("T4");
    tasksList.push_back("T5");
    tasksList.push_back("T6");

    TEST_INFO("Starts the starter with 6 concurrent tasks");
    starter->addTasksToStart(tasksList);
    starter->start();
    TEST_SUCCEED();

    TEST_INFO("Waiting a while to let the tasks complete their work");
	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OsSupport::OS::sleep(timescale*1000);

    TEST_INFO("Stops the starter");
    starter->stop();
    TEST_SUCCEED();

    TEST_INFO("Destroys the starter");
    delete starter;
    TEST_SUCCEED();

    TEST_INFO("Checks the concurrency of tasks");
    size_t maxNumberConcurrentTasks = taskCounter.getMaxNbConcurrentTasks();

    std::cout << "Maximum number of concurrent tasks : "
        << maxNumberConcurrentTasks << std::endl;

    if (maxNumberConcurrentTasks > 1)
        TEST_SUCCEED();
    else
        TEST_FAILED();


    TEST_INFO("Creates a dummy starter with no tasks to start");
    starter = new Starter(&taskFactory);
    callback = new MyCallback("Dummy Starter completed");
    starter->setCompletionCallback(callback);

    TEST_INFO("Starts the dummy starter");
    taskCounter.reset();
    starter->start();

    TEST_INFO("Waiting a while");
    OsSupport::OS::sleep(timescale*1000);

    TEST_INFO("Stops the starter");
    starter->stop();
    TEST_SUCCEED();

    TEST_INFO("Destroys the starter");
    delete starter;
    TEST_SUCCEED();

    TEST_INFO("Checks that nothing has been performed");
    if (taskCounter.getMaxNbConcurrentTasks() == 0)
        TEST_SUCCEED();
    else
        TEST_FAILED();


    TEST_INFO("Starts a new starter and stops it immediately");
    starter = new Starter(&taskFactory);
    callback = new MyCallback("Dummy Starter completed");
    starter->setCompletionCallback(callback);
    starter->addTasksToStart(tasksList);

    taskCounter.reset();
    starter->start();
    starter->stop();

    TEST_INFO("Destroys the starter");
    delete starter;
    TEST_SUCCEED();

}



