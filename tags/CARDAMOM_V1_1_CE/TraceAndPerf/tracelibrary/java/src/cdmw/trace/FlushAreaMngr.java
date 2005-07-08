/* =========================================================================== *
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
 * =========================================================================== */


package cdmw.trace;

import java.util.Vector;

import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

import com.thalesgroup.CdmwTrace.TraceProducerPackage.CollectorData;

/**
 * This class is the central class of the Cdmw Trace service. This
 * class is used as a singleton to provide acces to the trace system
 * from everywhere. The FlushAreaMngr is in charge of:
 *  - initialising the FlushArea used to contain message not
 *    already output to Collectors
 *  - initialising the AreaFlusher, this object is a thread
 *    which send the FlushArea to the Collectors
 *  - starting an internal thread, which trigger the flush of
 *    area although the FlushArea is not full. 
 *  - wrapped the allocation done by the FlushArea
 *  - wrapped the decision to trace or not a Domain/Level
 *    (done by the FilterMngr)
 *  - initialised the CORBA object in charge of handling client
 *    requests (TraceProducer).
 * 
 * Features:
 * Thread safe
 *
 * @see  TraceProducerImpl
 * @see  FlushArea
 * @see  FilterMngr
 * @see  AreaFlusher
 *
 */
public class FlushAreaMngr extends Thread {

    /**
     * Default number of area used to store messages
     */
    public static final int DEFAULT_AREA_NB = 2;

    /**
     * Default size of each area
     */
    public static final int DEFAULT_AREA_SIZE = 50 * 1024;

    /**
     * Default maximum number of messages
     */
    public static final int DEFAULT_AREA_NB_MESSAGES = 1024;

    /**
     * Default time for flushing
     */
    public static final int DEFAULT_FLUSHING_TIME = 5000;

    /**
     * Flush mode for flushing the data to the Collectors 
     * in a background thread.
     */
    public static final int ASYNCHRONOUS = 0;

    /**
     * Flush mode for directly transfer the message 
     * to the Collectors.
     */
    public static final int SEMI_SYNCHRONOUS = 1;

    /**
     * Flush mode for sending data to the Collectors.
     */
    public static final int LINEAR = 2;

    /**
     * Flush mode for not sending data to collectors, but keep it
     * in an internal circular buffer.
     */
    public static final int CIRCULAR = 4;

    /**
     * error message when running out of flush areas
     */
    private static final String OUT_FLUSH_AREA_MSG =
        "trace Flush Area Manager is running out of flush areas";

    /**
     * Singleton instance of FlushAreaMngr
     */
    private static FlushAreaMngr theInstance = null;

    /**
     * This variable is set to true, since the init() function has been called.
     * The cleanup() function reset this variable to false.
     */
    private static boolean isInitDone = false;

    /**
     * Object in charge of flushing data to the collectors.
     * This work may be done, depending on the current mode,
     * in a background thread or in the caller thread.
     */
    private AreaFlusher areaFlusher;

    /**
     * Minimum time in milliseconds allowed before flushing an area,
     * although the area is not full.
     */
    private int flushingTime;

    /**
     * Contains the reference handling request of the outside world:
     * registration of collector, change of levels, ...
     */
    private com.thalesgroup.CdmwTrace.TraceProducer traceProducer;

    /**
     * Hold all Domain/Level which are to be traced or not
     */
    private FilterMngr filterMngr;

    /**
     * Contains the process name
     */
    private String processName;

    /**
     * current flush area used to store messages
     */
    private FlushArea currentFlushArea;

    /**
     * set of free flush areas
     */
    private Vector freeFlushAreas;

    /**
     * flush mode for backup (LINEAR or CIRCULAR)
     */
    private int flushModeThread;

    /**
     * flush mode for thread (ASYNCHRONOUS or SEMI_SYNCHRONOUS)
     */
    private int flushModeBackup;

    /**
     * When this variable becomes true, the internal thread
     * can flush an area when timer triggered
     */
    private boolean isFlushActive;

    /**
     * When this variable becomes true, the internal thread
     * will terminate since the threadMonitor will be
     * notified.
     */
    private boolean threadMustTerminate;

