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


#include <CCMContainer/ccmcomponentserver/ComponentServer_impl.hpp>
#include <CCMContainer/ccmcontainer/Container_impl.hpp>
#include <CCMContainer/ccmcontainer/HomesServantActivator.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include "CCMContainer/idllib/CdmwDeployment.stub.hpp"

#include <algorithm>
#include <functional>

#include <osthreads/MutexGuard.hpp>

namespace
{
    /**
     *
     */
    const char* SINGLE_THREAD_HOMES_POA_NAME = "HomeSerializePOA";
    const char* MULTI_THREAD_HOMES_POA_NAME  = "HomeMultithreadPOA";
    
    static const char* ComponentServer_ids_array[] = {
      "IDL:thalesgroup.com/CdmwCcmContainer/Container:1.0",
      "IDL:omg.org/Components/Deployment/Container:1.0",
      "IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase:1.0",
      0
    };
    static const CORBA::ULong ComponentServer_nb_ids = 3;

    struct ContainerRemover {
        ContainerRemover() {}
        void operator()(const Components::Deployment::Container_var & container) const
            throw(Components::RemoveFailure,
                  CORBA::SystemException)
        {
//            std::cerr << "ContainerRemover::operator()" << std::endl;
            container->remove();
        }
    };
    /**
     * Template binary function for use with stl containers to test two object references
     * for equivalence.
     */
    template <typename T_var>
    struct is_equivalent_to : std::binary_function<T_var,T_var,bool> {
        bool operator() (const T_var& x, const T_var& y) const
        {
            return x->_is_equivalent(y.in());
        }
    };

}; // End of anonymous namespace

namespace Cdmw
{
namespace CCM
{
namespace ComponentServer
{
    
ComponentServer_impl::ComponentServer_impl(CORBA::ORB_ptr                      orb,
                                           PortableServer::POA_ptr             poa,
                                           Components::HomeRegistration_ptr    home_registration,
                                           CdmwEvent::EventChannelFactory_ptr event_channel_factory,
                                           const std::string &                 process_name,
                                           const std::string &                 application_name,
                                           const Components::ConfigValues&     config)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_homes_serialize_poa(PortableServer::POA::_nil()),
      m_homes_multithread_poa(PortableServer::POA::_nil()),
      m_current(PortableServer::Current::_nil()),
      m_config_values(config),
      m_server_activator(Components::Deployment::ServerActivator::_nil()),
      m_component_installation(CdmwDeployment::ComponentInstallation::_nil()),
      m_home_registration(Components::HomeRegistration::_duplicate(home_registration)),
      m_event_channel_factory(CdmwEvent::EventChannelFactory::_duplicate(event_channel_factory)),
      m_process_name(process_name),
      m_application_name(application_name),
      m_is_removed(false)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));
    CORBA::Object_var obj =
      orb->resolve_initial_references("POACurrent");
    m_current = PortableServer::Current::_narrow(obj.in());
 
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

        m_homes_serialize_poa = Cdmw::OrbSupport::OrbSupport::create_POA(m_poa.in(),
                                                                          SINGLE_THREAD_HOMES_POA_NAME,
                                                                          poaManager.in(), 
                                                                          policies,
                                                                          poaStrategies);
        //
        // Create and install servant activator
        //
        {
            PortableServer::ServantActivator_var the_activator =
                new Container::HomesServantActivator();
            m_homes_serialize_poa -> set_servant_manager(the_activator.in());
        }
        
        
        // Create a multi thread poa
        policies[6] = m_poa ->
            create_thread_policy(PortableServer::ORB_CTRL_MODEL);
        
