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


#ifndef INCL_PLATFORMMNGT_EVENT_NOTIFIER_HPP 
#define INCL_PLATFORMMNGT_EVENT_NOTIFIER_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "SystemMngt/platformlibrary/MessageQueue.hpp"
#include "SystemMngt/platformlibrary/BoundSyncCall.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/CommonLogMessageId.hpp"

#include "Foundation/orbsupport/CORBA.hpp"

#include <map>

namespace Cdmw
{
namespace PlatformMngt
{


/**
* Purpose:
* Thrown when the EventNotifier is stopped.
*/
class EventNotifierStoppedException  : public Exception {
    
    public:
        EventNotifierStoppedException()
            : Exception(Exception::SAFE, "EventNotifier stopped") { }

};


/**
*Purpose:
* Event notifier template.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
template <class Event, class EventDeallocator,
    class PtrEventObserver, class ClassEventObserver> class EventNotifier : public OsSupport::Thread
{

    public:


        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~EventNotifier() throw()
        {
            for (typename std::map<std::string, PtrEventObserver>::iterator it =
                m_observers.begin(); it != m_observers.end(); ++it)
            {

                PtrEventObserver observer = it->second;
                CORBA::release(observer);
            }
            stop();
        }


        /**
         *Purpose:
         *<p> Adds an event for notification.
         * If successful, this method takes ownership of the event.
         *
         *@param event The event to notify.
         *
         * @exception BadParameterException if the event is NULL.
         * @exception EventNotifierStoppedException if the event notifier is stopped.
         */
        void addEvent( Event* event )
            throw(BadParameterException, EventNotifierStoppedException,
            OutOfMemoryException)
        {

            if (event == NULL)
                CDMW_THROW2(BadParameterException, "event", "NULL");

            try
            {
                m_eventQueue.push(event);
            }
            catch(const MessageQueueDestroyedException&)
            {
                CDMW_THROW(EventNotifierStoppedException);
            }
        }


        /**
         *Purpose:
         *<p> Registers the observer with the specified name.
         *
         * @param observerName The name identifying the observer.
         * @param observer The observer to register.
         * @return the observer previously registered with observerName otherwise returns
         * ClassEventObserver::_nil().
         *
         * @exception BadParameterException if the observer is NIL.
         * @exception EventNotifierStoppedException if the event notifier is stopped.
         */
        PtrEventObserver registerObserver(const std::string& observerName, PtrEventObserver observer)
            throw(BadParameterException, EventNotifierStoppedException, OutOfMemoryException )
        {

            if (CORBA::is_nil(observer))
                CDMW_THROW2(BadParameterException, "observer", "NIL");

            CDMW_MUTEX_GUARD(m_mutex);

            if (m_stopped)
                CDMW_THROW(EventNotifierStoppedException);

            // gets the observer previously registered with observerName
            // if any
            PtrEventObserver previousObserver = ClassEventObserver::_nil();

            typename std::map<std::string, PtrEventObserver>::iterator it =
                m_observers.find(observerName);

            if (it != m_observers.end())
            {
                previousObserver = it->second;
            }


            // registers the observer
            PtrEventObserver newObserver = ClassEventObserver::_nil();

            try
            {
                newObserver = ClassEventObserver::_duplicate(observer);
                m_observers[observerName] = newObserver;

            }
            catch(const std::bad_alloc&)
            {
                if (!CORBA::is_nil(newObserver))
                    CORBA::release(newObserver);

                CDMW_THROW(OutOfMemoryException);
            }

            return previousObserver;

        }


        /**
         *Purpose:
         *<p> Unregisters the specified observer.
         *
         * @param observerName The name identifying the observer.
         * @return the observer or ClassEventObserver::_nil() if no observer was
         * registered with observerName.
         *
         * @exception EventNotifierStoppedException if the event notifier is stopped.
         */
        PtrEventObserver unregisterObserver(const std::string& observerName)
            throw( EventNotifierStoppedException )
        {

            CDMW_MUTEX_GUARD(m_mutex);

            if (m_stopped)
                CDMW_THROW(EventNotifierStoppedException);

            PtrEventObserver previousObserver = ClassEventObserver::_nil();

            typename std::map<std::string, PtrEventObserver>::iterator it =
                m_observers.find(observerName);

            if (it != m_observers.end())
            {
                previousObserver = it->second;

                m_observers.erase(observerName);
            }

            return previousObserver;

        }


        /**
         *Purpose:
         *<p> Stops this event notifier.
         */
        void stop() throw()
        {
            if (m_stopped)
                return;
                
            {
                CDMW_MUTEX_GUARD(m_mutex);
                m_stopped = true;
            }

            m_eventQueue.destroy();

            this->join();
        }


