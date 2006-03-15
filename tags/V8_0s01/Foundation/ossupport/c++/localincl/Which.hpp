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


#ifndef INCL_OSSUPPORT_WHICH_HPP
#define INCL_OSSUPPORT_WHICH_HPP

#include <sys/stat.h>
#include <unistd.h>
#include <string>



#define FS_EXISTS  0x1
#define FS_EXECABLE  0x2
#define u_mode_bits(x) (((x) & 0000700) >> 6)
#define g_mode_bits(x) (((x) & 0000070) >> 3)
#define o_mode_bits(x) (((x) & 0000007) >> 0)
#define X_BIT(x) ((x) & 1)

namespace Cdmw
{

    namespace OsSupport
    {

        /**
        * Purpose:
        * <p> chech if the command existe and if it executable
        * 
        * @param : the name of the command
        *
        * @return : 1 if the command existe and if ///////////////a completer
        * @exception 
        */

        int file_status(const std::string& name);


        /**
        * Purpose:
        * <p> built the full command path
        * 
        * @param : the name of the command and the possible path of the command 
        *
        * @return : the full command path
        * @exception 
        */

        std::string make_full_pathname(const std::string& path, const std::string& name);


        /**
        * Purpose:
        * <p> find the next path in the full path 
        * 
        * @param : the full path and a index pointer on this full path
        *
        * @return : the next path
        * @exception 
        */

        std::string get_next_path_element(const std::string& path_list, int *path_index_pointer);


        /**
        * Purpose:
        * <p> find the next path in the full path 
        * 
        * @param : the command name, the full path and a index pointer on this full path
        *
        * @return : the command path or "" if the command path doesn't existe
        * @exception 
        */

        std::string find_command_in_path( std::string& name, std::string& path_list, unsigned int *path_index, std::string property = "EXE");


        /**
        * Purpose:
        * <p> main function
        * 
        * @param : the name of the function
        *
        * @return : 1 if the path is find, if not 0
        * @exception 
        */

        int path_search(const std::string& cmd);



    } // End namespace ossupport

} // End namespace Cdmw

#endif // INCL_OSSUPPORT_WHICH_HPP

