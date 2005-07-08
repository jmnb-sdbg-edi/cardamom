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


#include "ccmcontainer/Session2Context_impl.hpp"
#include <Foundation/common/Assert.hpp>
//#include <osthreads/Mutex.hpp>
//#include <osthreads/MutexGuard.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <CCMContainer/ccmcommon/CCMUtils.hpp>
#include <sstream>
#include <iomanip>

#include "ccmcontainer/MacroDefs.hpp"


#ifdef ENABLE_DEBUG_ECHO
#define PRINT_POA_NAME(poa, comment) \
do {\
    CORBA::String_var s = poa->the_name();\
    std::cerr << "--->" << comment << ": POA name is \'" << s.in() << "\' " << std::endl;\
} while(0)
#else
#define PRINT_POA_NAME(poa, comment)
#endif

namespace
{
    
    // **********************************************************************
    // A SERVANT ACTIVATOR
    // **********************************************************************
    class TheServantActivator : 
        public virtual PortableServer::ServantActivator,
        public virtual Cdmw::OrbSupport::RefCountLocalObject
    {
    public:
        
        TheServantActivator(Cdmw::LifeCycle::ServantProviderBase * provider)
            : m_servant_provider(provider)
        {
            DEBUG_ECHO << "TheServantActivator::TheServantActivator()" << std::endl;
            CDMW_ASSERT(provider != 0);
            PortableServer::Servant servant 
                = dynamic_cast<PortableServer::Servant> (provider);
            CDMW_ASSERT(servant != 0);
            
            servant->_add_ref();
        }
        ~TheServantActivator()
        {
            DEBUG_ECHO << "TheServantActivator::~TheServantActivator()" << std::endl;
            // NOTA: There is no need for a servant provider allocator as this
            // also a home allocator.
            PortableServer::Servant servant 
                = dynamic_cast<PortableServer::Servant> (m_servant_provider);
            CDMW_ASSERT(servant != 0);
            
            servant->_remove_ref();
            
            m_servant_provider = 0;
        }
        
        virtual PortableServer::Servant 
        incarnate(const PortableServer::ObjectId& oid, 
                  PortableServer::POA_ptr         poa)
            throw(PortableServer::ForwardRequest, CORBA::SystemException)
        {
            CDMW_ASSERT(m_servant_provider != 0);             
            return m_servant_provider->lookup_servant(oid);
        }
        
        virtual void 
        etherealize(const PortableServer::ObjectId& oid,
                    PortableServer::POA_ptr poa,
                    PortableServer::Servant servant,
                    CORBA::Boolean cleanup_in_progress,
                    CORBA::Boolean remaining_activations)
            throw(CORBA::SystemException)
        {
            CDMW_ASSERT(m_servant_provider != 0);
            
            if(!remaining_activations)
            {
                m_servant_provider -> release_servant (oid, poa, servant);
            }
        }
    private:        
        Cdmw::LifeCycle::ServantProviderBase * m_servant_provider;
    };
    // **********************************************************************
    // A SERVANT LOCATOR
    // **********************************************************************

    class TheServantLocator : 
            public virtual PortableServer::ServantLocator,
            public virtual Cdmw::OrbSupport::RefCountLocalObject
    {
    public:

        TheServantLocator(Cdmw::LifeCycle::ServantProviderBase * provider)
            : m_servant_provider(provider)
        {
            DEBUG_ECHO << "TheServantLocator::TheServantLocator()" << std::endl;
            CDMW_ASSERT(provider != 0);
            PortableServer::Servant servant 
                = dynamic_cast<PortableServer::Servant> (provider);
            CDMW_ASSERT(servant != 0);
            
            servant->_add_ref();
        }
        ~TheServantLocator()
        {
            DEBUG_ECHO << "TheServantLocator::~TheServantLocator()" << std::endl;
            // NOTA: There is no need for a servant provider allocator as this
            // also a home allocator.
            PortableServer::Servant servant 
                = dynamic_cast<PortableServer::Servant> (m_servant_provider);
            CDMW_ASSERT(servant != 0);
            
            servant->_remove_ref();
            
            m_servant_provider = 0;
        }

        PortableServer::Servant 
        preinvoke(const PortableServer::ObjectId& oid,
                  PortableServer::POA_ptr poa,
                  const char* operation,
                  PortableServer::ServantLocator::Cookie&
                 the_cookie)
            throw(CORBA::SystemException)
        {
            CDMW_ASSERT(m_servant_provider != 0);              
            return m_servant_provider->lookup_servant(oid);
        }
        void 
        postinvoke(const PortableServer::ObjectId& oid,
                   PortableServer::POA_ptr poa,
                   const char* operation,
                   PortableServer::ServantLocator::Cookie the_cookie,
                   PortableServer::Servant the_servant)
            throw(CORBA::SystemException)
        {
            CDMW_ASSERT(m_servant_provider != 0);
            m_servant_provider -> release_servant (oid, poa, the_servant);
        }
        private:        
            Cdmw::LifeCycle::ServantProviderBase * m_servant_provider;
    }; // End TheServantLocator

