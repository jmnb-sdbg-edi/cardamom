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

package cdmw.lifecycle;

import cdmw.cdmwinit.ProcessControl;
import cdmw.cdmwinit.CdmwInterface;
import cdmw.namingandrepository.RepositoryInterface;
import com.thalesgroup.CdmwNamingAndRepository.Repository;

public class MyProcessControl extends ProcessControl {

    /**
     * The ORB.
     */
    private org.omg.CORBA.ORB orb;
    
    /**
     * Commands for the client
     */
    private java.io.BufferedReader reader;
    
    /**
     * Stream to print results
     */
    private java.io.PrintStream out;
    
    /**
     * Client
     */
    private ClientThread client;
    
    /**
     * The constructor.
     *
     */ 
    public MyProcessControl(org.omg.CORBA.ORB orb, 
        java.io.BufferedReader reader, java.io.PrintStream out) {
        this.orb = orb;
        this.reader = reader;
        this.out = out;
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
     * operation
     */
    public void onInitialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind) {
        
        Repository repository = RepositoryInterface.getRepository();
        client = new ClientThread(orb, repository, reader, out);
        
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/next_step:1.0
     * operation
     */
    public void onNextStep()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.InvalidStep {

    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
     * operation
     */
    public void onRun()
        throws com.thalesgroup.CdmwPlatformMngt.ProcessPackage.NotReadyToRun {
        System.out.println("Run requested: starting tests...");
        client.start();
    }

    /**
     * The behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
     * operation
     */
    public void onStop() {
        System.out.println("Stopping...");
        try {
            client.join();
        } catch (InterruptedException ie) {}
    }

}

