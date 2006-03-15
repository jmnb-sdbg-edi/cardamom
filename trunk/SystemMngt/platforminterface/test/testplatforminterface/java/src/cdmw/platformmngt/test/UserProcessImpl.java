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


package cdmw.platformmngt.test;

import cdmw.platformmngt.ProcessDelegateImpl;
import cdmw.platformmngt.ProcessBehaviour;

/**
 * User specific Process implementation.
 *
 */
public class UserProcessImpl extends ProcessDelegateImpl {

    public UserProcessImpl(org.omg.CORBA.ORB orb, ProcessBehaviour behaviour) {
        super(orb, behaviour);
    }
    
    public UserProcessImpl(org.omg.CORBA.ORB orb) {
        super(orb, new DefaultBehaviour());
    }
    
    public int nb_steps() {
        return 1;
    }

    public org.omg.CORBA.Object get_service() {
        return null;
    }

    public void initialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
    }
    
    public void next_step() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.InvalidStep {
        throw new com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.InvalidStep();
    }

    public void run() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun {
        // do nothing
    }

    public void stop() {
        // do nothing
    }
    
    static class DefaultBehaviour extends ProcessBehaviour {

        public DefaultBehaviour() {
            // do nothing
        }

        public void initialise(
            com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
            System.out.println("initialise requested");
        }

        public void run() 
            throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun {
            System.out.println("run requested");
        }

        public void stop() {
            System.out.println("stop requested");
        }

    }

}

