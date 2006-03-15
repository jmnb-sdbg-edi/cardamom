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


#ifndef INCL_TRACE_MESSAGE_HPP 
#define INCL_TRACE_MESSAGE_HPP 


#include <cstdlib>
#include <string>

#include "Foundation/common/Exception.hpp"

// To get ThreadId
#include "Foundation/osthreads/ThreadHandle.hpp"

// To get Timeval
#include "Foundation/ossupport/OS.hpp"

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
*<p>    This class holds one message created by developer
*    using the stream object. This class holds the rax data
*    in a formatted way, and a structure describing the origin
*    of the message (file, line, thread id, etc.)
*
*Features:
*<p> None
*
*@see  FlushArea
*
*/
class Message 
{

    public:
    
        /**
		* Contains the different format for tracing message
		*/
		enum TraceFormat 
	    {
		  /**
		  * Vertical format
		  */
		  V_FORMAT,

		  /**
		  * Horizontal format
		  */
		  H_FORMAT
		};
		
        /**
		* Contains the strategy of the trace circular buffer mode when it is full
		*/
		enum CircularMode 
	    {
		  /**
		  * The latest received message is dropped
		  */
		  DROP_LATEST,

		  /**
		  * The oldest message in buffer is removed
		  */
		  REMOVE_OLDEST
		};
		
		
        /**
	    * This structure fully describes the origin of a message, and 
	    * it is use to prefix each message.
	    */
        struct MessageHeader
	    {
	        /**
		    * The thread which has created the message.
		    */
	        OsSupport::Thread::ThreadId m_tid;
		
		    /**
		    * The line where the message has been produced.
		    */
		    int m_fileLine;
		    
		    /**
		    * The file where the message has been produced.
		    */
		    std::string m_fileName;

            /**
             * The component name.
             */
            // ECR-0123
            std::string m_componentName;

		    /**
		    * The user specified domain origin of the message
		    */
		    std::string m_userDomain;
		    
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
	    * This structure is used for Message header in raw format 
	    */
        struct MessageHeaderRaw
	    {
	        /**
		    * The thread which has created the message.
		    */
	        OsSupport::Thread::ThreadId m_tid;
		
		    /**
		    * The line where the message has been produced.
		    */
		    int m_fileLine;
		    
		    /**
		    * The file name where the message has been produced.
		    */
		    char *m_pFileName;

		    /**
		    * The name size of file, origin of trace
		    */
	        size_t m_szFileName;

            /**
             * The component name.
             */
            // ECR-0123
            char* m_pComponentName;

            /**
             * The size of the component name string.
             */
            // ECR-0123
            size_t m_szComponentName;

		    /**
		    * The user specified domain origin of the message
		    */
		    char *m_pUserDomain;

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
        * <p> Construct an empty message without memory
	    *    allocated.
        * 
        */ 
        Message( )
                throw();

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        *@param rhs
        *
        */ 
        Message(const Message& rhs)
                throw();

        /**
        * Purpose:
        * <p> No Assignement operator
        * 
        *@param rhs
        *
        */ 
        Message&
        operator=(const Message& rhs)
                throw();



        /**
        * Purpose:
        * <p> Destructor 
        * 
        */ 
        virtual 
        ~Message()
                throw();
		
		


        /**
        * Purpose:
        * <p> get size of message header to allocate in buffer area
        * 
        *@param header header describing the origin of the message
        *
        */ 
		static
		size_t getMessageHeaderStringSize (const MessageHeader& header);


        /**
         * Compare 2 message headers.
         * Note: the timestamp is ignored.
         *
         * @param hdr1 the first message header
         * @param hdr2 the second message header
         */
        static bool
        headercmp(const MessageHeader & hdr1, const MessageHeader & hdr2);


        /**
        * Purpose:
        * <p> Copy the specified message into the internal buffer
        * 
        *@param header header describing the origin of the message
        *@param str message to be stored internally
        *
        *@exception AssertionFailedException
        *
        */ 
	    void set_message (const MessageHeader& header, const std::string& str)
		    throw (AssertionFailedException);

        /**
        * Purpose:
        * <p> Returns the internal buffer containing the raw message
        * 
        *@return the buffer containing the raw message
        */ 
	    const char* get_buffer() const
	        throw ();

        /**
        * Purpose:
        * <p> Returns the internal buffer containing the body message
        * 
        *@return the buffer containing the message
        */ 
	    const char* get_bodyBuffer() const
	        throw ();

        /**
        * Purpose:
        * <p> Returns the header of the contained message
        * 
        *@return the header of the message
        *
        */ 
        const MessageHeaderRaw& get_header() const
        	throw ();



    protected:
    
        friend class FlushArea;

        /**
        * Purpose:
        * <p> Set a new internal buffer. as the previous one,
	    * must be allocated by FlushArea, no memory free is done.
        * 
        *@return the buffer containing the message
        */ 
	    void set_buffer (char* pBuffer, size_t szBuffer)
	        throw();

        /**
	    * Buffer holding the message as raw data. The memory contained in this
	    * buffer has been allocating by a FlushArea, and so must not be freed,
	    * by standard C++ way.
	    */
        char* m_pBuffer;
	
	    /**
	    * Total size of the buffer allocated and contained in m_pBuffer.
	    */
	    size_t m_szTotalBuffer;
	
        /**
	    * Buffer holding the body message.
	    */
        char* m_pBodyBuffer;
	
	    /**
	    * Size of the buffer allocated for body message
	    */
	    size_t m_szBodyBuffer;
	
	    
	    /**
	    * The header containing information about the origin of the message in raw format
	    */
	    MessageHeaderRaw m_messageHeaderRaw;


}; // End class Message 

} // End namespace Trace
} // End namespace Cdmw
#endif // INCL_TRACE_MESSAGE_HPP