    /**
     * The thread wait on this monitor during flushingTime.
     * When timeout, it makes a flushArea()
     */
    private Object threadMonitor;

    /**
     * The thread test this condition just after the wait().
     * If true (by default), a flushArea is done.
     * If we want just reset the timeout, set this to false before notify()
     */
    private boolean flushAreaToBeDone;

    /**
     * Class constructor
     *
     * @param tracePOA POA to use for trace servant.
     * @param collectorList List of registered Trace Collectors.
     * @param domain the domain where the level must be activated
     * @param level  the level to be activated
     * @param flushingTime time to wait in milliseconds before flushing
     *       the current area although it is not full.
     * @param nbFlushArea number of FlushArea to be used to store messages
     * @param sizeFlushArea size of each FlushArea to be used to store messages
     * @param nbMessages maximum number of massages in each FlushArea
     *
     * @exception BadParameterException
     * @exception InternalErrorException
     */
    private FlushAreaMngr(
        org.omg.PortableServer.POA tracePOA,
        CollectorData[] collectorList,
        String domain,
        short level,
        String processName,
        int flushingTime,
        int nbFlushArea,
        int sizeFlushArea,
        int nbMessages)
        throws BadParameterException, InternalErrorException {
        super("cdmw.trace.FlushAreaMngr_thread");
        this.processName = processName;
        this.flushingTime = flushingTime;
        this.threadMustTerminate = false;
        this.threadMonitor = new Object();

        // Default flush mode is ASYNCHRONOUS LINEAR
        flushModeThread = ASYNCHRONOUS;
        flushModeBackup = LINEAR;

        // Create the required number of FlushArea
        createFlushAreas(nbFlushArea, sizeFlushArea, nbMessages);

        // Create the thread in charge of flushing Area as a background task
        areaFlusher = new AreaFlusher();

        // Create the object in charge of determining,
        // if a specific level/domain must be traced
        filterMngr = new FilterMngr();

        // Initialize the domain and level to trace if defined
        if (domain != "") {
            activateLevel(domain, level);
        }

        // Create the Servant to handle registration request
        TraceProducerImpl traceProducerServant =
            new TraceProducerImpl(areaFlusher, filterMngr);

        // Activate servant on tracePOA
        try {
            byte[] oid = tracePOA.activate_object(traceProducerServant);
            org.omg.CORBA.Object object = tracePOA.id_to_reference(oid);
            traceProducer =
                com.thalesgroup.CdmwTrace.TraceProducerHelper.narrow(object);

        } catch (org.omg.PortableServer.POAPackage.WrongPolicy e) {
            throw new cdmw.common.BadParameterException(
                "Wrong POA Policy for activate_object",
                "NULL");
        } catch (org.omg.CORBA.UserException e) {
            System.err.println("Trace Producer servant activation error");
            System.err.println(e.toString());
            cdmw.common.Assert.assertionFailed();
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println("Trace Producer servant activation error");
            System.err.println(e.toString());
            cdmw.common.Assert.assertionFailed();
        }

        // notify each collector of birth of TraceProducer
        if (collectorList != null) {
            for (int i = 0; i < collectorList.length; i++) {

                try {
                    // call collector update to register the trace producer
                    collectorList[i].the_collector.update(traceProducer);

                    // register collector in areaFlusher
                    areaFlusher.registerCollector(
                        collectorList[i].the_collector,
                        collectorList[i].the_collectorObjName,
                        collectorList[i].the_collectorMnemoName);

                } catch (org.omg.CORBA.UserException e) {
                    System.err.println(
                        "Trace Collector access error : "
                            + collectorList[i].the_collectorObjName);
                    System.err.println(e.toString());
                } catch (org.omg.CORBA.SystemException e) {
                    System.err.println(
                        "Trace Collector access error : "
                            + collectorList[i].the_collectorObjName);
                    System.err.println(e.toString());
                }
            }
        }

        // Ok, we start the background thread in charge of fluhsing data
        areaFlusher.start();
    }

