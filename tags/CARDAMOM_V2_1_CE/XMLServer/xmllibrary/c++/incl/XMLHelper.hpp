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


#ifndef INCL_XML_HELPER_HPP
#define INCL_XML_HELPER_HPP

#include "XMLServer/idllib/CdmwXML.stub.hpp"

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW XMLUtils types and data.
    */

    namespace XMLUtils
    {

        /**
        *Purpose:
        *<p>    A utility class for use by the LifeCycle Framewotk
        * for object creation and activation.
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        class XMLHelper
        {

            public:

                /**
                * Purpose:
                * <p> Converts a DOM string to a char array.
                *
                *@param string The DOM string to convert
                */
                static char* to_char_array(CdmwXML::DOMString* string);

                /**
                * Purpose:
                * <p> Converts a char array to a DOM string.
                *
                *@param string The char array to convert
                */
                static CdmwXML::DOMString& to_DOM_string(const char* string);

                /**
                * Purpose:
                * <p> Creates and installs valuetypes factories.
                *
                * @param orb A valid reference to an ORB pseudo-object
                */
                static void register_value_factories(CORBA::ORB_ptr orb);

        };


    }

    ; // End namespace XMLUtils
}

; // End namespace Cdmw

#endif // INCL_XML_HELPER_HPP

