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


#ifndef INCL_DEFAULTSHAREDOBJECT_OS_HPP
#define INCL_DEFAULTSHAREDOBJECT_OS_HPP

#include "Foundation/common/MacroDefs.h"
#ifdef CDMW_POSIX
//#ifndef AIX

#include <string>
#include <dlfcn.h>
#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/ossupport/SharedObjectHandle.hpp"

namespace Cdmw
{

    namespace OsSupport
    {

        class DefaultSharedObjectHandle
        {

            public :
                typedef void *OsShlibHandle;
                typedef void *FunctionHandle;

        };

        class DefaultSharedObject : public SharedObjectHandle_T<DefaultSharedObjectHandle>
        {

            public:
                //
                // dlopen implementation
                //

                static DefaultSharedObject* dlopen(const char* libname, int flag = RTLD_LAZY);

                //
                // dlclose implementation
                //

                int dlclose(void);

                //
                // dlsym implementation
                //

                DefaultSharedObjectHandle::FunctionHandle dlsym(const char* symname);

                //
                // dlerror implementation
                //

                static std::string dlerror(void);

                //
                // print
                //

                void print(std::ostream& os);

                //
                // Equality and inequality operation
                //

                bool
                is_null()
                {
                    return m_OS_handle == NULL;
                }


                bool
                operator==(const DefaultSharedObject & rhs) const
                {
                    return m_OS_handle == rhs.m_OS_handle;
                }

                bool
                operator!=(const DefaultSharedObject & rhs) const
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

                DefaultSharedObject();

                //
                // Handle of the library
                //

                OsShlibHandle m_OS_handle;

        };


    } // End namespace ossupport

} // End namespace Cdmw



//#endif
#endif
#endif
