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


#ifndef INCL_TRACE_FLUSHAREAMNGR_HPP 
#define INCL_TRACE_FLUSHAREAMNGR_HPP 

#include <vector>
#include <list>
#include <memory>

#include "TraceAndPerf/tracelibrary/Message.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/Barrier.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.skel.hpp"
#include "TraceAndPerf/idllib/CdmwTraceCollector.stub.hpp"


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
class AreaFlusher;
class FilterMngr;


// To suppress GCC warning
class Foo;


/**
*Purpose:
*<p>    This class is the central class of the Cdmw Trace service. This
*       class is used as a singleton to provide acces to the trace system
*       from everywhere. The FlushAreaMngr is in charge of:
*           - initialising the FlushArea used to contain message not
*             already output to Collectors
*           - initialising the AreaFlusher, this object is a thread
*             which send the FlushArea to the Collectors
*           - starting an internal thread, which trigger the flush of
*             area although the FlushArea is not full.
*           - wrapped the allocation done by the FlushArea
*           - wrapped the decision to trace or not a Domain/Level
*             (done by the FilterMngr)
*           - initialised the CORBA object in charge of handling client
*             requests (TraceProducer).
*
*Features:
*<p> Thread safe
*
*@see  TraceProducer_impl
*@see  FlushArea
*@see  FilterMngr
*@see  AreaFlusher
*
*/
class FlushAreaMngr : public OsSupport::Thread
{

    friend class AreaFlusher;

    public:


#       ifndef _MSC_VER
        /**
	    * Default number of area used to store messages
	    */
        static const size_t DEFAULT_AREA_NB=2;
	
	    /**
	    * Default size for each area
	    */
	    static const size_t DEFAULT_AREA_SIZE=50*1024;

	    /**
	    * Default number of messages for each area
	    */
	    static const size_t DEFAULT_AREA_NB_MESSAGE=1024;
        
	    /**
	    * Default time for flushing
	    */
	    static const size_t DEFAULT_FLUSHING_TIME=5000;	    

#       else
        enum 
		{
            DEFAULT_AREA_NB=2,
            DEFAULT_AREA_NB_MESSAGE=1024,
            DEFAULT_FLUSHING_TIME=5000,
            DEFAULT_AREA_SIZE=50*1024
        };
#       endif


        /**
        * Contains the different state of the data flush
        */
        enum FlushMode 
		{
            /**
            * The data are flushed to the Collectors in a background
            * thread.
            */
            ASYNCHRONOUS=0,

            /**
            * As a Message has been built, it is directly transfer to the
            * Collectors.
            */
            SEMI_SYNCHRONOUS=1,

            /**
            * The data are sent to the Collectors.
            */
            LINEAR=2,

            /**
            * The data are not sent to collectors, but kept in an 
            * internal circular buffer.
            */
            CIRCULAR=4
        };

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~FlushAreaMngr()
            throw();


	    
        /**
        * Purpose:
        * <p> Returns the FlushAreaMngr singleton
        * 
        *@return FlushAreaMngr singleton
	    *
        */ 
	    static
        FlushAreaMngr& instance()
            throw ();


        /**
        * Purpose:
        * <p> Initialize the Trace library. This function must be called
        *   before any other call. The Trace service must be closed
        *   by a cal to cleanup().
        * 
        *@param tracePOA POA to use for trace servant.
        *@param collectorList List of registered Trace Collectors.
		*@param domain the domain where the level must be activated
		*@param level  the level to be activated
		*@param applicationName the name of the application
		*@param processName the name of the process
        *@param flushingTime time to wait before flushing the current area
        *       although it is not full.
        *@param nbFlushArea number of FlushArea to be used to store messages
        *@param sizeFlushArea size of each FlushArea to be used to store messages
	    *@param areaNbMessage the maximum number of messages for each flush area
		*
	    *@return the pointer to the TraceProducer servant object is returned
        *
        *@exception OutOfMemoryException
        *@exception BadParameterException
        *@exception InternalErrorException        
        */
        static
        CdmwTrace::TraceProducer_ptr init (PortableServer::POA_ptr tracePOA,
						                    std::list<CdmwTrace::TraceProducer::CollectorData>& collectorList,
					                        const std::string& domain, 
											long level,
											const std::string& applicationName,
					                        const std::string& processName, 
                                            size_t flushingTime = DEFAULT_FLUSHING_TIME,
                                            size_t nbFlushArea = DEFAULT_AREA_NB, 
                                            size_t sizeFlushArea = DEFAULT_AREA_SIZE,
                                            size_t areaNbMessage = DEFAULT_AREA_NB_MESSAGE)
                
            throw (OutOfMemoryException,
                   BadParameterException,
                   InternalErrorException);