    /**
     *
     */
/*    std::string
    create_port_identifier(const char* name)
        throw(std::bad_alloc)
    {
        try {
            const unsigned long MAX_COUNTER = 0xFFFFFFFF;
            static Cdmw::OsSupport::Mutex count_mutex;
            static unsigned long counter=0L;
            CDMW_MUTEX_GUARD(count_mutex);
            
            if (counter == MAX_COUNTER)
                counter = 0L;
            else
                counter++;
            
            std::stringstream ostr;
            ostr << name << "_" << std::setw(3) << std::setfill('0') << counter;
            
            return ostr.str();
        } catch (...) {
            throw std::bad_alloc();
        }
    }
*/
    /**
     * Creates an Event Channel object through the CDMW's Event Channel Factory.
     */
    ::CosEventChannelAdmin::EventChannel_ptr
    create_event_channel(::CdmwEvent::EventChannelFactory_ptr evt_factory,
                         const std::string &                   process_name,
                         const std::string &                   application_name,
                         ::Components::PortDescription*        port)
        throw (::Components::CreateFailure,CORBA::SystemException)
    {
        CDMW_ASSERT(!CORBA::is_nil(evt_factory));
        CDMW_ASSERT(port != 0);
        
        std::string key;
        try {
            std::string port_id = Cdmw::CCM::Common::create_unique_identifier(port->name(),3);
            
            std::ostringstream ostr;
            ostr << port_id << "@" << process_name << "@" << application_name;
        
            std::string kind("EventChannel");
            
            CosNaming::Name name(1);
            name.length(1);
            name[0].id   = ostr.str().c_str();
            name[0].kind = kind.c_str();
            
            using namespace Cdmw::NamingAndRepository;
            key = NamingInterface::to_string(name);

            ::CosEventChannelAdmin::EventChannel_var ec
                  = evt_factory->create_event_channel(key.c_str());

            return ec._retn();
        } catch (const std::bad_alloc & ex) {
            DEBUG_ECHO << "File :" << __FILE__ << "\n"
                       << "Line :" << __LINE__ << "\n"
                       << "Std Exception :" << ex.what() << std::endl;
            throw CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                   CORBA::COMPLETED_NO);
        } catch (const CORBA::SystemException & ex) {
            DEBUG_ECHO << "File :" << __FILE__ << "\n"
                       << "Line :" << __LINE__ << "\n"
                       << "CORBA System Exception :" << ex << std::endl;
            throw;
        } catch (const CdmwLifeCycle::AlreadyExistant & ex) {
            DEBUG_ECHO << "File :" << __FILE__ << "\n"
                       << "Line :" << __LINE__ << "\n"
                       << "Exception : event channel '"<< key << "' : " << ex << std::endl;
            throw ::Components::CreateFailure(CdmwDeployment::EVENT_CHANNEL_ALREADY_EXISTS);
        } catch (const CdmwLifeCycle::InvalidName & ex) {
            DEBUG_ECHO << "File :" << __FILE__ << "\n"
                       << "Line :" << __LINE__ << "\n"
                       << "Exception :" << ex << std::endl;
            throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                  CORBA::COMPLETED_NO);
        } catch (const CdmwEvent::CreationError & ex) {
            DEBUG_ECHO << "File :" << __FILE__ << "\n"
                       << "Line :" << __LINE__ << "\n"
                       << "Exception :" << ex << std::endl;
            throw ::Components::CreateFailure(CdmwDeployment::CREATE_FAILURE);
        }
    
    }

    
}; // End of anonymous namespace



