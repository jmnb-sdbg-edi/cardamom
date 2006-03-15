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


#ifndef INCL_TRACE_AREAFLUSHER_HPP 
#define INCL_TRACE_AREAFLUSHER_HPP 

#include "Foundation/common/System.hpp"

#include <vector>
#include <map>


#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/Barrier.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "TraceAndPerf/idllib/CdmwTraceCollector.stub.hpp"
#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.skel.hpp"

#include "TraceAndPerf/tracelibrary/Message.hpp"

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW Trace types and data.
*/
namespace Trace
{

class FlushArea;

/**
*Purpose:
*<p>    This class output the content of a FlushArea
*       to the registered Collectors.
*
*Features:
*<p> Thread safe, exception safe
*
*@see  FlushAreaMngr 
*@see  FlushArea
*
*/
class AreaFlusher : public OsSupport::Thread
{

    public:


		/**
		* This structure describes a collector 
	    */
		struct CollectorDescriptor
	    {
		  /**
		  * The mnemonic of collector
		  */
		  std::string collectorMnemoName;

		  /**
		  * The name of collector object
		  */
		  std::string collectorObjName;

		  /**
		  * The object reference of collector
		  */
		  CdmwTrace::Collector_var collectorRef;
        };


        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param barrier the barrier is used by the internal thread
        *       to notify its creator it is ready to operate
        *
        *@exception OutOfMemoryException
        *@exception AssertionFailedException
        *@exception InternalErrorException
        *
        */ 
        AreaFlusher (OsSupport::Barrier& barrier)
                throw (OutOfMemoryException,
                       AssertionFailedException,
                       InternalErrorException);




        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~AreaFlusher()
                throw();




        /**
        * Purpose:
        * <p> Take possession of the specified FlushArea
        *   and schedule it to be flushed out ot the
        *   collector.
        *   if the AreaFluser is set to SYNCHRONOUS, the
        *   the area is immediately flushed from the caller
        *   thread.
        *   if the AreaFluser is set to ASYNCHRONOUS, the
        *   FlushArea is recorded, and will be flushed after
        *   from another thread.
        * 
        *@param area_to_flush area to be flushed
        *
        *@exception OutOfMemoryException
        *@exception InternalErrorException
        *
        */ 
        void flush_area (FlushArea* area_to_flush)
            throw (OutOfMemoryException,
				   InternalErrorException);

        
        /**
        * Purpose:
        * <p> Register the specified Collector. When message
        * will be available, the collector will received all
        * message. The collector may be suppress of the 
        * receiver list, by calling AreaFlusher::unregister
        * 
        *@param collector the collector to be notified from now
        *
        *@param collectorObjName the name of collector object
        *
        *@param collectorMnemoName the mnemonic name of collector
        *
        *@return the registration id of collector
		*
        *@exception InvalidRegistration
		*           OutOfMemoryException
        *
        *@see unregister_collector
        */ 
        unsigned long register_collector (CdmwTrace::Collector_ptr collector,
						                  const char* collectorObjName,
						                  const char* collectorMnemoName)
			throw (CdmwTrace::TraceProducer::InvalidRegistration,
                   OutOfMemoryException);



        
        /**
        * Purpose:
        * <p> Unregister the specified Collector. The id to used
        * has been returned from a previous call to register_collector
        * The specified collector will no more receive message. If
        * there is messages in processing, the Collector will received
        * these messages, but no more after.
        * If the id doesn't correspond to an existing collector, the
        * call return without doing anything.
        *
        *@param collector the collector to be notified from now
        *
        *@exception InvalidRegistration
		*           OutOfMemoryException
        *
        *@see register_collector
        */ 
        void unregister_collector (unsigned long idCollector)
			throw (CdmwTrace::TraceProducer::InvalidRegistration,
                   OutOfMemoryException);


		/**
		* Purpose:
		* <p> get the sequence of registered collectors 
		* 
		*@return The sequence of Collectors
		*
		*@exception CORBA::SystemException
		*/ 
		CdmwTrace::TraceProducer::RegistrationSeq* get_registered_collectors ()
		    throw(OutOfMemoryException);
						

				
        
        /**
        * Purpose:
        * <p> Ask to the internal thread to stop its job, the thread
        * will exit only when the current FlushArea, if it is the case,
        * will have been flushed to the collector. When this function 
        * return the internal thread have stopped.
        *
        *@exception InternalErrorException
        *
        */ 
        void stop_thread()
            throw (InternalErrorException);


        
        /**
	    * Purpose:
	    * <p> activate trace message flushing by AreaFlusher
        *
        */
        void activate_flushing ()
            throw();

        /**
	    * Purpose:
	    * <p> deactivate trace message flushing by AreaFlusher
        *
        */
        void deactivate_flushing ()
            throw();
            
            
        /**
        * Determine the different state use to send data to collectors
        */
        enum FlushState {

            /**
            * The thread calling flush_area() is blocked until all
            * data have been transmitted.
            */
            SYNCHRONOUS,