        /**
        * Purpose:
        * <p> Free all allocated memory and stop the trace service
        * 
        *@exception InternalErrorException
        */ 
        static
        void cleanup()
            throw (InternalErrorException);


        /**
        * Purpose:
        * <p> Adds the specified message for flushing
        * 
        *@param header the header of the message
        *@param body   the body of the message
        *
        *@exception InternalErrorException        
        */ 
	    bool add_message (const Message::MessageHeader& header, const std::string& body)
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
	    * Purpose:
	    * <p> set the way the data are sent or not the Collectors:
        *
        *   - ASYNCHRONOUS: the data are flushed by a background thread
        *   - SEMI_SYNCHRONOUS: when a message is fully built, it send to Collctors
        *   - LINEAR: the data are sent to Collectors
        *   - CIRCULAR: the data are not sent, but kept in an internal buffer.
        *
        */
        void set_flushMode (FlushMode flushMode)
            throw();

        
		/**
		* Purpose:
		* <p> Defines the specified level as activated
		* 
		*@param domain the domain where the level must be activated
		*@param level  the level to be activated
		*
		*@exception OutOfMemoryException
		*/ 
		void activate_level (const std::string& domain, long level)
		    throw (OutOfMemoryException);

		/**
		* Purpose:
		* <p> Defines the specified level as deactivated
		* 
		*@param domain the domain where the level must be deactivated
		*@param level  the level to be deactivated
		*
		*@exception OutOfMemoryException
		*
		*/ 
		void deactivate_level (const std::string& domain, long level)
		    throw (OutOfMemoryException);

        /**
        * Purpose:
        * <p> returns true if the specified level has been noted
        * to be traced
        * 
        *@param domain the Domain from where the level status must be returned
        *@param level the level to be interrogated
        *
        *@return true if the level is to be traced
        *
        *@exception OutOfMemoryException
		*
        */ 
        static
        bool is_to_be_traced (const std::string& domain, long level)
            throw (OutOfMemoryException);
        

        /**
		* Purpose:
		* <p> get the Application name
		* 
		*@param
		*@return A string containing the Application Name
		*@exception
		*/
		const std::string &getApplicationName ();
		
		
		/**
		* Purpose:
		* <p> get the Process name
		* 
		*@param
		*@return A string containing the Process Name
		*@exception
		*/
		const std::string &getProcessName ();

		/**
		* Purpose:
		* <p> Returns the id of the process
		* 
		*@param
		*@return  the process id
		*@exception
		*/	
		OsSupport::OS::ProcessId getProcessId ();

    protected:
    

        /**
        * Purpose:
        * <p> Call in a new thread, this thread is awake cyclicly,
        * and if the Area has been been flushed since a time specified
        * in the initialisation, the thread flushes the current area.
        */
        void run()
            throw();


        /**
	    * Purpose:
	    * <p> Sends the current flush area to the AreaFlusher thread
        * according to the flush mode.
        * Called when the current flush area is full and when
        * the timeout has expired
	    *
        * @exception OutOfMemoryException
        * @exception InternalErrorException        
        */
        void flush_area()
            throw (OutOfMemoryException,
                   InternalErrorException);


        
        /**
	    * Purpose:
	    * <p> Fill the internal store of flush area with FlushArea, whose
	    * dimension are specified.
	    *
	    * @param nbFlushArea number of FlushArea to be used
	    * @param sizeFlushArea size of each FlushArea
	    * @param areaNbMessage the maximum number of messages for each flush area
	    *
        * @exception OutOfMemoryException
	    */
        void create_FlushAreas (size_t nbFlushArea, size_t sizeFlushArea, 
						        size_t sizeOutFlushAreaMsg,size_t areaNbMessage)
            throw (OutOfMemoryException);
	

	
		/**
	    * Purpose:
	    * <p> Delete all internal used FlushArea
	    */
        void delete_FlushAreas()
            throw();
	
	


        /**
        * Purpose:
        * <p> Delivers the current flush area to the AreaFlusher
        * for actual flushing and updates the current flush area.
        * 
        * If the current flush area was empty or if there was no
        * current flush area, does nothing.
        *
	    * The m_currentFlushArea_mtx mutex is expected to
        * have been acquired.
        *
        *@exception OutOfMemoryException
        *@exception InternalErrorException        
        */
        void deliver_flushArea ()
            throw (OutOfMemoryException,
                   InternalErrorException);


        /**
	    * Purpose:
	    * <p> Updates the current flush area with the first
        * flush area from the list of available flush areas,
        * or with NULL if no flush area is available.
	    *
	    * The m_currentFlushArea_mtx mutex is expected to
        * have been acquired.
        */
        void update_currentFlushArea ()
            throw();
	    
