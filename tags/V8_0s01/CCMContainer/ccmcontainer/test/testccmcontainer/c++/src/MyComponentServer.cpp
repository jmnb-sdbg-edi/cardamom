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


#include "testccmcontainer/MyComponentServer.hpp"
#include "CCMContainer/ccmcontainer/Container_impl.hpp"
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <CCMContainer/ccmcontainer/HomesServantActivator.hpp>
#include "Foundation/common/Assert.hpp"
#include "CCMContainer/idllib/CdmwDeployment.stub.hpp"
#include "Event/idllib/CdmwEvent.stub.hpp"

namespace TestCdmwCCM
{
    
MyComponentServer::MyComponentServer(CORBA::ORB_ptr          orb,
                                     PortableServer::POA_ptr poa,
                                     CdmwDeployment::ComponentInstallation_ptr component_installation)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_container(Components::Deployment::Container::_nil()),
      m_current(PortableServer::Current::_nil()),
      m_component_installation(CdmwDeployment::ComponentInstallation::_duplicate(component_installation)),
      m_is_removed(false)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    CORBA::Object_var obj =
      orb->resolve_initial_references("POACurrent");
    m_current = PortableServer::Current::_narrow(obj.in());
}

    
MyComponentServer::~MyComponentServer()
    throw ()
{
    if (m_is_removed && !CORBA::is_nil(m_container.in())) {
        try {
            m_container->remove();
            m_container = Components::Deployment::Container::_nil();
        } catch (...) {
            // ignore ...
        }
    }
}
    
//
// IDL:omg.org/Components/Deployment/ComponentServer/configuration:1.0
//
Components::ConfigValues*
MyComponentServer::configuration()
    throw(CORBA::SystemException)
{
    return 0;
}
        
//
// IDL:omg.org/Components/Deployment/ComponentServer/get_server_activator:1.0
//
Components::Deployment::ServerActivator_ptr
MyComponentServer::get_server_activator()
    throw(CORBA::SystemException)
{
    return Components::Deployment::ServerActivator::_nil();
}

