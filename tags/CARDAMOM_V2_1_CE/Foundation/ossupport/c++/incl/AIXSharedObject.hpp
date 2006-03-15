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


#ifndef INCL_AIXSHAREDOBJECT_OS_HPP
#define INCL_AIXSHAREDOBJECT_OS_HPP
#ifdef AIX

#include <string>
#include <list>
#include <map>
#include "Foundation/common/MacroDefs.h"
#include <dlfcn.h>
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/ossupport/SharedObjectHandle.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        class AIXSharedObjectHandle
        {

            public:
                typedef void *OsShlibHandle;
                typedef void *FunctionHandle;
        };


        class AIXSharedObjectImpl;

        class AIXSharedObject : public SharedObjectHandle_T<AIXSharedObjectHandle>
        {

            public:
                //
                // dlopen implementation
                //

                static AIXSharedObject* dlopen(const char* libname, int flag = RTLD_LAZY);

                //
                // dlclose implementation
                //

                int dlclose(void);

                //
                // dlsym implementation
                //

                AIXSharedObjectHandle::FunctionHandle dlsym(const char* symname);

                //
                // dlerror implementation
                //

                static std::string dlerror(void);

                //
                //print
                //

                void print(std::ostream& os);

            private:
                //
                // Constructor
                //

                explicit AIXSharedObject(AIXSharedObjectImpl * impl);

                //
                // Copy constructor
                //

                AIXSharedObject(const AIXSharedObject & so);

                //
                // operator =
                //

                AIXSharedObject & operator=(const AIXSharedObject & rhs);

            public:

                //
                // destructor
                //
                ~AIXSharedObject();

                //
                // Equality and inequality operation
                //

                bool
                is_null()
                {
                    return m_OS_handle == NULL;
                }


                bool
                operator==(const AIXSharedObject & rhs) const
                {
                    return m_OS_handle == rhs.m_OS_handle;
                }

                bool
                operator!=(const AIXSharedObject & rhs) const
                {
                    return m_OS_handle != rhs.m_OS_handle;
                }

                bool
                operator!() const
                {
                    return m_OS_handle == NULL;
                }

                operator bool() const
                {
                    return m_OS_handle != NULL;
                }

            private:
                //
                // Constructor
                //
                AIXSharedObject();

                //
                // Handle of the library
                //
                OsShlibHandle m_OS_handle;


                AIXSharedObjectImpl* m_AIX_shared_object_impl;


        };



    } // End namespace ossupport

} // End namespace Cdmw


#endif
#endif