		/**
	    * Purpose:
	    * <p> Takes ownership of the specified flush area that
        * has been released by the area flusher.
	    *
        *@param pFlushedArea the FlushArea to take ownership
        *
        *@exception OutOfMemoryException
	    */
	    void put_flushedArea (FlushArea* pFlushedArea)
            throw (OutOfMemoryException);
	    
	
        /**
        * Mutex protecting all access to the current flush area.
        */
        OsSupport::Mutex m_currentFlushArea_mtx;

	
	    /**
	    * The current available flush area
	    */
	    FlushArea* m_pCurrentFlushArea;
	    

	    /**
	    * Contains the available flush areas except the current one
	    */
        std::vector<FlushArea*> m_vpFlushArea;

	
    private:
    
        // To suppress GCC warning
        friend class Foo;
    
        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param tracePOA POA to use for trace servant.
        *@param collectorList List of registered Trace Collectors.
		*@param domain the domain where the level must be activated
		*@param level  the level to be activated
		*@param applicationName the name of the application
		*@param processName the name of the process
        *@param flushingTime time to wait before flushing the current area
        *       although it is not full.
        *@param nbFlushArea number of FlushArea to be used to store messages
        *@param sizeFlushArea size of each FlushArea to be used to store messages
	    *@param areaNbMessage the maximum number of messages for each flush area
		*
        *@exception OutOfMemoryException
        *@exception BadParameterException
        *@exception InternalErrorException        
        */ 
        FlushAreaMngr (PortableServer::POA_ptr tracePOA,
					   std::list<CdmwTrace::TraceProducer::CollectorData>& collectorList,
					   const std::string& domain, 
					   long level,
					   const std::string& applicationName,
					   const std::string& processName, 
                       size_t flushingTime,
                       size_t nbFlushArea , 
                       size_t sizeFlushArea,
                       size_t areaNbMessage)
                throw (OutOfMemoryException,
					   BadParameterException,
                       InternalErrorException);


    
    
    
        /**
        * Purpose:
        * <p> Copy Constructor
        * 
        */ 
        FlushAreaMngr (const FlushAreaMngr& rhs)
                throw();


        /**
        * Purpose:
        * <p> Assignement operator
        * 
        */ 
        FlushAreaMngr& operator=(const FlushAreaMngr& rhs)
                throw();



        /**
        * Contains the object in charge of flushing data to the 
        * collectors. This work may be done, depending on the current
        * mode, in a background thread or in the caller thread.
        */
        std::auto_ptr<AreaFlusher> m_spAreaFlusher;


        FlushMode m_flushModeThread;
        FlushMode m_flushModeBackup;

        /**
        * This variable is set to true, since the init() function has been called.
        * The cleanup() function reset this variable to false.
        */
        static  
        bool m_initDone;

        static
        FlushAreaMngr* m_pSingleton; 

        /**
        * This barrier is used to be synchronised with the AreaFlusher
        * thread.
        */
        OsSupport::Barrier m_barrier;


        
        /**
        * Minimum time allowed before flushing an area, although
        * the area is not full.
        */
        size_t m_flushingTime;


        /**
        * When this variable becomes true, the internal thread
        * can flush an area when timer triggered
        */
        bool m_flush_active;


        /**
        * Mutex protecting all access to the FlushAreaMngr.
        */
        OsSupport::Mutex m_flushAreaMngr_mtx;

        /**
        * Condition used to wake up cyclicly the flush area manager.
        */
        OsSupport::Condition m_flushingTimeout_cond;


        /**
        * When this variable becomes true, the internal thread
        * will terminate since the m_flushIsDone_cond will be
        * signaled.
        */
        bool m_thread_must_terminate;


        
        /**
        * Contains the reference handling request of the outside world:
        * registration of collector, change of levels, ...
        */
        CdmwTrace::TraceProducer_var m_traceProducer;


        /**
        * Hold all Domain/Level which are to be traced or not
        */
        FilterMngr*  m_pFilterMngr;

        /**
        * Contains the application name
        */
		std::string  m_applicationName;
		
        /**
        * Contains the process name
        */
		std::string  m_processName;

        /**
        * Contains the process id
        */
		OsSupport::OS::ProcessId m_processId;
        
}; // End class FlushAreaMngr 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string &FlushAreaMngr::getApplicationName ()
{
  return m_applicationName;
}

inline const std::string &FlushAreaMngr::getProcessName ()
{
  return m_processName;
}

inline OsSupport::OS::ProcessId FlushAreaMngr::getProcessId ()
{
  return m_processId;
}



} // End namespace Trace
} // End namespace Cdmw
#endif // INCL_TRACE_FLUSHAREAMNGR_HPP

