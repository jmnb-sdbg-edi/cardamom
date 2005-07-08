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


#ifndef INCL_COMMON_STRING_HPP
#define INCL_COMMON_STRING_HPP

#include <vector>
#include <string>

#include "Foundation/common/System.hpp"
#include "Foundation/common/Exception.hpp"

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for common types used by CDMW runtime.
    */

    namespace Common
    {

        /**
        *Purpose:
        *<p>    This class provides several helper tools
        *        for the manipulation of strings
        *
        *Features:
        *<p>    none
        *
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class String
        {

            public:

                /**
                * Purpose:
                * <p> Create a vector of string from one string.
                * The string is composed of token separated by a
                * whitespace. Each token is returned into a
                * separate string inserted into the vector.
                * 
                *@param strs a string whose token are separated by
                *    a whitespace (e.g. :"foo bar")
                *
                *@return the vector of strings extracted
                *
                *@exception OutOfMemoryException not enough memory to
                *    complete the operation
                *@exception InternalErrorException the library is 
                *    corrupted
                */
                static
                std::vector< std::string >
                to_strings( std::string strs )
                throw ( OutOfMemoryException,
                        InternalErrorException);



                /**
                * Purpose:
                * <p> Create an C array of C string from a
                * vector of strings
                * 
                *@param strs the vector of string to be transformed
                *
                *@return a char** whose caller is responsible to free
                *     using delete[], and delete[]
                *
                *@exception
                */
                static
                char**
                to_char_array( const std::vector< std::string >& strs )
                throw ( OutOfMemoryException,
                        InternalErrorException);


            protected:


            private:

                /**
                * Purpose:
                * <p> [Constructor description if necessary]
                * 
                *@param
                *@exception
                */
                String()
                throw();

                /**
                * Purpose:
                * <p>  [Copy constructor description if necessary]
                * 
                *@param
                *@exception
                */
                String(const String& rhs)
                throw();


                /**
                * Purpose:
                * <p> [ Destructor description if necessary]
                * 
                */
                virtual
                ~String()
                throw();


                /**
                * Purpose:
                * <p> [ Assignement operator description if necessary]
                * 
                *@param
                *@return
                *@exception
                */
                String&
                operator=(const String& rhs)
                throw();


        }

        ; // End class String



        /**
        * Purpose:
        * <p> Compare two strings
        * 
        *@param s1   the first std string to compare
        *@param pos1 the position in string s1 to start comparaison
        *@param n1   the number of chars in string s1 to compare
        *@param s2   the char pointer on second string
        *
        *@return =0 if string are identical
        *        <0 if min(n1, length of s1 - pos1) is < length of s2 (\0 excluded)
        *        >0 if min(n1, length of s1 - pos1) is > length of s2 (\0 excluded)
        */
        int string_compare (const std::string& s1,
                            size_t pos1,
                            size_t n1,
                            const char* s2);



    } // End namespace Common
} // End namespace Cdmw



#endif // INCL_COMMON_STRING_HPP

