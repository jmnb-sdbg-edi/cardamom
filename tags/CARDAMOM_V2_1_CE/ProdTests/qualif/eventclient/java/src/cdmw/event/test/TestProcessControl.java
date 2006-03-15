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

package cdmw.event.test;

import cdmw.common.Locations;
import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;

import cdmw.cdmwinit.ProcessControl;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;
import cdmw.platformmngt.PlatformInterface;

import com.thalesgroup.CdmwPlatformMngtBase.INF;

public class TestProcessControl extends ProcessControl {

    public final static String TEST_PROC_1 = "TestProcess1";
    public final static String TEST_PROC_2 = "TestProcess2";
    public final static String TEST_PROC_3 = "TestProcess3";
    public final static String TEST_PROC_4 = "TestProcess4";

    private org.omg.CORBA.ORB orb;
    
    private org.omg.CosNaming.NamingContext nc;
    
    private boolean typedEvtTest;
    
    private int testProcNb;

    public TestProcessControl(org.omg.CORBA.ORB orb, boolean typedEvtTest) 
        throws org.omg.CORBA.SystemException {
        
        this.orb = orb;
        this.typedEvtTest = typedEvtTest;
        this.testProcNb = 0;
    }
               
            
    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
     * operation
     */
    public void onInitialise(
        com.thalesgroup.CdmwPlatformMngtBase.StartupKind startupKind)
        throws org.omg.CORBA.SystemException {

        try {
            PlatformInterface.notifyMessage( INF.value,
                                             PlatformInterface.getProcessName(),
                                             "Initialisation requested");
            String testProcName = PlatformInterface.getProcessName();
        
            if (testProcName.equals(TEST_PROC_1)) {
                testProcNb = 1;
            } else if (testProcName.equals(TEST_PROC_2)) {
                testProcNb = 2;
            } else if (testProcName.equals(TEST_PROC_3)) {
                testProcNb = 3;
            } else if (testProcName.equals(TEST_PROC_4)) {
                testProcNb = 4;
            }

            //
            // Get NamingInterface object from RepositoryInterface
            //
            NamingInterface ni = RepositoryInterface.getDomainNamingInterface(
                Locations.CDMW_SERVICES_NAME_DOMAIN);

            //
            // Get a reference on the EventChannel1 and EventChannel2
            //
            org.omg.CORBA.Object obj = ni.resolve("EVENT");
            nc = org.omg.CosNaming.NamingContextHelper.narrow(obj);

            if (nc == null) {
                System.out.println("NamingContext is a nil object reference");
                PlatformInterface.notifyFatalError(testProcName,
                    "NamingContext is a nil object reference!");
                return;
            }
        
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            nnd.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(nnd.getMessage());
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            in.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(in.getMessage());
        } catch(org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            nf.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(nf.getMessage());
        } catch(org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            cp.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(cp.getMessage());
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            in.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(in.getMessage());
        } catch(BadOrderException boe) {
            boe.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(boe.getMessage());
        } catch(BadParameterException bpe) {
            bpe.printStackTrace();
            throw new org.omg.CORBA.BAD_PARAM(bpe.getMessage());
        }
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/next_step:1.0
     * operation
     */
    public void onNextStep() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.InvalidStep {
        try {
            PlatformInterface.notifyMessage(INF.value,
                PlatformInterface.getProcessName(),
                "nextStep requested");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
     * operation
     */
    public void onRun() 
        throws com.thalesgroup.CdmwPlatformMngt.ProcessDelegatePackage.NotReadyToRun, 
        org.omg.CORBA.SystemException {

        try {
            PlatformInterface.notifyMessage(INF.value,
                PlatformInterface.getProcessName(),
                "Run requested");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}

        // Testing TestCdmwEventService
        if (typedEvtTest) {
            CdmwTypedEventServiceTest testCdmwTypedEvent 
                = new CdmwTypedEventServiceTest(orb, nc, testProcNb);
            testCdmwTypedEvent.start();
        } else {
            CdmwEventServiceTest testCdmwEvent 
                = new CdmwEventServiceTest(orb, nc, testProcNb);
            testCdmwEvent.start();
        }
    }

    /**
     * the behaviour for the
     * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
     * operation
     */
    public void onStop() throws org.omg.CORBA.SystemException {   
        try {
            PlatformInterface.notifyMessage( INF.value,
                PlatformInterface.getProcessName(),
                "Stop requested");
        } catch(BadOrderException boe) {
        } catch(BadParameterException bpe) {}
    }

}

