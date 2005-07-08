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


package cdmw.performance;

/**
 * This class is used to store the time to process one API call.
 * 
 * Features:
 * Time is stored in a thread-safe way
 *
 * @see ProbeCollector 
 */
public class Probe {

    /**
     * Status of the probe when it just has been constructed.
     */
    private static final int PROBE_IDLE    = 0;
    
    /**
     * Status of the probe when it have been started.
     */
    private static final int PROBE_STARTED = 1;

    /**
     * Status of the probe when it have been stopped.
     */
    private static final int PROBE_STOPPED = 2;

    
    
    /**
     * Name of the API.
     */
    private String apiName;
    
    /**
     * Flag to indicate that start time is recorded by constructor.
     */
    private boolean autoStart;
    
    /**
     * Probe status: idle, started or stopped
     *
     * @see PROBE_IDLE PROBE_STARTED PROBE_STOPPED
     */
    private int probeStatus;
    
    /**
     * Reference to the Probe collector singleton.
     */
    private ProbeCollector probeCollector;
    
    /**
     * Time of start.
     */
    private cdmw.ossupport.Timeval startTime;
    
    /**
     * Time of stop.
     */
    private cdmw.ossupport.Timeval stopTime;
    
    
    
    /**
     * Constructs a new Probe and start time measure if autoStart.
     * 
     * @param apiName  A string to indicate the API name to measure
     * @param autoStart A boolean to indicate to store start time 
     *                 automatically by constructor.
     *
     * @exception BadParameterException If apiName is null.
     */     
    public Probe(String apiName, boolean autoStart) 
    throws cdmw.common.BadParameterException {
        // API name must be defined
        if (apiName == null) {
            throw new cdmw.common.BadParameterException("API name", "NULL");
        }
        
        this.apiName = apiName;
        this.autoStart = autoStart;
        this.probeStatus = PROBE_IDLE;
        
        // get the Probe Collector singleton
        probeCollector = ProbeCollector.getInstance();
        
        // if auto start is set : start measure
        if (autoStart) {
            start();
        }        
    }
    
    
    /**
     * Constructs a new Probe and start time measure.
     * 
     * @param apiName  A string to indicate the API name to measure
     *
     * @exception BadParameterException If apiName is null.
     */         
    public Probe(String apiName) 
    throws cdmw.common.BadParameterException {
        this(apiName, true);
    }
    
    
    /**
     * Called by the garbage collector when garbage collection determines 
     * that there are no more references this Probe. If Probe has been
     * automatically started, and if it has not been stopped, this method 
     * stop the Probe.
     * 
     * <b>WARNING:</b> This method could be called a long time after
     * the probe get out of scope, or be never called. We strongly recommand
     * to explicitly call stop()
     *
     * @see Probe#stop
     */
    public void finalize() {
        if (autoStart) {
            stop();
        }    
    }
    
    
    /**
     * Start the time measure.
     */
    public void start() {
        if (probeCollector.isActivated()) {
            if (probeStatus == PROBE_IDLE || probeStatus == PROBE_STOPPED) {
                startTime = cdmw.ossupport.OS.getTime();
                probeStatus = PROBE_STARTED;
            }
        }
    }

    /**
     * Stop the time measure.
     */    
    public void stop() {
        if (probeStatus == PROBE_STARTED) {
            if (probeCollector.isActivated()) {
                stopTime = cdmw.ossupport.OS.getTime();
                probeCollector.addProbe(this);
            }
            probeStatus = PROBE_STOPPED;
        }        
    }
    
    /**
     * Get the elapsed time.
     *
     * @return The elapsed time in microseconds.
     */
    public long getElapsedTime() {
        long elapsedTime = 0;
        
        elapsedTime = (stopTime.getSeconds() - startTime.getSeconds()) * 1000000;
        elapsedTime += (stopTime.getMicroseconds() - startTime.getMicroseconds());
        
        return elapsedTime;
    }
    
    
    /**
     * Get the API name.
     *
     * @return A string containing the API Name.
     */
    public String getApiName() {
        return apiName;
    }
    
    
    /**
     * Set the API name.
     *
     * @param apiName A string containing the new API name.
     */
    public void setApiName(String apiName) {
        this.apiName = apiName;
    }
    
    
    
}

