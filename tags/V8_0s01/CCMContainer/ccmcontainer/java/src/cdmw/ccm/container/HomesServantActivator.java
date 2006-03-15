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


package cdmw.ccm.container;

import cdmw.common.Assert;

/**
 * A Servant Activator for home servants.
 *
 */
public class HomesServantActivator
    extends org.omg.CORBA.LocalObject
    implements org.omg.PortableServer.ServantActivator {

    public HomesServantActivator() {}

    public org.omg.PortableServer.Servant incarnate(
        byte[] oid,
        org.omg.PortableServer.POA poa)
        throws org.omg.PortableServer.ForwardRequest {

        String s = new String(oid);
        // Should not happen (at this stage!)
        Assert.check(false);
        return null; // TODO: Maybe raise org.omg.CORBA.TRANSIENT()

    }

    /**
     * The main purpose of this operation is to provide a hook
     * that will be called by the ORB during its shutdown phase
     * to etherealize component homes objects.
     */
    public void etherealize(
        byte[] oid,
        org.omg.PortableServer.POA poa,
        org.omg.PortableServer.Servant servant,
        boolean cleanupInProgress,
        boolean remainingActivations) {

        try {
            String s = new String(oid);
            HomeAllocatorBase homeallocator =
                HomeAllocatorRegistry.getAllocator(s);
            Assert.check(homeallocator != null);
            homeallocator.dispose(servant);
            HomeAllocatorRegistry.unregister(s);
        } catch (Exception e) {
            Assert.check(false); // Shouldn't happen
        }

    }

}

