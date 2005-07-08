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


#include "tracecollector/CircularBuffer.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

namespace Cdmw
{
namespace Trace
{

//
// Ctor
//
CircularBuffer::CircularBuffer ()
        throw()
    : m_read(0),
      m_write(0),
      m_queueSz(0),
      m_wrapType(idReadWrap),
      m_pQueue(NULL)
{
}


//
// Dtor
//
CircularBuffer::~CircularBuffer ()
        throw()
{
    release();
}

//
// Create the memory queue
//
void CircularBuffer::create (const size_t queueSz)
        throw(Cdmw::BadParameterException,
              Cdmw::OutOfMemoryException)
{
    try
    {
      // if invalid queue size
      if (queueSz <= 0 )
      {
        CDMW_THROW2(Cdmw::BadParameterException, 
                     "Bad Circular Buffer Queue size", "<= 0");
      }

      // create queue
      m_queueSz = queueSz;
      m_pQueue = new char[m_queueSz];
    }
    catch (const std::bad_alloc &)
    {
      m_queueSz = 0;
      m_pQueue = 0;
      CDMW_THROW(Cdmw::OutOfMemoryException);
    }
}    


//
// Release the memory queue
//
void CircularBuffer::release ()
        throw ()
{
    if (m_pQueue == 0)
    {
      return;
    }

    // reset position pointers
    reset();

    // release queue
    delete[] m_pQueue;
    m_pQueue   = 0;
    m_queueSz = 0;
}




//
// Reset position pointers and wrapping
//
void CircularBuffer::reset ()
        throw ()
{
    // reset position pointers and wrap
    m_read  = 0;
    m_write    = 0;
    m_wrapType = idNoWrap;
}


//
// Clear Circular Buffer : reset position pointers and reset memory
//
void CircularBuffer::clear ()
        throw ()
{
    // reset position pointers
    reset();

    // clear memory queue
    memset (m_pQueue, 0, m_queueSz);
}


// 
// increment read/write position pointers based
// on operation
// note : incrementation size is under control of getCount method
//        which gives the max number of bytes to read or write in memory queue
//
void CircularBuffer::incPos (const size_t inc, const long op)
        throw (Cdmw::BadParameterException)
{
    // if inc > queue size stop
    if (inc > m_queueSz)
    {
      CDMW_THROW2(Cdmw::BadParameterException, 
                   "Bad Circular Buffer Pointer increment",
                   "> queue size");
    }

    // get pos to modify
    unsigned long *p_pos = &m_write;
    if (op == idRead)
    {
      p_pos = &m_read;
    }

    // get length from pointer position to queue end
    size_t lenToEnd = getLengthToEnd(op);

    // if incrementation not beyond queue end
    if (inc <= lenToEnd)
    {
      *p_pos += inc;
    }
    else
    {
      // set pos to length from start
      // because of wrap
      *p_pos = inc - lenToEnd;
    }

    // if either pos has wrapped to other : read/write position pointer are equal
    if (m_read == m_write)
    {
      // if write operation : queue is full
      if (op == idWrite)
      {
        m_wrapType = idWriteWrap;
      }
      // else read operation : queue is empty
      else
      {
        m_wrapType = idReadWrap;
      }
    }
    else
    {
      m_wrapType = idNoWrap;
    }
}


//
// set read/write position pointers based
// on operation
//
void CircularBuffer::setPos (const unsigned long set, const long op)
        throw (Cdmw::BadParameterException)
{
    // if set > queue size stop
    if (set > m_queueSz)
    {
      CDMW_THROW2(Cdmw::BadParameterException, 
                   "Bad Circular Buffer Pointer setting",
                   "> queue size");
    }

    // get pos to modify
    unsigned long *p_pos = &m_write;
    if (op == idRead)
    {
      p_pos = &m_read;
    }

    // set new position
    *p_pos = set;

    // if either pos has wrapped to other : read/write position pointer are equal
    if (m_read == m_write)
    {
      // if write operation : queue is full
      if (op == idWrite)
      {
        m_wrapType = idWriteWrap;
      }
      // else read operation : queue is empty
      else
      {
        m_wrapType = idReadWrap;
      }
    }
    else
    {
      m_wrapType = idNoWrap;
    }
}

//
// get number of bytes which can be written or read according to operation
//
bool CircularBuffer::getCount (size_t& count, const long op)
        throw ()
{
    // get write and read position pointer
    // take a snap shot of position
    long write = m_write;
    long read  = m_read;

    // get the write count
    count = 0;

    // if wrapped position
    if (write == read)
    {
      // if write wrap type : buffer is full
      if ( m_wrapType == idWriteWrap )
      {
        count = 0;
      }
      // if read wrap type : buffer is empty
      else
      {
        count = m_queueSz;
      }
    }
    // if write position is below read position
    else if ( write > read )
    {
      // sub queue size from next read pos then add
      // to end pos 
      count = read + (m_queueSz - write);
    }
    // if write position is above read position
    else
    {
      // sub cur end from next pos to write
      count = read - write;
    }

    // if need read count then sub read
    // count from queue size
    if (op == idRead)
    {
      // if read wrap type : buffer is empty
      if (m_wrapType == idReadWrap)
      {
        count = 0;
      }
      else
      {
        count = m_queueSz - count;
      }
    }

    // if nothing to write show none
    bool retn_flag;
    if (count == 0)
    {
      retn_flag = false;
    }
    else
    {
      retn_flag = true;
    }

    return retn_flag;
}

//
// Read data from circular buffer
// Requested number of bytes to read is given as input
// The effective number of read bytes is returned
//
bool CircularBuffer::readData (void* pData, size_t& length)
        throw ()
{
    bool success = false;

    try
    {
      // set mutex for buffer updating
      CDMW_MUTEX_GUARD (m_lock);

      // read the length of the read frame 
      size_t count = 0;
      if (getReadCount(count) == false)
      {
        // there is nothing to read 
        length = 0;
        return true;
      }

      // read no more than requested number
      if (length < count)
      {
        count = length;
      }
    
      // read the data
      success = read (pData,count);

      // update returned length with number of bytes read
      if (success == true)
      {
        length = count;
      }
      else
      {
        length = 0;
      }
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
      CDMW_ASSERT(false);
    }

    return success;
}

//
// Read number of bytes from circular buffer
//
bool CircularBuffer::read (void* pData, const size_t length)
        throw ()
{
    char *pData_char = (char *) pData;

    // show data block to long
    if (length > m_queueSz)
    {
      return false;
    }

    // read length to queue end
    size_t lenToEnd = getLengthToEnd(idRead);

    // init data length to read 
    size_t lenData = 0;

    // copy from read to end of buffer
    if (lenToEnd > 0)
    {
      // set data length to read 
      lenData = length;
      if (lenData > lenToEnd)
      {
        lenData = lenToEnd;
      }

      memcpy(pData_char, &m_pQueue[m_read], lenData);
    }

    // copy remainder to start of buffer
    size_t lenRemaining = length - lenData;
    if (lenRemaining > 0)
    {
      memcpy(&pData_char[lenToEnd], &m_pQueue[0], lenRemaining);
    }

    // inc read pos
    incPos (length, idRead);

    return true;
}

//
// Flush data from circular buffer
// Requested number of bytes to flush is given as input
// The effective number of flushed bytes is returned
//
bool CircularBuffer::flushData (size_t& length)
        throw ()
{
    bool success = false;

    try
    {
      // set mutex for buffer updating
      CDMW_MUTEX_GUARD (m_lock);

      // read the length of the read frame 
      size_t count = 0;
      if (getReadCount(count) == false)
      {
        // there is nothing to flush 
        length = 0;
        return true;
      }

      // flush no more than requested number
      if (length < count)
      {
        count = length;
      }
    
      // read the data
      success = flush (count);

      // update returned length with number of bytes read
      if (success == true)
      {
        length = count;
      }
      else
      {
        length = 0;
      }
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
      CDMW_ASSERT(false);
    }

    return success;
}

//
// Flush number of bytes from circular buffer
//
bool CircularBuffer::flush (const size_t length)
        throw ()
{
    // show data block to long
    if (length > m_queueSz)
    {
      return false;
    }

    // inc read pos
    incPos(length, idRead);

    return true;
}


//
// Write data into circular buffer
// Requested number of bytes to write is given as input
// The effective number of written bytes is returned
// If there is no enough space to store message false is returned
//
bool CircularBuffer::writeData (void* pData, size_t& length)
        throw ()
{
    bool success = false;

    try
    {
      // set mutex for buffer updating
      CDMW_MUTEX_GUARD (m_lock);

      // read the length of the write frame 
      size_t count = 0;
      if (getWriteCount(count) == false || count < length)
      {
        // there is no enough space to write 
        length = 0;
        return false;
      }

      // write no more than requested number
      if (length < count)
      {
        count = length;
      }
    
      // write the data
      success = write (pData,count);

      // update returned length with number of bytes written
      if (success == false)
      {
        length = 0;
      }
    }

    // catch any exception from Mutex and assert it
    catch (...)
    {
      CDMW_ASSERT(false);
    }

    return success;
}

bool CircularBuffer::write (void* pData, const size_t length)
        throw ()
{
    char *pData_char = (char *) pData;

    // show data block to long
    if (length > m_queueSz)
    {
      return false;
    }

    // read length to que end
    size_t lenToEnd = getLengthToEnd(idWrite);

    // init data length to copy 
    size_t lenData = 0;

    // copy to end of buffer
    if (lenToEnd > 0)
    {
      // set data length to copy 
      lenData = length;
      if (lenData > lenToEnd)
      {
        lenData = lenToEnd;
      }

      memcpy(&m_pQueue[m_write], pData_char, lenData);
    }

    // copy remainder to start of buffer
    size_t lenRemaining = length - lenData;
    if (lenRemaining > 0)
    {
      memcpy(&m_pQueue[0], &pData_char[lenToEnd], lenRemaining);
    }

    // inc write pos
    incPos (length, idWrite);

    return true;
}

}; // End namespace Trace
}; // End namespace Cdmw

