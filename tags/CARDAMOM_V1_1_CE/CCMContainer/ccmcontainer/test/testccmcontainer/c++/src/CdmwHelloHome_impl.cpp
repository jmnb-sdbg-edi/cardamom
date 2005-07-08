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


#include "testccmcontainer/CdmwHelloHome_impl.hpp"
#include "testccmcontainer/HelloWorld_impl.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "testccmcontainer/MacroDefs.hpp"
#include <iostream>
#include <algorithm>
#include <functional>


namespace
{
    
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

struct ComponentRemover
{
    void operator()(const HelloModule::HelloWorld_var & hello) const
        throw()
    {
        try {
            hello->remove();
        } catch (...) {
            // ignore...
        }
    }
};

}; // End of anonymous namespace
namespace TestCdmwCCM
{
    

CdmwHelloHome_impl::CdmwHelloHome_impl(Components::HomeExecutorBase_ptr            home_executor,
                                       const Cdmw::LifeCycle::ObjectDeactivator & deactivator,
                                       const Components::ConfigValues &           config)
    throw(CORBA::SystemException)
    : Cdmw::LifeCycle::LifeCycleSingleObject(deactivator),
      m_home_executor(HelloModule::CCM_HelloHome::_narrow(home_executor)),
      m_session_context(CdmwCcmContainer::Session2Context::_nil())
{
    ECHO_CCMCONTAINER << "CdmwHelloHome_impl::CdmwHelloHome_impl() created." << std::endl;
    CDMW_ASSERT(!CORBA::is_nil(m_home_executor.in()));
}

CdmwHelloHome_impl::~CdmwHelloHome_impl() throw()
{
    ECHO_CCMCONTAINER << "CdmwHelloHome_impl::~CdmwHelloHome_impl() called." << std::endl;
}


//
// IDL:omg.org/Components/CCMHome/get_component_def:1.0
//
CORBA::IRObject_ptr
CdmwHelloHome_impl::get_component_def()
    throw(CORBA::SystemException)
{
    return CORBA::IRObject::_nil();
}

//
// IDL:omg.org/Components/CCMHome/get_home_def:1.0
//
CORBA::IRObject_ptr
CdmwHelloHome_impl::get_home_def()
    throw(CORBA::SystemException)
{
    return CORBA::IRObject::_nil();
}

//
// IDL:omg.org/Components/CCMHome/remove_component:1.0
//
void
CdmwHelloHome_impl::remove_component(Components::CCMObject_ptr comp)
    throw(Components::RemoveFailure,
          CORBA::SystemException)
{
    HelloModule::HelloWorld_var hello = HelloModule::HelloWorld::_narrow(comp);
    if (CORBA::is_nil(hello.in()))
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMNilObjectReference,
                               CORBA::COMPLETED_NO);
    
    std::list<HelloModule::HelloWorld_var>::iterator i =
        std::find_if(m_ccm_objects.begin(),
                     m_ccm_objects.end(),
                     std::bind1st (is_equivalent_to<HelloModule::HelloWorld_var>(),
                                   hello));
    if (i != m_ccm_objects.end()) {
        (*i)->remove();
        m_ccm_objects.erase(i);
    } else 
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMUnknownComponentReference,
                               CORBA::COMPLETED_NO);
}


//
// IDL:thalesgroup.com/HelloModule/HelloHomeImplicit/create:1.0
//
HelloModule::HelloWorld_ptr
CdmwHelloHome_impl::create()
    throw(Components::CreateFailure,
          CORBA::SystemException)
{

    static const char* repid= "IDL:thalesgroup.com/HelloModule/HelloWorld:1.0";

    CORBA::Object_var obj = m_session_context->create_ref(repid);
    HelloModule::HelloWorld_var comp = HelloModule::HelloWorld::_narrow(obj.in());
    m_ccm_objects.push_back(comp);
    return comp._retn();
}
//
// IDL:omg.org/Components/KeylessCCMHome/create_component:1.0
//
Components::CCMObject_ptr
CdmwHelloHome_impl::create_component()
    throw(Components::CreateFailure,
          CORBA::SystemException)
{
    return create();
}

//
// IDL:omg.org/Components/HomeConfiguration/set_configurator:1.0
//
void CdmwHelloHome_impl::set_configurator(Components::Configurator_ptr cfg)
    throw(CORBA::SystemException)
{
}
        
//
// IDL:omg.org/Components/HomeConfiguration/set_configuration_values:1.0
//
void CdmwHelloHome_impl::set_configuration_values(const Components::ConfigValues& config)
    throw(CORBA::SystemException)
{
}

