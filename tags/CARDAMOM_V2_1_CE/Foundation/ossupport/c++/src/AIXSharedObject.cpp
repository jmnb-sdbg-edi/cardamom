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


#ifdef AIX

#include "Foundation/ossupport/AIXSharedObject.hpp"
#include "ossupport/Which.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include <iostream>
#include "ossupport/AIXSharedObjectImpl.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        //
        // Constructor
        //
        AIXSharedObject::AIXSharedObject()
                : m_AIX_shared_object_impl(NULL), m_OS_handle(NULL)
        {}

        //
        // Constructor
        //
        AIXSharedObject::AIXSharedObject(AIXSharedObjectImpl * impl)
                : m_AIX_shared_object_impl(impl)
        {

            if (impl != NULL)
            {
                m_OS_handle = impl->m_OS_handle;
            }
        }

        //
        // Copy constructor
        //
        AIXSharedObject::AIXSharedObject(const AIXSharedObject & so)
                : m_AIX_shared_object_impl(so.m_AIX_shared_object_impl), m_OS_handle (so.m_OS_handle)
        {

            if (m_AIX_shared_object_impl != NULL)
                m_AIX_shared_object_impl->_add_ref();

        }

        //
        // Operator=
        //
        AIXSharedObject &
        AIXSharedObject::operator=(const AIXSharedObject &rhs)
        {
            if (this != &rhs)
            {

                if ( rhs.m_AIX_shared_object_impl != NULL)
                {
                    AIXSharedObjectImpl * impl = NULL ;

                    if (this->m_AIX_shared_object_impl != NULL)
                    {
                        impl = this->m_AIX_shared_object_impl;
                    }

                    this->m_AIX_shared_object_impl = rhs.m_AIX_shared_object_impl;

                    this->m_OS_handle = rhs.m_OS_handle;

                    if (this-> m_AIX_shared_object_impl != NULL)
                        this->m_AIX_shared_object_impl->_add_ref();

                    if (impl != NULL)
                        impl->_remove_ref();
                }
            }

            return *this;
        }

        //
        // Destructor
        //
        AIXSharedObject::~AIXSharedObject()
        {
            if (m_AIX_shared_object_impl != NULL)
                m_AIX_shared_object_impl->_remove_ref();
        }


        //
        // dlerror
        //
        std::string AIXSharedObject::dlerror(void)
        {
            return AIXSharedObjectImpl::dlerror();
        }

        //
        // print
        //
        void AIXSharedObject::print(std::ostream& os)
        {
            if ( m_OS_handle != NULL)
                os << m_OS_handle;
        }

        //
        // dlopen
        //
        AIXSharedObject* AIXSharedObject::dlopen(const char* libname, int flag)
        {
            AIXSharedObjectImpl * obj = AIXSharedObjectImpl::dlopen(libname, flag);
            AIXSharedObject* obj_AIX_shared_object = new AIXSharedObject (obj);
            return obj_AIX_shared_object;
        }

        //
        // dlclose
        //
        int AIXSharedObject::dlclose(void)
        {
            if (m_AIX_shared_object_impl != NULL)
                return m_AIX_shared_object_impl->dlclose();
            else
                return 1;
        }


        //
        // dlsym
        //
        AIXSharedObject::FunctionHandle AIXSharedObject::dlsym(const char* symname)
        {
            if (m_AIX_shared_object_impl != NULL)
                return m_AIX_shared_object_impl->dlsym(symname);
            else
                return NULL;
        }





    } // End namespace ossupport

} // End namespace Cdmw


#endif

