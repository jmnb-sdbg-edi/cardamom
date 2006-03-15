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


#ifndef INCL_CDMWINIT_INIT_UTILS_HPP 
#define INCL_CDMWINIT_INIT_UTILS_HPP


#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessDelegate.stub.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "ConfAndPlug/cdmwinit/ProcessControl.hpp"
#include "ConfAndPlug/cdmwinit/DefaultProcessControl.hpp"
#include "ConfAndPlug/cdmwinit/CdmwProcess_impl.hpp"
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"

#include <string>
#include <cstddef>
#include <vector>

namespace Cdmw 
{
namespace CdmwInit
{
/**
 * Purpose:
 * <p> This class provides helper functions to be used by the CDMW generated 
 * CdmwInterface.cpp file.
 *
 *@see CDMW_init
 *@see CDMW_cleanup
 */
class InitUtils
{
public:
    /**
     * Purpose:
     * <p> 
     * 
     *@param orb  A reference to the ORB pseudo-object
     *@param argc The number of arguments passed to the program
     *@param argv The list of parameters received of the command line	
     *@param proc
     *
     *@return
     *
     *@exception CORBA::SystemException
     */ 
    static 
    void 
    init_platform_interface(CORBA::ORB_ptr orb, 
			    int & argc, char** argv,
			    CdmwPlatformMngt::ProcessDelegate_ptr proc)
	throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);
	//throw (CORBA::SystemException);

    /**
     * Purpose:
     * <p> Retrieves the CDMW Application Agent's object reference from
     * Program command line arguments.
     * 
     *@param orb  A reference to the ORB pseudo-object
     *@param argc The number of arguments passed to the program
     *@param argv The list of parameters received of the command line	
     *
     *@return A reference to the CDMW Application Agent
     *
     *@exception CORBA::BAD_PARAM Invalid parameter. The following CDMW minor codes 
     * detail the reason: <UL><LI><B>BAD_PARAMApplicationAgentReferenceMissing</B>
     * when the Application Agent's reference is missing from the command line
     * arguments; and <LI><B>BAD_PARAMInvalidApplicationAgentReference</B> when the
     * input IOR is nil or does not denote an Application Agent.</UL>
     *@exception CORBA::NO_MEMORY OsSupport library could not allocate memory
     *@exception CORBA::INTERNAL  Internal Error within OsSupport library
     */ 
    //static
    //CdmwPlatformMngt::ApplicationAgent_ptr
    //get_cdmw_application_agent(CORBA::ORB_ptr orb, 
    //                            int & argc, char** argv)
    //    throw (CORBA::BAD_PARAM,
    //           CORBA::NO_MEMORY,
    //           CORBA::INTERNAL);

    /**
     * Purpose:
     * <p> Returns the reference of the RootPOA pseudo object.
     * 
     *@param orb  A reference to the ORB pseudo-object
     *
     *@return A reference to the RootPOA pseudo object
     *
     *@exception CORBA::SystemException Any CORBA system exception
     */ 
    static 
    PortableServer::POA_ptr
    get_root_POA(CORBA::ORB_ptr orb)
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);
	//throw (CORBA::SystemException);

    /**
     * Purpose:
     * <p> Returns the reference of the CDMW upper most POA, parent of all
     * CDMW-specific POAs.
     * 
     *@param orb  A reference to the ORB pseudo-object
     *
     *@return A reference to the CDMW RootPOA pseudo object
     *
     *@exception CORBA::SystemException Any CORBA system exception
     */ 
    static 
    PortableServer::POA_ptr
    create_cdmw_root_POA(CORBA::ORB_ptr orb)
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper);
	//throw (CORBA::SystemException);

    /**
     * Purpose:
     * <p> Gets the CDMW Naming and Repository's object reference from the CDMW
     * Platform management.
     * 
     *
     *@return A reference to the CDMW Naming and Repository
     *
     * TODO: Update minor codes
     *@exception CORBA::BAD_PARAM Invalid parameter. The following CDMW minor codes 
     *details tha reason: <UL><LI><B>BAD_PARAMRepositoryNotFound</B> when there 
     *is no CDMW Naming and Repository registered for the current process within the
     *Application Agent; <LI><B>BAD_PARAMUnknownProcessName</B> when the process name
     *is not known by the Application Agent; <LI><B>BAD_PARAMCannotGetRepositoryReference</B>
     *when an error occurred during retrieval of the object reference from the Application
     *Agent; and <LI><B>BAD_PARAMInvalidRepositoryReference</B> when the object reference
     *received from the Application Agent is either nil or does not denote an CDMW Naming
     *and Repository service.</UL>
     */ 
    static
    CdmwNamingAndRepository::Repository_ptr
    get_cdmw_repository()
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
               Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER>);
//         throw (CORBA::BAD_PARAM,
// 	       CORBA::BAD_INV_ORDER);
        
    /**
     * Purpose:
     * <p> Returns the process name as allocated by the CDMW platform management.
     *
     *@return The name of current CDMW Process
     *
     *@exception CORBA::BAD_INV_ORDER TODO: Add minor codes
     */ 
    static
    std::string 
    get_cdmw_process_name()
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER>);
//         throw (CORBA::BAD_INV_ORDER);

    /**
     * Purpose:
     * <p> Returns the name of the application owning the process.
     *
     *@return The name of CDMW application
     *
     *@exception CORBA::BAD_INV_ORDER TODO: Add minor codes
     */ 
    static
    std::string
    get_cdmw_application_name()
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER>);
//         throw (CORBA::BAD_INV_ORDER);

    /**
     * Purpose:
     * <p> Returns the name of the application owning the process.
     *
     *@return The name of CDMW application
     *
     *@exception CORBA::BAD_PARAM TODO: Add minor codes
     *@exception CORBA::INTERNAL TODO: Add minor codes
     *@exception CORBA::BAD_INV_ORDER TODO: Add minor codes
     */ 
    static
    CORBA::Object_ptr
    InitUtils::get_service(const char* service_name)
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
               Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
               Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER> );

               
    /**
     *
     */
    static
    void init_xml_library()
        throw(Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>);         
               

    /**
     *
     */
    static
    void cleanup_xml_library()               
        throw(Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>);         
       
               
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
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
               Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
               Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>);
//         throw (CORBA::BAD_PARAM,
//                CORBA::NO_MEMORY,
//                CORBA::INTERNAL);




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
        throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
               Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>);
//         throw (CORBA::NO_MEMORY,
//                CORBA::INTERNAL);


}; // End class InitUtils



}; // End namespace CdmwInit
}; // End namespace Cdmw 

#endif // INCL_CDMWINIT_INIT_UTILS_HPP


