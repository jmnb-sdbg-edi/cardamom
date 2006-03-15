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


#ifndef INCL_CDMW_CCM_CONTAINER_CCM_CONTEXT_FACTORY_HPP
#define INCL_CDMW_CCM_CONTAINER_CCM_CONTEXT_FACTORY_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include "CCMContainer/idllib/Components.stub.hpp"
#include "CCMContainer/idllib/CdmwCcmContainer.stub.hpp"
#include "CCMContainer/idllib/CdmwDeployment.stub.hpp"
#include "Event/idllib/CdmwEvent.stub.hpp"
#include "ccmcontainer/ValueTypeDependencyList.hpp"
#include <CCMContainer/ccmcontainer/ComponentServantProvider.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <string>

namespace Cdmw
{

namespace CCM
{
namespace Container
{
    /**
     * Purpose:
     * <p> A Factory of CCM Contexts
     */
    class CCMContextFactory
    {
    public:
        /**
        * Purpose:
        * <p> Created a CCM2Context object.
        *
        *@param orb  A reference to the ORB pseudo-object.
        *@param parent  A parent POA to the home instance-specific POA.
        *@param home Object reference to the component's home.
        *@param home_id Unique Identifier to a home within the context of its container.
        *@param servant_provider A component/facet servant provider.
        *@param component_kind Component Kind
        *@param servant_lifetime Servant Lifetime
        *@param threading_policy Threading Policy
        *@param home_registration A reference to a HomeRegistration for use to register component
        * homes so they may be located by the HomeFinder
        *@param executor_shlib_handle Handle of the component's shared/dynamically linked library
        *@param valuetype_dependencies A list of value type dependencies
        *
        *@return A reference to a CCM2Context object.
        *
        *@exception CORBA::SystemException TODO:
        */
        CdmwCcmContainer::CCM2Context_ptr
        create_CCMContext(CORBA::ORB_ptr                              orb,
                          PortableServer::POA_ptr                     parent,
                          Components::CCMHome_ptr                     home,
                          const std::string &                         home_id,
                          ComponentServantProvider*                   servant_provider,
                          CdmwDeployment::ComponentKindValue         component_kind,
                          CdmwDeployment::ServantLifetimeValue       servant_lifetime,
                          CdmwDeployment::ThreadingPolicyValue       threading_policy,
                          Components::HomeRegistration_ptr            home_registration,
                          CdmwEvent::EventChannelFactory_ptr         event_channel_factory,
                          const std::string &                         process_name,
                          const std::string &                         application_name,
                          Cdmw::OsSupport::OS::SharedObjectHandle*   executor_shlib_handle,
                          const ValueTypeDependencyList &             valuetype_dependencies)
            throw (CORBA::SystemException);


        /**
         * We use a static instance
         */
        static CCMContextFactory TheInstance;
       
    protected:
        CCMContextFactory() {}
        virtual ~CCMContextFactory() throw() {}
    private:
        CCMContextFactory(const CCMContextFactory&);
    };
}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_CCM_CONTEXT_FACTORY_HPP

