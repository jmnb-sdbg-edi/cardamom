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


#ifndef INCL_PLATFORMMNGT_STATE_MACHINE_HPP 
#define INCL_PLATFORMMNGT_STATE_MACHINE_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"

#include <string>
#include <map>


namespace Cdmw
{
namespace PlatformMngt
{


/**
* Purpose:
* Thrown when an unknown state is requested.
*/
class UnknownStateException  : public Exception {
    
    public:
        UnknownStateException(const std::string& stateMachine, const std::string& stateName)
            : Exception(Exception::FATAL, "Unknown state")
        {
            m_what = std::string(Exception::what()) + ":" + stateName +
                " in " + stateMachine + " state machine.";
        }

  virtual ~UnknownStateException() throw() {};
  
  virtual const char* what() const throw()
        {
            return m_what.c_str();
        }

    private:
       std::string m_what;

};


/**
*Purpose:
*<p> The abstract state machine base class.
* Provides lock for reading and writing.
*/
class AbstractStateMachine
{

    public:

    /**
    * Purpose:
    * <p>
    * Locks the state machine for reading.
    */ 
    void readLock()
    {
        m_rwLock.readLock();
    }


    /**
    * Purpose:
    * <p>
    * Unlocks the state machine after reading.
    */ 
    void readUnlock()
    {
        m_rwLock.readUnlock();
    }


    /**
    * Purpose:
    * <p>
    * Locks the state machine for writing.
    */ 
    void writeLock()
    {
        m_rwLock.writeLock();
    }


    /**
    * Purpose:
    * <p>
    * Unlocks the state machine after writing.
    */ 
    void writeUnlock()
    {
        m_rwLock.writeUnlock();
    }

protected:

    /**
    * The reader/writer lock used to manage concurrent thread safe access to
    * the state machine.
    */
    Cdmw::OsSupport::ReaderWriterLock m_rwLock;

};


/**
*Purpose:
*<p> Automatically locks/unlocks the state machine for reading.
* "Resource acquisition is initialisation" principle.
*
*Features:
*<p> Thread safe, exception safe.
*/
class StateMachineReaderLockGuard 
{

    public:

    /**
    * Purpose:
    * <p> Constructor.
    * Locks the state machine for reading.
    *
    * @exception AlreadyDoneException the state machine is already
    * locked by the calling thread
    * @exception InternalErrorException	    	    
    */
    StateMachineReaderLockGuard(AbstractStateMachine& stateMachine)
        throw (AlreadyDoneException, InternalErrorException)
        : m_stateMachine(stateMachine)
    {
        m_stateMachine.readLock();
    }

    /**
    * Purpose:
    * <p> Destructor.
    * Unlocks the state machine after reading.
    */
    virtual ~StateMachineReaderLockGuard() throw()
    {
        try
        {
            m_stateMachine.readUnlock();
        }
        catch(...)
        {
            CDMW_ASSERT(false);
        }
    }

    private:
    
    /**
    * Copy constructor is not allowed.
    */
    StateMachineReaderLockGuard(const StateMachineReaderLockGuard& rhs);

    /**
    * Assignement operator is not allowed.
    */
    StateMachineReaderLockGuard& operator=(const StateMachineReaderLockGuard& rhs);

    /**
    * The state machine to lock/unlock.
    */
    AbstractStateMachine& m_stateMachine;

}; // End class StateMachineReaderLockGuard 


/**
*Purpose:
*<p> Automatically locks/unlocks the state machine for writing.
* "Resource acquisition is initialisation" principle.
*
*Features:
*<p> Thread safe, exception safe.
*/
class StateMachineWriterLockGuard 
{

    public:

    /**
    * Purpose:
    * <p> Constructor.
    * Locks the state machine for writing.
    *
    * @exception AlreadyDoneException the state machine is already
    * locked by the calling thread
    * @exception InternalErrorException	    	    
    */
    StateMachineWriterLockGuard(AbstractStateMachine& stateMachine)
        throw (AlreadyDoneException, InternalErrorException)
        : m_stateMachine(stateMachine)
    {
        m_stateMachine.writeLock();
    }

    /**
    * Purpose:
    * <p> Destructor.
    * Unlocks the state machine after writing.
    */
    virtual ~StateMachineWriterLockGuard() throw()
    {
        try
        {
            m_stateMachine.writeUnlock();
        }
        catch(...)
        {
            CDMW_ASSERT(false);
        }
    }

    private:
    
    /**
    * Copy constructor is not allowed.
    */
    StateMachineWriterLockGuard(const StateMachineWriterLockGuard& rhs);

    /**
    * Assignement operator is not allowed.
    */
    StateMachineWriterLockGuard& operator=(const StateMachineWriterLockGuard& rhs);

