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


#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "SystemMngt/platformlibrary/PullMonitor.hpp"
#include "SystemMngt/platformlibrary/LogMngr.hpp"
#include "SystemMngt/platformlibrary/CommonLogMessageId.hpp"


#include <memory>


namespace Cdmw
{
namespace PlatformMngt
{

/**
*Implementation notes: 
*<p>
*Portability issues:  none 
*<p>
*/





//
// Ctor
//    
PullMonitor::PullMonitor (CdmwPlatformMngt::PullMonitorable* p_pullMonitorable,
                          unsigned long monitoringInterval,
                          unsigned long monitoringTimeout)
    : m_stopped (false),
      m_timerCondition (m_mutex),
      m_monitoringInterval (monitoringInterval),
      m_monitoringTimeout (monitoringTimeout)
      
{
    m_pResponseCallback = NULL;
    m_pNoResponseCallback = NULL; 
    
    // pullMonitorable can be nil
    m_pullMonitorable = 
           CdmwPlatformMngt::PullMonitorable::_duplicate (p_pullMonitorable);
           
    m_pBoundSyncCallFactory = BoundSyncCallFactory::getFactory();
}




//
// Dtor
//
PullMonitor::~PullMonitor()
    throw()
{
	// delete the callback objects if defined
    if (m_pResponseCallback != NULL)
    {
    	delete m_pResponseCallback;
    }
    
    if (m_pNoResponseCallback != NULL)
    {
    	delete m_pNoResponseCallback;
    }
}



//
// Ask to the internal thread for stopping
//
void PullMonitor::stop()
    throw (InternalErrorException)
{
	if (m_stopped == false)
	{
		try 
	    {
	        // Lock the mutex
	        m_mutex.lock();

            // We require the thread to terminate
            m_stopped = true;

            // We awake the thread, if it is sleeping
            m_timerCondition.broadcast();

            // Unlock the mutex
	        m_mutex.unlock();
	
            // Wait the termination of the internal thread
            this->join();
        } 
        
        // No explanation, how is it possible : corrupted
	    catch (const OsSupport::DeadlockException&)
	    {
            CDMW_THROW (InternalErrorException);
        }
        catch (...)
	    {
            CDMW_THROW (InternalErrorException);
        }
    }
}




//
// Call in a new thread by the Thread system
//
void PullMonitor::run()
    throw()
{
    try 
	{
		OsSupport::Condition::WaitReturn status;
		
		// lock mutex
		CDMW_MUTEX_GUARD (m_mutex);
		
        // Infinite loop, we stop when requested
        while (m_stopped == false) 
	    {
            // rearm timer and wait condition
            status = m_timerCondition.wait(m_monitoringInterval);  
            
            // The time has expired, start monitoring
            if (status == OsSupport::Condition::TIMEDOUT)
            {
            	try
            	{
            	    // get pull Monitorable object
            	    CdmwPlatformMngt::PullMonitorable_var pullMonitorable =
            	                                       get_pullMonitorable();
            	    
            	    // if object to monitor is not defined             
            	    if (CORBA::is_nil (pullMonitorable.in()))
            	    {
            	        if (m_pNoResponseCallback != NULL)
            	        {
                            // execute no response callback if defined
                            m_pNoResponseCallback->execute();
                        }
            	    }
            	    else
            	    {                        	                 
                        // create a PullMonitoring call using the BoundSyncCallFactory
                        std::auto_ptr<PullMonitoringCall> p_monitoringCall (
                           m_pBoundSyncCallFactory->createPullMonitoringCall(
                                                              pullMonitorable.in(),
                                                              m_monitoringTimeout));
                                       
                        // poll the pullMonitorable object via the PullMonitoring call
                        BoundSyncCallReturn result = p_monitoringCall->execute();
                    
                        // if monitorable object is alive
                        if (result == SUCCEED)
                        {
            	            if (m_pResponseCallback != NULL)
            	            {
                                // execute response callback if defined
                                m_pResponseCallback->execute();
                            }
                        }
                        else
                        {
                            if (m_pNoResponseCallback != NULL)
            	            {
                                // execute no response callback if defined
                                m_pNoResponseCallback->execute();
                            }
                        }
                   }
                }
                catch(const OutOfMemoryException&)
                {
                    LogMngr::logMessage(ERR, MSG_ID_MONITORING_NO_MEMORY); 
                }
            }
            
            else if (status == OsSupport::Condition::SIGNALLED)
            {
              LogMngr::logMessage(INF, MSG_ID_MONITORING_SIGNAL);
            }              
        }
    }
    
	// catch any exception and assert it
	catch (...)
	{
	  CDMW_ASSERT(false);
	}
}

//
// get pull monitable
//
const CdmwPlatformMngt::PullMonitorable_ptr PullMonitor::get_pullMonitorable()
            throw()
{
    // copy object reference in var (implicit duplicate)
    CdmwPlatformMngt::PullMonitorable_var pullMonitorable = m_pullMonitorable;
    return pullMonitorable._retn();
}
            


//
// Set the response callback
//
void PullMonitor::setResponseCallback (Callback* p_callback)
            throw (BadParameterException)
{
	if (p_callback == NULL)
    {
        CDMW_THROW2(BadParameterException, "p_callback", "NULL");
    }
    
    m_pResponseCallback = p_callback;
}

//
// Set the no response callback
//
void PullMonitor::setNoResponseCallback (Callback* p_callback)
            throw (BadParameterException)
{
	if (p_callback == NULL)
    {
        CDMW_THROW2(BadParameterException, "p_callback", "NULL");
    }
    
    m_pNoResponseCallback = p_callback;
}

} // End namespace PlatformMngt
} // End namespace Cdmw

