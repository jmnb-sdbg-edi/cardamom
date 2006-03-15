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


#include <CCMContainer/ccmcontainer/Container_impl.hpp>
#include "ccmcontainer/CORBAHelpers.hpp"
#include <CCMContainer/ccmcontainer/HomeAllocator.hpp>
#include <CCMContainer/ccmcontainer/HomeAllocatorRegistry.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>
#include <CCMContainer/ccmcontainer/ComponentServantProvider.hpp>
#include <CCMContainer/idllib/CdmwDeployment.stub.hpp>
#include <LifeCycle/idllib/CosLifeCycle.stub.hpp>
#include "ccmcontainer/CCMContextFactory.hpp"
#include "ccmcontainer/ValueTypeInstaller_impl.hpp"

//#include <osthreads/Mutex.hpp>
//#include <osthreads/MutexGuard.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <CCMContainer/ccmcommon/CCMUtils.hpp>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <memory> // for std::auto_ptr

#include <Foundation/orbsupport/OrbSupport.hpp> // for CORBA::Exception <<
#include <Foundation/osthreads/MutexGuard.hpp>

#include "ccmcontainer/MacroDefs.hpp"

namespace
{
   const CORBA::ULong MAX_HOME_CONFIG_VALUES=1;
   
    //---------------------------------------------------------------------------------------
    /**
     * Purpose:
     * <p>Read a config value
     *
     *@param name Name of the configuration data
     *@param config A sequence of all configuration values
     *@param value Out CORBA::Any parameter for holding the configuration value
     *
     *@return <B>true</B> if the configration value is found, <B>false</B> otherwise.
     */
    bool read_config_value(const std::string & name,
                           const Components::ConfigValues & config,
                           CORBA::Any_out value)
        throw()
    {
        bool found = false;
        CORBA::ULong len = config.length();
        for (CORBA::ULong count = 0; count < len; ++count)
            if (name.compare(config[count]->name()) == 0) {
                value = new CORBA::Any(config[count]->value());
                found = true;
                break;
            }
        return found;
    }
    //---------------------------------------------------------------------------------------
    /**
     * Converts a ComponentKindValue to a string representation
     *
     *@param compond_kind A component kind as specified in the CdmwDeployment IDL module.
     *
     *@return A string representation of the component kind
     */
    std::string ComponentKindValue_to_string(CdmwDeployment::ComponentKindValue compond_kind)
    {
        std::string result("");
        switch (compond_kind) {
        case CdmwDeployment::SERVICE:
            result = "SERVICE";
            break;
        case CdmwDeployment::SESSION:
            result = "SESSION";
            break;
        case CdmwDeployment::PROCESS:
            result = "PROCESS";
            break;
        case CdmwDeployment::ENTITY:
            result = "ENTITY";
            break;
        default:
            result = "UNCLASSIFIED";
        };
        return result;
    }

    /**
     * A functor for use to fill a Sequence of Repository IDs with repository_id
     * fields of CCMHomesList elements.
     */
    class RepositoryIdSeqFiller
    {
    public:
        /**
         *Purpose:
         * <p>Constructor.
         * <B>NOTA:</B> The sequence should be allocated enough space before calling operator().
         * maximum and length parameters of the sequence should be set to the proper values before
         * using RepositoryIdSeqFiller object.
         *
         *@param seq A reference to a sequence of Repository IDs.
         *
         */
        RepositoryIdSeqFiller(CdmwLifeCycle::RepositoryIdSeq& seq)
            : m_repid_seq(seq),
              m_id(0L)
        {}
        typedef Cdmw::CCM::Container::Container_impl::CCMHomesList::value_type value_type;
        
        void operator()(const value_type & home)
        {
            m_repid_seq[m_id++] = CORBA::string_dup(home.second.repository_id.c_str());
        }
    private:
        CdmwLifeCycle::RepositoryIdSeq& m_repid_seq;
        CORBA::ULong                     m_id;
    };

}; // End anonymous namespace