    /**
     * Called by the garbage collector on an object when garbage collection
     * determines that there are no more references to the object.
     */
    protected void finalize() {
        // If the user has not called the cleanup function
        cdmw.common.Assert.check(!isInitDone);
    }

    /**
     *  Return the singleton instance of FlushAreaMngr.
     *  init() has to be called before.
     *
     * @return the singleton instance.
     */
    public static FlushAreaMngr getInstance() {
        // The user has not called the init() function
        cdmw.common.Assert.check(isInitDone);

        return theInstance;
    }

    /**
     *  get the Process name
     *
     * @return A string containing the Process Name
     */
    public String getProcessName() {
        return processName;
    }

    /**
     *  Initialize the Trace library. This function must be called
     *   before any other call. The Trace service must be closed
     *   by a call to cleanup().
     *
     * @param tracePOA POA to use for trace servant.
     * @param collectorList List of registered Trace Collectors.
     * @param domain the domain where the level must be activated
     * @param level  the level to be activated
     * @param processName the name of the process
     * @param flushingTime time to wait before flushing the current area
     *       although it is not full.
     * @param nbFlushArea number of FlushArea to be used to store messages
     * @param sizeFlushArea size of each FlushArea to be used to store messages
     * @param nbMessages maximum number of massages in each FlushArea
     *
     * @return the pointer to the TraceProducer servant object is returned
     *
     * @exception BadParameterException
     * @exception InternalErrorException
     */
    public static com.thalesgroup.CdmwTrace.TraceProducer init(
        org.omg.PortableServer.POA tracePOA,
        CollectorData[] collectorList,
        String domain,
        short level,
        String processName,
        int flushingTime,
        int nbFlushArea,
        int sizeFlushArea,
        int nbMessages)
        throws BadParameterException, InternalErrorException {
        // The user try to initialise twice the tracelibrary
        cdmw.common.Assert.check(!isInitDone);

        theInstance =
            new FlushAreaMngr(
                tracePOA,
                collectorList,
                domain,
                level,
                processName,
                flushingTime,
                nbFlushArea,
                sizeFlushArea,
                nbMessages);

        // Start the thread in charge of flushing in case of timeout
        theInstance.start();

        isInitDone = true;

        // Return the reference on the object managing communication with
        // collector, it will be registered in the repository by the caller
        return theInstance.traceProducer;
    }

    /**
     * Initialize the Trace library with default value for nbMessages.
     * This function must be called before any other call. 
     * The Trace service must be closed by a call to cleanup().
     *
     * @param tracePOA POA to use for trace servant.
     * @param collectorList List of registered Trace Collectors.
     * @param domain the domain where the level must be activated
     * @param level  the level to be activated
     * @param processName the name of the process
     * @param flushingTime time to wait before flushing the current area
     *       although it is not full.
     * @param nbFlushArea number of FlushArea to be used to store messages
     * @param sizeFlushArea size of each FlushArea to be used to store messages
     *
     * @return the pointer to the TraceProducer servant object is returned
     *
     * @exception BadParameterException
     * @exception InternalErrorException
     */
    public static com.thalesgroup.CdmwTrace.TraceProducer init(
        org.omg.PortableServer.POA tracePOA,
        CollectorData[] collectorList,
        String domain,
        short level,
        String processName,
        int flushingTime,
        int nbFlushArea,
        int sizeFlushArea)
        throws BadParameterException, InternalErrorException {
        // The user try to initialise twice the tracelibrary
        cdmw.common.Assert.check(!isInitDone);

        theInstance =
            new FlushAreaMngr(
                tracePOA,
                collectorList,
                domain,
                level,
                processName,
                flushingTime,
                nbFlushArea,
                sizeFlushArea,
                DEFAULT_AREA_NB_MESSAGES);

        // Start the thread in charge of flushing in case of timeout
        theInstance.start();

        isInitDone = true;

        // Return the reference on the object managing communication with
        // collector, it will be registered in the repository by the caller
        return theInstance.traceProducer;
    }

