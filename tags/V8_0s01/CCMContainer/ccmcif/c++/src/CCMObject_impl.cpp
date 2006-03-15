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

#include <iostream>

// Cdmw files
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/common/Assert.hpp>

// Cdmw CCM files
#include <CCMContainer/ccmcif/CCMObject_impl.hpp>
#include <CCMContainer/ccmcif/CCMUtils.hpp>
#include <CCMContainer/ccmcommon/CCMUtils.hpp>

#include <CCMContainer/ccmcif/ComponentPortDescription_impl.hpp>
#include <CCMContainer/ccmcif/FacetDescription_impl.hpp>
#include <CCMContainer/ccmcif/ReceptacleDescription_impl.hpp>
#include <CCMContainer/ccmcif/ConnectionDescription_impl.hpp>
#include <CCMContainer/ccmcif/ConsumerDescription_impl.hpp>
#include <CCMContainer/ccmcif/PublisherDescription_impl.hpp>
#include <CCMContainer/ccmcif/EmitterDescription_impl.hpp>
#include <CCMContainer/ccmcif/SubscriberDescription_impl.hpp>


#include <ccmcif/PushConsumer_impl.hpp>

#include <LifeCycle/idllib/CosLifeCycle.stub.hpp>
#include <CCMContainer/idllib/CdmwDeployment.stub.hpp>


namespace 
{

    const char* PUSH_CONSUMER_ID = "IDL:CosEventComm::PushConsumer:1.0";

    const char* PSEUDO_EMITTER_CK_VALUE = "Emitter";

} // end anonymous namespace


namespace Cdmw {

namespace CCM {

namespace CIF {

//
// IDL:CCMObject:1.0
//
CCMObject_impl::CCMObject_impl(const std::string& comp_oid,
                               Context*           ctx,
                               const std::string& rep_id,
                               CORBA::Object_ptr  comp_ref,
                               bool               is_monolithic)
  : m_context(ctx),
    m_rep_id(rep_id),
    m_comp_ref(CORBA::Object::_duplicate(comp_ref)),
    m_configured(false),
    m_component_oid(comp_oid),
    m_is_removed(false),
    m_is_monolithic(is_monolithic)
{
    CDMW_ASSERT(ctx);
}

CCMObject_impl::~CCMObject_impl()
{
    PRINT_INFO("CCMObject_impl destructor called!");
}

CORBA::Object_ptr
CCMObject_impl::_get_component()
{
    return CORBA::Object::_duplicate(m_comp_ref.in());
}


//
// IDL:omg.org/Components/CCMObject/get_component_def:1.0
//
CORBA::IRObject_ptr
CCMObject_impl::get_component_def()
    throw(CORBA::SystemException)
{
    check_is_removed();
    Components::CCMHome_var home = m_context->get_CCM_home();
    return home->get_component_def();
}

//
// IDL:omg.org/Components/CCMObject/get_ccm_home:1.0
//
Components::CCMHome_ptr
CCMObject_impl::get_ccm_home()
    throw(CORBA::SystemException)
{
    check_is_removed();
    return m_context->get_CCM_home();
}

//
// IDL:omg.org/Components/CCMObject/get_primary_key:1.0
//
Components::PrimaryKeyBase*
CCMObject_impl::get_primary_key()
    throw(Components::NoKeyAvailable,
          CORBA::SystemException)
{
    check_is_removed();

    // THIS CODE shall be moved when an EntityContext class will be created
    // Check that there is an associated primary key.
    // If so, context is an entity context
    //Components::EntityContext_var ectx = 
    //   Components::EntityContext::_narrow(m_context.in());
    //
    //if (CORBA::is_nil(ectx.in()))
    //{
    //    throw Components::NoKeyAvailable();
    //}
    //
    //Components::PrimaryKeyBase* primary_key;
    //try
    //{
    //    primary_key = ectx->get_primary_key();
    //}
    //catch (Components::IllegalState &)
    //{
    //    throw Components::NoKeyAvailable();
    //}

    return m_context->get_primary_key();;
}

//
// IDL:omg.org/Components/CCMObject/configuration_complete:1.0
//
void
CCMObject_impl::configuration_complete()
    throw(Components::InvalidConfiguration,
          CORBA::SystemException)
{
    check_is_removed();

    // Check receptacle are connected
    ReceptacleMap::iterator iter = m_receptacles.begin();
    
    while (iter != m_receptacles.end())
    {
        ReceptacleMap::value_type receptacle = *iter;

        ReceptacleInfo r_info = receptacle.second;
        if (r_info.connections.size() == 0)
        {
            throw Components::InvalidConfiguration();
        }

        iter++;
    }

    m_configured = true;
}

//
// IDL:omg.org/Components/CCMObject/remove:1.0
//
void
CCMObject_impl::remove()
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    check_is_removed();

