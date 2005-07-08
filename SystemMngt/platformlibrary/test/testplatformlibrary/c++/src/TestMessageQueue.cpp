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


#include "testplatformlibrary/TestMessageQueue.hpp"
#include "testplatformlibrary/MessageQueueWrapper.hpp"
#include "testplatformlibrary/MessageQueueWriter.hpp"
#include "testplatformlibrary/MessageQueueDestructor.hpp"
#include "SystemMngt/platformlibrary/MessageQueue.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/System.hpp"
#include "Foundation/testutils/Testable.hpp"

#include <iostream>
#include <string>
#include <memory>


TestMessageQueue::TestMessageQueue(const std::string& name)
    : Testable(name)
{
}


TestMessageQueue::~TestMessageQueue()
  throw()
{
}


void TestMessageQueue::do_tests()
{
    // set number of requested successfull tests
    set_nbOfRequestedTestOK (17);
    

    Cdmw::PlatformMngt::MessageQueue<std::string> queue;

    TEST_INFO("Adds M1 in the message queue");

    try
    {
        std::auto_ptr<std::string> mes(new std::string("M1"));

        queue.push(mes.get());

        mes.release();
        TEST_SUCCEED();
    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Adds M2 in the message queue");
    try
    {
        std::auto_ptr<std::string> mes(new std::string("M2"));

        queue.push(mes.get());

        mes.release();
        TEST_SUCCEED();
    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Tries to add a NULL message in the message queue");
    try
    {

        queue.push(NULL);
        TEST_FAILED();
    }
    catch(const Cdmw::BadParameterException& e)
    {
        std::cout << e.what() << std::endl;
        TEST_SUCCEED();
    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Checks the size of the message queue");
    try
    {
        if (queue.size() == 2)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Adds M3, M4 in the message queue");
    try
    {
        std::list<std::string*> messages;

        std::auto_ptr<std::string> mes1(new std::string("M3"));
        messages.push_back(mes1.get());
        mes1.release();

        std::auto_ptr<std::string> mes2(new std::string("M4"));
        messages.push_back(mes2.get());
        mes2.release();

        queue.push(messages);
        TEST_SUCCEED();
    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Checks the size of the message queue");
    try
    {
        if (queue.size() == 4)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Tries to add M5, NULL in the message queue");
    try
    {
        std::list<std::string*> messages;

        std::auto_ptr<std::string> mes1(new std::string("M5"));
        messages.push_back(mes1.get());
        mes1.release();

        messages.push_back(NULL);

        queue.push(messages);
        TEST_FAILED();
    }
    catch(const Cdmw::BadParameterException& e)
    {
        std::cout << e.what() << std::endl;
        TEST_SUCCEED();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Tries to add NULL, M5 in the message queue");
    try
    {
        std::list<std::string*> messages;

        messages.push_back(NULL);

        std::auto_ptr<std::string> mes1(new std::string("M5"));
        messages.push_back(mes1.get());
        mes1.release();

        queue.push(messages);
        TEST_FAILED();
    }
    catch(const Cdmw::BadParameterException& e)
    {
        std::cout << e.what() << std::endl;
        TEST_SUCCEED();
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Checks the size of the message queue");
    try
    {
        if (queue.size() == 4)
            TEST_SUCCEED();
        else
            TEST_FAILED();

    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }


    TEST_INFO("Retrieves messages");
    try
    {
        std::auto_ptr<std::string> mes1(queue.pop());
        std::auto_ptr<std::string> mes2(queue.pop());
        std::auto_ptr<std::string> mes3(queue.pop());

        std::cout << mes1->c_str() << std::endl;
        std::cout << mes2->c_str() << std::endl;
        std::cout << mes3->c_str() << std::endl;

        if (queue.isEmpty())
            TEST_FAILED();
        else
        {
            std::auto_ptr<std::string> mes4(queue.pop());
            std::cout << mes4->c_str() << std::endl;
        }

        if (!queue.isEmpty())
            TEST_FAILED();
        else
            TEST_SUCCEED();
    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        TEST_FAILED();
    }

    
    TEST_INFO("Threading tests");
    MessageQueueWrapper queueWrapper(queue);

    size_t nbA = 60;
    MessageQueueWriter writerA(queueWrapper, "A", nbA, 7);
    size_t nbB = 60;
    MessageQueueWriter writerB(queueWrapper, "B", nbB, 5);
    size_t nbC = 60;
    MessageQueueWriter writerC(queueWrapper, "C", nbC, 3);
    size_t nbD = 60;
    MessageQueueWriter writerD(queueWrapper, "D", nbD, 1);

    // starts the writers
    writerA.start();
    writerB.start();
    writerC.start();
    writerD.start();

    size_t messageCount = 0;
	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OsSupport::OS::sleep(timescale*20);

    do
    {
        std::auto_ptr<std::string> mes(queueWrapper.pop());
        std::cout << mes->c_str() << std::endl;

        ++messageCount;

    }
    while (messageCount < nbA + nbB + nbC + nbD);

    // joins the writers' thread 
    writerA.join();
    writerB.join();
    writerC.join();
    writerD.join();

    unsigned short maxNbThreads = queueWrapper.maxNbThreads();
    std::cout << "Maximum number of threads detected : " << maxNbThreads << std::endl;
    if (maxNbThreads > 1)
    {
        TEST_SUCCEED();
    }
    else
    {
        std::cout << "More than one thread was expected." << std::endl;
        TEST_FAILED();
    }

    TEST_INFO("Destroy tests");
    MessageQueueDestructor destructor(queue, 4000);

    destructor.start();

    try
    {
        std::cout << "Waiting for a message." << std::endl;
        std::auto_ptr<std::string> mes(queue.pop());
        TEST_FAILED();
    }
    catch(const Cdmw::PlatformMngt::MessageQueueDestroyedException&)
    {
        std::cout << "The thread blocked by pop() has been released by destroy()." << std::endl;
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }

    destructor.join();


    TEST_INFO("Try to access the message queue after its destruction");
    try
    {
        bool ret = queue.isEmpty();
        ret = false; // to avoid warning
        TEST_FAILED();
    }
    catch(const Cdmw::PlatformMngt::MessageQueueDestroyedException&)
    {
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }

    try
    {
        size_t size = queue.size();
        size = 0; // to avoid warning
        TEST_FAILED();
    }
    catch(const Cdmw::PlatformMngt::MessageQueueDestroyedException&)
    {
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }

    try
    {
        std::auto_ptr<std::string> mes(new std::string("X"));
        queue.push(mes.get());
        mes.release();

        TEST_FAILED();
    }
    catch(const Cdmw::PlatformMngt::MessageQueueDestroyedException&)
    {
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }

    try
    {
        std::list<std::string*> messages;

        std::auto_ptr<std::string> mes1(new std::string("X"));
        messages.push_back(mes1.get());
        mes1.release();

        queue.push(messages);
        TEST_FAILED();
    }
    catch(const Cdmw::PlatformMngt::MessageQueueDestroyedException&)
    {
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }

    try
    {
        std::auto_ptr<std::string> mes(queue.pop());
        TEST_FAILED();
    }
    catch(const Cdmw::PlatformMngt::MessageQueueDestroyedException&)
    {
        TEST_SUCCEED();
    }
    catch(...)
    {
        TEST_FAILED();
    }

}



