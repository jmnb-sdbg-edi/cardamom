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


#ifndef INCL_TEST_CDMW_CCM_HELLO_WORLD_IMPL_HPP
#define INCL_TEST_CDMW_CCM_HELLO_WORLD_IMPL_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include "testccmcontainer/hello.skel.hpp"
#include "CCMContainer/idllib/CdmwCcmContainer.stub.hpp"

namespace TestCdmwCCM
{
    
    /**
    * Purpose:
    * <p> 
    *
    */
    class HelloWorld_impl : virtual public POA_HelloModule::HelloWorld,
                            virtual public Cdmw::OrbSupport::RefCountServantBase
    {
    public:
        /**
        * Purpose:
        * <p> 
        *
        *@param component_context
        *@param home_executor
        */
        HelloWorld_impl(CdmwCcmContainer::CCM2Context_ptr ctx)
             throw(CORBA::SystemException);
        /**
        * Purpose:
        * <p> 
        *
        *@param component_context
        *@param home_executor
        */
        ~HelloWorld_impl() throw();

        
        //
        // IDL:omg.org/Components/CCMObject/get_component_def:1.0
        //
        virtual CORBA::IRObject_ptr get_component_def()
            throw(CORBA::SystemException);

        //
        // IDL:omg.org/Components/CCMObject/get_ccm_home:1.0
        //
        virtual Components::CCMHome_ptr get_ccm_home()
            throw(CORBA::SystemException);

        //
        // IDL:omg.org/Components/CCMObject/get_primary_key:1.0
        //
        virtual Components::PrimaryKeyBase* get_primary_key()
            throw(Components::NoKeyAvailable,
                  CORBA::SystemException);

        //
        // IDL:omg.org/Components/CCMObject/configuration_complete:1.0
        //
        virtual void configuration_complete()
            throw(Components::InvalidConfiguration,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/CCMObject/remove:1.0
        //
        virtual void remove()
            throw(Components::RemoveFailure,
                  CORBA::SystemException);

        //
        // IDL:omg.org/Components/CCMObject/get_all_ports:1.0
        //
        virtual Components::ComponentPortDescription* get_all_ports()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Navigation/provide_facet:1.0
        //
        virtual CORBA::Object_ptr provide_facet(const char* name)
            throw(Components::InvalidName,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Navigation/get_all_facets:1.0
        //
        virtual Components::FacetDescriptions* get_all_facets()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Navigation/get_named_facets:1.0
        //
        virtual Components::FacetDescriptions* get_named_facets(const Components::NameList& names)
            throw(Components::InvalidName,
                  CORBA::SystemException);

        //
        // IDL:omg.org/Components/Navigation/same_component:1.0
        //
        virtual CORBA::Boolean same_component(CORBA::Object_ptr object_ref)
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Receptacles/connect:1.0
        //
        virtual Components::Cookie* connect(const char* name,
                                            CORBA::Object_ptr connection)
            throw(Components::InvalidName,
                  Components::InvalidConnection,
                  Components::AlreadyConnected,
                  Components::ExceededConnectionLimit,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Receptacles/disconnect:1.0
        //
        virtual void disconnect(const char* name,
                                Components::Cookie* ck)
            throw(Components::InvalidName,
                  Components::InvalidConnection,
                  Components::CookieRequired,
                  Components::NoConnection,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Receptacles/get_connections:1.0
        //
        virtual Components::ConnectionDescriptions* get_connections(const char* name)
            throw(Components::InvalidName,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Receptacles/get_all_receptacles:1.0
        //
        virtual Components::ReceptacleDescriptions* get_all_receptacles()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Receptacles/get_named_receptacles:1.0
        //
        virtual Components::ReceptacleDescriptions* get_named_receptacles(const Components::NameList& names)
            throw(Components::InvalidName,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/get_consumer:1.0
        //
        virtual Components::EventConsumerBase_ptr get_consumer(const char* sink_name)
            throw(Components::InvalidName,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/subscribe:1.0
        //
        virtual Components::Cookie* subscribe(const char* publisher_name,
                                              Components::EventConsumerBase_ptr subscriber)
            throw(Components::InvalidName,
                  Components::InvalidConnection,
                  Components::ExceededConnectionLimit,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/unsubscribe:1.0
        //
        virtual void unsubscribe(const char* publisher_name,
                                 Components::Cookie* ck)
            throw(Components::InvalidName,
                  Components::InvalidConnection,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/connect_consumer:1.0
        //
        virtual void connect_consumer(const char* emitter_name,
                                      Components::EventConsumerBase_ptr consumer)
            throw(Components::InvalidName,
                  Components::AlreadyConnected,
                  Components::InvalidConnection,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/disconnect_consumer:1.0
        //
        virtual Components::EventConsumerBase_ptr disconnect_consumer(const char* source_name)
            throw(Components::InvalidName,
                  Components::NoConnection,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/get_all_consumers:1.0
        //
        virtual Components::ConsumerDescriptions* get_all_consumers()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/get_named_consumers:1.0
        //
        virtual Components::ConsumerDescriptions* get_named_consumers(const Components::NameList& names)
            throw(Components::InvalidName,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/get_all_emitters:1.0
        //
        virtual Components::EmitterDescriptions* get_all_emitters()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/get_named_emitters:1.0
        //
        virtual Components::EmitterDescriptions* get_named_emitters(const Components::NameList& names)
            throw(Components::InvalidName,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/get_all_publishers:1.0
        //
        virtual Components::PublisherDescriptions* get_all_publishers()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Events/get_named_publishers:1.0
        //
        virtual Components::PublisherDescriptions* get_named_publishers(const Components::NameList& names)
            throw(Components::InvalidName,
                  CORBA::SystemException);

        
        //
        // IDL:thalesgroup.com/HelloModule/Hello/say_hello:1.0
        //
        virtual void say_hello(HelloModule::Message* msg)
            throw(CORBA::SystemException);
        
        //
        // IDL:thalesgroup.com/HelloModule/Hello/log_message:1.0
        //
        virtual void log_message(HelloModule::TraceMessage* msg)
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>This checks whether the CORBA object incarnated by the current
         * object (this) has been removed (<I>remove</I> operation has been 
         * called).
         *
         *@return <B>true</B> if the object has been removed, <B>false</B> otherwise.
         */ 
        bool is_removed() const throw()
        {
            return m_is_removed == true;
        }
            
        
    private:
        HelloWorld_impl();
        // Hide copy constructor/assignment operator
        HelloWorld_impl(const HelloWorld_impl& rhs)
            throw();
        
        HelloWorld_impl&
        operator=(const HelloWorld_impl& rhs)
            throw();

        CdmwCcmContainer::CCM2Context_var m_context;
        bool                               m_is_removed;
    };


}; // End namespace TestCdmwCCM

#endif // INCL_TEST_CDMW_CCM_HELLO_WORLD_IMPL_HPP

