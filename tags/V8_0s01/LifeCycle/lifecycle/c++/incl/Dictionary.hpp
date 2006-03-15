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


#ifndef INCL_LIFECYCLE_DICTIONARY_HPP
#define INCL_LIFECYCLE_DICTIONARY_HPP

#include <map>

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW LifeCycle Frameworks types and data.
    */

    namespace LifeCycle
    {

        /**
        *Purpose:
        *<p> This class provides a template for dictionaries of instances 
        * of specified types.
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */ 
        // Compliments to Philippe Grimaud

        template <typename T>

        class Dictionary :
                    public std::map<std::string, T>
        {

            public:

                inline Dictionary()
                        : std::map<std::string, T>()
                {}

                /**
                * This is an accessor to a static instance of the dictionary.
                *@return The instance of the dictionary.
                */
                static Dictionary & Get();
        };

        template <typename T>
        Dictionary<T> &
        Dictionary<T>::Get() // TODO exception specs
        {
            static Dictionary<T> d;
            return d;
        }


    }

    ; // End namespace LifeCycle
}

; // End namespace Cdmw

#endif // INCL_LIFECYCLE_DICTIONARY_HPP

