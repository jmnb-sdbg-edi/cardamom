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


package cdmw.cdmwinit;

import cdmw.common.Assert;
import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;

import cdmw.platformmngt.ProcessBehaviour;
import cdmw.platformmngt.PlatformInterface;

/**
 * The default implementation of a managed process.
 *
 */
public class CdmwProcessImpl
    extends com.thalesgroup.CdmwPlatformMngt.ProcessPOA {

    /**
     * The ORB.
     */
    private org.omg.CORBA.ORB orb;

    /**
     * ORB Root POA.
     */
    private org.omg.PortableServer.POA poa;

    /**
     * CDMW Root POA.
     */
    private org.omg.PortableServer.POA cdmwRootPOA;

    /**
     * The actual control of the process.
     */
    private ProcessControl control = null;

    /**
     * Is there an exception raised during the CDMW init ?
     */
    private boolean cdmwInitExceptionRaised = false;

    /**
     * Is CDMW init done ?
     */
    private boolean cdmwInitDone = false;

    /**
     *  Constructor.
     *  Takes ownership of the supplied ProcessControl
     */
    public CdmwProcessImpl(org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA poa,
        org.omg.PortableServer.POA cdmwRootPOA, ProcessControl ctrl) {
        Assert.check(orb != null);
        Assert.check(ctrl != null);
        this.orb = orb;
        this.poa = poa;
        this.cdmwRootPOA = cdmwRootPOA;
        this.control = ctrl;

    }

    public synchronized void setCdmwInitExceptionRaised() {
        cdmwInitExceptionRaised = true;
        notify();
    }

    public synchronized void setCdmwInitDone() {
        cdmwInitDone = true;
        notify();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
     * attribute
     */
    public int nb_steps() {
        Assert.check(control != null);
        return control.nbSteps();
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
        Assert.check(control != null);
        return control.getService();
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
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {

        try {
            synchronized(this) {
                if (!(cdmwInitExceptionRaised || cdmwInitDone)) {
                    // Wait for cdmw_init end
                    wait();
                }

                if (cdmwInitExceptionRaised) {
                    PlatformInterface.notifyFatalError(
                        PlatformInterface.getProcessName(),
                            "Exception raised during CDMW_init!");
                } else {
                    if (cdmwInitDone) {
                        // CDMW POA Managers is in the discarding state.
                        // So we have to re-activate it.
                        // NOTA: We do activate the POA manager before
                        //       on_initialise() to enable nested callbacks
                        //         are needed (user receives an incomming request
                        //       while making an outgoing call).
                        this.cdmwRootPOA.the_POAManager().activate();
                        control.onInitialise(startupKind);
                    } else {
                        PlatformInterface.notifyFatalError(
                            PlatformInterface.getProcessName(),
                                "Unexpected statement reached!");
                    }
                }
            }
        } catch (org.omg.PortableServer.POAManagerPackage.AdapterInactive ai) {
        } catch(BadParameterException bpe) {
        } catch(BadOrderException boe) {
        } catch(InterruptedException ie) {}

    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    public void next_step()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep {
        Assert.check(control != null);
        // TODO: Add CDMW specific init if necessary
        control.onNextStep();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    public void run()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun {
        Assert.check(control != null);
        // TODO: Add CDMW specific stuff if necessary
        control.onRun();
    }

    /**
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    public void stop() {
        Assert.check(control != null);
        control.onStop();
        // TODO: Add CDMW specific stuff if necessary
        // then initiate ORB shutdown
        orb.shutdown(false);
    }

}

