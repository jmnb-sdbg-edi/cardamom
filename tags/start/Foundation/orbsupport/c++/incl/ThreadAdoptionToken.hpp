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


#ifndef INCL_ORBSUPPORT_THREAD_ADOPTION_TOKEN_HPP
#define INCL_ORBSUPPORT_THREAD_ADOPTION_TOKEN_HPP

#include "Foundation/orbsupport/CORBA.hpp"

namespace Cdmw
{

    namespace OrbSupport
    {

        /**
        *Purpose:
        *<p>    This class provide a object identifying registration of a calling thread
        * with the ORB-specific threading system. Its deletion shall automatically unregister
        * the calling thread. ThreadAdoptionToken instances use reference counting to keep 
        * track of each instance and delete it automatically when the reference count
        * drops to zero.
        *
        *Features:
        *<p> Thread safe
        *
        */

        class ThreadAdoptionToken
        {

            public:

                /**
                 * Purpose:
                 * <p> Used to increment the reference count of a ThreadAdoptionToken instance.
                 */
                virtual void _add_ref() throw() = 0;

                /**
                 * Purpose:
                 * <p> Used to decrement the reference count of a ThreadAdoptionToken instance
                 * and delete the instance when the reference count drops to zero.
                 */
                virtual void _remove_ref() throw() = 0;

            protected:
                /**
                 * Purpose:
                 * <p> Destructor is protected to enforce the use of _add_ref/_remove_ref
                 * operations and allocation of the instance using new.
                 */
                virtual ~ThreadAdoptionToken() throw()
                {}

                ThreadAdoptionToken() throw()
                {}

                ThreadAdoptionToken(const ThreadAdoptionToken&) throw()
                {}

                ThreadAdoptionToken& operator=(const ThreadAdoptionToken&) throw()
                {
                    return * this;
                }
        }

        ; // End class ThreadAdoptionToken


        // template <typename T>
        // class T_var
        // {
        // public:

        //     T_var() : m_ptr(0) { }
        //     T_var(T* p) : m_ptr(p) { }
        //     T_var(const T_var<T>& r)
        //         : m_ptr(r.m_ptr)
        //         {
        //             if (m_ptr != 0)
        //                 m_ptr->_add_ref();
        //         }

        //     ~T_var()
        //         {
        //             if (m_ptr != 0)
        //                 m_ptr->_remove_ref();
        //         }


        //     T_var& operator=(T* p)
        //         {
        //             if (m_ptr != 0) m_ptr->_remove_ref();
        //             m_ptr = p;
        //             return *this;
        //         }

        //     T_var&
        //     operator=(const T_var<T>& b)
        //         {
        //             if (m_ptr != b.m_ptr) {
        //                 if (m_ptr != 0) m_ptr->_remove_ref();
        //                 if ((m_ptr = b.m_ptr) != 0)
        //                     m_ptr->_add_ref();
        //             }
        //             return *this;
        //         }

        //     T* operator->() const { return m_ptr; }

        //     T*  in() const { return m_ptr; }
        //     T*& inout() { return m_ptr; }
        //     T*& out()
        //         {
        //             if (m_ptr != 0) m_ptr->_remove_ref();
        //             m_ptr = 0;
        //             return m_ptr;
        //         }
        //     T* _retn()
        //         {
        //             T* retval = m_ptr;
        //             m_ptr = 0;
        //             return retval;
        //         }

        // private:
        //     T* m_ptr;

        // }; // End class T_var



        /**
        *Purpose:
        *<p>    This class provide a wrapper for safely manipulate ThreadTokenObjects.
        * It automatically calls _add_ref() and _remove_ref() operations when needed.
        * See the T_var description within the OMG IDL to C++ language mapping.
        *Features:
        *<p> Thread safe
        *
        */

        class ThreadAdoptionToken_var
        {

            public:

                ThreadAdoptionToken_var() throw() : m_token(0)
                { }

                ThreadAdoptionToken_var(ThreadAdoptionToken* p) throw() : m_token(p)
                { }

                ThreadAdoptionToken_var(const ThreadAdoptionToken_var& r) throw()
                        : m_token(r.m_token)
                {
                    if (m_token != 0)
                        m_token->_add_ref();
                }

                ~ThreadAdoptionToken_var() throw()
                {
                    if (m_token != 0)
                        m_token->_remove_ref();
                }


                ThreadAdoptionToken_var& operator=(ThreadAdoptionToken* p) throw()
                {
                    if (m_token != 0)
                        m_token->_remove_ref();

                    m_token = p;

                    return *this;
                }

                ThreadAdoptionToken_var&
                operator=(const ThreadAdoptionToken_var& b) throw()
                {
                    if (m_token != b.m_token)
                    {
                        if (m_token != 0)
                            m_token->_remove_ref();

                        if ((m_token = b.m_token) != 0)
                            m_token->_add_ref();
                    }

                    return *this;
                }

                ThreadAdoptionToken* operator->() const throw()
                {
                    return m_token;
                }

                ThreadAdoptionToken* in() const throw()
                {
                    return m_token;
                }

                ThreadAdoptionToken*& inout() throw()
                {
                    return m_token;
                }

                ThreadAdoptionToken*& out() throw()
                {
                    if (m_token != 0)
                        m_token->_remove_ref();

                    m_token = 0;

                    return m_token;
                }

                ThreadAdoptionToken* _retn() throw()
                {
                    ThreadAdoptionToken* retval = m_token;
                    m_token = 0;
                    return retval;
                }

            private:
                ThreadAdoptionToken* m_token;

        }

        ; // End class ThreadAdoptionToken_var





    } // End namespace OrbSupport
} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_THREAD_ADOPTION_TOKEN_HPP

