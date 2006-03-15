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


// Cdmw Files 
#include "Event/eventsupport/EventChannelFactory_impl.hpp"
#include "Event/eventinterface/EventChannelInterface.hpp"
#include "eventsupport/EventChannelUtils_TAO.hpp"

#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"

// TAO files
#include "orbsvcs/CosEvent/CEC_EventChannel.h"
#include "orbsvcs/CosEvent/CEC_Default_Factory.h"


#include "Event/idllib/CdmwEventAdmin.stub.hpp"

namespace
{
    // Maximum number of retries on receiving CORBA::TRANSIENT exception
    // for registering objects.
    const signed long MAX_RETRIES = 1;
    
    const char* DEFAULT_EVENT_CHANNEL_FACTORY = "DefaultEventChannelFactory";
    
    // Supported interface
    const char* EVT_CHANNEL_REP_ID = "IDL:omg.org/CosEventChannelAdmin/EventChannel:1.0";

    const char* PUSH_SUPPIER_REP_ID = "IDL:omg.org/CosEventComm/PushSupplier:1.0";


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
            throw (CORBA::SystemException)
           : m_poa(PortableServer::POA::_duplicate(poa)),
             m_domain(CdmwLifeCycle::ObjectRegistration::_duplicate(name_domain)),
             m_channel_name(channel_name)
        {
        }

        // Override _default_POA
        virtual PortableServer::POA_ptr _default_POA() 
        {
            return PortableServer::POA::_duplicate(m_poa.in());
        }