    /**
    * The state machine to lock/unlock.
    */
    AbstractStateMachine& m_stateMachine;

}; // End class StateMachineWriterLockGuard 


/**
* Facility macro for using a StateMachineReaderLockGuard.
*/
#define CDMW_SM_READER_LOCK_GUARD(sm) Cdmw::PlatformMngt::StateMachineReaderLockGuard lockGuard##__LINE__(sm)


/**
* Facility macro for using a StateMachineWriterLockGuard.
*/
#define CDMW_SM_WRITER_LOCK_GUARD(sm) Cdmw::PlatformMngt::StateMachineWriterLockGuard lockGuard##__LINE__(sm)


/**
*Purpose:
*<p> 
* Generic implementation of a complete UML state machine with in particular
* completion transition.
* This implementation offers also a typed access to the context of the state machine
* and a typed access to the states of the state machine.
*
*Features:
*<p> Thread safe :
* Access to the state machine must be done through the lock methods.
* These lock methods may also be used to protect state related data
* that is managed by the context.
*<p> Exception safe.
*<p>
*Usage:
*<p>
* C : The context of the state machine. C must be forward declared.
* The context provides the whole set of transitions of the state machine.
* Each transition method makes use of the state machine in order to change
* the current state.
* S : The base class of the concrete states of the state machine.
* It provides the whole set of transitions of the state machine.
* Each transition method must have the state machine as the first parameter.
* Concrete states are expected to be singletons.
*<p>
* C and S must be forward declared as the state machine provides typed access
* to them. Moreover S must inherit the StateMachine::AbstractState once the
* the template has been instantiated.
*
* Example :
*
* // forward declarations
* class Context;
* class BaseState;
*
* // the typedef of the state machine
* typedef StateMachine<Context, BaseState> SampleStateMachine;
*
* // definition of BaseState must follow immediately the state machine typedef
* class BaseState : virtual public SampleStateMachine::AbstractState
* {
*     // ...
* }
*
*/
template <class C, class S> class StateMachine : public AbstractStateMachine
{

    public:

        /**
        * Purpose:
        * <p> The abstract state of the state machine.
        * Provides the default implementation of a UML state.
        */ 
        class AbstractState
        {
            public:
            virtual void entryAction(StateMachine* stateMachine) { }
            virtual void doActivity(StateMachine* stateMachine) { }
            virtual void exitAction(StateMachine* stateMachine) { }
            virtual void completionTransition(StateMachine* stateMachine) { }

            virtual ~AbstractState() { }

        };


        /**
        * Purpose:
        * <p> Constructor.
        *
        * @param context The context of the state machine.
        * @param name The name identifying the state machine.
        */ 
        StateMachine(C* context, const std::string& name)
            : m_name(name)
        {
            if (context == NULL)
                CDMW_THROW2(BadParameterException, "context", "NULL");

            m_context = context;
            m_state = NULL;
            m_running = false;
        }


        /**
        * Purpose:
        * <p>
        * Returns the context of the state machine.
        */ 
        C* getContext()
        {
            return m_context;
        }


        /**
        * Purpose:
        * <p>
        * Adds the specified state with the given name.
        * The state machine doesn't take ownership of the state.
        * The state is expected to be a singleton that may be shared
        * by multiple instances of the state machine.
        *
        * Thread safety : the state machine must have been locked for writing.
        *
        * @param stateName The name identifying the state.
        * @param state The state to be added to the state machine.
        *
        * @exception AlreadyDoneException if the state machine is already running.
        * @exception OutOfMemoryException
        */ 
        void addState(const std::string& stateName, AbstractState* state)
            throw (AlreadyDoneException, OutOfMemoryException)
        {

            CDMW_ASSERT(state != NULL);

            if (m_running)
                CDMW_THROW(AlreadyDoneException);

            try
            {
                m_stateMap[stateName] = state;
            }
            catch(const std::bad_alloc &)
            {
                CDMW_THROW(OutOfMemoryException);
            }

        }


        /**
        * Purpose:
        * <p>
        * Changes the state of the state machine. The first invocation of this
        * method makes the state machine run.
        *
        * Thread safety : the state machine must have been locked for writing.
        *
        * @param stateName The name identifying the new state.
        *
        * @exception UnknownStateException if stateName doesn't refer to a known state.
        * @exception Any exception raises by entryAction,doActivity,completionTransition 
        */ 
        void setState(const std::string& stateName)
        {

            // finds the state specified by stateName
            AbstractState *state = NULL;

            typename std::map<std::string, AbstractState*>::iterator it =
                m_stateMap.find(stateName);

            if (it != m_stateMap.end())
            {
                state = it->second;
            }
            else
            {
                CDMW_THROW2(UnknownStateException, m_name, stateName);
            }

            m_running = true;

 
            // Exits the current state if any
            if (m_state != NULL)
            {
                m_state->exitAction(this);
            }


            // Enters the new state
            m_state = state;
            m_state->entryAction(this);

            // Performs the activity of the state
            m_state->doActivity(this);

            // Fires the completion transition if any
            m_state->completionTransition(this);

        }


        /**
        * Purpose:
        * <p>
        * Returns the current state of the state machine.
        *
        * Thread safety : the state machine must have been locked for reading
        * or writing.
        */ 
        S* getState()
        {
            return dynamic_cast<S*>(m_state);
        }


    private:

        /**
        * The name identifying the state machine.
        * Used for debugging purpose only.
        */
        std::string m_name;

        /**
        * The context of the state machine.
        */
        C* m_context;

        /**
        * Indicates whether the state machine is running.
        */
        bool m_running;

        /**
        * The map of the state machine's states.
        */
        std::map<std::string, AbstractState*> m_stateMap;

        /**
        * The current state of the state machine.
        */
        AbstractState* m_state;


}; // End template StateMachine 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_STATE_MACHINE_HPP

