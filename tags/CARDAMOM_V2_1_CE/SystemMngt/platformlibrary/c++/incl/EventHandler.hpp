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


#ifndef INCL_PLATFORMMNGT_EVENT_HANDLER_HPP
#define INCL_PLATFORMMNGT_EVENT_HANDLER_HPP

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "SystemMngt/platformlibrary/MessageQueue.hpp"
#include "SystemMngt/platformlibrary/DataStoreBaseDefinition.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp"

#include <iostream>

namespace Cdmw
{
namespace PlatformMngt
{

class Event;
class EventHandler;


/**
 * Purpose:
 * Thrown when the event queue is stopped.
 */
class EventHandlerStoppedException  : public Exception
{
public:
    EventHandlerStoppedException()
        : Exception (Exception::SAFE, "EventHandler Stopped")
    {
    };
};

/**
 *Purpose:
 *<p> Base class for all events.
 *
 *Features:
 *<p> 
 */        
class Event
{
public:
    /**
     * Purpose:
     * <p> Default destructor.
     */ 
    virtual
    ~Event()
    {
    };

    /**
     *Purpose:
     *<p> Abstract method for processing this event's action.
     */
    virtual
    void execute() = 0;
    
    /**
     *Purpose:
     *<p> Get the event key.
     */
    CdmwPlatformMngtBase::EventKey& getEventKey()
    {
    	return m_record.header.event_key;
    };
    
    /**
     *Purpose:
     *<p> Set the event timestamp (used for event key).
     */
    void setEventKey(const CdmwPlatformMngtBase::EventKey& event_key)
    {
    	m_record.header.event_key = event_key;
    };
    
    const CdmwPlatformMngt::EventReportRecord& getRecord ()
    {
        return m_record;
    };
    
    
    
protected:

    /**
     * The Event report record used for datastore.
     */
    CdmwPlatformMngt::EventReportRecord m_record;
};

/**
 *Purpose:
 *<p> Thread safe event handler. Event queueing and processing.
 *
 *Features:
 *<p> 
 */        
class EventHandler : public OsSupport::Thread
{
    /**
     *Purpose:
     *<p> Thread safe event queue.
     *
     *Features:
     *<p> Thread safe
     */        
    typedef MessageQueue<Event> EventQueue;

private:

    /**
     *Purpose:
     *<p> Indicates whether this event handler has been requested to stop.
     */
    bool m_stopped;

    /**
     *Purpose:
     *<p> The event queue.
     */
    EventQueue m_eventQueue;

    /**
     * The mutex protecting concurrent access to m_stopped.
     */
    OsSupport::Mutex m_mutex;
    
    
    /**
     * Event report datastore
     */
    EventReportDataStore* m_event_report_ds;
    
    
    /**
     * The mutex protecting event identification.
     */
    OsSupport::Mutex m_event_key_mutex;
    
    /**
     * primary key for event
     */
    unsigned long m_evt_primary_key;
    
    /**
     * current time (sec and microseconds) for the previous sent event
     */
    unsigned long m_evt_seconds;
    unsigned long m_evt_microsec;
        
    /**
     * index for event identification
     * incremented if 2 events have the same timestamp
     */
    unsigned long m_evt_index;
    
    /**
    * The handler freeze flag.
    */
    bool m_handler_freeze;
    
    /**
    * The lock protecting concurrent access to freeze flag.
    */
    OsSupport::Mutex m_handler_freeze_mutex;
    
    /**
    * The condition on the freeze flag.
    */
    OsSupport::Condition m_handler_freeze_condition;

private:
    /**
    * Purpose:
    * <p>
    * Copy constructor is not allowed.
    */ 
    EventHandler(const EventHandler& rhs);

    /**
    * Purpose:
    * <p>
    * Assignment operator is not allowed.
    */ 
    EventHandler& operator=(const EventHandler& rhs)
    {
        return *this;
    };

    
    /**
     *Purpose:
     *<p> Insert the event in DataStore.
     *
     *@param event The event to be placed in the data store.
     *
     */
    void insertEventInDataStore (Event* event);
    
    /**
     *Purpose:
     *<p> remove the event from DataStore.
     *
     *@param event_key The key of event to remove from the data store.
     *
     */
    void removeEventInDataStore (CdmwPlatformMngtBase::EventKey& event_key);
    

public:
    /**
     * Purpose:
     * <p> Default Constructor.
     */
    EventHandler();
    
    /**
     * Purpose:
     * <p> Destructor.
     */ 
    virtual ~EventHandler() 
       throw();


    /**
     *Purpose:
     *<p> Adds an event for processing.
     * If successful, this method takes ownership of the event.
     *
     *@param event The event to be placed in the event queue.
     *
     *@exception BadParameterException if message is NULL.
     *@exception EventHandlerStoppedException if the event queue is stopped.
     *@exception InternalErrorException
     *@exception OutOfMemoryException.
     */
    void addEvent (Event* event)
    throw (BadParameterException,
           EventHandlerStoppedException,
           InternalErrorException,
           OutOfMemoryException);

    /**
     *Purpose:
     *<p> Stops this event handler.
     */
    void stop();
    
    
    /**
	* Purpose:
	* <p>
	* freeze the handler, event are kept in queue
	*/   
    void freeze_handler();
    
    /**
	* Purpose:
	* <p>
	* unfreeze the handler
	*/   
    void unfreeze_handler();
     

protected:
    /**
     *Purpose:
     *<p> Event's handler activity.
     */
    void run() throw();
 
};

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_EVENT_HANDLER_HPP

