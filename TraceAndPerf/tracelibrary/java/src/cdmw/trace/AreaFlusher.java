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


package cdmw.trace;

import java.util.Vector;
import java.util.Hashtable;
import java.util.Set;
import java.util.Map;
import java.util.Iterator;
import java.util.NoSuchElementException;

import cdmw.common.Assert;

import com.thalesgroup.CdmwTrace.TraceProducerPackage.Registration;
import com.thalesgroup.CdmwTrace.TraceProducerPackage.InvalidRegistration;

/**
 * This class output the content of a FlushArea
 * to the registered Collectors.
 * 
 * Features:
 * Thread safe.
 *
 * @see  FlushAreaMngr
 * @see  FlushArea
 *
 */
public class AreaFlusher extends Thread {

    /**
     * This class describes a collector
     */
    class CollectorDescriptor {

        /**
         * The object reference of collector
         */
        public com.thalesgroup.CdmwTrace.Collector collectorRef;

        /**
         * The name of collector object
         */
        public String collectorObjName;

        /**
         * The mnemonic of collector
         */
        public String collectorMnemoName;

        /**
         * The registration id for the collector
         */
        public int collectorId;

        /**
         * Class constructor
         *
         * @param collectorRef The object reference of collector
         * @param collectorObjName The name of collector object
         * @param collectorMnemoName The mnemonic of collector
         * @param collectorId The registration id for the collector
         */
        public CollectorDescriptor(
            com.thalesgroup.CdmwTrace.Collector collectorRef,
            String collectorObjName,
            String collectorMnemoName,
            int collectorId) {
            this.collectorRef = collectorRef;
            this.collectorObjName = collectorObjName;
            this.collectorMnemoName = collectorMnemoName;
            this.collectorId = collectorId;
        }
    }

    /**
     * State of data flushing used to send data to collectors.
     * The thread calling flush_area() is blocked until all
     * data have been transmitted.
     */
    public static final int SYNCHRONOUS = 0;

    /**
     * State of data flushing used to send data to collectors.
     * The thread calling flush_area() returns immediatly, the
     * data will be sent to collectors in another thread.
     */
    public static final int ASYNCHRONOUS = 1;

    /**
     * Specify the current mode used to send data to Collectors.
     */
    private int flushState;

    /**
     * Contains the list of all FlushArea ready to be flushed
     * but not already flushed.
     * <br> (Vector is synchronized)
     */
    private Vector areaToBeFlushed;

    /**
     * Contains the list of all collector registered
     * to be notified when there is messages.
     * <br> (a Hashtable is synchronized and slightly faster
     *       than a HashMap synchronized with Collections.synchronizedMap)
     */
    private Hashtable collectors;

    /**
     * Next key available to be assigned to collectors
     * after their registration.
     */
    private int keyCollectors;

    /**
     * At each iteration the thread test this variable, to
     * see if it must stop it's processing. If true
     * this thread terminates.
     */
    private boolean threadMustTerminate;

    /**
     * If there is no area to be flushed, the thread wait
     * on this monitor. When an area to be flushed is added,
     * or when the thread must be stopped, this monitor is notified
     */
    private Object threadMonitor;
    
    /**
     * When this variable becomes true, the internal thread
     * can flush an area when timer triggered
     */
    private boolean flushActive = false;

    /**
     * Class constructor.
     * Thread Name will be "cdmw.trace.AreaFlusher_thread"
     */
    public AreaFlusher() {
        super("cdmw.trace.AreaFlusher_thread");
        flushState = ASYNCHRONOUS;
        areaToBeFlushed = new Vector();
        collectors = new Hashtable();
        keyCollectors = 0;
        threadMustTerminate = false;
        threadMonitor = new Object();
    }

    /**
     * Register the specified Collector. When message
     * will be available, the collector will received all
     * message. The collector may be suppress of the
     * receiver list, by calling AreaFlusher.unregisterCollector
     *
     * @param collector the collector to be notified from now
     * @param collectorObjName the name of collector object
     * @param collectorMnemoName the mnemonic name of collector
     * @return the registration id of collector
     *
     * @exception InvalidRegistration
     *
     * @see AreaFlusher#unregisterCollector
     */
    public int registerCollector(
        com.thalesgroup.CdmwTrace.Collector collector,
        String collectorObjName,
        String collectorMnemoName)
        throws InvalidRegistration {

        if (keyCollectors < Integer.MAX_VALUE) {

            // We assign the next ID
            int newKey = keyCollectors++;

            // create Collector descriptor
            CollectorDescriptor collectorDesc =
                new CollectorDescriptor(
                    collector,
                    collectorObjName,
                    collectorMnemoName,
                    newKey);

            // insert new collector in the map
            collectors.put(new Integer(newKey), collectorDesc);

            return newKey;

        } else {
            throw new InvalidRegistration(
                "max collector id reached " + keyCollectors);
        }

    }