namespace Cdmw
{

template <>
bool is_equivalent_to<CCM::Container::Container_impl::CCMHomesList::value_type>::operator()
    (const CCM::Container::Container_impl::CCMHomesList::value_type& x,
     const CCM::Container::Container_impl::CCMHomesList::value_type& y) const
{
        return (x.first)->_is_equivalent((y.first).in());
}

template <>
bool is_a<CCM::Container::Container_impl::CCMHomesList::value_type>::operator()
    (const CCM::Container::Container_impl::CCMHomesList::value_type& x,
     const std::string & repid) const
{
    return (x.first)->_is_a(repid.c_str());
}

namespace CCM
{
namespace Container
{

Container_impl::Container_impl(CORBA::ORB_ptr                              orb,
                               PortableServer::POA_ptr                     poa,
                               PortableServer::POA_ptr                     homes_serialize_poa,
                               PortableServer::POA_ptr                     homes_multithread_poa,
                               CdmwDeployment::ComponentInstallation_ptr  component_installation,
                               Components::Deployment::ComponentServer_ptr component_server,
                               Components::HomeRegistration_ptr            home_registration,
                               CdmwEvent::EventChannelFactory_ptr         event_channel_factory,
                               const std::string &                         process_name,
                               const std::string &                         application_name,
                               const Components::ConfigValues& config)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_homes_serialize_poa(PortableServer::POA::_duplicate(homes_serialize_poa)),
      m_homes_multithread_poa(PortableServer::POA::_duplicate(homes_multithread_poa)),
      m_component_installation(CdmwDeployment::ComponentInstallation::_duplicate(component_installation)),
      m_component_server(Components::Deployment::ComponentServer::_duplicate(component_server)),
      m_config_values(config),
      m_object_deactivator(orb),
      m_home_registration(Components::HomeRegistration::_duplicate(home_registration)),
      m_event_channel_factory(CdmwEvent::EventChannelFactory::_duplicate(event_channel_factory)),
      m_process_name(process_name),
      m_application_name(application_name),
      m_is_removed(false)
{    
}

Container_impl::~Container_impl()
    throw()
{
    CDMW_MUTEX_GUARD(m_container_mutex);
    if (m_is_removed) {
        try {
            remove_all_homes();
        } catch (...) {
        }
    }
    m_ccm_homes.clear();
    dlclose_all();
}


//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
//
CORBA::Boolean
Container_impl::supports(const char* repository_id)
    throw(CORBA::SystemException)
{
    
    CDMW_MUTEX_GUARD(m_container_mutex);
    CCMHomesList::iterator i =
        std::find_if(m_ccm_homes.begin(),
                     m_ccm_homes.end(),
                     std::bind2nd (is_a<CCMHomesList::value_type>(),std::string(repository_id)));
    if (i != m_ccm_homes.end())
        return true;
    else
        return false;
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
//
CdmwLifeCycle::RepositoryIdSeq*
Container_impl::get_supported_objects()
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_container_mutex);
    const CORBA::ULong lg = CORBA::ULong(m_ccm_homes.size());
    CdmwLifeCycle::RepositoryIdSeq_var seq
        = new CdmwLifeCycle::RepositoryIdSeq(lg);
    seq->length(lg);
    
    std::for_each(m_ccm_homes.begin(),m_ccm_homes.end(),RepositoryIdSeqFiller(seq.inout()));
    
    return seq._retn();
}
        
//
// IDL:omg.org/Components/Deployment/Container/configuration:1.0
//
Components::ConfigValues*
Container_impl::configuration()
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_container_mutex);
    // Accessing the configuration attribute returns the sequence of ConfigValue objects
    // passed in to the create_container operation at the Container creation.
    return Cdmw::duplicate_sequence(m_config_values);
}

//
// IDL:omg.org/Components/Deployment/Container/get_component_server:1.0
//
Components::Deployment::ComponentServer_ptr
Container_impl::get_component_server()
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_container_mutex);
    // The get_component_server operation returns a reference to the
    // ComponentServer object that created this Container.
    return Components::Deployment::ComponentServer::_duplicate(m_component_server.in());
}

