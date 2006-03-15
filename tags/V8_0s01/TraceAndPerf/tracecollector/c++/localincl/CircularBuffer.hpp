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


#ifndef INCL_CIRCULARBUFFER_HPP 
#define INCL_CIRCULARBUFFER_HPP 

#include <memory>

#include "Foundation/common/Exception.hpp"

#include "Foundation/osthreads/Mutex.hpp"

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
*<p>    This class is used for circular buffer
*       Two pointers for writer and for reader are managed
*       Writer fills data in buffer using the write pointer
*       Reader flush data in buffer using the read pointer
*       Circular buffer is declared full when write pointer reaches the read pointer
*       Circular buffer is declared empty when read pointer reaches the write pointer
*
*Features:
*<p> Thread safe
*
*/

class CircularBuffer 
{
public:

    /**
    * Purpose:
    * <p> Constructor
    * 
    *@param
    *@exception
    */ 
    CircularBuffer ()
            throw ();

    /**
    * Purpose:
    * <p> Destructor
    * 
    *@param
    *@exception
    */ 
    virtual
    ~CircularBuffer ()
            throw ();

    /**
    * Purpose:
    * <p> Create the memory queue for circular buffer
    * 
    *@param queueSz Size of memory queue in Bytes
    *@exception OutOfMemoryException
    */ 
    void create (const size_t queueSz)
            throw (Cdmw::BadParameterException,
                   Cdmw::OutOfMemoryException);

    /**
    * Purpose:
    * <p> Release the memory queue for circular buffer
    * 
    *@param 
    *@exception 
    */ 
    void release ()
            throw ();

    /**
    * Purpose:
    * <p> reset read/write pointer to start of queue
    * 
    *@param 
    *@exception 
    */ 
    void reset ()
            throw ();

    /**
    * Purpose:
    * <p> reset read/write pointer to start of queue
    *     and clear the memory queue
    * 
    *@param 
    *@exception 
    */ 
    void clear ()
            throw ();

    /**
    * Purpose:
    * <p> request to read a number of bytes from buffer
    * 
    *@param pData contains buffer address where bytes from circular buffer are stored
    *@param length contains size of data buffer, after call, contains data size
    *       effectively stored in data buffer
    *       Wrap readCount and read methods
    *@return true if no error
    *@exception 
    */ 
    virtual
    bool readData (void* pData, size_t& length)
            throw ();

    /**
    * Purpose:
    * <p> request to flush a number of bytes from buffer
    *     allow to free data from circular buffer, in this case stored data are lost
    * 
    *@param length contains size of data to flush, after call, contains data size
    *       effectively flushed from circular buffer
    *       Wrap readCount and flush methods
    *@return true if no error
    *@exception 
    */ 
    virtual
    bool flushData (size_t& length)
            throw ();


    /**
    * Purpose:
    * <p> request to write a number of bytes in buffer
    * 
    *@param pData contains buffer address where bytes are to be stored in circular buffer
    *@param length contains size of data buffer
    *       Wrap writeCount and write methods
    *@return true if successfull
    *@exception 
    */ 
    virtual
    bool writeData (void* pData, size_t& length)
            throw ();

    /**
    * Purpose:
    * <p> get the read pointer position in circular buffer
    * 
    *@return pointer position
    *@exception 
    */ 
    unsigned long getReadPos ()
            throw ();

    /**
    * Purpose:
    * <p> get the write pointer position in circular buffer
    * 
    *@return pointer position
    *@exception 
    */ 
    unsigned long getWritePos ()
            throw ();

    /**
    * Purpose:
    * <p> set read pointer
    * 
    *@param set contains the value the pointers has to be set
    *@exception BadParameterException
    */ 
    void setReadPos (const unsigned long set)
            throw (Cdmw::BadParameterException);

    /**
    * Purpose:
    * <p> set write pointer
    * 
    *@param set contains the value the pointers has to be set
    *@exception BadParameterException
    */ 
    void setWritePos (const unsigned long set)
            throw (Cdmw::BadParameterException);


    /**
    * Purpose:
    * <p> get the memory queue size
    * 
    *@return size
    *@exception 
    */ 
    size_t getSize ()
            throw ();
    
protected:


    /**
    * enum for operation type: read or write
    */ 
    enum
    {
        idRead,
        idWrite
    };

    /**
    * enum for wrap configuration
    *   NoWrap : read and write pointers are different
    *   ReadWrap : read and write pointers are equal, buffer is empty
    *   WriteWrap : read and write pointers are equal, buffer is full
    */ 
    enum
    {
        idNoWrap,
        idReadWrap,
        idWriteWrap
    };

    /**
    * Purpose:
    * <p> get number of bytes from read/write pointer to end of memory queue
    * 
    *@param op contains the operation type: read or write
    *@return count return number of bytes
    *@exception 
    */ 
    size_t getLengthToEnd (const long op)
            throw ();

