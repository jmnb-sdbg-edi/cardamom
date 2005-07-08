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


#include "testccmcontainer/HelloWorld_impl.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "testccmcontainer/MacroDefs.hpp"
#include <iostream>

namespace TestCdmwCCM
{

//
// IDL:CCMObject:1.0
//
HelloWorld_impl::HelloWorld_impl(CdmwCcmContainer::CCM2Context_ptr ctx)
    throw(CORBA::SystemException)
    : m_context(CdmwCcmContainer::CCM2Context::_duplicate(ctx)),
      m_is_removed(false)
{
    ECHO_CCMCONTAINER << "HelloWorld_impl::HelloWorld_impl() created." << std::endl;
}

HelloWorld_impl::~HelloWorld_impl()
    throw()
{
    ECHO_CCMCONTAINER << "HelloWorld_impl::~HelloWorld_impl() called." << std::endl;
}


//
// IDL:omg.org/Components/CCMObject/get_component_def:1.0
//
CORBA::IRObject_ptr
HelloWorld_impl::get_component_def()
    throw(CORBA::SystemException)
{
    return CORBA::IRObject::_nil();
}

//
// IDL:omg.org/Components/CCMObject/get_ccm_home:1.0
//
Components::CCMHome_ptr
HelloWorld_impl::get_ccm_home()
    throw(CORBA::SystemException)
{
    return m_context->get_CCM_home();
}

//
// IDL:omg.org/Components/CCMObject/get_primary_key:1.0
//
Components::PrimaryKeyBase*
HelloWorld_impl::get_primary_key()
    throw(Components::NoKeyAvailable,
          CORBA::SystemException)
{
    throw Components::NoKeyAvailable();
}

//
// IDL:omg.org/Components/CCMObject/configuration_complete:1.0
//
void
HelloWorld_impl::configuration_complete()
    throw(Components::InvalidConfiguration,
          CORBA::SystemException)
{
}

//
// IDL:omg.org/Components/CCMObject/remove:1.0
//
void
HelloWorld_impl::remove()
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    try {
        if (is_removed())
            throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                          CORBA::COMPLETED_NO);
        // deactivate servant
        m_context->req_passivate();
            
        // register object as removed
        m_is_removed=true;
    } catch (const CORBA::SystemException & ) {
        // rethrow
        throw;
    } catch (...) {
        throw Components::RemoveFailure();
    }

}

//
// IDL:omg.org/Components/CCMObject/get_all_ports:1.0
//
Components::ComponentPortDescription*
HelloWorld_impl::get_all_ports()
    throw(CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Navigation/provide_facet:1.0
//
CORBA::Object_ptr
HelloWorld_impl::provide_facet(const char* name)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    return CORBA::Object::_nil();;
}

//
// IDL:omg.org/Components/Navigation/get_all_facets:1.0
//
Components::FacetDescriptions*
HelloWorld_impl::get_all_facets()
    throw(CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Navigation/get_named_facets:1.0
//
Components::FacetDescriptions*
HelloWorld_impl::get_named_facets(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Navigation/same_component:1.0
//
CORBA::Boolean
HelloWorld_impl::same_component(CORBA::Object_ptr object_ref)
    throw(CORBA::SystemException)
{
    CdmwCcmContainer::Session2Context_var session
        = CdmwCcmContainer::Session2Context::_narrow(m_context.in());
    if (CORBA::is_nil(session.in()))
        throw CORBA::INTERNAL();
    
    CORBA::Object_var obj = session ->get_CCM_object();
    return obj->_is_equivalent(object_ref);
}

//
// IDL:omg.org/Components/Receptacles/connect:1.0
//
Components::Cookie*
HelloWorld_impl::connect(const char* name,
                          CORBA::Object_ptr connection)
    throw(Components::InvalidName,
          Components::InvalidConnection,
          Components::AlreadyConnected,
          Components::ExceededConnectionLimit,
          CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Receptacles/disconnect:1.0
//
void
HelloWorld_impl::disconnect(const char* name,
                           Components::Cookie* ck)
    throw(Components::InvalidName,
          Components::InvalidConnection,
          Components::CookieRequired,
          Components::NoConnection,
          CORBA::SystemException)
{
}

//
// IDL:omg.org/Components/Receptacles/get_connections:1.0
//
Components::ConnectionDescriptions*
HelloWorld_impl::get_connections(const char* name)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Receptacles/get_all_receptacles:1.0
//
Components::ReceptacleDescriptions*
HelloWorld_impl::get_all_receptacles()
    throw(CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Receptacles/get_named_receptacles:1.0
//
Components::ReceptacleDescriptions*
HelloWorld_impl::get_named_receptacles(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Events/get_consumer:1.0
//
Components::EventConsumerBase_ptr
HelloWorld_impl::get_consumer(const char* sink_name)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Events/subscribe:1.0
Components::Cookie*
HelloWorld_impl::subscribe(const char* publisher_name,
                            Components::EventConsumerBase_ptr subscriber)
    throw(Components::InvalidName,
          Components::InvalidConnection,
          Components::ExceededConnectionLimit,
          CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Events/unsubscribe:1.0
//
void
HelloWorld_impl::unsubscribe(const char* publisher_name,
                              Components::Cookie* ck)
    throw(Components::InvalidName,
          Components::InvalidConnection,
          CORBA::SystemException)
{
}

//
// IDL:omg.org/Components/Events/connect_consumer:1.0
//
void
HelloWorld_impl::connect_consumer(const char* emitter_name,
                                   Components::EventConsumerBase_ptr consumer)
    throw(Components::InvalidName,
          Components::AlreadyConnected,
          Components::InvalidConnection,
          CORBA::SystemException)
{
}

//
// IDL:omg.org/Components/Events/disconnect_consumer:1.0
//
Components::EventConsumerBase_ptr
HelloWorld_impl::disconnect_consumer(const char* source_name)
    throw(Components::InvalidName,
          Components::NoConnection,
          CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Events/get_all_consumers:1.0
//
Components::ConsumerDescriptions*
HelloWorld_impl::get_all_consumers()
    throw(CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Events/get_named_consumers:1.0
//
Components::ConsumerDescriptions*
HelloWorld_impl::get_named_consumers(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Events/get_all_emitters:1.0
//
Components::EmitterDescriptions*
HelloWorld_impl::get_all_emitters()
    throw(CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Events/get_named_emitters:1.0
//
Components::EmitterDescriptions*
HelloWorld_impl::get_named_emitters(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Events/get_all_publishers:1.0
//
Components::PublisherDescriptions*
HelloWorld_impl::get_all_publishers()
    throw(CORBA::SystemException)
{
    return 0;
}

//
// IDL:omg.org/Components/Events/get_named_publishers:1.0
//
Components::PublisherDescriptions*
HelloWorld_impl::get_named_publishers(const Components::NameList& names)
    throw(Components::InvalidName,
          CORBA::SystemException)
{
    return 0;
}
        

//
// IDL:thalesgroup.com/HelloModule/Hello/say_hello:1.0
//
void HelloWorld_impl::say_hello(HelloModule::Message* msg)
    throw(CORBA::SystemException)
{
    std::cout << "Hello world :-)  message is '"  << msg->the_message()  << "'" << std::endl;
}
//
// IDL:thalesgroup.com/HelloModule/Hello/log_message:1.0
//
void HelloWorld_impl::log_message(HelloModule::TraceMessage* msg)
    throw(CORBA::SystemException)
{
    std::cout << "Hello world :-)  Trace message is '"  << msg->the_message()  << "' level is "
              << msg->the_level() << std::endl;
}

}; // End namespace TestCdmwCCM


