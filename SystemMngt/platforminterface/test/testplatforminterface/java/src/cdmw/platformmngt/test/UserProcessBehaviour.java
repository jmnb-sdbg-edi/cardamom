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


package cdmw.platformmngt.test;

import cdmw.platformmngt.ProcessBehaviour;

/**
 * A user process behaviour.
 *
 */
public class UserProcessBehaviour extends ProcessBehaviour {
    
    public static final int INITIAL = 0;
    public static final int INITIALISED = 1;
    public static final int RUNNING = 2;
    public static final int STOPPED = 3;
    
    private int state = INITIAL;
    private int currentStep = 1;
    
    public UserProcessBehaviour() {
        // do nothing
    }

    public int nbSteps() {
        return 2;
    }
    
    public int getCurrentStep() {
        return currentStep;
    }
    
    public int getState() {
        return state;
    }

    public void nextStep()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep {
        
        if (currentStep == nbSteps()) {
            throw new com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep();
        } else {
            currentStep++;
            // do initialisations for this step
            System.out.println("Initialisation done for step " + currentStep);
        }
        
    }

    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
        System.out.println("Initialisation Requested");
        state = INITIALISED;
    }

    public void run()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun {
        System.out.println("Run Requested");
        state = RUNNING;
    }

    public void stop() {
        System.out.println("Stop Requested");
        state = STOPPED;
    }

}