    /**
     *  Initialize the Trace library with default values for
     *   sizeFlushArea and nbMessages.
     *   This function must be called before any other call.
     *   The Trace service must be closed by a call to cleanup().
     *
     * @param tracePOA POA to use for trace servant.
     * @param collectorList List of registered Trace Collectors.
     * @param domain the domain where the level must be activated
     * @param level  the level to be activated
     * @param processName the name of the process
     * @param flushingTime time to wait before flushing the current area
     *       although it is not full.
     * @param nbFlushArea number of FlushArea to be used to store messages
     *
     * @return the pointer to the TraceProducer servant object is returned
     *
     * @exception BadParameterException
     * @exception InternalErrorException
     */
    public static com.thalesgroup.CdmwTrace.TraceProducer init(
        org.omg.PortableServer.POA tracePOA,
        CollectorData[] collectorList,
        String domain,
        short level,
        String processName,
        int flushingTime,
        int nbFlushArea)
        throws BadParameterException, InternalErrorException {
        return init(
            tracePOA,
            collectorList,
            domain,
            level,
            processName,
            flushingTime,
            nbFlushArea,
            DEFAULT_AREA_SIZE,
            DEFAULT_AREA_NB_MESSAGES);
    }

    /**
     *  Initialize the Trace library with default values for
     *   nbFlushArea, sizeFlushArea and nbMessages.
     *   This function must be called before any other call.
     *   The Trace service must be closed by a call to cleanup().
     *
     * @param tracePOA POA to use for trace servant.
     * @param collectorList List of registered Trace Collectors.
     * @param domain the domain where the level must be activated
     * @param level  the level to be activated
     * @param processName the name of the process
     * @param flushingTime time to wait before flushing the current area
     *       although it is not full.
     *
     * @return the pointer to the TraceProducer servant object is returned
     *
     * @exception BadParameterException
     * @exception InternalErrorException
     */
    public static com.thalesgroup.CdmwTrace.TraceProducer init(
        org.omg.PortableServer.POA tracePOA,
        CollectorData[] collectorList,
        String domain,
        short level,
        String processName,
        int flushingTime)
        throws BadParameterException, InternalErrorException {
        return init(
            tracePOA,
            collectorList,
            domain,
            level,
            processName,
            flushingTime,
            DEFAULT_AREA_NB,
            DEFAULT_AREA_SIZE,
            DEFAULT_AREA_NB_MESSAGES);
    }

    /**
     *  Initialize the Trace library with default values for
     *   flushingTime, nbFlushArea, sizeFlushArea and nbMessages.
     *   This function must be called before any other call.
     *   The Trace service must be closed by a call to cleanup().
     *
     * @param tracePOA POA to use for trace servant.
     * @param collectorList List of registered Trace Collectors.
     * @param domain the domain where the level must be activated
     * @param level  the level to be activated
     * @param processName the name of the process
     *
     * @return the pointer to the TraceProducer servant object is returned
     *
     * @exception BadParameterException
     * @exception InternalErrorException
     */
    public static com.thalesgroup.CdmwTrace.TraceProducer init(
        org.omg.PortableServer.POA tracePOA,
        CollectorData[] collectorList,
        String domain,
        short level,
        String processName)
        throws BadParameterException, InternalErrorException {
        return init(
            tracePOA,
            collectorList,
            domain,
            level,
            processName,
            DEFAULT_FLUSHING_TIME,
            DEFAULT_AREA_NB,
            DEFAULT_AREA_SIZE,
            DEFAULT_AREA_NB_MESSAGES);
    }

    /**
     *  Activate trace message flushing by AreaFlusher
     */
    public void activateFlushing() {
        isFlushActive = true;
        this.areaFlusher.activateFlushing();
    }

    /**
     *  Deactivate trace message flushing by AreaFlusher
     */
    public void deactivateFlushing() {
        isFlushActive = false;
        this.areaFlusher.deactivateFlushing();
    }

