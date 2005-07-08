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

import java.util.Vector;
import java.util.Enumeration;

import cdmw.ossupport.ReadWriteLock;

import cdmw.ossupport.OS;
import cdmw.testutils.Testable;

public class ReadWriteLockTest extends Testable {

    public ReadWriteLockTest() {
        this.name = "ReadWriteLockTest";
        this.out = System.out;
    }

    public void doBasicTests()
        throws InterruptedException {

        println("Creating the ReadWriteLock");
        ReadWriteLock lock = new ReadWriteLock();
        succeed("ReadWriteLock created successfully.");

        println("Creating the value to be protected");
        int val = 0;

        BasicReaderThread brt = new BasicReaderThread(lock, val);
        BasicWriterThread bwt = new BasicWriterThread(lock, val);

        println("Locking as a reader");
        lock.readLock();
        succeed("Locked successfully");

        println("Starting the Writer thread");
        bwt.start();

        println("Sleeping a while to see if the writer change the variable");
        OS.sleep(getTimescale() * 2000);
        check(bwt.getVal() == 0);

        println("Starting the Reader thread");
        brt.start();
        println("Checking if the Reader thread has not been blocked by the lock");
        brt.join();
        succeed("The Reader has not been blocked");

        println("Unlocking the lock");
        lock.readUnlock();

        println("Sleeping a while to see if the writer change the variable");
        OS.sleep(getTimescale() * 2000);
        check(bwt.getVal() != 0);
        bwt.join();

    }

    public void doConcurrentTests()
        throws InterruptedException {

        // The time max in each thread is 3s * NB_TEST_LOCKS
        //    as each thread sleep between 0 and 3s
        int nbTestLocks = 5;

        println("Creating the ReadWriteLock");
        ReadWriteLock lock = new ReadWriteLock();
        succeed("ReadWriteLock created successfully.");

        println("Creating the object protected by the lock");
        ReadWriteLockThread rwlt = new ReadWriteLockThread(System.out);
        rwlt.start();

        println("Creating the Two writer threads");
        Vector threads = new Vector();

        for(int iWriterThread=0; iWriterThread < 50; ++iWriterThread) {
            threads.add( new WriterThread(lock,rwlt,nbTestLocks) );
        }

        for(int iReaderThread=0; iReaderThread < 8; ++iReaderThread) {
            threads.add( new ReaderThread(lock,rwlt,nbTestLocks) );
        }

        println("Starting all threads");
        for (Enumeration e = threads.elements() ; e.hasMoreElements() ;) {
            ( (Thread)e.nextElement() ).start();
         }

         println("Waiting for the completion of all threads");
         for (Enumeration e = threads.elements() ; e.hasMoreElements() ;) {
            ( (Thread)e.nextElement() ).join();
         }

        rwlt.join();

        succeed("Concurrent tests successful");


    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(7);
    	
        try {
            println("Starting basic tests...");
            doBasicTests();
            println("Starting concurrent tests...");
            doConcurrentTests();
        } catch(InterruptedException ie) {
            ie.printStackTrace(out);
        }
    }


}

