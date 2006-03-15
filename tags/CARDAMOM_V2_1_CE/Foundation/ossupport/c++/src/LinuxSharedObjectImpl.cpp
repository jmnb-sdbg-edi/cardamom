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


#ifdef linux

#include <iostream>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/ossupport/LinuxSharedObject.hpp>
#include "ossupport/LinuxSharedObjectImpl.hpp"
#include "ossupport/Which.hpp"

namespace Cdmw {
    namespace OsSupport {

        /**
         * Static objects.
         */
        std::map<std::string, LinuxSharedObjectImpl*>
        LinuxSharedObjectImpl::M_mapDlLibraryStringToObject;

        Cdmw::OsSupport::Mutex
        LinuxSharedObjectImpl::M_count_mutex;

        /**
         * Constructor
         */
        LinuxSharedObjectImpl::LinuxSharedObjectImpl(std::string libraryName,
                                                     OsShlibHandle OS_Handle)
            : m_OS_handle(OS_Handle),
              m_ref_count(0),
              m_dlopen_count(1),
              m_libraryName(libraryName)
        {}

        /**
         * destructor
         */
        LinuxSharedObjectImpl::~LinuxSharedObjectImpl() {}

        /**
         * dlerror
         */
        std::string
        LinuxSharedObjectImpl::dlerror(void)
        {
            return ::dlerror();
        }

        /**
         * print
         */
        void
        LinuxSharedObjectImpl::print(std::ostream& os)
        {
            if (m_OS_handle != NULL) {
                os << m_OS_handle;
            }
        }

        /**
         * is_null
         */
        bool
        LinuxSharedObjectImpl::is_null()
        {
            return (m_OS_handle == NULL);
        }

        /**
         * dlopen
         *
         * the dlopen returns a new LinuxSharedObjectImpl object if it is the
         * first call for the library else it returns the LinuxSharedObjectImpl
         * object (stored in a map) that refers to the library.
         */
        LinuxSharedObjectImpl*
        LinuxSharedObjectImpl::dlopen(const char* libname, int flag)
        {
            std::string cmd = (libname != NULL) ? libname : "";
            LinuxSharedObjectImpl::OsShlibHandle temp = NULL;
            LinuxSharedObjectImpl* pimpl = NULL;
            std::string result;
            struct stat finfo;

            // take the LD_LIBRARY_PATH environment variable
            std::string path_list = getenv("LD_LIBRARY_PATH");

            if (((path_list != "") && (path_list[0] != '\0'))
                || (stat(cmd.c_str(), &finfo) >= 0))
            {
                unsigned int path_index = 0;
                
                // find_command_in_path return the full path of the library
                result = find_command_in_path(cmd, path_list, &path_index, "LIB");

                // if the library is found
                if (result != "") {
                    // find the library in the map
                    SharedObjectsMap::iterator it =
                        M_mapDlLibraryStringToObject.find(cmd);
                    
                    CDMW_MUTEX_GUARD(LinuxSharedObjectImpl::M_count_mutex);
                    
                    // if the library isn't in the map
                    // we run the dlopen and return the value
                    // (if NULL, dlopen raised an error)
                    // then save the library name and the handle into the map.

                    if (it == M_mapDlLibraryStringToObject.end()) {
                        temp = ::dlopen(libname, flag);

                        if (temp == NULL) {
                            return NULL ;
                        }

                        pimpl = new LinuxSharedObjectImpl(cmd, temp);
                        M_mapDlLibraryStringToObject[cmd] = pimpl;
                    } else {
                        // if the library name is found in the map,
                        // we increase the reference counter.
                        pimpl = (*it).second;
                        ++(pimpl->m_dlopen_count);
                    }
                } else {
                    // the library was not found
                    
                    // if the libname is NULL
                    // (dlopen must return a handle under the main process)

                    if (libname == NULL) {
                        // find if NULL is already opened
                        
                        SharedObjectsMap::iterator it =
                            M_mapDlLibraryStringToObject.find(NULL);
                            //M_mapDlLibraryStringToObject.find(cmd);
                            
                        CDMW_MUTEX_GUARD(LinuxSharedObjectImpl::M_count_mutex);
                        
                        // if the NULL reference is not already opened
                        if (it == M_mapDlLibraryStringToObject.end()) {
                            temp = ::dlopen(libname, flag);
                            
                            // store the NULL and the handle into the map
                            pimpl = new LinuxSharedObjectImpl(libname, temp);
                            M_mapDlLibraryStringToObject[libname] = pimpl;
                        } else {
                            // increase the reference counter
                            pimpl = (*it).second;
                            ++(pimpl->m_dlopen_count);
                            cmd = libname;
                        }
                    } else {
                        // the library is not found or the libname is not NULL,
                        // call the dlopen to store the error in the dlerror.
                        temp = ::dlopen(libname, flag);
                    }
                }
            }

            if (pimpl != NULL) {
                pimpl->_add_ref();
            }

            return pimpl;
        }

        /**
         * dlclose
         */
        int
        LinuxSharedObjectImpl::dlclose(void)
        {
            CDMW_MUTEX_GUARD(LinuxSharedObjectImpl::M_count_mutex);
            
            // decrease the reference counter
            if (m_dlopen_count != 0) {
                --m_dlopen_count;
            }

            // if the reference counter equals zero,
            // delete the library reference into the map
            if (m_dlopen_count == 0) {
                LinuxSharedObjectImpl::M_mapDlLibraryStringToObject.erase(m_libraryName);
                // return the dlclose
                return ::dlclose(m_OS_handle);
            } else {
                return 0;
            }
        }

        /**
         * dlsym
         */
        LinuxSharedObjectImpl::FunctionHandle
        LinuxSharedObjectImpl::dlsym(const char* symname)
        {
            return ::dlsym(m_OS_handle, symname);
        }

        /**
         * _add_ref
         */
        void
        LinuxSharedObjectImpl::_add_ref()
        {
            CDMW_MUTEX_GUARD(M_count_mutex);
            ++m_ref_count;

        }

        /**
         * _remove_ref
         */
        void
        LinuxSharedObjectImpl::_remove_ref()
        {
            bool del = false;
            
            {
                CDMW_MUTEX_GUARD(M_count_mutex);
                
                if (--m_ref_count == 0) {
                    del = true;
                }
            }

            if (del) {
                delete this;
            }
        }

    } // End namespace ossupport
} // End namespace Cdmw

#endif
