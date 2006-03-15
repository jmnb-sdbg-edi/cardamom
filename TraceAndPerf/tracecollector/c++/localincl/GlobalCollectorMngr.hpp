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


#ifndef _CDMW_TRC_GLOBALCOLLECTORMNGR_HPP_
#define _CDMW_TRC_GLOBALCOLLECTORMNGR_HPP_

#include <string>

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/osthreads/Barrier.hpp>
#include <Foundation/orbsupport/CORBA.hpp>

#include <TraceAndPerf/idllib/CdmwTraceCollector.stub.hpp>
#include <TraceAndPerf/tracelibrary/Message.hpp>


namespace Cdmw
{
namespace Trace
{


class CollectorFlusher;
class FilterMngr;
class TraceCircularBuffer;


/**
 * This singleton class is used to bufferize messages comming from any
 * local trace collector before they are actually stored into the log files
 * by the CollectorFlusher class.
 *
 * The GlobalCollectorMngr is in charge of:
 *     - initialising the Global Collector Manager.
 *     - initialising the Collector Flusher, this object is a thread
 *       that flushes the messages to files.
 *     - wrapped the allocation done by the Collector.
 *     - wrapped the decision to trace or not a Component/Domain/Level
 *       (done by the FilterMngr).
 *     - initialised the CORBA object in charge of handling client
 *       requests.
 *
 * Features:
 *     - Thread safe
 *
 * @see  TraceCollector_impl
 * @see  CollectorFlusher
 * @see  FilterMngr
 * @see  TraceCircularBuffer
 */
class GlobalCollectorMngr
{
    friend class CollectorFlusher;

    public:
        // default trace file name.
        static const char* DEFAULT_TRACE_FILE;

#ifndef _MSC_VER
        /**
         * Default sizes for the circular buffer.
         */
        static const size_t DEFAULT_QUEUE_SZ            = 1024 * 100;
        static const size_t DEFAULT_STRING_SZ           = 1024;
        static const size_t DEFAULT_TRACE_FILE_MESG_NBR = 100;
        static const size_t DEFAULT_TRACE_FILE_BKP_NBR  = 1;
        static const size_t DEFAULT_TRACE_FILE_NBR      = 1;
#else
        enum {
            DEFAULT_QUEUE_SZ            = 100,
            DEFAULT_STRING_SZ           = 1,
            DEFAULT_TRACE_FILE_MESG_NBR = 100,
            DEFAULT_TRACE_FILE_BKP_NBR  = 1,
            DEFAULT_TRACE_FILE_NBR      = 1
        };
#endif


        /**
         * Destructor.
         */ 
        virtual 
        ~GlobalCollectorMngr()
            throw();

      
        /**
         * Create an instance of GlobalCollectorMngr.
         * 
         * @return the GlobalCollectorMngr singleton.
         */ 
        static GlobalCollectorMngr&
        Instance()
            throw();