    try 
    {
            // advise executor of its remove
            PRINT_INFO("advise_executor_of_remove_component...");
            this->advise_executor_of_remove_component();

            if (!m_is_monolithic)
            {
                PRINT_INFO("component not monolithic : remove facets");
                // remove all facets
                FacetMap::iterator f_iter = m_facets.begin();
            
                while (f_iter != m_facets.end())
                {
                    FacetMap::value_type facet = *f_iter;
                    FacetInfo f_info = facet.second;
                
                    CosLifeCycle::LifeCycleObject_var facet_base 
                        = CosLifeCycle::LifeCycleObject::_narrow(f_info.obj_ref.in());

                    facet_base->remove();

                    f_iter++;
                }
            }

            PRINT_INFO("clear facets...");
            m_facets.clear();

            if (!m_is_monolithic)
            {
                PRINT_INFO("component not monolithic : remove consumers");
                // remove all consumers
                ConsumerMap::iterator c_iter = m_consumers.begin();
            
                while (c_iter != m_consumers.end())
                {
                    ConsumerMap::value_type consumer = *c_iter;
                    ConsumerInfo c_info = consumer.second;
                
                    CosLifeCycle::LifeCycleObject_var consumer_base 
                       = CosLifeCycle::LifeCycleObject::_narrow(c_info.consumer.in());

                    consumer_base->remove();

                    c_iter++;
                }
            }

            PRINT_INFO("clear consumers...");
            m_consumers.clear();

            // remove all publishers
            PRINT_INFO("remove all publisher...");
            PublisherMap::iterator p_iter = m_publishers.begin();
            
            while (p_iter != m_publishers.end())
            {
                PublisherMap::value_type publisher = *p_iter;

                if (!CORBA::is_nil(publisher.second.proxy_consumer.in()))
                    publisher.second.proxy_consumer->disconnect_push_consumer();

                if (publisher.second.channel)
                {
                    publisher.second.channel->destroy();
                    delete publisher.second.channel;
                }

                p_iter++;
            }

            // remove all emitters
            PRINT_INFO("remove all emitters...");
            EmitterMap::iterator e_iter = m_emitters.begin();
            
            while (e_iter != m_emitters.end())
            {
                EmitterMap::value_type emitter = *e_iter;
                
                if (emitter.second.channel)
                    delete emitter.second.channel;

                if (!CORBA::is_nil(emitter.second.proxy_consumer.in()))
                    emitter.second.proxy_consumer->disconnect_push_consumer();

                e_iter++;
            }

            // register object as removed
            m_is_removed = true;

            // deactivate servant
            PRINT_INFO("ask to the context req_passivate...");
            m_context->req_passivate();
    } catch (const CORBA::SystemException & e) {
        PRINT_ERROR("CORBA::SystemException raised : " << e);
        // rethrow
        throw;
    }

}

//
// IDL:omg.org/Components/CCMObject/get_all_ports:1.0
//
Components::ComponentPortDescription*
CCMObject_impl::get_all_ports()
    throw(CORBA::SystemException)
{
    check_is_removed();

    Components::FacetDescriptions_var facet_desc = get_all_facets ();
    Components::ReceptacleDescriptions_var recep_desc = get_all_receptacles ();
    Components::ConsumerDescriptions_var consumer_desc = get_all_consumers ();
    Components::EmitterDescriptions_var emitter_desc = get_all_emitters ();
    Components::PublisherDescriptions_var publisher_desc = get_all_publishers ();

    Components::ComponentPortDescription_var component_port_desc =
       new ComponentPortDescription_impl();

    component_port_desc->facets (facet_desc.in());
    component_port_desc->receptacles (recep_desc.in());
    component_port_desc->consumers (consumer_desc.in());
    component_port_desc->emitters (emitter_desc.in());
    component_port_desc->publishers (publisher_desc.in());

    return component_port_desc._retn();
}

//
// IDL:omg.org/Components/Navigation/provide_facet:1.0
//
CORBA::Object_ptr
CCMObject_impl::provide_facet(const char* name)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    check_is_removed();

    if (!name)
    {
        throw Components::InvalidName();
    }

    Components::NameList name_list(1);
    name_list.length(1);
    name_list[0] = name;

    Components::FacetDescriptions_var f_desc = get_named_facets(name_list);

    if (f_desc->length() != 1)
    {
        throw Components::InvalidName();
    }

    return CORBA::Object::_duplicate(f_desc[0L]->facet_ref());
}

//
// IDL:omg.org/Components/Navigation/get_all_facets:1.0
//
Components::FacetDescriptions*
CCMObject_impl::get_all_facets()
    throw(CORBA::SystemException)
{
    check_is_removed();

    Components::NameList name_list;
    
    name_list.length(m_facets.size());
    FacetMap::iterator iter = m_facets.begin();
    CORBA::ULong i_name = 0;

    while (iter != m_facets.end())
    {
        FacetMap::value_type facet = *iter;

        name_list[i_name] = facet.first.c_str();
        i_name++;
        iter++;
    }

    try
    {
        Components::FacetDescriptions_var f_desc = get_named_facets(name_list);
        return f_desc._retn();
    }
    catch (const Components::InvalidName &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNALComponentInvalidName, CORBA::COMPLETED_NO);
    }
}

//
// IDL:omg.org/Components/Navigation/get_named_facets:1.0
//
Components::FacetDescriptions*
CCMObject_impl::get_named_facets(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    check_is_removed();

    Components::FacetDescriptions_var f_desc = new Components::FacetDescriptions;
    f_desc->length(names.length());
    
    for (CORBA::ULong i_name=0; i_name<names.length(); i_name++) 
    {
        std::string f_name(names[i_name]);
        FacetMap::iterator f_iter = m_facets.find(f_name);

        if (f_iter != m_facets.end())
        {
            // receptacle_name exists
            FacetInfo f_info = m_facets[f_name];

            f_desc[i_name] = new FacetDescription_impl();

            f_desc[i_name]->name(f_name.c_str());
            f_desc[i_name]->type_id(f_info.rep_id.c_str());

            CORBA::Object_var facet_ref = get_object_ref(f_info.obj_ref.in());
            f_desc[i_name]->facet_ref(facet_ref.in());
        }
        else
        {
            // name not found => error
            throw Components::InvalidName();
        }
    }
    
    return f_desc._retn();
}

//
// IDL:omg.org/Components/Navigation/same_component:1.0
//
CORBA::Boolean
CCMObject_impl::same_component(CORBA::Object_ptr object_ref)
    throw(CORBA::SystemException)
{
    check_is_removed();

    if (CORBA::is_nil(object_ref))
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

    // First case: object_ref is facet
    // Check object reference is equivalent with a facet of the component.
    FacetMap::iterator f_iter = m_facets.begin();

    while (f_iter != m_facets.end())
    {
        FacetMap::value_type f_valuetype = *f_iter;
        CORBA::Object_var obj_ref 
            = CORBA::Object::_duplicate(f_valuetype.second.obj_ref.in());

        if (obj_ref->_is_equivalent(object_ref))
        {
            return true;
        }

        f_iter++;
    }

    // Second case: object_ref is a component ref
    /*
    // Check facets of this component is equivalent with the current ones
    try 
    {
        Components::CCMObject_var component_ref 
            = Components::CCMObject::_narrow(object_ref);

        if (CORBA::is_nil(component_ref))
        {
            return false;
        }
        Components::FacetDescriptions_var remote_facets = component_ref->get_all_facets();
        Components::FacetDescriptions_var my_facets = get_all_facets();

        CORBA::ULong nb_facet = my_facets->length();

        if (nb_facet != remote_facets->length())
        {
            // components have not the same number of facet
            return false;
        }

        for (CORBA::ULong i=0; i<nb_facet; i++)
        {
            CORBA::Object_ptr obj_ref = my_facets[i]->facet_ref();
            CORBA::Object_ptr obj_ref2 = remote_facets[i]->facet_ref();
            if (!obj_ref->_is_equivalent(obj_ref2) )
            {
                return false;
            }
        }
    }
    catch (...)
    {
        // exception raised
        // object ref is propably not a component ref
        return false;
    }
    */

    try
    {
        CORBA::Object_var comp_ref = m_context->get_CCM_object();
        return comp_ref->_is_equivalent(object_ref);
    }
    catch (const CORBA::Exception&)
    {
        // exception raised
        return false;
    }

    return true;
}

