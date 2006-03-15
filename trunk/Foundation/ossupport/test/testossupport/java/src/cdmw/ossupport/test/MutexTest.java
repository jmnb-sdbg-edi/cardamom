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


package cdmw.ossupport.test;

import cdmw.ossupport.OS;
import cdmw.ossupport.Mutex;
//import cdmw.ossupport.MutexGuard;

import cdmw.testutils.Testable;

public class MutexTest extends Testable {

    private boolean validOnly;

    public MutexTest() {
        this.name = "MutexTest";
        this.out = System.out;
        this.validOnly = false;
    }

    public MutexTest(boolean validationOnly) {
        this.name = "MutexTest";
        this.out = System.out;
        this.validOnly = validationOnly;
    }

    /*
    public void doTestWithGuard()
        throws InterruptedException {

        println("Creating a Mutex");
        Mutex mutex = new Mutex();

        println("Locking the mutex");
        mutex.lock();

        println("Creating a thread using automatic lock/unlock of a thread");
        int val = 0;
        MutexGuardThread mgt = new MutexGuardThread(mutex, val);

        println("Starting the thread");
        mgt.start();

        println("Sleeping a while to see if the thread changes the variable");
        OS.sleep(getTimescale() * 2000);

        check( mgt.getVal() == 0 );

        println("Unlocking the mutex");
        mutex.unlock();

        println("Sleeping a while to see if the thread changes the variable");
        OS.sleep(getTimescale() * 2000);

        check( mgt.getVal() != 0 );

        println("Waiting for the completion of the thread");
        mgt.join();

    }
    */

    public void doTestWithoutGuard()
        throws InterruptedException {

        println("Creating a Mutex");
        Mutex mutex = new Mutex();

	println("Ckech the mutex is unlock");
	check( !mutex.is_locked() );

        println("Locking the mutex");
        mutex.lock();

	println("Ckech the mutex is lock");
	check( mutex.is_locked() );

        println("Creating a thread with the mutex as reference and a variable to be changed");
        int val = 0;
        MutexThread mt = new MutexThread(mutex, val);

        println("Starting the thread");
        mt.start();

        println("Sleeping a while to see if the thread changes the variable");
        OS.sleep(getTimescale() * 2000);

        check( mt.getVal() == 0 );

        println("Unlocking the mutex");
        mutex.unlock();

	println("Ckech the mutex is unlock");
	check( !mutex.is_locked() );

        println("Sleeping a while to see if the thread changes the variable");
        OS.sleep(getTimescale() * 2000);

        check( mt.getVal() != 0 );

        println("Waiting for the completion of the thread");
        mt.join();

    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(5);

        try {
            // if necessary use validationOnly in the code if you want
            // to select validation tests
            doTestWithoutGuard();
            // doTestWithGuard();
        }
        catch(InterruptedException ie) {
            println(ie.getMessage());
        }

    }

}

