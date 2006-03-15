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


#ifndef INCL_PLATFORMMNGT_MESSAGEQUEUEWRAPPER_HPP 
#define INCL_PLATFORMMNGT_MESSAGEQUEUEWRAPPER_HPP 

#include "SystemMngt/platformlibrary/MessageQueue.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

/**
*Purpose:
* Wrapper around MessageQueue for threading tests. 
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class MessageQueueWrapper
{

    public:

        /**
        */ 
        MessageQueueWrapper(Cdmw::PlatformMngt::MessageQueue<std::string>& queue)
            : m_queue(queue), m_nbThreads(0), m_maxNbThreads(0)
        {
        }


        /**
        */ 
        virtual ~MessageQueueWrapper() throw() {};


        /**
        */ 
        unsigned short maxNbThreads()
        {
            CDMW_MUTEX_GUARD(m_nbThreads_mutex);
            return m_maxNbThreads;
        }


        /**
        */ 
        bool isEmpty()
        {
            try
            {
                enterMethod("MessageQueue::isEmpty");
                bool ret = m_queue.isEmpty();
                exitMethod("MessageQueue::isEmpty");
                return ret;
            }
            catch(...)
            {
                exitMethod("MessageQueue::isEmpty");
                throw;
            }
        }


        /**
        */ 
        size_t size()
        {
            try
            {
                enterMethod("MessageQueue::size");
                size_t size = m_queue.size();
                exitMethod("MessageQueue::size");
                return size;
            }
            catch(...)
            {
                exitMethod("MessageQueue::size");
                throw;
            }
        }


        /**
        * Purpose:
        */ 
        void push(std::string* message)
        {
            try
            {
                enterMethod("MessageQueue::push");
                m_queue.push(message);
                exitMethod("MessageQueue::push");
            }
            catch(...)
            {
                exitMethod("MessageQueue::push");
                throw;
            }
        }


        /**
        */ 
        void push(std::list<std::string*>& messages)
        {
            try
            {
                enterMethod("MessageQueue::push");
                m_queue.push(messages);
                exitMethod("MessageQueue::push");
            }
            catch(...)
            {
                exitMethod("MessageQueue::push");
                throw;
            }
        }


        /**
        */ 
        std::string* pop()
        {
            try
            {
                enterMethod("MessageQueue::pop");
                std::string* mes = m_queue.pop();
                exitMethod("MessageQueue::pop");

                return mes;
            }
            catch(...)
            {
                exitMethod("MessageQueue::pop");
                throw;
            }

        }


        /**
        */ 
        void destroy()
        {
            try
            {
                enterMethod("MessageQueue::destroy");
                m_queue.destroy();
                exitMethod("MessageQueue::destroy");
            }
            catch(...)
            {
                exitMethod("MessageQueue::destroy");
                throw;
            }
        }


    private:

        /**
        * The actual message queue.
        */
        Cdmw::PlatformMngt::MessageQueue<std::string>& m_queue;

        /**
        * The number of threads accessing the naming context
        */
        unsigned short m_nbThreads;

        /**
        * The maximum number of threads detected
        */
        unsigned short m_maxNbThreads;

        /**
        * The mutex protecting concurrent access to m_nbThreads and m_maxNbThreads.
        */
        Cdmw::OsSupport::Mutex m_nbThreads_mutex;


        void enterMethod(const std::string& method)
        {

            CDMW_MUTEX_GUARD(m_nbThreads_mutex);
            m_nbThreads++;

            if (m_nbThreads > m_maxNbThreads)
            {
                m_maxNbThreads = m_nbThreads;
            }

        }

        void exitMethod(const std::string& method)
        {

            CDMW_MUTEX_GUARD(m_nbThreads_mutex);
            m_nbThreads--;

        }


}; // End class MessageQueueWrapper 


#endif // INCL_PLATFORMMNGT_MESSAGEQUEUEWRAPPER_HPP

