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


#include "Event/eventsupport/TypedEventChannelFactory_impl.hpp"
#include "Event/eventinterface/TypedEventChannelInterface.hpp"
#include "eventsupport/EventChannelUtils_ORBacusCPP.hpp"

#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "Event/idllib/CdmwEventAdmin.stub.hpp"
#include <sstream>
namespace
{
    // Maximum number of retries on receiving CORBA::TRANSIENT exception
    // for registering objects.
    const signed long MAX_RETRIES = 1;

    const char* DEFAULT_TYPED_EVENT_CHANNEL_FACTORY = "DefaultTypedEventChannelFactory";

    // Supported interface
    const char* TYPED_EVT_CHANNEL_REP_ID = "IDL:omg.org/CosTypedEventChannelAdmin/TypedEventChannel:1.0";

    const char* PUSH_SUPPLIER_REP_ID = "IDL:omg.org/CosEventComm/PushSupplier:1.0";

    const std::string EMPTY_STRING = "";

    // ----------------------------------------------------------------------
    // Event Channel destruction notifier
    // ----------------------------------------------------------------------

    class EventChannelDestructionNotifier : 
            public virtual POA_CosEventComm::PushSupplier,
            public virtual PortableServer::RefCountServantBase
    {
    public:
        EventChannelDestructionNotifier(PortableServer::POA_ptr poa,
                                        CdmwLifeCycle::ObjectRegistration_ptr   name_domain,
                                        const std::string& channel_name)
           : m_poa(PortableServer::POA::_duplicate(poa)),
             m_domain(CdmwLifeCycle::ObjectRegistration::_duplicate(name_domain)),
             m_channel_name(channel_name)
        {
        }

        // Override _default_POA
        virtual PortableServer::POA_ptr _default_POA() 
        {
            return PortableServer::POA::_duplicate(m_poa);
        }

        void
        disconnect_push_supplier()
            throw (CORBA::SystemException)
        {
            CDMW_ASSERT(!(CORBA::is_nil(m_domain)));

            try 
            {
                m_domain->unregister_object(m_channel_name.c_str());
            } 
            catch (const CosNaming::NamingContext::NotFound & ) 
            {
                // This should not happen 
            } 
            catch (const CosNaming::NamingContext::InvalidName & ) 
            {
                // This should not happen either 
            } 
        }

    private:
            PortableServer::POA_var m_poa;
            CdmwLifeCycle::ObjectRegistration_var   m_domain;    // domain where event channels are registered
            std::string                         m_channel_name;  // channel which has been destroyed

    }; // End EventChannelDestructionNotifier

    // ----------------------------------------------------------------------
    // Resource Servant activator
    // ----------------------------------------------------------------------

    class ServantLocator_impl : 
            public virtual POA_PortableServer::ServantLocator,
            public virtual PortableServer::RefCountServantBase
    {
    public:

        ServantLocator_impl(CdmwLifeCycle::ObjectRegistration_ptr name_domain)
            : m_domain(CdmwLifeCycle::ObjectRegistration::_duplicate(name_domain))
        {
        }

        ~ServantLocator_impl()
        {
        }

        PortableServer::Servant 
        preinvoke(const PortableServer::ObjectId& oid,
                  PortableServer::POA_ptr poa,
                  const char* operation,
                  PortableServer::ServantLocator::Cookie&
                 the_cookie)
            throw(CORBA::SystemException)
        {    
            // TODO: Begin for test only
            {
                CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
                std::cout << "\t[EVENT]===> Pre-invoking " << s.in() << std::endl;
            }       

            // Turn the OID into a string
            CORBA::String_var oid_str;
            try 
            {
                oid_str = PortableServer::ObjectId_to_string(oid);
            } 
            catch (const CORBA::BAD_PARAM &) 
            {
                throw CORBA::OBJECT_NOT_EXIST(); // Malformed OID
            }

            return new EventChannelDestructionNotifier(poa, m_domain.in(), oid_str.in());
        }

        void 
        postinvoke(const PortableServer::ObjectId& oid,
                   PortableServer::POA_ptr poa,
                   const char* operation,
                   PortableServer::ServantLocator::Cookie the_cookie,
                   PortableServer::Servant the_servant)
            throw(CORBA::SystemException)
        {           
            // TODO: Begin for test only
            {
                CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
                std::cout << "\t[EVENT]===> Post-invoking " << s.in() << std::endl;
            }       
     
            the_servant->_remove_ref();
        }

        private:        
            CdmwLifeCycle::ObjectRegistration_var m_domain;  // domain where event channels are registered
    }; // End TheServantLocator

};

