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


package cdmw.ossupport.test;

import java.io.File;
import java.io.IOException;

import java.util.Date;
import java.util.Properties;

import cdmw.ossupport.OS;
import cdmw.ossupport.Timeval;
import cdmw.ossupport.ThreadedProcess;
import cdmw.testutils.Testable;

public class OSTest extends Testable {

    public OSTest() {
        this.name = "OSTest";
        this.out = System.out;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(8);

        // TODO: finding how to test the returned date.
        println("Trying to get the time");
        Timeval time = OS.getTime();
        println(time.toString());

        println("Number of milliseconds since the epoch: "+time.getMilliseconds());
        println("Number of seconds since the epoch: "+time.getSeconds());
        println("Number of microseconds remaining: "+time.getMicroseconds());

        println("Trying to get the hostname");
        println(OS.getHostname());

        println("Creating a temporary file name");
        String filename = new String("Cdmw.tmp");

        println("Checking this file not exists");
        check( !OS.fileExists(filename) );

        println("Creating a new process");
        String classname = "cdmw.ossupport.test.MainTest"
                            + " --create-file=" + filename;

        ThreadedProcess p = null;
        try {
            p = OS.createJavaProcess(classname, getProperties());
            succeed("New process successfully created.");
        } catch (java.io.IOException ioe) {
            fail("Unable to create a new process.");
        }

        println("Test of the sleep function");
        println("We sleep a while ...");
        try {
            OS.sleep(getTimescale() * 2000);
            succeed("Sleep successful.");
        } catch(InterruptedException ie) {
            fail("Unable to sleep process: " + ie.getMessage() );
        }
        catch(NumberFormatException nfe) {
            fail("Unable to sleep process: " + nfe.getMessage() );
        }

        if ( p!=null ) {
            println("We wait for the process termination");
            int exit = 0;
            try {
                exit = OS.waitProcess(p);
            } catch(InterruptedException ie) {
                println(ie.getMessage());
            }

            println("Checking if the process termination is normal");
            check( exit == 0);

            println("Checking if the process has created a new file");
            check( OS.fileExists(filename) );

            println("Trying to execute a non executable file");
            try {
                p = OS.createProcess(filename);
                OS.sleep(getTimescale() * 2000);
                fail("Error process created.");
            } catch(IOException ioe) {
                succeed("Unable to create the process.");
            } catch(InterruptedException ie) {
            }

            println("Trying to delete an existing file");
            check( OS.unlink(filename) );

            println("Trying to delete an unexisting file");
            check( !OS.unlink(filename) );

        }

    }

}

