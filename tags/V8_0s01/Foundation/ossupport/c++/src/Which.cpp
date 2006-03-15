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


#include "ossupport/Which.hpp"
#include <iostream>

namespace Cdmw
{

    namespace OsSupport
    {

        /* Return some flags based on information about this file.
           The EXISTS bit is non-zero if the file is found.
           The EXECABLE bit is non-zero the file is executble.
           Zero is returned if the file is not found. */
        int file_status (const std::string& name)
        {

            struct stat finfo;
            static int user_id = -1;

            /* Determine whether this file exists or not. */

            if (stat (name.c_str(), &finfo) < 0)
                return (0);

            /* If the file is a directory, then it is not "executable" in the
               sense of the shell. */
            if (S_ISDIR (finfo.st_mode))
                return (FS_EXISTS);


            /* Find out if the file is actually executable.  By definition, the
               only other criteria is that the file has an execute bit set that
               we can use. */
            if (user_id == -1)
                user_id = geteuid (); /* CHANGED: bash uses: current_user.euid; */

            /* Root only requires execute permission for any of owner, group or
               others to be able to exec a file. */
            if (user_id == 0)
            {
                int bits;

                bits = (u_mode_bits (finfo.st_mode) |
                        g_mode_bits (finfo.st_mode) |
                        o_mode_bits (finfo.st_mode));

                if (X_BIT (bits))
                    return (FS_EXISTS | FS_EXECABLE);
            }

            /* If we are the owner of the file, the owner execute bit applies. */

#if defined(linux)
            unsigned int user_ident = (unsigned int) user_id;

#else

            int user_ident = user_id;

#endif

            if (user_ident == finfo.st_uid && X_BIT (u_mode_bits (finfo.st_mode)))
                return (FS_EXISTS | FS_EXECABLE);

            /* If we are in the owning group, the group permissions apply. */
            /* problem with the function group_member */
            //  if (group_member (finfo.st_gid) && X_BIT (g_mode_bits (finfo.st_mode)))
            //    return (FS_EXISTS | FS_EXECABLE);

            /* If `others' have execute permission to the file, then so do we,
               since we are also `others'. */
            if (X_BIT (o_mode_bits (finfo.st_mode)))
                return (FS_EXISTS | FS_EXECABLE);
            else
                return (FS_EXISTS);

        }


        /* Turn PATH, a directory, and NAME, a filename, into a full pathname and returns it. */
        std::string make_full_pathname (const std::string& path, const std::string& name)
        {
            int path_len;

            path_len = path.size();
            std::string full_path;
            full_path = path;
            //  if (full_path[path_len-1]!='/')
            full_path += '/';
            full_path += name;
            return (full_path);
        }


        /* Return the next element of PATH_LIST or "" if there are no more. */
        std::string get_next_path_element (const std::string& path_list, unsigned int *path_index_pointer)
        {

            std::string path;

            unsigned int i, start;

            i = *path_index_pointer;

            if (i >= path_list.size())
                return ("");

            if (i && path_list[i] == ':')
                i++;

            start = i;

            while (path_list[i] && path_list[i] != ':')
                i++;

            *path_index_pointer = i;

            if (i == start)
            {
                if (path_list[i])
                    (*path_index_pointer)++;

                return ("");
            }
            else
            {
                std::string value;
                value = path_list.substr(start, i - start);

                value [i - start] = '\0';
                return (value);
            }

            return (path);
        }





        /* this function scan all the path and search the command name in all the path possible*/
        std::string find_command_in_path( std::string& name, std::string& path_list, unsigned int *path_index, std::string property )
        {
            int absolute_path_given;
            std::string abs_path;
            std::string found, full_path;
            int status, name_len;

            name_len = name.size();

            if (name.find('/') > name.size())
            {
                absolute_path_given = 0;
            }
            else
            {
                int p;
                absolute_path_given = 1;
                abs_path = "";

                if (name[0] != '.' && name[0] != '/' && name[0] != '~')
                {
                    abs_path = "./";
                    abs_path += name;
                }
                else
                {
                    abs_path = name;
                }


                p = abs_path.find_last_of ('/');
                name = abs_path.substr(p + 1, abs_path.size());
                path_list = abs_path.substr(0, p);
            }


            while ((path_list != "") && (*path_index < path_list.size()))
            {
                std::string path;

                if (absolute_path_given)
                {
                    path = path_list;
                    *path_index = path.size();
                }
                else
                    path = get_next_path_element(path_list, path_index);



                if ((path == "") && (*path_index == path_list.size()))
                    break;

                full_path = make_full_pathname(path, name);

                status = file_status(full_path);

                if (property == "EXE")
                {
                    if ((status & FS_EXISTS) && (status & FS_EXECABLE))
                    {
                        found = full_path;
                        break;
                    }
                }
                else
                {
                    if ((status & FS_EXISTS))
                    {
                        found = full_path;
                        break;
                    }
                }
            }

            return (found);
        }




        /* This function take the command name and return 1 if the command are found in the path,
        if not 0. */

        int path_search( const std::string& cmd)
        {
            std::string cmd2 = cmd;
            std::string result;
            int found_something = 0;

            try
            {
                // get PATH content
                std::string path_list = getenv("PATH");

                if ((path_list != "") && (path_list[0] != '\0') && (cmd != ""))
                {
                    unsigned int path_index = 0;
                    result = find_command_in_path(cmd2, path_list, &path_index);

                    if (result != "")
                    {
                        //std::cout<<"process used :"<<result<<std::endl;
                        found_something = 1;
                    }
                }

            }
            catch ( ...)
            {
                std::cerr << "error" << std::endl;
            }

            return found_something;
        }

    } // End namespace ossupport

} // End namespace Cdmw