namespace Cdmw
{
namespace Event
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
* The TypedEventChannelFactory_impl class is a user-defined factory while it doesn't 
* localy create event channel but let the Event Server doing it.
* 
*Portability issues: [if no pertinent write none ]
*<p>
*/


TypedEventChannelFactory_impl::TypedEventChannelFactory_impl(
        CORBA::ORB_ptr                           orb, 
      PortableServer::POA_ptr                  parent,
      CdmwLifeCycle::ObjectRegistration_ptr   name_domain,
      const std::string &                      name_domain_name,
      const std::string &                      factory_name,
      Cdmw::OrbSupport::POAThreadingPolicy &  threading_policy,
      int & argc, char** argv)
    throw(Cdmw::BadParameterException,
          Cdmw::InternalErrorException,
          CORBA::SystemException)
     : m_orb(CORBA::ORB::_duplicate(orb)), 
      m_poa(PortableServer::POA::_duplicate(parent)),
      m_domain(CdmwLifeCycle::ObjectRegistration::_duplicate(name_domain)),
      m_domain_name(name_domain_name),
      m_factory_name(factory_name)
{
    // Create policy list for simple persistence
    CORBA::PolicyList pl;
    CORBA::ULong len = pl.length();
    pl.length(len + 1);
    pl[len++] = m_poa->create_lifespan_policy(PortableServer::PERSISTENT);

    pl.length(len + 1);
    pl[len++] = m_poa->create_id_assignment_policy(PortableServer::USER_ID);

    pl.length(len + 1);
    pl[len++] = m_poa->create_implicit_activation_policy(
        PortableServer::NO_IMPLICIT_ACTIVATION);
    
    pl.length(len + 1);
    pl[len++] = m_poa->create_servant_retention_policy(PortableServer::NON_RETAIN);

    pl.length(len + 1);
    pl[len++] = 
        m_poa->create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);
    
    // Get parent POA's POA manager
    PortableServer::POAManager_var pmanager = m_poa->the_POAManager();

    // Create new POA
    std::string notifier_poa_name = factory_name + "POA";
    m_notifier_poa = m_poa->create_POA(notifier_poa_name.c_str(), pmanager, pl);
    
    // Create and register servant locator
    PortableServer::ServantBase_var servant_locator =
        new ServantLocator_impl(name_domain);
    PortableServer::ServantLocator_var the_locator = 
        dynamic_cast<ServantLocator_impl*>(servant_locator.in())->_this();
    m_notifier_poa->set_servant_manager(the_locator);

    // Clean up
    for (CORBA::ULong i = 0; i < len; ++i)
        pl[i]->destroy();
}


