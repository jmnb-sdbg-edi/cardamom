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


#include "Foundation/common/MacroDefs.h"


#ifdef  _WIN32

#include <windows.h>
#define RTLD_LAZY 1
#include "Foundation/ossupport/SharedObjectHandle.hpp"


namespace Cdmw
{

    namespace OsSupport
    {

        class Win32DllObjectHandle
        {
                typedef HINSTANCE OsShlibHandle;
                typedef void (*FunctionHandle)(LPTSTR);
        }

        class Win32DllObject : public SharedObjectHandle_T<Win32DllObjectHandle>
        {

            public:

                //
                // dlopen implementation
                //

                static Win32DllObject* dlopen(const char* libname, int flag = RTLD_LAZY);

                //
                // dlclose implementation
                //

                int dlclose(void);

                //
                // dlsym implementation
                //

                Win32DllObjectHandle::FunctionHandle dlsym(const char* symname);

                //
                // dlerror implementation
                //

                static std::string dlerror(void);

                //
                // destructor
                //

                ~Win32dllObject();

                //
                // Equality and inequality operation
                //

                bool
                is_null()
                {
                    return m_OS_handle == NULL;
                }

                bool
                operator==(const Win32DllObject & rhs) const
                {
                    return m_OS_handle == rhs.m_OS_handle;
                }

                bool
                operator!=(const Win32DllObject & rhs) const
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

                Win32dllObject();

                //
                // Handle of the library
                //

                OsShlibHandle m_OS_handle;

        };



    } // End namespace ossupport

} // End namespace Cdmw




#endif