namespace Cdmw
{

namespace CCM
{
namespace Container
{

    
Session2Context_impl::Session2Context_impl(CORBA::ORB_ptr                            orb,
                                           PortableServer::POA_ptr                   parent,
                                           Components::CCMHome_ptr                   home,
                                           const std::string &                       home_id,
                                           ComponentServantProvider*                 servant_provider,
                                           CdmwDeployment::ServantLifetimeValue     servant_lifetime,
                                           CdmwDeployment::ThreadingPolicyValue     threading_policy,
                                           Components::HomeRegistration_ptr          home_registration,
                                           CdmwEvent::EventChannelFactory_ptr       event_channel_factory,
                                           const std::string &                       process_name,
                                           const std::string &                       application_name,
                                           Cdmw::OsSupport::OS::SharedObjectHandle*    executor_shlib_handle,
                                           const ValueTypeDependencyList &           valuetype_dependencies)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_current(PortableServer::Current::_nil()),
      m_poa(PortableServer::POA::_nil()),
      m_home(Components::CCMHome::_duplicate(home)),
      m_home_registration(Components::HomeRegistration::_duplicate(home_registration)),
      m_servant_lifetime(servant_lifetime),
      m_event_channel_factory(CdmwEvent::EventChannelFactory::_duplicate(event_channel_factory)),
      m_process_name(process_name),
      m_application_name(application_name),
      m_executor_shlib_handle(executor_shlib_handle),
      m_valuetype_dependencies(valuetype_dependencies)
{
    CDMW_ASSERT(!CORBA::is_nil(orb));

#ifdef CDMW_USE_FAULTTOLERANCE  
    try
    {
       CORBA::Object_var obj = m_orb->resolve_initial_references("FTGroupRepository");

       if (CORBA::is_nil(obj))
       {
          // this case is possible: component may be compiled with FT but embedded in a non FT process.
          m_group_repository = CdmwFT::Location::GroupRepository::_nil();
       }
       else
       {
          m_group_repository = CdmwFT::Location::GroupRepository::_narrow(obj.in());
       }
    }
    catch (const CORBA::ORB::InvalidName& )
    {
        // this case is possible: component may be compiled with FT but embedded in a non FT process.
    }
    catch (const CORBA::Exception& e)
    {
        DEBUG_ECHO << "File : " << __FILE__ << "\n"
                   << "Line : " << __LINE__ << "\n"
                   << "CORBA Exception : " << ex << std::endl;
        _set_ref_count(0);
        throw;
    }

#endif
    try {
        CORBA::Object_var obj =
            orb->resolve_initial_references("POACurrent");
        m_current = PortableServer::Current::_narrow(obj.in());
        DEBUG_ECHO << "Session2Context_impl::Session2Context_impl() created" << std::endl;
        
        // A Context object is created at home installation. This will setup a whole lifecycle
        // framework for components and components' facets managed by this home.
        //
        // A lifecycle framework contains the following actors:
        //   A - *) An object factory ( the home created by the container)
        //   B - *) An object adapter
        //   C - *) A servant manager
        //   D - *) A servant provider
        //
        // A) Create the object adapter
        // POA policies depend on the component_kind, lifetime_policy, and threading_policy
        
        using namespace Cdmw::OrbSupport;        
        PortableServer::POAManager_var poaManager = parent -> the_POAManager();
        CORBA::PolicyList policies;
        policies.length(7);
        policies[0] =
            parent -> create_id_assignment_policy(PortableServer::USER_ID);
        policies[1] =
            parent -> create_lifespan_policy(PortableServer::TRANSIENT);
        
        if ((servant_lifetime == CdmwDeployment::COMPONENT_LIFETIME) ||
            (servant_lifetime == CdmwDeployment::CONTAINER_LIFETIME)) {
            policies[2] =
                parent -> create_servant_retention_policy(PortableServer::RETAIN);
        } else if (servant_lifetime == CdmwDeployment::METHOD_LIFETIME) {
            policies[2] =
                parent -> create_servant_retention_policy(PortableServer::NON_RETAIN);
        } else {
            // TRANSACTION_LIFETIME servant lifetime policy is not supported!
            throw CORBA::NO_IMPLEMENT(NO_IMPLEMENTTransactionsNotSupported,
                                      CORBA::COMPLETED_NO);
        }
        policies[3] =
            parent -> create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
        policies[4] = parent ->
            create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);
        policies[5] = parent ->
            create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);
        if (threading_policy == CdmwDeployment::SERIALIZE) {
            policies[6] = parent ->
                create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
        } else {
            policies[6] = parent ->
                create_thread_policy(PortableServer::ORB_CTRL_MODEL);
        }
        