//
// IDL:omg.org/Components/Receptacles/connect:1.0
//
Components::Cookie*
CCMObject_impl::connect(const char* name,
                          CORBA::Object_ptr connection)
    throw(Components::InvalidName,
          Components::InvalidConnection,
          Components::AlreadyConnected,
          Components::ExceededConnectionLimit,
          CORBA::SystemException)
{
    check_is_removed();

    Components::Cookie_var cookie_value;

    if (!name)
    {
        throw Components::InvalidName();
    }


    if (CORBA::is_nil(connection))
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

    // find receptacle
    std::string r_name(name);
    ReceptacleMap::iterator r_iter = m_receptacles.find(r_name);
    
    if (r_iter != m_receptacles.end())
    {
        // receptacle_name exists
        ReceptacleInfo& r_info = m_receptacles[r_name];

        if (!r_info.is_multiplex)
        {
            // simplex receptacle: check if a receptacle is not already connected!
            if (r_info.connections.size() > 0)
            {
                throw Components::AlreadyConnected();
            }                   
        }
        else
        {
            // multiplex receptacle
            // No limit defined for connection : exception ExceededConnectionLimit not raised!
        }

        // Check connection supports the interface declared in uses statement
        if (!connection->_is_a(r_info.rep_id.c_str()))
        {
            throw Components::InvalidConnection();
        }

        // object has the right interface, insert new connection in the list
        // Cookie is created even for simplex receptacle for homogeneity
        cookie_value = Cdmw::CCM::Common::Cookie_impl::create_cookie();
        CORBA::Object_var obj(CORBA::Object::_duplicate(connection));
        r_info.connections.insert(ConnectionMap::value_type(cookie_value, obj));

        if (!r_info.is_multiplex)
        {
            cookie_value = 0;
        }
    }
    else
    {
        // no receptacle named name exist!
        throw Components::InvalidName();
    }

    return cookie_value._retn();
}

//
// IDL:omg.org/Components/Receptacles/disconnect:1.0
//
void
CCMObject_impl::disconnect(const char* name,
                           Components::Cookie* ck)
    throw(Components::InvalidName,
          Components::InvalidConnection,
          Components::CookieRequired,
          Components::NoConnection,
          CORBA::SystemException)
{
    check_is_removed();

    if (!name)
    {
        throw Components::InvalidName();
    }

    // find receptacle
    std::string r_name(name);
    ReceptacleMap::iterator r_iter = m_receptacles.find(r_name);
    
    if (r_iter != m_receptacles.end())
    {
        // receptacle_name exists
        ReceptacleInfo& r_info = m_receptacles[r_name];

        if (r_info.is_multiplex)
        {
            if (!ck)
            {
                throw Components::CookieRequired();
            }
            
            // try to remove this connection from the list
            if (r_info.connections.erase(ck) == 0)
            {
                // no connection found 
                throw Components::InvalidConnection();
            }
        }
        else
        {
            // simplex receptacle
            if (r_info.connections.size() == 0)
            {
                throw Components::NoConnection();
            }            
            
            // erase all element in map (there is only one!)
            r_info.connections.clear();
        }
    }
    else
    {
        // no receptacle named name exist!
        throw Components::InvalidName();
    }
}

//
// IDL:omg.org/Components/Receptacles/get_connections:1.0
//
Components::ConnectionDescriptions*
CCMObject_impl::get_connections(const char* name)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    check_is_removed();

    if (!name)
    {
        throw Components::InvalidName();
    }

    Components::NameList name_list(1);
    name_list.length(1);
    name_list[0] = name;

    Components::ReceptacleDescriptions_var r_desc = get_named_receptacles(name_list);
    return new Components::ConnectionDescriptions (r_desc[0L]->connections());
}

//
// IDL:omg.org/Components/Receptacles/get_all_receptacles:1.0
//
Components::ReceptacleDescriptions*
CCMObject_impl::get_all_receptacles()
    throw(CORBA::SystemException)
{
    check_is_removed();

    Components::NameList name_list;
    
    name_list.length(m_receptacles.size());
    ReceptacleMap::iterator iter = m_receptacles.begin();
    CORBA::ULong i_name = 0;

    while (iter != m_receptacles.end())
    {
        ReceptacleMap::value_type receptacle = *iter;

        name_list[i_name] = receptacle.first.c_str();
        i_name++;
        iter++;
    }
    try
    {
        Components::ReceptacleDescriptions_var r_desc 
            = get_named_receptacles(name_list);

        return r_desc._retn();
    }
    catch (const Components::InvalidName &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNALComponentInvalidName, CORBA::COMPLETED_NO);
    }

}

//
// IDL:omg.org/Components/Receptacles/get_named_receptacles:1.0
//
Components::ReceptacleDescriptions*
CCMObject_impl::get_named_receptacles(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    check_is_removed();

    Components::ReceptacleDescriptions_var r_desc = new Components::ReceptacleDescriptions;
    r_desc->length(names.length());
    
    for (CORBA::ULong i_name=0; i_name<names.length(); i_name++) 
    {
        std::string r_name(names[i_name]);
        ReceptacleMap::iterator r_iter = m_receptacles.find(r_name);

        if (r_iter != m_receptacles.end())
        {
            // receptacle_name exists
            ReceptacleInfo r_info = m_receptacles[r_name];

            r_desc[i_name] = new ReceptacleDescription_impl();

            r_desc[i_name]->name(r_name.c_str());
            r_desc[i_name]->type_id(r_info.rep_id.c_str());
            r_desc[i_name]->is_multiple(r_info.is_multiplex);

            r_desc[i_name]->connections().length(r_info.connections.size());
            ConnectionMap::iterator c_iter = r_info.connections.begin();
            CORBA::ULong i_cnx = 0;

            while (c_iter != r_info.connections.end())
            {
                ConnectionMap::value_type cnx = *c_iter;

                r_desc[i_name]->connections()[i_cnx] = new ConnectionDescription_impl();
                r_desc[i_name]->connections()[i_cnx]->ck(cnx.first.in());
                cnx.first->_add_ref();
                r_desc[i_name]->connections()[i_cnx]->objref(CORBA::Object::_duplicate(cnx.second.in()));

                i_cnx++;
                c_iter++;
            }            
        }
        else
        {
            // name not found => error
            throw Components::InvalidName();
        }
    }
    
    return r_desc._retn();
}

