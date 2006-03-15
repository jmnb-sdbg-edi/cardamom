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


package cdmw.resources.test;

import cdmw.ossupport.OS;

public class CommandThread extends Thread {

    private org.omg.CORBA.ORB orb;
    private String processURL;
    private int waitDuration;
    private int processingDuration;
    
    public void run() {        
        
        int timescale = cdmw.testutils.Testable.getTimescale();
        try {
            // Wait for waitDuration before calling initialise
            OS.sleep(timescale * waitDuration);
            
            com.thalesgroup.CdmwPlatformMngt.ProcessDelegate proc = null;
            org.omg.CORBA.Object obj = orb.string_to_object(processURL);
            
            if (obj!=null) {
                proc = com.thalesgroup.CdmwPlatformMngt.ProcessDelegateHelper.narrow(obj);
                cdmw.tools.ProcessAdmin admin = new cdmw.tools.ProcessAdmin(orb, proc);
                
                // send initialise order
                String commands= "verbose \n init LAST 2\n exit \n";
                admin.run(new java.io.BufferedReader(new java.io.StringReader(commands)), System.out);
                
                // send run order
                commands = "verbose \n run\n exit \n";
                admin.run(new java.io.BufferedReader(new java.io.StringReader(commands)), System.out);
                
                // Wait for m_processing_duration before calling initialise
                OS.sleep(timescale * processingDuration);
                
                // send stop order
                commands = "verbose \n stop\n exit \n";
                admin.run(new java.io.BufferedReader(new java.io.StringReader(commands)), System.out);
            }
            
        } catch (Exception e) {
            System.out.println("Exception raised within CommandThread !");
        }        
        
    }
    
    public CommandThread(org.omg.CORBA.ORB orb, 
        String procURL, int waitDuration, int processingDuration) {
        this.orb = orb;
        this.processURL = procURL;
        this.waitDuration = waitDuration;
        this.processingDuration = processingDuration;
    }
    
}