        m_homes_multithread_poa = Cdmw::OrbSupport::OrbSupport::create_POA(m_poa.in(),
                                                                            MULTI_THREAD_HOMES_POA_NAME,
                                                                            poaManager.in(), 
                                                                            policies,
                                                                            poaStrategies);
        //
        // Create and install servant activator
        //
        {
            PortableServer::ServantActivator_var the_activator =
                new Container::HomesServantActivator();
            m_homes_multithread_poa -> set_servant_manager(the_activator.in());
        }
            
    } catch (const PortableServer::POA::AdapterAlreadyExists &) {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                              CORBA::COMPLETED_YES);
    } catch (const PortableServer::POA::InvalidPolicy &) {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                              CORBA::COMPLETED_YES);
    } catch (const CORBA::SystemException &) {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        throw; // rethrow
    }   
}

    
ComponentServer_impl::~ComponentServer_impl()
    throw ()
{
    CDMW_MUTEX_GUARD(m_component_server_mutex);
    if (m_is_removed) {
        try {
            remove_all_containers();
        } catch (...) {
        }
    }
    m_containers.clear();
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
//
CORBA::Boolean
ComponentServer_impl::supports(const char* repository_id)
    throw(CORBA::SystemException)
{
    bool result = false;
    const char** ids = ComponentServer_ids_array;
    const CORBA::ULong nb_ids = ComponentServer_nb_ids;
    for(CORBA::ULong i = 0 ; i < nb_ids ; ++i)
      if(strcmp(repository_id, ids[i]) == 0) {
          result = true;
          break;
      }

    return result;
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
//
CdmwLifeCycle::RepositoryIdSeq*
ComponentServer_impl::get_supported_objects()
    throw(CORBA::SystemException)
{
    const char** ids = ComponentServer_ids_array;
    const CORBA::ULong nb_ids = ComponentServer_nb_ids;

    CdmwLifeCycle::RepositoryIdSeq_var rep_ids
      = new CdmwLifeCycle::RepositoryIdSeq(nb_ids);
    rep_ids->length (nb_ids);
    for (CORBA::ULong i = 0; i < nb_ids ; ++i)
        rep_ids[i] = CORBA::string_dup(ids[i]);

    return rep_ids._retn();
}
    
//
// IDL:omg.org/Components/Deployment/ComponentServer/configuration:1.0
//
Components::ConfigValues*
ComponentServer_impl::configuration()
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_component_server_mutex);
    return new Components::ConfigValues(m_config_values);
}
        
//
// IDL:omg.org/Components/Deployment/ComponentServer/get_server_activator:1.0
//
Components::Deployment::ServerActivator_ptr
ComponentServer_impl::get_server_activator()
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_component_server_mutex);
    return Components::Deployment::ServerActivator::_duplicate(m_server_activator.in());
}

//
// IDL:omg.org/Components/Deployment/ComponentServer/create_container:1.0
//
Components::Deployment::Container_ptr
ComponentServer_impl::create_container(const Components::ConfigValues& config)
    throw(Components::CreateFailure,
          Components::Deployment::InvalidConfiguration,
          CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_component_server_mutex);
    using namespace Components;
    // NOTA: These are the configuration values understood by the component server
    // for container creation
    //
    // +====================================+=============================+
    // | Config Name                        | Type                        |
    // +====================================+=============================+
    // | COMPONENT_KIND (*)                 | ComponentKindValue          |
    // +------------------------------------+-----------------------------+
    //
    // (*) mandatory
    //
    // - Get Component kind (compulsory)
    //
    CdmwDeployment::ComponentKindValue component_kind;
    {
        bool found = false;
        CORBA::ULong len = config.length();
        std::string config_name(CdmwDeployment::COMPONENT_KIND);
        for (CORBA::ULong count = 0; count < len; ++count)
            if (config_name.compare(config[count]->name()) == 0) {
                config[count]->value() >>= component_kind;
                found = true;
                break;
            }
        if (!found) {
            // TODO: Add traces 
            throw Deployment::InvalidConfiguration();
        }
    }

    // Only Session container type is supported!
    if ((component_kind != CdmwDeployment::SERVICE) &&
        (component_kind != CdmwDeployment::SESSION))
        throw CORBA::NO_IMPLEMENT(Cdmw::OrbSupport::NO_IMPLEMENTComponentKindNotSupported,
                                  CORBA::COMPLETED_NO);
    
    
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
    
    // Create a container instance
    Cdmw::CCM::Container::Container_impl* cont =
        new Cdmw::CCM::Container::Container_impl(m_orb.in(),
                                                  m_poa.in(),
                                                  m_homes_serialize_poa.in(),
                                                  m_homes_multithread_poa.in(),
                                                  m_component_installation.in(),
                                                  component_server.in(),
                                                  m_home_registration.in(),
                                                  m_event_channel_factory.in(),
                                                  m_process_name,
                                                  m_application_name,
                                                  config);
    PortableServer::ServantBase_var servant = cont;
    
    Components::Deployment::Container_var container = cont->_this();

    m_containers.push_back(container);
    
    return container._retn();
}
        