        StrategyList poaStrategies;
        
        m_poa = Cdmw::OrbSupport::OrbSupport::create_POA(parent,
                                                          home_id.c_str(),
                                                          poaManager.in(), 
                                                          policies,
                                                          poaStrategies);
        PRINT_POA_NAME(m_poa,"Session2Context_impl() m_poa");
        //
        // Create and install servant activator
        //
        {
            PortableServer::ServantManager_var the_servant_manager;
            if (servant_lifetime == CdmwDeployment::METHOD_LIFETIME)
                the_servant_manager = new TheServantLocator(servant_provider);
            else 
                the_servant_manager = new TheServantActivator(servant_provider);
            m_poa -> set_servant_manager(the_servant_manager.in());
        }
    } catch (const CORBA::Exception & ex) {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        DEBUG_ECHO << "File : " << __FILE__ << "\n"
                   << "Line : " << __LINE__ << "\n"
                   << "CORBA Exception : " << ex << std::endl;
        _set_ref_count(0);
        throw;
    } catch (...) {
        // This is to avoid Assertion failure in debug mode as internal ref count
        // equals 1 (CORBA C++ mapping oblige!)
        _set_ref_count(0);
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    }
    
}

Session2Context_impl::~Session2Context_impl()
    throw ()
{
    DEBUG_ECHO << "Session2Context_impl::~Session2Context_impl() called" << std::endl;
    // Delete home's POA

    try {
        PRINT_POA_NAME(m_poa,"~Session2Context_impl() m_poa");
        m_poa->destroy(true,false);
        m_poa = PortableServer::POA::_nil();
    } catch (const CORBA::Exception & ex) {
        DEBUG_ECHO << "File : " << __FILE__ << "\n"
                   << "Line : " << __LINE__ << "\n"
                   << "CORBA Exception : " << ex << std::endl;
    }
    // Unload component shared/dynamically linked library
    Cdmw::OsSupport::OS::dlclose(m_executor_shlib_handle);
}

PortableServer::ObjectId*
Session2Context_impl::create_ObjectId()
    throw (std::bad_alloc)
{
    try {
        unsigned long counter = Cdmw::CCM::Common::create_identifier();
        
        using namespace Cdmw::OsSupport;
        OS::Timeval tv = OS::get_time();
        
        
        std::stringstream ostr;
        ostr << tv.seconds << ":" << tv.microseconds << ":" << counter;
        
        return PortableServer::string_to_ObjectId(ostr.str().c_str());
    } catch (...) {
        throw std::bad_alloc();
    }
}


//--- From CCMContext -----------------------------------------------//
//
// IDL:omg.org/Components/CCMContext/get_caller_principal:1.0
//
Components::Principal_ptr
Session2Context_impl::get_caller_principal()
    throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT(Cdmw::OrbSupport::NO_IMPLEMENTSecurityNotSupported,
                              CORBA::COMPLETED_NO);
    return Components::Principal::_nil();
}
        
//
// IDL:omg.org/Components/CCMContext/get_CCM_home:1.0
//
Components::CCMHome_ptr
Session2Context_impl::get_CCM_home()
    throw (CORBA::SystemException)
{
    return Components::CCMHome::_duplicate(m_home.in());
}

//
// IDL:omg.org/Components/CCMContext/get_rollback_only:1.0
//
CORBA::Boolean
Session2Context_impl::get_rollback_only() //raises (IllegalState);
    throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT(Cdmw::OrbSupport::NO_IMPLEMENTTransactionsNotSupported,
                              CORBA::COMPLETED_NO);
}

//
// IDL:omg.org/Components/CCMContext/get_user_transaction:1.0
//
Components::Transaction::UserTransaction_ptr
Session2Context_impl::get_user_transaction() // raises (IllegalState);
    throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT(Cdmw::OrbSupport::NO_IMPLEMENTTransactionsNotSupported,
                              CORBA::COMPLETED_NO);
}
        