            /**
            * The thread calling flush_area() returns immediatly, the
            * data will be sent to collectors in another thread.
            */
            ASYNCHRONOUS
        };


        
        /**
        * Purpose:
        * <p> Change the current FlushState.
        *
        */ 
        void set_flushState (FlushState flushState)
            throw();


    protected:



        /**
        * Purpose:
        * <p> Call by the underlying thread system in a new thread
        * 
        */ 
        virtual
        void run()
            throw();


        
        /**
        * Purpose:
        * <p> Returns the next FlushArea to be sent to 
        *   the registered Collectors
        *
        * @return the next FlushArea to be flushed out
        *
        *@exception AssertionFailedException
        *@exception InternalErrorException        
        *
        */ 
        FlushArea* get_next_FlushArea()
                throw (AssertionFailedException,
                       InternalErrorException);


        
        /**
        * Purpose:
        * <p> Send the specified FlushArea to the 
        *   specified Collector
        *
        *@param collector the receiver Collector
        *@param pFlushArea the are to be flushed out
        *
        *@exception OutOfMemoryException
        *
        */ 
        void send_FlushArea (CdmwTrace::Collector_ptr collector,
							 const CdmwTrace::MessagesHeader& messagesHeader,
						     const CdmwTrace::FormattedMessageSeq& formattedMessageSeq)
            throw ();

        


        /**
        * Purpose:
        * <p> Send the specified FlushArea to all registered 
        *   Collectors
        *
        *@param pFlushArea the are to be flushed out
        *
        *@exception OutOfMemoryException
        *
        */ 
        void do_flush_area (FlushArea* pFlushArea)
            throw (OutOfMemoryException);


        /**
        * Purpose:
        * <p> Flush the specified FlushArea from the caller thread
        *
        *@param pFlushArea the are to be flushed out
        *
        *@exception InternalErrorException
        *
        *@see do_asynchronousFlush
        */ 
        void do_synchronousFlush (FlushArea* pFlushArea)
            throw (InternalErrorException);


        
        /**
        * Purpose:
        * <p> Flush the specified FlushArea from a background
        * thread.
        *
        *@param pFlushArea the are to be flushed out
        *
        *@exception OutOfMemoryException
        *@exception InternalErrorException
        *
        *@see do_synchronousFlush
        */ 
        void do_asynchronousFlush (FlushArea* pFlushArea)
            throw (OutOfMemoryException,
                   InternalErrorException);


    private:

        /**
        * Define the container used to storea all FlushArea
        * currently managed by the AreaFlusher (flushed out or
        * not).
        */
        typedef std::vector <FlushArea*> FlushAreaCtr;



        /**
        * Define the container used to store all registered
        * Collector, to be notified in case of message.
        */
        typedef std::map <unsigned long, struct CollectorDescriptor> CollectorCtr;




        //
        // Copy ctor
        //
        AreaFlusher (const AreaFlusher& rhs)
                throw();




        //
        // Assignement operator
        //
        AreaFlusher& operator=(const AreaFlusher& rhs)
                 throw();


        /**
        * At each iteration the thread test this variable, to
        * see if it must stop it's processing. If true
        * the thread terminates.
        */
        bool m_thread_must_terminate;


        /**
        * When this variable becomes true, the internal thread
        * can flush an area when timer triggered
        */
        bool m_flush_active;
        
        
        /**
        * Mutex protecting the FlushArea containers
        */
        OsSupport::Mutex m_FlushArea_ctr_mtx;



        /**
        * Condition on which the thread is sleeping
        * if there is no more job.
        */
        OsSupport::Condition m_area_available;




        /**
        * Contains the list of all FlushArea ready to be flushed
        * but not already flush.
        * This list mut be manipulated with the m_FlushArea_ctr_mtx
        * Mutex acquired.
        */ 
        FlushAreaCtr m_area_to_be_flushed;


        /**
        * Read/Write Mutex protecting the m_collectors list
        */
		OsSupport::ReaderWriterLock m_collectors_mtx;

        

        /**
        * Contains the list of all collector registered
        * to be notified when there is messages. This list
        * must be manipulated with the m_collectors_mtx acquired
        */
        CollectorCtr m_collectors;

        /**
        * Next key available to be assigned to collectors
        * after their registration. This variable
        * must be manipulated with the m_collectors_mtx acquired
        */
        unsigned long m_key_collectors;


        /**
        * Specify the current mode used to send data to Collectors.
        *
        * @see FlushState
        *
        */
        FlushState m_flushState;


        /**
        * This barrier is used to be synchronised with the FlushAreaMngr.
        * By signalling this barrier we are able to notify it, when the
        * thread is operational.
        */
        OsSupport::Barrier& m_barrier;           


}; // End class AreaFlusher 

} // End namespace Trace
} // End namespace Cdmw
#endif // INCL_TRACE_AREAFLUSHER_HPP

