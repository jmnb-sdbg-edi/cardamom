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


#ifndef INCL_EVENTADMIN_INITUTILS_HPP 
#define INCL_EVENTADMIN_INITUTILS_HPP 

#include <string>

#include "Foundation/orbsupport/CORBA.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"



namespace Cdmw
{
namespace EventAdmin
{

/**
*Purpose:
*<p> This class provides helper functions to init cdmw application
*
*
*@see   CDMW_init 
*
*/
class InitUtils 
{

    public:
        static const char* PROFILE_XML_FILE_OPTION;

    /**
     * Purpose:
     * <p> Retrieves the CDMW Process XML initialisation file name
     * from Program command line arguments.
     * 
     *@param orb  A reference to the ORB pseudo-object
     *@param argc The number of arguments passed to the program
     *@param argv The list of parameters received of the command line    
     *
     *@return The name of current CDMW Process XML initialisation file name
     *
     *@exception CORBA::BAD_PARAM Invalid parameter. The following CDMW minor codes 
     * details tha reason: <B>BAD_PARAMProcessXMLFileNameMissing</B> when the CDMW
     * Process XML initialisation file name is missing from the command line arguments
     *@exception CORBA::NO_MEMORY OsSupport library could not allocate memory
     *@exception CORBA::INTERNAL  Internal Error within OsSupport library
     */ 
    static
    std::string 
    get_xml_initialisation_file(int & argc, char** argv)
        throw (CORBA::BAD_PARAM,
               CORBA::NO_MEMORY,
               CORBA::INTERNAL);




    /**
     * Purpose:
     * <p> Retrieves the CDMW Process XML initialisation file validation option. It
     * instructs the XML parser whether to validate the XML document or not.
     * This will return <B>true</B> XML document willis validated whenever 
     * VALIDATE_XML_OPTION appears in the argument list.
     *
     *@param argc The number of arguments passed to the program
     *@param argv The list of parameters received of the command line    
     *
     *@return <B>true</B> if the XML document has to be validated by the XML Parser,
     * <B>false</B> otherwise.
     *
     *@exception CORBA::NO_MEMORY OsSupport library could not allocate memory
     *@exception CORBA::INTERNAL  Internal Error within OsSupport library
     */ 
    static
    bool
    get_xml_validation_option(int & argc, char** argv)
        throw (CORBA::NO_MEMORY,
               CORBA::INTERNAL);

        /**
        *[Attribute description]
        */ 


    protected:


    private:

}; // End class InitUtils 

}; // End namespace EventAdmin
}; // End namespace Cdmw
#endif // INCL_EVENTADMIN_INITUTILS_HPP


