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


#include "Foundation/ossupport/Win32DllObject.hpp"

#ifdef  _WIN32

namespace Cdmw
{

    namespace OsSupport
    {

        //
        // Constructor
        //
        Win32dllObject::Win32dllObject()
        {}

        //
        // Desctructor
        //
        Win32dllObject::~Win32dllObject()
        {}

        //
        // print
        //

        void Win32dllObject::print(std::ostream& os)
        {
            os << m_OS_handle;
        }

        //
        // dlopen
        //
        Win32dllObject* Win32dllObject::dlopen(const char* libname, int flag)
        {
            //we create an instance of Win32dllObject by dlopen
            Win32dllObject* obj = new Win32dllObject();
            obj->m_OS_handle = ::LoadLibrary(libname);
            return *obj;
        }

        int Win32dllObject::dlclose(void)
        {
            return ::FreeLibrary(m_OS_handle);
        }

        //
        // dlsym
        //
        Win32dllObject::FunctionHandle Win32dllObject::dlsym(const char* symname)
        {
            return (FunctionHandle)::GetProcAddress(m_OS_handle, symname);
        }

        //
        // dlerror
        //
        std::string Win32dllObject::dlerror(void)
        {
            char buf[128];
            ::FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,
                             NULL,
                             ::GetLastError(),
                             0,
                             buf,
                             sizeof buf / sizeof buf[0],
                             NULL);
            return buf;
        }


    } // End namespace ossupport

} // End namespace Cdmw


#endif

