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
 * This singleton class is used to store the time spent for each registered API name.
 * Statistics are recorded in Functionstat object for one API name. 
 * FunctionStat object is created if it is the first time the API is measured or is updated with
 * new measures. FunctionStat objects are holded in a map with API name as key.
 * Statistics from all FunctionStat objects can be written in a log file.
 * 
 * Features:
 * Thread safety
 *
 * @see FunctionStat 
 */
public class ProbeCollector {

    /**
     * Singleton instance
     */
    private static ProbeCollector theInstance = null;

    /**
     * performance activation
     */
    private boolean active;

    /**
     * Map of Function Stat objects 
     * indexed by API function name.
     */
    private java.util.Map functionStatMap;
    

    /**
     * Constructs a new ProbeCollector non activated
     * with an empty list of FunctionStat map.
     */
    protected ProbeCollector() {
        active = false;
        functionStatMap = new java.util.TreeMap();
    }
    
    
    /**
     * Get the singleton instance of object.
     *
     * @return The ProbeCollector singleton object
     */
    public static synchronized ProbeCollector getInstance() {
        if (theInstance == null) {
            theInstance = new ProbeCollector();
        }
        
        return theInstance;
    }
    
    
    /**
     * Manage the input Probe object or update it if already managed.
     *
     * @param probe A Probe object to manage or to update
     */
    public synchronized void addProbe(Probe probe) {
        FunctionStat functionStat = 
            (FunctionStat) functionStatMap.get(probe.getApiName());
            
        if (functionStat == null) {
            functionStat = new FunctionStat(probe.getApiName());
            functionStatMap.put(functionStat.getFunctionName(), functionStat);
        }
        
        functionStat.update(probe.getElapsedTime());        
    }
    
    
    /**
     * Write the statistics recorded in Probe objects into file.
     *
     * @param logFileName A string indicating the name of file to write.
     *
     * @exception BadParameterException If the file cannot be opened.
     */
    public synchronized void writeStatistic(String logFileName)
    throws cdmw.common.BadParameterException {
        try {
            java.io.FileOutputStream perfoLogStream = 
                new java.io.FileOutputStream(logFileName);
                
            java.io.PrintStream perfoLog = 
                new java.io.PrintStream(perfoLogStream);
                
            for (java.util.Iterator it = functionStatMap.values().iterator();
                 it.hasNext(); )
            {
                FunctionStat functionStat = (FunctionStat) it.next();
                perfoLog.println(functionStat.getFunctionName()   + ";" +
                                 functionStat.getMinElapsedTime() + ";" +
                                 functionStat.getMaxElapsedTime() + ";" +
                                 functionStat.getSumElapsedTime() + ";" +
                                 functionStat.getHitCount()
                                );
            }
            
            perfoLog.close();
            perfoLogStream.close();
            
        } catch (java.io.FileNotFoundException e) {
            throw new cdmw.common.BadParameterException(
                "Perfomance Log File cannot be opened",
                logFileName);
        } catch (java.io.IOException e) {
            System.err.println("Error while closing file '" + logFileName + "'");
        }
        
    }
    
    
    /**
     * Get the FunctionStat corresponding to the Probe.
     *
     * @param probe A Probe object.
     *
     * @return A pointer to a FunctionStat object 
     *        corresponding to input Probe object.
     */
    public FunctionStat getFunctionStat(Probe probe) {
        return (FunctionStat) functionStatMap.get(probe.getApiName());
    }
    
    
    /**
     * Get the FunctionStat corresponding to the API name.
     *
     * @param apiName A string corresponding to the API name.
     *
     * @return A pointer to a FunctionStat object 
     *        corresponding to input Probe object.
     */    
    public FunctionStat getFunctionStat(String apiName) {
        return (FunctionStat) functionStatMap.get(apiName);
    }
    
    
    /**
     * To get if performance collector is activated.
     *
     * @return A boolean to indicate the activation
     */
    public boolean isActivated() {
        return active;
    }
    
    /**
     * Store activation setting.
     *
     * @param value New activation state
     */
    public void setActivity(boolean value) {
        active = value;
    }
    
    
}