        void
        disconnect_push_supplier()
            throw (CORBA::SystemException)
        {
            CDMW_ASSERT(!(CORBA::is_nil(m_domain.in())));

            try 
            {
                std::cerr << "EventChannelDestructionNotifier disconnect order" << std::endl;
                m_domain->unregister_object(m_channel_name.c_str());
            } 
            catch (const CosNaming::NamingContext::NotFound & ) 
            {
                // This should not happen 
                std::cerr << "EventChannelDestructionNotifier : NameDomain::NotRegistered!" << std::endl;
            } 
            catch (const CosNaming::NamingContext::InvalidName & ) 
            {
                // This should not happen either 
                std::cerr << "EventChannelDestructionNotifier : InvalidName!" << std::endl;
            } 
            catch (const CORBA::SystemException& e)
            {
                std::cerr << "EventChannelDestructionNotifier :" << e << std::endl;
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
            public virtual PortableServer::ServantLocator,
            public virtual Cdmw::OrbSupport::RefCountLocalObject
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
* The EventChannelFactory_impl class is a user-defined factory while it doesn't 
* localy create event channel but let the Event Server doing it.
* 
*Portability issues: [if no pertinent write none ]
*<p>
*/
        
        
EventChannelFactory_impl::EventChannelFactory_impl(
     CORBA::ORB_ptr                           orb, 
     PortableServer::POA_ptr                  parent,
     CdmwLifeCycle::ObjectRegistration_ptr   name_domain,
     const std::string &                      name_domain_name,
     const std::string &                      factory_name,
     Cdmw::OrbSupport::POAThreadingPolicy &   threading_policy,
     int & argc, char** argv,
     bool use_profile_manager)
     throw(Cdmw::BadParameterException,
     Cdmw::InternalErrorException,
     CORBA::SystemException)
     : m_orb(CORBA::ORB::_duplicate(orb)), 
     m_poa(PortableServer::POA::_duplicate(parent)),
     m_domain(CdmwLifeCycle::ObjectRegistration::_duplicate(name_domain)),
     m_domain_name(name_domain_name),
     m_factory_name(factory_name),
     m_use_profile_manager(use_profile_manager)
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
    m_notifier_poa = m_poa->create_POA(notifier_poa_name.c_str(), pmanager.in(), pl);

    // Create and register servant locator
    PortableServer::ServantLocator_var the_locator = 
        new ServantLocator_impl(name_domain);
    m_notifier_poa->set_servant_manager(the_locator.in());

    // Clean up
    for (CORBA::ULong i = 0; i < len; ++i)
        pl[i]->destroy();

    // Create an other new POA for TAO CEC
    // The attributes used to create eventchannel includes
    // the POA used to activate the ConsumerAdmin, SupplierAdmin, 
    // ProxyPushConsumer, ProxyPushSupplier, ProxyPullConsumer and the 
    // ProxyPullSupplier objects; these POAs must have the IMPLICIT_ACTIVATION 
    // and the SYSTEM_ID policies (as the RootPOA does).     
    len = 0;
    pl.length(len + 1);
    pl[len++] = m_poa->create_id_assignment_policy(PortableServer::SYSTEM_ID);

    pl.length(len + 1);
    pl[len++] 
        = m_poa->create_implicit_activation_policy(PortableServer::IMPLICIT_ACTIVATION);
    
    std::string internal_poa_name = factory_name + "InternalPOA";
    m_internal_poa = m_poa->create_POA(internal_poa_name.c_str(), pmanager.in(), pl);

    // Clean up
    for (CORBA::ULong i = 0; i < len; ++i)
        pl[i]->destroy();
}
        
        
EventChannelFactory_impl::~EventChannelFactory_impl()
    throw()
{

    // get all channel name
    ChannelMap::iterator c_iter = m_created_channels.begin();
    
    // Print out the number of days in June.
    while (c_iter != m_created_channels.end ())
    {
        try 
        {
            m_domain->unregister_object((*c_iter).first.c_str());
        } 
        catch (const CosNaming::NamingContext::NotFound & ) 
        {
            // This could happen if channel has been destroyed! 
        } 
        catch (const CosNaming::NamingContext::InvalidName & ) 
        {
            // This should not happen  
            std::cout << "~EventChannelFactory_impl : InvalidName!" << std::endl;
        } 
        catch (const CORBA::SystemException& e)
        {
            std::cout << "~EventChannelFactory_impl :" << e << std::endl;
        }
        catch (...)
        {
            std::cout << "~EventChannelFactory_impl : unexpected exception raised" << std::endl;
        }

        c_iter++;
    }
    m_created_channels.clear();
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/EventChannelFactory/create_event_channel:1.0
//
CosEventChannelAdmin::EventChannel_ptr
EventChannelFactory_impl::create_event_channel(const char* name)
    throw(CdmwLifeCycle::AlreadyExistant,
          CdmwLifeCycle::InvalidName,
          CdmwEvent::CreationError,
          CORBA::SystemException)
{
    CosEventChannelAdmin::EventChannel_var channel;
    
    std::cout << "INFO: begin creation untyped channel named: " << name << std::endl;
    
    try 
    {
        // create a new factory able to load CEC TAO options
        TAO_CEC_Default_Factory* tao_factory = new TAO_CEC_Default_Factory();

        std::string factory_properties = "";
        
        if (m_use_profile_manager) {
            // get information from eventchannelmanager to init factory with tao properties
            // init (argc, argv), set to 0 for the moment!
            factory_properties = EventChannelUtils::get_factory_properties(name);
        }
        
          tao_factory->init(Common::String::to_strings(factory_properties).size(), 
                          Common::String::to_char_array(
                                Common::String::to_strings(factory_properties)));


        // get internal POA while the attributes use to create eventchannel includes
        // the POA used to activate the ConsumerAdmin, SupplierAdmin, 
        // ProxyPushConsumer, ProxyPushSupplier, ProxyPullConsumer and the 
        // ProxyPullSupplier objects; these POAs must have the IMPLICIT_ACTIVATION 
        // and the SYSTEM_ID policies (as the RootPOA does). 
        TAO_CEC_EventChannel_Attributes attributes(m_internal_poa.in (),
                                                   m_internal_poa.in ());

        TAO_CEC_EventChannel* ec_impl 
            = new TAO_CEC_EventChannel(attributes, tao_factory, true);

          ec_impl->activate();

        channel = ec_impl->_this();

        /*ChannelInfo channel_info;
        channel_info.channel_servant = ec_impl;
        channel_info.channel_ref = CosEventChannelAdmin::EventChannel::_nil();
        //    = CosEventChannelAdmin::EventChannel::_duplicate(channel.in());
        m_created_channels.insert(ChannelMap::value_type(std::string(name), channel_info));
        */
        ChannelMap::iterator c_iter = m_created_channels.find(std::string(name));
        if (c_iter != m_created_channels.end())
        {
            // a channel with the same name has been destroyed, replace it!
            // We are sure it has been destroyed while naming service doesn't raise already exist!
            m_created_channels.erase(c_iter);
        }
        m_created_channels.insert(ChannelMap::value_type(std::string(name), ec_impl));

        if (CORBA::is_nil(channel.in()))
        {
            std::cerr << "ERROR: channel is nil" << std::endl;
            CdmwEvent::CreationError ex;
            std::string reason = "Channel is nil!";
            ex.reason = CORBA::string_dup(reason.c_str());
            throw ex;
        }
    }
    catch (const CdmwEvent::CreationError&)
    {
        std::cerr << "ERROR: CdmwEvent::CreationError exception raised" << std::endl;
        throw;
    }
    catch (const CORBA::SystemException&)
    {
        std::cerr << "ERROR: System exception raised" << std::endl;
        throw;
    }
     catch (...)
     {
        std::cerr << "ERROR: Unexpected exception raised" << std::endl;
        CdmwEvent::CreationError ex;
        std::string reason = "Unexpected exception raised";
        ex.reason = CORBA::string_dup(reason.c_str());
        throw ex;
     }
    
    // Convert channelname to OID
    PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(name);

    // Make a new reference.
    CORBA::Object_var obj = m_notifier_poa->create_reference_with_id(oid.in(), PUSH_SUPPIER_REP_ID);
    CosEventComm::PushSupplier_var notifier = CosEventComm::PushSupplier::_narrow(obj.in());


    // connect notifier to channel in order to be notified when event channel is destroyed
    try 
    {
        Event::EventChannelInterface eventChannelInt(channel.in());

        CosEventComm::PushConsumer_var proxyConsumer 
            = eventChannelInt.get_pushConsumer(notifier.in());
    }
    catch (...)
    {
        // destroy channel, it will be unregistered automatically by ObjectRegistrationGuard
        channel->destroy();
        
        CdmwEvent::CreationError ex;
        std::string reason = "Impossible to connect channel destruction notifier";
        ex.reason = CORBA::string_dup(reason.c_str());
        
        throw ex;
    }
    
    // Register (name,object) within the repository. 
    EventChannelUtils::register_object(m_domain.in(), m_domain_name, channel.in(), name, MAX_RETRIES);

    // Return the object reference
    return channel._retn();
}


//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
//
CORBA::Boolean 
EventChannelFactory_impl::supports(const char* repository_id)
throw(CORBA::SystemException)
{
    bool is_supported = false;
    
    if (strcmp (repository_id, EVT_CHANNEL_REP_ID) == 0)
        is_supported = true;
    
    return is_supported;
}

//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
//
CdmwLifeCycle::RepositoryIdSeq* 
EventChannelFactory_impl::get_supported_objects()
throw(CORBA::SystemException)
{
    CdmwLifeCycle::RepositoryIdSeq_var repIDSeq = new CdmwLifeCycle::RepositoryIdSeq();
    
    repIDSeq->length(1);
    (*repIDSeq)[0] = CORBA::string_dup(EVT_CHANNEL_REP_ID);
    
    return repIDSeq._retn();
}


}; // End namespace Event
}; // End namespace Cdmw


