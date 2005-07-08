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


#ifndef INCL_NAMINGANDREPOSITORY_NAMINGUTILITIES_HPP
#define INCL_NAMINGANDREPOSITORY_NAMINGUTILITIES_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/CosNaming.stub.hpp"
#include "Foundation/common/Exception.hpp"

#include <string>

namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Purpose:
*<p>
*Utilities for extracting/converting CosNaming::Name and strings.
*<p>
*Features:
*<p>
*Thread safe.
*
*/
class NamingUtilities
{

public:

    /**
    * Purpose:
    * <p>
    * Returns the string form of the name.
    *
    * @param n The name
    * @return  The string form of the simple name or the empty string
    * if n is empty.
    */
    static std::string to_string(const CosNaming::Name& n)
        throw(std::bad_alloc);

    /**
    * Purpose:
    * <p>
    * Indicates whether a stringified name is valid and is a simple name.
    *
    * @param name The name to check
    * @return true if name is valid and is a simple name.
    */
    static bool isSimpleName(const char* sn, std::string& reason)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Returns the string form of a simple name.
    *
    * @param n The simple name
    * @return  The string form of the simple name
    * @exception CosNaming::NamingContext::InvalidName if 
    * n is not a simple name or if n is invalid
    */
    static std::string simple_name_to_string(const CosNaming::Name& n)
        throw(CosNaming::NamingContext::InvalidName, OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Returns a name containing only the first name component from
    * the specified name.
    *
    * @param n The name
    * @return  The new name (may be empty if n is empty)
    */
    static CosNaming::Name* nameFirstPart(const CosNaming::Name& n)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Returns the end part of the specified name
    * (copy of the initial name but without the first name component).
    *
    * @param n The name
    * @return  The new name (may be empty if the length of n is not > 1)
    */
    static CosNaming::Name* nameEndPart(const CosNaming::Name& n)
        throw (OutOfResourcesException);

    /**
    * Purpose:
    * <p>
    * Returns the stringified first part of the specified name
    * (the stringified first name component)
    *
    * @param name The name
    * @return The stringified first part (may be empty if n is empty)
    */
    static std::string stringifiedNameFirstPart(const CosNaming::Name& n)
        throw (OutOfResourcesException);
        
    /**
    * Purpose:
    * <p>
    * Returns the stringified end part of the specified name
    * (copy of the initial name but without the first name component)
    *
    * @param name The name
    * @return The stringified end part
    * (may be empty if the length of n is not > 1)
    */
    static std::string stringifiedNameEndPart(const CosNaming::Name& n)
        throw (OutOfResourcesException);


}; //End of NamingUtilities

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

