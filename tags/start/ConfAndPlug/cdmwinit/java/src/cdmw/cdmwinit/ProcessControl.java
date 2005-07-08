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

/**
 * The abstract process behaviour.
 *
 */
public abstract class ProcessControl {

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
     * attribute
     */
    public int nbSteps() {
        return 1;
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
     * operation
     */
    public org.omg.CORBA.Object getService() {
        return null;
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    public abstract void onInitialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind);

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    public void onNextStep()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep
    {
        throw new com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep();
    }
    

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    public abstract void onRun() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun;

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    public abstract void onStop();

}

