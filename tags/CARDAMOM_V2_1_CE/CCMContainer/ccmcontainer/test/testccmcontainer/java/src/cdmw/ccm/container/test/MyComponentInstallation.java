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


package cdmw.ccm.container.test;

public class MyComponentInstallation
    extends com.thalesgroup.CdmwDeployment.ComponentInstallationPOA {

    private org.omg.PortableServer.POA poa;

    public MyComponentInstallation(org.omg.PortableServer.POA poa) {
        this.poa = poa;
    }

    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/install:1.0
    //
    public void install(String implUUID, String component_loc)
        throws
            org.omg.Components.Deployment.InvalidLocation,
            org.omg.Components.Deployment.InstallationFailure {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/replace:1.0
    //
    public void replace(String implUUID, String component_loc)
        throws
            org.omg.Components.Deployment.InvalidLocation,
            org.omg.Components.Deployment.InstallationFailure {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/remove:1.0
    //
    public void remove(String implUUID)
        throws
            org.omg.Components.Deployment.UnknownImplId,
            org.omg.Components.RemoveFailure {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    //
    // IDL:omg.org/Components/Deployment/ComponentInstallation/get_implementation:1.0
    //
    public String get_implementation(String implUUID)
        throws
            org.omg.Components.Deployment.UnknownImplId,
            org.omg.Components.Deployment.InstallationFailure {
        return "";
    }

    //
    // IDL:thalesgroup.com/CdmwDeployment/ComponentInstallation/get_valuetypefactory_location:1.0
    //
    public String get_valuetypefactory_location(String implUUID, String repid)
        throws
            org.omg.Components.Deployment.UnknownImplId,
            com.thalesgroup.CdmwDeployment.UnknownValuetype,
            org.omg.Components.Deployment.InstallationFailure {
        return "";
    }

}

