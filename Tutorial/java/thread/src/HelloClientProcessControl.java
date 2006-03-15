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


package cdmw.tutorial;


class HelloClientProcessControl extends cdmw.cdmwinit.ProcessControl 
{
    private org.omg.CORBA.ORB orb;
    
    private String applicationName;
    private String processName;
    private HelloClientThread helloClientThread;
    
    
    public HelloClientProcessControl(org.omg.CORBA.ORB orb) {
        this.orb = orb;
        this.helloClientThread = null;
        this.applicationName = "";
        this.processName = "";
    }
    
    
    protected void finalize() {
        // ==================================================
        // if client thread is defined call onStop
        //     to stop it if still running
        // ==================================================
        if (helloClientThread != null) 
        {
            this.onStop();
        }    
    }
    
    
    
    // process initialisation called by platformmngt 
    public void onInitialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind)
    {
        try {
            // ==============================================================
            // Get the process name and application name
            // ==============================================================                 
            processName = cdmw.platformmngt.PlatformInterface.getProcessName();
            applicationName = cdmw.platformmngt.PlatformInterface.getApplicationName();
                        
            // ==============================================================
            // example of using the PlatformInterface for notifying a message
            // ==============================================================
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>> Initialisation requested by supervision");

            // ========================================================
            // Get the naming and repository from Repository interface
            // ========================================================
            com.thalesgroup.CdmwNamingAndRepository.Repository repository = null;
                                          
            repository = cdmw.namingandrepository.RepositoryInterface.getRepository();
                
                
            // ==================================================                                
            // create the hello client thread
            // ==================================================
            helloClientThread = new HelloClientThread (orb, 
                                                       repository,
                                                       applicationName,
                                                       processName);
                

        } catch (cdmw.common.BadParameterException e) {
            // This exception shouldn't append if notify level is not CdmwPlatformMngtBase.FTL,
            // if notify issuer is not null or CdmwPlatformMngtBase.PFMMNG, 
            // and if notify message is not null.
            e.printStackTrace();
            cdmw.common.Assert.check(false);
            
        } catch (cdmw.common.BadOrderException e) {
            // This exception shouldn't append if creation  
            // acknowledgement has been performed
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }
                    
    }
      
      
    // process to run called by platformmngt    
    public void onRun() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun
    {
        try {
            // ==============================================================        
            // example of using the PlatformInterface for notifying a message
            // ==============================================================
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>> Run requested by supervision");                
    
            // ======================================                                
            // start the client thread
            // ======================================
            if (helloClientThread != null) {
                helloClientThread.start();
            }
     
        } catch (cdmw.common.BadParameterException e) {
            // This exception shouldn't append if notify level is not CdmwPlatformMngtBase.FTL,
            // if notify issuer is not null or CdmwPlatformMngtBase.PFMMNG, 
            // and if notify message is not null.
            e.printStackTrace();
            cdmw.common.Assert.check(false);
            
        } catch (cdmw.common.BadOrderException e) {
            // This exception shouldn't append if creation  
            // acknowledgement has been performed
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        }
    }
    
    
    // process to stop called by platformmngt 
    public void onStop()
    {
        try {
            // ==============================================================         
            // example of using the PlatformInterface for notifying a message
            // ==============================================================
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>> Stop requested by supervision");
    
            if (helloClientThread != null) {
                // First signal termination to hello client thread if it was 
                // at least requested to run
                
                if (helloClientThread.isAlive()) {
                    helloClientThread.stopThread();
                    helloClientThread.join();
                }
                
                helloClientThread = null;;
            }            
            
        } catch (cdmw.common.BadParameterException e) {
            // This exception shouldn't append if notify level is not CdmwPlatformMngtBase.FTL,
            // if notify issuer is not null or CdmwPlatformMngtBase.PFMMNG, 
            // and if notify message is not null.
            e.printStackTrace();
            cdmw.common.Assert.check(false);
            
        } catch (cdmw.common.BadOrderException e) {
            // This exception shouldn't append if creation  
            // acknowledgement has been performed
            e.printStackTrace();
            cdmw.common.Assert.check(false);
        
        } catch (java.lang.InterruptedException e) {
            System.out.println("Unexpected Exception:");            
            e.printStackTrace();
        }
        
    }    
      
}

