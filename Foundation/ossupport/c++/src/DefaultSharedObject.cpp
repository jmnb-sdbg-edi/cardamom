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


#include <Foundation/ossupport/DefaultSharedObject.hpp>

#ifdef CDMW_POSIX
//#ifndef AIX


#include <iostream>

namespace Cdmw
{

    namespace OsSupport
    {

        //
        // Constructor
        //
        DefaultSharedObject::DefaultSharedObject()
                : m_OS_handle(NULL)
        {}

        //
        // print
        //

        void DefaultSharedObject::print(std::ostream& os)
        {
            if ( m_OS_handle != NULL)
                os << m_OS_handle;
        }

        //
        // dlopen
        //
        DefaultSharedObject* DefaultSharedObject::dlopen(const char* libname, int flag)
        {
            //we create an instance of DefaultSharedObject by dlopen
            DefaultSharedObject* obj = new DefaultSharedObject();
            obj->m_OS_handle = ::dlopen(libname, flag);
            return obj;

        }

        //
        // dlclose
        //
        int DefaultSharedObject::dlclose(void)
        {
            //the linux dlclose don't return the same status as Sun dlclose
#ifdef sun
            return ::dlclose(m_OS_handle);

#elif linux

            int status = 0;
            status = ::dlclose(m_OS_handle);

            if (status == -1)
                return 1;
            else
                return status;

#endif
        }

        //
        //dlsym
        //
        DefaultSharedObjectHandle::FunctionHandle DefaultSharedObject::dlsym(const char* symname)
        {
            return ::dlsym(m_OS_handle, symname);
        }

        //
        // dlerror
        //
        std::string DefaultSharedObject::dlerror(void)
        {
            return ::dlerror();
        }


    } // End namespace ossupport

} // End namespace Cdmw



//#endif
#endif