    /**
     *  Set the way the data are sent or not the Collectors.
     *  
     *   - ASYNCHRONOUS: the data are flushed by a background thread
     *   - SEMI_SYNCHRONOUS: when a message is fully built, it send to Collectors
     *   - LINEAR: the data are sent to Collectors
     *   - CIRCULAR: the data are not sent, but kept in an internal buffer.
     *
     * @param flushMode the new flushMode
     *
     * @exception BadParameterException
     *
     * @see FlushAreaMngr#ASYNCHRONOUS
     * @see FlushAreaMngr#SEMI_SYNCHRONOUS
     * @see FlushAreaMngr#LINEAR
     * @see FlushAreaMngr#CIRCULAR
     */
    public synchronized void setFlushMode(int flushMode)
        throws BadParameterException {
        switch (flushMode) {
            case CIRCULAR :
                flushModeBackup = CIRCULAR;
                flushModeThread = SEMI_SYNCHRONOUS;
                break;

            case LINEAR :
                flushModeBackup = LINEAR;
                break;

            case ASYNCHRONOUS :
                flushModeBackup = LINEAR;
                flushModeThread = ASYNCHRONOUS;
                areaFlusher.setFlushState(AreaFlusher.ASYNCHRONOUS);
                break;

            case SEMI_SYNCHRONOUS :
                flushModeThread = SEMI_SYNCHRONOUS;
                areaFlusher.setFlushState(AreaFlusher.SYNCHRONOUS);
                break;

            default :
                throw new BadParameterException(
                    "flushMode must be ASYNCHRONOUS, "
                        + "SEMI_SYNCHRONOUS, LINEAR or CIRCULAR");
        }
    }

    /**
     *  Defines the specified level as activated
     *
     * @param domain the domain where the level must be activated
     * @param level  the level to be activated
     */
    public void activateLevel(String domain, short level) {
        filterMngr.activateLevel(domain, level);
    }

    /**
     *  Defines the specified level as deactivated
     *
     * @param domain the domain where the level must be deactivated
     * @param level  the level to be deactivated
     */
    public void deactivateLevel(String domain, short level) {
        filterMngr.deactivateLevel(domain, level);
    }

    /**
     * Returns true if the specified level has been noted
     * to be traced
     *
     * @param domain the Domain from where the level status must be returned
     * @param level the level to be interrogated
     *
     * @return true if the level is to be traced
     */
    public static boolean isToBeTraced(String domain, short level) {
        if (!isInitDone) {
            // The user has not called FlushAreaMngr::init() before
            // message is not traced in this case
            return false;

        } else {
            return theInstance.filterMngr.isActivated(domain, level);
        }
    }

    /**
     *  Fill the internal store of flush area with FlushArea, whose
     * dimension are specified.
     *
     * @param nbFlushArea number of FlushArea to be used
     * @param sizeFlushArea size of each FlushArea to be used to store messages
     * @param nbMessages maximum number of messages in each FlushArea
     */
    protected synchronized void createFlushAreas(
        int nbFlushArea,
        int sizeFlushArea,
        int nbMessages) {
        currentFlushArea = new FlushArea(nbMessages, sizeFlushArea, 0);
        freeFlushAreas = new Vector(nbFlushArea - 1);

        for (int i = 1; i < nbFlushArea; i++) {
            freeFlushAreas.add(new FlushArea(nbMessages, sizeFlushArea, i));
        }
    }

    /**
     *  Takes ownership of the specified flush area that
     * has been released by the area flusher.
     *
     * @param flushedArea the FlushArea to take ownership
     */
    protected void putFlushedArea(FlushArea flushedArea) {
        cdmw.common.Assert.check(flushedArea != null);

        if (currentFlushArea == null) {
            // Reuse immediately the flushed area
            currentFlushArea = flushedArea;

            // reset the timeout
            synchronized (threadMonitor) {
                flushAreaToBeDone = false;
                threadMonitor.notify();
            }
        } else {
            freeFlushAreas.add(flushedArea);
        }
    }