//
// IDL:omg.org/Components/Events/get_consumer:1.0
//
Components::EventConsumerBase_ptr
CCMObject_impl::get_consumer(const char* sink_name)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    check_is_removed();

    Components::EventConsumerBase_var result = Components::EventConsumerBase::_nil();

    if (!sink_name)
    {
        throw Components::InvalidName();
    }

    std::string s_name = sink_name;
    ConsumerMap::iterator c_iter = m_consumers.find(s_name);
    
    if (c_iter != m_consumers.end())
    {
        // sink exist
        ConsumerInfo c_info = m_consumers[s_name];
        CORBA::Object_var c_obj = get_object_ref(c_info.consumer.in());
        result = Components::EventConsumerBase::_narrow(c_obj.in());
    }
    else
    {
        // no event sink defined with this name for this component
        throw Components::InvalidName();
    }

    return result._retn();
}

//
// IDL:omg.org/Components/Events/subscribe:1.0
Components::Cookie*
CCMObject_impl::subscribe(const char* publisher_name,
                          Components::EventConsumerBase_ptr subscriber)
    throw(Components::InvalidName,
          Components::InvalidConnection,
          Components::ExceededConnectionLimit,
          CORBA::SystemException)
{
    check_is_removed();

    Components::Cookie_var cookie_value;

    if (!publisher_name)
    {
        throw Components::InvalidName();
    }

    if (CORBA::is_nil(subscriber))
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

    // find publisher
    std::string pub_name(publisher_name);
    PublisherMap::iterator pub_iter = m_publishers.find(pub_name);
    
    if (pub_iter != m_publishers.end())
    {
        // publisher_name exists
        PublisherInfo& pub_info = m_publishers[pub_name];

        // Check connection supports the interface declared in uses statement
        if (!subscriber->_is_a(pub_info.rep_id.c_str()))
        {
            throw Components::InvalidConnection();
        }

        // object has the right interface, insert new subscription in the list
        Cdmw::CCM::Common::Cookie_impl* cookie 
            = Cdmw::CCM::Common::Cookie_impl::create_cookie();
        cookie_value = cookie;

        // Subscribe consumer to the right event channel
        SubscriberInfo s_info;

        s_info.ccm_consumer 
            = Components::EventConsumerBase::_duplicate(subscriber);
        
        // test if subscriber is a cdmw subscriber or not (that is implement 
        // CosEventComm::PushConsumer)
        s_info.push_consumer  
            = CosEventComm::PushConsumer::_narrow(subscriber);
 
        // insert subscription now because we need to find it in lookup-servant
        // when narrow operation is called!
        pub_info.subscriptions.insert(SubscriberMap::value_type(cookie_value, 
                                                              s_info));
               
        SubscriberInfo& s_info_ref = pub_info.subscriptions[cookie_value];

        if (CORBA::is_nil(s_info_ref.push_consumer.in()))
        {
            // event_sink is not a CDMW one
            // Create my local push consumer to receive event 
            // this consumer will send to the EventConsumerBase.
            s_info_ref.push_consumer_servant 
               = new PushConsumer_impl(m_context, subscriber);

            std::string cookie_str = cookie->get_cookie_string();
            PRINT_INFO("cookie_str=" << cookie_str);
            std::string push_consumer_oid_str 
                = create_oid(m_component_oid, pub_name, cookie_str);
                
            CORBA::Object_var obj
                = m_context->create_ref_from_oid_str(push_consumer_oid_str, 
                                                     PUSH_CONSUMER_ID);
                
            s_info_ref.push_consumer 
                = CosEventComm::PushConsumer::_narrow(obj.in());
        }

        try
        {
            s_info_ref.proxy_supplier = pub_info.channel->connect_pushConsumer
               (s_info_ref.push_consumer.in());
        }
        catch (const CORBA::SystemException& )
        {
            pub_info.subscriptions.erase(cookie_value);
            PRINT_ERROR("Exception raised when connecting consumer to channel");
            throw;
        }
    }
    else
    {
        // no publisher named name exist!
        throw Components::InvalidName();
    }

    return cookie_value._retn();
}

//
// IDL:omg.org/Components/Events/unsubscribe:1.0
//
void
CCMObject_impl::unsubscribe(const char* publisher_name,
                            Components::Cookie* ck)
    throw(Components::InvalidName,
          Components::InvalidConnection,
          CORBA::SystemException)
{
    check_is_removed();

    if (!publisher_name)
    {
        throw Components::InvalidName();
    }

    if (!ck)
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

    // store ck in _var in order to manipulate only var 
    Components::Cookie_var ck_var = ck;
    CORBA::add_ref(ck_var);

    // find publisher
    std::string p_name(publisher_name);
    PublisherMap::iterator p_iter = m_publishers.find(p_name);
    
    if (p_iter != m_publishers.end())
    {
        // publisher_name exists
        PublisherInfo& p_info = m_publishers[p_name];

        SubscriberMap::iterator s_iter = p_info.subscriptions.find(ck_var);

        if (s_iter != p_info.subscriptions.end())
        {
           // Unsubscribe consumer from the event channel
            SubscriberInfo s_info = p_info.subscriptions[ck_var];

            s_info.proxy_supplier->disconnect_push_supplier();

            CdmwCcmCif::PushConsumer_var local_push_consumer
                = CdmwCcmCif::PushConsumer::_narrow(s_info.push_consumer.in());

            if (!CORBA::is_nil(local_push_consumer.in()))
            {
                // local_consumer created, remove it!
                local_push_consumer->remove();
            }

            // remove subscription
            p_info.subscriptions.erase(ck_var);
        }
        else
        {
            // no connection found 
            throw Components::InvalidConnection();
        }
    }
    else
    {
        // no publisher named publisher_name exist!
        throw Components::InvalidName();
    }
}

