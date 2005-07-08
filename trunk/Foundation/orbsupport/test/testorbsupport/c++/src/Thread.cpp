/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include "testorbsupport/Thread.hpp"

#include <cstdio>
#include <new>
#include <algorithm>

#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"

#include "testorbsupport/ThreadImpl.hpp"







namespace Cdmw
{

    namespace TestOrbSupport
    {



        /**
        * The mem_fun implementation shipped with VC++ is broken as 
        * it doesn't handle void function member. This implementation
        * handle it
        */

        template <typename TY>

        class mem_fun_t
        {

            public:
                mem_fun_t( void (TY::*ptr_memb_func)() )
                        : m_ptr_memb_func( ptr_memb_func )
                {
                }

                void operator()(TY* obj_ptr)
                {
                    (obj_ptr->*m_ptr_memb_func)();
                }

            private:

                void (TY::*m_ptr_memb_func)();

        };


        template <typename TY>
        mem_fun_t<TY> mem_fun (void (TY::*ptr_mem_func)() )
        {
            return mem_fun_t<TY>(ptr_mem_func) ;
        }





        //
        // Ctor
        //
        ThreadSchedulingException::ThreadSchedulingException()
        throw()
                : Exception( Exception::SAFE, "Unable to set scheduling parameters" )
        {

        }



        //
        // Ctor
        //
        DeadlockException::DeadlockException()
        throw()
                : Exception( Exception::SAFE, "A deadlock between two thread has been detected" )
        {

        }



        //
        // Ctor
        //
        Thread::Thread()
        throw (
            OutOfMemoryException,
            InternalErrorException,
            BadParameterException )

                : m_spImpl( NULL ),
                m_has_been_joined(false)
        {
            try
            {


                // Pimpl idiom
#   ifdef _MSC_VER
                // The auto_ptr implementation shipped with visual is not compliant
                //  and doesn't provide reset(), but it is broken as it used an assignement
                //  operator with const auto_ptr&, we use this deviation
                m_spImpl = std::auto_ptr<ThreadImpl>(new ThreadImpl( this ) );
#   else

                m_spImpl.reset( new ThreadImpl( this ) );
#   endif


            }
            catch ( const std::bad_alloc& )
            {

                CDMW_THROW(OutOfMemoryException);
            }
        }


        //
        // Dtor
        //
        Thread::~Thread()
        throw()
        {
            // If the thread is destroy, it must have ended, its
            //  task before
            CDMW_ASSERT( ! m_spImpl->is_running() );

            // In case of no other thread has waiting for us
            //  we join to avoid memory leak
            bool threadImpl_is_ended = m_spImpl->is_ended();

            if ( threadImpl_is_ended && ( ! m_has_been_joined ) )
            {

                this->join();

            }

        }



        //
        // Start the new thread
        //
        Thread::ThreadId Thread::start()
        throw (
            OutOfMemoryException,
            ThreadSchedulingException,
            InternalErrorException)
        {
            CDMW_ASSERT ( m_spImpl.get() != NULL );

            return m_spImpl->start();
        }



        //
        // Start the user code from the new thread
        //
        void
        Thread::_run()
        throw ()
        {
            // We invoke the user code
            this->run();
        }




        //
        // Return the id of the calling thread
        //
        Thread::ThreadId
        Thread::self()
        throw()
        {

            return ThreadImpl::self();

        }



        //
        // Wait until the completion if the thread
        //
        void
        Thread::join()
        throw (
            DeadlockException,
            InternalErrorException)
        {
            // The thread has been joined by another one,
            //  it will be useless to "auto-join" in the
            //  destructor to avoid memory leak
            m_has_been_joined = true;

            m_spImpl->join();
        }



#if defined(CDMW_HAVE_THREAD_STACK) 
        //
        // Set the size of the stack
        //
        void
        Thread::set_stackSize(size_t stackSize)
        throw (
            BadParameterException,
            AlreadyDoneException,
            InternalErrorException )
        {
            m_spImpl->set_stackSize( stackSize );
        }



        //
        // get the size of the stack
        //
        size_t
        Thread::get_stackSize() const
        throw ( InternalErrorException)
        {
            return m_spImpl->get_stackSize();
        }

#endif // defined(CDMW_HAVE_THREAD_STACK)



        //
        // Set the priority of the thread
        //
        void
        Thread::set_priority( Priority priority)
        throw (
            BadParameterException,
            AlreadyDoneException,
            InternalErrorException )
        {
            m_spImpl->set_priority(priority);
        }



        //
        // get the priority of the thread
        //
        Thread::Priority
        Thread::get_priority() const
        throw (
            InternalErrorException )
        {
            return m_spImpl->get_priority();
        }

    } // End namespace TestOrbSupport

} // End namespace Cdmw

