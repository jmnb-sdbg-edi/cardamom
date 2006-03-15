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


#ifndef INCL_TRACE_TRACECIRCULARBUFFER_HPP 
#define INCL_TRACE_TRACECIRCULARBUFFER_HPP 

#include <memory>

#include "TraceAndPerf/tracelibrary/Message.hpp"

#include "Foundation/common/Exception.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/Condition.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
#include "TraceAndPerf/idllib/CdmwTraceCommon.stub.hpp"

#include "tracecollector/CircularBuffer.hpp"

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

/**
*Purpose:
*<p>    This class is used for trace circular buffer
*       It is a specialized class of CircularBuffer class
*
*Features:
*<p> Thread safe
*
*/

class TraceCircularBuffer : public CircularBuffer
{
    
public:

    /**
    * This structure is used for Trace main header in raw format 
    */
    struct TraceMainHeaderRaw
    {
      /**
      * The process which has produced the trace.
      */
      OsSupport::OS::ProcessId m_pid;

      /**
      * The application name which has produced the trace (optionnal).
      */
      char   *m_pApplicationName;
      size_t m_szApplicationName;
      
      /**
      * The process name which has produced the trace (optionnal).
      */
      char   *m_pProcessName;
      size_t m_szProcessName;
                                                            
      /**
      * The id of the host where the trace has been produced. 
      */
      unsigned long m_hid;
                                                            
      /**
      * The host name where the trace has been produced (optionnal).
      */
      char   *m_pHostName;
      size_t m_szHostName;
    };

    /**
    * This structure is used for Trace message in raw format 
    * it includes main header, message header and length of all message strings
    */
    struct TraceMessageRaw
    {
      size_t m_messageSize;
      size_t m_bodyMessageSize;
      char   *m_pBodyMessage;
      struct TraceMainHeaderRaw m_mainHeader;
      struct Message::MessageHeaderRaw m_messageHeader;
    };

    
    /**
    * Contains the status of the circular buffer writing process
    */
    enum WriteStatus 
    {
      /**
      * The writing is without error
      */
      STS_WRITE_OK,

      /**
      * The latest received message is dropped
      */
      STS_DROP_LATEST,

      /**
      * The oldest message in buffer is removed
      */
      STS_REMOVE_OLDEST
    };
            

    /**
    * Purpose:
    * <p> Constructor
    * 
    *@param queueSz contains size of memory queue for circular buffer in Bytes
    *@param stringSz contains size of memory for string buffer in Bytes
    *@param queueStrategy contains strategie of circular when it is full
    *                     - do not store new message
    *                     - remove the oldest message
    *@exception BadParameterException
    *@exception OutOfMemoryException
    */ 
    TraceCircularBuffer (size_t queueSz, size_t stringSz,
                         Message::CircularMode queueStrategy=Message::REMOVE_OLDEST)
            throw(Cdmw::BadParameterException,
                  Cdmw::OutOfMemoryException);

    /**
    * Purpose:
    * <p> Destructor
    * 
    *@param
    *@exception
    */ 
    virtual
    ~TraceCircularBuffer ()
            throw ();

    /**
    * Purpose:
    * <p> Returns the next trace message from circular buffer,
    *     if no message in buffer, calling thread is set in waiting state,
    *     must be called by only one thread
    *     NOTE : As the method can reallocate the string buffer for reading, the 
    *     returned mesgData must be consumed before recalling this method
    *
    *@param mesgData returns message structure data stored in circular buffer
    *@param dataRead indicates if message has been read
    *@return true if no error
    *
    *@exception OutOfMemoryException
    *
    */ 
    bool getNextMessage(TraceMessageRaw &mesgData, bool &dataRead)
            throw(Cdmw::OutOfMemoryException);

    /**
    * Purpose:
    * <p> Ask to stop the message reading
    *
    *@exception
    *
    */ 
    void stopReading();


    /**
    * Purpose:
    * <p> request to write a trace message
    * 
    *@param messagesHdr contains the main header of trace message from collector servant
    *@param fmtMessage contains the formatted trace message from collector servant
    *@param status contains the status of writing process
    *@return true if successfull
    *
    *@exception OutOfMemoryException
    */ 
    bool writeMessage (const CdmwTrace::MessagesHeader& messagesHdr,
                       const CdmwTrace::FormattedMessage& fmtMessage,
                       WriteStatus& status)
            throw(Cdmw::OutOfMemoryException);
            
            

protected:

    

private:



