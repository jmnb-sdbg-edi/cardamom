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


#ifndef INCL_PLATFORMMNGT_TASKFACTORY_HPP 
#define INCL_PLATFORMMNGT_TASKFACTORY_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"

#include <string>


namespace Cdmw
{
namespace PlatformMngt
{


/**
*Purpose:
*<p>
* Invalid Task Id Exception
*
* @param taskId the task id
*/
class InvalidTaskIdException : public Exception {
    
    public:
        InvalidTaskIdException(const std::string& taskId)
            : Exception( Exception::SAFE, "Invalid Task Id" )
        {
            m_what = std::string(Exception::what()) + " : " + taskId;
        }

  ~InvalidTaskIdException() throw() {};
  virtual const char* what() const throw()
        {
            return m_what.c_str();
        }

    protected:

    private:
       std::string m_what;

};



class TaskObserver;


/**
*Purpose:
* The abstract simple task.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class Task : public OsSupport::Thread
{

    public:

        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~Task() throw();


        /**
         *Purpose:
         *<p> Starts the task.
         */
        virtual void startTask() throw();


    protected:

        /**
        * Purpose:
        * <p> Constructor.
        *
        * @param id the identifier of the task.
        * @param observer the observer of the task 
        *
        * @exception BadParameterException if id is empty or observer is NULL.
        */ 
        Task(const std::string& id, TaskObserver* observer);


        /**
         *Purpose:
         *<p> Ends the task by calling the observer.
         */
        void end() throw();


        /**
         *Purpose:
         *<p>
         */
        virtual void run() throw();


        /**
         *Purpose:
         *<p> The actual activity of the task.
         * Returns true if the execution has succeeded.
         */
        virtual bool execute() throw() = 0;


    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        Task(const Task& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        Task& operator=(const Task& rhs);



    protected:
       
        /**
         * The identifier of this task.
         */
        std::string m_id;

    private:

        /**
         * The observer.
         */
        TaskObserver* m_observer;


}; // End Task


/**
*Purpose:
* The simple task factory.
*<p> 
*
*Features:
*<p> Thread safe. Exception safe.
*/
class TaskFactory
{

    public:

        /**
        * Purpose:
        * <p> Destructor.
        */ 
        virtual ~TaskFactory();


        /**
         *Purpose:
         *<p> Creates the specified task.
         *
         * @param taskId the identifier of the task.
         * @param taskObserver the observer of the created task.
         */
        virtual Task* createTask(const std::string& taskId,
            TaskObserver* taskObserver)
            throw(InvalidTaskIdException, OutOfMemoryException) = 0;


    protected:

        /**
        * Purpose:
        * <p> Constructor.
        */ 
        TaskFactory();


    private:

        /**
        * Purpose:
        * <p>
        * Copy constructor is not allowed.
        */ 
        TaskFactory(const TaskFactory& rhs);


        /**
        * Purpose:
        * <p>
        * Assignment operator is not allowed.
        */ 
        TaskFactory& operator=(const TaskFactory& rhs);


}; // End class TaskFactory 

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_TASKFACTORY_HPP