    /**
    * Purpose:
    * <p> increment read/write pointer base on operation
    * 
    *@param inc contains the size the pointers has to be incremented
    *@param op contains the operation type: read or write
    *@exception BadParameterException
    */ 
    void incPos (const size_t inc, const long op)
            throw (Cdmw::BadParameterException);

    /**
    * Purpose:
    * <p> set read/write pointer base on operation
    * 
    *@param set contains the value the pointers has to be set
    *@param op contains the operation type: read or write
    *@exception BadParameterException
    */ 
    void setPos (const unsigned long set, const long op)
            throw (Cdmw::BadParameterException);

    /**
    * Purpose:
    * <p> get the read/write pointer position in circular buffer
    * 
    *@param op contains the operation type: read or write
    *@return pointer position
    *@exception 
    */ 
    unsigned long getPos (const long op)
            throw ();

    /**
    * Purpose:
    * <p> get number of bytes which can be written or read from buffer
    * 
    *@param count return number of bytes in buffer
    *@param op contains the operation type: read or write
    *@return true if bytes can be written or read else false is returned
    *@exception 
    */ 
    bool getCount (size_t& count, const long op)
            throw ();


    /**
    * Purpose:
    * <p> get queue lock
    * 
    *@return mutex
    *@exception 
    */ 
    OsSupport::Mutex& getLock ()
            throw ();




    /**
    * Purpose:
    * <p> get number of bytes which can be read from buffer
    * 
    *@param count return number of bytes to read in buffer
    *@return true if bytes can be read else false is returned
    *@exception 
    */ 
    bool getReadCount (size_t& count)
            throw ();

    /**
    * Purpose:
    * <p> get number of bytes which can be written in buffer
    * 
    *@param count return number of free bytes in buffer
    *@return true if bytes can be written else false is returned
    *@exception 
    */ 
    bool getWriteCount (size_t& count)
            throw ();


    /**
    * Purpose:
    * <p> read bytes from buffer
    * 
    *@param pData contains buffer address where bytes from circular buffer are stored
    *@param length contains number of bytes to read
    *@return true if successfull
    *@exception 
    */ 
    bool read (void* pData, const size_t length)
            throw ();

    /**
    * Purpose:
    * <p> flush bytes from buffer
    * 
    *@param length contains number of bytes to flush
    *@return true if successfull
    *@exception 
    */ 
    bool flush (const size_t length)
            throw ();

    /**
    * Purpose:
    * <p> write bytes in buffer
    * 
    *@param pData contains buffer address where bytes are to be stored in circular buffer
    *@param length contains number of bytes to write
    *@return true if successfull
    *@exception 
    */ 
    bool write (void* pData, const size_t length)
            throw ();


private:

    /**
    * Purpose:
    * <p>  Copy constructor
    * 
    *@param
    *@exception
    */ 
    CircularBuffer (const CircularBuffer& rhs)
            throw();

    /**
    * Purpose:
    * <p>  Assignement operator
    * 
    *@param
    *@exception
    */ 
    CircularBuffer& operator=(const CircularBuffer& rhs)
            throw();





    /**
    * start position to read from memory queue
    */ 
    unsigned long    m_read;

    /**
    * start position to write into memory queue
    */ 
    unsigned long   m_write;

    /**
    * size of memory queue
    */ 
    size_t  m_queueSz;

    /**
    * stores wrap type, did one position get included to another position
    */ 
    long        m_wrapType;

    /**
    * memory queue pointer
    */ 
    char*        m_pQueue;

    /**
    * lock for queue and pointers access
    */
    OsSupport::Mutex m_lock;

}; // End class CircularBuffer 



inline
unsigned long CircularBuffer::getReadPos ()
       throw ()
{ 
    return m_read; 
}

inline
unsigned long CircularBuffer::getWritePos ()
       throw ()
{ 
    return m_write; 
}

inline
unsigned long CircularBuffer::getPos (const long op)
       throw ()
{
    if (op == idRead)
    {
      return m_read;
    }
    else
    {
      return m_write;
    }
}


inline
void CircularBuffer::setReadPos (const unsigned long set)
       throw (Cdmw::BadParameterException)
{
    setPos (set,idRead);
}

inline
void CircularBuffer::setWritePos (const unsigned long set)
       throw (Cdmw::BadParameterException)
{
    setPos (set,idWrite);
}

inline
bool CircularBuffer::getReadCount (size_t& count)
       throw ()
{
    return getCount (count,idRead);
}

inline
bool CircularBuffer::getWriteCount (size_t& count)
       throw ()
{
    return getCount (count,idWrite);
}



inline
size_t CircularBuffer::getLengthToEnd (const long op)
       throw ()
{
    if (op == idRead)
    {
      return (m_queueSz - m_read);
    }
    else
    {
      return (m_queueSz - m_write);
    }
}


inline
size_t CircularBuffer::getSize ()
       throw ()
{
    return m_queueSz;
}
    
inline
OsSupport::Mutex& CircularBuffer::getLock ()
            throw ()
{
    return m_lock;
}
    


}; // End namespace Trace
}; // End namespace Cdmw
#endif // INCL_CIRCULARBUFFER_HPP

