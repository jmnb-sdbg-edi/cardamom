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


#include <CCMContainer/ccmcomponentserver/InitUtils.hpp>
#include <CCMContainer/ccmcomponentserver/ComponentServer_impl.hpp>
#include <CCMContainer/ccmcomponentserver/StandardHomeRegistration.hpp>
#include <CCMContainer/ccmcomponentserver/DefaultHomeRegistration.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

namespace
{    

    /**
     * Name of the POA root of CDMW ComponentServer
     */
    static const char*  COMPONENT_SERVER_POA_NAME = "ComponentServerPOA";


    bool get_home_registration_option(int & argc, char** argv)
        throw ( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL> )
    {
        bool def_home_registration = false;
        
        using namespace Cdmw::OsSupport;
        using namespace Cdmw::OrbSupport;
        using namespace Cdmw::Common;
        
        try {
            std::string home_reg_option
                = OS::get_option_value( argc, argv, 
                                        Options::DEFAULT_HOME_REGISTRATION_OPTION);
            
            if ( (home_reg_option == "yes") || 
                 (home_reg_option != "no") ) {
                def_home_registration = true;
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
        
        return def_home_registration;
    }


}; // End anonymous namespace

namespace Cdmw {

namespace CCM {

namespace ComponentServer {


CdmwCcmComponentServer::ComponentServer_ptr
InitUtils::create_component_server(CORBA::ORB_ptr                      orb,
                                   PortableServer::POA_ptr             parent,
                                   CdmwEvent::EventChannelFactory_ptr event_channel_factory,
                                   const std::string &                 process_name,
                                   const std::string &                 application_name,
                                   int & argc, char** argv)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
    
    try {
        CdmwCcmComponentServer::ComponentServer_var the_component_server =
            CdmwCcmComponentServer::ComponentServer::_nil();
    
        // A) Create a set of ConfigValues.
        Components::ConfigValues cfg_compserver(0);
        cfg_compserver.length(0L);
        
        // B) Create A ComponentServerPOA (with Retain and Multi-thread policicies)
        // to be used to activate the ComponentServer and Container objects.
        // Lifespan policy            = TRANSIENT
        // Id Assignment policy       = SYSTEM_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY
        // Servant Retention policy   = RETAIN;
        // Thread Policy              = ORB_CTRL_MODEL
        //        
        using namespace PortableServer;
        PortableServer::POAManager_var poaManager = parent -> the_POAManager();
        CORBA::PolicyList policies;
        policies.length(7);
        policies[0] = parent -> create_id_assignment_policy       (SYSTEM_ID);
        policies[1] = parent -> create_lifespan_policy            (TRANSIENT);
        policies[2] = parent -> create_servant_retention_policy   (RETAIN);
        policies[3] = parent -> create_id_uniqueness_policy       (UNIQUE_ID);
        policies[4] = parent -> create_request_processing_policy  (USE_ACTIVE_OBJECT_MAP_ONLY);
        policies[5] = parent -> create_implicit_activation_policy (IMPLICIT_ACTIVATION);
        policies[6] = parent -> create_thread_policy              (ORB_CTRL_MODEL);

        Cdmw::OrbSupport::StrategyList poaStrategies;

        PortableServer::POA_var cs_poa =
            Cdmw::OrbSupport::OrbSupport::create_POA(parent,
                                                      COMPONENT_SERVER_POA_NAME,
                                                      poaManager.in(), 
                                                      policies,
                                                      poaStrategies);
        // C) Create the ComponentServer servant
        Components::HomeRegistration_var home_registration = Components::HomeRegistration::_nil();

        if (get_home_registration_option(argc, argv)) {
            //
            // Get naming service
            //
            try {
                CosNaming::NamingContext_var nc = CosNaming::NamingContext::_nil();
                CORBA::Object_var obj = orb -> resolve_initial_references("NameService");
                if(CORBA::is_nil(obj.in())) {
                    std::cerr << argv[0] << ": NameService' is a nil object reference" << std::endl;
                } else {
                    nc = CosNaming::NamingContext::_narrow(obj.in());
                    if(CORBA::is_nil(nc.in())) {
                        std::cerr << argv[0] << ": `NameService' is not a NamingContext object reference"
                                  << std::endl;
                    } else {
                        home_registration = new DefaultHomeRegistration(nc.in());
                    }
                }
            } catch(const CORBA::ORB::InvalidName&) {
                std::cerr << argv[0] << ": Can't resolve `NameService'" << std::endl;
            }
        } else
            home_registration = new StandardHomeRegistration();

        ComponentServer_impl * cs = new ComponentServer_impl(orb,
                                                             cs_poa.in(),
                                                             home_registration.in(),
                                                             event_channel_factory,
                                                             process_name,
                                                             application_name,
                                                             cfg_compserver);
        PortableServer::ServantBase_var serv  = cs;

        // D) Activate the ComponentServer
        the_component_server = cs->_this();
        
        return the_component_server._retn();
    } catch (const PortableServer::POA::AdapterAlreadyExists &) {
        // A component server is a singleton!
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                      CORBA::COMPLETED_NO) );
    } catch (const PortableServer::POA::InvalidPolicy &) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                      CORBA::COMPLETED_NO) );
    } catch (const CORBA::SystemException & ex) {
        CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
    }
}


} // End namespace ComponentServer

} // End namespace CCM

} // End namespce Cdmw

