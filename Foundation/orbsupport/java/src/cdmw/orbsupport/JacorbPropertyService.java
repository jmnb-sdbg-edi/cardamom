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


package cdmw.orbsupport;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

 public class JacorbPropertyService {

    private static byte[] synchronizer = new byte[0];
	public static ThreadedProcess proc;
    
    public static void main(String [] args)
    {
       try{
	    String port = OS.getOptionValue(args, "--PropPort");
	    if (port == "no")
	       proc = OS.createProcess("cdmw_property_server_standalone");
        else
	       proc = OS.createProcess("cdmw_property_server_standalone --PropPort=" + port);
    		
       Runnable shutDownLogic = new Runnable() {
    		ThreadedProcess cppProc = proc;

       		public void run() {
       			cppProc.killProcess();
       		}
       	};

       	Runtime runtime = Runtime.getRuntime();
       	runtime.addShutdownHook(new Thread(shutDownLogic));

    	synchronized (synchronizer) {
    		while (true) {
   				try {
   					synchronizer.wait();
   				}
   				catch (InterruptedException ie) {}
    		}
    	}
      
       }catch(java.io.IOException ex)
       {
	    ex.printStackTrace();
       }
    }

}

