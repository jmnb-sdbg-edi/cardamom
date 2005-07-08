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


#include "eventchannelmanager/InitUtils.hpp"

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Locations.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

// CDMW idl include
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"




namespace 
{
}; // end anonymous anamespace

namespace Cdmw
{
namespace EventAdmin
{
   const char* InitUtils::PROFILE_XML_FILE_OPTION="--ProfileXMLFile";

    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::Common;

std::string 
InitUtils::get_xml_initialisation_file(int & argc, char** argv)
    throw (CORBA::BAD_PARAM,
           CORBA::NO_MEMORY,
           CORBA::INTERNAL)
{
    // Get the XML file Name
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::Common;
    
    // OutOfMemoryException, InternalErrorException
    std::string file_name;
    try {
        file_name = OS::get_option_value(argc, argv, 
                                         PROFILE_XML_FILE_OPTION);
    } catch (const Cdmw::OutOfMemoryException & ) {
        throw CORBA::NO_MEMORY(NO_MEMORY,CORBA::COMPLETED_NO);
    } catch (const Cdmw::InternalErrorException & ) {
        throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
    }
    
    if ((file_name == "no") ||
        (file_name == "yes")) {
        // Cannot Proceed with no file name
        throw CORBA::BAD_PARAM(BAD_PARAMProcessXMLFileNameMissing,
                               CORBA::COMPLETED_NO);
    }
    
    return file_name;
}


bool
InitUtils::get_xml_validation_option(int & argc, char** argv)
    throw (CORBA::NO_MEMORY,
           CORBA::INTERNAL)
{
    // XML document is validated whenever VALIDATE_XML_OPTION appears in 
    // the argument list.
    bool validate = false;

    // Get the XML file Name
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::Common;
   
    try {
        std::string validateXML_option
            = OS::get_option_value( argc, argv, 
                                    Options::VALIDATE_XML_OPTION);
    
        if ( (validateXML_option == "yes") || 
             (validateXML_option != "no") ) {
            validate = true;
        }
    } catch (const Cdmw::OutOfMemoryException & ) {
        throw CORBA::NO_MEMORY(NO_MEMORY,CORBA::COMPLETED_NO);
    } catch (const Cdmw::InternalErrorException & ) {
        throw CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO);
    }
     
    return validate;
}


}; // End namespace EventAdmin
}; // End namespace Cdmw

