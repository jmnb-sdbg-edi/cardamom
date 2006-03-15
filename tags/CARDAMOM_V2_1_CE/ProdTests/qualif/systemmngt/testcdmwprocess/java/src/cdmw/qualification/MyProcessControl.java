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

package cdmw.platformmngt;

import cdmw.common.BadOrderException;
import cdmw.cdmwinit.DefaultProcessControl;
import cdmw.cdmwinit.CdmwInterface;
//import cdmw.namingandrepository.RepositoryInterface;
//import com.thalesgroup.CdmwNamingAndRepository.Repository;
import cdmw.ossupport.OS;

public class MyProcessControl extends DefaultProcessControl {

    /**
     * The ORB.
     */
    private org.omg.CORBA.ORB orb;
    
    /**
     * Stream to print results
     */
    private java.io.PrintStream out;
    
    /**
     * The process name.
     */
    private String processName;

    /**
    * time to sleep in msec
    */
    private int sleepTime;

    /**
    * PullMonitorable request
    */
    private boolean request;

    /**
    * PullMonitorable exception to raise
    */
    private boolean except;

    /**
    * PullMonitorable counter
    */
    private int counter;

    /**
     * The constructor.
     *
     */ 
    public MyProcessControl(org.omg.CORBA.ORB orb, 
        java.io.PrintStream out,
        int sleepTime, 
        boolean request, 
        boolean except) {
        this.orb = orb;
        this.out = out;
        this.sleepTime = sleepTime;
        this.request = request;
        this.except = except;
        this.counter = 2;
    }



    /**
     * This operation informs the pull-based Fault Detector whether the object is able to
     *    accept requests and produce replies.
     *    This operation may be used to perform some user-defined sanity checks ...
     *
     * @return true if the object is alive and ready to take further requests, and false otherwise.
     *
     */
    public boolean isAlive() {
        try { 
            processName = PlatformInterface.getProcessName();
            counter = counter - 1;

            OS.sleep(sleepTime);
            out.println(processName + " is alive called, liveliness:" + request);

            if (except && (counter == 0))
                {
                    int val = 10;
                    val = val/counter;
                }
        } catch(InterruptedException ie) {
            ie.printStackTrace();
        } catch(BadOrderException boe) {
            boe.printStackTrace();
        }
        return request;
    }
}