    /**
     *  Adds the specified message for flushing
     *
     * @param header the header of the message
     * @param body   the body of the message
     *
     * @return true if the message has been successfully added
     */
    public synchronized boolean addMessage(MessageHeader header, String body) {
        cdmw.common.Assert.check(areaFlusher != null);

        boolean retry;

        do {
            retry = false;

            if (currentFlushArea == null) {
                return false;
            }

            // Try to add message in current flush area
            int areaStatus = currentFlushArea.addMessage(header, body);

            switch (areaStatus) {
                case FlushArea.OK :
                    // nothing to do; true will be returned
                    break;

                case FlushArea.AREA_FULL :
                    // flush immediately the current area
                    flushArea();
                    // reset the timeout
                    synchronized (threadMonitor) {
                        flushAreaToBeDone = false;
                        threadMonitor.notify();
                    }
                    break;

                case FlushArea.SIZE_TOO_SMALL :
                    // flush immediately the current area
                    flushArea();
                    // try to use the next current flush area
                    retry = true;
                    break;

                case FlushArea.MESSAGE_TOO_BIG :
                    System.err.println(
                        "Warning : Cdmw::Trace::FlushAreaMngr :");
                    System.err.println(
                        "Inappropriate area size. "
                            + "Cannot manage message with body length of "
                            + body.length()
                            + " characters.");
                    return false;

                default :
                    //CDMW_NEVER_HERE
                    cdmw.common.Assert.check(false);
                    break;
            }

        } while (retry);

        return true;
    }

    /**
     *  Sends the current flush area to the AreaFlusher thread
     * according to the flush mode.
     * Called when the current flush area is full and when
     * the timeout has expired
     */
    protected synchronized void flushArea() {

            switch (flushModeBackup) {
                case LINEAR :
                    // Deliver the current flush area to the area flusher
                    deliverFlushArea();
                    break;

                case CIRCULAR :
                    // In circular mode, the area is never flushed.
                    // The contents of this area may be inspected
                    // only with a debugger.

                    // The current area is simply reset
                    currentFlushArea.reset();
                    break;

                default :
                    //CDMW_NEVER_HERE
                    cdmw.common.Assert.check(false);
                    break;
            }

    }

    /**
     *  Delivers the current flush area to the AreaFlusher
     * for actual flushing and updates the current flush area.
     *
     * If the current flush area was empty or if there was no
     * current flush area, does nothing.
     */
    protected void deliverFlushArea() {
        cdmw.common.Assert.check(areaFlusher != null);

        if (currentFlushArea != null) {
            if (!currentFlushArea.isEmpty()) {
                // save current flush area pointer
                FlushArea flushAreaToDeliver = currentFlushArea;

                // update the current flush area
                updateCurrentFlushArea();

                // if no more flush area available, store out of flush area error message
                if (currentFlushArea == null) {
                    flushAreaToDeliver.storeOutFlushAreaMsg(OUT_FLUSH_AREA_MSG);
                }

                // actually deliver to the area flusher
                areaFlusher.flushArea(flushAreaToDeliver);
            }
        }
    }

    /**
     *  Updates the current flush area with the first
     * flush area from the list of available flush areas,
     * or with null if no flush area is available.
     */
    protected void updateCurrentFlushArea() {
        if (!freeFlushAreas.isEmpty()) {
            // updates the current flush area with the first
            // element of the available flush areas.
            currentFlushArea = (FlushArea) freeFlushAreas.remove(0);
        } else {
            currentFlushArea = null;
        }
    }

    /**
     *  Call by the underlying thread system in a new thread
     */
    public void run() {
        while (!threadMustTerminate) {
            // arm timer and wait it
            synchronized (threadMonitor) {
                try {
                    flushAreaToBeDone = true;
                    threadMonitor.wait(flushingTime);
                } catch (InterruptedException e) {
                    // ignore
                }
            }

            // If the time has expired, we must flush the area
            if (!threadMustTerminate && flushAreaToBeDone) {
                flushArea();
            }
        }
    }

    /**
     *  Ask to the internal thread to stop its job.
     * When this function return the internal thread have stopped.
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

        // We stop the thread in charge of flushing data
        areaFlusher.stopThread();
    }

    /**
     *  Free all allocated memory and stop the trace service
     *
     */
    public static void cleanup() {
        // if library is initialised
        if (isInitDone) {

            // Stop our internal thread managing the timeout
            theInstance.stopThread();

            isInitDone = false;
            theInstance = null;
        }
    }

}

