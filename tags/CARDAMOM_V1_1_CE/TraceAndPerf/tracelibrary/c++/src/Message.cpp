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


#include "TraceAndPerf/tracelibrary/Message.hpp"

// To get memcpy
#include <cstring>
#include <cstdlib>

#include "Foundation/common/Assert.hpp"
#include "tracelibrary/FlushArea.hpp"


namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes:  none 
*<p>
*Portability issues:  none 
*<p>
*/


//
// Ctor
//    
Message::Message()
    throw()
    : m_pBuffer (NULL),
      m_szTotalBuffer(0),
      m_pBodyBuffer (NULL),
      m_szBodyBuffer(0)
{
    m_messageHeaderRaw.m_tid = 0;
    m_messageHeaderRaw.m_fileLine = 0;
    m_messageHeaderRaw.m_pFileName = NULL;
    m_messageHeaderRaw.m_szFileName = 0;
    m_messageHeaderRaw.m_pUserDomain = NULL;
    m_messageHeaderRaw.m_szUserDomain = 0;
    m_messageHeaderRaw.m_userLevel = 0;
    m_messageHeaderRaw.m_timestamping.seconds = 0;
    m_messageHeaderRaw.m_timestamping.microseconds = 0;
}



//
// Copy Ctor
//  Remember the memory is not allocated by new, so
//  the memory is not copied or reallocated, we use
//  the same zone
//
Message::Message (const Message& rhs)
    throw()
    : m_pBuffer (rhs.m_pBuffer),
      m_szTotalBuffer(rhs.m_szTotalBuffer),
      m_pBodyBuffer (rhs.m_pBodyBuffer),
      m_szBodyBuffer(rhs.m_szBodyBuffer)
{
    m_messageHeaderRaw = rhs.m_messageHeaderRaw;
}




//
// Assignement operator
//
Message&
Message::operator=(const Message& rhs)
    throw()
{
    m_pBuffer = rhs.m_pBuffer;
    m_szTotalBuffer = rhs.m_szTotalBuffer;
    m_pBodyBuffer = rhs.m_pBodyBuffer;
    m_szBodyBuffer = rhs.m_szBodyBuffer;
    m_messageHeaderRaw = rhs.m_messageHeaderRaw;

    return *this;
}



//
// Dtor
//
Message::~Message()
    throw()
{
    // Nothing to do
}

//
// Get size of message header to allocate in buffer area
//
size_t Message::getMessageHeaderStringSize (const MessageHeader& header)
{
  size_t hdrStrSize = header.m_fileName.length() + 1;
  hdrStrSize += header.m_userDomain.length() + 1;
  return hdrStrSize;
}


//
// Store the message with the header in internal data
//
void Message::set_message (const MessageHeader& header, const std::string& str)
    throw (AssertionFailedException)
{
	// init current pointer on buffer area and current allocated size
    char*  pCurrentBuffer = m_pBuffer;
	size_t szCurrentBuffer = m_szTotalBuffer;
	size_t szParameter;

	// store thread id and file line
    m_messageHeaderRaw.m_tid = header.m_tid;
    m_messageHeaderRaw.m_fileLine = header.m_fileLine;

    // check that the size of the buffer is enough for file name
	// store file name in buffer area, its pointer and its size in header
	szParameter = header.m_fileName.length() + 1;
    CDMW_ASSERT (szParameter < szCurrentBuffer);
    m_messageHeaderRaw.m_pFileName = pCurrentBuffer;
    ::strcpy (m_messageHeaderRaw.m_pFileName, header.m_fileName.c_str());
    m_messageHeaderRaw.m_szFileName = szParameter;
	
	// increment current pointer on buffer area and decrement current allocated size
	pCurrentBuffer += szParameter;
	szCurrentBuffer -= szParameter;
	

    // check that the size of the buffer is enough for user domain
	// store user domain in buffer area, its pointer and its size in header
	szParameter = header.m_userDomain.length() + 1;
    CDMW_ASSERT (szParameter < szCurrentBuffer);
    m_messageHeaderRaw.m_pUserDomain = pCurrentBuffer;
    ::strcpy (m_messageHeaderRaw.m_pUserDomain, header.m_userDomain.c_str());
    m_messageHeaderRaw.m_szUserDomain = szParameter;

	// increment current pointer on buffer area and decrement current allocated size
	pCurrentBuffer += szParameter;
	szCurrentBuffer -= szParameter;
	

	// store user level, time stamping
    m_messageHeaderRaw.m_userLevel = header.m_userLevel;
    m_messageHeaderRaw.m_timestamping = header.m_timestamping;
	
    // check that the size of the buffer is enough for body message
	// store body message in buffer area, its pointer and its size in message
	szParameter = str.size();
    CDMW_ASSERT (szParameter < szCurrentBuffer);
    m_pBodyBuffer = pCurrentBuffer;
    ::strcpy (m_pBodyBuffer, str.c_str());
    m_szBodyBuffer = szParameter;
}




//
// Returns the internal buffer coming from a FlushArea
//
const char* Message::get_buffer() const
	throw ()
{
    return 	m_pBuffer;	
}	


//
// Returns the body message buffer coming from a FlushArea
//
const char* Message::get_bodyBuffer() const
	throw ()
{
    return 	m_pBodyBuffer;	
}	




//
// Returns the internal header defining the origin of the message
//
const Message::MessageHeaderRaw& Message::get_header() const
	throw ()
{
    return 	m_messageHeaderRaw;	
}	




//
// A FlushArea uses this method to assign raw memory to this message
//
void Message::set_buffer (char* pBuffer, size_t szBuffer)
	throw ()
{
    m_pBuffer = pBuffer;	
    m_pBuffer[0] = '\0';
    m_szTotalBuffer = szBuffer;
}	



} // End namespace Trace
} // End namespace Cdmw