//
// IDL:omg.org/Components/Deployment/ComponentServer/remove_container:1.0
//
void
ComponentServer_impl::remove_container(Components::Deployment::Container_ptr cref)
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_component_server_mutex);
    using namespace Components;
    Deployment::Container_var container(Deployment::Container::_duplicate(cref));
    std::list<Deployment::Container_var>::iterator i =
        std::find_if(m_containers.begin(),
                     m_containers.end(),
                     std::bind1st (is_equivalent_to<Deployment::Container_var>(),
                                   container));
    if (i != m_containers.end()) {
        ContainerRemover cr;
        cr(*i);
        m_containers.erase(i);
    } else 
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMUnknownContainerReference,
                               CORBA::COMPLETED_NO);
}
        
//
// IDL:omg.org/Components/Deployment/ComponentServer/get_containers:1.0
//
Components::Deployment::Containers*
ComponentServer_impl::get_containers()
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_component_server_mutex);
    // The get_homes operation returns a sequence of all CCMHome objects created 
    // by this Container.
    const CORBA::ULong max  = CORBA::ULong(m_containers.size());
    using namespace Components;
    Deployment::Containers_var containers = new Deployment::Containers(max);
    containers->length(max);

    std::list<Deployment::Container_var>::const_iterator i = m_containers.begin();
    std::list<Deployment::Container_var>::const_iterator end = m_containers.end();
    
    CORBA::ULong idx = 0L;
    while (i != end) {
        containers[idx++]=*i++;
    }

    return containers._retn();
}

void
ComponentServer_impl::remove_all_containers()
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    std::for_each(m_containers.begin(),m_containers.end(),ContainerRemover());
}        
//
// IDL:omg.org/Components/Deployment/ComponentServer/remove:1.0
//
void
ComponentServer_impl::remove()
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_component_server_mutex);
//    std::cerr << "ComponentServer_impl::remove()" << std::endl;
    if (m_is_removed)
        throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                      CORBA::COMPLETED_NO);
    try {
        PortableServer::ObjectId_var oid = m_poa->servant_to_id(this);
        m_poa->deactivate_object(oid.in());
        
        // remove_all_containers();
        
        m_is_removed = true;
    } catch (const CORBA::Exception & ex ) {
        throw Components::RemoveFailure(CdmwDeployment::CONTAINER_ERROR);
    }
}

//
// IDL:thalesgroup.com/CdmwCcmComponentServer/ComponentServer/set_component_installation:1.0
//
void
ComponentServer_impl::set_component_installation(
    CdmwDeployment::ComponentInstallation_ptr component_installation)
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_component_server_mutex);
    m_component_installation = CdmwDeployment::ComponentInstallation::_duplicate(component_installation);
}
//
// IDL:thalesgroup.com/CdmwCcmComponentServer/ComponentServer/set_server_activator:1.0
//
void
ComponentServer_impl::set_server_activator(
    Components::Deployment::ServerActivator_ptr server_activator)
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_component_server_mutex);
    m_server_activator = Components::Deployment::ServerActivator::_duplicate(server_activator);
}

}; // End namespace ComponentServer
}; // End namespace CCM
}; // End namespace Cdmw

