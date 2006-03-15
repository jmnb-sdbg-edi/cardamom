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


#ifndef INCL_TRACE_COLLECTORMNGR_HPP 
#define INCL_TRACE_COLLECTORMNGR_HPP 

#include <string>

#include "TraceAndPerf/tracelibrary/Message.hpp"

#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/osthreads/Barrier.hpp"
#include "Foundation/ossupport/OS.hpp"

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

class CollectorFlusher;
class FilterMngr;
class TraceCircularBuffer;



/**
*Purpose:
*<p>    This class is used as a singleton to bufferize message comming from any TraceProducer
*       before to be write into trace file by CollectorFlusher class.
*       The CollectorMngr is in charge of:
*           - initialising the Collector Manager used to bufferize comming trace message
*           - initialising the Collector Flusher, this object is a thread
*             which flushes the trace message to trace file
*           - wrapped the allocation done by the Collector
*           - wrapped the decision to trace or not a Domain/Level
*             (done by the FilterMngr)
*           - initialised the CORBA object in charge of handling client
*             requests (TraceCollector).
*
*Features:
*<p> Thread safe
*
*@see  TraceCollector_impl
*@see  CollectorFlusher
*@see  FilterMngr
*@see  TraceCircularBuffer
*
*/
class CollectorMngr
{

    friend class CollectorFlusher;

    public:

        // define default trace file name
        static const char* DEFAULT_TRACE_FILE;

#       ifndef _MSC_VER
        /**
        * Default size for message circular buffer
        */
        static const size_t DEFAULT_QUEUE_SZ=1024*100;
        static const size_t DEFAULT_STRING_SZ=1024;
        static const size_t    DEFAULT_TRACE_FILE_MESG_NBR=100;
        static const size_t    DEFAULT_TRACE_FILE_NBR=1;
#       else
        enum 
        {
            DEFAULT_QUEUE_SZ=100,
            DEFAULT_STRING_SZ=1,
            DEFAULT_TRACE_FILE_MESG_NBR=100,
            DEFAULT_TRACE_FILE_NBR=1
        };
#       endif


        

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~CollectorMngr()
            throw();


        
        /**
        * Purpose:
        * <p> Returns the CollectorMngr singleton
        * 
        *@return CollectorMngr singleton
        *
        *@exception
        */ 
        static
        CollectorMngr& Instance()
            throw ();


        /**
        * Purpose:
        * <p> Initialize the Trace collector. This function must be called
        *   before any other call. The Trace collector must be closed
        *   by a call to cleanup().
        * 
        *@param tracePOA POA used for trace servant.
        *@param domain the domain where the level must be activated
        *@param level  the level to be activated
        *@param queueStrategy contains strategie of circular when it is full
        *                     - do not store new message
        *                     - remove the oldest message
        *
        *@param queueSize size of queue in circular buffer in Kbytes
        *@param stringSize size of buffer used for string data in KBytes
        *@param trace_file_name contains generic name for trace file
        *@param max_trace_file_nbr contains max number of trace files
        *@param max_trace_message_nbr contains max number of message in a trace file
        *@param time_mode contains gmt or local mode for display time
        *
        *@return the pointer to the Collector servant object is returned
        *
        *@exception OutOfMemoryException
        *@exception BadParameterException
        *@exception InternalErrorException        
        */
        static
        CdmwTrace::Collector_ptr Init (PortableServer::POA_ptr tracePOA,
                                        const std::string& domain, 
                                        long level,
                                        Message::CircularMode queueStrategy = Message::REMOVE_OLDEST,
                                        size_t queueSize = DEFAULT_QUEUE_SZ,
                                        size_t stringSize = DEFAULT_STRING_SZ,
                                        const std::string& trace_file_name = DEFAULT_TRACE_FILE, 
                                        short max_trace_file_nbr = DEFAULT_TRACE_FILE_NBR, 
                                        int max_trace_message_nbr = DEFAULT_TRACE_FILE_MESG_NBR,
                                        OsSupport::OS::TimeMode time_mode = OsSupport::OS::LOCAL_TIME)
            throw (OutOfMemoryException,
                   BadParameterException,
                   InternalErrorException);


        /**
         * Initialize the Trace collector.
         * This method must be called prior to any others.
         * The Trace collector must be terminated by calling cleanup().
         *
         * @param tracePOA POA used for the trace servant.
         * @param componentName the component to which the domain is associated
         * @param domain the domain where the level must be activated
         * @param level the level to be activated
         * @param queueStrategy strategy to follow when the circular buffer is
         *                      full (whether to discard new messages or
         *                      to remove the oldest)
         * @param queueSize size of queue in circular buffer in Kbytes
         * @param stringSize size of buffer used for string data in KBytes
         * @param trace_file_name generic name for trace file
         * @param max_trace_file_nbr max number of trace files
         * @param max_trace_message_nbr max number of messages in a log file
         * @param time_mode GMT or local mode for the time
         *
         * @return the pointer to the Collector servant object
         *
         * @exception OutOfMemoryException
         * @exception BadParameterException
         * @exception InternalErrorException
         */
        // ECR-0123
        static CdmwTrace::Collector_ptr
        Init(PortableServer::POA_ptr tracePOA,
             const std::string& componentName,
             const std::string& domain,
             long level,
             Message::CircularMode queueStrategy = Message::REMOVE_OLDEST,
             size_t queueSize = DEFAULT_QUEUE_SZ,
             size_t stringSize = DEFAULT_STRING_SZ,
             const std::string& trace_file_name = DEFAULT_TRACE_FILE,
             short max_trace_file_nbr = DEFAULT_TRACE_FILE_NBR,
             int max_trace_message_nbr = DEFAULT_TRACE_FILE_MESG_NBR,
             OsSupport::OS::TimeMode time_mode = OsSupport::OS::LOCAL_TIME)

