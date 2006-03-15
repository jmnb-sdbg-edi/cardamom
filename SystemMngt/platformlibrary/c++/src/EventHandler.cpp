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


#include "SystemMngt/platformlibrary/EventHandler.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/CommonLogMessageId.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"

#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"


namespace Cdmw
{
namespace PlatformMngt
{

// ----------------------------------------------------------------------
// EventHandler class.
// ----------------------------------------------------------------------
EventHandler::EventHandler() : m_stopped(false),                                
                               m_evt_seconds(0),
                               m_evt_microsec(0),
                               m_evt_index(0),
                               m_handler_freeze(false),
                               m_handler_freeze_condition(m_handler_freeze_mutex)
{
    // use "this" address as the primary key to store events
    // received by this handler
    m_evt_primary_key = reinterpret_cast <unsigned long> (this);
    
    try
    {
        Cdmw::CommonSvcs::DataStore::DataStoreBase* ds =
            Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
                   Configuration::Get_event_report_datastore_id());
        m_event_report_ds = 
                 dynamic_cast<Cdmw::PlatformMngt::EventReportDataStore*>(ds);
    }
    catch (const NotExistException& e)
    {
        CDMW_ASSERT(false);
    }
}

EventHandler::~EventHandler() 
       throw()
{
}


void EventHandler::addEvent(Event* event)
throw (BadParameterException,
       EventHandlerStoppedException,
       InternalErrorException,
       OutOfMemoryException)
{
    if (event == NULL)
        CDMW_THROW2 (BadParameterException, "event", "NULL");

    // insert event in datastore
    insertEventInDataStore (event);
    
    // push event in message queue
    try
    {
        m_eventQueue.push (event);
    }
    catch (const MessageQueueDestroyedException&)
    {
        CDMW_THROW (EventHandlerStoppedException);
    }
}

void EventHandler::stop()
{

    {
        CDMW_MUTEX_GUARD (m_mutex);
        m_stopped = true;
    }

    m_eventQueue.destroy();

    this->join();
}


void EventHandler::freeze_handler()
{
    // first lock the mutex of freeze flag
    CDMW_MUTEX_GUARD(m_handler_freeze_mutex);
        
    // set the ready flag
    m_handler_freeze = true;
}
  
void EventHandler::unfreeze_handler()
{
    // first lock the mutex of freeze flag
    CDMW_MUTEX_GUARD(m_handler_freeze_mutex);
    
    if (m_handler_freeze == true)
    {        
        // reset the ready flag
        m_handler_freeze = false;
        
        // broadcast the condition
        m_handler_freeze_condition.broadcast();
    }
}



void EventHandler::run()
  throw()
{
    try
    {
        do
        {
            // check stop flag
            {
                CDMW_MUTEX_GUARD (m_mutex);
                if (m_stopped)
                    break;
            }

            Event* event = m_eventQueue.pop();
            
            
            // check if handler has to be frozen
            {
                // first lock the mutex of freeze flag
                CDMW_MUTEX_GUARD(m_handler_freeze_mutex);
        
                // check the ready flag
                while (m_handler_freeze == true)
                {        
                    // wait the condition if freeze activated
                    m_handler_freeze_condition.wait();
                }
            }
            
            // execute the event
            
            try
            {
                event->execute();
            }
            catch(...)
            {
                LogMngr::logMessage(ERR, MSG_ID_EVENT_HANDLER_EXEC_EXCEPT);
            }

            // remove event from datastore
            removeEventInDataStore (event->getEventKey());
            
            delete event;
        }
        while(true);
    }
    catch (const MessageQueueDestroyedException&)
    {
    }
    catch( ... )
    {
        LogMngr::logMessage(ERR, MSG_ID_EVENT_HANDLER_UNEXP_EXCEPT);
    }
}


void EventHandler::insertEventInDataStore (Event* event)
{
    if (Configuration::Is_primary())
    { 
        // set the event timestamp identification    
        CdmwPlatformMngtBase::EventKey event_key;
    
        {
            CDMW_MUTEX_GUARD (m_event_key_mutex);
            
            // set primary key
            event_key.primary_key = m_evt_primary_key;
    
            // get time val
            OsSupport::OS::Timeval time_val = OsSupport::OS::get_time();
            
            event_key.seconds = 
                       static_cast <unsigned long> (time_val.seconds);
            event_key.microseconds = 
                       static_cast <unsigned long> (time_val.microseconds);
    
            bool evt_inx_incr = false;
                      
            if (event_key.seconds < m_evt_seconds)
            {
    	        event_key.seconds = m_evt_seconds;
    	        event_key.microseconds = m_evt_microsec;
    	        evt_inx_incr = true;
            }
    
            else if (event_key.seconds == m_evt_seconds)
            {
    	        if (event_key.microseconds < m_evt_microsec)
                {
    	            event_key.microseconds = m_evt_microsec;
    	            evt_inx_incr = true;
                }
                else if (event_key.microseconds == m_evt_microsec)
                {
        	        evt_inx_incr = true;
                }  
            }
    
            // if there is no difference between previous event time stamp
            // increment the evt index to build an unique event key
            if (evt_inx_incr)
            {
    	        m_evt_index ++;
            }
            else
            {
    	        // reset the index (time is different)
    	        // counter index in header is already set to 0
    	        m_evt_index = 0;
            }
    
            event_key.counter_inx = m_evt_index;

            // store event key in event
            event->setEventKey (event_key);
            
            // save the current time used for index
            m_evt_seconds = event_key.seconds;
    	    m_evt_microsec = event_key.microseconds;
        }
    
        // update the datastore
        try
        {
            m_event_report_ds->insert(event_key, event->getRecord());
        }
        catch (Common::AlreadyExistException&)
        {
            std::cout << "event report record already exists in datastore : " 
                      << event_key.primary_key << ":"
                      << event_key.seconds << ":" 
                      << event_key.microseconds << ":" 
                      << event_key.counter_inx << std::endl;
                      
            // CDMW_ASSERT(false);
        } 
        catch (...)
        {
            std::cout << "event report record unexpected exception in insertion : " 
                      << event_key.primary_key << ":"
                      << event_key.seconds << ":" 
                      << event_key.microseconds << ":" 
                      << event_key.counter_inx << std::endl;
        }
    }
    
}

void EventHandler::removeEventInDataStore (CdmwPlatformMngtBase::EventKey& event_key)
{
    if (Configuration::Is_primary())
    {
        try
        {
            m_event_report_ds->remove(event_key);
        }
        catch (Common::NotFoundException&)
        {
        }
    }
}




} // End namespace PlatformMngt
} // End namespace Cdmw

