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

package cdmw.lifecycle;

import cdmw.testutils.Testable;
import cdmw.common.Assert;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import com.thalesgroup.CF.Application;
import com.thalesgroup.CF.Device;
import com.thalesgroup.CF.DevicePackage.UsageType;
import com.thalesgroup.CF.DevicePackage.InvalidLocation;
import com.thalesgroup.CF.ResourcePackage.StartError;
import com.thalesgroup.CF.ResourcePackage.StopError;
import com.thalesgroup.CF.CreateApplicationError;

import com.thalesgroup.CFFactories.AnonymousResourceFactory;
import com.thalesgroup.CFFactories.AnonymousResourceFactoryHelper;


public class ServiceStrategyTest extends Testable {

    private String args;

    public ServiceStrategyTest(String args, java.io.PrintStream out) {
        this.name = "ServiceStrategyTest";
        this.out = out;
        this.args = args;
    }

    public void doTests() {
            
        setNbOfRequestedTestOK(6);
        
        try {
        //-------------------------------------------------
        // This tests Session strategy
        //-------------------------------------------------

        String factoryDomainName
            = TestData.get_FACTORIES_DOMAIN_NAME();
        String factoryName
            = TestData.get_SERVICE_FACTORY_NAME();
        String devLabel
            = TestData.get_SERVICE_DEVICE_LABEL();
        String appl1Name
            = TestData.get_PROGRAM_EXECUTABLE_1();
        String appl2Name
            = TestData.get_PROGRAM_EXECUTABLE_2();


        // NOTA: - Device.stop() deactivates Device Object, while
        //         Device.start() or any other operation will reactivate it.
        //       - Application.start() will launch a process, while
        //         Application.stop() will kill the process and deactivate
        //         the Application object.

        NamingInterface ni =
            RepositoryInterface.getDomainNamingInterface(factoryDomainName);

        println( "Testing " + factoryName + " factory.");

        org.omg.CORBA.Object obj = ni.resolve(factoryName);
        AnonymousResourceFactory fac = AnonymousResourceFactoryHelper.narrow(obj);

        Assert.check(fac!=null);

        Device dev = null;
        try {
            println("Creating Device <" + devLabel + ">");
            dev = fac.create_device(devLabel);
            succeed();
        } catch (InvalidLocation il) {
            il.printStackTrace();
            fail(); return;
        } catch (org.omg.CORBA.SystemException se) {
            se.printStackTrace();
            fail(); return;
        }

        try {
            println("Getting state and label of <" + devLabel + ">");
            UsageType state = dev.state();
            String state_s = "IDLE";
            if ( state.equals(UsageType.ACTIVE) ) {
                state_s = "ACTIVE";
            } else if ( state.equals(UsageType.BUSY) ) {
                state_s = "BUSY";
            }

            String location = dev.label();
            println("Device <" + location + "> in state " + state_s);
            succeed();
        } catch (org.omg.CORBA.SystemException se) {
            fail(); return;
        }

        Application appli = null;
        try {
            println("Creating Application I <" + appl1Name + ">");
            appli = fac.create_application(appl1Name);
            succeed();
        } catch (CreateApplicationError cae) {
            fail(); return;
        } catch (org.omg.CORBA.SystemException se) {
            fail(); return;
        }

        try {
            println("Getting name of <" + appl1Name + ">");
            String name = appli.name();
            println("Application I <" + name + ">");
            succeed();
        } catch (org.omg.CORBA.SystemException se) {
            fail(); return;
        }

        Application appli2 = null;
        try {
            println("Creating Application II <" + appl2Name + ">");
            appli2 = fac.create_application(appl2Name);
            succeed();
        } catch (CreateApplicationError cae) {
            fail(); return;
        } catch (org.omg.CORBA.SystemException se) {
            fail(); return;
        }

        try {
            println("Getting name of <" + appl2Name + ">");
            String name = appli2.name();
            println("Application I <" + name + ">");
            succeed();
        } catch (org.omg.CORBA.SystemException se) {
            fail(); return;
        }


        } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            println("ERROR:" + nf.getMessage());
            fail();
        } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            println("ERROR:" + cp.getMessage());
            fail();
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            println("ERROR:" + nnd.getMessage());
            fail();
        } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            println("ERROR:" + in.getMessage());
            fail();
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in2) {
            println("ERROR:" + in2.getMessage());
            fail();
        }

    }

}

