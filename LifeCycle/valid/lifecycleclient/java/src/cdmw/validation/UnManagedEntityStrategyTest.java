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

import cdmw.common.Assert;
import cdmw.testutils.Testable;

import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import com.thalesgroup.CF.Application;
import com.thalesgroup.CF.Device;
import com.thalesgroup.CF.DevicePackage.InvalidLocation;
import com.thalesgroup.CF.ResourcePackage.StartError;
import com.thalesgroup.CF.ResourcePackage.StopError;
import com.thalesgroup.CF.CreateApplicationError;

import com.thalesgroup.CFFactories.ResourceFactory;
import com.thalesgroup.CFFactories.ResourceFactoryHelper;


public class UnManagedEntityStrategyTest extends Testable {

    private String args;

    public UnManagedEntityStrategyTest(String args, java.io.PrintStream out) {
        this.name = "UnManagedEntityStrategyTest";
        this.out = out;
        this.args = args;
    }

    public void doTests() {

        setNbOfRequestedTestOK(8);
        
        //-------------------------------------------------
        // This tests UnManagedEntity strategy
        //-------------------------------------------------
        println("Testing UnManagedEntity strategy");

        try {
            String factory_domaine_name
                = TestData.get_FACTORIES_DOMAIN_NAME();
            String factory_name
                = TestData.get_UNMANAGED_ENTITY_FACTORY_NAME();
            String dev_key
                = TestData.get_UNMANAGED_ENTITY_DEVICE_KEY();
            String dev_label
                = TestData.get_UNMANAGED_ENTITY_DEVICE_LABEL();
            String appl_1_key
                = TestData.get_UNMANAGED_ENTITY_APPLICATION_I_KEY();
            String appl_1_name
                = TestData.get_PROGRAM_EXECUTABLE_1();
            String appl_2_key
                = TestData.get_UNMANAGED_ENTITY_APPLICATION_II_KEY();
            String appl_2_name
                = TestData.get_PROGRAM_EXECUTABLE_2();


            // NOTA: - Device.stop() deactivates Device Object, while
            //         Device.start() or any other operation will reactivate it.
            //       - Application.start() will launch a process, while
            //         Application.stop() will kill the process and deactivate
            //         the Application object.

            NamingInterface ni
                = RepositoryInterface.
                    getDomainNamingInterface(factory_domaine_name);

            println( "Testing " + factory_name + " factory.");

            org.omg.CORBA.Object obj = ni.resolve(factory_name);
            ResourceFactory fac = ResourceFactoryHelper.narrow(obj);

            Assert.check(fac!=null);

            Device dev = null;
            try {
                println("Creating Device <" + dev_key + "," + dev_label + ">");
                dev = fac.create_device(dev_key,dev_label);
                succeed();
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                fail(); return;
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                fail(); return;
            } catch (InvalidLocation il) {
                fail(); return;
            } catch (org.omg.CORBA.SystemException se) {
                fail(); return;
            }

            try {
                println("Starting it");
                dev.start();
                succeed();
            } catch (StartError se) {
                fail(); return;
            } catch (org.omg.CORBA.SystemException se) {
                fail(); return;
            }

            Application appli = null;
            try {
                println("Creating Application I <" + appl_1_key + "," + appl_1_name + ">");
                appli = fac.create_application (appl_1_key, appl_1_name);
                succeed();
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                fail(); return;
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                fail(); return;
            } catch (CreateApplicationError cae) {
                fail(); return;
            } catch (org.omg.CORBA.SystemException se) {
                fail(); return;
            }

            try {
                println("Starting it");
                appli.start();
                succeed();
            } catch (StartError se) {
                fail(); return;
            } catch (org.omg.CORBA.SystemException sex) {
                fail(); return;
            }

            Application appli2 = null;
            try {
                println("Creating Application I <" + appl_2_key + ","+ appl_2_name + ">");
                appli2 = fac.create_application (appl_2_key, appl_2_name);
                succeed();
            } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
                fail(); return;
            } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                fail(); return;
            } catch (CreateApplicationError cae) {
                fail(); return;
            } catch (org.omg.CORBA.SystemException se) {
                fail(); return;
            }
            try {
                println("Starting it");
                appli2.start();
                succeed();
            } catch (StartError se) {
                fail(); return;
            } catch (org.omg.CORBA.SystemException sex) {
                fail(); return;
            }

            // Cdmw::OsSupport::OS::sleep(4000);

            try {
                println("Stopping Application I");
                appli.stop();
                succeed();
            } catch (StopError se) {
                fail(); return;
            } catch (org.omg.CORBA.SystemException sex) {
                fail(); return;
            }

            try {
                println("Deleting Application II");
                appli2.remove();
                succeed();
            } catch (org.omg.CosLifeCycle.NotRemovable nr){
                fail(); return;
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