//
// IDL:omg.org/Components/Deployment/Container/install_home:1.0
//
Components::CCMHome_ptr
Container_impl::install_home(const char* id,
                             const char* entrypt,
                             const Components::ConfigValues& config)
    throw(Components::Deployment::UnknownImplId,
          Components::Deployment::ImplEntryPointNotFound,
          Components::Deployment::InstallationFailure,
          Components::Deployment::InvalidConfiguration,
          CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_container_mutex);
    // id is the UUID of the home

    // The install_home operation installs and returns a new CCMHome object. The id and
    // entrypt parameters are used by the container to locate an implementation file and
    // instantiate a new home object. The config parameter represents a sequence of
    // ConfigValue objects that provide name value pairs used to configure the installation
    // of the new home instance, e.g. provide persistency source, transaction and security
    // policies which must be applied to the home and its components. The operation raises
    // an UnknownImplId exception if the id parameter does not correspond to any
    // component packages installed using the ComponentInstallation object. The
    // operation raises an ImplEntryPointNotFound exception if the entrypt parameter
    // cannot be found in the implementation returned from ComponentInstallation. The
    // operation raises an InstallationFailure exception if the home could not be installed in
    // the container for internal reasons such as insufficient resources or inadequate
    // implementation for this container (e.g. installing a C++ home implementation in a Java
    // container). The operation raises an InvalidConfiguration exception if the config
    // parameter does not contain valid configuration name value pairs.

    using namespace Components::Deployment;
    
    // TODO: Add traces...

    // Get shared library location. This can raise UnknownImplId or InstallationFailure
    // exceptions.
    if (CORBA::is_nil(m_component_installation.in())) {
        DEBUG_ECHO << "ComponentInstallation reference not set!" << std::endl;
        throw InstallationFailure(CdmwDeployment::COMPONENT_INSTALLATION_NOT_FOUND);
    }
    Location_var location = m_component_installation->get_implementation(id);


    
    // NOTA: These are the configuration values understood by the compiler
    // for component home installation.
    //
    // +====================================+======================================+
    // | Config Name                        | Type                                 |
    // +====================================+======================================+
    // | COMPONENT_KIND (*)                 | ComponentKindValue                   |
    // +------------------------------------+--------------------------------------+
    // | SERVANT_LIFETIME (**)              | ServantLifetimeValue                 |
    // +------------------------------------+--------------------------------------+
    // | POA_POLICIES_FOR_UNCLASSIFIED (***)| sequence<PoaPolicy>                  |
    // +------------------------------------+--------------------------------------+
    // | THREADING_POLICY (*)               | ThreadingPolicyValue                 |
    // +------------------------------------+--------------------------------------+
    // | EXTENDED_POA_POLICIES              | sequence<PoaPolicy>                  |
    // +------------------------------------+--------------------------------------+
    // | HOME_REPOSITORY_ID (*)             | CORBA::RepositoryId                  |
    // +------------------------------------+--------------------------------------+
    // | HOME_SERVANT_CLASSNAME (*)         | string                               |
    // +------------------------------------+--------------------------------------+
    // | VALUETYPE_FACTORY_DEPENDENCIES     | sequence<ValuetypeFactoryDescription>|
    // +------------------------------------+--------------------------------------+
    // | FAULT_TOLERANCE_REPLICATION_STYLE  | enum FTReplicationStyleValue         |
    // | (****)                             |                                      |
    // +------------------------------------+--------------------------------------+
    // 
    //
    // (*) compulsory
    // (**) compulsory for session, process, and entity and NOT for service component kind
    // (***) compulsory for unclassified component kind
    // (****) compulsory for Home creating FT component.
    //
    // We have a home executor. 
    // 1) Get the home allocator for the specified home servant classname
    // 2) Create a CCM context
    // 3) Create/get an object deactivator
    // home type and allocate a home servant.

    // *NOTA*
    // HomeAllocators do not only depend on the home's repository Id
    // but are also dependent on the home and component executors. This is
    // expressed by the <composition> in CIDL. We have then chosen the home's
    // (generated) servant's class name as a key to the home allocator.
    // Also note that there is an analogy between a CDMW LifeCycle framework
    // and CIDL composition. The composition defines a tuple 
    // (home executor,component category, component executor)
    // that can be mapped to CDMW's (factory, strategy, servant provider).

    // *NOTA*
    // 1) The container creates a POA for all component homes it hosts
    // associated with a servant activator.
    // 2) It creates a POA per home instance that activates both components created by
    // the home and all components' facets.
    // 3) Depending on the servant lifetime policy, an appropriate servant manager
    // will be created.

    // I - # Get appropriate home allocator
    //     #

    //
    // - Get Component kind (compulsory)
    //
    CdmwDeployment::ComponentKindValue component_kind;
    {
        CORBA::Any_var feature_any;
        if (read_config_value (CdmwDeployment::COMPONENT_KIND,config,feature_any)) {
            feature_any.in() >>= component_kind;
        } else {
            // TODO: Add traces 
            throw InvalidConfiguration();
        }
    }
    DEBUG_ECHO << "component_kind = " << component_kind << std::endl;

    //
    // - Get's home repository Id (compulsory)
    // 
    std::string home_repid;
    {
        CORBA::Any_var feature_any;
        if (read_config_value (CdmwDeployment::HOME_REPOSITORY_ID,config,feature_any)) {
            const char* repid;
            feature_any.in() >>= repid;
            home_repid = repid;
        } else {
            // TODO: Add traces 
            throw InvalidConfiguration();
        }
    }
    DEBUG_ECHO << "home_repid = " << home_repid << std::endl;
    //
    // - Get Home's servant classname (compulsory)
    //
    std::string home_servant_classname;
    {
        CORBA::Any_var feature_any;
        if (read_config_value (CdmwDeployment::HOME_SERVANT_CLASSNAME,config,feature_any)) {
            const char* classname;
            feature_any.in() >>= classname;
            home_servant_classname = classname;
        } else {
            // TODO: Add traces 
            throw InvalidConfiguration();
        }
    }
    DEBUG_ECHO << "home_servant_classname = " << home_servant_classname << std::endl;
    //
    // - Get servant lifetime policy (not compulsory for service)
    //
    CdmwDeployment::ServantLifetimeValue servant_lifetime;
    {
        CORBA::Any_var feature_any;
        if (read_config_value (CdmwDeployment::SERVANT_LIFETIME,config,feature_any)) {
            feature_any.in() >>= servant_lifetime;
        } else {
            if (component_kind == CdmwDeployment::SERVICE) {
                servant_lifetime = CdmwDeployment::METHOD_LIFETIME;
            } else {
                // TODO: Add traces 
                throw InvalidConfiguration();
            }
            
        }
        if ((component_kind == CdmwDeployment::SERVICE) &&
            (servant_lifetime != CdmwDeployment::METHOD_LIFETIME)) {
                // TODO: Add traces 
                throw InvalidConfiguration();
        } 
    }
    DEBUG_ECHO << "servant_lifetime = " << servant_lifetime << std::endl;
    //
    // - Get POA policies for unclassified if component_kind == UNCLASSIFIED
    //
    const CdmwDeployment::PoaPolicySeq * unclassified_poa_policies;
    if (component_kind == CdmwDeployment::UNCLASSIFIED) {
        CORBA::Any_var feature_any;
        if (read_config_value (CdmwDeployment::POA_POLICIES_FOR_UNCLASSIFIED,config,feature_any)) {
            feature_any.in() >>= unclassified_poa_policies;
            // Ignore for the moment as this is not supported yet!
        } else {
            // TODO: Add traces 
            throw InvalidConfiguration();
        }
    }
    //
    // - Get the threading policy (compulsery)
    //
    CdmwDeployment::ThreadingPolicyValue threading_policy;
    {
        CORBA::Any_var feature_any;
        if (read_config_value (CdmwDeployment::THREADING_POLICY,config,feature_any)) {
            feature_any.in() >>= threading_policy;
        } else {
            // TODO: Add traces 
            throw InvalidConfiguration();
        }
    }
    //
    // - Get extended poa policies
    //
    const CdmwDeployment::PoaPolicySeq * extended_poa_policies;
    {
        CORBA::Any_var feature_any;
        if (read_config_value (CdmwDeployment::EXTENDED_POA_POLICIES,config,feature_any)) {
            feature_any.in() >>= extended_poa_policies;
            // Ignore for the moment as this is not supported yet!
        }
    }
    
    //
    // - Get possible Valuetype factories dependencies
    //
    CCMHomeData::DependencyList dependencies;
            
    const CdmwDeployment::ValuetypeFactoryDescriptionSeq * valuetype_dependencies = 0;
    {
        CORBA::Any_var feature_any;
        if (read_config_value (CdmwDeployment::VALUETYPE_FACTORY_DEPENDENCIES,config,feature_any)) {
            feature_any.in() >>= valuetype_dependencies;
            static CdmwCcmContainer::ValueTypeInstaller_var vt_installer =
                new ValueTypeInstaller_impl(m_orb.in());
            
            for (CORBA::ULong i = 0L; i < valuetype_dependencies->length(); ++i) {
                
                CdmwDeployment::ValuetypeFactoryDescription desc = (*valuetype_dependencies)[i];
                // Register needed valuetype factories
                
                // Get shared library location. This can raise Components::Deployment::UnknownImplId,
                // CdmwDeployment::UnknownValuetype, and Components::Deployment::InstallationFailure
                // exceptions.
                Location_var val_location =
                    m_component_installation->get_valuetypefactory_location(id,desc.repid.in());

                const char* repid = desc.repid.in();
                const char* factory_entrypoint = desc.factory_entrypoint.in();
                CdmwCcmContainer::ValueTypeRegistration_var reg =
                    vt_installer->install_valuetype_factory(repid,
                                                            val_location.in(),
                                                            factory_entrypoint);
                
                dependencies.push_back(reg);
            }
        }
    }
       
        // Open shared library
    using namespace Cdmw::OsSupport;    

