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


#ifndef INCL_PLATFORMMNGT_MESSAGE_QUEUE_HPP 
#define INCL_PLATFORMMNGT_MESSAGE_QUEUE_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/Condition.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include <list>
#include <sstream>


namespace Cdmw
{
namespace PlatformMngt
{


/**
* Purpose:
* Thrown when the MessageQueue is destroyed.
*/
class MessageQueueDestroyedException  : public Exception {
    
    public:
        MessageQueueDestroyedException()
            : Exception(Exception::SAFE, "MessageQueue destroyed") { }

};


/**
*Purpose:
* The default deallocator of messages managed by MessageQueue.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
template <class MessageType> class MessageDeallocator
{
    public:

        /**
        * Purpose:
        * <p>
        * Deallocates the message.
        */ 
        static void deallocate(MessageType* message)
            throw ()
        {
            delete message;
        }

};


/**
*Purpose:
* Thread safe message queue.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
template <class MessageType, class MessageDealloc = MessageDeallocator<MessageType> > class MessageQueue
{

    public:


        /**
        * Purpose:
        * <p> Constructor.
        */ 
        MessageQueue()
            : m_destroyed(false), m_message_condition(m_queue_mutex)
        {
        
        }


        /**
        * Purpose:
        * <p> Destructor.
        * Deletes the remaining messages if any.
        */ 
        virtual ~MessageQueue() throw()
        {

            while (!m_queue.empty())
            {
                MessageType* mes = m_queue.front(); 
                m_queue.pop_front(); 
                MessageDealloc::deallocate(mes);
            }

        };


        /**
        * Purpose:
        * <p>
        * Indicates whether the queue is empty or not in a thread safe manner.
        *
        * @exception MessageQueueDestroyedException if the queue is destroyed.
        */ 
        bool isEmpty() throw(MessageQueueDestroyedException)
        {
            bool ret;

            CDMW_MUTEX_GUARD(m_queue_mutex);
            
            if (m_destroyed)
                CDMW_THROW(MessageQueueDestroyedException);

            ret = m_queue.empty(); 

            return ret; 
        }


        /**
        * Purpose:
        * <p>
        * Returns the size of the queue in a thread safe manner.
        *
        * @exception MessageQueueDestroyedException if the queue is destroyed.
        */ 
        size_t size() throw(MessageQueueDestroyedException)
        {
            size_t size;

            CDMW_MUTEX_GUARD(m_queue_mutex);
            
            if (m_destroyed)
                CDMW_THROW(MessageQueueDestroyedException);

            size = m_queue.size(); 

            return size;
        }


        /**
        * Purpose:
        * <p>
        * Pushes the message in the queue in a thread safe manner.
        * If successful, this method takes ownership of the message.
        *
        * @exception BadParameterException if message is NULL.
        * @exception MessageQueueDestroyedException if the queue is destroyed.
        * @exception OutOfMemoryException.
        */ 
        void push(MessageType* message)
            throw (BadParameterException, MessageQueueDestroyedException,
            OutOfMemoryException)
        {

            try
            {
                CDMW_MUTEX_GUARD(m_queue_mutex);
            
                if (m_destroyed)
                    CDMW_THROW(MessageQueueDestroyedException);

                if (message == NULL)
                    CDMW_THROW2(BadParameterException, "message", "NULL");

                m_queue.push_back(message);

                m_message_condition.broadcast();

            }
            catch(const std::bad_alloc &)
            {
                CDMW_THROW(OutOfMemoryException);
            }

        }


        /**
        * Purpose:
        * <p>
        * Pushes the messages in the queue in a thread safe manner.
        * If successful, this method takes ownership of each message.
        *
        * @exception BadParameterException if one of the messages is NULL.
        * @exception MessageQueueDestroyedException if the queue is destroyed.
        * @exception OutOfMemoryException.
        */ 
        void push(std::list<MessageType*>& messages)
            throw(BadParameterException, MessageQueueDestroyedException,
            OutOfMemoryException)
        {

            try
            {
                CDMW_MUTEX_GUARD(m_queue_mutex);

                if (m_destroyed)
                    CDMW_THROW(MessageQueueDestroyedException);

                std::list<MessageType*> temp_queue(m_queue);

                size_t index = 0;
                typename std::list<MessageType*>::const_iterator it;

                for (it = messages.begin(); it != messages.end(); ++it)
                {
                    if (*it == NULL)
                    {
                        std::ostringstream temp;
                        temp << "messages[" << index << "]" << std::ends;

                        std::string str = temp.str();

                        CDMW_THROW2(BadParameterException, str, "NULL");
                    }

                    temp_queue.push_back(*it);
                    ++index;
                }

                // updates m_queue
                m_queue.swap(temp_queue);

                m_message_condition.broadcast();

            }
            catch(const std::bad_alloc &)
            {
                CDMW_THROW(OutOfMemoryException);
            }
 
        }


        /**
        * Purpose:
        * <p>
        * Pops the message from the front in a thread safe manner.
        * If there is no message in the queue, the thread making the call is
        * blocked.
        * The ownership of the message is given to the caller.
        *
        * @exception MessageQueueDestroyedException if the queue is destroyed.
        */ 
        MessageType* pop() throw(MessageQueueDestroyedException)
        {

            CDMW_MUTEX_GUARD(m_queue_mutex);

            if (m_destroyed)
                CDMW_THROW(MessageQueueDestroyedException);

            while (m_queue.empty())
            {
                m_message_condition.wait();

                if (m_destroyed)
                    CDMW_THROW(MessageQueueDestroyedException);
            }

            MessageType* mes = m_queue.front(); 
            m_queue.pop_front(); 

            return mes;

        }


        /**
        * Purpose:
        * <p>
        * Destroys the message queue.
        */ 
        void destroy() throw()
        {

            CDMW_MUTEX_GUARD(m_queue_mutex);

            m_destroyed = true;

            m_message_condition.broadcast();

        }


    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        MessageQueue(const MessageQueue& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        MessageQueue& operator=(const MessageQueue& rhs);


        /**
        * The actual message queue.
        */
        std::list<MessageType*> m_queue;

        /**
        * Indicates whether the message queue is about to be destroyed.
        */
        bool m_destroyed;
        
        /**
        * The mutex protecting concurrent access to m_queue and m_destroyed.
        */
        OsSupport::Mutex m_queue_mutex;

        /**
        * The condition associated with m_queue_mutex.
        */
        OsSupport::Condition m_message_condition;


}; // End class MessageQueue 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_MESSAGE_QUEUE_HPP

