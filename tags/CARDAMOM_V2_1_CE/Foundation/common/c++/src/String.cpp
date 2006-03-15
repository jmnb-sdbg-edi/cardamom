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


#include "Foundation/common/System.hpp"

#include "Foundation/common/String.hpp"

#include <sstream>
#include <cstring>


namespace Cdmw
{

    namespace Common
    {





        //
        // Transform a string of token into a vector of strings
        //    each one containing a string
        //
        std::vector< std::string >
        String::to_strings( std::string strs )
        throw ( OutOfMemoryException,
                InternalErrorException)
        {
            try
            {

                // This stream contains the list of string
                //    separated by a whitespace
                std::istringstream strs_to_parse( strs );

                std::string str;
                std::vector< std::string > strs_vector;

                while ( strs_to_parse >> str )
                {
                    strs_vector.push_back( str );
                }

                return strs_vector;

            }
            catch ( const std::bad_alloc& )
            {

                CDMW_THROW(OutOfMemoryException);

            }
            catch ( ...)
            {

                CDMW_THROW(InternalErrorException);
            }
        }


        char**
        String::to_char_array( const std::vector< std::string >& strs )
        throw ( OutOfMemoryException,
                InternalErrorException)
        {
            char** ppstr;

            // We allocate enough place to contain each string
            //  and a NULL string to indicate the end
            ppstr = new char * [strs.size() + 1 ];

            // We copy each string into a C string

            for ( size_t iString = 0; iString < strs.size(); ++iString)
            {

                // Don't forget the NULL end character (+1)
                ppstr[iString] = new char [ strs[iString].size() + 1 ];

                strcpy( ppstr[iString], strs[iString].c_str() );
            }

            // We mark the end
            ppstr[strs.size()] = NULL;

            return ppstr;

        }


        /**
        * function for string compare.
        *   Added for stl compatibility
        */
        int string_compare (const std::string& s1,
                            size_t pos1,
                            size_t n1,
                            const char* s2)
        {
            int result;
            result = s1.compare(pos1, n1, s2);
            return result;
        }

    } // End namespace Common
} // End namespace Cdmw

