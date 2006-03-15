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


package cdmw.platformmngt;

import cdmw.common.Assert;
import cdmw.ossupport.OS;

/**
 * The default implementation of a managed process.
 *
 */
public class ProcessDelegateImpl extends com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePOA {

    /**
     * The actual behaviour of the process.
     */
    private ProcessBehaviour behaviour = null;
    private org.omg.FT.PullMonitorable pullMonitorable = null;

    /**
     *  Constructor.
     *  Takes ownership of the supplied behaviour which will be destroyed
     *  by the destructor.
     */ 
    public ProcessDelegateImpl(org.omg.CORBA.ORB orb, ProcessBehaviour behaviour) {
        Assert.check(behaviour != null);
        this.behaviour = behaviour;

        try {
          org.omg.CORBA.Object obj = orb.resolve_initial_references("RootPOA");

          org.omg.PortableServer.POA poa = org.omg.PortableServer.POAHelper.narrow(obj);

          poa.the_POAManager().activate();
        } catch(Exception e) {
            e.printStackTrace();
        }

        PullMonitorableImpl pullImpl = new PullMonitorableImpl(behaviour);
        pullMonitorable = pullImpl._this(orb);
    }
    

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/nb_steps:1.0
     * attribute
     */
    public int nb_steps() {
        Assert.check(behaviour != null);
        return behaviour.nbSteps();
    }


    /**
     * implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
     * operation
     */
    public org.omg.CORBA.Object get_service() {
        Assert.check(behaviour != null);
        return behaviour.getService();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_pull_monitorable:1.0
     * operation
     */
    public org.omg.FT.PullMonitorable 
        get_pull_monitorable() {
        return pullMonitorable;
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_push_monitorable:1.0
     * operation
     */
    public com.thalesgroup.CdmwPlatformMngt.PushMonitorable 
        get_push_monitorable() {
        return null;
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.BadOrder {
        Assert.check(behaviour != null);
        behaviour.initialise(startupKind);
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
     * operation
     */
    public void next_step() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.BadOrder,
        com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.InvalidStep {
        Assert.check(behaviour != null);
        behaviour.nextStep();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    public void run() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun,
        com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.AlreadyDone {
        Assert.check(behaviour != null);
        behaviour.run();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    public void stop() {
        Assert.check(behaviour != null);
        behaviour.stop();
    }

    private class PullMonitorableImpl extends org.omg.FT.PullMonitorablePOA {

       /**
        * The actual behaviour of the process.
        */
       private ProcessBehaviour behaviour = null;

       public PullMonitorableImpl(ProcessBehaviour behaviour) {
           Assert.check(behaviour != null);
           this.behaviour = behaviour;
       }

        public boolean is_alive()
        {
           return this.behaviour.is_alive();
        }
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    public com.thalesgroup.CdmwPlatformMngt.CommandResponse 
        call_command(String command_name, 
                     com.thalesgroup.CdmwPlatformMngt.CmdParameterValue[] command_args, 
                     org.omg.CORBA.StringHolder result_info) 
        throws com.thalesgroup.CdmwPlatformMngt.CommandCallBackNotFound {
        // TODO
        throw new org.omg.CORBA.NO_IMPLEMENT (cdmw.orbsupport.ExceptionMinorCodes.NO_IMPLEMENT, org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    }
        

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
	 public com.thalesgroup.CdmwPlatformMngt.CommandResponse 
        call_entity_command(String entity_name, 
                            String command_name, 
                            com.thalesgroup.CdmwPlatformMngt.CmdParameterValue[] command_args, 
                            org.omg.CORBA.StringHolder result_info) 
        throws com.thalesgroup.CdmwPlatformMngt.CommandCallBackNotFound {
        // TODO
        throw new org.omg.CORBA.NO_IMPLEMENT (cdmw.orbsupport.ExceptionMinorCodes.NO_IMPLEMENT, org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    }


}

