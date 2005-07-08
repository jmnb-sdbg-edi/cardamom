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

import cdmw.testutils.Testable;
import cdmw.platformmngt.ProcessImpl;
import com.thalesgroup.CdmwPlatformMngt.ProcessPackage.ActivityPointMonitoringModel;

/**
 * The Process & Behaviour test.
 *
 */
public class ProcessBehaviourTest extends Testable {

    public ProcessBehaviourTest() {
        this.name = "Process & Behaviour Test";
        this.out = System.out;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(17);
    
        out.println("Testing Process & Behaviour...");

        out.println("Use case n°1: behaviour with the default process impl");        
        UserProcessBehaviour behaviour = new UserProcessBehaviour();
        ProcessImpl process = new ProcessImpl(behaviour);
        
        out.println("Checking current step...");
        if ( behaviour.getCurrentStep() == 1) {
            succeed();
        } else {
            fail();
        }
        
        out.println("Calling next_step...");
        try {
            process.next_step();
            succeed();
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep is) {
            fail();
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessPackage.BadOrder bo) {
            fail();
        }
        
        out.println("Checking current step...");
        if ( behaviour.getCurrentStep() == 2) {
            succeed();
        } else {
            fail();
        }
        
        out.println("Calling next_step (would raise an exception)...");
        try {
            process.next_step();
            fail();
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep is) {
            succeed();
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessPackage.BadOrder bo) {
            fail();
        }

        out.println("Calling get_service...");
        if ( process.get_service() == null ) {
            succeed();
        } else {
            fail();
        }
        
        out.println("Calling initialise...");
        try {
            process.initialise(null);
            succeed();
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessPackage.BadOrder bo) {
            fail();
        }
        if ( behaviour.getState() == behaviour.INITIALISED ) {
            succeed();
        } else {
            fail();
        }

        out.println("Calling run...");
        try {
            process.run();
            if ( behaviour.getState() == behaviour.RUNNING ) {
                succeed();
            } else {
                fail();
            }
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun nrtr) {
            fail();
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessPackage.AlreadyDone ad) {
            fail();
        }

        out.println("Calling stop...");
        process.stop();
        if ( behaviour.getState() == behaviour.STOPPED ) {
            succeed();
        } else {
            fail();
        }

        out.println("Calling the default nb_activity_points...");
        if ( process.nb_activity_points() == 0 ) {
            succeed();
        } else {
            fail();
        }
        
        out.println("Calling the default get_activity_point " 
            + "(should raise an exception)...");
        try {
            process.get_activity_point(0);
            fail();
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessPackage.OutOfRange oor) {
            succeed();
        }

        out.println("Calling get_all_activity_points...");
        if ( process.get_all_activity_points().length == 0) {
            succeed();
        } else {
            fail();
        }

        out.println("Calling get_pull_monitorable...");
        if ( process.get_pull_monitorable() == null) {
            succeed();
        } else {
            fail();
        }

        out.println("Calling get_push_monitorable...");
        if ( process.get_push_monitorable() == null) {
            succeed();
        } else {
            fail();
        }
        
        out.println("Use case n°2: behaviour with a user defined process impl");
        UserProcessImpl process2 = new UserProcessImpl();
        
        out.println("Calling initialise...");
        process2.initialise(null);
        succeed();
        
        out.println("Calling get_all_activity_points...");
        if ( process2.get_all_activity_points().length == 1 ) {
            succeed();
        } else {
            fail();
        }

        out.println("Calling get_activity_point...");
        try {
            if ( process2.get_activity_point(0).monitoring_model.value() ==
                ActivityPointMonitoringModel.PUSH_MONITORING_MODEL.value() ) {
                succeed();
            } else {
                fail();
            }
        } catch(com.thalesgroup.CdmwPlatformMngt.ProcessPackage.OutOfRange oor) {
            fail();
        }
    
    }

}