#ifdef _WIN32
#error "Testing needed for OS::dlopen(NULL)";
#endif
    const char* shlib_pathname = location.in();
    DEBUG_ECHO << "OS::dlopen(" << shlib_pathname << ")..." << std::endl;
    if (strlen(shlib_pathname) == 0)
        shlib_pathname = NULL; // Access global symbol object
    OS::SharedObjectHandle *shlib = OS::dlopen(shlib_pathname,RTLD_NOW | RTLD_GLOBAL);
    //    if (shlib == NULL) {
    if (shlib->is_null()) {
        DEBUG_ECHO << "Cannot open '" << shlib_pathname << "'\nReason:"
                   << OS::dlerror() << std::endl;
        throw InstallationFailure(CdmwDeployment::CANNOT_OPEN_SHARED_LIB);
    }
    DEBUG_ECHO << "shlib = " << shlib << std::endl;
    
    // Get the home executor factory function
    
    //The entry point should have "C" linkage (i.e. no name-mangling) and 
    // have the following signature:
    //
    //                 Components::HomeExecutorBase* (*)();
    //
    Components::HomeExecutorBase_ptr (*factory_fn_ptr) ();
    DEBUG_ECHO << "OS::dlsym(" << shlib << "," << entrypt << ")..." << std::endl;
    factory_fn_ptr = (Components::HomeExecutorBase_ptr (*) ()) OS::dlsym(shlib,entrypt);
    if (factory_fn_ptr == NULL) {
        DEBUG_ECHO << "Entry point '" << entrypt << "' not found.\nReason:"
                   << OS::dlerror() << std::endl;
        throw ImplEntryPointNotFound();
    }
    DEBUG_ECHO << "factory_fn_ptr = " << factory_fn_ptr << std::endl;
    
    // Get Home executor
    Components::HomeExecutorBase_var home_exec = (*factory_fn_ptr)();

    DEBUG_ECHO << "HomeAllocatorRegistry::GetAllocator(" << home_servant_classname << ").." << std::endl;
    HomeAllocatorBase * home_allocator = HomeAllocatorRegistry::GetAllocator(home_servant_classname);
    if (home_allocator == 0)
        throw InstallationFailure(CdmwDeployment::UNSUPPORTED_COMPONENT_TYPE);
    DEBUG_ECHO << "home_allocator = " << home_allocator << std::endl;
   

    // Create a component context!
    PortableServer::POA_var home_parent_poa = m_homes_multithread_poa;
    if (threading_policy == CdmwDeployment::SERIALIZE)
        home_parent_poa = m_homes_serialize_poa;
    
    // Allocate a home servant.
    // TODO: catch std::bad_alloc exception
    PortableServer::ServantBase_var servant;
    try {
        Components::ConfigValues home_config(MAX_HOME_CONFIG_VALUES);
        CORBA::ULong len = config.length();
        std::string name = CdmwDeployment::FAULT_TOLERANCE_REPLICATION_STYLE;
        for (CORBA::ULong count = 0; count < len; ++count)
            if (name.compare(config[count]->name()) == 0) {
                // FT Config values found
                home_config.length(1);
                home_config[0] = config[count];
                break;
            }
        servant = home_allocator->allocate(home_exec.in(),
                                           m_object_deactivator,
                                           home_config);
    } catch (const std::bad_alloc & ) {
        // TODO: Add Traces
        throw CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                               CORBA::COMPLETED_NO);
    }
    catch (const CORBA::BAD_PARAM& ex)
    {    
        // TODO: check minor code is BAD_PARAMInvalidConfigValues
        if (OrbSupport::BAD_PARAMInvalidConfigValues == ex.minor())
        {
            throw InvalidConfiguration();
        }
        throw;
    }

    ComponentServantProvider * servant_provider =
        dynamic_cast<ComponentServantProvider *>(servant.in());
    if (servant_provider == 0) {
        // TODO: Add traces 
        throw InstallationFailure(CdmwDeployment::NO_COMPONENT_SERVANT_PROVIDER);
    }


    // Create Home object reference
    std::string home_identifier(create_home_identifier(id));
    
    PortableServer::ObjectId_var hoid;
    try {
        hoid = PortableServer::string_to_ObjectId(home_identifier.c_str());
        home_parent_poa -> activate_object_with_id (hoid.in(),servant.in());
    } catch (const PortableServer::POA::ServantAlreadyActive &) {
        // Thrown by activate_object_with_id.
        // We use UNIQUE_ID policy so the servant should not be reused for
        // another ObjectId with this POA.
        // TODO: Add traces 
        throw InstallationFailure(CdmwDeployment::CONTAINER_ERROR);
    } catch (const PortableServer::POA::ObjectAlreadyActive &) {
        // Thrown by activate_object_with_id.
        // The object is already active! Has the servant provider activated it?
        // TODO: Add traces 
        throw InstallationFailure(CdmwDeployment::CONTAINER_ERROR);
    } catch (const PortableServer::POA::WrongPolicy &) {
        // Thrown by activate_object_with_id. 
        // This should not happen as wa are using RETAIN policy.
        // TODO: Add traces 
        throw InstallationFailure(CdmwDeployment::CONTAINER_ERROR);
    } catch (const CORBA::SystemException &) {
        // This should not happen at this stage since the object is just
        // about to be created ...
        // TODO: Add traces 
        throw InstallationFailure(CdmwDeployment::CONTAINER_ERROR);
    }
    // Create object reference
    // This may raise
    //    PortableServer::POA::ObjectNotActive,
    //    PortableServer::POA::WrongPolicy, and
    //    CORBA::SystemException
    Components::CCMHome_var home_ref;
    try {
        CORBA::Object_var obj = home_parent_poa  -> id_to_reference (hoid.in());
            
        home_ref = Components::CCMHome::_narrow (obj.in());
        // [d] Assert the object is really of the expected type
        CDMW_ASSERT(!CORBA::is_nil(home_ref.in()));
    } catch (...) {
        // TODO: Add traces 
        throw InstallationFailure(CdmwDeployment::CONTAINER_ERROR);
    }      
    
    // Register the home allocator using home_identifier (the ObjectId) for use
    // in the ServantActivator::etherealize for home 'disposal'.

    // TODO: Do not forget to call dlclose if an exception occurs!
    HomeAllocatorRegistry::Register(home_identifier,home_allocator);  

    // Create component context
    try {
        CdmwCcmContainer::CCM2Context_var component_context = 
            CCMContextFactory::TheInstance.create_CCMContext(m_orb.in(),
                                                             home_parent_poa.in(),
                                                             home_ref.in(),
                                                             home_identifier,
                                                             servant_provider,
                                                             component_kind,
                                                             servant_lifetime,
                                                             threading_policy,
                                                             m_home_registration.in(),
                                                             m_event_channel_factory.in(),
                                                             m_process_name,
                                                             m_application_name,
                                                             shlib,
                                                             dependencies);
        servant_provider->set_context(component_context.in());
    } catch (const CORBA::Exception & ex) {
        // TODO: Add traces
        DEBUG_ECHO << "File :" << __FILE__ << "\n"
                   << "Line :" << __LINE__ << "\n"
                   << "CORBA Exception :" << ex << std::endl;
        throw InstallationFailure(CdmwDeployment::CONTAINER_ERROR);
    }
    // ** store current home reference
    m_ccm_homes.push_back(std::make_pair(home_ref,CCMHomeData(shlib,home_repid,dependencies)));

    // Store the Home reference within the CDMW Naming and Repository
    if (!CORBA::is_nil(m_home_registration.in())) {
        //  Register it as Cdmw::Common::Locations::ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN/
        //      home_idenifier@process_name@application_name.ComponentHome
        std::string home_name;
        try {
            std::stringstream ostr;
            ostr << home_identifier << "@" << m_process_name << "@" << m_application_name;// << std::ends;
            
            std::string kind("ComponentHome");
            
            CosNaming::Name name(1);
            name.length(1);
            name[0].id   = ostr.str().c_str();
            name[0].kind = kind.c_str();
            
            using namespace Cdmw::CommonSvcs::Naming;
            std::string key = NamingInterface::to_string(name);
            
            home_name = Cdmw::Common::Locations::ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN;
            home_name += "/" + key;
             
            m_home_registration->register_home(home_ref.in(),
                                               home_name.c_str());
        } catch (const CORBA::Exception & ex) {
            // TODO: Add traces
            DEBUG_ECHO << "File :" << __FILE__ << "\n"
                       << "Line :" << __LINE__ << "\n"
                       << "Cannot register '" << home_name << "\n"
                       << "CORBA Exception :" << ex << std::endl;
        }
    }
    
    return home_ref._retn();
}

