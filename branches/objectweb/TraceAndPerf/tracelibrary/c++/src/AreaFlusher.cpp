/* =========================================================================== *
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
 * =========================================================================== */


#include <algorithm>
#include <iostream>
#include <sstream>
#include <list>

#include "Foundation/common/System.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"


#include "TraceAndPerf/tracelibrary/Message.hpp"
#include "TraceAndPerf/tracelibrary/FlushAreaMngr.hpp"

#include "tracelibrary/AreaFlusher.hpp"
#include "tracelibrary/FlushArea.hpp"
#include "tracelibrary/TraceTools.hpp"


// To get all symbol coming from the idl
using namespace CdmwTrace;


namespace Cdmw
{
namespace Trace
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
AreaFlusher::AreaFlusher (OsSupport::Barrier& barrier)
    throw (OutOfMemoryException,
           AssertionFailedException,
           InternalErrorException)

    : m_thread_must_terminate (false),
      m_flush_active(false),
      m_area_available (m_FlushArea_ctr_mtx),
      m_key_collectors (0),
      m_flushState (ASYNCHRONOUS),
      m_barrier (barrier)
{
    // Nothing to do
}




//
// Dtor
//
AreaFlusher::~AreaFlusher()
    throw()
{

    
}



//
// Transmit to the specified collector the FlushArea content
//
void AreaFlusher::send_FlushArea (CdmwTrace::Collector_ptr collector,
				                  const CdmwTrace::MessagesHeader& messagesHeader,
				                  const CdmwTrace::FormattedMessageSeq& formattedMessageSeq)
     throw ()
{
    // If we get a trouble with CORBA, we ignore it, the message will be loss for
    // this collector. Others collectors may exists, an this one may be done
    try 
	{
        collector->put_formatted_messages (messagesHeader, formattedMessageSeq);
    }
	catch (const CORBA::Exception& ex)
	{
        //TODO: maybe retry, or notify a supervision
    }
}





//
// Send the specified FlushArea to all registered collectors
//
void AreaFlusher::do_flush_area (FlushArea* pFlushArea)
     throw (OutOfMemoryException)
{
    CDMW_TRACE_DEBUG ("AreaFlusher::do_flush_area(" << pFlushArea->id() << ")");

    try 
	{
        // Firstly, we make a copy of the collector to be notified
        //  to avoid to lock it a long time
		std::list<CdmwTrace::Collector_var> collectorsList;
		
		{
          CDMW_READER_LOCK_GUARD(m_collectors_mtx);

          for (CollectorCtr::iterator itrCollector = m_collectors.begin(); 
			   itrCollector != m_collectors.end(); itrCollector++)
		  {
			collectorsList.push_back(CdmwTrace::Collector::_duplicate(
								    	itrCollector->second.collectorRef.in()));
          }
        }

        // We transform the FlushArea into a sequence of message
		CdmwTrace::FormattedMessageSeq_var formattedMessageSeq = pFlushArea->create_FormattedMessageSeq();
		CdmwTrace::MessagesHeader_var messagesHeader = pFlushArea->create_MessagesHeader();

        // And we sent it to each Collector
        std::list<CdmwTrace::Collector_var>::iterator itrCollector; 
        for (itrCollector = collectorsList.begin(); 
			 itrCollector != collectorsList.end(); itrCollector++)
		{
            send_FlushArea ((*itrCollector).in(), messagesHeader.in(), formattedMessageSeq.in());
        }

        // We can delete the content of the flush area and release this flush area
        // to the flush area manager
        pFlushArea->reset();
        FlushAreaMngr::instance().put_flushedArea(pFlushArea);
    } 
	catch (std::bad_alloc&)
	{
        CDMW_THROW (OutOfMemoryException);
    }
	
	// catch any exception from Mutex and assert it
	catch (...)
	{
	    CDMW_ASSERT(false == true);
	}
}




//
// Return the next FlushArea to be flushed
//
FlushArea* AreaFlusher::get_next_FlushArea()
    throw (AssertionFailedException,
           InternalErrorException)
{
    try
	{
      CDMW_MUTEX_GUARD (m_FlushArea_ctr_mtx);

      // We check, if someone has not ask the thread to terminate
      while (! m_thread_must_terminate) 
	  {
        // If there is Area to be flushed, we extract the next one
        //  and return it
        if (m_flush_active && m_area_to_be_flushed.size() != 0)
		{
            // We look for an Area available for flush:
            //  - inserted in the list of 'to be flushed'
            //  - no more Messages are already in used
            FlushAreaCtr::iterator itrArea = m_area_to_be_flushed.begin(); 

            // If we have found an area to be flushed, we return it
            if (itrArea != m_area_to_be_flushed.end())
			{
                FlushArea*  pFlushArea = *itrArea;
                m_area_to_be_flushed.erase (itrArea);

                return pFlushArea;
            }
        } 

        // If there is no data, we wait they become available
        m_area_available.wait();
      }
    }

	catch (AssertionFailedException&)
	{
	    throw;
	}
	
	catch (InternalErrorException&)
	{
	    throw;
	}
	
	// catch any exception from Mutex and assert it
	catch (...)
	{
	    CDMW_ASSERT(false == true);
	}

    // Here the thread must terminate
    return NULL;
}



//
// Ask to the internal thread for stopping
//
void AreaFlusher::stop_thread()
    throw (InternalErrorException)
{

    // We require the thread to terminate
    m_thread_must_terminate = true;

    // We awake the thread, if it is sleeping
    m_area_available.broadcast();

    try 
	{
        // We wait the termination of our internal thread
        this->join();

    // No explanation, how is it possible : corrupted
    } 
	catch (const OsSupport::DeadlockException&)
	{
        CDMW_THROW (InternalErrorException);
    }
}



//
// Activate message flushing 
//
void AreaFlusher::activate_flushing ()
    throw()
{
    m_flush_active = true;
    
    try 
	{
        CDMW_MUTEX_GUARD (m_FlushArea_ctr_mtx);

        // We awake the thread, if it is sleeping
        m_area_available.broadcast();

    }
	// catch any exception from Mutex and assert it
	catch (...)
	{
	    CDMW_ASSERT(false);
	}
}


//
// Deactivate message flushing 
//
void AreaFlusher::deactivate_flushing ()
    throw()
{
    m_flush_active = false;
}


//
// Call in a new thread by the Thread system
//
void AreaFlusher::run()
    throw()
{
    try 
	{
      // We notify the FlushAreaMngr, we are ready
      m_barrier.wait();

      // Infinite loop, we stop when get_next_FlushArea()
      //  returns NULL meaning that someone has ask
      //  to stop the thread.
      do 
	  {
        FlushArea* pFlushArea = get_next_FlushArea();

        if (pFlushArea != NULL)
        {
            do_flush_area (pFlushArea);
        }
        // else NULL is returned : stop the thread
        else
        {
            try
            {
                // before stopping flush the remaining areas to be flushed
                for (FlushAreaCtr::iterator itrArea = m_area_to_be_flushed.begin();
                     itrArea != m_area_to_be_flushed.end() ; ++itrArea)
                {
                    do_flush_area (*itrArea);
                }                
            }
            catch (...)
	        {
	            // do nothing
	        }
            
            return;
        }
        
      } while (true);
    }
	// catch any exception and assert it
	catch (...)
	{
	  CDMW_ASSERT(false);
	}
}


//
// Transfer the flush area into the to do list for the 
//  background thread
//
void AreaFlusher::do_asynchronousFlush (FlushArea* area_to_flush)
      throw (OutOfMemoryException,
             InternalErrorException)
{
    CDMW_TRACE_DEBUG("AreaFlusher::do_asynchronousFlush(" << area_to_flush->id() << ")" );

    try 
	{
        CDMW_MUTEX_GUARD (m_FlushArea_ctr_mtx);

        CDMW_TRACE_DEBUG("AreaFlusher : flush area " << area_to_flush->id() << " to be flushed");
 
        // We insert it in the list of area to be flushed
        m_area_to_be_flushed.push_back (area_to_flush);

        // We awake the thread, if it is sleeping
        m_area_available.broadcast();

    }
    catch (const std::bad_alloc&)
    {
        CDMW_THROW (OutOfMemoryException);
    }
	catch (InternalErrorException&)
	{
	    throw;
	}
	
	// catch any exception from Mutex and assert it
	catch (...)
	{
	    CDMW_ASSERT(false);
	}
}




//
// Send the flush area to the Collectors
//
void AreaFlusher::do_synchronousFlush (FlushArea* pFlushArea)
     throw (InternalErrorException)
{
    if (m_flush_active)
    {
        CDMW_TRACE_DEBUG("AreaFlusher::do_synchronousFlush(" << pFlushArea->id() << ")" );

        // We directly asked to send the FlushArea
        do_flush_area (pFlushArea);
    }
}



//
// Register the area to be flushed
//
void AreaFlusher::flush_area (FlushArea* pFlushArea)
     throw (OutOfMemoryException, 
			InternalErrorException)
{
    switch (m_flushState)
	{
        case SYNCHRONOUS:
            do_synchronousFlush (pFlushArea);
            break;

        case ASYNCHRONOUS:
            do_asynchronousFlush (pFlushArea);
            break;

        default:
            CDMW_NEVER_HERE();
            break;
    }
}



//
// Register a new collector and return an id for it
//
unsigned long AreaFlusher::register_collector(CdmwTrace::Collector_ptr collector,
				                              const char* collectorObjName,
				                              const char* collectorMnemoName)
	 throw (CdmwTrace::TraceProducer::InvalidRegistration,
            OutOfMemoryException)
{
    try 
	{
        CDMW_WRITER_LOCK_GUARD (m_collectors_mtx);
        
		if (m_key_collectors < 0xffffffff)
        {
          // We assign the next ID
          m_key_collectors++;

		  // create Collector descriptor
		  struct CollectorDescriptor collector_desc;
		  collector_desc.collectorMnemoName = collectorMnemoName;
		  collector_desc.collectorObjName = collectorObjName;
		  collector_desc.collectorRef = CdmwTrace::Collector::_duplicate(collector);

		  // insert new collector in the map
          m_collectors[m_key_collectors] = collector_desc;
        }
		else
	    {
		  std::ostringstream reason;
		  reason << "max collector id reached" << m_key_collectors;
				  
		  CdmwTrace::TraceProducer::InvalidRegistration e;
		  e.reason = CORBA::string_dup(reason.str().c_str());
	      throw e;
		}
    } 
	catch (std::bad_alloc&) 
	{
        CDMW_THROW (OutOfMemoryException);
    }
	catch (CdmwTrace::TraceProducer::InvalidRegistration&) 
	{
        throw;
    }
	
	// catch any exception from Mutex and assert it
	catch (...)
	{
	    CDMW_ASSERT(false == true);
	}

    return m_key_collectors;
}

//
// Unregister a collector
//
void AreaFlusher::unregister_collector (unsigned long idCollector)
	 throw (CdmwTrace::TraceProducer::InvalidRegistration,
            OutOfMemoryException)
{
    try
	{
        CDMW_WRITER_LOCK_GUARD (m_collectors_mtx);

        int key_erase = m_collectors.erase (idCollector);

		// if collector not exist, throw exception
		if (key_erase == 0)
		{
		  std::ostringstream reason;
		  reason << "not found collector id :" << idCollector;
				  
		  CdmwTrace::TraceProducer::InvalidRegistration e;
		  e.reason = CORBA::string_dup(reason.str().c_str());
	      throw e;
		}
    } 
	catch (std::bad_alloc&)
	{
        CDMW_THROW (OutOfMemoryException);
    }
	catch (CdmwTrace::TraceProducer::InvalidRegistration&) 
	{
        throw;
    }
	
	// catch any exception from Mutex and assert it
	catch (...)
	{
	    CDMW_ASSERT(false == true);
	}
}


//
//
//
CdmwTrace::TraceProducer::RegistrationSeq* AreaFlusher::get_registered_collectors ()
     throw(OutOfMemoryException)
{
    CdmwTrace::TraceProducer::RegistrationSeq* p_collectorSeq = NULL;

    try
    {
      CDMW_READER_LOCK_GUARD (m_collectors_mtx);

      // allocate the collector sequence with the number of registered collectors
	  p_collectorSeq = new CdmwTrace::TraceProducer::RegistrationSeq;
      p_collectorSeq->length(m_collectors.size());
    
      // fill sequence with collector mnemonic name, object name and ident
      CollectorCtr::iterator itrCollector = m_collectors.begin(); 
      for (int i=0; 
  	       itrCollector != m_collectors.end(); itrCollector++,i++)
      {
	    (*p_collectorSeq)[i].collectorId = itrCollector->first;
	    (*p_collectorSeq)[i].collectorMnemoName = 
			  CORBA::string_dup((itrCollector->second).collectorMnemoName.c_str());
	    (*p_collectorSeq)[i].collectorObjName = 
			  CORBA::string_dup((itrCollector->second).collectorObjName.c_str());
      }
    }

    catch (std::bad_alloc&)
    {
      CDMW_THROW (OutOfMemoryException);
    }
	
	// catch any exception from Mutex and assert it
	catch (...)
	{
	    CDMW_ASSERT(false == true);
	}
  
    return p_collectorSeq;
}    


//
// Change the current flushing mode
//
void AreaFlusher::set_flushState (FlushState flushState)
    throw()
{
    m_flushState = flushState;
}

} // End namespace Trace
} // End namespace Cdmw

