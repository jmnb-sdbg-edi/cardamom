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


class HelloProcessBehaviour extends cdmw.platformmngt.ProcessBehaviour 
{
    private org.omg.CORBA.ORB orb;
    
    private String applicationName;
    private String processName;
    
    
    
    public HelloProcessBehaviour(org.omg.CORBA.ORB orb) {
        this.orb = orb;
        applicationName = null;
        processName = null;
    }
    
    
    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
    * attribute
    */
    public int nbSteps() {
        return 1;
    }

    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
    * operation
    */
    public org.omg.CORBA.Object getService() {
        return null;
    }

    
    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
    * operation
    */
    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind)
    {
        try {
            // ==============================================================
            // get application and process names   
            // ==============================================================                 
            applicationName = cdmw.platformmngt.PlatformInterface.getApplicationName();
            processName = cdmw.platformmngt.PlatformInterface.getProcessName();
                        
            // ==============================================================
            // example of using the PlatformInterface for notifying a message
            // ==============================================================
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>>>>>>>>>>>>> Initialisation requested by supervision");

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


    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
	* operation
	*/
    public void run() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun
    {    
        try {
            // ==============================================================        
            // example of using the PlatformInterface for notifying a message
            // ==============================================================
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>>>>>>>>>>>>> Run requested by supervision");                

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

    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
	* operation
	*/
    public void stop()
    {    
        try {
            // ==============================================================         
            // example of using the PlatformInterface for notifying a message
            // ==============================================================
            cdmw.platformmngt.PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                processName, 
                ">>>>>>>>>>>>>> Stop requested by supervision");
    
            // ==============================================================
            // Shut down the orb
            // ==============================================================
            orb.shutdown(false);
            
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
    
}

