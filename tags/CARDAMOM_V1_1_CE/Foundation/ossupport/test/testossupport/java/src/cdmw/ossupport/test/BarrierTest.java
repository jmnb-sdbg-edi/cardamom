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

import cdmw.ossupport.Barrier;
import cdmw.ossupport.OS;

import cdmw.testutils.Testable;

public class BarrierTest extends Testable {

    public BarrierTest() {
        this.name = "BarrierTest";
        this.out = System.out;
    }

    public void doTimedOutTests()
        throws InterruptedException {

        println("Creating a barrier for 3 threads");
        Barrier b = new Barrier(3);

        println("Creating 1 thread waiting on the barrier");
        println("   with a value to be changed ");
        int val1 = 0;
        BarrierThread bt1 = new BarrierThread(b, val1);

        println("Starting the thread");
        bt1.start();

        println("Waiting on the Barrier with a timeout");
        int status = b.waitForAll(500);
        check( status == Barrier.TIMEDOUT );

        println("Creating a second thread waiting on the barrier");
        println("   with a value to be changed ");
        int val2 = 0;
        BarrierThread bt2 = new BarrierThread(b, val2);
        println("Starting the thread");
        bt2.start();

        println("Sleeping a while to see if all thread have gone across the Barrier");
        OS.sleep(getTimescale() * 2000);
        check( bt1.getVal() == 999 && bt2.getVal() == 999 );

        bt1.join();
        bt2.join();

    }

    public void doBasicTests()
        throws InterruptedException {

        println("Creating a barrier for 3 threads");
        Barrier b = new Barrier(3);

        println("Creating 2 threads waiting on the barrier");
        println("   with a value to be changed for each one");
        int val1 = 0;
        int val2 = 0;
        BarrierThread bt1 = new BarrierThread(b, val1);
        BarrierThread bt2 = new BarrierThread(b, val2);

        println("Starting all threads");
        bt1.start();
        bt2.start();

        println("Sleeping a while to see if threads are blocked by the Barrier");
        OS.sleep(getTimescale() * 2000);
        check( bt1.getVal() == 0 && bt2.getVal() == 0);

        println("Waiting on the Barrier and see if we unblocks the Barrier");
        b.waitForAll();
        succeed("Threads unblocked.");

        println("Sleeping a while to see if all thread have gone across the Barrier");
        OS.sleep(getTimescale() * 2000);
        check( bt1.getVal() == 999 && bt2.getVal() == 999);

        bt1.join();
        bt2.join();

    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(5);
    	
        try {
            doBasicTests();
            doTimedOutTests();
        } catch(InterruptedException ie) {
            println(ie.getMessage());
        }
    }

}