    /**
     * Unregister the specified Collector. The id to used
     * has been returned from a previous call to registerCollector
     * The specified collector will no more receive message.
     * <br> If there is messages in processing, the Collector will
     * received these messages, but no more after.
     * <br> If the id doesn't correspond to an existing collector,
     * the call return without doing anything.
     *
     * @param idCollector the id of the collector to be unregistred from now
     *
     * @exception InvalidRegistration
     *
     * @see AreaFlusher#registerCollector
     */
    public void unregisterCollector(int idCollector)
        throws InvalidRegistration {
        if (collectors.remove(new Integer(idCollector)) == null) {
            // if collector not exist, throw exception
            throw new InvalidRegistration(
                "not found collector id :" + idCollector);
        }
    }

    /**
     * Get the sequence of registered collectors
     *
     * @return The sequence of registration
     */
    public Registration[] getRegisteredCollectors() {
        Registration[] registrations;

        // syncronize collectors to prevent modification
        synchronized (collectors) {

            // get a set of registred collectors
            Set s = collectors.entrySet();

            // allocate the registration sequence
            // with the number of registered collectors
            registrations = new Registration[s.size()];

            Iterator it = s.iterator();
            int i = 0;

            // fill sequence with collector mnemonic name, object name and ident
            while (it.hasNext()) {
                Map.Entry entry = (Map.Entry) it.next();
                CollectorDescriptor collectorDesc =
                    (CollectorDescriptor) entry.getValue();

                registrations[i] =
                    new Registration(
                        collectorDesc.collectorId,
                        collectorDesc.collectorMnemoName,
                        collectorDesc.collectorObjName);
                i++;
            }
        }

        return registrations;
    }

    /**
     * Change the current FlushState.
     *
     * @param flushState new FlushState
     *
     * @see AreaFlusher#SYNCHRONOUS
     * @see AreaFlusher#ASYNCHRONOUS
     */
    public void setFlushState(int flushState) {
        Assert.check(
            (flushState == SYNCHRONOUS) || (flushState == ASYNCHRONOUS));

        this.flushState = flushState;
    }

    /**
     *  Take possession of the specified FlushArea
     *  and schedule it to be flushed out ot the
     *  collector.
     *  <br> If the AreaFluser is set to SYNCHRONOUS, the
     *  the area is immediately flushed from the caller
     *  thread.
     *  <br> If the AreaFluser is set to ASYNCHRONOUS, the
     *  FlushArea is recorded, and will be flushed after
     *  from another thread.
     *
     * @param flushArea area to be flushed
     */
    public void flushArea(FlushArea flushArea) {
        switch (flushState) {
            case SYNCHRONOUS :
                doSynchronousFlush(flushArea);
                break;

            case ASYNCHRONOUS :
                doAsynchronousFlush(flushArea);
                break;

            default :
                //CDMW_NEVER_HERE()
                Assert.check(false);
                break;
        }
    }

    /**
     * Flush the specified FlushArea from the caller thread
     *
     * @param flushArea the are to be flushed out
     *
     * @see AreaFlusher#doAsynchronousFlush
     */
    protected void doSynchronousFlush(FlushArea flushArea) {
        if (flushActive) {
            // We directly asked to send the FlushArea
            doFlushArea(flushArea);
        }
    }

    /**
     * Flush the specified FlushArea from a background
     * thread.
     *
     * @param flushArea the are to be flushed out
     *
     * @see AreaFlusher#doSynchronousFlush
     */
    protected void doAsynchronousFlush(FlushArea flushArea) {
        // We insert it in the list of area to be flushed
        areaToBeFlushed.add(flushArea);

        // We awake the thread, if it is sleeping
        synchronized (threadMonitor) {
            threadMonitor.notify();
        }
    }

