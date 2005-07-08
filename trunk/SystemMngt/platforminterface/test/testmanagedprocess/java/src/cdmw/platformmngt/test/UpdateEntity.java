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

import cdmw.common.Assert;
import cdmw.common.InternalErrorException;

import cdmw.ossupport.OS;
import cdmw.ossupport.Mutex;

/**
 * This class implements the updating of entity
 *
 * Features: Thread safe, exception safe
 *
 */
public class UpdateEntity extends Thread {

    /**
     * At each iteration the thread test this variable, to
     * see if it must stop it's processing. If true
     * the thread terminates.
     */
    private boolean stopped = false;    

    /**
     * Mutex protecting stop flag and associated to condition
     */
    private Mutex mutex = new Mutex();

    /**
     * updating interval used to reactivate the thread in msec
     */
    private long updatingInterval;

    /**
     * EntityToUpdate object
     */
    private EntityToUpdate entityToUpdate;
    
    /**
     * Boolean to detect timeout
     */
    private boolean updateProcess = false;

    /**
     * Constructor
     * 
     * @param UpdateEntityable reference to UpdateEntityable object
     * @param monitoringInterval monitoring interval used to reactivate 
     * the thread in msec
     * @param monitoringTimeout timeout used by UpdateEntityingCall object
     * to wait response of UpdateEntityable object during polling
     */
    public UpdateEntity(EntityToUpdate entityToUpdate, long updatingInterval) {
        this.updatingInterval = updatingInterval;
        this.entityToUpdate = entityToUpdate;
    }

    /**
     * Ask to the internal thread to stop its job, 
     *     When this function return the internal thread have stopped.
     */ 
    public void end() throws InternalErrorException {
    
        if (stopped == false) {
            try {
                // Lock the mutex
                mutex.lock();

                // set stop flag
                stopped = true;

                synchronized(this) {
                    updateProcess = false;
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
                updateProcess = true;
                // arm timer and wait condition
                wait(updatingInterval);
            }

            // The time has expired, start updating
            try {
                if (entityToUpdate != null && updateProcess) {
                    entityToUpdate.update();
                }
            } catch(Exception e) {
                e.printStackTrace();
            }
            

        } catch (Exception e) {
            Assert.check(false);
        } finally {
            mutex.unlock();
        }
    
    }

}