//
// IDL:omg.org/Components/Events/connect_consumer:1.0
//
void
CCMObject_impl::connect_consumer(const char* emitter_name,
                                 Components::EventConsumerBase_ptr consumer)
    throw(Components::InvalidName,
          Components::AlreadyConnected,
          Components::InvalidConnection,
          CORBA::SystemException)
{
    check_is_removed();

    if (!emitter_name)
    {
        throw Components::InvalidName();
    }

    if (CORBA::is_nil(consumer))
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);

    // find emitter
    std::string e_name(emitter_name);
    EmitterMap::iterator e_iter = m_emitters.find(e_name);
    
    if (e_iter != m_emitters.end())
    {
        // emitter_name exists
        EmitterInfo& e_info = m_emitters[e_name];

        // Check connection supports the interface declared in uses statement
        if (!consumer->_is_a(e_info.rep_id.c_str()))
        {
            throw Components::InvalidConnection();
        }

        if (CORBA::is_nil(e_info.ccm_consumer.in()))
        {
            // No consumer connected.

            e_info.ccm_consumer 
                = Components::EventConsumerBase::_duplicate(consumer);

            // test if subscriber is a cdmw subscriber or not (that is implement CosEventComm::PushConsumer)
            e_info.push_consumer  
                = CosEventComm::PushConsumer::_narrow(consumer);
            
            if (CORBA::is_nil(e_info.push_consumer.in()))
            {
                // event_sink is not a CDMW one
                // Create my local push consumer to receive event 
                // this consumer will send to the EventConsumerBase.
                e_info.push_consumer_servant = new PushConsumer_impl(m_context,
                                                                     consumer);

                std::string push_consumer_oid_str 
                    = create_oid(m_component_oid, 
                                 e_name, 
                                 PSEUDO_EMITTER_CK_VALUE);
                    
                CORBA::Object_var obj
                    = m_context->create_ref_from_oid_str(push_consumer_oid_str, 
                                                         PUSH_CONSUMER_ID);
                
                e_info.push_consumer 
                   = CosEventComm::PushConsumer::_narrow(obj.in());
            }
                          
            try
            {
                e_info.proxy_supplier = e_info.channel->connect_pushConsumer
                   (e_info.push_consumer.in());
            }
            catch (const CORBA::SystemException& )
            {
                PRINT_ERROR("Exception raised when connecting consumer to channel");
                throw;
            }
        }
        else
        {
            throw Components::AlreadyConnected();
        }
    }
    else
    {
        // no emitter named emitter_name exist!
        throw Components::InvalidName();
    }
}

//
// IDL:omg.org/Components/Events/disconnect_consumer:1.0
//
Components::EventConsumerBase_ptr
CCMObject_impl::disconnect_consumer(const char* source_name)
    throw(Components::InvalidName,
          Components::NoConnection,
          CORBA::SystemException)
{
    check_is_removed();

    Components::EventConsumerBase_var result = Components::EventConsumerBase::_nil();

    if (!source_name)
    {
        throw Components::InvalidName();
    }

    // find emitter
    std::string e_name(source_name);
    EmitterMap::iterator e_iter = m_emitters.find(e_name);
    
    if (e_iter != m_emitters.end())
    {
        // emitter_name exists
        EmitterInfo& e_info = m_emitters[e_name];

        if (CORBA::is_nil(e_info.ccm_consumer.in()))
        {
            throw Components::NoConnection();

        }
        else
        {
            // disconnect consumer from event channel
            e_info.proxy_supplier->disconnect_push_supplier();

            CdmwCcmCif::PushConsumer_var local_push_consumer
                = CdmwCcmCif::PushConsumer::_narrow(e_info.push_consumer.in());
            
            if (!CORBA::is_nil(local_push_consumer.in()))
            {
                // local_consumer created, remove it!
                local_push_consumer->remove();
            }

            // remove consumer
            result = Components::EventConsumerBase::_duplicate(e_info.ccm_consumer.in());
            e_info.ccm_consumer = Components::EventConsumerBase::_nil();
            e_info.push_consumer = CosEventComm::PushConsumer::_nil();
            e_info.push_consumer_servant = 0;

        }
    }
    else
    {
        // no emitter named emitter_name exist!
        throw Components::InvalidName();
    }

    return result._retn();
}

//
// IDL:omg.org/Components/Events/get_all_consumers:1.0
//
Components::ConsumerDescriptions*
CCMObject_impl::get_all_consumers()
    throw(CORBA::SystemException)
{
    check_is_removed();

    Components::NameList name_list;
    
    name_list.length(m_consumers.size());
    ConsumerMap::iterator iter = m_consumers.begin();
    CORBA::ULong i_name = 0;

    while (iter != m_consumers.end())
    {
        ConsumerMap::value_type consumer = *iter;

        name_list[i_name] = consumer.first.c_str();
        i_name++;
        iter++;
    }
    try
    {
        Components::ConsumerDescriptions_var c_desc 
            = get_named_consumers(name_list);

        return c_desc._retn();
    }
    catch (const Components::InvalidName &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNALComponentInvalidName, CORBA::COMPLETED_NO);
    }
}

//
// IDL:omg.org/Components/Events/get_named_consumers:1.0
//
Components::ConsumerDescriptions*
CCMObject_impl::get_named_consumers(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    check_is_removed();

    Components::ConsumerDescriptions_var c_desc = new Components::ConsumerDescriptions;
    c_desc->length(names.length());
    
    for (CORBA::ULong i_name=0; i_name<names.length(); i_name++) 
    {
        std::string c_name(names[i_name]);
        ConsumerMap::iterator c_iter = m_consumers.find(c_name);

        if (c_iter != m_consumers.end())
        {
            // consumer_name exists
            ConsumerInfo c_info = m_consumers[c_name];

            c_desc[i_name] = new ConsumerDescription_impl();

            c_desc[i_name]->name(c_name.c_str());
            c_desc[i_name]->type_id(c_info.rep_id.c_str());
            CORBA::Object_var c_obj = get_object_ref(c_info.consumer.in());
            c_desc[i_name]->consumer(Components::EventConsumerBase::_narrow(c_obj.in()));
        }
        else
        {
            // name not found => error
            throw Components::InvalidName();
        }
    }
    
    return c_desc._retn();
}

//
// IDL:omg.org/Components/Events/get_all_emitters:1.0
//
Components::EmitterDescriptions*
CCMObject_impl::get_all_emitters()
    throw(CORBA::SystemException)
{
    check_is_removed();

    Components::NameList name_list;
    
    name_list.length(m_emitters.size());
    EmitterMap::iterator iter = m_emitters.begin();
    CORBA::ULong i_name = 0;

    while (iter != m_emitters.end())
    {
        EmitterMap::value_type emitter = *iter;

        name_list[i_name] = emitter.first.c_str();
        i_name++;
        iter++;
    }
    try
    {
        Components::EmitterDescriptions_var e_desc 
            = get_named_emitters(name_list);

        return e_desc._retn();
    }
    catch (const Components::InvalidName &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNALComponentInvalidName, CORBA::COMPLETED_NO);
    }
}

