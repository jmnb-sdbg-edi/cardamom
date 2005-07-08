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


#include "tracelibrary/FlushArea.hpp"

#include <algorithm>
#include <sstream>
#include <new>
#include <iostream>


#include "Foundation/common/Assert.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "TraceAndPerf/tracelibrary/FlushAreaMngr.hpp"
#include "tracelibrary/TraceTools.hpp"

namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes: none
*<p>
*Portability issues: none
*<p>
*/



//
// Ctor
//    
FlushArea::FlushArea (size_t nbMessages, size_t sizeArea, size_t sizeOutFlushAreaMsg, unsigned int id)
    throw (OutOfMemoryException)
    : m_messages(nbMessages+1),
      m_id(id),
      m_indexFreeMessage(0),
      m_userSizeArea(sizeArea),
      m_indexFreeBlock(0)
{
	// a supplementary message is allocated in the message container for out flush area error message
	// a supplementary area zone is allocated in raw buffer for error message

	m_totalSizeArea = m_userSizeArea + sizeOutFlushAreaMsg + 100;
    m_pArea = new(std::nothrow) char[m_totalSizeArea];

    if (m_pArea == NULL) 
	{
        CDMW_THROW (OutOfMemoryException);
    }
}




//
// Dtor
//
FlushArea::~FlushArea()
    throw()
{
    delete[] m_pArea;
}



//
// add the specified message
//
FlushArea::AreaStatus FlushArea::add_message (const Message::MessageHeader& header, 
				                              const std::string& body)
    throw()
{

    AreaStatus ret = OK;

    // the actual size of the message body that includes the terminating null character
    size_t actualMessageSize = body.length() + 1;

	// add size to allocate for message header
	actualMessageSize += Message::getMessageHeaderStringSize (header);
    
	// if message is greater than area buffer
    if (actualMessageSize > m_userSizeArea)
    {
        ret = MESSAGE_TOO_BIG;
	}
    else
    {
	    // Is there enough room from available area for user?
	    if ((m_indexFreeBlock + actualMessageSize) <= m_userSizeArea)
		{
		    // We get back the next message from the pool
            Message& message = m_messages[m_indexFreeMessage];

		    // We assign enough room the the internal message
		    //	buffer
		    message.set_buffer (m_pArea + m_indexFreeBlock, actualMessageSize);

            try
            {
                // store message in area buffer
                message.set_message (header, body);

				// increment message index and area buffer index
                m_indexFreeMessage++;
                m_indexFreeBlock += actualMessageSize;

				// if area is full (number of messages reached or area buffer full
                if (m_indexFreeMessage == (m_messages.size()-1) ||
                    m_indexFreeBlock == m_userSizeArea )
                {
                    ret = AREA_FULL;
                }
            }
            catch(const OutOfMemoryException &)
            {
                ret = OUT_OF_MEMORY;
            }
	    }

		// not enough room in area buffer
        else
        {
            ret = SIZE_TOO_SMALL;
	    }
    }	    

    return ret;
}

//
// add the specified out flush area error message
//
void FlushArea::store_outFlushAreaMsg (const std::string& errmsg)
    throw()
{
	// create header for error message
    Message::MessageHeader header;

	header.m_fileLine = 0;
	header.m_fileName = "";
	header.m_userLevel = CdmwTrace::ALL_VALUES;
	header.m_userDomain = CdmwTrace::ALL_DOMAINS;
	header.m_tid = OsSupport::Thread::self();
	header.m_timestamping = OsSupport::OS::get_time();

    // the actual size of the message body that includes the terminating null character
    size_t actualMessageSize = errmsg.length() + 1;

	// add size to allocate for message header
	actualMessageSize += Message::getMessageHeaderStringSize (header);
    
    if (actualMessageSize <= m_totalSizeArea &&
	    m_indexFreeBlock + actualMessageSize <= m_totalSizeArea)
    {
	  // We get back the nex message from the pool
      Message& message = m_messages[m_indexFreeMessage];

	  // We assign enough room the the internal message
	  //	buffer
      message.set_buffer (m_pArea + m_indexFreeBlock, actualMessageSize);

	  // store header and error message
      message.set_message (header, errmsg);

      m_indexFreeMessage++;
      m_indexFreeBlock += actualMessageSize;
    }	    
}

//
//
bool FlushArea::is_empty()
    throw()
{
    return (m_indexFreeMessage == 0);
}


//
// Reset the internal state and delete all messages
//
void FlushArea::reset()
    throw()
{
    CDMW_TRACE_DEBUG(id() << " : FlushArea::reset()");

    m_indexFreeBlock = 0;
    m_indexFreeMessage = 0;
}    