TypedEventChannelFactory_impl::~TypedEventChannelFactory_impl()
    throw()
{
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/TypedEventChannelFactory/create_event_channel:1.0
//
CosTypedEventChannelAdmin::TypedEventChannel_ptr
TypedEventChannelFactory_impl::create_event_channel(const char* name)
    throw(CdmwLifeCycle::AlreadyExistant,
         CdmwLifeCycle::InvalidName,
         CdmwEvent::CreationError,
         CORBA::SystemException)
{
    CosTypedEventChannelAdmin::TypedEventChannel_var channel;
    OBTypedEventChannelFactory::TypedEventChannelFactory_var factory;
    CORBA::Object_var obj;
    
    // check supported interface is specified in xml file
    std::string interfaceKey = "";    
    try
    {
        interfaceKey = EventChannelUtils::find_interface(name);
    }
    catch (const CdmwEvent::CreationError &)
    {       
        throw;
    }
    catch (const CORBA::SystemException &)
    {
        throw;
    }
        
    // if no interface Key  specified do not create typed event channel.
    if (interfaceKey == EMPTY_STRING)
    {       
        CdmwEvent::CreationError ex;
        std::string reason = "No supported key defined for channel:";
        reason = reason + name;
        ex.reason = CORBA::string_dup(reason.c_str());
        
        throw ex;
    }
    
    // find the factory able to create this event channel 
    factory = 
        EventChannelUtils::find_OBFactory<OBTypedEventChannelFactory::TypedEventChannelFactory>
        (m_orb, name, DEFAULT_TYPED_EVENT_CHANNEL_FACTORY);
    
    if (CORBA::is_nil(factory))
    {
        CdmwEvent::CreationError ex;
        std::string reason = "Invalid factory for channel ";
        reason = reason + name;
        ex.reason = CORBA::string_dup(reason.c_str());
        
        throw ex;
    }
    
    try 
    {
        channel = factory->create_channel(name);
    }
    catch (OBEventChannelFactory::ChannelAlreadyExists&)
    {
        CdmwLifeCycle::AlreadyExistant ex;
        ex.name = CORBA::string_dup(name);
        throw ex;
    }
    
    // Convert channelname to OID
    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(name);
    
    // Make a new reference.
    obj = m_notifier_poa->create_reference_with_id(oid, PUSH_SUPPLIER_REP_ID);
    CosEventComm::PushSupplier_var notifier = CosEventComm::PushSupplier::_narrow(obj);

    // connect notifier to channel in order to be notified when event channel is destroyed
    try 
    {
        Event::TypedEventChannelInterface typedEventChannelInt(channel);

        CosEventComm::PushConsumer_var proxyConsumer 
            = typedEventChannelInt.get_typedPushConsumer<CosEventComm::PushConsumer>(interfaceKey.c_str(), notifier);
    }
    catch (const CosTypedEventChannelAdmin::NoSuchImplementation & e)
    {
        // destroy channel, it will be unregistered automatically by ObjectRegistrationGuard
        channel->destroy();
        
        CdmwEvent::CreationError ex;
        std::string reason = interfaceKey;
        reason += " not implemented! Check Interface repository!";
        ex.reason = CORBA::string_dup(reason.c_str());
        
        throw ex;
    }
    catch (const CosEventChannelAdmin::TypeError& e)
    {
        // destroy channel, it will be unregistered automatically by ObjectRegistrationGuard
        channel->destroy();

        CdmwEvent::CreationError ex;
        std::string reason = "Type error with interface ";
        reason += interfaceKey;
        ex.reason = CORBA::string_dup(reason.c_str());
        
        throw ex;
    }
    catch (const CORBA::SystemException& e)
    {
        // destroy channel, it will be unregistered automatically by ObjectRegistrationGuard
        channel->destroy();

        CdmwEvent::CreationError ex;
        std::ostringstream os;
        os << "Impossible to connect channel destruction notifier: system exception raised!" << std::endl;
        os << e << std::endl;
        ex.reason = CORBA::string_dup(os.str().c_str());
        
        throw ex;
    }
    catch (const CORBA::Exception& e)
    {
        // destroy channel, it will be unregistered automatically by ObjectRegistrationGuard
        channel->destroy();

        CdmwEvent::CreationError ex;
        std::ostringstream os;
        os << "Impossible to connect channel destruction notifier: corba exception raised!" << std::endl;
        os << e << std::endl;
        ex.reason = CORBA::string_dup(os.str().c_str());
        
        throw ex;
    }
    catch (...)
    {
        // destroy channel, it will be unregistered automatically by ObjectRegistrationGuard
        channel->destroy();

        CdmwEvent::CreationError ex;
        std::string reason = "Impossible to connect channel destruction notifier: unexpected exception";
        ex.reason = CORBA::string_dup(reason.c_str());

        throw ex;
    }

    // Register (name,object) within the repository. 
    EventChannelUtils::register_object(m_domain, m_domain_name, channel.in(), name, MAX_RETRIES);

    // Return the object reference
    return channel._retn();
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
//
CORBA::Boolean 
TypedEventChannelFactory_impl::supports(const char* repository_id)
    throw(CORBA::SystemException)
{
    bool is_supported = false;

    if (strcmp (repository_id, TYPED_EVT_CHANNEL_REP_ID) == 0)
        is_supported = true;

    return is_supported;
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
//
CdmwLifeCycle::RepositoryIdSeq* 
TypedEventChannelFactory_impl::get_supported_objects()
    throw(CORBA::SystemException)
{
    CdmwLifeCycle::RepositoryIdSeq_var repIDSeq = new CdmwLifeCycle::RepositoryIdSeq();

    repIDSeq->length(1);
    (*repIDSeq)[0] = CORBA::string_dup(TYPED_EVT_CHANNEL_REP_ID);

    return repIDSeq._retn();
}


}; // End namespace Event
}; // End namespace Cdmw


