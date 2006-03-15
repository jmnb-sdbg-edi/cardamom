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


#ifndef INCL_TEST_CDMW_CCM_CDMW_HELLO_HOME_IMPL_HPP
#define INCL_TEST_CDMW_CCM_CDMW_HELLO_HOME_IMPL_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <CCMContainer/ccmcontainer/ComponentServantProvider.hpp>
#include <LifeCycle/lifecycle/LifeCycleSingleObject.hpp>
#include "testccmcontainer/Cdmw_hello.skel.hpp"
#include <list>

namespace TestCdmwCCM
{
    
    /**
    * Purpose:
    * <p> 
    *
    */
    class CdmwHelloHome_impl : virtual public POA_Cdmw_HelloModule::HelloHome,
                                public Cdmw::CCM::Container::ComponentServantProvider,
                                public Cdmw::LifeCycle::LifeCycleSingleObject,
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
        CdmwHelloHome_impl(Components::HomeExecutorBase_ptr            home_executor,
                            const Cdmw::LifeCycle::ObjectDeactivator & deactivator,
                            const Components::ConfigValues &           config)
            throw(CORBA::SystemException);
        /**
        * Purpose:
        * <p> 
        *
        *@param component_context
        *@param home_executor
        */
        ~CdmwHelloHome_impl() throw();

        
        //
        // IDL:omg.org/Components/CCMHome/get_component_def:1.0
        //
        virtual CORBA::IRObject_ptr get_component_def()
            throw(CORBA::SystemException);

        //
        // IDL:omg.org/Components/CCMHome/get_home_def:1.0
        //
        virtual CORBA::IRObject_ptr get_home_def()
            throw(CORBA::SystemException);

        //
        // IDL:omg.org/Components/CCMHome/remove_component:1.0
        //
        virtual void remove_component(Components::CCMObject_ptr comp)
            throw(Components::RemoveFailure,
                  CORBA::SystemException);

        //
        // IDL:thalesgroup.com/HelloModule/HelloHomeImplicit/create:1.0
        //
        virtual HelloModule::HelloWorld_ptr create()
            throw(Components::CreateFailure,
                  CORBA::SystemException);

        //
        // IDL:omg.org/Components/KeylessCCMHome/create_component:1.0
        //
        virtual Components::CCMObject_ptr create_component()
            throw(Components::CreateFailure,
                  CORBA::SystemException);

        
        //
        // IDL:omg.org/Components/HomeConfiguration/set_configurator:1.0
        //
        virtual void set_configurator(Components::Configurator_ptr cfg)
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/HomeConfiguration/set_configuration_values:1.0
        //
        virtual void set_configuration_values(const Components::ConfigValues& config)
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/HomeConfiguration/complete_component_configuration:1.0
        //
        virtual void complete_component_configuration(CORBA::Boolean b)
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/HomeConfiguration/disable_home_configuration:1.0
        //
        virtual void disable_home_configuration()
            throw(CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
        //
        virtual CORBA::Boolean supports(const char* repository_id)
            throw(CORBA::SystemException);
        
        //
        // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
        //
        virtual CdmwLifeCycle::RepositoryIdSeq* get_supported_objects()
            throw(CORBA::SystemException);

        //
        // IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
        //
        virtual void remove()
            throw(CosLifeCycle::NotRemovable,
                  CORBA::SystemException);

        // From Cdmw::CCM::Container::ComponentServantProvider
        virtual
        void set_context(CdmwCcmContainer::CCM2Context_ptr ctxt)
            throw (Components::CCMException,
                   CORBA::SystemException);

        // From Cdmw::LifeCycle::ServantProviderBase
        virtual bool may_incarnate_multiple_objects() const throw();
        
        virtual PortableServer::Servant 
        lookup_servant(const PortableServer::ObjectId& oid) 
            throw(PortableServer::ForwardRequest,
                  CORBA::SystemException);
        virtual void 
        release_servant(const PortableServer::ObjectId& oid,
                        PortableServer::POA_ptr         poa,
                        PortableServer::Servant         servant) 
            throw(CORBA::SystemException);
        virtual std::string
        get_supported_interface(const PortableServer::ObjectId& oid) const
        throw(CORBA::OBJECT_NOT_EXIST);
    protected:
        virtual const char ** get_ids() const throw();
        virtual CORBA::Long   get_number_of_ids() const throw();
        
    private:
        CdmwHelloHome_impl();
        // Hide copy constructor/assignment operator
        CdmwHelloHome_impl(const CdmwHelloHome_impl& rhs)
            throw();
        
        CdmwHelloHome_impl&
        operator=(const CdmwHelloHome_impl& rhs)
            throw();

        HelloModule::CCM_HelloHome_var              m_home_executor;
        CdmwCcmContainer::Session2Context_var      m_session_context;
        std::list<HelloModule::HelloWorld_var>      m_ccm_objects;
    };


}; // End namespace TestCdmwCCM

#endif // INCL_TEST_CDMW_CCM_CDMW_HELLO_HOME_IMPL_HPP

