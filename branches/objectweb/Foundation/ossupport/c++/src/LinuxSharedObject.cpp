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


#ifdef linux

#include <iostream>
#include <Foundation/ossupport/LinuxSharedObject.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include "ossupport/Which.hpp"
#include "ossupport/LinuxSharedObjectImpl.hpp"

namespace Cdmw {
    namespace OsSupport {

        /**
         * Constructor
         */
        LinuxSharedObject::LinuxSharedObject()
            : m_OS_handle(NULL),
              m_Linux_shared_object_impl(NULL)
        {}

        /**
         * Constructor
         */
        LinuxSharedObject::LinuxSharedObject(LinuxSharedObjectImpl* impl)
            : m_Linux_shared_object_impl(impl)
        {
            if (impl != NULL) {
                m_OS_handle = impl->m_OS_handle;
            } else {
                m_OS_handle = NULL;
            }
        }

        /**
         * Copy constructor
         */
        LinuxSharedObject::LinuxSharedObject(const LinuxSharedObject& so)
            : m_OS_handle (so.m_OS_handle),
              m_Linux_shared_object_impl(so.m_Linux_shared_object_impl)
        {
            if (m_Linux_shared_object_impl != NULL) {
                m_Linux_shared_object_impl->_add_ref();
            }
        }

        /**
         * Operator =
         */
        LinuxSharedObject&
        LinuxSharedObject::operator=(const LinuxSharedObject& rhs)
        {
            if (this != &rhs) {
                if (rhs.m_Linux_shared_object_impl != NULL) {
                    LinuxSharedObjectImpl* impl = NULL;

                    if (this->m_Linux_shared_object_impl != NULL) {
                        impl = this->m_Linux_shared_object_impl;
                    }

                    this->m_Linux_shared_object_impl = rhs.m_Linux_shared_object_impl;
                    this->m_OS_handle = rhs.m_OS_handle;

                    if (this-> m_Linux_shared_object_impl != NULL) {
                        this->m_Linux_shared_object_impl->_add_ref();
                    }

                    if (impl != NULL) {
                        impl->_remove_ref();
                    }
                }
            }

            return *this;
        }

        /**
         * Destructor
         */
        LinuxSharedObject::~LinuxSharedObject()
        {
            if (m_Linux_shared_object_impl != NULL) {
                m_Linux_shared_object_impl->_remove_ref();
            }
        }

        /**
         * dlerror
         */
        std::string LinuxSharedObject::dlerror(void)
        {
            return LinuxSharedObjectImpl::dlerror();
        }

        /**
         * print
         */
        void LinuxSharedObject::print(std::ostream& os)
        {
            if ( m_OS_handle != NULL) {
                os << m_OS_handle;
            }
        }

        /**
         * dlopen
         */
        LinuxSharedObject*
        LinuxSharedObject::dlopen(const char* libname, int flag)
        {
            LinuxSharedObjectImpl* obj = LinuxSharedObjectImpl::dlopen(libname, flag);
            LinuxSharedObject* obj_Linux_shared_object = new LinuxSharedObject(obj);
            return obj_Linux_shared_object;
        }

        /**
         * dlclose
         */
        int
        LinuxSharedObject::dlclose(void)
        {
            if (m_Linux_shared_object_impl != NULL) {
                return m_Linux_shared_object_impl->dlclose();
            } else {
                return 1;
            }
        }

        /**
         * dlsym
         */
        LinuxSharedObject::FunctionHandle
        LinuxSharedObject::dlsym(const char* symname)
        {
            if (m_Linux_shared_object_impl != NULL) {
                return m_Linux_shared_object_impl->dlsym(symname);
            } else {
                return NULL;
            }
        }

    } // End namespace ossupport
} // End namespace Cdmw

#endif
