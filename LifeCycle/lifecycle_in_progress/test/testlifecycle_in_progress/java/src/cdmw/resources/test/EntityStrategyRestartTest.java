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


package cdmw.resources.test;

import cdmw.testutils.Testable;
import cdmw.common.Assert;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import com.thalesgroup.CF.Application;
import com.thalesgroup.CF.ApplicationHelper;
import com.thalesgroup.CdmwLifeCycle.AlreadyExistant;

import com.thalesgroup.CFFactories.ResourceFactory;
import com.thalesgroup.CFFactories.ResourceFactoryHelper;
import com.thalesgroup.CF.ResourcePackage.StartError;
import com.thalesgroup.CF.ResourcePackage.StopError;
import com.thalesgroup.CF.CreateApplicationError;

import com.thalesgroup.CdmwLifeCycle.AlreadyExistant;
import com.thalesgroup.CdmwNamingAndRepository.NoNameDomain;

import org.omg.CORBA.SystemException;
import org.omg.CosNaming.NamingContextPackage.NotFound;
import org.omg.CosNaming.NamingContextPackage.CannotProceed;
import org.omg.CosLifeCycle.NotRemovable;


public class EntityStrategyRestartTest extends Testable {

    private String args;

    public EntityStrategyRestartTest(String args, java.io.PrintStream out) {
        this.name = "EntityStrategyRestartTest";
        this.out = out;
        this.args = args;
    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(4);
        
        //-------------------------------------------------
        // This tests Entity strategy
        //-------------------------------------------------
        println("Testing Entity strategy");

        try {
            String factoryDomainName = TestData.get_FACTORIES_DOMAIN_NAME();
            String factoryName = TestData.get_ENTITY_FACTORY_NAME();
            String appl1Key = TestData.get_ENTITY_APPLICATION_I_KEY();
            String appl1Name = TestData.get_PROGRAM_EXECUTABLE_1();

            String resourceDomainName = TestData.get_RESOURCES_DOMAIN_NAME();


            // NOTA: - Application.start() will launch a process, while
            //         Application.stop() will kill the process and deactivate
            //         the Application object.


            println( "Testing " + factoryName + " factory.");


            ResourceFactory fac = null;

            NamingInterface ni
                = RepositoryInterface.getDomainNamingInterface
                    (factoryDomainName);

            org.omg.CORBA.Object obj = ni.resolve(factoryName);
            fac = ResourceFactoryHelper.narrow(obj);

            Assert.check(fac!=null);

            Application appli = null;
            
            if (args==null) {
                println("ERROR: no parameter.");
                fail();
                return;
            }

            if ( ( args.equals("create") )||
                ( args.equals("all") ) ) {
                try {
                    println("Creating Application I <"
                              + appl1Key + "," + appl1Name + ">");
                    appli = fac.create_application(appl1Key,appl1Name);
                    succeed();
                } catch (AlreadyExistant ae) {
                    ae.printStackTrace();
                    fail(); return;
                } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
                    in.printStackTrace();
                    fail(); return;
                } catch (CreateApplicationError cae) {
                    cae.printStackTrace();
                    fail(); return;
                } catch (SystemException se) {
                    se.printStackTrace();
                    fail(); return;
                }
            } else {
                ni = RepositoryInterface.getDomainNamingInterface
                        (resourceDomainName);
                obj = ni.resolve(appl1Key);
                appli = ApplicationHelper.narrow(obj);

            }

            if ( ( args.equals("start") )||
                ( args.equals("all") ) ) {
                try {
                    println("Starting it");
                    appli.start();
                    succeed();
                } catch (StartError se) {
                    fail(); return;
                } catch (SystemException ex) {
                    fail(); return;
                }
            }

            if ( ( args.equals("stop") )||
                ( args.equals("all") ) ) {
                try {
                    println("Stopping Application I");
                    appli.stop();
                    succeed();
                } catch (StopError se) {
                    fail(); return;
                } catch (SystemException ex) {
                    fail(); return;
                }
            }

            if ( ( args.equals("remove") )||
                ( args.equals("all") ) ) {
                try {
                    println("Deleting Application I");
                    appli.remove();
                    succeed();
                } catch (NotRemovable nr){
                    fail(); return;
                } catch (SystemException ex) {
                    fail(); return;
                }
            }

        } catch (NotFound nf) {
            println("ERROR:" + nf.getMessage());
            fail();
        } catch (CannotProceed cp) {
            println("ERROR:" + cp.getMessage());
            fail();
        } catch (NoNameDomain nnd) {
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

