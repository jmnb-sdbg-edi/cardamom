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

#include "testmanagedprocess/MyProcessBehaviour.hpp"

#include "testmanagedprocess/AutoEnding.hpp"


/**
*Implementation notes: 
*<p>
*Portability issues:  none 
*<p>
*/

//
// Ctor
//    
AutoEnding::AutoEnding (int endTimeout)
    : m_stopped(false),
      m_timerCondition (m_mutex),
      m_endingTimeout (endTimeout),
      m_pProcessBehaviour (NULL)

{

}




//
// Dtor
//
AutoEnding::~AutoEnding()
    throw()
{
    try
    {
        // if thread has been started (timeout not null)
        // stop it if not already done
        if (m_endingTimeout > 0)
	    {
	        stop();
	    }
	}
	catch (...)
	{
	}
}

void AutoEnding::set_ProcessBehaviour (MyProcessBehaviour* p_ProcessBehaviour)
{
    m_pProcessBehaviour = p_ProcessBehaviour;
}

//
// Ask to the internal thread for stopping
//
void AutoEnding::stop()
    throw (InternalErrorException)
{
	if (m_stopped == false)
	{
		try 
	    {
	        // Lock the mutex
	        m_mutex.lock();
	        
	        // set stop flag
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
void AutoEnding::run()
    throw()
{
    try 
	{
		OsSupport::Condition::WaitReturn status;
		
		// lock mutex
		CDMW_MUTEX_GUARD (m_mutex);
		
        // arm timer and wait condition
        status = m_timerCondition.wait(m_endingTimeout);  
               
        // The time has expired, stop process
        if (status == OsSupport::Condition::TIMEDOUT)
        {
            if (m_pProcessBehaviour != NULL)
            {
                m_pProcessBehaviour->autoEnd();
            }
        }
           
    }
    
	// catch any exception
	catch (...)
	{
	}
}




