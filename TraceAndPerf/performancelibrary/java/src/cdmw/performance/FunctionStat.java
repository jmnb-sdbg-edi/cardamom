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


package cdmw.performance;


/**
 * This class is used as a container to store the Probe measure for an API call
 * 
 * Features:
 * Thread Safe
 *
 * @see ProbeCollector 
 */
public class FunctionStat {

    /**
     * name of the API function
     */
    private String functionName;
    
    /**
     * minimum elapsed time spent in API in microseconds
     */
    private long minElapsedTime;

    /**
     * maximum elapsed time spent in API in microseconds
     */
    private long maxElapsedTime;

    /**
     * total elapsed time spent in API in microseconds
     * by all the threads
     */
    private long sumElapsedTime;

    /**
     * number of times this API has been called by all the threads
     */
    private int hitCount;
    
    

    /**
     * Constructs a new Function Stat with an api name,
     * and all stats set to 0.
     * 
     * @param apiName  A string to indicate the API name
     */     
    public FunctionStat(String apiName) {
        functionName = apiName;
        minElapsedTime = 0;
        maxElapsedTime = 0;
        sumElapsedTime = 0;
        hitCount = 0;
    }
    
    
    /**
     * Update the object with the new elapsed time spent in the API.
     * 
     * @param elapsedTime A long containing the new elapsed time
     */    
    public synchronized void update(long elapsedTime) {
        if (minElapsedTime == 0 || elapsedTime < minElapsedTime) {
            minElapsedTime = elapsedTime;
        }
        
        if (elapsedTime > maxElapsedTime) {
            maxElapsedTime = elapsedTime;
        }
        
        sumElapsedTime += elapsedTime;
        hitCount++;
    }
    
    /**
     * Get the API function name.
     *
     * @return A string containing the API function Name.
     */
    public String getFunctionName() {
        return functionName;
    }
    
    /**
     * Get the minimum time spent in the API.
     *
     * @return A long containing the minimum time spent in the API.
     */
    public long getMinElapsedTime() {
        return minElapsedTime;
    }
    
    /**
     * Get the maximum time spent in the API.
     *
     * @return A long containing the maximum time spent in the API.
     */
    public long getMaxElapsedTime() {
        return maxElapsedTime;
    }
    
    /**
     * Get the total of time spent in the API by all the threads.
     *
     * @return A long containing the total time spent in the API.
     */
    public long getSumElapsedTime() {
        return sumElapsedTime;
    }
    
    /**
     * Get the number of time spent the API has been by all the threads
     *
     * @return A long containing the number of time the API has been called
     */
    public int getHitCount() {
        return hitCount;
    }
    
    
}

