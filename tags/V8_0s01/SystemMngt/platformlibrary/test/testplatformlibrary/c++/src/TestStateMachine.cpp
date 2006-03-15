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


#include "SystemMngt/platformlibrary/StateMachineTemplate.hpp"
#include "testplatformlibrary/TestStateMachine.hpp"

#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/TestManager.hpp"

#include <string>
#include <iostream>
#include <memory>


using namespace Cdmw;

CPPUNIT_TEST_SUITE_REGISTRATION(TestStateMachine);

class Context;
class BaseState;

typedef Cdmw::PlatformMngt::StateMachine<Context, BaseState>
    SampleStateMachine;



/**
* The base of the state machine's concrete states.
*
* This base provides the same transitions as the context but
* the transition methods take the state machine as the first parameter.
*/
class BaseState : virtual public SampleStateMachine::AbstractState
{
public:
    // standard actions and standard activity
    void entryAction(SampleStateMachine* stateMachine) { display("no entry"); }
    void exitAction(SampleStateMachine* stateMachine) { display("no exit"); }
    void doActivity(SampleStateMachine* stateMachine) { display("no activity"); }

    virtual void initialise(SampleStateMachine* stateMachine) { }
    virtual void start(SampleStateMachine* stateMachine) { }
    virtual void stop(SampleStateMachine* stateMachine) { }

    virtual ~BaseState() { }

protected:

    BaseState(const std::string& name) : m_name(name) { }

    void display(const std::string& info)
    {
        std::cout << m_name << ":" << info << std::endl << std::endl;
    }

private:

    std::string m_name;

};


/**
* The context of the state machine.
*
* This context provides the following transitions :
* - initialise
* - start
* - stop
*/
class Context
{

friend class TestStateMachine;

private:

    SampleStateMachine m_stateMachine;

    /**
    * A data related to the state of the context.
    * Protected by the state machine.
    */
    std::string m_info;

public:

    Context() : m_stateMachine(this, "Context") {}
    virtual ~Context() { }

    /**
    * Returns the state info safely.
    */
    virtual std::string info()
    {
        CDMW_SM_READER_LOCK_GUARD(m_stateMachine);
        return m_info; 
    }

    /**
    * Sets the state info.
    * The state machine must have been locked for writing.
    */
    void setInfo(const std::string& info)
    {
        m_info = info;
    }

    // The transitions
    void initialise()
    {
        CDMW_SM_WRITER_LOCK_GUARD(m_stateMachine);
        (m_stateMachine.getState())->initialise(&m_stateMachine);
    }

    void start()
    {
        CDMW_SM_WRITER_LOCK_GUARD(m_stateMachine);
        (m_stateMachine.getState())->start(&m_stateMachine);
    }

    void stop()
    {
        CDMW_SM_WRITER_LOCK_GUARD(m_stateMachine);
        (m_stateMachine.getState())->stop(&m_stateMachine);
    }

    void change_to_invalid_state()
    {
        CDMW_SM_WRITER_LOCK_GUARD(m_stateMachine);
        m_stateMachine.setState("X");
    }

};


/**
* The initial state.
*/
class StateInitial : virtual public BaseState
{
public:

    StateInitial(const std::string& name) : BaseState(name) { }

    // Standard actions
    void entryAction(SampleStateMachine* stateMachine) { display("specific entry"); }
    void exitAction(SampleStateMachine* stateMachine) { display("specific exit"); }

    // Transitions
    void initialise(SampleStateMachine* stateMachine)
    {
        stateMachine->setState("Initialising");
    }

};


/**
* The initialising state.
*/
class StateInitialising : virtual public BaseState
{
public:

    StateInitialising(const std::string& name, unsigned int delay )
        : BaseState(name), m_delay(delay) { }