//
// IDL:omg.org/Components/Deployment/Container/remove_home:1.0
//
void
Container_impl::remove_home(Components::CCMHome_ptr href)
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_container_mutex);
    // The remove_home operation removes a home from the container and all components
    // that were created by this home. The operation raises a BAD_PARAM system
    // exception if the href parameter was not created by this container. 
    // The operation raises a RemoveFailure exception if the home or any of its 
    // components could not be removed from the container for any internal reasons.

    using namespace Components;
    using namespace Cdmw::OsSupport;

    OS::SharedObjectHandle *no_handle = NULL;
    std::string      no_repid("");
    CCMHomeData::DependencyList no_dependencies;
    
    CCMHomeData dummy_home_data(no_handle,no_repid,no_dependencies);
    CCMHome_var home(CCMHome::_duplicate(href));
    CCMHomesList::iterator i =
        std::find_if(m_ccm_homes.begin(),
                     m_ccm_homes.end(),
                     std::bind1st (is_equivalent_to<CCMHomesList::value_type>(),
                                   std::make_pair(home,dummy_home_data)));
    if (i != m_ccm_homes.end()) {
        HomeRemover hr;
        hr(*i);
        m_ccm_homes.erase(i);
    } else 
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMUnknownHomeReference,
                               CORBA::COMPLETED_NO);
}

