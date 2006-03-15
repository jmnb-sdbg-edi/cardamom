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


#ifndef INCL_ORBSUPPORT_OBJECT_TRAITS_HPP
#define INCL_ORBSUPPORT_OBJECT_TRAITS_HPP

#include "Foundation/orbsupport/CORBA.hpp"

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW OrbSupport types and data.
    */

    namespace OrbSupport
    {

        /**
        *Purpose:
        *<p>    Traits class for CORBA objects.
        *
        */

        template <typename T>

        struct object_traits
        {
            typedef typename T::_ptr_type _ptr_type;
            typedef typename T::_var_type _var_type;

            static _ptr_type _duplicate(_ptr_type obj)
            {
                return T::_duplicate(obj);
            }

            static _ptr_type _narrow(CORBA::Object_ptr obj)
            {
                return T::_narrow(obj);
            }

            static _ptr_type _nil()
            {
                return T::_nil();
            }
        };

        /**
        *Purpose:
        *<p>    Instantiation of the traits class for CORBA::Object.
        * This is added because _ptr_type and _var_type are not defined for CORBA::Object.
        */
        template <>

        struct object_traits<CORBA::Object>
        {
            typedef CORBA::Object_ptr _ptr_type;
            typedef CORBA::Object_var _var_type;

            static _ptr_type _duplicate(_ptr_type obj)
            {
                return CORBA::Object::_duplicate(obj);
            }

            static _ptr_type _narrow(CORBA::Object_ptr obj)
            {
                return CORBA::Object::_duplicate(obj);
            }

            static _ptr_type _nil()
            {
                return CORBA::Object::_nil();
            }
        };

    } // End namespace OrbSupport
} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_OBJECT_TRAITS_HPP