//
// Create the CORBA MessageHeader from the c++ MessageHeader
//
CdmwTrace::MessageHeader* FlushArea::create_MessageHeader (const Trace::Message& message)
    throw (OutOfMemoryException)
{
    using namespace CdmwTrace;

    try 
	{
        // NB: you may ask why this function is not member of MessageHeader,
        // the reason is I don't want to impose in the user namespace Corba symbol
        // or definition, if he doesn't want them (may to port non-Corba code) while
        // keeping the capability to use the trace service.

        const Trace::Message::MessageHeaderRaw& header = message.get_header();

        MessageHeader_var messageHeader= new MessageHeader();

        // Information about the trace origine
        messageHeader->the_file_line = header.m_fileLine;
        char* fileName = CORBA::string_dup (header.m_pFileName);
        check_corba_str (fileName);
        messageHeader->the_file_name = fileName;

        // Information about the user defined characteristic
        messageHeader->the_level.the_value  = header.m_userLevel;
        char* domain = CORBA::string_dup (header.m_pUserDomain);
        check_corba_str (domain);
        messageHeader->the_level.the_domain = domain;

        // Which thread has produced the message
        messageHeader->the_thread_id = header.m_tid;

        // And finally, when the message has been produced (stamping)
        messageHeader->time_stamping.seconds = header.m_timestamping.seconds;
        messageHeader->time_stamping.microseconds = header.m_timestamping.microseconds;

		// return pointer and pass its ownership to caller
        return messageHeader._retn();

    } 
	catch (const std::bad_alloc&)
	{
        CDMW_THROW (OutOfMemoryException);
    }
}



//
// Create the CORBA FormattedMessage from the c++ Message
//
CdmwTrace::FormattedMessage* FlushArea::create_FormattedMessage (const Trace::Message& message)
    throw (OutOfMemoryException)

{
    using namespace CdmwTrace;

    try
	{
        FormattedMessage_var formattedMessage = new FormattedMessage();

        MessageHeader_var messageHeader= create_MessageHeader (message);
        formattedMessage->header = messageHeader.in();
        
        char* buffer = CORBA::string_dup (message.get_bodyBuffer());
        check_corba_str (buffer);
        formattedMessage->message = buffer;

		// return pointer and pass its ownership to caller
        return formattedMessage._retn();
    } 
	catch (const std::bad_alloc&)
	{
        CDMW_THROW (OutOfMemoryException);
    }
}



//
// Create the CORBA sequence of message from the internal message container
//
CdmwTrace::FormattedMessageSeq* FlushArea::create_FormattedMessageSeq()
    throw (OutOfMemoryException)
{
    using namespace CdmwTrace;
    
    try 
	{
        FormattedMessageSeq_var formattedMessageSeq = new FormattedMessageSeq();

        // We create a sequence enough large to hold all contained messages
        formattedMessageSeq->length(m_indexFreeMessage);

        size_t indexMessage(0);

        // We get back each message contained in the flush area
        for (MessageCtr::iterator itrMessage= m_messages.begin(); 
						indexMessage != m_indexFreeMessage; indexMessage++)
        {        
            CDMW_ASSERT (itrMessage != m_messages.end());
            FormattedMessage_var formattedMessage = create_FormattedMessage (*itrMessage);
            formattedMessageSeq[indexMessage] = formattedMessage.in();
            itrMessage++;
        }

		// return pointer and pass its ownership to caller
        return formattedMessageSeq._retn();
        
    } 
	catch (const std::bad_alloc&) 
	{
        CDMW_THROW (OutOfMemoryException);
    }
}




//
// Create from the host information the CORBA header for a set of messages
//
CdmwTrace::MessagesHeader* FlushArea::create_MessagesHeader()
    throw (OutOfMemoryException)
{
    using namespace CdmwTrace;
    
    try 
	{
        MessagesHeader_var messagesHeader= new MessagesHeader();

        // TODO
        messagesHeader->the_host_id = 0;
        char* the_host_name = CORBA::string_dup (OsSupport::OS::get_hostname().c_str());
        check_corba_str (the_host_name);
        messagesHeader->the_host_name = the_host_name;

        messagesHeader->the_process_id = FlushAreaMngr::instance().getProcessId();

        char* the_application_name = CORBA::string_dup (FlushAreaMngr::instance().getApplicationName().c_str());
        check_corba_str (the_application_name);
        messagesHeader->the_application_name = the_application_name;
        
        char* the_process_name = CORBA::string_dup (FlushAreaMngr::instance().getProcessName().c_str());
        check_corba_str (the_process_name);
        messagesHeader->the_process_name = the_process_name;

		// return pointer and pass its ownership to caller
        return messagesHeader._retn();
        
    } 
	catch (const std::bad_alloc&)
	{
        CDMW_THROW (OutOfMemoryException);
    }
}

} // End namespace Trace
} // End namespace Cdmw

