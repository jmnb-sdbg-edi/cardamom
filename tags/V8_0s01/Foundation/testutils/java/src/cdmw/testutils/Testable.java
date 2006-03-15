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


package cdmw.testutils;

import java.io.File;

import java.io.PrintStream;
import java.util.*;

/**
 * Base class for Test classes. It provides routines to start unit tests
 * and count the number of tests that have succeeded or failed.
 *
 */
public abstract class Testable {
    
    /**
     * Name of the system property which defines the CDMW timescale.
     */
    public static final String CDMW_TIMESCALE = "CDMW_TIMESCALE";

    public static final String CDMW_ORB_VDR = "CDMW_ORB_VDR";

    public static final String JACORB = "jacorb";

    public static final String ORBACUS = "orbacus";
    
    /**
     * Variable used to store tests environment.
     */
    private static Vector properties = null;

    /**
     * Number of succeeded test
     */
    private int succeededTests = 0;

    /**
     * Number of failed test
     */
    private int failedTests = 0;
    
    /**
     * Number of requested succeeded tests
     */
    private int nbOfRequestedTestOK = 0;

    /**
     * Name of the test
     */
    protected String name;

    /**
     * Result Stream
     */
    protected PrintStream out;

    /**
     * Return the stream where test results and
     * information must be printed.
     *
     * @return The stream where results must be printed.
     */
    public PrintStream getResultStream() {
        return out;
    }

    /**
     * Return the name of the test
     *
     * @return Name of the test.
     */
    public String getName() {
        return name;
    }

    /**
     * Return the number of succeeded tests
     *
     * @return Number of succeeded tests.
     */
    public int succeededTestsCount() {
        return succeededTests;
    }

    /**
     * Return the number of failed tests
     *
     * @return Number of failed tests
     */
    public int failedTestsCount() {
        return failedTests;
    }

    /**
     * Print a string to the result stream
     *
     * @param s string to be printed
     */
    public void println(String s) {
        getResultStream().println(s);
    }

    /**
     * Start unit tests
     */
    public void start() {
        succeededTests = 0;
        failedTests = 0;
        
        getHeader();
        doTests();
        getSummary();
    }
    
    /**
     * Print a summary of the tests
     */
    public void getSummary() {
        println("===================================================");
        println("Ending test " + getName());
        println("===================================================");
        println("Summary:");
        println("   Failed    : " + failedTestsCount());
        println("   Succeeded : " + succeededTestsCount());
        
        if (getNbOfRequestedTestOK() != 0) {
            println("   Expected successfull tests (JAVA) : " + 
                getNbOfRequestedTestOK());
        }
        
        if (succeededTestsCount() == getNbOfRequestedTestOK() 
            && failedTestsCount() == 0) {
            println("   >>>> Test result is set as TESTABLE_OK <<<<");
        } else if (succeededTestsCount() != getNbOfRequestedTestOK() 
            || failedTestsCount() != 0) {
            println("   >>>> Test result is NOT OK <<<<");
        }

    }

    /**
     * Print a header
     */
    public void getHeader() {
        println("===================================================");
        println("Starting test " + getName());
        println("===================================================");
    }

    /**
     * Abstract method called to do specific CSC tests
     */
    abstract public void doTests();

    /**
     * Check a condition
     *
     * @param condition boolean value to be tested
     */
    public void check(boolean condition) {
        if (condition) {
            println("Test successful.");
            succeededTests++;
        } else {
            println("Test failed.");
            failedTests++;
        }
    }

    /**
     * Print a message in case of a failed test
     *
     * @param message message to be printed
     */
    public void fail(String message) {

        println(message);
        failedTests++;

    }

    /**
     * Print a message in case of a failed test
     */
    public void fail() {

        println("Test failed.");
        failedTests++;

    }

    /**
     * Print a message in case of a succeeded test
     *
     * @param message message to be printed
     */
    public void succeed(String message) {

        println(message);
        succeededTests++;

    }

    /**
     * Print a message in case of a succeeded test
     */
    public void succeed() {

        println("Test successful.");
        succeededTests++;

    }

    /**
     * Sets the number of requested succeeded tests
     * @param nbOfRequestedTestOK Number of requested succeeded tests
     */
    public void setNbOfRequestedTestOK(int nbOfRequestedTestOK) {
        this.nbOfRequestedTestOK = nbOfRequestedTestOK;
    }
    
    /**
     * Adds a number of requested succeeded tests
     * @param Number of requested succeeded tests to add
     */
    public void  addNbOfRequestedTestOK(int nbOfRequestedTestOK) {
        this.nbOfRequestedTestOK += nbOfRequestedTestOK;
    }
    
    /**
     * Returns the number of requested succeeded tests
     * @return The number of requested succeeded tests
     */
    public int getNbOfRequestedTestOK() {
        return this.nbOfRequestedTestOK;
    }

    /**
     * Return the complete path to execute a program coming from another CSC
     *
     * @param cdmwRoot Path to the runtime directory of CDMW
     * @param cscName Name of the CSC
     * @param cscExec Name of the program
     *
     * @return Complete path to the program to be executed
     */
    public static String getCSCExePath(
        String cdmwRoot, 
        String cscName, 
        String cscExec) {

        // path to the CDMW runtime directory is passed as a parameter 
        // because we can't get the environment variable value 
        // in a platform indenpendent way

        // The environment variable has not been defined 
        // or is not correctly defined
        if (cdmwRoot == null || !(new File(cdmwRoot).exists()) ) {
            System.out.println("Fatal: " + cdmwRoot + 
                                " does not exist, aborting...");
            System.exit(0);
        }
        String separator = File.separator;
        return cdmwRoot + separator +
                cscName + separator +
                "java" + separator +
                "prod" + separator +
                cscExec;
    }
    
    /**
     * Returns the value of the timescale if defined, 1 otherwise.
     * NB: if a new JVM is launched the property is not preserved.
     */
    public static int getTimescale() {
        
        int timescale = 1;
        try {
            timescale = Integer.parseInt(
                System.getProperty(CDMW_TIMESCALE));
        } catch(NumberFormatException nfe) {}
        return timescale;
        
    }
    
    /**
     * Returns the system properties needed for the tests
     * as a Vector, each element of which follows the format name=value
     * 
     * @return Vector containing all the needed system properties.
     */
    public static Vector getProperties() {
        
        if (properties != null) {
            return properties;
        }
 	properties = new Vector();
	/*Properties props = System.getProperties();
	Enumeration enum =  props.propertyNames();
	while (enum.hasMoreElements()) {
		String name = (String)enum.nextElement();
		properties.add( name + "=" + props.getProperty(name));
	}*/

	properties.add( JACORB + "=" + System.getProperty(JACORB));       
	properties.add( CDMW_ORB_VDR + "=" + System.getProperty(CDMW_ORB_VDR));
	
	properties.add(CDMW_TIMESCALE + "=" + getTimescale());
        return properties;
        
    }

}

