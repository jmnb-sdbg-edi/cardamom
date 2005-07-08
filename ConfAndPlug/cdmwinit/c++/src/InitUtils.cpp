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


#include <cstdlib>
#include <string>
#include <sstream>

#include "ConfAndPlug/cdmwinit/InitUtils.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"

#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/naminginterface/NamingUtil.hpp"
#include "Foundation/orbsupport/POAThreadingPolicy.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"


namespace
{    
    /**
     * Name of the CDMW POA root of all CDMW-specific POAs
     */
    // static const char* CDMW_ROOT_POA_NAME="CDMW_RootPOA";

    /**
     * Default Thread pool size of the CDMW root POA
     */
    const size_t CDMW_ROOT_POA_THREAD_POOL_SIZE = 5;

}; // End anonymous namespace

namespace  Cdmw
{

namespace CdmwInit
{

void
InitUtils::init_platform_interface(CORBA::ORB_ptr orb, 
				   int & argc, char** argv,
				   CdmwPlatformMngt::Process_ptr proc)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    
    using namespace Cdmw::PlatformMngt;
    using namespace Cdmw::OrbSupport;
    try {
	PlatformInterface::setup(orb,argc,argv);
	
	if (CORBA::is_nil(proc))
            CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(BAD_PARAMNilObjectReference,CORBA::COMPLETED_NO) );
	
	// Acknowledge the creation of the process
	PlatformInterface::acknowledgeCreation(proc);
    } catch (const CORBA::SystemException & ex) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
    } catch (const Cdmw::BadOrderException & ex) {
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER>,
                      CORBA::BAD_INV_ORDER(BAD_INV_ORDERInitAlreadyDone,CORBA::COMPLETED_NO),
                      ex.what() );
    } catch (const Cdmw::BadParameterException & ex) {
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMProcessCallbackReferenceMissing,CORBA::COMPLETED_NO),
                      ex.what() );
    }
}


PortableServer::POA_ptr
InitUtils::get_root_POA(CORBA::ORB_ptr orb)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    
    PortableServer::POA_var root = PortableServer::POA::_nil();
    try {  
        //
        // Resolve the Root POA
        //            
        CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
        root = PortableServer::POA::_narrow(obj.in());
    } catch (const CORBA::ORB::InvalidName & ) {
        // Raised by resolve_initial_references("RootPOA") (shouldn't happen)
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                              CORBA::COMPLETED_NO) );
    } catch (const CORBA::SystemException & ex) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
    }
    return root._retn();
}


PortableServer::POA_ptr
InitUtils::create_cdmw_root_POA(CORBA::ORB_ptr orb)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
    //
    // Create the CDMW root POA with its own POA manager.
    // Use Default POA policies:
    //
    // Lifespan policy            = TRANSIENT
    // Id Assignment policy       = SYSTEM_ID
    // Id Uniqueness policy       = UNIQUE_ID
    // Implicit Activation policy = IMPLICIT_ACTIVATION
    // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY
    // Servant Retention policy   = RETAIN;
    // Thread Policy              = ORB_CTRL_MODEL;
    //

    using namespace PortableServer;

    // Resolve the Root POA. May raise a CORBA::SystemException.
    POA_var root = get_root_POA(orb);

    CORBA::PolicyList policies;
    policies.length(7);
    policies[0] = root -> create_id_assignment_policy       (SYSTEM_ID);
    policies[1] = root -> create_lifespan_policy            (TRANSIENT);
    policies[2] = root -> create_servant_retention_policy   (RETAIN);
    policies[3] = root -> create_id_uniqueness_policy       (UNIQUE_ID);
    policies[4] = root -> create_request_processing_policy  (USE_ACTIVE_OBJECT_MAP_ONLY);
    policies[5] = root -> create_implicit_activation_policy (IMPLICIT_ACTIVATION);
    policies[6] = root -> create_thread_policy              (ORB_CTRL_MODEL);

    Cdmw::OrbSupport::StrategyList poaStrategies;
#if CDMW_ORB_VDR == tao
    Cdmw::OrbSupport::ThreadPerConnectionPolicy threading;
#else
    Cdmw::OrbSupport::ThreadPoolPolicy threading(CDMW_ROOT_POA_THREAD_POOL_SIZE);
#endif
    threading.append_to_StrategyList(poaStrategies);

    POA_var CDMW_rootPOA = POA::_nil();

    try {
        CDMW_rootPOA 
            = Cdmw::OrbSupport::OrbSupport::create_POA(root.in(),
                                                        OrbSupport::CDMW_ROOT_POA_NAME,
                                                        POAManager::_nil(),
                                                        policies,
                                                        poaStrategies);  

        
    } catch (const POA::AdapterAlreadyExists & ) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMCdmwRootPOAExists,
                                       CORBA::COMPLETED_NO) );
    } catch (const POA::InvalidPolicy & ) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                      CORBA::COMPLETED_NO) );
    } catch (const CORBA::SystemException & ex) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
        // TODO:?
    }

    return CDMW_rootPOA._retn();    
}