    protected:


        /**
        * Purpose:
        * <p> Constructor.
        */ 
        EventNotifier() : m_stopped(false)
        {
        }


        /**
         *Purpose:
         *<p> Returns the string description of the event used for
         * logging.
         * Must be defined by concrete sub-classes.
         */
        virtual std::string eventToString(Event* event) = 0;

        /**
         *Purpose:
         *<p> Creates an event notification call corresponding to the event
         * and the observer.
         * Must be defined by concrete sub-classes.
         *
         * @param observer The observer to notify.
         * @param event The event to notify.
         * @return the bound synchronous call.
         *
         * @exception OutOfMemoryException
         */
        virtual BoundSyncCall* createEventNotificationCall(PtrEventObserver observer, Event* event)
            throw(OutOfMemoryException) = 0;


    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        EventNotifier(const EventNotifier& rhs) { }


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        EventNotifier& operator=(const EventNotifier& rhs) { return *this; }


        /**
         *Purpose:
         *<p> The event notifier's activity.
         */
        void run()
	  throw()
        {

            bool stopped = false;

            LogMngr::logMessage(INF, MSG_ID_EVENT_NOTIF_RUNNING);

            do
            {
                m_mutex.lock();
                stopped = m_stopped;
                m_mutex.unlock();

                if (stopped)
                    break;

                Event *event = NULL;

                try
                {

                    event = m_eventQueue.pop();

                    // gets the string description of the event
                    std::string eventStr = eventToString(event);

                    // make a copy of the observers in order to release the mutex
                    m_mutex.lock();
                    std::map<std::string, PtrEventObserver> observers(m_observers);
                    m_mutex.unlock();

                    for (typename std::map<std::string, PtrEventObserver>::iterator it =
                        observers.begin(); it != observers.end(); ++it)
                    {

                        const std::string& observerName(it->first);
                        PtrEventObserver observer = it->second;

                        try
                        {
                            BoundSyncCall* notifCall =
                                createEventNotificationCall(observer, event);

                            // performs the notification via the
                            // bound synchronous call
                            BoundSyncCallReturn ret = notifCall->execute();

                            switch (ret)
                            {
                                case SUCCEED:
                                {
                                    //LogMngr::logMessage(INF, MSG_ID_EVENT_NOTIF_SUCCESS,
                                    //    observerName, eventStr);
                                }
                                break;

                                case TIMEOUT:
                                {
                                    LogMngr::logMessage(WRN, MSG_ID_EVENT_NOTIF_TIMEOUT,
                                        observerName, eventStr);
                                }
                                break;

                                case EXCEPTION:
                                {
                                    const char *exceptionName =
                                        notifCall->getExceptionName();

                                    LogMngr::logMessage(WRN, MSG_ID_EVENT_NOTIF_EXCEPTION,
                                        exceptionName, observerName, eventStr);
                                }
                                break;
                            }

                            delete notifCall;

                        }
                        catch( const OutOfMemoryException& )
                        {
                            // due to the allocation of notifCall
                            // the event is lost for the observer
                            LogMngr::logMessage(ERR, MSG_ID_EVENT_NOTIF_NOTIF_LOST, observerName,
                                    eventStr);
                        }

                    }

                    // deallocates the event
                    EventDeallocator::deallocate(event);

                }
                catch( const MessageQueueDestroyedException& )
                {
                    stopped = true;
                }
                catch( const std::bad_alloc& )
                {
                    // due to the allocation of eventStr
                    // or the allocation of observers
                    // => the event is lost for all observers
                    LogMngr::logMessage(ERR, MSG_ID_EVENT_NOTIF_EVENT_LOST);
                    EventDeallocator::deallocate(event);
                }
                catch( ... )
                {
                    // due to an unexpected exception
                    // => the event is lost for all observers
                    LogMngr::logMessage(ERR, MSG_ID_EVENT_NOTIF_UNEXP_EXCEPT);
                    EventDeallocator::deallocate(event);
                }
            }
            while (!stopped);
            
        }

        
        /**
        * The event queue.
        */
        MessageQueue<Event, EventDeallocator> m_eventQueue;

        /**
         *Purpose:
         *<p> Indicates whether this event notifier has been requested to stop.
         */
        bool m_stopped;

        /**
         * The mutex protecting concurrent access to m_stopped and m_observers.
         */
        OsSupport::Mutex m_mutex;
 
        /**
        * The event queue.
        */
        std::map<std::string, PtrEventObserver> m_observers;


}; // End class EventNotifier 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_EVENT_NOTIFIER_HPP

