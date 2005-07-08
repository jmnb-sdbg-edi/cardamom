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


package cdmw.trace.test;


class TargetThread1 extends Thread {

    private static short LEVEL1 = 1;
    private static short LEVEL2 = 2;
    private static short LEVEL3 = 3;


    public TargetThread1() {
        super("TargetThread1");   
    }
    
    public void run() {        
        cdmw.trace.Tracer.trace("test", LEVEL1, "start target thread1");
    
        for (int i = 0; i < 1; i++) {
            String message = "target loop cycle " +i;
            
            cdmw.trace.Tracer.trace("domain1", LEVEL1, message);
            cdmw.trace.Tracer.trace("domain1", LEVEL2, message);
            cdmw.trace.Tracer.trace("domain1", LEVEL3, message);
            cdmw.trace.Tracer.trace("domain2", LEVEL1, message);
            cdmw.trace.Tracer.trace("domain2", LEVEL2, message);
            cdmw.trace.Tracer.trace("domain2", LEVEL3, message);
            cdmw.trace.Tracer.trace("domain3", LEVEL1, message);
            cdmw.trace.Tracer.trace("domain3", LEVEL2, message);
            cdmw.trace.Tracer.trace("domain3", LEVEL3, message);
            
            try {
                cdmw.ossupport.OS.sleep(
                    cdmw.testutils.Testable.getTimescale() * 30);
            } catch (InterruptedException e) {
                // ignore
            }
             
        }

        cdmw.trace.Tracer.trace("test", LEVEL1, "end target thread1");        
    }    
    
}




