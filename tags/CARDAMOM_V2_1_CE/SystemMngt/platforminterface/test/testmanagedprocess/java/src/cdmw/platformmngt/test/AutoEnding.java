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

import cdmw.common.Assert;
import cdmw.common.InternalErrorException;

import cdmw.ossupport.OS;
import cdmw.ossupport.Mutex;

/**
 * This class implements the Auto Ending after timeout
 * for managed process
 *
 * Features:
 * Thread safe, exception safe
 *
 */
public class AutoEnding extends Thread {

    /**
     * Stop flag
     */
    private boolean stopped = false;

    /**
     * Mutex protecting stop flag and associated to condition
     */
    private Mutex mutex = new Mutex();

    /**
     * Ending timeout used by AutoEnding object
     * to stop process in msec
     */
    private long endingTimeout;

    /**
     * ProcessBehaviour object pointer
     */
    private MyProcessBehaviour processBehaviour = null;
    
    /**
     * Boolean to detect timeout
     */
    private boolean stopProcess = false;

    /**
     * Constructor
     *
     * @param endTimeout timeout used by AutoEnding object
     * to stop process in msec
     */
    public AutoEnding(long endTimeout) {
        this.endingTimeout = endTimeout;
    }

    /**
     * Set the process behaviour reference
     *
     * @param processBehaviour process behaviour object
     */
    public void setProcessBehaviour(MyProcessBehaviour processBehaviour) {
        this.processBehaviour = processBehaviour;
    }

    /**
     * Ask to the internal thread to stop its job,
     * When this function return the internal thread have stopped.
     * @exception InternalErrorException
     */
    public void end() throws InternalErrorException {

        if (stopped == false) {
            try {
                // Lock the mutex
                mutex.lock();

                // set stop flag
                stopped = true;

                synchronized(this) {
                    stopProcess = false;
                    // We awake the thread, if it is sleeping
                    notifyAll();
                }

                // Unlock the mutex
                mutex.unlock();

                // Wait the termination of the internal thread
                this.join();
            } catch (Exception e) {
                throw new InternalErrorException();
            }
        }

    }

    /**
     * Call by the underlying thread system in a new thread
     */
    public void run() {

        try {
            // lock mutex
            mutex.lock();

            synchronized(this) {
                stopProcess = true;
                // arm timer and wait condition
                wait(endingTimeout);
            }

            // The time has expired, stop process
            if (processBehaviour != null && stopProcess) {
                processBehaviour.autoEnd();
            }

        } catch (Exception e) {
            Assert.check(false);
        } finally {
            mutex.unlock();
        }

    }

}

