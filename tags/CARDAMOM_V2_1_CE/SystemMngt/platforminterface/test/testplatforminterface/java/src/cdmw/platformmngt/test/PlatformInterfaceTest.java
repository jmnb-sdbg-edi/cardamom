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


package cdmw.platformmngt.test;

import cdmw.testutils.Testable;
import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;

import cdmw.platformmngt.ProcessDelegateImpl;

/**
 * The PlatformInterface test.
 *
 */
public class PlatformInterfaceTest extends Testable {

    private String[] args;
    
    public static int INITIAL = 0;
    public static int SETUP_PERFORMED = 1;
    public static int ACKNOWLEDGEMENT_PERFORMED = 2;

    public PlatformInterfaceTest(String[] args) {
        this.out = System.out;
        this.name = "PlatformInterface Test";
        this.args = args;
    }

    public void doTests() {
    	
    	  // set number of requested successfull tests
		  setNbOfRequestedTestOK(23);
    
        String eventIssuer = "An issuer";
        String entityName = "An entity name";
        
        out.println("Testing PlatformInterface...");
        
        out.println("Calling isLaunchedByPlatformManagement " 
            + "with no callback IOR argument...");
        if ( PlatformInterface.isLaunchedByPlatformManagement(args) ) {
            fail("isLaunchedByPlatformManagement should not return true.");
        } else {
            succeed();
        }
        
        org.omg.CORBA.ORB orb = org.omg.CORBA.ORB.init(
            args, cdmw.orbsupport.ORBUtils.getProperties());
        
        out.println("Calling setup with no callback IOR argument...");
        try {
            PlatformInterface.setup(orb, args);
            fail();
        } catch(BadParameterException bpe) {
            succeed();
        } catch(BadOrderException boe) {
            fail();
        }
        
        out.println("Checking status = INITIAL...");
        if (PlatformInterface.isStatus(INITIAL)) {
            succeed();
        } else {
            fail();
        }
        
        UserProcessBehaviour behaviour = null;
        com.thalesgroup.CdmwPlatformMngt.ProcessDelegate process = null;
        
        out.println("Calling acknowledgeCreation with a behaviour before setup...");
        try {
            PlatformInterface.acknowledgeCreation(behaviour);
            fail();
        } catch(BadOrderException boe) {
            succeed();
        } catch(BadParameterException bpe) {
            fail();
        }
        
        
        out.println("Calling acknowledgeCreation with a process before setup...");
        try {
            PlatformInterface.acknowledgeCreation(process);
            fail();
        } catch(BadOrderException boe) {
            succeed();
        } catch(BadParameterException bpe) {
            fail();
        }
        
        out.println("Status forced to SETUP_PERFORMED");
        PlatformInterface.forceStatus(SETUP_PERFORMED);
        
        out.println("Calling acknowledgeCreation with a null behaviour...");
        try {
            PlatformInterface.acknowledgeCreation(behaviour);
        } catch(BadParameterException bpe) {
            succeed();
        } catch(BadOrderException boe) {
            fail();
        }

        out.println("Calling acknowledgeCreation with a null process...");
        try {
            PlatformInterface.acknowledgeCreation(process);
        } catch(BadParameterException bpe) {
            succeed();
        } catch(BadOrderException boe) {
            fail();
        }

        out.println("Checking status = SETUP_PERFORMED...");
        if (PlatformInterface.isStatus(SETUP_PERFORMED)) {
            succeed();
        } else {
            fail();
        }
        
        out.println("Calling notifyMessage...");
        try {
            PlatformInterface.notifyMessage( 
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                com.thalesgroup.CdmwPlatformMngtBase.PFMMNG.value,
                "message");
            fail();
        } catch(BadOrderException boe) {
            succeed();
        } catch(BadParameterException bpe) {
            fail();
        }
        
        out.println("Status forced to ACKNOWLEDGEMENT_PERFORMED");
        PlatformInterface.forceStatus(ACKNOWLEDGEMENT_PERFORMED);
        
        out.println("Calling notifyMessage with FTL level...");
        try {
            PlatformInterface.notifyMessage( 
                com.thalesgroup.CdmwPlatformMngtBase.FTL.value,
                com.thalesgroup.CdmwPlatformMngtBase.PFMMNG.value,
                "message");
            fail();
        } catch(BadOrderException boe) {
            fail();
        } catch(BadParameterException bpe) {
            succeed();
        }
    
        out.println("Calling notifyMesssage with a null issuer...");
        try {
            PlatformInterface.notifyMessage( 
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                null,
                "message");
            fail();
        } catch(BadOrderException boe) {
            fail();
        } catch(BadParameterException bpe) {
            succeed();
        }
    
        out.println("Calling notifyMesssage with a PFMNG issuer...");
        try {
            PlatformInterface.notifyMessage( 
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                com.thalesgroup.CdmwPlatformMngtBase.PFMMNG.value,
                "message");
            fail();
        } catch(BadOrderException boe) {
            fail();
        } catch(BadParameterException bpe) {
            succeed();
        }
        
        out.println("Calling notifyMesssage with a null message...");
        try {
            PlatformInterface.notifyMessage( 
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                eventIssuer,
                null);
            fail();
        } catch(BadOrderException boe) {
            fail();
        } catch(BadParameterException bpe) {
            succeed();
        }
    
        out.println("Status forced to INITIAL");
        PlatformInterface.forceStatus(INITIAL);
    
        out.println("Calling notifyFatalError before setup...");
        try {
            PlatformInterface.notifyFatalError(eventIssuer, "error message");
            fail();
        } catch(BadOrderException boe) {
            succeed();
        } catch(BadParameterException bpe) {
            fail();
        }
    
        out.println("Status forced to SETUP_PERFORMED");
        PlatformInterface.forceStatus(SETUP_PERFORMED);
    
        out.println("Calling notifyFatalError with null issuer...");
        try {
            PlatformInterface.notifyFatalError(null, "error message");
            fail();
        } catch(BadOrderException boe) {
            fail();
        } catch(BadParameterException bpe) {
            succeed();
        }
    
        out.println("Calling notifyFatalError with PFMMNG issuer...");
        try {
            PlatformInterface.notifyFatalError(
                com.thalesgroup.CdmwPlatformMngtBase.PFMMNG.value, 
                "error message");
            fail();
        } catch(BadOrderException boe) {
            fail();
        } catch(BadParameterException bpe) {
            succeed();
        }
    
        out.println("Calling notifyFatalError with null message...");
        try {
            PlatformInterface.notifyFatalError(
                eventIssuer, 
                null);
            fail();
        } catch(BadOrderException boe) {
            fail();
        } catch(BadParameterException bpe) {
            succeed();
        }
    
        out.println("Calling getApplicationName before acknowlegment...");
        String appName = "";
        try {
            appName = PlatformInterface.getApplicationName();
            fail();
        } catch(BadOrderException boe) {
            succeed();
        }
    
        out.println("Calling getProcessName before acknowlegment...");
        String procName = "";
        try {
            procName = PlatformInterface.getProcessName();
            fail();
        } catch(BadOrderException boe) {
            succeed();
        }

        out.println("Calling getService before acknowlegment...");
        String service = "";
        try {
            PlatformInterface.getService("A Service Name");
        } catch(BadOrderException boe) {
            succeed();
        } catch(Exception e) {
            fail();
        }
    
        out.println("Status forced to ACKNOWLEDGEMENT_PERFORMED");
        PlatformInterface.forceStatus(ACKNOWLEDGEMENT_PERFORMED);
    
        out.println("Calling getService with null service name...");
        try {
            PlatformInterface.getService(null);
        } catch(BadParameterException bpe) {
            succeed();
        } catch(Exception e) {
            fail();
        }
        
        /****
        out.println("Status forced to SETUP_PERFORMED");
        PlatformInterface.forceStatus(SETUP_PERFORMED);
        out.println("Calling setSystemEntityStatus before acknowlegment...");
        try {
            PlatformInterface.setSystemEntityStatus(
                entityName,
                com.thalesgroup.CdmwPlatformMngtEntity.EntityStatus.ENTITY_FUNCTIONING,
                "An entity info");
            fail();
        } catch(BadOrderException boe) {
            succeed();
        } catch(Exception e) {
            fail();
        }
    
        out.println("Status forced to ACKNOWLEDGEMENT_PERFORMED");
        PlatformInterface.forceStatus(ACKNOWLEDGEMENT_PERFORMED);
    
        out.println("Calling setSystemEntityStatus with null entity name...");
        try {
            PlatformInterface.setSystemEntityStatus(
                null,
                com.thalesgroup.CdmwPlatformMngtEntity.EntityStatus.ENTITY_FUNCTIONING,
                "An entity info");
            fail();
        } catch(BadParameterException boe) {
            succeed();
        } catch(Exception e) {
            fail();
        }
    
        out.println("Calling setSystemEntityStatus with null entity info...");
        try {
            PlatformInterface.setSystemEntityStatus(
                entityName,
                com.thalesgroup.CdmwPlatformMngtEntity.EntityStatus.ENTITY_FUNCTIONING,
                null);
            fail();
        } catch(BadParameterException boe) {
            succeed();
        } catch(Exception e) {
            fail();
        }
        ****/
        
        
        out.println("Creating a header with an INF level...");
        com.thalesgroup.CdmwPlatformMngtBase.EventHeader eventHeader 
            = PlatformInterface.createEventHeader(com.thalesgroup.CdmwPlatformMngtBase.INF.value);
        
        if ( eventHeader.level == com.thalesgroup.CdmwPlatformMngtBase.INF.value ) {
            succeed();
        } else {
            fail();
        }
        
        out.println("Creating a CDMW time stamp...");
        com.thalesgroup.CdmwPlatformMngtBase.TimeStamp timeStamp 
            = PlatformInterface.createCdmwTimeStamp();
        out.println("Time stamp : " +
            " Year=  " + timeStamp.year +
            " Month= " + timeStamp.month +
            " Day=   " + timeStamp.day +
            " Hour=  " + timeStamp.hour +
            " Min=   " + timeStamp.minute +
            " Sec=   " + timeStamp.second +
            " Milli= " + timeStamp.millisecond +
            " Micro= " + timeStamp.microsecond);
            
        out.println("Doing basic tests on the created time stamp...");
        if ( timeStamp.year > 1901 &&
            timeStamp.month > 0 && timeStamp.month < 13 &&
            timeStamp.day > 0 && timeStamp.day < 32 &&
            timeStamp.hour  > -1 && timeStamp.hour < 24 &&
            timeStamp.minute > -1 && timeStamp.minute < 60 &&
            timeStamp.second > -1 && timeStamp.second < 60 &&
            timeStamp.millisecond > -1 && timeStamp.millisecond < 1000 &&
            timeStamp.microsecond > -1 && timeStamp.microsecond < 1000 ) {
            succeed();
        } else {
            fail();
        }
        
    }

}