//
// IDL:omg.org/Components/CCMContext/is_caller_in_role:1.0
//
CORBA::Boolean
Session2Context_impl::is_caller_in_role(const char* role)
    throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT(Cdmw::OrbSupport::NO_IMPLEMENTSecurityNotSupported,
                              CORBA::COMPLETED_NO);
}
        
//
// IDL:omg.org/Components/CCMContext/set_rollback_only:1.0
//
void
Session2Context_impl::Session2Context_impl::set_rollback_only() // raises (IllegalState);
    throw (CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT(Cdmw::OrbSupport::NO_IMPLEMENTTransactionsNotSupported,
                              CORBA::COMPLETED_NO);
}

//--- From CCM2Context ----------------------------------------------//
//
// IDL:omg.org/Components/CCM2Context/get_home_registration:1.0
//
Components::HomeRegistration_ptr
Session2Context_impl::get_home_registration()
    throw (CORBA::SystemException)
{
    return Components::HomeRegistration::_duplicate(m_home_registration.in());
}

//
// IDL:omg.org/Components/CCM2Context/req_passivate:1.0
//
void
Session2Context_impl::req_passivate()
    throw (CORBA::SystemException)
{
    if ((m_servant_lifetime != CdmwDeployment::COMPONENT_LIFETIME) &&
        (m_servant_lifetime != CdmwDeployment::CONTAINER_LIFETIME))
        throw Components::PolicyMismatch();

    try {
        PortableServer::POA_var poa = m_current->get_POA();
        PortableServer::ObjectId_var oid = m_current->get_object_id();
        
        PRINT_POA_NAME(poa,"req_passivate() current ");
        poa->deactivate_object(oid.in());
    } catch (const PortableServer::Current::NoContext & ) {
        throw Components::PolicyMismatch();
        // TODO: Raise an issue in the components FTF.
        // throw Component::IllegalState();
    } catch (const PortableServer::POA::ObjectNotActive & ) {
        // should not happen!
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    } catch  (const PortableServer::POA::WrongPolicy & ) {
        throw Components::PolicyMismatch();
    } catch (const CORBA::SystemException &) {
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    }
}
        
//
// IDL:omg.org/Components/CCM2Context/get_persistence:1.0
//
Components::CatalogBase_ptr
Session2Context_impl::get_persistence(const char* catalog_type_id)
    throw (CORBA::SystemException)
{
    throw Components::PersistenceNotAvailable();
}

//--- From SessionContext -------------------------------------------//
//
// IDL:omg.org/Components/SessionContext/get_CCM_object:1.0
//
CORBA::Object_ptr
Session2Context_impl::get_CCM_object()
    throw (CORBA::SystemException)
{
    //
    // CAUTION: Potential problem in using POA::Current (found in Orbacus/C++ 4.1.0)
    //          Following a narrow within a Home_impl object, the ORB used the same
    //          thread for the is_a() message and the POA::Current return the POA
    //          used for the initial invocation (home operation) and the object id
    //          of the home.
    try {
        PortableServer::POA_var poa = m_current->get_POA();
        PortableServer::ObjectId_var oid = m_current->get_object_id();
        
        PRINT_POA_NAME(poa,"get_CCM_object() current ");
        return poa->id_to_reference(oid.in());
    } catch (const PortableServer::Current::NoContext & ) {
        throw Components::IllegalState();
    } catch (const PortableServer::POA::ObjectNotActive & ) {
        // should not happen!
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    } catch  (const PortableServer::POA::WrongPolicy & ) {
        // TODO/FIXME: Check for NON_RETAIN servant retention policy
        // should not happen!
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    } catch (const CORBA::SystemException &) {
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    }
}
        
