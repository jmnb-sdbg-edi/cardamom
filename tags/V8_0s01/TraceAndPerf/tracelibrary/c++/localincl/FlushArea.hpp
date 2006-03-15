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


#ifndef INCL_TRACE_FLUSHAREA_HPP 
#define INCL_TRACE_FLUSHAREA_HPP 

#include <vector>
#include <string>

#include "TraceAndPerf/tracelibrary/Message.hpp"

#include "Foundation/orbsupport/CORBA.hpp"
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

/**
*Purpose:
*<p>    This class store a raw buffer whose size has been
*       specified during initialisation. The FlushArea
*       uses this memory to alloc chunk of memory and return
*       them wrapped in a Message object. When full this area
*       return NULL when an allocation request is done. The 
*       FlushArea is able to transform all internally stored
*       message in CORBA structure to be sent to Collectors.
*
*Features:
*<p> No Thread safe
*
*@see  FlushAreaManager 
*
*/
class FlushArea 
{
    protected:

        /**
        * Container used to stored the created messages.
        */
        typedef std::vector<Message> MessageCtr;
        
    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param nbMessages number of user messages 
        *@param sizeArea size in octet of the area to be created. The usable
        *  size is smaller as the area contains also the terminating NULL
        *  character for each message.
        *@param sizeOutFlushAreaMsg size in octet of the message for out flush area error
		*       including the terminating NULL 
        *@param id the id of the created FlushArea used for debug purpose
        *
        *@exception OutOfMemoryException
        */ 
        FlushArea (size_t nbMessages, size_t sizeArea, size_t sizeOutFlushAreaMsg, unsigned int id = 0)
                throw (OutOfMemoryException);


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~FlushArea()
                throw();


        /**
        * Purpose:
        * <p> Returns the id of the flush area
        */ 
	    unsigned int id() { return m_id; }


        /**
        * The status of the flush area returned by the add_message method :
        * - OK : The message has been added successfully and the area
        * can receive other messages.
        * - AREA_FULL : The message has been added successfully but
        * the number of messages or the size of the area has been reached.
        * - SIZE_TOO_SMALL : The message cannot be added to the area as
        * the size of the message exceeds the available size of the area.
        * - OUT_OF_MEMORY : The message cannot be added to the area as
        * there is not enough memory for the copy of the header.
        * - MESSAGE_TOO_BIG : The message can never be added to the area
        * as the size of the message exceeds the total size of the area.
        */ 
        enum AreaStatus 
		{
            OK,
            AREA_FULL,
            SIZE_TOO_SMALL,
            OUT_OF_MEMORY,
            MESSAGE_TOO_BIG
        } ;


        /**
        * Purpose:
        * <p> Adds the specified message to the flush area
        * 
        *@param header the header of the message
        *@param body   the body of the message
        *
        *@return the status of the area after the call
        */ 
        virtual AreaStatus add_message (const Message::MessageHeader& header, const std::string& body)
	        throw();

        /**
        * Purpose:
        * <p> store the specified out flush area error message
        * 
        *@param errmsg the body of the error message
        *
        */ 
        virtual
        void store_outFlushAreaMsg (const std::string& errmsg)
	        throw();



        /**
        * Purpose:
        * <p> Returns true if the area is empty.
        */ 
        virtual
        bool is_empty() throw();


        /**
        * Purpose:
        * <p> Reset the internal set of the FlushArea, each message
        * stored are lost. This methods is intended to be used when 
        * all contained message have been stored elsewhere
        *
        */ 
        virtual
        void reset()
	        throw() ;


        

        /**
        * Purpose:
        * <p> Build from the internally stored messages, the
        * equivalent CORBA structure.
        * 
        *@exception OutOfMemoryException
        *
        */ 
        virtual
        CdmwTrace::FormattedMessageSeq* create_FormattedMessageSeq()
            throw (OutOfMemoryException) ;



        /**
        * Purpose:
        * <p> Build from the internally stores messages, the
        * equivalent CORBA Header structure .
        * 
        *@exception OutOfMemoryException
        *
        */
        virtual
        CdmwTrace::MessagesHeader* create_MessagesHeader()
            throw (OutOfMemoryException);

       
    protected:
    
    
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        FlushArea (const FlushArea& rhs)
                throw();

    
        /**
        * Purpose:
        * <p> Assignement operator 
        * 
        */ 
        FlushArea&
        operator=(const FlushArea& rhs)
                throw();


        
        /**
        * Purpose:
        * <p> Create the header of one specific message. This
        * function is use internally to build the FormattedMessagesSeq
        * 
        *@param message the message whose C++ header representation must be 
        *   transform into CORBA representation
        *
        *@return the CORBA header
        *
        *@exception OutOfMemoryException
        *
        */ 
        CdmwTrace::MessageHeader* create_MessageHeader (const Trace::Message& message)
            throw (OutOfMemoryException);



        
        /**
        * Purpose:
        * <p> Create the CORBA message associated to an internal C++
        * message.This function is use internally to build 
        * the FormattedMessagesSeq
        * 
        *@param message the message whose C++ representation must be 
        *   transform into CORBA representation
        *
        *@return the CORBA FormattedMessage
        *
        *@exception OutOfMemoryException
        *
        */ 
        CdmwTrace::FormattedMessage* create_FormattedMessage (const Trace::Message& message)
                throw (OutOfMemoryException);


        /**
        * Container used to store all messages created by the user
        */
        MessageCtr m_messages; 


	    /**
	    * Contains the id of the flush area.
	    */
	    unsigned int m_id;

	
	    /**
	    * Contains the index of the nex available message in
	    * m_messages.
	    */
	    unsigned int m_indexFreeMessage;


        /**
        * The size of the useful internal raw buffer.
        */
	    size_t m_userSizeArea;

        /**
        * The total size of the internal raw buffer with flush area error
        */
	    size_t m_totalSizeArea;


	    /**
	    * Contains the index of the next available chunk of
	    * memory.
	    */
	    unsigned int m_indexFreeBlock;


        /**
        * The pointer to the raw buffer.
        */
	    char* m_pArea;


}; // End class FlushArea 

} // End namespace Trace
} // End namespace Cdmw
#endif // INCL_TRACE_FLUSHAREA_HPP

