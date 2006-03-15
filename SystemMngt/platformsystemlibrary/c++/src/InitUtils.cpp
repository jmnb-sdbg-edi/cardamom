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

#include <string>

#include "platformsystemlibrary/InitUtils.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/orbsupport/POAThreadingPolicy.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Assert.hpp"


namespace Cdmw {        // Begin namespace Cdmw
namespace PlatformMngt { // Begin namespace PlatformMngt


PortableServer::POA_ptr
InitUtils::Get_root_POA(CORBA::ORB_ptr orb)
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
InitUtils::Create_cdmw_root_POA(CORBA::ORB_ptr orb)
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
    POA_var root = Get_root_POA(orb);

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


std::string 
InitUtils::Get_xml_initialisation_file(int & argc, char** argv)
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
        CDMW_THROW2(CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                    CORBA::NO_MEMORY(NO_MEMORY,CORBA::COMPLETED_NO),
                    ex.what());
    } catch (const Cdmw::InternalErrorException & ex) {
        CDMW_THROW2(CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                    CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO),
                    ex.what());
    }
    
    if ((file_name == "no") ||
        (file_name == "yes")) 
    {
        std::cerr << "Cannot Proceed with no file name for the option "  
                  << Options::PROCESS_XML_FILE_OPTION
                  << std::endl;
        // Cannot Proceed with no file name
        CDMW_THROW1(CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                    CORBA::BAD_PARAM(BAD_PARAMProcessXMLFileNameMissing,
                                     CORBA::COMPLETED_NO) );
    }
    
    return file_name;
}



bool
InitUtils::Get_xml_validation_option(int & argc, char** argv)
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
        CDMW_THROW2(CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                    CORBA::NO_MEMORY(NO_MEMORY,CORBA::COMPLETED_NO),
                    ex.what());
    } catch (const Cdmw::InternalErrorException & ex) {
        CDMW_THROW2(CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                    CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO),
                    ex.what());
    }
     
    return validate;
}

void
InitUtils::Get_parameters(CdmwInitConfiguration& xml_data,
                          std::string&  multicast_port,
                          std::string&  multicast_domain,
                          std::string&  multicast_IP_address,
                          int&          max_transaction_in_progress,
                          int&          max_transaction_done,
                          Cdmw::OsSupport::OS::Timeval& cohort_timeout,
                          Cdmw::OsSupport::OS::Timeval& coordinator_timeout,
                          Cdmw::OsSupport::OS::Timeval& freeze_timeout,
                          long&         number_of_scheduler_thread,
                          long&         chunk_size,
                          long&         freeze_size)
    throw (BadParameterException, XMLErrorException, OutOfMemoryException)
{
    try {
        std::string res;
        unsigned long res_long;       
        res = xml_data.getDataStoresAttributeValue("max-transaction-in-progress"); 
        max_transaction_in_progress = atoi(res.c_str());
    
        res = xml_data.getDataStoresAttributeValue("max-transaction-done"); 

        max_transaction_done = atoi(res.c_str());
    
        res = xml_data.getDataStoresAttributeValue("cohort-timeout");
        res_long = atoi(res.c_str());                 
        cohort_timeout.seconds = res_long/1000;
        cohort_timeout.microseconds = (res_long%1000)*1000;
    
        res = xml_data.getDataStoresAttributeValue("coordinator-timeout");                 
        res_long = atoi(res.c_str());                 
        coordinator_timeout.seconds = res_long/1000;
        coordinator_timeout.microseconds = (res_long%1000)*1000;
    
        res = xml_data.getDataStoresAttributeValue("number-of-scheduler-thread");
        number_of_scheduler_thread = atoi(res.c_str());
    
        res = xml_data.getDataStoresAttributeValue("chunk-size");
        chunk_size = atoi(res.c_str());
    
        res = xml_data.getDataStoresAttributeValue("freeze-size");
        freeze_size = atoi(res.c_str());
    
        res = xml_data.getDataStoresAttributeValue("freeze-timeout");
        res_long = atoi(res.c_str());                 
        freeze_timeout.seconds = res_long/1000;
        freeze_timeout.microseconds = (res_long%1000)*1000;
    
    } catch (const Cdmw::BadParameterException& ex) {
        throw;
    }
}
   
void
InitUtils::Get_datastore_parameters(CdmwInitConfiguration& xml_data,
                                    const char*   datastore_name,
                                    int           datastoreInstance,
                                    CORBA::ULong& datastore_id)
    throw (BadParameterException, XMLErrorException, OutOfMemoryException)
{
    try {
        std::string res;

        res = xml_data.getDataStoreAttributeValue(datastore_name,
                                                  1,
                                                  "datastore-id");
        datastore_id = atoi(res.c_str());
    } catch (const Cdmw::BadParameterException& ex) {
        throw;
    }
}
   
} // End namespace PlatformMngt
} // End namespace Cdmw

