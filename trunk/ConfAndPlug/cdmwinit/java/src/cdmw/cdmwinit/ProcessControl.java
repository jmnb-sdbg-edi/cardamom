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


package cdmw.cdmwinit;

/**
 * The abstract process behaviour.
 *
 */
public abstract class ProcessControl {

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/nb_steps:1.0
     * attribute
     */
    public int nbSteps() {
        return 1;
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
     * operation
     */
    public org.omg.CORBA.Object getService() {
        return null;
    }

    /**
     * This operation informs the pull-based Fault Detector whether the object is able to
     *    accept requests and produce replies.
     *    This operation may be used to perform some user-defined sanity checks ...
     *
     * @return true if the object is alive and ready to take further requests, and false otherwise.
     *
     */
    public boolean isAlive() {
        return true;
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    public abstract void onInitialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind);

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
     * operation
     */
    public void onNextStep()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.InvalidStep
    {
        throw new com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.InvalidStep();
    }
    

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    public abstract void onRun() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun;

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    public abstract void onStop();

}

