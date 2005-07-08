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


#include "Foundation/osthreads/Mutex.hpp"

#include "osthreads/MutexImpl.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        //
        // Ctor
        //
        Mutex::Mutex()
        throw (
            OutOfMemoryException,
            InternalErrorException)

                : m_spImpl (NULL)
        {
            try
            {
#   ifdef _MSC_VER
                // The auto_ptr implementation shipped with visual is not compliant
                //  and doesn't provide reset(), but it broken as it uses an assignement
                //  operator with const auto_ptr&, we use this deviation
                m_spImpl = std::auto_ptr<MutexImpl> (new MutexImpl());
#   else

                m_spImpl.reset (new MutexImpl);
#       endif



            }
            catch (const std::bad_alloc&)
            {

                CDMW_THROW(OutOfMemoryException);
            }
        }



        //
        // Dtor
        //
        Mutex::~Mutex() throw()
        {
            // Nothing to do
        }




        //
        // Lock the mutex
        //
        void

        Mutex::lock ()
            throw (
                AlreadyDoneException,
                InternalErrorException)
        {

            m_spImpl->lock ()

            ;
        }



        //
        // Unlock the mutext
        //
        void
        Mutex::unlock()
        throw (
            PermissionDeniedException,
            InternalErrorException)
        {
            m_spImpl->unlock();
        }

        //
        // is_locked
        //
        bool
        Mutex::is_locked()
        throw ()
        {
            bool is_locked = m_spImpl->is_locked();
            return is_locked;
        }

        //
        // Used by the Condition implementation to
        //    retreive the implementaion mutex
        //
        MutexImpl&
        Mutex::get_implementation()
        throw()
        {
            return *m_spImpl;
        }



    } // End namespace osthreads

} // End namespace Cdmw



