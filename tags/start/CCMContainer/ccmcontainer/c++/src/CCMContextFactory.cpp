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


#include "ccmcontainer/CCMContextFactory.hpp"
#include <Foundation/common/Assert.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <LifeCycle/lifecycle/ServantProviderBase.hpp>
#include "ccmcontainer/Session2Context_impl.hpp"


namespace Cdmw
{
namespace CCM
{
namespace Container
{

CCMContextFactory CCMContextFactory::TheInstance;


CdmwCcmContainer::CCM2Context_ptr
CCMContextFactory::create_CCMContext(CORBA::ORB_ptr                            orb,
                                     PortableServer::POA_ptr                   parent,
                                     Components::CCMHome_ptr                   home,
                                     const std::string &                       home_id,
                                     ComponentServantProvider*                 servant_provider,
                                     CdmwDeployment::ComponentKindValue       component_kind,
                                     CdmwDeployment::ServantLifetimeValue     servant_lifetime,
                                     CdmwDeployment::ThreadingPolicyValue     threading_policy,
                                     Components::HomeRegistration_ptr          home_registration,
                                     CdmwEvent::EventChannelFactory_ptr       event_channel_factory,
                                     const std::string &                       process_name,
                                     const std::string &                       application_name,
                                     Cdmw::OsSupport::OS::SharedObjectHandle*  executor_shlib_handle,
                                     const ValueTypeDependencyList &           valuetype_dependencies)
    throw (CORBA::SystemException)
{
    // This operation is called at home installation. Context objects encapsulate a whole lifecycle
    // framework for components and components' facets managed by this home.
    //
    // A lifecycle framework contains the following actors:
    //   A - *) An object factory ( the home created by the container)
    //   B - *) An object adapter
    //   C - *) A servant manager
    //   D - *) A servant provider
    //

    // Only Session container type is supported!
    if ((component_kind != CdmwDeployment::SERVICE) &&
        (component_kind != CdmwDeployment::SESSION))
        throw CORBA::NO_IMPLEMENT(Cdmw::OrbSupport::NO_IMPLEMENTComponentKindNotSupported,
                                  CORBA::COMPLETED_NO);
        
    return new Session2Context_impl(orb,
                                    parent,
                                    home,
                                    home_id,
                                    servant_provider,
                                    servant_lifetime,
                                    threading_policy,
                                    home_registration,
                                    event_channel_factory,
                                    process_name,
                                    application_name,
                                    executor_shlib_handle,
                                    valuetype_dependencies);
}

    
}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

