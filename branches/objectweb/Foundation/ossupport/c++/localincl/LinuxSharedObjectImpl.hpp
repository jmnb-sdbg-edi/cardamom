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


#ifndef INCL_LINUXSHAREDOBJECTIMPL_OS_HPP
#define INCL_LINUXSHAREDOBJECTIMPL_OS_HPP

#ifdef linux

#include <string>
#include <list>
#include <map>
#include <dlfcn.h>
#include <Foundation/common/MacroDefs.h>
#include <Foundation/common/Exception.hpp>
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/ossupport/SharedObjectHandle.hpp>

namespace Cdmw {
    namespace OsSupport {

        class LinuxSharedObjectHandleImpl {
            public:
                typedef void* OsShlibHandle;
                typedef void* FunctionHandle;
        };

        class LinuxSharedObjectImpl
            : public SharedObjectHandle_T<LinuxSharedObjectHandleImpl>
        {
                /**
                 * This class must be used only by the LinuxSharedObject class.
                 */

                friend class LinuxSharedObject;

            private:
                /**
                 * dlopen implementation
                 */

                static LinuxSharedObjectImpl*
                dlopen(const char* libname, int flag = RTLD_LAZY);

                /**
                 * dlclose implementation
                 */

                int
                dlclose(void);

                /**
                 * dlsym implementation
                 */

                FunctionHandle
                dlsym(const char* symname);

                /**
                 * dlerror implementation
                 */

                static std::string
                dlerror(void);

                /**
                 * print
                 */

                void
                print(std::ostream& os);

                /**
                 * add ref
                 */

                void
                _add_ref(void);

                /**
                 * remove ref
                 */

                void
                _remove_ref(void);


                bool
                is_null(void);

                /**
                 * Handle of the library
                 */

                OsShlibHandle m_OS_handle;

                /**
                 * Constructor
                 */

                LinuxSharedObjectImpl(std::string libraryName, OsShlibHandle OS_Handle);

            public:

                /**
                 * destructor
                 */

                virtual ~LinuxSharedObjectImpl();

            private:
            
                /**
                 * A Mutex to protect from concurrent access to internal
                 * reference counter.
                 */

                static Cdmw::OsSupport::Mutex M_count_mutex;

                /**
                 * Map
                 */

                typedef std::map<std::string , LinuxSharedObjectImpl *> SharedObjectsMap;
                static SharedObjectsMap M_mapDlLibraryStringToObject;

                /**
                 * Instance reference counter.
                 */

                unsigned long m_ref_count;

                /**
                 * Instance reference counter.
                 */

                unsigned long m_dlopen_count;

                /**
                 * Name of the library
                 */

                std::string m_libraryName;
        };

    } // End namespace ossupport
} // End namespace Cdmw

#endif
#endif
