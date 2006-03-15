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


#include "namingandrepository/NamingUtilities.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Implementation notes : none
*<p>
*Portability issues :   none
*/


namespace
{
    // TODO: [i18n]put strings in a message catalog
    const char* COMPOUND_NAME = "Compound name not allowed !";
}



std::string NamingUtilities::to_string(const CosNaming::Name& n)
    throw(std::bad_alloc)
{

    std::string str_name;

    try
    {
        // get the string form of the name
        str_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(n);
    }
    catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
    {
        // the empty string will be returned
    }

    return str_name;

}




bool NamingUtilities::isSimpleName(const char* sn, std::string& reason)
    throw (OutOfResourcesException)
{

    try
    {
        bool result = false;

        try
        {
            CosNaming::Name_var name = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(sn);
        
            if (name->length() == 1)
            {
                result = true;
            }
            else
            {
                reason = COMPOUND_NAME;
            }
        
        }
        catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &e)
        {
            // invalid name
            reason = e.what();
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}




std::string NamingUtilities::simple_name_to_string(const CosNaming::Name& n)
    throw(CosNaming::NamingContext::InvalidName, OutOfResourcesException)
{

    try
    {
        // get the string form of the name
        std::string str_name;

        if (n.length() > 1)
        {
            // the name is not a simple name
            throw CosNaming::NamingContext::InvalidName();
        }

        try
        {
            str_name = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(n);
        }
        catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
        {
            throw CosNaming::NamingContext::InvalidName();
        }

        if (str_name.compare(".") == 0)
        {
            // the name is empty !!
            throw CosNaming::NamingContext::InvalidName();
        }

        return str_name;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



CosNaming::Name* NamingUtilities::nameFirstPart(const CosNaming::Name& n)
    throw (OutOfResourcesException)
{

    try
    {
        CosNaming::Name_var result = new CosNaming::Name;

        CORBA::ULong len = n.length();

        if (len > 0)
        {

            result->length(1);
            (*result)[0] = n[0];
        }
	// FIXME handle length==0, throw exception

        return result._retn();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}


CosNaming::Name* NamingUtilities::nameEndPart(const CosNaming::Name& n)
    throw (OutOfResourcesException)
{

    try
    {
        CosNaming::Name_var result = new CosNaming::Name;

        CORBA::ULong len = n.length();

        if (len > 1)
        {

            result->length(len - 1);

            // copy the sequence
            for (CORBA::ULong i = 1; i < len; ++i)
            {

                (*result)[i-1] = n[i];

            }

        }

        return result._retn();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }

}



std::string NamingUtilities::stringifiedNameFirstPart(const CosNaming::Name& n)
    throw (OutOfResourcesException)
{

    try
    {

        std::string result;

        // create the first part
        CosNaming::Name_var firstPart = nameFirstPart(n);
        
        try
        {
            result = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(firstPart.in());
        
        }
        catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
        {
            // invalid name
            // => result remain empty
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
         
}




std::string NamingUtilities::stringifiedNameEndPart(const CosNaming::Name& n)
    throw (OutOfResourcesException)
{

    try
    {
        std::string result;

        // create the end part
        CosNaming::Name_var endPart = nameEndPart(n);
        
        try
        {
            result = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(endPart.in());
        
        }
        catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
        {
            // invalid name
            // => result remain empty
        }

        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
        
}



} // End of namespace NamingAndRepository
} // End of namespace Cdmw