//--- From Session2Context ------------------------------------------//
//
// IDL:omg.org/Components/Session2Context/create_ref:1.0
//
CORBA::Object_ptr
Session2Context_impl::create_ref(const char* repid)
    throw (CORBA::SystemException)
{
    try {
        PortableServer::ObjectId_var oid = create_ObjectId();
        
        PRINT_POA_NAME(m_poa,"create_ref() using m_poa ");
        return m_poa->create_reference_with_id(oid.in(),repid);
    } catch (const std::bad_alloc & ) {
        throw CORBA::NO_MEMORY(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    }
}
        
//
// IDL:omg.org/Components/Session2Context/create_ref_from_oid:1.0
//
CORBA::Object_ptr
Session2Context_impl::create_ref_from_oid(const ::CORBA::OctetSeq& oid,
                                          const char* repid)
    throw (CORBA::SystemException)
{
    PortableServer::ObjectId_var oid_ =
        new PortableServer::ObjectId(oid.maximum(),
                                     oid.length(),
                                     const_cast<CORBA::Octet*>(oid.get_buffer()),
                                     false);
    PRINT_POA_NAME(m_poa,"create_ref_from_oid() using m_poa ");
    return m_poa->create_reference_with_id(oid_.in(),repid);
}
        
//
// IDL:omg.org/Components/Session2Context/get_oid_from_ref:1.0
//
CORBA::OctetSeq*
Session2Context_impl::get_oid_from_ref(CORBA::Object_ptr objref)
    throw (CORBA::SystemException)
//  raises (IllegalState, BadComponentReference);
{
//  The get_oid_from_ref operation is used by the component to extract the  oid 
//  encapsulated in the reference. The  objref parameter specifies the reference which 
//  contains the  oid. This operation must be called within an operation invocation. If not 
//  the IllegalState exception shall be raised. If the reference was not created by this 
//  container, the  BadComponentReference with the  WRONG_CONTAINER minor 
//  code is raised.
    try {
        PortableServer::POA_var poa = m_current->get_POA();
        
        PRINT_POA_NAME(m_poa,"get_oid_from_ref() using m_poa ");
        PRINT_POA_NAME(poa,"get_oid_from_ref() current ");
        PortableServer::ObjectId_var oid = m_poa->reference_to_id(objref);

        const CORBA::ULong seqmax = oid->maximum();
        const CORBA::ULong seqlen = oid->length();

        CORBA::OctetSeq_var oseq =
              new CORBA::OctetSeq(seqmax, seqlen,
                                  oid->get_buffer(true),
                                  true);
        return oseq._retn();
    } catch (const PortableServer::Current::NoContext & ) {
        throw Components::IllegalState();
    } catch (const PortableServer::POA::WrongAdapter & ) {
        throw Components::BadComponentReference(Components::WRONG_CONTAINER);
    } catch (const PortableServer::POA::WrongPolicy & ) {
        // should not happen!
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    } catch (const CORBA::SystemException &) {
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    }
}

//
// IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/obtain_event_channel_for_emitter:1.0
//
::CosEventChannelAdmin::EventChannel_ptr
Session2Context_impl::obtain_event_channel_for_emitter(::Components::PortDescription* port)
    throw (CORBA::SystemException)
{
    if (!CORBA::is_nil(m_event_channel_factory.in())) {
        try {
            return create_event_channel(m_event_channel_factory.in(),
                                        m_process_name,
                                        m_application_name,
                                        port);
        } catch (const ::Components::CreateFailure & ex ) {
            throw CdmwCcmContainer::NoChannel(ex.reason);
        }
    } else
        throw CdmwCcmContainer::NoChannel(CdmwDeployment::NO_EVENT_CHANNEL_FACTORY);
}

//
// IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/create_event_channel_for_publisher:1.0
//
::CosEventChannelAdmin::EventChannel_ptr
Session2Context_impl::create_event_channel_for_publisher(::Components::PortDescription* port)
    throw (CORBA::SystemException)
{
    if (!CORBA::is_nil(m_event_channel_factory.in())) {
        return create_event_channel(m_event_channel_factory.in(),
                                    m_process_name,
                                    m_application_name,
                                    port);
    } else
        throw Components::CreateFailure(CdmwDeployment::NO_EVENT_CHANNEL_FACTORY);
}

//
// IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/resolve_initial_references:1.0
//
CORBA::Object_ptr
Session2Context_impl::resolve_initial_references(const char* identifier)
    throw (CdmwCcmContainer::CCM2Context::InvalidName, 
           CORBA::SystemException)
{
    try {
        CORBA::Object_var obj = m_orb->resolve_initial_references(identifier);
        return obj._retn();
    } catch (const CORBA::ORB::InvalidName& ) {
       // identifier not defined throw it again to prevent the user.
       throw CdmwCcmContainer::CCM2Context::InvalidName();
    } catch (const CORBA::SystemException& e) {
        throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,CORBA::COMPLETED_NO);
    }
}


}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw


