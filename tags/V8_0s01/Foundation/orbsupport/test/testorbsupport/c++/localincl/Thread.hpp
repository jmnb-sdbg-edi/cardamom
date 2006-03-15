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


#ifndef CDMW_TESTORBSUPPORT_THREAD_HPP
#define CDMW_TESTORBSUPPORT_THREAD_HPP

#include "Foundation/common/System.hpp"

#include <memory>
#include <list>

#include "Foundation/common/MacroDefs.h"
#include "Foundation/common/Exception.hpp"

namespace Cdmw
{

    namespace TestOrbSupport
    {

        // Forward declaration for use with the pimpl idiom

        class ThreadImpl;


        /**
        *Purpose:
        *<p> Exception thrown when the user ask for scheduling parameter
        * not allowed for him.
        *
        */

        class ThreadSchedulingException : public Exception
        {

            public:
                ThreadSchedulingException()
                throw();

            protected:

            private:

        };



        /**
        *Purpose:
        *<p> Exception thrown when the user try to lock twice a mutex
        *    from the same thread
        *
        */

        class DeadlockException : public Exception
        {

            public:
                DeadlockException()
                throw();

            protected:

            private:

        };




        /**
        *Purpose:
        *<p> This abstract class allow to create a new thread 
        * by derivation. When a thread
        * invokes the start method of the class, a new thread
        * is created and the run() method is called from the
        * new one. The developer must override run() to be
        * called from a new thread.
        */

        class Thread
        {

            public:

                // TODO: on solaris ThreadId is a unsigned int ( as stated in man)
                //    but on other OS, it may be a more complex structure
                //    We need to change ThreadId type into a class and define
                //    the equality operator
                /**
                * Identify a thread in the current process.
                */
                typedef unsigned int ThreadId;


                /**
                * Define values authorized for priority.
                */
                typedef int Priority;


#      ifndef _MSC_VER
                /**
                 * Default stack size when the value is not set.
                 */
                static const int DEFAULT_STACK_SIZE = 2048*1024;



                /**
                 * Default priority assigned to the thread when
                 * none is specified.
                 */
                static const int DEFAULT_PRIORITY = 0;
#      else

                // Workaround for Visual C++, it's why the enum is not named,
                //   we just need to introduces in the class scope the names
                //   with their values
                enum {
                    DEFAULT_PRIORITY = 0,
                    DEFAULT_STACK_SIZE = 2048 * 1024
            };

#      endif


                /**
                * Construct the thread object
                *
                * @exception OutOfMemoryException
                * @exception InternalErrorException
                * @exception BadParameterException              
                */
                Thread ()
                throw (
                    OutOfMemoryException,
                    InternalErrorException,
                    BadParameterException);



                //
                // Dtor
                //
                virtual
                ~Thread()
                throw();



                /**
                * Purpose:
                * <p> Allows to wait for the completion of the thread.
                * This methods return only when the thread has returned 
                * from the run() method.
                *
                * @exception DeadlockException the calling thread tries to
                *    wait on itself.
                * @exception InternalErrorException
                *
                */
                void
                join()
                throw (
                    DeadlockException,
                    InternalErrorException);


#      if defined(CDMW_HAVE_THREAD_STACK)

                /**
                * Purpose:
                * <p> Change the value of the stack size.
                * 
                * @exception BadParameterException the specified size is not valid
                * @exception AlreadyDoneException the is already started, its stack can
                *    not be changed.
                * @exception InternalErrorException     
                */
                void
                set_stackSize(size_t stackSize)
                throw (
                    BadParameterException,
                    AlreadyDoneException,
                    InternalErrorException );

                /**
                * Purpose:
                * <p> Returns the stack size of the thread.
                * 
                * @return the stack size of the thread
                *
                * @exception InternalErrorException
                */
                size_t
                get_stackSize() const
                throw (
                    InternalErrorException);


#          endif // defined(CDMW_HAVE_THREAD_STACK)


                /**
                * Purpose:
                * <p> Set the priority of the current thread.
                *
                * @exception BadParameterException the specified priority is not valid
                * @exception AlreadyDoneException the thread is already started
                *  its priority can no more be changed
                * @exception InternalErrorException     
                *
                */
                void
                set_priority( Priority priority)
                throw (
                    BadParameterException,
                    AlreadyDoneException,
                    InternalErrorException );

                /**
                * Purpose:
                * <p> get the priority of the current thread.
                *
                * @exception InternalErrorException
                *
                */
                Priority
                get_priority() const
                throw ( InternalErrorException );



                /**
                * Purpose:
                * <p> This method create a new thread and invoke
                * the run() method in this new thread.
                *
                * @return the id of the newly created thread.
                *
                * @exception OutOfMemoryException
                * @exception ThreadSchedulingException the caller is not allowed
                *            to start a thread from this priority
                * @exception InternalErrorException
                */
                ThreadId
                start()
                throw (
                    OutOfMemoryException,
                    ThreadSchedulingException,
                    InternalErrorException);


                /**
                * Purpose:
                * <p> Returns the identifier of the calling thread.
                * 
                * @return the id of the caller thread
                */
                static
                ThreadId
                self()
                throw() ;


            protected:

                /**
                 * The implementation shall be able to invoke
                 * run() from a new thread.
                 */

                friend class ThreadImpl;

                /**
                 * Purpose:
                 * <p>This methods must be overriden by the
                 * derived class to implement a thread object.
                 * This method is always called in a new thread
                 * after a start() call.
                 */
                virtual void run() throw() = 0;


                /**
                 * Purpose:
                 * <p>This methods is called by the OS dependent
                 * implementation in a new thread.
                 */
                void
                _run()
                throw();



            private:

                /**
                 * Body 
                 */
                std::auto_ptr<ThreadImpl> m_spImpl;


                /**
                * Set if the thread has been join, to avoid
                * memory leak, the thread is automatically join,
                * if it has ended, but not joined, in the destruction.
                *
                */
                bool m_has_been_joined;
        };


    } // End namespace TestOrbSupport

} // End namespace Cdmw

#endif //CDMW_TESTORBSUPPORT_THREAD_HPP