        /**
         * Initialize the Trace collector.
         * This method must be called prior to any others.
         * The Trace collector must be terminated by calling Cleanup().
         *
         * @param tracePOA POA used for the trace servant.
         * @param domain the domain where the level must be activated
         * @param level the level to be activated
         * @param traceFormat contains format of trace (Horizontal or Vertical)
         * @param queueStrategy strategy to follow when the circular buffer is
         *                      full (whether to discard new messages or
         *                      to remove the oldest)
         * @param queueSize size of queue in circular buffer in Kbytes
         * @param stringSize size of buffer used for string data in KBytes
         * @param trace_file_name generic name for trace file
         * @param trace_file_bkp_nbr contains number of backup of trace files
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
        static CdmwTrace::Collector_ptr
        Init(PortableServer::POA_ptr tracePOA,
             const std::string& domain,
             long level,
             Message::TraceFormat trace_format = Message::V_FORMAT,
             Message::CircularMode queueStrategy = Message::REMOVE_OLDEST,
             size_t queueSize = DEFAULT_QUEUE_SZ,
             size_t stringSize = DEFAULT_STRING_SZ,
             const std::string& trace_file_name = DEFAULT_TRACE_FILE,
             short trace_file_bkp_nbr = DEFAULT_TRACE_FILE_BKP_NBR,
             short max_trace_file_nbr = DEFAULT_TRACE_FILE_NBR,
             int max_trace_message_nbr = DEFAULT_TRACE_FILE_MESG_NBR,
             OsSupport::OS::TimeMode time_mode = OsSupport::OS::LOCAL_TIME)
            throw(OutOfMemoryException,
                  BadParameterException,
                  InternalErrorException);


        /**
         * Initialize the Trace collector.
         * This method must be called prior to any others.
         * The Trace collector must be terminated by calling Cleanup().
         *
         * @param tracePOA POA used for the trace servant.
         * @param componentName the component to which the domain is associated
         * @param domain the domain where the level must be activated
         * @param level the level to be activated
         * @param traceFormat contains format of trace (Horizontal or Vertical)
         * @param queueStrategy strategy to follow when the circular buffer is
         *                      full (whether to discard new messages or
         *                      to remove the oldest)
         * @param queueSize size of queue in circular buffer in Kbytes
         * @param stringSize size of buffer used for string data in KBytes
         * @param trace_file_name generic name for trace file
         * @param trace_file_bkp_nbr contains number of backup of trace files
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
        static CdmwTrace::Collector_ptr
        Init(PortableServer::POA_ptr tracePOA,
             const std::string& componentName,
             const std::string& domain,
             long level,
             Message::TraceFormat trace_format = Message::V_FORMAT,
             Message::CircularMode queueStrategy = Message::REMOVE_OLDEST,
             size_t queueSize = DEFAULT_QUEUE_SZ,
             size_t stringSize = DEFAULT_STRING_SZ,
             const std::string& trace_file_name = DEFAULT_TRACE_FILE,
             short trace_file_bkp_nbr = DEFAULT_TRACE_FILE_BKP_NBR,
             short max_trace_file_nbr = DEFAULT_TRACE_FILE_NBR,
             int max_trace_message_nbr = DEFAULT_TRACE_FILE_MESG_NBR,
             OsSupport::OS::TimeMode time_mode = OsSupport::OS::LOCAL_TIME)
            throw(OutOfMemoryException,
                  BadParameterException,
                  InternalErrorException);


        /**
         * Free allocated memories and stop the trace collector.
         * 
         * @exception InternalErrorException.
         */ 
        static void
        Cleanup()
            throw(InternalErrorException);


        /**
         * Write a message.
         * 
         * @param messagesHd the main header of trace message
         *                   from collector servant.
         * @param fmtMessage the formatted trace message
         *                   from collector servant.
         *
         * @return true if successful.
         *
         * @exception OutOfMemoryException.
         */
        bool
        writeMessage(const CdmwTrace::MessagesHeader& messagesHdr,
                     const CdmwTrace::FormattedMessage& fmtMessage)
            throw (OutOfMemoryException);


        /**
         * Check if the specified level is activated.
         *
         * @param domain look for the specified level in this domain.
         * @param level the level.
         *
         * @return true or false.
         */
        static bool
        Is_to_be_traced(const std::string& domain, long level);


        /**
         * Check if the specified level is activated.
         *
         * @param componentName the component to which the domain is associated
         * @param domain look for the specified level in this domain
         * @param level the level
         *
         * @return true or false
         */
        static bool
        Is_to_be_traced(const std::string& componentName,
                        const std::string& domain,
                        long level);