    /**
    * This structure is used to store Trace message in circular buffer 
    */
    struct BufferTraceMessage
    {
      /**
      * The total size of message 
      * (body, application name, process name, host name, file name, user domain)
      */
      size_t m_messageSize;
      
      /**
      * The size of body message
      */
      size_t m_bodyMessageSize;

      /**
      * The process which has produced the trace.
      */
      OsSupport::OS::ProcessId m_pid;

      /**
      * The size of application name which has produced the trace
      */
      size_t m_szApplicationName;
      
      /**
      * The size of process name which has produced the trace
      */
      size_t m_szProcessName;
                                                            
      /**
      * The id of the host where the trace has been produced. 
      */
      unsigned long m_hid;
                                                            
      /**
      * The size of host name where the trace has been produced
      */
      size_t m_szHostName;
      
      
      /**
	  * The thread which has created the message.
      */
	  OsSupport::Thread::ThreadId m_tid;
		
	  /**
      * The line where the message has been produced.
	  */
      int m_fileLine;
		    
	  /**
	  * The name size of file, origin of trace
	  */
	  size_t m_szFileName;

      /**
       * The component name.
       */
      // ECR-0123
      size_t m_szComponentName;

	  /**
	  * The size of user specified domain
	  */
	  size_t m_szUserDomain;
		    
	  /**
	  * The user specified level of the message
	  */
	  int m_userLevel;
		    
	  /**
	  * The time when the trace has been produced.
	  */
	  OsSupport::OS::Timeval m_timestamping;
    };
    
    
    /**
    * Purpose:
    * <p>  Copy constructor
    * 
    *@param
    *@exception
    */ 
    TraceCircularBuffer (const TraceCircularBuffer& rhs)
            throw();

    /**
    * Purpose:
    * <p>  Assignement operator
    * 
    *@param
    *@exception
    */ 
    TraceCircularBuffer& operator=(const TraceCircularBuffer& rhs)
            throw();



    /**
    * Purpose:
    * <p> Reallocation of string buffer for reading if too small
    * 
    *@param stringSz contains new size of memory for string buffer
    *
    *@exception OutOfMemoryException
    */ 
    void reallocReadStringBuffer (size_t stringSz)
            throw(Cdmw::OutOfMemoryException);


    /**
    * Purpose:
    * <p> Reallocation of string buffer for writing if too small
    * 
    *@param stringSz contains new size of memory for string buffer
    *
    *@exception OutOfMemoryException
    */ 
    void reallocWriteStringBuffer (size_t stringSz)
            throw(Cdmw::OutOfMemoryException);

    /**
    * Purpose:
    * <p> request to read a trace message
    *     must be called by only one thread
    * 
    *@param mesgData returns message structure data stored in circular buffer
    *@param dataRead indicates if message has been read
    *@return true if no error
    *
    *@exception OutOfMemoryException
    *
    */ 
    bool readMessage (TraceMessageRaw &mesgData, bool &dataRead)
            throw(Cdmw::OutOfMemoryException);


    /**
    * Purpose:
    * <p> request to flush a trace message
    * 
    *@return true if no error
    */ 
    bool flushMessage ()
            throw();


    /**
    * Condition on which the reading thread is sleeping
    * if there is no more job.
    */
    OsSupport::Condition m_mesg_available;

    /**
    * strategy of trace circular buffer wheh it is full
    */ 
    Message::CircularMode m_queueStrategy;

    /**
    * write message header buffer
    */ 
    struct BufferTraceMessage m_writeMessage;

    /**
    * size of string write buffer
    */ 
    size_t  m_stringWriteBufSz;

    /**
    * string write buffer pointer
    */ 
    char*   m_pStringWriteBuf;

    /**
    * flush message header buffer
    */ 
    struct BufferTraceMessage m_flushMessage;

    /**
    * size of string read buffer
    */ 
    size_t  m_stringReadBufSz;

    /**
    * string read buffer pointer
    */ 
    char*   m_pStringReadBuf;
    
    
    /**
    * At each iteration the thread test this variable, to
    * see if it must stop it's reading processing. If true
    * the read waiting terminates.
    */
    bool m_read_must_terminate;


}; // End class TraceCircularBuffer 

}; // End namespace Trace
}; // End namespace Cdmw
#endif // INCL_TRACE_CIRCULARBUFFER_HPP

