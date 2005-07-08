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


package cdmw.ccm.container;

import cdmw.orbsupport.ExceptionMinorCodes;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;

/**
 * A Factory of CCM Contexts
 *
 */
public class CCMContextFactory {

    /**
     * We use a static instance
     */
    public final static CCMContextFactory TheInstance = new CCMContextFactory();

    /**
     * Created a CCM2Context object.
     *
     * @param orb  A reference to the ORB pseudo-object.
     * @param parent  A parent POA to the home instance-specific POA.
     * @param home Object reference to the component's home.
     * @param homeId Unique Identifier to a home
     * within the context of its container.
     * @param servantProvider A component/facet servant provider.
     * @param componentKind Component Kind
     * @param servantLifetime Servant Lifetime
     * @param threadingPolicy Threading Policy
     * @param homeRegistration A reference to a HomeRegistration for use
     * to register component homes so they may be located by the HomeFinder.
     *
     * @return A reference to a CCM2Context object.
     */
    public com.thalesgroup.CdmwCcmContainer.CCM2Context createCCMContext(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        org.omg.Components.CCMHome home,
        String homeId,
        ComponentServantProviderInterface servantProvider,
        ComponentKindValue componentKind,
        com.thalesgroup.CdmwDeployment.ServantLifetimeValue servantLifetime,
        com.thalesgroup.CdmwDeployment.ThreadingPolicyValue threadingPolicy,
        org.omg.Components.HomeRegistration homeRegistration,
        com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory,
        String processName,
        String applicationName) {

        // This operation is called at home installation.
        // Context objects encapsulate a whole lifecycle
        // framework for components and components' facets managed by this home.
        //
        // A lifecycle framework contains the following actors:
        //   A - *) An object factory ( the home created by the container)
        //   B - *) An object adapter
        //   C - *) A servant manager
        //   D - *) A servant provider
        //

        // Only Session container type is supported!
        if (componentKind.value() != ComponentKindValue.SERVICE.value() &&
            componentKind.value() != ComponentKindValue.SESSION.value()) {
            throw new org.omg.CORBA.NO_IMPLEMENT(
                ExceptionMinorCodes.NO_IMPLEMENTComponentKindNotSupported,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

        return new Session2ContextImpl(
            orb,
            parent,
            home,
            homeId,
            servantProvider,
            servantLifetime,
            threadingPolicy,
            homeRegistration,
            eventChannelFactory,
            processName,
            applicationName);

    }

}