//
// IDL:omg.org/Components/Events/get_named_emitters:1.0
//
Components::EmitterDescriptions*
CCMObject_impl::get_named_emitters(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    check_is_removed();

    Components::EmitterDescriptions_var e_desc = new Components::EmitterDescriptions;
    e_desc->length(names.length());
    
    for (CORBA::ULong i_name=0; i_name<names.length(); i_name++) 
    {
        std::string e_name(names[i_name]);
        EmitterMap::iterator e_iter = m_emitters.find(e_name);

        if (e_iter != m_emitters.end())
        {
            // emitter_name exists
            EmitterInfo e_info = m_emitters[e_name];

            e_desc[i_name] = new EmitterDescription_impl();

            e_desc[i_name]->name(e_name.c_str());
            e_desc[i_name]->type_id(e_info.rep_id.c_str());
            e_desc[i_name]->consumer(e_info.ccm_consumer.in());
        }
        else
        {
            // name not found => error
            throw Components::InvalidName();
        }
    }
    
    return e_desc._retn();
}

//
// IDL:omg.org/Components/Events/get_all_publishers:1.0
//
Components::PublisherDescriptions*
CCMObject_impl::get_all_publishers()
    throw(CORBA::SystemException)
{
    check_is_removed();

    Components::NameList name_list;
    
    name_list.length(m_publishers.size());
    PublisherMap::iterator iter = m_publishers.begin();
    CORBA::ULong i_name = 0;

    while (iter != m_publishers.end())
    {
        PublisherMap::value_type publisher = *iter;

        name_list[i_name] = publisher.first.c_str();
        i_name++;
        iter++;
    }
    try
    {
        Components::PublisherDescriptions_var p_desc 
            = get_named_publishers(name_list);

        return p_desc._retn();
    }
    catch (const Components::InvalidName &)
    {
        throw CORBA::INTERNAL(OrbSupport::INTERNALComponentInvalidName, CORBA::COMPLETED_NO);
    }
}

//
// IDL:omg.org/Components/Events/get_named_publishers:1.0
//
Components::PublisherDescriptions*
CCMObject_impl::get_named_publishers(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    check_is_removed();

    Components::PublisherDescriptions_var p_desc = new Components::PublisherDescriptions;
    p_desc->length(names.length());
    
    for (CORBA::ULong i_name=0; i_name<names.length(); i_name++) 
    {
        std::string p_name(names[i_name]);
        PublisherMap::iterator p_iter = m_publishers.find(p_name);

        if (p_iter != m_publishers.end())
        {
            // publisher_name exists
            PublisherInfo p_info = m_publishers[p_name];

            p_desc[i_name] = new PublisherDescription_impl();

            p_desc[i_name]->name(p_name.c_str());
            p_desc[i_name]->type_id(p_info.rep_id.c_str());

            p_desc[i_name]->consumers().length(p_info.subscriptions.size());
            SubscriberMap::iterator s_iter = p_info.subscriptions.begin();
            CORBA::ULong i_cons = 0;

            while (s_iter != p_info.subscriptions.end())
            {
                SubscriberMap::value_type sbc = *s_iter;

                p_desc[i_name]->consumers()[i_cons] 
                    = new SubscriberDescription_impl();
                p_desc[i_name]->consumers()[i_cons]->ck(sbc.first.in());
                p_desc[i_name]->consumers()[i_cons]->consumer(sbc.second.ccm_consumer.in());

                i_cons++;
                s_iter++;
            }            
        }
        else
        {
            // name not found => error
            throw Components::InvalidName();
        }
    }
    
    return p_desc._retn();
}


//////////////////////////
//                      //
// Internal Operations  //
//                      //
//////////////////////////

/**
* Purpose:
* <p>
*   add a facet in FacetMap
* 
*@param comp_oid     The component oid.
*@param facet_name   The facet name.
*@param rep_id       The facet repository id.
*
*@return  The reference to be exported to clients
*@exception AlreadyDoneException
*@exception InternalErrorException
*/ 

CORBA::Object_ptr 
CCMObject_impl::declare_facet(const std::string&  comp_oid,
                              const std::string&  facet_name,
                              const std::string&  rep_id)
    throw (AlreadyDoneException, InternalErrorException)
{
    FacetInfo facet_info;
    
    facet_info.rep_id = rep_id;
    facet_info.obj_ref = CORBA::Object::_nil();

    try 
    {      
        // check a facet with a same name has not been already declared
        if (m_facets.find(facet_name) == m_facets.end())
        {
            if (m_is_monolithic)
            {
                facet_info.obj_ref 
                   = m_context->create_ref_from_oid_str(comp_oid, 
                                                        m_rep_id);
            }
            else
            {
                std::string facet_oid_str 
                    = create_oid(comp_oid, facet_name);
                
                facet_info.obj_ref 
                    = m_context->create_ref_from_oid_str(facet_oid_str, rep_id);
            }

            m_facets.insert(FacetMap::value_type(facet_name, facet_info));
        }
        else
        {
            PRINT_ERROR2(facet_name, "already declared");
            CDMW_THROW(AlreadyDoneException);
        }
    }
    catch (const Components::IllegalState& )
    {
        PRINT_ERROR("IllegalState exception raised!");
        CDMW_THROW(InternalErrorException);
    }
    catch (const Components::BadComponentReference& )
    {
        PRINT_ERROR("BadComponentReference exception raised!");
        CDMW_THROW(InternalErrorException);
    }
    catch (const CORBA::SystemException& ex)
    {
        std::cerr << ex << std::endl; 
        CDMW_THROW(InternalErrorException);
    }

    return facet_info.obj_ref._retn();
}

/**
* Purpose:
* <p>
*   add a receptacle in ReceptacleMap
* 
*@param receptacle_name  The receptacle name.
*@param rep_id           The receptacle repository id.
*@param is_multiplex     This is set to true if receptacle is multiple
*
*@return void
*@exception AlreadyDoneException
*/ 
void 
CCMObject_impl::declare_receptacle(const std::string&  receptacle_name,
                                   const std::string&  rep_id,
                                   bool         is_multiplex)
    throw (AlreadyDoneException)
{
    ReceptacleInfo   receptacle_info;

    receptacle_info.rep_id = rep_id;
    receptacle_info.is_multiplex = is_multiplex;

    // check a receptacle with a same name has not been already declared
    if (m_receptacles.find(receptacle_name) == m_receptacles.end())
    {
        m_receptacles.insert(ReceptacleMap::value_type(receptacle_name, receptacle_info));
    }
    else
    {
        PRINT_ERROR2(receptacle_name, " already declared");
        CDMW_THROW(AlreadyDoneException);
    }
}

