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


class TargetThread2 extends Thread {
    
    private static short LEVEL1 = 1;

    protected int READY   = 1;
    protected int RUNNING = 2;
    protected int STOPPED = 3;
    
    private int status;
    
    
    
    public TargetThread2() {
        super("TargetThread2");  
        status = READY; 
    }
    
    public void run() {
        if (status == READY) {

            System.out.println("\ntarget thread is started");            
            cdmw.trace.Tracer.trace("test", LEVEL1, "start target 2");

            status = RUNNING;
            int loopInx = 0;
            
            while (status == RUNNING) {
                if (loopInx > 255) {
                    loopInx = 0;   
                }
                
                loopInx++;
                cdmw.trace.Tracer.trace("test", LEVEL1, 
                                         "target 2 loop cycle " + loopInx);
                                         
                try {
                    cdmw.ossupport.OS.sleep(
                        cdmw.testutils.Testable.getTimescale() * 30);
                } catch (InterruptedException e) {
                    // ignore
                }
                
            }            
        }

        cdmw.trace.Tracer.trace("test", LEVEL1, "end target 2");
        System.out.println("target thread is stopped");            
    }
    
    public void stopThread() {
        if (status == RUNNING) {
            status = STOPPED;   
        }
        
        System.out.println("request target thread to stop");
    }
    
}



