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

import cdmw.testutils.Testable;
import cdmw.ossupport.OS;

public class MainTest {

    public static void printHelp() {

        System.out.println("Cdmw OsSupport Unit testing");
        System.out.println(" options are:");
        System.out.println("   --help:           print this message");
        System.out.println("   --test-all:       test all the CSC");
        System.out.println("   --test-OS:        test the OS class");
        System.out.println("   --test-Mutex:     test the Mutex class");
        System.out.println("   --test-Lock:      test the ReadWriteLock class");
        System.out.println("   --test-Barrier:   test the Barrier class");

    }

    public static void main(String[] args) {

        boolean all = false;
        boolean one = false;

        // Creation of a file to test (needed for the other tests)
        String tmpFile = OS.getOptionValue(args,"--create-file");
        if ( !tmpFile.equals("no") ) {
            File file = new File(tmpFile);
            try {
                if (file.createNewFile()) {
                    System.out.println("File " + file + " created successfully.");
                } else {
                    System.out.println("Unable to create the file " +
                        file +": the named file already exists.");
                }
            } catch(IOException ioe) {
                System.out.println("Unable to create the file " +
                    file + ": " + ioe.getMessage());
            }
            one = true;
        }

        // Sleep
        String sleep = OS.getOptionValue(args,"--sleep");
        if ( !sleep.equals("no") ) {
            try {
                int time = Testable.getTimescale() * Integer.parseInt(sleep);
                OS.sleep(time * 1000);
                System.out.println("Sleep of " + time + " successful.");
            } catch(InterruptedException ie) {
                System.out.println("Unable to sleep process: " +
                    ie.getMessage() );
            } catch(NumberFormatException nfe) {
                System.out.println("Unable to sleep process: " +
                    nfe.getMessage() );
            }
            one = true;
        }

        // No arguments we do all the test
        if ( args.length == 0 ) {
            all = true;
        }

        // --test-all supersedes all other options
        if ( OS.getOptionValue(args,"--test-all").equals("yes") ) {
            all = true;
        }

        // Testing the OS support functions
        if ( all || OS.getOptionValue(args,"--test-OS").equals("yes") ) {
            OSTest ostest = new OSTest();
            ostest.start();
            one = true;
        }

        // Testing the Mutex object
        if ( all || OS.getOptionValue(args,"--test-Mutex").equals("yes") ) {
            MutexTest mt = new MutexTest();
            mt.start();
            one = true;
        }

        // Testing the ReadWriteLock object
        if ( all || OS.getOptionValue(args,"--test-Lock").equals("yes") ) {
            ReadWriteLockTest rwlt = new ReadWriteLockTest();
            rwlt.start();
            one = true;
        }

        // Testing the Barrier object
        if ( all || OS.getOptionValue(args,"--test-Barrier").equals("yes") ) {
            BarrierTest bt = new BarrierTest();
            bt.start();
            one = true;
        }

        // Help message
        if ( !one || OS.getOptionValue(args,"--help").equals("yes") ) {
            printHelp();
        }

    }

}

