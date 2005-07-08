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


#include "namingandrepository/ORBFacility.hpp"


namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Implementation notes : none
*<p>
*Portability issues :   none
*/


// Initialisation

CORBA::ORB_ptr ORBFacility::M_ORB = NULL;


void ORBFacility::initialize(CORBA::ORB_ptr orb)
    throw (AssertionFailedException)
{

    if (!CORBA::is_nil(M_ORB))
    {
        CDMW_THROW(AssertionFailedException);
    }

    M_ORB = CORBA::ORB::_duplicate(orb);

}


std::string ORBFacility::object_to_string(CORBA::Object_ptr object)
throw (OutOfResourcesException, AssertionFailedException, InternalErrorException)
{

    if (CORBA::is_nil(M_ORB))
    {
        CDMW_THROW(AssertionFailedException);
    }

    if (CORBA::is_nil(object))
    {
        CDMW_THROW(AssertionFailedException);
    }

    try
    {

        // get the stringified reference of the object
        CORBA::String_var str_object = M_ORB->object_to_string(object);
        std::string result = str_object.in();
        
        return result;

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const CORBA::SystemException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}


CORBA::Object_ptr ORBFacility::string_to_object(const std::string& str)
throw (OutOfResourcesException, AssertionFailedException, InternalErrorException)
{

    if (CORBA::is_nil(M_ORB))
    {
        CDMW_THROW(AssertionFailedException);
    }


    try
    {

        // get the stringified reference of the object
        CORBA::Object_var object = M_ORB->string_to_object(str.c_str());
        
        return object._retn();

    }
    catch(const std::bad_alloc &)
    {
        CDMW_THROW(OutOfResourcesException);
    }
    catch(const CORBA::SystemException &)
    {
        CDMW_THROW(InternalErrorException);
    }

}



} // End of namespace NamingAndRepository
} // End of namespace Cdmw