//
// IDL:omg.org/Components/HomeConfiguration/complete_component_configuration:1.0
//
void CdmwHelloHome_impl::complete_component_configuration(CORBA::Boolean b)
    throw(CORBA::SystemException)
{
}
        
//
// IDL:omg.org/Components/HomeConfiguration/disable_home_configuration:1.0
//
void CdmwHelloHome_impl::disable_home_configuration()
    throw(CORBA::SystemException)
{
}

        

//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
//
CORBA::Boolean
CdmwHelloHome_impl::supports(const char* repository_id)
    throw(CORBA::SystemException)
{
    std::string repid(repository_id);
    return Cdmw::LifeCycle::ServantProviderBase::supports(repid);
}
        
//
// IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
//
CdmwLifeCycle::RepositoryIdSeq*
CdmwHelloHome_impl::get_supported_objects()
    throw(CORBA::SystemException)
{
    return Cdmw::LifeCycle::ServantProviderBase::get_supported_objects();
}

//
// IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
//
void CdmwHelloHome_impl::remove()
    throw(CosLifeCycle::NotRemovable,
          CORBA::SystemException)
{
    // Remove All components
    std::for_each(m_ccm_objects.begin(),m_ccm_objects.end(),ComponentRemover());
    m_ccm_objects.clear();
    Cdmw::LifeCycle::LifeCycleSingleObject::remove();
    // release context
    m_session_context = CdmwCcmContainer::Session2Context::_nil();
}


// From Cdmw::CCM::Container::ComponentServantProvider
void CdmwHelloHome_impl::set_context(CdmwCcmContainer::CCM2Context_ptr ctxt)
    throw (Components::CCMException,
           CORBA::SystemException)
{
    m_session_context = CdmwCcmContainer::Session2Context::_narrow(ctxt);
}

// From Cdmw::LifeCycle::ServantProviderBase
bool
CdmwHelloHome_impl::may_incarnate_multiple_objects() const
    throw()
{
    return false;
}

PortableServer::Servant 
CdmwHelloHome_impl::lookup_servant(const PortableServer::ObjectId& oid) 
    throw(PortableServer::ForwardRequest,
          CORBA::SystemException)
{
    CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
    ECHO_CCMCONTAINER << "Looking for component <" << s.in() << "> ..." << std::endl;
    ECHO_CCMCONTAINER << "Creating component executor..." << std::endl;
    ::Components::EnterpriseComponent_var exec =  m_home_executor->create();
    
    PortableServer::ServantBase_var servant = new HelloWorld_impl(m_session_context.in());
    return servant._retn();
}

void 
CdmwHelloHome_impl::release_servant(const PortableServer::ObjectId& oid,
                                     PortableServer::POA_ptr         poa,
                                     PortableServer::Servant         servant) 
    throw(CORBA::SystemException)
{
    CORBA::String_var s = PortableServer::ObjectId_to_string(oid);
    ECHO_CCMCONTAINER << "Releasing component <" << s.in() << "> ..." << std::endl;
    HelloWorld_impl* hello = dynamic_cast<HelloWorld_impl*>(servant);
    if (hello && hello->is_removed()) {
        ECHO_CCMCONTAINER << "Component <" << s.in() << "> is removed!" << std::endl;
    }
    servant->_remove_ref();
}

std::string
CdmwHelloHome_impl::get_supported_interface(const PortableServer::ObjectId& oid) const
    throw(CORBA::OBJECT_NOT_EXIST)
{
    return "";
}

const char **
CdmwHelloHome_impl::get_ids() const
    throw()
{
    static const char* ids[] = {
      "IDL:thalesgroup.com/Cdmw_HelloModule/HelloHome:1.0",
      "IDL:thalesgroup.com/HelloModule/HelloHome:1.0",
      "IDL:thalesgroup.com/HelloModule/HelloHomeImplicit:1.0",
      "IDL:thalesgroup.com/HelloModule/HelloHomeExplicit:1.0",
      "IDL:omg.org/Components/KeylessCCMHome:1.0",
      "IDL:omg.org/Components/CCMHome:1.0",
      "IDL:omg.org/Components/HomeConfiguration:1.0",
      "IDL:thalesgroup.com/CdmwCcmCif/CCMHome:1.0",
      "IDL:omg.org/CosLifeCycle/LifeCycleObject:1.0",
      "IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase:1.0",
    0
    };
    return ids;
}

CORBA::Long
CdmwHelloHome_impl::get_number_of_ids() const
    throw()
{
    static const CORBA::Long nb = 10;

    return nb;
}



}; // End namespace TestCdmwCCM