    private:
        /**
         * Constructor.
         *
         * @param tracePOA POA used for the trace servant.
         * @param domain the domain where the level must be activated
         * @param level the level to be activated
         * @param traceFormat contains format of trace (Horizontal or Vertical)
         * @param queueStrategy strategy to follow when the circular buffer is
         *                      full (whether to discard new messages or
         *                      to remove the oldest)
         * @param queueSize size of queue in circular buffer in Kbytes
         * @param stringSize size of buffer used for string data in KBytes
         * @param trace_file_name generic name for trace file
         * @param trace_file_bkp_nbr contains number of backup of trace files
         * @param max_trace_file_nbr max number of trace files
         * @param max_trace_message_nbr max number of messages in a log file
         * @param time_mode GMT or local mode for the time
         *
         * @exception OutOfMemoryException
         * @exception BadParameterException
         * @exception InternalErrorException
         */
        GlobalCollectorMngr(PortableServer::POA_ptr tracePOA,
                            const std::string& domain, 
                            long level,
                            Message::TraceFormat trace_format,
                            Message::CircularMode queueStrategy,
                            size_t queueSize,
                            size_t stringSize,
                            const std::string& trace_file_name,
                            short trace_file_bkp_nbr, 
                            short max_trace_file_nbr, 
                            int max_trace_message_nbr,
                            OsSupport::OS::TimeMode time_mode)
            throw(OutOfMemoryException,
                  BadParameterException,
                  InternalErrorException);


        /**
         * Constructor.
         *
         * @param tracePOA POA used for the trace servant.
         * @param componentName the component to which the domain is associated
         * @param domain the domain where the level must be activated
         * @param level the level to be activated
         * @param traceFormat contains format of trace (Horizontal or Vertical)
         * @param queueStrategy strategy to follow when the circular buffer is
         *                      full (whether to discard new messages or
         *                      to remove the oldest)
         * @param queueSize size of queue in circular buffer in Kbytes
         * @param stringSize size of buffer used for string data in KBytes
         * @param trace_file_name generic name for trace file
         * @param trace_file_bkp_nbr contains number of backup of trace files
         * @param max_trace_file_nbr max number of trace files
         * @param max_trace_message_nbr max number of messages in a log file
         * @param time_mode GMT or local mode for the time
         *
         * @exception OutOfMemoryException
         * @exception BadParameterException
         * @exception InternalErrorException
         */
        GlobalCollectorMngr(PortableServer::POA_ptr tracePOA,
                            const std::string& componentName,
                            const std::string& domain,
                            long level,
                            Message::TraceFormat trace_format,
                            Message::CircularMode queueStrategy,
                            size_t queueSize,
                            size_t stringSize,
                            const std::string& trace_file_name,
                            short trace_file_bkp_nbr,
                            short max_trace_file_nbr,
                            int max_trace_message_nbr,
                            OsSupport::OS::TimeMode time_mode)
            throw(OutOfMemoryException,
                  BadParameterException,
                  InternalErrorException);


        /**
         * Copy constructor.
         */ 
        GlobalCollectorMngr(const GlobalCollectorMngr& rhs)
            throw();


        /**
         * Assignement operator.
         */ 
        GlobalCollectorMngr&
        operator=(const GlobalCollectorMngr& rhs)
            throw();


        /**
         * This variable is set to true when the init() function has
         * been called. The Cleanup() function resets this variable to false.
         */
        static bool M_initDone;


        /**
         * The singleton.
         */
        static
        GlobalCollectorMngr* M_pSingleton; 


        /**
         * The object in charge of flushing data to the trace file.
         */
        std::auto_ptr<CollectorFlusher> m_spCollectorFlusher;


        /**
         * This barrier is used for synchronizing with the CollectorFlusher
         * thread.
         */
        OsSupport::Barrier m_barrier;


        /**
         * The global trace collector.
         */
        CdmwTrace::Collector_var m_traceCollector;


        /**
         * The filter manager.
         */
        FilterMngr* m_pFilterMngr;

        
        /**
         * The circular buffer.
         */
        TraceCircularBuffer* m_pCircularBuffer;
};


} // End namespace Trace
} // End namespace Cdmw


#endif // _CDMW_TRC_GLOBALCOLLECTORMNGR_HPP_