//
// IDL:omg.org/Components/Deployment/ComponentServer/create_container:1.0
//
Components::Deployment::Container_ptr
MyComponentServer::create_container(const Components::ConfigValues& config)
    throw(Components::CreateFailure,
          Components::Deployment::InvalidConfiguration,
          CORBA::SystemException)
{
    
    using namespace Components;
    
    HomeRegistration_ptr      home_registration = HomeRegistration::_nil();

    Deployment::ComponentServer_var       component_server = Deployment::ComponentServer::_nil();
    try {
        PortableServer::POA_var poa = m_current->get_POA();
        PortableServer::ObjectId_var oid = m_current->get_object_id();

        CORBA::Object_var obj = poa->id_to_reference(oid.in());
        component_server = Deployment::ComponentServer::_narrow(obj.in());
        if (CORBA::is_nil(component_server.in()))
            throw Components::CreateFailure(CdmwDeployment::CONTAINER_ERROR);
    } catch (...) {
        throw Components::CreateFailure(CdmwDeployment::CONTAINER_ERROR);
    }

    PortableServer::POA_var homes_serialize_poa;
    PortableServer::POA_var homes_multithread_poa;
    // Create a SingleThread POA for single threaded homes and a Multithread POA
    // for multithread homes.
    try {
        using namespace Cdmw::OrbSupport;        
        PortableServer::POAManager_var poaManager = m_poa -> the_POAManager();
        CORBA::PolicyList policies;
        policies.length(7);
        policies[0] =
            m_poa -> create_id_assignment_policy(PortableServer::USER_ID);
        policies[1] =
            m_poa -> create_lifespan_policy(PortableServer::PERSISTENT);
        policies[2] =
            m_poa -> create_servant_retention_policy(PortableServer::RETAIN);
        policies[3] =
            m_poa -> create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
        policies[4] = m_poa ->
            create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);
        policies[5] = m_poa ->
            create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);
        // Create a single thread poa
        policies[6] = m_poa ->
            create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
                             

        StrategyList poaStrategies;
        
        const char* SINGLE_THREAD_HOMES_POA_NAME = "HomeSerializePOA";
        const char* MULTI_THREAD_HOMES_POA_NAME  = "HomeMultithreadPOA";

        homes_serialize_poa = Cdmw::OrbSupport::OrbSupport::create_POA(m_poa.in(),
                                                                        SINGLE_THREAD_HOMES_POA_NAME,
                                                                        poaManager.in(), 
                                                                        policies,
                                                                        poaStrategies);
        //
        // Create and install servant activator
        //
        {
            PortableServer::ServantActivator_var the_activator =
                new Cdmw::CCM::Container::HomesServantActivator();
            homes_serialize_poa -> set_servant_manager(the_activator.in());
        }
        
        
        // Create a multi thread poa
        policies[6] = m_poa ->
            create_thread_policy(PortableServer::ORB_CTRL_MODEL);
        
        homes_multithread_poa = Cdmw::OrbSupport::OrbSupport::create_POA(m_poa.in(),
                                                                          MULTI_THREAD_HOMES_POA_NAME,
                                                                          poaManager.in(), 
                                                                          policies,
                                                                          poaStrategies);
        //
        // Create and install servant activator
        //
        {
            PortableServer::ServantActivator_var the_activator =
                new Cdmw::CCM::Container::HomesServantActivator();
            homes_multithread_poa -> set_servant_manager(the_activator.in());
        }
            
    } catch (const PortableServer::POA::AdapterAlreadyExists &) {
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                              CORBA::COMPLETED_YES);
    } catch (const PortableServer::POA::InvalidPolicy &) {
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                              CORBA::COMPLETED_YES);
    } catch (const CORBA::SystemException &) {
        throw; // rethrow
    } 

    CdmwEvent::EventChannelFactory_var no_event_channel_factory
        = CdmwEvent::EventChannelFactory::_nil();
    std::string process_name("DummyProcessName");
    std::string application_name("DummyApplicationName");
    // Create a container instance
    Cdmw::CCM::Container::Container_impl* cont =
        new Cdmw::CCM::Container::Container_impl(m_orb.in(),
                                                  m_poa.in(),
                                                  homes_serialize_poa.in(),
                                                  homes_multithread_poa.in(),
                                                  m_component_installation.in(),
                                                  component_server.in(),
                                                  home_registration,
                                                  no_event_channel_factory.in(),
                                                  process_name,
                                                  application_name,
                                                  config);
    PortableServer::ServantBase_var servant = cont;

    m_container = cont->_this();
    
    return Components::Deployment::Container::_duplicate(m_container.in());
}
        
//
// IDL:omg.org/Components/Deployment/ComponentServer/remove_container:1.0
//
void
MyComponentServer::remove_container(Components::Deployment::Container_ptr cref)
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    if (!CORBA::is_nil(m_container.in()) && m_container->_is_equivalent(cref)) {
        m_container->remove();
        m_container = Components::Deployment::Container::_nil();
    } else
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMUnknownContainerReference,
                               CORBA::COMPLETED_NO);
}
        
//
// IDL:omg.org/Components/Deployment/ComponentServer/get_containers:1.0
//
Components::Deployment::Containers*
MyComponentServer::get_containers()
    throw(CORBA::SystemException)
{
    Components::Deployment::Containers_var containers =
        new Components::Deployment::Containers(1);
    if (CORBA::is_nil(m_container.in())) {
        containers->length(0);
    } else {
        containers->length(1);
        containers->operator[](0)= Components::Deployment::Container::_duplicate(m_container.in());
    }
    return containers._retn();
}
        
//
// IDL:omg.org/Components/Deployment/ComponentServer/remove:1.0
//
void
MyComponentServer::remove()
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    if (m_is_removed)
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                      CORBA::COMPLETED_NO);
    try {
        PortableServer::ObjectId_var oid = m_poa->servant_to_id(this);
        m_poa->deactivate_object(oid.in());
        
        m_is_removed = true;
    } catch (const CORBA::Exception & ex ) {
        throw Components::RemoveFailure(CdmwDeployment::CONTAINER_ERROR);
    }
}

}; // End namespace TestCdmwCCM

