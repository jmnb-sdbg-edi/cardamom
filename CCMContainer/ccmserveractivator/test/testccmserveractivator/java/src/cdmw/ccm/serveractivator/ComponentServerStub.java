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


package cdmw.ccm.serveractivator;

import org.omg.Components.Deployment.ServerActivator;
import org.omg.Components.Deployment.Container;
import org.omg.Components.Deployment.InvalidConfiguration;



class ComponentServerStub
    extends com.thalesgroup.CdmwCcmComponentServer.ComponentServerPOA
{
    
    private org.omg.PortableServer.POA poa;
    private boolean isRemoved;
    
    public ComponentServerStub(org.omg.PortableServer.POA poa) {
        this.poa = poa;
        isRemoved = false;
    }
    
    public boolean isRemoved() {
        return isRemoved;
    }
    
    public org.omg.PortableServer.POA _default_POA() {
        return poa;
    }
    
    
    //
    // IDL:omg.org/Components/Deployment/ComponentServer/configuration:1.0
    //
    public org.omg.Components.ConfigValue[]
    configuration()
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }   

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/get_server_activator:1.0
    //
    public ServerActivator
    get_server_activator()
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }   

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/create_container:1.0
    //
    public Container
    create_container(org.omg.Components.ConfigValue[] config)
        throws org.omg.Components.CreateFailure,
               InvalidConfiguration
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }   

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/remove_container:1.0
    //
    public void
    remove_container(Container cref)
        throws org.omg.Components.RemoveFailure
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }   

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/get_containers:1.0
    //
    public Container[]
    get_containers()
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }   

    //
    // IDL:omg.org/Components/Deployment/ComponentServer/remove:1.0
    //
    public void
    remove()
        throws org.omg.Components.RemoveFailure
    {
        isRemoved = true;
    }   
        

    //
    // IDL:thalesgroup.com/CdmwCcmComponentServer/ComponentServer/set_component_installation:1.0
    //
    public void
    set_component_installation(com.thalesgroup.CdmwDeployment.ComponentInstallation component_installation)
    {
       // do nothing
    }

    //
    // IDL:thalesgroup.com/CdmwCcmComponentServer/ComponentServer/set_server_activator:1.0
    //
    public void
    set_server_activator(org.omg.Components.Deployment.ServerActivator server_activator)
    {
       // do nothing
    }
    

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    public boolean
    supports(String repository_id)
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    public String[]
    get_supported_objects()
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }
    
    
        
}