//
// IDL:omg.org/Components/Deployment/Container/get_homes:1.0
//
Components::CCMHomes*
Container_impl::get_homes()
    throw(CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_container_mutex);
    // The get_homes operation returns a sequence of all CCMHome objects created 
    // by this Container.
    const CORBA::ULong max  = CORBA::ULong(m_ccm_homes.size());
    
    Components::CCMHomes_var homes = new Components::CCMHomes(max);
    homes->length(max);

    CCMHomesList::const_iterator i = m_ccm_homes.begin();
    CCMHomesList::const_iterator end = m_ccm_homes.end();
    
    CORBA::ULong idx = 0L;
    while (i != end) {
        homes[idx]=(*i).first;
        ++idx;
        ++i;
    }

    return homes._retn();
}

void
Container_impl::HomeRemover::operator()(const CCMHomesList::value_type & home) const
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    
    CosLifeCycle::LifeCycleObject_var obj =
        CosLifeCycle::LifeCycleObject::_narrow(home.first);
    if (CORBA::is_nil(obj.in()))
        throw Components::RemoveFailure(CdmwDeployment::CONTAINER_ERROR);
    try {
        obj->remove();
    } catch (const CosLifeCycle::NotRemovable & ) {
        throw Components::RemoveFailure(CdmwDeployment::CONTAINER_ERROR);
    }
}

void
Container_impl::DlCloser::operator()(const CCMHomesList::value_type & home) const
    throw()
{
//    Cdmw::OsSupport::OS::dlclose(home.second.shlib_handle);
}

void
Container_impl::dlclose_all()
    throw()
{
    std::for_each(m_ccm_homes.begin(),m_ccm_homes.end(),DlCloser());
}

void
Container_impl::remove_all_homes()
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    std::for_each(m_ccm_homes.begin(),m_ccm_homes.end(),HomeRemover());
}
//
// IDL:omg.org/Components/Deployment/Container/remove:1.0
//
void
Container_impl::remove()
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    CDMW_MUTEX_GUARD(m_container_mutex);
    // The remove operation removes the container and all homes and components 
    // created in the container. The operation raises a RemoveFailure exception 
    // if the container or any of its homes could not be removed for any internal 
    // reasons.
    DEBUG_ECHO << "Container_impl::remove()" << std::endl;
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


std::string
Container_impl::create_home_identifier(const std::string& uuid)
    throw(std::bad_alloc)
{
    return Cdmw::CCM::Common::create_unique_identifier(uuid,3);
}

}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw


