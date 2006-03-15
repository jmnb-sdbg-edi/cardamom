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


package cdmw.ossupport;

/**
 * Allows several threads to wait each thread reach a point.
 * The Barrier is initialised with the number of thread to be waited,
 * each thread is blocked in the waitForAll() call until the number
 * of thread have called waitForAll().
 * When all thread are waiting on the waitForAll(), all threads are unblocked.
 *
 */
public class Barrier {

    /**
     * TIMEDOUT is returned when the specified timeout
     * has elapsed but all thread have not reached the Barrier
     */
    public final static int REACHED = 0;


    /**
     * REACHED is returned when the thread is unblocked
     * because all the threads have reached the Barrier.
     */
    public final static int TIMEDOUT = 1;


    /**
     * Number of thread to be wait before releasing all threads.
     */
    private int totalThreads;


    /**
     * Number of thread currently waiting on the barrier.
     */
    private int currentThreads;

    /**
     * Initialise the barrier with the total
     * number of thread to be waited.
     *
     * @param count number of thread to be waited
     */
    public Barrier(int count) {
        totalThreads = count;
        currentThreads = 0;
    }

    /**
     * Blocks the calling thread until the number
     * of thread specified in the constructor
     * have called a waitForAll() function.
     */
    public synchronized void waitForAll() {
        currentThreads++;
        if(currentThreads < totalThreads) {
            try {
                wait();
            } catch (InterruptedException e) {}
        } else {
            currentThreads = 0;
            notifyAll();
        }
    }

    /**
     * Blocks the calling thread until the number of thread specified 
     * in the constructor have called a waitForAll() function.
     * If all the thread have not reached the Barrier before milliseconds,
     * the calling thread is unblock, and waitForAll() returns
     * TIMEDOUT. Although the calling thread is unblocked, 
     * the barrier counter keeps the same.
     *
     * @param millis time expressed in milli-seconds before
     *       returning although all threads are not on the Barrier
     *
     * @return REACHED if all the thread have reached the Barrier
     *        TIMEDOUT if the time has expired.
     */
    public synchronized int waitForAll(long millis) {

        int status = Barrier.REACHED;
        currentThreads++;
        if(currentThreads < totalThreads) {
            try {
                wait(millis);
                status = Barrier.TIMEDOUT;
            } catch (InterruptedException e) {}
        } else {
            currentThreads = 0;
            notifyAll();
        }
        return status;

    }

}

