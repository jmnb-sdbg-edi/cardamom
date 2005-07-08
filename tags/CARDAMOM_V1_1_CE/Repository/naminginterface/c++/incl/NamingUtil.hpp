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


#ifndef INCL_NAMINGANDREPOSITORY_NAMING_UTIL_HPP
#define INCL_NAMINGANDREPOSITORY_NAMING_UTIL_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/ObjectTraits.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"
#include "Foundation/common/TypeMismatchException.hpp"

#include <string>

namespace Cdmw
{

    namespace NamingAndRepository
    {
        /**
        *Purpose:
        *<p>    A utility class for use with the NamingInterface.
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */

        template <typename T>

        class NamingUtil
        {
                typedef Cdmw::OrbSupport::object_traits<T> OT;

            public:
                /**
                * An alias to a CORBA _ptr type of the object
                */
                typedef typename OT::_ptr_type _ptr_type;

                /**
                * An alias to a CORBA _var type of the object
                */
                typedef typename OT::_var_type _var_type;

                /**
                 * Purpose:
                 * <p> This retrieves the object bound to a name in a given context and
                 * narrows the object's reference to the desired type. The name must exactly
                 * match the bound name.
                 * 
                 *@param ni A NamingInterface object for use to resolve the name
                 *@param s Name of the binding. It is a stringified representation of CosNaming::Name
                 * as defined in the OMG's Interoperable Naming Service (INS) specification. It name can
                 * have multiple components; therefore, name resolution can traverse multiple contexts.
                 * 
                 *@return The object bound to s. The type of the object is not returned, so
                 * the caller is responsible for narrowing the object to the appropriate type.
                 *
                 *@exception CosNaming::NamingContext::NotFound Indicates the name does 
                 * not identify a binding. The <I>why</I> member explains the reason for 
                 * the exception - <I>missing_node</I> or <I>not_context</I>.
                 *@exception CosNaming::NamingContext::CannotProceed Indicates that the 
                 * implementation has given up for some reason. The caller, however, may be
                 * able to continue the operation at the returned naming context (<I>ctx</I> member).
                 * The <I>rest_of_name</I> member contains the remainder of the non-working name.
                 *@exception CosNaming::NamingContext::InvalidName Indicates the name is invalid.
                 * (The string does not represent a valid CosNaming::Name. Implementation of
                 * the internal naming context may place other restrictions on the name. An empty
                 * string for the name is invalid)
                 *@exception CORBA::SystemException Any CORBA system exception.
                 *@exception Common::TypeMismatchException Failed to narrow to target object type.
                 *@see resolve
                 */
                static _ptr_type
                resolve_name(NamingInterface & ni,
                             const std::string& s)
                throw(CosNaming::NamingContext::NotFound,
                      CosNaming::NamingContext::CannotProceed,
                      CosNaming::NamingContext::InvalidName,
                      CORBA::SystemException,
                      Cdmw::Common::TypeMismatchException)
                {
                    CORBA::Object_var obj = ni.resolve(s);

                    _var_type ref = OT::_narrow(obj.in());

                    if (!CORBA::is_nil(obj.in()) &&
                            (CORBA::is_nil(ref.in())))
                    {
                        CDMW_THROW(Cdmw::Common::TypeMismatchException);
                    }

                    return ref._retn();
                }
        };



    }

    ; // End namespace NamingAndRepository
}

; // End namespace Cdmw
#endif // INCL_NAMINGANDREPOSITORY_NAMING_UTIL_HPP