    /**
     * Send the specified FlushArea to all registered
     * Collectors
     *
     * @param flushArea the are to be flushed out
     */
    protected void doFlushArea(FlushArea flushArea) {
        // Firstly, we make a copy of the collectors to be notified
        //  to avoid to lock it a long time
        com.thalesgroup.CdmwTrace.Collector[] collectorsList;

        synchronized (collectors) {
            // get a set of registred collectors
            Set s = collectors.entrySet();

            // allocate the registration sequence
            // with the number of registered collectors
            collectorsList = new com.thalesgroup.CdmwTrace.Collector[s.size()];

            Iterator it = s.iterator();
            int i = 0;

            // fill list with collectors
            while (it.hasNext()) {
                Map.Entry entry = (Map.Entry) it.next();
                CollectorDescriptor collectorDesc =
                    (CollectorDescriptor) entry.getValue();
                collectorsList[i] = collectorDesc.collectorRef;
                i++;
            }
        }

        // We transform the FlushArea into a sequence of message
        com.thalesgroup.CdmwTrace.FormattedMessage[] formattedMessageSeq =
            flushArea.createFormattedMessageSeq();
        com.thalesgroup.CdmwTrace.MessagesHeader messagesHeader =
            flushArea.createMessagesHeader();

        // And we sent it to each Collector
        for (int i = 0; i < collectorsList.length; i++) {
            sendFlushArea(
                collectorsList[i],
                messagesHeader,
                formattedMessageSeq);
        }

        // We can delete the content of the flush area and release
        // this flush area to the flush area manager
        flushArea.reset();
        FlushAreaMngr.getInstance().putFlushedArea(flushArea);
    }

    /**
     * Send content of a flushArea to the
     *   specified Collector
     *
     * @param collector the receiver Collector
     * @param messagesHeader MessagesHeader for the flushArea
     * @param formattedMessageSeq sequence of formatted messages of the flushArea
     */
    protected void sendFlushArea(
        com.thalesgroup.CdmwTrace.Collector collector,
        com.thalesgroup.CdmwTrace.MessagesHeader messagesHeader,
        com.thalesgroup.CdmwTrace.FormattedMessage[] formattedMessageSeq) {
        // If we get a trouble with CORBA, we ignore it, the message will be loss for
        // this collector. Others collectors may exists, an this one may be done
        try {
            collector.put_formatted_messages(
                messagesHeader,
                formattedMessageSeq);
        } catch (org.omg.CORBA.SystemException e) {
             System.out.println("sendFlushArea SystemException:" + e.toString());
            //TODO: maybe retry, or notify a supervision
        }
    }

    /**
     * Returns the next FlushArea to be sent to
     *   the registered Collectors
     *
     * @return the next FlushArea to be flushed out
     */
    protected FlushArea getNextFlushArea() {
        // We check, if someone has not ask the thread to terminate
        while (!threadMustTerminate) {

            if (!areaToBeFlushed.isEmpty() && flushActive) {
                // If there is Area to be flushed, we extract the next one
                //  and return it
                return (FlushArea) areaToBeFlushed.remove(0);

            } else {
                // If there is no area to be flushed, we wait for new area
                // or to be stopped.
                synchronized (threadMonitor) {
                    try {
                        threadMonitor.wait();
                    } catch (InterruptedException ie) {
                        // ignore
                    }
                }
            }
        }

        // Here the thread must terminate
        return null;
    }

    /**
     * Call by the underlying thread system in a new thread
     */
    public void run() {
        // Infinite loop, we stop when getNextFlushArea()
        //  returns NULL meaning that someone has ask
        //  to stop the thread.
        do {
            FlushArea flushArea = getNextFlushArea();

            if (flushArea != null) {
                doFlushArea(flushArea);
            }
            // null is returned : stop the thread
            else {
                try {
                    // before stopping flush remaining areas to be flushed
                    java.util.Enumeration e = areaToBeFlushed.elements();
                    for (; e.hasMoreElements();) {
                        doFlushArea(flushArea);
                    }
                } catch (Exception e) {
                    // do nothing
                }
                return;
            }
        } while (true);
    }

    /**
     * Ask to the internal thread to stop its job. The thread
     * will exit only when the current FlushArea, if it is the case,
     * will have been flushed to the collector. When this function
     * return the internal thread have stopped.
     */
    public synchronized void stopThread() {

        // We require the thread to terminate
        threadMustTerminate = true;

        // We awake the thread, if it is sleeping
        synchronized (threadMonitor) {
            threadMonitor.notify();
        }

        // We wait the termination of our internal thread
        try {
            this.join();
        } catch (InterruptedException e) {
            // ignore
        }
    }
    
    /**
     * Activate trace message flushing by AreaFlusher
     */
    public void activateFlushing () {
        this.flushActive = true;
    
        // We awake the thread, if it is sleeping
        synchronized (threadMonitor) {
            threadMonitor.notify();
        }
    }

    /**
     * Deactivate trace message flushing by AreaFlusher
     */
    public void deactivateFlushing () {
        this.flushActive = false;
    }

}

