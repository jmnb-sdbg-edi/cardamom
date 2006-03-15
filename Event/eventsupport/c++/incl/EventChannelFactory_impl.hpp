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


#ifndef INCL_EVENT_EVENTCHANNELFACTORY_IMPL_HPP 
#define INCL_EVENT_EVENTCHANNELFACTORY_IMPL_HPP 

#include <string>
#include <map>

#include "Foundation/orbsupport/RefCountServantBase.hpp"
#include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"
#include "Foundation/orbsupport/POAThreadingPolicy.hpp"
#include "LifeCycle/lifecycle/FactoryAllocator.hpp"


#include "Event/idllib/CdmwEvent.skel.hpp"

/**
* Root namespace for Cdmw runtime.
*/

namespace Cdmw
{

/**
* Namespace for Cdmw Event types and data.
*/

namespace Event
{

/**
*Purpose:
*<p>    
* Implementation of the CdmwEvent::EventChannelFactory interface.
*
*Features:
*<p> 
*    None
*
*/
class EventChannelFactory_impl : virtual public POA_CdmwEvent::EventChannelFactory,
                                 virtual public PortableServer::RefCountServantBase

{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param orb A reference to the ORB pseudo-object
        *@param parent Parent POA for the factory
        *@param name_domain Object reference of the name domain where the
        * factory will register objects it creates
        *@param name_domain_name The fully qualified name of the NameDomain
        *@param factory_name Fully qualified name of the Cdmw factory
        *@param argc Number of additional arguments
        *@param argv Additional program arguments
        *@param use_profile_manager use an EventChannelProfileManager to get
        *       each event channel profile at creation. Default value is true.
        *       If false, each created event channel won't have a specific profile.s
        *
        *@exception Cdmw::BadParameterException Wrong parameter value, such as 
        * threading ... etc.
        *@exception Cdmw::InternalErrorException Internal Error
        *@exception CORBA::SystemException Any CORBA system exception
        */ 
        EventChannelFactory_impl(CORBA::ORB_ptr                           orb, 
                                 PortableServer::POA_ptr                  parent,
                                 CdmwLifeCycle::ObjectRegistration_ptr   name_domain,
                                 const std::string &                      name_domain_name,
                                 const std::string &                      factory_name,
                                 Cdmw::OrbSupport::POAThreadingPolicy &   threading_policy,
                                 int & argc, char** argv,
                                 bool use_profile_manager = true)
                throw(Cdmw::BadParameterException,
                      Cdmw::InternalErrorException,
                      CORBA::SystemException);

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~EventChannelFactory_impl()
                throw();

          
        /**
        * Purpose:
        * <p>  Return the POA with which the servant is activated
        */
        virtual PortableServer::POA_ptr _default_POA() 
        {
            return PortableServer::POA::_duplicate(m_poa.in());
        }

        /**
        * Purpose:
        * <p>  Create untyped event channel
        * 
        *@param name The event channel name
        *@return A reference on the created event channel
        *@exception CdmwLifeCycle::AlreadyExistant Event channel already exists
        *@exception CdmwLifeCycle::InvalidName  Name format invalid
        *@exception CdmwEvent::CreationError  Error occurred during event channel creation
        *@exception CORBA::SystemException  System exception
        */ 
        //
        // IDL:thalesgroup.com/CdmwEvent/EventChannelFactory/create_event_channel:1.0
        //
        virtual CosEventChannelAdmin::EventChannel_ptr create_event_channel(const char* name)
            throw(CdmwLifeCycle::AlreadyExistant,
                  CdmwLifeCycle::InvalidName,
                  CdmwEvent::CreationError,
                  CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
        //
        /**
        * Purpose:
        * <p> Supports the following objects:
        *     IDL:omg.org/CosEventChannelAdmin/EventChannel:1.0
        */
        virtual CORBA::Boolean supports(const char* repository_id)
            throw(CORBA::SystemException);
    
        //
        // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supported_objects:1.0
        //
        /**
        * Purpose:
        * <p> This operation returns a sequence of CORBA Repository IDs of Cdmw 
        * objects that can be created by the factory.
        *
        * @return A sequence of CORBA Repository IDs of Cdmw objects that 
        * can be created by the factory that is:
        * IDL:omg.org/CosEventChannelAdmin/EventChannel:1.0
        */ 
        virtual CdmwLifeCycle::RepositoryIdSeq* get_supported_objects()
            throw(CORBA::SystemException);

          
    protected:


    private:
        // Hide copy constructor/assignment operator
        EventChannelFactory_impl(const EventChannelFactory_impl& rhs)
                throw();

        EventChannelFactory_impl&
        operator=(const EventChannelFactory_impl& rhs)
                throw();

        /**
        *[Attribute description]
        */ 
        CORBA::ORB_var                           m_orb;         // A reference to the ORB pseudo-object
        PortableServer::POA_var                  m_poa;         // Parent POA for the factory
        CdmwLifeCycle::ObjectRegistration_var   m_domain;      //Object reference of the name domain
                                                                //where the factory will register
        std::string                              m_domain_name; // The fully qualified name of the 
                                                                                     //NameDomain
        std::string                              m_factory_name;// Fully qualified name of the Cdmw 
                                                                                     //factory
        PortableServer::POA_var                  m_notifier_poa;// Persistent POA of notifier
        PortableServer::POA_var                  m_internal_poa;// Non persistent POA for internal use

        // find created event channels by name
        typedef std::map<std::string, PortableServer::ServantBase_var , std::less<std::string> > ChannelMap;
        ChannelMap                               m_created_channels;
        
        bool                                     m_use_profile_manager;

}; // End class EventChannelFactory_impl 

/**
* Instanciates the factory allocator template for this factory.
*/
typedef Cdmw::LifeCycle::NamedObjectsFactoryAllocator<EventChannelFactory_impl> EventChannelFactory_impl_Allocator;

}; // End namespace Event
}; // End namespace Cdmw
#endif // INCL_EVENT_EVENTCHANNELFACTORY_IMPL_HPP