    // Standard actions
    void doActivity(SampleStateMachine* stateMachine)
    {
        display("Initialisation in progress ...");
        stateMachine->getContext()->setInfo("CORBA is fun");
		int timescale = Cdmw::TestUtils::get_timescale();
        OsSupport::OS::sleep(timescale*m_delay);
    }

    // Transitions
    void completionTransition(SampleStateMachine* stateMachine)
    {
        stateMachine->setState("Initialised");
    }
private:

    unsigned int m_delay; // the initialisation delay

};


/**
* The initialised state.
*/
class StateInitialised : virtual public BaseState
{
public:

    StateInitialised(const std::string& name) : BaseState(name) { }

    // Standard actions
    void entryAction(SampleStateMachine* stateMachine) { display("specific entry"); }

    // Transitions
    void start(SampleStateMachine* stateMachine)
    {
        stateMachine->setState("Starting");
    }

};


/**
* The starting state.
*/
class StateStarting : virtual public BaseState
{
public:

    StateStarting(const std::string& name) : BaseState(name) { }

    // Standard actions
    void doActivity(SampleStateMachine* stateMachine)
    {
        std::string mes("specific activity ... ");
        display(mes);
        stateMachine->getContext()->setInfo("CORBA is very fun");
    }

    void completionTransition(SampleStateMachine* stateMachine)
    {
        stateMachine->setState("Started");
    }

};

/**
* The started state.
*/
class StateStarted : virtual public BaseState
{
public:

    StateStarted(const std::string& name) : BaseState(name) { }

    void entryAction(SampleStateMachine* stateMachine)
    {
        display("specific entry");
    }

};


/**
*
*/
class StateMachineExciter : virtual public OsSupport::Thread
{

public:

    StateMachineExciter(Context& context, unsigned int delay)
        : m_context(context), m_delay(delay)
    {
    }

    virtual ~StateMachineExciter() throw() {}

protected:

    Context& m_context;
    unsigned int m_delay;

    void run() throw()
    {
	        int timescale = Cdmw::TestUtils::get_timescale();
            OsSupport::OS::sleep(timescale*m_delay);
            m_context.start();
    }

};


TestStateMachine::TestStateMachine()
{
}


TestStateMachine::~TestStateMachine()
{
}


void TestStateMachine::do_tests()
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (2);
    

    Context context;

    unsigned int delay = 100;

    std::auto_ptr<StateInitial> initial(new StateInitial("Initial"));
    std::auto_ptr<StateInitialising> initialising(new StateInitialising("Initialising", 40*delay));
    std::auto_ptr<StateInitialised> initialised(new StateInitialised("Initialised"));
    std::auto_ptr<StateStarting> starting(new StateStarting("Starting"));
    std::auto_ptr<StateStarted> started(new StateStarted("Started"));

    // direct use of state machine locking
    (context.m_stateMachine).writeLock();

    context.m_stateMachine.addState("Initial", initial.get());
    context.m_stateMachine.addState("Initialising", initialising.get());
    context.m_stateMachine.addState("Initialised", initialised.get());
    context.m_stateMachine.addState("Starting", starting.get());
    context.m_stateMachine.addState("Started", started.get());

    TEST_INFO("Sets the initial state");
    context.m_stateMachine.setState("Initial");

    (context.m_stateMachine).writeUnlock();

    TEST_INFO("start() will be requested by exciter thread");
    StateMachineExciter smExciter(context, delay);
    smExciter.start();

    TEST_INFO("initialise() is requested by main thread");
    context.initialise();

    TEST_INFO("Tries to change to an invalid state");
    try
    {
        context.change_to_invalid_state();
    }
    catch(const Cdmw::PlatformMngt::UnknownStateException &e)
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(true);
    }
    catch(const Cdmw::Exception& e)
    {
        std::cout << e.what() << std::endl;
        CPPUNIT_ASSERT(false);
    }

    smExciter.join();

    std::cout << "Info = '" << context.info() << "'" << std::endl;
    CPPUNIT_ASSERT(context.info() == "CORBA is very fun");

}


