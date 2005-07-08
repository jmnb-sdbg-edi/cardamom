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


#ifndef INCL_NAMINGANDREPOSITORY_ORBFACILITY_HPP
#define INCL_NAMINGANDREPOSITORY_ORBFACILITY_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/common/Exception.hpp"

#include <string>

namespace Cdmw
{
namespace NamingAndRepository
{


/**
*Purpose:
*<p>
*Facility for converting Object reference to string and vice et versa.
*<p>
*Features:
*<p>
*Thread safe.
*
*/
class ORBFacility
{

public:

    /**
    * Purpose:
    * <p>
    * Initializes the M_ORB members.
    *
    * @param orb The ORB
    * @exception AssertionFailedException if initialization has
    * already been performed
    */
    static void initialize(CORBA::ORB_ptr orb)
            throw (AssertionFailedException);


    /**
    * Purpose:
    * <p>
    * Returns the stringified reference of the specified CORBA object.
    */
    static std::string object_to_string(CORBA::Object_ptr object)
        throw (OutOfResourcesException, AssertionFailedException, InternalErrorException);


    /**
    * Purpose:
    * <p>
    * Returns the CORBA object corresponding to the specified
    * stringified reference.
    */
    static CORBA::Object_ptr string_to_object(const std::string& str)
        throw (OutOfResourcesException, AssertionFailedException, InternalErrorException);



    /**
    * Purpose:
    * <p>
    * Activates the specified servant with the given id and
    * returns the corresponding CORBA object.
    *
    * @param id      The identifier of the servant
    * @param servant The servant to activate
    * @return        The reference to the CORBA object
    * @exception AssertionFailedException if initialization has not been performed
    */
    template <class CO, class S> static CO createActivatedReference(
            const std::string& id, S *servant)
            throw (AssertionFailedException, InternalErrorException)
    {

        PortableServer::POA_var poa = servant->_default_POA();
    
        if (CORBA::is_nil(poa.in()))
        {
            CDMW_THROW(AssertionFailedException);
        }

        try
        {
            PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(id.c_str());
            
            poa->activate_object_with_id(oid.in(), servant);

            return servant->_this();

        }
        catch(...)
        {
            CDMW_THROW(InternalErrorException);
        }
    
    }


private:

    /**
    * The ORB.
    */
    static CORBA::ORB_ptr M_ORB;

}; //End of NamingUtilities

} // End of namespace NamingAndRepository
} // End of namespace Cdmw

#endif

