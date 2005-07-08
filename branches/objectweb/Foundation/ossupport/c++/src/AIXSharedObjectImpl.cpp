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


#ifdef AIX

#include "ossupport/AIXSharedObjectImpl.hpp"
#include "Foundation/ossupport/AIXSharedObject.hpp"
#include "ossupport/Which.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include <iostream>

namespace Cdmw
{

    namespace OsSupport
    {


        //
        // declaration of the static object
        //
        std::map<std::string, AIXSharedObjectImpl*> AIXSharedObjectImpl::M_mapDlLibraryStringToObject;

        Cdmw::OsSupport::Mutex AIXSharedObjectImpl::M_count_mutex;

        //
        // Constructor
        //
        AIXSharedObjectImpl::AIXSharedObjectImpl(std::string libraryName, OsShlibHandle OS_Handle )
                : m_dlopen_count(1), m_ref_count(0), m_libraryName(libraryName), m_OS_handle(OS_Handle)
        {
        }

        //
        // destructor
        //
        AIXSharedObjectImpl::~AIXSharedObjectImpl()
        {
        }

        //
        // dlerror
        //
        std::string AIXSharedObjectImpl::dlerror(void)
        {
            return ::dlerror();
        }

        //
        // print
        //
        void AIXSharedObjectImpl::print(std::ostream& os)
        {
            if (m_OS_handle != NULL)
                os << m_OS_handle;
        }

        //
        // is_null
        //
        bool AIXSharedObjectImpl::is_null()
        {
            return (m_OS_handle == NULL);
        }

        //
        // dlopen
        // the dlopen returns a new AIXSharedObjectImpl object if it is the first call for the library
        // else it returns the AIXSharedObjectImpl object stored in a map and which refer to the library
        //
        AIXSharedObjectImpl* AIXSharedObjectImpl::dlopen(const char* libname, int flag)
        {

            std::string cmd = libname;

            AIXSharedObjectImpl::OsShlibHandle temp = NULL;

            AIXSharedObjectImpl* pimpl = NULL;

            // the libname under AIX is define as "libc.a(shr.o)" (the library name plus the object file)
            // to find the name in the LIBPATH, we cut the (shr.o)

            if (cmd.find("(") != -1)
                cmd = cmd.erase(cmd.find("("));

            std::string result;

            struct stat finfo;

            // take the LIBPATH environment variable
            std::string path_list = getenv("LIBPATH");

            if (((path_list != "") && (path_list[0] != '\0')) || (stat (cmd.c_str(), &finfo) >= 0))
            {
                unsigned int path_index = 0;
                // find_command_in_path return the full path of the library
                result = find_command_in_path(cmd, path_list, &path_index, "LIB");
                // if the library is found

                if (result != "")
                {
                    // find the library in the map
                    SharedObjectsMap::iterator it = M_mapDlLibraryStringToObject.find(cmd);
                    CDMW_MUTEX_GUARD(AIXSharedObjectImpl::M_count_mutex);
                    // if the library isn't in the map
                    // we run the dlopen, return the value (if NULL, the dlopen is bad), save the library name and the handle into the map

                    if ( it == M_mapDlLibraryStringToObject.end())
                    {
                        temp = ::dlopen(libname, flag);

                        if (temp == NULL)
                            return NULL ;

                        pimpl = new AIXSharedObjectImpl(cmd, temp);

                        M_mapDlLibraryStringToObject[cmd] = pimpl;
                    }

                    // if the library name is found in the map, we increase the reference counter
                    else
                    {
                        pimpl = (*it).second;
                        ++(pimpl->m_dlopen_count);
                    }
                }

                // if the library isn't found
                else
                {
                    // if the libname is NULL (dlopen must return a handle under the main process

                    if (libname == NULL)
                    {
                        // find if NULL is already open
                        SharedObjectsMap::iterator it = M_mapDlLibraryStringToObject.find(cmd);
                        CDMW_MUTEX_GUARD(AIXSharedObjectImpl::M_count_mutex);
                        // if the NULL reference are not already open

                        if (it == M_mapDlLibraryStringToObject.end())
                        {
                            std::cout << "fisrt" << std::endl;
                            temp = ::dlopen(libname, flag);
                            // store the NULL and the handle into the map
                            pimpl = new AIXSharedObjectImpl(libname, temp);
                            M_mapDlLibraryStringToObject[libname] = pimpl;
                        }

                        // increase the reference counter
                        else
                        {
                            std::cout << "second" << std::endl;
                            pimpl = (*it).second;
                            ++(pimpl->m_dlopen_count);
                            cmd = libname;
                        }
                    }

                    // if the library is not find or the libnameis not NULL, call the dlopen to store the error in the dlerror
                    else
                    {
                        temp = ::dlopen(libname, flag);
                    }
                }
            }

            if (pimpl != NULL)
                pimpl->_add_ref();

            return pimpl;

        }

        //
        // dlclose
        //
        int AIXSharedObjectImpl::dlclose(void)
        {
            CDMW_MUTEX_GUARD(AIXSharedObjectImpl::M_count_mutex);
            // decrease the reference counter

            if (m_dlopen_count != 0)
                --m_dlopen_count;

            // if the reference counter is equal to zero, delete the library reference into the map
            if (m_dlopen_count == 0)
            {

                AIXSharedObjectImpl::M_mapDlLibraryStringToObject.erase(m_libraryName);
                // return the dlclose
                return ::dlclose(m_OS_handle);

            }
            else
            {
                return 0;
            }


        }

        //
        // dlsym
        //
        AIXSharedObjectImpl::FunctionHandle AIXSharedObjectImpl::dlsym(const char* symname)
        {
            return ::dlsym(m_OS_handle, symname);
        }


        //
        // _add_ref
        //
        void AIXSharedObjectImpl::_add_ref()
        {
            CDMW_MUTEX_GUARD(M_count_mutex);
            ++m_ref_count;

        }

        //
        // _remove_ref
        //
        void AIXSharedObjectImpl::_remove_ref()
        {
            bool del = false;
            {
                CDMW_MUTEX_GUARD(M_count_mutex);

                if (--m_ref_count == 0)
                    del = true;
            }

            if (del)
                delete this;

        }



    } // End namespace ossupport

} // End namespace Cdmw


#endif

