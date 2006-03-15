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


package cdmw.cdmwinit;

import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;
import cdmw.platformmngt.PlatformInterface;

/**
 * The Default Process Control class
 *
 */
public class DefaultProcessControl extends ProcessControl {

    /**
     * The constructor
     *
     */ 
    public DefaultProcessControl() {
        
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    public void onInitialise(com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {

        try {
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                PlatformInterface.getProcessName(), 
                "Received 'initialise' message");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}
        
    }
    
    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
     * operation
     */
    public void onNextStep() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.InvalidStep {
        
        try {
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                PlatformInterface.getProcessName(), 
                "Received 'next_step' message");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}
        
    }
    
    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    public void onRun()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun {
        
        try {
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                PlatformInterface.getProcessName(), 
                "Received 'run' message");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}
        
    }
    
    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    public void onStop() {
    
        try {
            PlatformInterface.notifyMessage(
                com.thalesgroup.CdmwPlatformMngtBase.INF.value,
                PlatformInterface.getProcessName(), 
                "Received 'stop' message");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}
    
    }

}

