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


#include <cstring>

#include "tracecollector/TraceCircularBuffer.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

namespace Cdmw
{
namespace Trace
{

const static int REALLOC_SIZE = 100; 

//
// Ctor
//
TraceCircularBuffer::TraceCircularBuffer (size_t queueSz, size_t stringSz,
                                          Message::CircularMode queueStrategy)
        throw(Cdmw::BadParameterException,
              Cdmw::OutOfMemoryException)
        : CircularBuffer (),
          m_mesg_available (getLock()),
          m_read_must_terminate (false)
{
    m_pStringWriteBuf = NULL;
    m_pStringReadBuf = NULL;
    
    try
    {
      // if invalid string buffer size
      if (stringSz == 0 )
      {
        CDMW_THROW2(Cdmw::BadParameterException, 
                     "Bad String Buffer size","== 0");
      }

      // create string buffers
      m_stringWriteBufSz = stringSz;
      m_pStringWriteBuf = new char[m_stringWriteBufSz];

      m_stringReadBufSz = stringSz;
      m_pStringReadBuf = new char[m_stringReadBufSz];


    }
    catch (const std::bad_alloc &)
    {
      if (m_pStringWriteBuf != NULL)
      {
        delete[] m_pStringWriteBuf;
        m_pStringWriteBuf = NULL;
      }

      m_stringWriteBufSz = 0;      

      if (m_pStringReadBuf != NULL)
      {
        delete[] m_pStringReadBuf;
        m_pStringReadBuf = NULL;
      }
      
      m_stringReadBufSz = 0;

      CDMW_THROW(Cdmw::OutOfMemoryException);
    }
    
    // create circular buffer queue
    create (queueSz);

    // set queue strategy
    m_queueStrategy = queueStrategy;
    
}


//
// Dtor
//
TraceCircularBuffer::~TraceCircularBuffer ()
        throw()
{
  if (m_pStringWriteBuf != NULL)
  {
    delete[] m_pStringWriteBuf;
    m_pStringWriteBuf = NULL;
  }

  m_stringWriteBufSz = 0;

  if (m_pStringReadBuf != NULL)
  {
    delete[] m_pStringReadBuf;
    m_pStringReadBuf = NULL;
  }

  m_stringReadBufSz = 0;
}


//
// Realloc string buffer for reading if too small
//
void TraceCircularBuffer::reallocReadStringBuffer (size_t stringSz)
        throw(Cdmw::OutOfMemoryException)
{
    char* p_readBuf = NULL;

    try
    {
      // if invalid string buffer size
      if (stringSz == 0 )
      {
        return;           // ----> return if invalid size
      }

      // create new string buffer     
      p_readBuf = new char[stringSz];

      // if alloc is OK release the previous buffer
      // and store its new reference
      if (m_pStringReadBuf != NULL)
      {
        delete[] m_pStringReadBuf;
      }

      m_stringReadBufSz = stringSz;
      m_pStringReadBuf = p_readBuf;
    }
    catch (const std::bad_alloc &)
    {
      if (p_readBuf != NULL)
      {
        delete[] p_readBuf;
      }

      CDMW_THROW(Cdmw::OutOfMemoryException);
    }
}


//
// Realloc string buffer for writing if too small
//
void TraceCircularBuffer::reallocWriteStringBuffer (size_t stringSz)
        throw(Cdmw::OutOfMemoryException)
{
    char* p_writeBuf = NULL;

    try
    {
      // if invalid string buffer size
      if (stringSz == 0 )
      {
        return;           // ----> return if invalid size
      }

      // create new string buffers
      p_writeBuf = new char[stringSz];

      if (m_pStringWriteBuf != NULL)
      {
        delete[] m_pStringWriteBuf;
      }

      m_stringWriteBufSz = stringSz;
      m_pStringWriteBuf = p_writeBuf;
    }
    catch (const std::bad_alloc &)
    {
      if (p_writeBuf != NULL)
      {
        delete[] p_writeBuf;
      }

      CDMW_THROW(Cdmw::OutOfMemoryException);
    }
}




//
// Return the next message to read, if no message thread is set to wait
//
bool TraceCircularBuffer::getNextMessage (TraceMessageRaw &mesgData, bool &dataRead)
        throw(Cdmw::OutOfMemoryException)
{
    bool success = false;

    try
    {
      // set mutex for buffer updating
      CDMW_MUTEX_GUARD (getLock());

      bool loop = true;

      while (loop)
      {
        // read message from circular buffer
        success = readMessage (mesgData, dataRead);

        // if error or data has been read or reading has to be stopped
        if (success == false ||
            dataRead == true ||
            m_read_must_terminate == true)
        {
          // break the loop to return to caller
          loop = false;
        }
        // else there is no message to read
        else
        {
          // If there is no data, wait they become available
          m_mesg_available.wait();
        }
      }
    }

    catch (Cdmw::OutOfMemoryException& ex)
    {
      throw (ex);
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
      CDMW_ASSERT(false);
    }

    return success;
}

//
// Ask to the reading process for stopping
//
void TraceCircularBuffer::stopReading()
{

    // require the reading to terminate
    m_read_must_terminate = true;

    // awake the thread, if it is sleeping
    m_mesg_available.broadcast();
}



//
// Read message from circular buffer
//
// as method uses local trace message string buffer,
// this method is not designed to be called by more than one thread
//
bool TraceCircularBuffer::readMessage (TraceMessageRaw &mesgData, bool &dataRead)
        throw (Cdmw::OutOfMemoryException)
{
    // read the length of the read frame 
    size_t count = 0;
    if (getReadCount(count) == false || count < sizeof (struct BufferTraceMessage))
    {
      // there is no header to read 
      dataRead = false;
      return true;           // ----> return if no message to read
    }


    // flag to indicate message has been read
    bool messageIsRead = false;
    
    // read trace header message from circular buffer
    
    // first get the initial read position
    size_t initial_read_pos = getReadPos ();
              
    // then read trace header
    BufferTraceMessage mesgFromBuffer;
    bool success = read (&mesgFromBuffer,sizeof (struct BufferTraceMessage));

    // header has been read
    dataRead = true;

    // from header get size of string buffer and read it  
    if (success)
    {
      // check there is enough space for string else realloc
      if (mesgFromBuffer.m_messageSize > m_stringReadBufSz)
      {
        reallocReadStringBuffer (mesgFromBuffer.m_messageSize + REALLOC_SIZE);
      }

      // read string buffer
      success = read (m_pStringReadBuf,mesgFromBuffer.m_messageSize);
      
      if (success)
      {
        // read is successfull
        messageIsRead = true;        
      }            
      else
      {
        // else read is not successfull restore the initial read position
        // to restore the trace header
        setReadPos (initial_read_pos);
      }
    }
     
     
    // if message has been read
    if (messageIsRead)
    {
      // copy data in the trace message raw
      mesgData.m_messageSize = mesgFromBuffer.m_messageSize;
      mesgData.m_bodyMessageSize = mesgFromBuffer.m_bodyMessageSize;
      mesgData.m_mainHeader.m_pid = mesgFromBuffer.m_pid;
      mesgData.m_mainHeader.m_szApplicationName = mesgFromBuffer.m_szApplicationName;
      mesgData.m_mainHeader.m_szProcessName = mesgFromBuffer.m_szProcessName;
      mesgData.m_mainHeader.m_hid = mesgFromBuffer.m_hid;
      mesgData.m_mainHeader.m_szHostName = mesgFromBuffer.m_szHostName;
      mesgData.m_messageHeader.m_tid = mesgFromBuffer.m_tid;
      mesgData.m_messageHeader.m_fileLine = mesgFromBuffer.m_fileLine;
      mesgData.m_messageHeader.m_szFileName = mesgFromBuffer.m_szFileName;
      // ECR-0123
      mesgData.m_messageHeader.m_szComponentName =
          mesgFromBuffer.m_szComponentName;
      mesgData.m_messageHeader.m_szUserDomain = mesgFromBuffer.m_szUserDomain;
      mesgData.m_messageHeader.m_userLevel = mesgFromBuffer.m_userLevel;
      mesgData.m_messageHeader.m_timestamping = mesgFromBuffer.m_timestamping;
      
      
      // initialize buffer pointer
      char *pBuf = m_pStringReadBuf; 

      // set application name
      mesgData.m_mainHeader.m_pApplicationName = pBuf;
      pBuf += mesgFromBuffer.m_szApplicationName;
      
      // set process name
      mesgData.m_mainHeader.m_pProcessName = pBuf;
      pBuf += mesgFromBuffer.m_szProcessName;

      // set host name
      mesgData.m_mainHeader.m_pHostName = pBuf;
      pBuf += mesgFromBuffer.m_szHostName;

      // set file name
      mesgData.m_messageHeader.m_pFileName = pBuf;
      pBuf += mesgFromBuffer.m_szFileName;

      // set the component name
      // ECR-0123
      mesgData.m_messageHeader.m_pComponentName = pBuf;
      pBuf += mesgFromBuffer.m_szComponentName;

      // set user domain
      mesgData.m_messageHeader.m_pUserDomain = pBuf;
      pBuf += mesgFromBuffer.m_szUserDomain;

      // set message body
      mesgData.m_pBodyMessage = pBuf;
    }

    return messageIsRead;
}

//
// Flush message from circular buffer
//
bool TraceCircularBuffer::flushMessage ()
        throw ()
{
    // read the length of the read frame 
    size_t count = 0;
    if (getReadCount(count) == false || count < sizeof (struct BufferTraceMessage))
    {
      // there is no header to flush 
      return true;           // ----> return if no message to flush
    }

    // read trace header message from circular buffer
    
    // first get the initial read position
    size_t initial_read_pos = getReadPos ();
              
    // then read trace header
    bool success = read (&m_flushMessage,sizeof (struct BufferTraceMessage));

    // from header get size of string buffer and flush it  
    if (success)
    {
      success = flush (m_flushMessage.m_messageSize);

      // if read is not successfull restore the initial read position
      // to restore the trace header
      if (success == false)
      {
        setReadPos (initial_read_pos);
      }
    }
     
    return success;
}



//
// Write message into circular buffer
//
bool TraceCircularBuffer::writeMessage (const CdmwTrace::MessagesHeader& messagesHdr,
                                        const CdmwTrace::FormattedMessage& fmtMessage,
                                        WriteStatus& status)
        throw (Cdmw::OutOfMemoryException)
{
    bool success = false;

    // set writing status to OK
    status = STS_WRITE_OK;
    
    try
    {
      // set mutex for buffer updating
      CDMW_MUTEX_GUARD (getLock());

      // transfer message in local structure
      
      // first get the total size for string
      m_writeMessage.m_szApplicationName = strlen(messagesHdr.the_application_name.in()) + 1;
      m_writeMessage.m_messageSize = m_writeMessage.m_szApplicationName;
      
      m_writeMessage.m_szProcessName = strlen(messagesHdr.the_process_name.in()) + 1;
      m_writeMessage.m_messageSize += m_writeMessage.m_szProcessName;

      m_writeMessage.m_szHostName = strlen(messagesHdr.the_host_name.in()) + 1;
      m_writeMessage.m_messageSize += m_writeMessage.m_szHostName;

      m_writeMessage.m_szFileName = strlen(fmtMessage.header.the_file_name.in()) + 1;
      m_writeMessage.m_messageSize += m_writeMessage.m_szFileName;

      // ECR-0123
      m_writeMessage.m_szComponentName =
          strlen(fmtMessage.header.the_level.the_component_name.in()) + 1;
      m_writeMessage.m_messageSize += m_writeMessage.m_szComponentName;

      m_writeMessage.m_szUserDomain = strlen(fmtMessage.header.the_level.the_domain.in()) + 1;
      m_writeMessage.m_messageSize += m_writeMessage.m_szUserDomain;

      m_writeMessage.m_bodyMessageSize = strlen(fmtMessage.message.in()) + 1;
      m_writeMessage.m_messageSize += m_writeMessage.m_bodyMessageSize;

      // get total message length
      size_t message_length = sizeof (struct BufferTraceMessage) + m_writeMessage.m_messageSize;

      // check if there enough space in circular buffer
      size_t count = 0;
      bool bufferReady = false;
      bool messageDropped = false;

      // while buffer not ready
      while (!bufferReady)
      {
        // if circular buffer is full or space is not enough
        if (getWriteCount(count) == false || count < message_length)
        {
          // if old message kept strategy
          if (m_queueStrategy == Message::DROP_LATEST)
          {
            // there is no enough space to write 
            // message is ignored
            
            // set writing status
            status = STS_DROP_LATEST;
            messageDropped = true;
            
            // stop the loop
            bufferReady = true;
          }
          // else old message drop strategy : flush oldest
          else
          {
            // set writing status
            status = STS_REMOVE_OLDEST;

            bool flush_err = flushMessage();
            if (flush_err == false)
            {
              // flush cannot be done message is ignored
              status = STS_DROP_LATEST;              
              messageDropped = true;
            
              // stop the loop
              bufferReady = true;
            }
          }
        }

        // buffer is ok, message can be stored
        else
        {
          // stop the loop
          bufferReady = true;
        }
      }
      
      
      // return if message has been ignored
      if (messageDropped)
      {
        return false;         // ----> return if message ignored
      }



      // check there is enough space for string else realloc
      if (m_writeMessage.m_messageSize > m_stringWriteBufSz)
      {
        reallocWriteStringBuffer (m_writeMessage.m_messageSize + REALLOC_SIZE);
      }

      // initialize buffer pointer
      char *pBuf = m_pStringWriteBuf; 

      // set application name
      strcpy (pBuf, messagesHdr.the_application_name.in());
      pBuf += m_writeMessage.m_szApplicationName;
      
      // set process name
      strcpy (pBuf, messagesHdr.the_process_name.in());
      pBuf += m_writeMessage.m_szProcessName;

      // set host name
      strcpy (pBuf, messagesHdr.the_host_name.in());
      pBuf += m_writeMessage.m_szHostName;

      // set file name
      strcpy (pBuf, fmtMessage.header.the_file_name.in());
      pBuf += m_writeMessage.m_szFileName;

      // set the component name
      // ECR-0123
      strcpy(pBuf, fmtMessage.header.the_level.the_component_name.in());
      pBuf += m_writeMessage.m_szComponentName;

      // set user domain
      strcpy (pBuf, fmtMessage.header.the_level.the_domain.in());
      pBuf += m_writeMessage.m_szUserDomain;

      // set message body
      strcpy (pBuf, fmtMessage.message.in());
      
      // set all other fields
      m_writeMessage.m_pid = messagesHdr.the_process_id;
      m_writeMessage.m_hid = messagesHdr.the_host_id;
      
      m_writeMessage.m_tid = fmtMessage.header.the_thread_id;
      m_writeMessage.m_fileLine = fmtMessage.header.the_file_line;
      m_writeMessage.m_userLevel = fmtMessage.header.the_level.the_value;
      m_writeMessage.m_timestamping.seconds = 
                                fmtMessage.header.time_stamping.seconds;
      m_writeMessage.m_timestamping.microseconds = 
                                fmtMessage.header.time_stamping.microseconds;

      // write trace message in circular buffer
      
      // first get the initial write position
      size_t initial_write_pos = getWritePos ();
              
      // then write trace header
      success = write (&m_writeMessage,sizeof (struct BufferTraceMessage));

      // if successfull
      if (success)
      {
        // write string buffer
        success = write (m_pStringWriteBuf,m_writeMessage.m_messageSize);

        // if write is not successfull restore the initial write position
        // to drop the trace header
        if (success == false)
        {
          setWritePos (initial_write_pos);
        }
        // if successfull
        else
        {
          // We awake the disk flush thread, if it is sleeping
          m_mesg_available.broadcast();
        }
      }
    }

    catch (Cdmw::OutOfMemoryException& ex)
    {
      throw (ex);
    }
    // catch any exception from Mutex and assert it
    catch (...)
    {
      CDMW_ASSERT(false);
    }

    return success;
}

}; // End namespace Trace
}; // End namespace Cdmw