/**
* Purpose:
* <p>
*   add a consumer in ConsumerMap
* 
*@param consumer_name     The consumer name.
*@param rep_id            The consumer repository id.
*@param consumer_servant  The consumer servant
*
*@return  The reference to be exported to clients
*@exception AlreadyDoneException
*@exception InternalErrorException
*/ 

CORBA::Object_ptr 
CCMObject_impl::declare_consumer(const std::string&  comp_oid,
                                 const std::string&  consumer_name,
                                 const std::string&  rep_id)
    throw (AlreadyDoneException, InternalErrorException)
{
    ConsumerInfo consumer_info;
    
    consumer_info.rep_id = rep_id;
    consumer_info.consumer = CORBA::Object::_nil();

    try 
    {      
        // check a consumer with a same name has not been already declared
        if (m_consumers.find(consumer_name) == m_consumers.end())
        {
            if (m_is_monolithic)
            {
                consumer_info.consumer 
                    = m_context->create_ref_from_oid_str(comp_oid, m_rep_id);
            }
            else
            {
                std::string consumer_oid_str 
                    = create_oid(comp_oid, consumer_name);
                
                consumer_info.consumer
                    = m_context->create_ref_from_oid_str(consumer_oid_str, rep_id);
            }

            m_consumers.insert(ConsumerMap::value_type(consumer_name, 
                                                       consumer_info));
        }
        else
        {
            PRINT_ERROR2(consumer_name, "already declared");
            CDMW_THROW(AlreadyDoneException);
        }
    }
    catch (const Components::IllegalState& )
    {
        PRINT_ERROR("IllegalState exception raised!");
        CDMW_THROW(InternalErrorException);
    }
    catch (const Components::BadComponentReference& )
    {
        PRINT_ERROR("BadComponentReference exception raised!");
        CDMW_THROW(InternalErrorException);
    }
    catch (const CORBA::SystemException& ex)
    {
        std::cerr << ex << std::endl; 
        CDMW_THROW(InternalErrorException);
    }

    return consumer_info.consumer._retn();
}


/**
* Purpose:
* <p>
*   add an emitter in EmitterMap
* 
*@param emitter_name   The emitter name.
*@param rep_id         The emitter repository id.
*
*@return void
*@exception AlreadyDoneException
*/ 
void 
CCMObject_impl::declare_emitter(const std::string&  emitter_name,
                                const std::string&  rep_id)
    throw (AlreadyDoneException, CORBA::SystemException)
{
    check_is_removed();

    // check a emitter with a same name has not been already declared
    if (m_emitters.find(emitter_name) == m_emitters.end())
    {
        // store information concerning the emitter
        EmitterInfo   emitter_info;
        
        emitter_info.rep_id = rep_id;
        
        // create an event channel associated to this component instance and 
        // this port name
        try
        {
            Components::PortDescription_var port_desc 
                = new PortDescription_impl();
            port_desc->name(emitter_name.c_str());
            port_desc->type_id(rep_id.c_str());
            
            CosEventChannelAdmin::EventChannel_var event_channel =
                m_context->obtain_event_channel_for_emitter(port_desc.in());
            
            emitter_info.channel = new Event::EventChannelInterface(event_channel.in());
        }
        catch (const CdmwCcmContainer::NoChannel& )
        {
            PRINT_ERROR2(emitter_name, " event channel doesn't exist!");
            throw CORBA::INTERNAL(OrbSupport::INTERNALNoChannel, CORBA::COMPLETED_NO);
        }
        catch (const CORBA::SystemException& )
        {
            PRINT_ERROR("System exception raised during event channel creation!");
            throw;
        }
        
        try
        {
            emitter_info.proxy_consumer = emitter_info.channel->get_pushConsumer();
        }
        catch (const CORBA::SystemException& )
        {
            PRINT_ERROR("System exception raised during emitter connection to event channel!");
            throw;
        }
        
        // emitter info completed, store it in map
        m_emitters.insert(EmitterMap::value_type(emitter_name, emitter_info));
    }
    else
    {
        PRINT_ERROR2(emitter_name, " already declared");
        CDMW_THROW(AlreadyDoneException);
    }
}

/**
* Purpose:
* <p>
*   add a publisher in PublisherMap
* 
*@param publisher_name   The publisher name.
*@param rep_id           The publisher repository id.
*
*@return void
*@exception AlreadyDoneException
*/ 
void 
CCMObject_impl::declare_publisher(const std::string&  publisher_name,
                                  const std::string&  rep_id)
    throw (AlreadyDoneException, CORBA::SystemException)
{
    check_is_removed();

    // check a publisher with a same name has not been already declared
    if (m_publishers.find(publisher_name) == m_publishers.end())
    {    
        // store information concerning the pusblisher
        PublisherInfo   publisher_info;
        publisher_info.rep_id = rep_id;
        
        // create an event channel associated to this component instance and 
        // this port name
        try
        {
            Components::PortDescription_var port_desc 
                = new PortDescription_impl();
            port_desc->name(publisher_name.c_str());
            port_desc->type_id(rep_id.c_str());
            
            CosEventChannelAdmin::EventChannel_var event_channel =
                m_context->create_event_channel_for_publisher(port_desc.in());
            
            publisher_info.channel = new Event::EventChannelInterface(event_channel.in());
        }
        catch (const Components::CreateFailure& )
        {
            PRINT_ERROR2(publisher_name, "event channel creation failure");
            throw CORBA::INTERNAL(OrbSupport::INTERNALNoChannel, CORBA::COMPLETED_NO);
        }
        catch (const CORBA::SystemException& )
        {
            PRINT_ERROR("System exception raised during event channel creation!");
            throw;
        }
        
        try
        {
            publisher_info.proxy_consumer = publisher_info.channel->get_pushConsumer();
        }
        catch (const CORBA::SystemException& )
        {
            PRINT_ERROR("System exception raised during publisher connection to event channel!");
            throw;
        }

        // publisher info completed, store it in map
        m_publishers.insert(PublisherMap::value_type(publisher_name,
                                                     publisher_info));
    }
    else
    {
        PRINT_ERROR2(publisher_name, " already declared");
        CDMW_THROW(AlreadyDoneException);
    }    
}