CdmwNamingAndRepository::Repository_ptr
InitUtils::get_cdmw_repository()
    throw ( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
            Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER> )
{

    using namespace Cdmw::PlatformMngt;
    using namespace Cdmw::OrbSupport;

    // Retreive Repository object reference from the platform management
    CdmwNamingAndRepository::Repository_var rep
        = CdmwNamingAndRepository::Repository::_nil();
    
    try {
	CORBA::Object_var obj
	    = PlatformInterface::getService(ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
	rep = CdmwNamingAndRepository::Repository::_narrow(obj.in());
    } catch (const CdmwPlatformMngtService::ServiceNotFound &) {
        // Cannot Proceed with no CDMW Repository Ior
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMRepositoryNotFound,CORBA::COMPLETED_NO) );
    } catch (const CdmwPlatformMngtService::ServiceUnavailable &) {
        // Cannot Proceed with no CDMW Repository Ior
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMCannotGetRepositoryReference,
                                       CORBA::COMPLETED_NO) );
    } catch (const CORBA::SystemException & ex) {
        std::string s("Received :");
        s += Cdmw::OrbSupport::OrbSupport::exception_to_string(ex);
        // Cannot Proceed with no CDMW Repository Ior
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMCannotGetRepositoryReference,
                                       CORBA::COMPLETED_NO) ,
                      s);
    } catch (const Cdmw::BadOrderException & ex) {
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER>,
                      CORBA::BAD_INV_ORDER(BAD_INV_ORDERInitNotCalled,CORBA::COMPLETED_NO),
                      ex.what());
    } catch (const Cdmw::BadParameterException & ex) {
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAM,CORBA::COMPLETED_NO),
                      ex.what());
    }
    if (CORBA::is_nil(rep.in()))
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMInvalidRepositoryReference,CORBA::COMPLETED_NO) );
    return rep._retn();
}

std::string 
InitUtils::get_cdmw_process_name()
    throw ( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER> )
{
    using namespace Cdmw::PlatformMngt;
    using namespace Cdmw::OrbSupport;
    try {
	std::string processName = PlatformInterface::getProcessName();
	return processName;
    } catch (const BadOrderException & ex) {
        std::string s("Cannot get the CDMW process name :");
        s += ex.what();
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER>,
                      CORBA::BAD_INV_ORDER(BAD_INV_ORDERAckNotDone,CORBA::COMPLETED_NO),
                      s );
    }
}


std::string 
InitUtils::get_cdmw_application_name()
    throw ( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER> )
{
    using namespace Cdmw::PlatformMngt;
    using namespace Cdmw::OrbSupport;
    try {
	std::string appliName = PlatformInterface::getApplicationName();
	return appliName;
    } catch (const BadOrderException & ex) {
        std::string s("Cannot get the CDMW application name :");
        s += ex.what();
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER>,
                      CORBA::BAD_INV_ORDER(BAD_INV_ORDERAckNotDone,CORBA::COMPLETED_NO) ,
                      s );
    }
}

CORBA::Object_ptr
InitUtils::get_service(const char* service_name)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
           Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
           Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER> )
{
    using namespace Cdmw::PlatformMngt;
    using namespace Cdmw::OrbSupport;
    try {
	CORBA::Object_var service = PlatformInterface::getService(service_name);
	return service._retn();
    } catch (const BadOrderException & ex) {
        std::string s("Cannot get the CDMW service : ");
        s += service_name;
        s += " ";
        s += ex.what();
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_INV_ORDER>,
                      CORBA::BAD_INV_ORDER(BAD_INV_ORDERAckNotDone,CORBA::COMPLETED_NO) ,
                      s );
    }
    catch (const BadParameterException & ex) {
        std::string s("Cannot get the CDMW service : ");
        s += service_name;
        s += " ";
        s += ex.what();
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMInvalidServiceName,CORBA::COMPLETED_NO) ,
                      s );
    }
    catch (const CdmwPlatformMngtService::ServiceNotFound& )
    {
        std::string s("Cannot get the CDMW service : ");
        s += service_name;
        s += " => Service not found!";
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMInvalidServiceName,CORBA::COMPLETED_NO),
                      s );
        
    }
    catch (const CdmwPlatformMngtService::ServiceUnavailable& )
    {
        std::string s("Cannot get the CDMW service : ");
        s += service_name;
        s += " => Service not found!";
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMInvalidServiceName,CORBA::COMPLETED_NO),
                      s );
        
    }
    catch (const CORBA::SystemException& ex)
    {
        std::string s("Cannot get the CDMW service : ");
        s += service_name;
        s += " ";
        s += Cdmw::OrbSupport::OrbSupport::exception_to_string(ex);
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
            CORBA::COMPLETED_NO),
            s );
        
    }

}


std::string 
InitUtils::get_xml_initialisation_file(int & argc, char** argv)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
           Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
           Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>)
{
    // Get the XML file Name
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::Common;
    
    // OutOfMemoryException, InternalErrorException
    std::string file_name;
    try {
        file_name = OS::get_option_value(argc, argv, 
                                         Options::PROCESS_XML_FILE_OPTION);
    } catch (const Cdmw::OutOfMemoryException & ex) {
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                      CORBA::NO_MEMORY(NO_MEMORY,CORBA::COMPLETED_NO),
                      ex.what());
    } catch (const Cdmw::InternalErrorException & ex) {
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO),
                      ex.what());
    }
    
    if ((file_name == "no") ||
        (file_name == "yes")) {
        // Cannot Proceed with no file name
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMProcessXMLFileNameMissing,CORBA::COMPLETED_NO) );
    }
    // TODO: for tests only
//    std::cout << "XML Process FileName = " << file_name << std::endl;
    
    return file_name;
}



bool
InitUtils::get_xml_validation_option(int & argc, char** argv)
    throw ( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
            Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL> )
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
    } catch (const Cdmw::OutOfMemoryException & ex) {
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                      CORBA::NO_MEMORY(NO_MEMORY,CORBA::COMPLETED_NO),
                      ex.what());
    } catch (const Cdmw::InternalErrorException & ex) {
        CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO),
                      ex.what());
    }
     
    return validate;
}

}; // namespace CdmwInit
}; // namespace Cdmw