            throw(OutOfMemoryException,
                  BadParameterException,
                  InternalErrorException);


        /**
        * Purpose:
        * <p> Free all allocated memory and stop the trace collector
        * 
        *@exception InternalErrorException
        */ 
        static
        void Cleanup()
            throw (InternalErrorException);


        /**
        * Purpose:
        * <p> Writes the specified message for flushing in trace file
        * 
        *@param messagesHdr contains the main header of trace message from collector servant
        *@param fmtMessage contains the formatted trace message from collector servant
        *@return true if successfull
        *
        *@exception OutOfMemoryException        
        */ 
        bool writeMessage (const CdmwTrace::MessagesHeader& messagesHdr,
                           const CdmwTrace::FormattedMessage& fmtMessage)
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
        */ 
        static
        bool Is_to_be_traced (const std::string& domain, long level);


        /**
         * Check if the messages at the specified level are traced.
         *
         * @param componentName the component to which the domain is associated
         * @param domain the domain where the level must be activated
         * @param level the level to be activated
         *
         * @return true or false
         *
         * @exception OutOfMemoryException
         */
        static bool
        Is_to_be_traced(const std::string& componentName,
                        const std::string& domain,
                        long level);


    protected:
    

    private:
    
    
        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param tracePOA POA used for trace servant.
        *@param domain the domain where the level must be activated
        *@param level  the level to be activated
        *@param queueStrategy contains strategie of circular when it is full
        *                     - do not store new message
        *                     - remove the oldest message
        *
        *@param queueSize size of queue in circular buffer in Kbytes
        *@param stringSize size of buffer used for string data in KBytes
        *@param trace_file_name contains generic name for trace file
        *@param max_trace_file_nbr contains max number of trace files
        *@param max_trace_message_nbr contains max number of message in a trace file
        *@param time_mode contains gmt or local mode for display time
        *
        *@exception OutOfMemoryException
        *@exception BadParameterException
        *@exception InternalErrorException        
        */ 
        CollectorMngr (PortableServer::POA_ptr tracePOA,
                       const std::string& domain, 
                       long level,
                       Message::CircularMode queueStrategy,
                       size_t queueSize,
                       size_t stringSize,
                       const std::string& trace_file_name, 
                       short max_trace_file_nbr, 
                       int max_trace_message_nbr,
                       OsSupport::OS::TimeMode time_mode)
            throw (OutOfMemoryException,
                   BadParameterException,
                   InternalErrorException);


        /**
         * Constructor.
         *
         * @param tracePOA POA used for the trace servant.
         * @param componentName the component to which the domain is associated
         * @param domain the domain where the level must be activated
         * @param level the level to be activated
         * @param queueStrategy strategy to follow when the circular buffer is
         *                      full (whether to discard new messages or
         *                      to remove the oldest)
         * @param queueSize size of queue in circular buffer in Kbytes
         * @param stringSize size of buffer used for string data in KBytes
         * @param trace_file_name generic name for trace file
         * @param max_trace_file_nbr max number of trace files
         * @param max_trace_message_nbr max number of messages in a log file
         * @param time_mode GMT or local mode for the time
         *
         * @exception OutOfMemoryException
         * @exception BadParameterException
         * @exception InternalErrorException
         */
        // ECR-0123
        CollectorMngr(PortableServer::POA_ptr tracePOA,
                      const std::string& componentName,
                      const std::string& domain,
                      long level,
                      Message::CircularMode queueStrategy,
                      size_t queueSize,
                      size_t stringSize,
                      const std::string& trace_file_name,
                      short max_trace_file_nbr,
                      int max_trace_message_nbr,
                      OsSupport::OS::TimeMode time_mode)

            throw(OutOfMemoryException,
                  BadParameterException,
                  InternalErrorException);


        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        CollectorMngr (const CollectorMngr& rhs)
                throw();


        /**
        * Purpose:
        * <p> Assignement operator
        * 
        */ 
        CollectorMngr& operator=(const CollectorMngr& rhs)
                throw();



        /**
        * This variable is set to true, since the init() function has been called.
        * The cleanup() function reset this variable to false.
        */
        static  
        bool M_initDone;

        static
        CollectorMngr* M_pSingleton; 

        /**
        * Contains the object in charge of flushing data to the trace file
        */
        std::auto_ptr<CollectorFlusher> m_spCollectorFlusher;

        /**
        * This barrier is used to be synchronised with the CollectorFlusher
        * thread.
        */
        OsSupport::Barrier m_barrier;


        /**
        * Contains the reference handling request of the outside world:
        * change of levels, ...
        */
        CdmwTrace::Collector_var m_traceCollector;


        /**
        * Hold all Domain/Level which are to be traced or not
        */
        FilterMngr*  m_pFilterMngr;

        
        /**
        * Hold circular buffer
        */
        TraceCircularBuffer*  m_pCircularBuffer;

        
}; // End class CollectorMngr 

} // End namespace Trace
} // End namespace Cdmw
#endif // INCL_TRACE_COLLECTORMNGR_HPP