/**
* Purpose:
* <p>This returns the proxy consumer for the event source specified by
* source_name. This operation has been created for internal use.
*
* @param source_name The event source name (publisher/emitter)
*
* @return the proxy consumer of the event service.
*/
CosEventComm::PushConsumer_ptr
CCMObject_impl::get_proxy_consumer(const std::string& source_name)
throw (CORBA::SystemException)
{
    check_is_removed();

    CosEventComm::PushConsumer_var consumer 
        = CosEventComm::PushConsumer::_nil();
    
    // find source name in publisher map and emitter map
    PublisherMap::iterator p_iter = m_publishers.find(source_name);
    
    if (p_iter != m_publishers.end())
    {
        // publisher_name exists
        PublisherInfo p_info = m_publishers[source_name];
        consumer = 
            CosEventComm::PushConsumer::_duplicate(p_info.proxy_consumer.in());
    }
    else
    {
        // find in emitter map
        EmitterMap::iterator e_iter = m_emitters.find(source_name);
        
        if (e_iter != m_emitters.end())
        {
            // emitter_name exists
            EmitterInfo e_info = m_emitters[source_name];
            consumer = 
                CosEventComm::PushConsumer::_duplicate(e_info.proxy_consumer.in());
        }
        else
        {
            // no emitter nor publisher exist!
            // nil is return
        }
    }
    
    return consumer._retn();
}

/**
* Purpose:
* <p>This returns the servant for the local push consumer specified by source_name.
*
* @param source_name The source name
*
* @return the local push consumer servant.
*/
PortableServer::Servant
CCMObject_impl::get_local_push_consumer_servant(const std::string& source_name, 
                                                const std::string& ck_value) 
throw (CORBA::SystemException)
{
    check_is_removed();

    PRINT_INFO("Entering in get_local_push_consumer_servant()");
    PRINT_INFO("source name = " << source_name);
    PRINT_INFO("ck_value = " << ck_value);

    if (ck_value != std::string(PSEUDO_EMITTER_CK_VALUE))
    {
        // look in publisher map
        PublisherMap::iterator p_iter = m_publishers.find(source_name);
        
        if (p_iter != m_publishers.end())
        {
            //  source_name is a publisher
            PublisherInfo& p_info = m_publishers[source_name];
            
            Components::Cookie_var ck = new Cdmw::CCM::Common::Cookie_impl(ck_value);
            SubscriberMap::iterator s_iter = p_info.subscriptions.find(ck);
            
            if (s_iter != p_info.subscriptions.end())
            {
                SubscriberInfo s_info = p_info.subscriptions[ck];
                
                if (s_info.push_consumer_servant.in())
                {
                    PortableServer::ServantBase_var consumer_servant
                        = s_info.push_consumer_servant;
                    return consumer_servant._retn();
                }
                else
                {
                    // No local push consumer with this name exist in this component
                    PRINT_ERROR("Local push consumer not found: does not exist for this connection.");
                    throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                        CORBA::COMPLETED_NO); 
                    
                }
            }
            else
            {
                // No local push consumer with this name exist in this component
                PRINT_ERROR("Local push consumer not found: no connexion exists in this component");
                throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                    CORBA::COMPLETED_NO); 
            }
        }
        else
        {
            // No local push consumer with this name exist in this component
            PRINT_ERROR("Local push consumer not found: no publisher exists in this component");
            throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                CORBA::COMPLETED_NO); 
        }
    }
    else
    {
        // look in emitter map
        EmitterMap::iterator e_iter = m_emitters.find(source_name);
        
        if (e_iter != m_emitters.end())
        {
            // source_name is an emitter
            EmitterInfo e_info = m_emitters[source_name];
            
           if (e_info.push_consumer_servant.in())
           {
               PortableServer::ServantBase_var consumer_servant
                   = e_info.push_consumer_servant;
               return consumer_servant._retn();
           }
           else
           {
               // No local push consumer with this name exist in this component
               PRINT_ERROR("Local push consumer not found: do not exists in this component for this emitter");
               throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                   CORBA::COMPLETED_NO); 

           }
        }
        else
        {
            // No local push consumer with this name exist in this component
            PRINT_ERROR("Local push consumer not found: no emitter exists in this component");
            throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                CORBA::COMPLETED_NO); 
        }
    }
}

/**
* Purpose:
* <p>
*   return an event consumer stored in PublisherMap at ck
* 
*@param publisher_name   The publisher name.
*@param ck               The cookie return at consumer subscription.
*
*@return a reference on the target consumer
*@exception Components::InvalidConnection if the connection does'nt exist
*/ 
Components::EventConsumerBase_ptr 
CCMObject_impl::get_consumer(const std::string&  publisher_name,
                             Components::Cookie* ck)
   throw(Components::InvalidName,
         Components::InvalidConnection, 
         CORBA::SystemException)
{
    check_is_removed();

    if (!ck)
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
    
    // find publisher
    Components::EventConsumerBase_var result = Components::EventConsumerBase::_nil();
    PublisherMap::iterator p_iter = m_publishers.find(publisher_name);
    
    if (p_iter != m_publishers.end())
    {
        // publisher_name exists
        PublisherInfo& p_info = m_publishers[publisher_name];
        
        // find subscription
        Components::Cookie_var ck_var = ck;
        CORBA::add_ref(ck_var);
        SubscriberMap::iterator s_iter = p_info.subscriptions.find(ck_var);
        
        if (s_iter != p_info.subscriptions.end())
        {
            SubscriberMap::value_type subscription = *s_iter;
            result = Components::EventConsumerBase::_duplicate(
                          subscription.second.ccm_consumer.in());
        }
        else
        {
            throw Components::InvalidConnection();
        }
    }
    else
    {
            throw Components::InvalidName();
    }

    return result._retn();
}

/**
* Purpose:
* <p>
*   return a group reference if the current request is invoked on a FT group reference.
*   Otherwise return the same reference.
* 
*@param obj_ref       The facet/consumer/component reference.
*
*@return  The reference to be exported to clients
*/ 
CORBA::Object_ptr 
CCMObject_impl::get_object_ref(const CORBA::Object_ptr obj_ref)
   throw (CORBA::SystemException)
{
    CORBA::Object_var result = CORBA::Object::_duplicate(obj_ref);
    return result._retn();
}


} // end namespace CIF
} // end namespace CCM
} // end namespace Cdmw

