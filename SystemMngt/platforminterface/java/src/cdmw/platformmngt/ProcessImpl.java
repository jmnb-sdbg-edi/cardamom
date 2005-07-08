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


package cdmw.platformmngt;

import cdmw.common.Assert;

/**
 * The default implementation of a managed process.
 *
 */
public class ProcessImpl extends com.thalesgroup.CdmwPlatformMngt.ProcessPOA {

    /**
     * The actual behaviour of the process.
     */
    private ProcessBehaviour behaviour = null;

    /**
     *  Constructor.
     *  Takes ownership of the supplied behaviour which will be destroyed
     *  by the destructor.
     */ 
    public ProcessImpl(ProcessBehaviour behaviour) {
        Assert.check(behaviour != null);
        this.behaviour = behaviour;
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
     * attribute
     */
    public int nb_steps() {
        Assert.check(behaviour != null);
        return behaviour.nbSteps();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_activity_points:1.0
     * attribute
     */
    public int nb_activity_points() {
        return 0;
    }

    /**
     * implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
     * operation
     */
    public org.omg.CORBA.Object get_service() {
        Assert.check(behaviour != null);
        return behaviour.getService();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_pull_monitorable:1.0
     * operation
     */
    public com.thalesgroup.CdmwPlatformMngt.PullMonitorable 
        get_pull_monitorable() {
        return null;
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_push_monitorable:1.0
     * operation
     */
    public com.thalesgroup.CdmwPlatformMngt.PushMonitorable 
        get_push_monitorable() {
        return null;
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_activity_point:1.0
     * operation
     */
    public com.thalesgroup.CdmwPlatformMngt.ProcessPackage.ActivityPointInfo
        get_activity_point(int pointIndex)
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.OutOfRange {
        throw new com.thalesgroup.CdmwPlatformMngt.ProcessPackage.OutOfRange();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_all_activity_points:1.0
     * operation
     */
    public com.thalesgroup.CdmwPlatformMngt.ProcessPackage.ActivityPointInfo[]
        get_all_activity_points() {
        
        com.thalesgroup.CdmwPlatformMngt.ProcessPackage.ActivityPointInfo[] pointInfos =
            new com.thalesgroup.CdmwPlatformMngt.ProcessPackage.ActivityPointInfo[0];
        return pointInfos;
        
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.BadOrder {
        Assert.check(behaviour != null);
        behaviour.initialise(startupKind);
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    public void next_step() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.BadOrder,
        com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep {
        Assert.check(behaviour != null);
        behaviour.nextStep();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    public void run() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun,
        com.thalesgroup.CdmwPlatformMngt.ProcessPackage.AlreadyDone {
        Assert.check(behaviour != null);
        behaviour.run();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    public void stop() {
        Assert.check(behaviour != null);
        behaviour.stop();
    }

}

