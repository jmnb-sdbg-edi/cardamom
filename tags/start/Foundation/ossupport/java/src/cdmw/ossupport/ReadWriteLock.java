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


package cdmw.ossupport;

import java.util.Vector;
import java.util.Enumeration;

import cdmw.common.Assert;

/**
 * This class allow to protect data against concurrent
 * access. The ReadWriteLock class allows more than one
 * thread access the data for reading, but only one thread
 * to access data for writing
 *
 * Features:
 * Thread safe, exception safe
 *
 * @see  Mutex
 *
 */
public class ReadWriteLock {

    /**
     * Number of readers currently locking the ReaderWriterLock.
     */
    private int nbActiveReaders = 0;

    /**
     * Number of writers currently locking the ReaderWriterLock.
     * This variable should be equal to 0 or 1.
     */
    private int nbActiveWriters = 0;

    /**
     * Number of readers currently waiting for the ReaderWriterLock.
     */
    private int nbWaitingReaders = 0;

    /**
     * Number of writers currently waiting for the ReaderWriterLock.
     */
    private int nbWaitingWriters = 0;

    /**
     * Each reader/writer thread must acquire this mutex before
     * changing data of the ReaderWriterLock.
     */
    private final Mutex mutex = new Mutex();
    
    /**
     * Lock to be acquired to read
     */
    protected final ReaderLock readerLock = new ReaderLock();
    
    /**
     * Lock to be acquired to write
     */
      protected final WriterLock writerLock = new WriterLock();

    /**
     * Debug boolean, default value is false.
     */
    private boolean debug = false;

    /**
     * Constructor
     */
    public ReadWriteLock() {
        // Nothing to do
    }

    /**
     * Acquire the lock for reading, more than one
     * thread can acquire the lock for reading.
     */
    public void readLock() {

        try {
            mutex.lock();
            log("Trying to read...");

            Assert.check( nbActiveReaders >= 0 );
            Assert.check( nbActiveWriters == 0 || nbActiveWriters == 1 );

            nbWaitingReaders++;

            // If there is a writer, we need to wait
            while (nbActiveWriters > 0) {
                log("Unable to read, waiting...");
                mutex.unlock();
                readerLock.waitReader();
                mutex.lock();
            }


            nbWaitingReaders--;
            nbActiveReaders++;
            log("Reading...");

            Assert.check( nbWaitingWriters >= 0 );
            Assert.check( nbWaitingReaders >= 0 );
            Assert.check( nbActiveReaders >= 1 );
            Assert.check( nbActiveWriters == 0 );
            mutex.unlock();
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        }

    }

    /**
     * Acquire the lock for writing, only one thread can access data, 
     * when the lock is acquired for writing.
     */
    public void writeLock() {

        try {
            mutex.lock();
            log("Trying to write...");

            Assert.check( nbActiveReaders >= 0 );
            Assert.check( nbActiveWriters <= 1 );

            nbWaitingWriters++;

            // To acquire exclusive access no reader or writer must be present
            while (nbActiveReaders != 0 || nbActiveWriters != 0) {
                log("Unable to write, waiting...");
                mutex.unlock();
                writerLock.waitWriter();
                mutex.lock();
            }

            nbWaitingWriters--;
            nbActiveWriters++;
            log("Writing...");

            Assert.check( nbWaitingWriters >= 0 );
            Assert.check( nbWaitingReaders >= 0 );
            Assert.check( nbActiveReaders == 0 );
            Assert.check( nbActiveWriters == 1 );
            mutex.unlock();
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        }

    }

    /**
     * Release the lock previously acquired for reading.
     */
    public void readUnlock() {

        try {
            mutex.lock();
            Assert.check( nbActiveReaders >= 1 );
            Assert.check( nbActiveWriters == 0 );

            nbActiveReaders--;
            log("Reading done...");

            // If there is no more reader, we allow writer to access
            if ( nbActiveReaders == 0 && nbWaitingWriters != 0) {
                writerLock.notifyWriter();
            }

            Assert.check( nbActiveWriters == 0 );
            Assert.check( nbActiveReaders >= 0 );
            mutex.unlock();
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        }

    }


    /**
     * Release the lock previously acquired for writing.
     */
    public void writeUnlock() {

        try {
            mutex.lock();
            Assert.check( nbActiveWriters == 1 );
            Assert.check( nbActiveReaders == 0 );

            nbActiveWriters--;
            log("Writing done...");

            if ( nbWaitingReaders != 0) {
                readerLock.notifyAllReaders();
            } else if ( nbWaitingWriters != 0 ) {
                writerLock.notifyWriter();
            }

            Assert.check( nbActiveWriters == 0 );
            Assert.check( nbActiveReaders == 0 );
            mutex.unlock();
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        }

    }

    /**
     * Log comments for debugging help.
     *
     * @param m Information to be printed in the standard output
     */
    public synchronized void log(String m) {

        if (debug) {
            System.out.println("***************************");
            System.out.println(m);
            System.out.println("Current state of the lock: ");
            System.out.println("Active Readers Number: "
                + nbActiveReaders);
            System.out.println("Active Writers Number: "
                + nbActiveWriters);
            System.out.println("Waiting Readers Number: "
                + nbWaitingReaders);
            System.out.println("Waiting Writers Number: "
                + nbWaitingWriters);
            System.out.println("***************************");
        }

    }

    /**
     * This class allow to keep waiting writers and notify one of them.
     *
     * Features:
     * Thread safe, exception safe
     *
     * @see  ReadWriteLock
     *
     */
    protected class WriterLock {

        /**
         * Call a wait method on a writer.
          */
        protected synchronized void waitWriter() {
            try {
                WriterLock.this.wait();
            } catch(InterruptedException ie) {
                ie.printStackTrace();
            }
        }

        /**
         * Notify a writer.
          */
        protected synchronized void notifyWriter() {
            WriterLock.this.notify();
        }

    }

    /**
     * This class allow to keep waiting readers and notify them.
     *
     * Features:
     * Thread safe, exception safe
     *
     * @see  ReadWriteLock
     *
     */
    protected class ReaderLock {

        /**
         * Call a wait method on a reader.
          */
        protected synchronized void waitReader() {
            try {
                ReaderLock.this.wait();
            }
            catch(InterruptedException ie) {
                ie.printStackTrace();
            }
        }

        /**
         * Notify all the readers.
          */
        protected synchronized void notifyAllReaders() {
            ReaderLock.this.notifyAll();
        }

    }

}

