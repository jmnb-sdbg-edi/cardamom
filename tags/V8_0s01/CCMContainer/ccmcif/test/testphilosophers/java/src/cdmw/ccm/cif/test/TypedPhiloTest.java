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


package cdmw.ccm.cif.test;


import cdmw.testutils.Testable;
import cdmw.ossupport.OS;
import cdmw.ccm.deployment.ConfigValueImpl;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ComponentKindValueHelper;
import com.thalesgroup.CdmwDeployment.ServantLifetimeValue;
import com.thalesgroup.CdmwDeployment.ServantLifetimeValueHelper;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValue;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValueHelper;
import com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescription;
import com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescriptionHelper;
import com.thalesgroup.CdmwDeployment.ValuetypeFactoryDescriptionSeqHelper;

public class TypedPhiloTest extends Testable {

    public final static String PHILOSOPHER_HOME_REP_ID =
        "IDL:acme.com/Cdmw_Dinner/PhilosopherHome:1.0";
    public final static String PHILOSOPHER_HOME_SERVANT_NAME =
        "Cdmw.CCM.CIF.CdmwDinner.SessionPhilosopherHome_impl";
    public final static String PHILOSOPHER_HOME_UID = "PhilosopherHome_1";
    public final static String PHILOSOPHER_HOME_ENTRY_POINT =
        "cdmw.ccm.cif.test.CCMPhilosopherHomeImpl.createCCMPhilosopherHome";

    public final static String OBSERVER_HOME_REP_ID =
        "IDL:acme.com/Cdmw_Dinner/ObserverHome:1.0";
    public final static String OBSERVER_HOME_SERVANT_NAME =
        "Cdmw.CCM.CIF.CdmwDinner.SessionObserverHome_impl";
    public final static String OBSERVER_HOME_UID = "ObserverHome_1";
    public final static String OBSERVER_HOME_ENTRY_POINT =
        "cdmw.ccm.cif.test.CCMObserverHomeImpl.createCCMObserverHome";

    public final static String FORKMANAGER_HOME_REP_ID =
        "IDL:acme.com/Cdmw_Dinner/ForkHome:1.0";
    public final static String FORKMANAGER_HOME_SERVANT_NAME =
        "Cdmw.CCM.CIF.CdmwDinner.SessionForkHome_impl";
    public final static String FORKMANAGER_HOME_UID = "ForkHome_1";
    public final static String FORKMANAGER_HOME_ENTRY_POINT =
        "cdmw.ccm.cif.test.CCMForkHomeImpl.createCCMForkHome";

    public final static String REGISTRATOR_HOME_REP_ID =
        "IDL:acme.com/Cdmw_Dinner/RegistratorHome:1.0";
    public final static String REGISTRATOR_HOME_SERVANT_NAME =
        "Cdmw.CCM.CIF.CdmwDinner.SessionRegistratorHome_impl";
    public final static String REGISTRATOR_HOME_UID = "RegistratorHome_1";
    public final static String REGISTRATOR_HOME_ENTRY_POINT =
        "cdmw.ccm.cif.test.CCMRegistratorHomeImpl.createCCMRegistratorHome";

    public final static String FORK_FACET_NAME = "the_fork";

    public final static String FORK_RIGHT_RECEP_NAME = "right";
    public final static String FORK_LEFT_RECEP_NAME = "left";
    public final static String REGISTER_RECEP_NAME = "register";

    public final static String INFO_CONSUMER_NAME = "info";
    public final static String INFO_PUBLISHER_NAME = "info";
    public final static String CONFIG_EMITTER_NAME = "config";

    public final static String EVENT_STATUSINFO_REP_ID =
        "IDL:acme.com/Dinner/StatusInfo:1.0";
    public final static String EVENT_STATUSINFO_ENTRYPOINT =
        "cdmw.ccm.cif.test.StatusInfoFactoryImpl.createStatusInfoFactory";

    private org.omg.CORBA.ORB orb;

    private org.omg.Components.Deployment.Container container;

    public TypedPhiloTest(org.omg.CORBA.ORB orb, String containerIOR) {
        this.name = "cdmw.ccm.cif.TypedPhiloTest";
        this.out = System.out;
        this.orb = orb;
        try {
            org.omg.CORBA.Object obj = this.orb.string_to_object(containerIOR);

            if (obj == null) {
                println("TEST FAILED : exception raised during test initialisation!");
            } else {
                this.container =
                    org.omg.Components.Deployment.ContainerHelper.narrow(obj);
            }
        } catch (org.omg.CORBA.SystemException se) {
            println("TEST FAILED : system exception raised during test initialisation!");
            throw se;
        } catch (Exception e) {
            println("TEST FAILED : unexpected exception raised during test initialisation!");
        }
    }

    public void doTests() {
    
        // set number of requested successfull tests
        setNbOfRequestedTestOK(44);

        boolean testOk = false;
        org.omg.Components.CCMHome philosopherHome = null;
        com.acme.Dinner.PhilosopherHome dinnerPhilosopherHome = null;
        org.omg.Components.CCMObject ccmPhilosopherComp = null;
        com.acme.Dinner.Philosopher dinnerPhilosopherComp1 = null;
        com.acme.Dinner.Philosopher dinnerPhilosopherComp2 = null;
        com.acme.Dinner.Philosopher dinnerPhilosopherComp3 = null;
        com.acme.Dinner.Philosopher dinnerPhilosopherComp4 = null;

        org.omg.Components.CCMHome observerHome = null;
        com.acme.Dinner.ObserverHome dinnerObserverHome = null;
        org.omg.Components.CCMObject ccmObserverComp = null;
        com.acme.Dinner.Observer dinnerObserverComp = null;
        com.acme.Dinner.StatusInfoConsumer infoConsumer = null;

        org.omg.Components.CCMHome forkHome = null;
        com.acme.Dinner.ForkHome dinnerForkHome = null;
        org.omg.Components.CCMObject ccmForkComp = null;
        com.acme.Dinner.ForkManager dinnerForkComp1 = null;
        com.acme.Dinner.ForkManager dinnerForkComp2 = null;
        com.acme.Dinner.ForkManager dinnerForkComp3 = null;
        com.acme.Dinner.ForkManager dinnerForkComp4 = null;
        com.acme.Dinner.Fork forkFacetObj1 = null;
        com.acme.Dinner.Fork forkFacetObj2 = null;
        com.acme.Dinner.Fork forkFacetObj3 = null;
        com.acme.Dinner.Fork forkFacetObj4 = null;

        org.omg.Components.CCMHome registratorHome = null;
        com.acme.Dinner.RegistratorHome dinnerRegistratorHome = null;
        com.acme.Dinner.Registrator dinnerRegistratorComp = null;

        org.omg.Components.Cookie ck1 = null;
        org.omg.Components.Cookie ck2 = null;
        org.omg.Components.Cookie ck2_bis = null;
        org.omg.Components.Cookie ck3 = null;
        org.omg.Components.Cookie ck4 = null;

        int timescale = getTimescale();

        // This tests CIF classes
        //-------------------------------------------------
        println("Testing CIF classes");

        try {
            println("Install a PhilosopherHome");
            testOk = false;
            org.omg.CORBA.Any value = null;
            org.omg.Components.ConfigValue[] configValues;

            try {
                configValues = new org.omg.Components.ConfigValue[5];
                value = this.orb.create_any();
                ComponentKindValueHelper.insert(
                    value,
                    ComponentKindValue.SESSION);
                configValues[0] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value,
                        value);

                value = this.orb.create_any();
                ServantLifetimeValueHelper.insert(
                    value,
                    ServantLifetimeValue.CONTAINER_LIFETIME);
                configValues[1] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.SERVANT_LIFETIME.value,
                        value);

                value = this.orb.create_any();
                ThreadingPolicyValueHelper.insert(
                    value,
                    ThreadingPolicyValue.MULTITHREAD);
                configValues[2] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.THREADING_POLICY.value,
                        value);

                value = this.orb.create_any();
                value.insert_string(PHILOSOPHER_HOME_REP_ID);
                configValues[3] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID.value,
                        value);

                value = this.orb.create_any();
                value.insert_string(PHILOSOPHER_HOME_SERVANT_NAME);
                configValues[4] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.HOME_SERVANT_CLASSNAME.value,
                        value);

                philosopherHome =
                    this.container.install_home(
                        PHILOSOPHER_HOME_UID,
                        PHILOSOPHER_HOME_ENTRY_POINT,
                        configValues);
                testOk = true;
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
            } catch (org.omg.Components.Deployment.InstallationFailure insf) {
                println("Unexpected exception raised: Components::Deployment::InstallationFailure ");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
            } catch (org.omg.CORBA.UserException ue) {
                println("Unexpected User exception raised: " + ue.toString());
            }
            check(testOk);

            println("Install a ObserverHome");
            testOk = false;

            try {
                configValues = new org.omg.Components.ConfigValue[6];
                value = this.orb.create_any();
                ComponentKindValueHelper.insert(
                    value,
                    ComponentKindValue.SESSION);
                configValues[0] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value,
                        value);

                value = this.orb.create_any();
                ServantLifetimeValueHelper.insert(
                    value,
                    ServantLifetimeValue.CONTAINER_LIFETIME);
                configValues[1] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.SERVANT_LIFETIME.value,
                        value);

                value = this.orb.create_any();
                ThreadingPolicyValueHelper.insert(
                    value,
                    ThreadingPolicyValue.MULTITHREAD);
                configValues[2] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.THREADING_POLICY.value,
                        value);

                value = this.orb.create_any();
                value.insert_string(OBSERVER_HOME_REP_ID);
                configValues[3] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID.value,
                        value);

                value = this.orb.create_any();
                value.insert_string(OBSERVER_HOME_SERVANT_NAME);
                configValues[4] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.HOME_SERVANT_CLASSNAME.value,
                        value);

                ValuetypeFactoryDescription[] seq
                    = new ValuetypeFactoryDescription[1];
                // Event StatusInfo valuetype
                ValuetypeFactoryDescription desc = new ValuetypeFactoryDescription();
                desc.repid = EVENT_STATUSINFO_REP_ID;
                desc.factory_entrypoint = EVENT_STATUSINFO_ENTRYPOINT;
                seq[0] = desc;

                value = this.orb.create_any();
                ValuetypeFactoryDescriptionSeqHelper.insert(value, seq);
                configValues[5] = new ConfigValueImpl(
                    com.thalesgroup.CdmwDeployment.VALUETYPE_FACTORY_DEPENDENCIES.value,
                    value);

                observerHome =
                    this.container.install_home(
                        OBSERVER_HOME_UID,
                        OBSERVER_HOME_ENTRY_POINT,
                        configValues);
                testOk = true;
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
            } catch (org.omg.Components.Deployment.InstallationFailure insf) {
                println("Unexpected exception raised: Components::Deployment::InstallationFailure");
            }
            check(testOk);
            println("Install a ForkHome");
            testOk = false;

            try {
                configValues = new org.omg.Components.ConfigValue[5];
                value = this.orb.create_any();
                ComponentKindValueHelper.insert(
                    value,
                    ComponentKindValue.SESSION);
                configValues[0] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value,
                        value);

                value = this.orb.create_any();
                ServantLifetimeValueHelper.insert(
                    value,
                    ServantLifetimeValue.CONTAINER_LIFETIME);
                configValues[1] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.SERVANT_LIFETIME.value,
                        value);

                value = this.orb.create_any();
                ThreadingPolicyValueHelper.insert(
                    value,
                    ThreadingPolicyValue.MULTITHREAD);
                configValues[2] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.THREADING_POLICY.value,
                        value);

                value = this.orb.create_any();
                value.insert_string(FORKMANAGER_HOME_REP_ID);
                configValues[3] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID.value,
                        value);

                value = this.orb.create_any();
                value.insert_string(FORKMANAGER_HOME_SERVANT_NAME);
                configValues[4] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.HOME_SERVANT_CLASSNAME.value,
                        value);

                forkHome =
                    this.container.install_home(
                        FORKMANAGER_HOME_UID,
                        FORKMANAGER_HOME_ENTRY_POINT,
                        configValues);
                testOk = true;
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
            } catch (org.omg.Components.Deployment.InstallationFailure insf) {
                println("Unexpected exception raised: Components::Deployment::InstallationFailure");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
            } catch (org.omg.CORBA.UserException ue) {
                println("Unexpected User exception raised: " + ue.toString());
            }
            check(testOk);

            println("Install a RegistratorHome");
            testOk = false;

            try {
                configValues = new org.omg.Components.ConfigValue[5];
                value = this.orb.create_any();
                ComponentKindValueHelper.insert(
                    value,
                    ComponentKindValue.SESSION);
                configValues[0] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.COMPONENT_KIND.value,
                        value);

                value = this.orb.create_any();
                ServantLifetimeValueHelper.insert(
                    value,
                    ServantLifetimeValue.CONTAINER_LIFETIME);
                configValues[1] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.SERVANT_LIFETIME.value,
                        value);

                value = this.orb.create_any();
                ThreadingPolicyValueHelper.insert(
                    value,
                    ThreadingPolicyValue.MULTITHREAD);
                configValues[2] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.THREADING_POLICY.value,
                        value);

                value = this.orb.create_any();
                value.insert_string(REGISTRATOR_HOME_REP_ID);
                configValues[3] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.HOME_REPOSITORY_ID.value,
                        value);

                value = this.orb.create_any();
                value.insert_string(REGISTRATOR_HOME_SERVANT_NAME);
                configValues[4] =
                    new ConfigValueImpl(
                        com.thalesgroup.CdmwDeployment.HOME_SERVANT_CLASSNAME.value,
                        value);

                registratorHome =
                    this.container.install_home(
                        REGISTRATOR_HOME_UID,
                        REGISTRATOR_HOME_ENTRY_POINT,
                        configValues);
                testOk = true;
            } catch (org.omg.CORBA.BAD_PARAM bp) {
                println(bp.toString());
            } catch (org.omg.Components.Deployment.InstallationFailure insf) {
                println("Unexpected exception raised: Components::Deployment::InstallationFailure");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
            } catch (org.omg.CORBA.UserException ue) {
                println("Unexpected User exception raised: " + ue.toString());
            }
            check(testOk);

            println("Create a Philosopher component using create_component() operation");
            testOk = false;
            try {
                org.omg.Components.KeylessCCMHome keylessPhilosopherHome =
                    org.omg.Components.KeylessCCMHomeHelper.narrow(
                        philosopherHome);

                if (keylessPhilosopherHome == null) {
                    println("ERROR: Created home is not a keyless home!");
                } else {
                    ccmPhilosopherComp =
                        keylessPhilosopherHome.create_component();

                    dinnerPhilosopherComp1 =
                        com.acme.Dinner.PhilosopherHelper.narrow(
                            ccmPhilosopherComp);

                    if (dinnerPhilosopherComp1 == null) {
                        println("ERROR: Created component is not a com.acme.Dinner.Philosopher!");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.Components.CreateFailure cf) {
                println("Unexpected exception raised: CreateFailure");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);

            println("Create other Philosopher component using create() operation");
            testOk = false;
            try {
                dinnerPhilosopherHome =
                    com.acme.Dinner.PhilosopherHomeHelper.narrow(
                        philosopherHome);

                if (dinnerPhilosopherHome == null) {
                    println("Home is not a com.acme.Dinner.PhilosopherHome!");
                } else {
                    dinnerPhilosopherComp2 = dinnerPhilosopherHome.create();
                    dinnerPhilosopherComp3 = dinnerPhilosopherHome.create();
                    dinnerPhilosopherComp4 = dinnerPhilosopherHome.create();

                    if (dinnerPhilosopherComp2 == null) {
                        println("ERROR: dinnerPhilosopherComp2 is nil!");
                    } else {
                        if (dinnerPhilosopherComp3 == null) {
                            println("ERROR: dinnerPhilosopherComp3 is nil!");
                        } else {
                            if (dinnerPhilosopherComp4 == null) {
                                println("ERROR: dinnerPhilosopherComp4 is nil!");
                            } else {
                                testOk = true;
                            }
                        }
                    }
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Create a Observer component using create_component() operation");
            testOk = false;
            try {
                org.omg.Components.KeylessCCMHome keylessObserverHome =
                    org.omg.Components.KeylessCCMHomeHelper.narrow(
                        observerHome);

                if (keylessObserverHome == null) {
                    println("ERROR: Created home is not a keyless home!");
                } else {
                    ccmObserverComp = keylessObserverHome.create_component();

                    dinnerObserverComp =
                        com.acme.Dinner.ObserverHelper.narrow(ccmObserverComp);

                    if (dinnerObserverComp == null) {
                        println("ERROR: Created component is not a com.acme.Dinner.Observer!");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.Components.CreateFailure cf) {
                println("Unexpected exception raised: CreateFailure");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);

            println("Create a ForkManager component using create_component() operation");
            testOk = false;
            try {
                org.omg.Components.KeylessCCMHome keylessForkHome =
                    org.omg.Components.KeylessCCMHomeHelper.narrow(forkHome);

                if (keylessForkHome == null) {
                    println("ERROR: Created home is not a keyless home!");
                } else {
                    ccmForkComp = keylessForkHome.create_component();

                    dinnerForkComp1 =
                        com.acme.Dinner.ForkManagerHelper.narrow(ccmForkComp);

                    if (dinnerForkComp1 == null) {
                        println("ERROR: Created component is not a com.acme.Dinner.ForkManager!");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.Components.CreateFailure cf) {
                println("Unexpected exception raised: CreateFailure");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);

            println("Create other ForkManager component using create() operation");
            testOk = false;
            try {
                dinnerForkHome =
                    com.acme.Dinner.ForkHomeHelper.narrow(forkHome);

                if (dinnerForkHome == null) {
                    println("Home is not a com.acme.Dinner.ForkHome!");
                } else {
                    dinnerForkComp2 = dinnerForkHome.create();
                    dinnerForkComp3 = dinnerForkHome.create();
                    dinnerForkComp4 = dinnerForkHome.create();

                    if (dinnerForkComp2 == null) {
                        println("ERROR: dinnerForkComp2 is nil!");
                    } else {
                        if (dinnerForkComp3 == null) {
                            println("ERROR: dinnerForkComp3 is nil!");
                        } else {
                            if (dinnerForkComp4 == null) {
                                println("ERROR: dinnerForkComp4 is nil!");
                            } else {
                                testOk = true;
                            }
                        }
                    }
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Create Registrator component using create() operation");
            testOk = false;
            try {
                dinnerRegistratorHome =
                    com.acme.Dinner.RegistratorHomeHelper.narrow(
                        registratorHome);

                if (dinnerRegistratorHome == null) {
                    println("Home is not a com.acme.Dinner.RegistratorHome!");
                } else {
                    dinnerRegistratorComp = dinnerRegistratorHome.create();

                    if (dinnerRegistratorComp == null) {
                        println("ERROR: dinnerRegistratorComp is nil!");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Testing generated operation on ForkManager component...");
            println("provide_the_fork operation on ForkManager component...");
            testOk = false;
            try {
                forkFacetObj1 = dinnerForkComp1.provide_the_fork();
                forkFacetObj2 = dinnerForkComp2.provide_the_fork();
                forkFacetObj3 = dinnerForkComp3.provide_the_fork();
                forkFacetObj4 = dinnerForkComp4.provide_the_fork();

                if (forkFacetObj1 == null) {
                    println("ERROR: no facet found for ForkManager1 component");
                } else if (forkFacetObj2 == null) {
                    println("ERROR: No facet found for ForkManager2 component");
                } else if (forkFacetObj3 == null) {
                    println("ERROR: No facet found for ForkManager3 component");
                } else if (forkFacetObj4 == null) {
                    println("ERROR: No facet found for ForkManager4 component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);

            println("Connect operation on Philosopher components to ForkManager...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_right(forkFacetObj1);
                dinnerPhilosopherComp1.connect_left(forkFacetObj2);
                dinnerPhilosopherComp2.connect_right(forkFacetObj2);
                dinnerPhilosopherComp2.connect_left(forkFacetObj3);
                dinnerPhilosopherComp3.connect_right(forkFacetObj3);
                dinnerPhilosopherComp3.connect_left(forkFacetObj4);
                dinnerPhilosopherComp4.connect_right(forkFacetObj4);
                dinnerPhilosopherComp4.connect_left(forkFacetObj1);
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Expected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Connect operation again on Philosopher components to ForkManager...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_right(forkFacetObj1);
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Expected exception raised: AlreadyConnected");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Connect operation on Philosopher components to Registrator...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_register(dinnerRegistratorComp);
                dinnerPhilosopherComp2.connect_register(dinnerRegistratorComp);
                dinnerPhilosopherComp3.connect_register(dinnerRegistratorComp);
                dinnerPhilosopherComp4.connect_register(dinnerRegistratorComp);
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_connections operation on Philosopher component...");
            testOk = false;
            try {
                org.omg.Components.ConnectionDescription[] conxDesc =
                    dinnerPhilosopherComp1.get_connections(REGISTER_RECEP_NAME);

                if (conxDesc.length != 1) {
                    println("ERROR: Unexpected number of connection found for Philosopher component => " + conxDesc.length);
                } else {
                    testOk = true;
                }
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_all_publishers operation on Philosopher component...");
            testOk = false;
            try {
                org.omg.Components.PublisherDescription[] publisherDesc =
                    dinnerPhilosopherComp1.get_all_publishers();

                if (publisherDesc.length != 1) {
                    println("ERROR: Unexpected number of publisher found for Philosopher component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_all_emitters operation on Philosopher component...");
            testOk = false;
            try {
                org.omg.Components.EmitterDescription[] emitterDesc =
                    dinnerPhilosopherComp1.get_all_emitters();

                if (emitterDesc.length != 1) {
                    println("ERROR: Unexpected number of emitter found for Philosopher component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_all_consumers operation on Observer component...");
            testOk = false;
            try {
                org.omg.Components.ConsumerDescription[] consumerDesc =
                    dinnerObserverComp.get_all_consumers();

                if (consumerDesc.length != 1) {
                    println("ERROR: Unexpected number of consumers found for Observer component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_named_consumers operation on Observer component...");
            testOk = false;
            try {
                String[] consumerNameList = new String[1];
                consumerNameList[0] = INFO_CONSUMER_NAME;
                org.omg.Components.ConsumerDescription[] consumerDesc =
                    dinnerObserverComp.get_named_consumers(consumerNameList);

                if (consumerDesc.length != 1) {
                    println("ERROR: Unexpected number of consumers found for Observer component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_consumer_info operation on Observer component...");
            testOk = false;
            try {
                infoConsumer = dinnerObserverComp.get_consumer_info();

                if (infoConsumer == null) {
                    println("ERROR: no consumer found for Observer component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Subscribe operation on Philosopher components from Observer...");
            testOk = false;
            try {
                ck1 = dinnerPhilosopherComp1.subscribe_info(infoConsumer);
                ck2 = dinnerPhilosopherComp2.subscribe_info(infoConsumer);
                ck3 = dinnerPhilosopherComp3.subscribe_info(infoConsumer);
                ck4 = dinnerPhilosopherComp4.subscribe_info(infoConsumer);
                testOk = true;
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Try to unsubscribe operation on Philosopher1 components with invalid cookie...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.unsubscribe_info(ck2);
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Expected exception raised: InvalidConnection");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Unsubscribe operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.unsubscribe_info(ck1);
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Subscribe operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                ck1 = dinnerPhilosopherComp1.subscribe_info(infoConsumer);
                ck2_bis = dinnerPhilosopherComp2.subscribe_info(infoConsumer);
                testOk = true;
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Connect_config operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_config(infoConsumer);
                testOk = true;
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Try to connect_config again operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_config(infoConsumer);
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Expected exception raised: AlreadyConnected");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Disconnect_config operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                org.omg.Components.EventConsumerBase consumer =
                    dinnerPhilosopherComp1.disconnect_config();
                testOk = true;
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Disconnect_config operation again on Philosopher1 components from Observer...");
            testOk = false;
            try {
                org.omg.Components.EventConsumerBase consumer =
                    dinnerPhilosopherComp1.disconnect_config();
            } catch (org.omg.Components.NoConnection nc) {
                println("Expected exception raised: NoConnection");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Disconnect_config operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                org.omg.Components.EventConsumerBase consumer =
                    dinnerPhilosopherComp2.disconnect_config();
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Try to connect_config again operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_config(infoConsumer);
                testOk = true;
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_ccm_home operation on Observer component...");
            testOk = false;
            try {
                org.omg.Components.CCMHome ccmHome =
                    dinnerObserverComp.get_ccm_home();

                if (!ccmHome._is_equivalent(observerHome)) {
                    println("ERROR: Unexpected home for Observer component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_primary_key operation on Observer component...");
            testOk = false;
            try {
                org.omg.Components.PrimaryKeyBase key =
                    dinnerObserverComp.get_primary_key();
            } catch (org.omg.Components.NoKeyAvailable nka) {
                println("Expected exception raised: NoKeyAvailable");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_all_ports operation on Philosopher component...");
            testOk = false;
            try {
                org.omg.Components.ComponentPortDescription portDesc =
                    dinnerPhilosopherComp2.get_all_ports();

                org.omg.Components.FacetDescription[] facetDesc =
                    portDesc.facets;
                org.omg.Components.ReceptacleDescription[] recepDesc =
                    portDesc.receptacles;
                org.omg.Components.ConsumerDescription[] consumerDesc =
                    portDesc.consumers;
                org.omg.Components.EmitterDescription[] emitterDesc =
                    portDesc.emitters;
                org.omg.Components.PublisherDescription[] publisherDesc =
                    portDesc.publishers;

                if (facetDesc.length != 0) {
                    println("ERROR: unexpected number of facet for Philosopher component");
                } else if (recepDesc.length != 3) {
                    println("ERROR: unexpected number of receptacle for Philosopher component");
                } else if (consumerDesc.length != 0) {
                    println("ERROR: unexpected number of consumer for Philosopher component");
                } else if (publisherDesc.length != 1) {
                    println("ERROR: unexpected number of publisher for Philosopher component");
                } else if (emitterDesc.length != 1) {
                    println("ERROR: unexpected number of emitter for Philosopher component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Configuration_complete operation on Philosopher components...");
            testOk = false;
            try {
                println("Configuration_complete operation on Philosopher1 components...");
                dinnerPhilosopherComp1.configuration_complete();

                OS.sleep(timescale * 1000);

                println("Configuration_complete operation on Philosopher2 components...");
                dinnerPhilosopherComp2.configuration_complete();
                OS.sleep(timescale * 1000);

                println("Configuration_complete operation on Philosopher3 components...");
                dinnerPhilosopherComp3.configuration_complete();
                OS.sleep(timescale * 1000);

                println("Configuration_complete operation on Philosopher4 components...");
                dinnerPhilosopherComp4.configuration_complete();
                OS.sleep(timescale * 1000);

                testOk = true;
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Unexpected exception raised: InvalidConfiguration");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);

            println("Configuration_complete operation on Observer component...");
            testOk = false;
            try {
                dinnerObserverComp.configuration_complete();
                testOk = true;
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Unexpected exception raised: InvalidConfiguration");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);

            println("Get name of Philosopher component1...");
            testOk = false;
            try {
                String philosopherName = dinnerPhilosopherComp1.name();
                println("Philosopher1 name is " + philosopherName);

                philosopherName = dinnerPhilosopherComp2.name();
                println("Philosopher1 name is " + philosopherName);

                philosopherName = dinnerPhilosopherComp3.name();
                println("Philosopher1 name is " + philosopherName);

                philosopherName = dinnerPhilosopherComp4.name();
                println("Philosopher1 name is " + philosopherName);

                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);

            OS.sleep(timescale * 30000);

            println("Disconnect_left and right operation on Philosopher component...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.disconnect_left();
                dinnerPhilosopherComp2.disconnect_left();
                dinnerPhilosopherComp3.disconnect_left();
                dinnerPhilosopherComp4.disconnect_left();
                dinnerPhilosopherComp1.disconnect_right();
                dinnerPhilosopherComp2.disconnect_right();
                dinnerPhilosopherComp3.disconnect_right();
                dinnerPhilosopherComp4.disconnect_right();
                testOk = true;
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
            }
            check(testOk);

            println("Disconnect_register operation on Philosopher components...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.disconnect_register();
                dinnerPhilosopherComp2.disconnect_register();
                dinnerPhilosopherComp3.disconnect_register();
                dinnerPhilosopherComp4.disconnect_register();
                testOk = true;
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Unsubscribe_info operation on Philosopher components...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.unsubscribe_info(ck1);
                dinnerPhilosopherComp2.unsubscribe_info(ck2);
                dinnerPhilosopherComp3.unsubscribe_info(ck3);
                dinnerPhilosopherComp4.unsubscribe_info(ck4);
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Disconnect_config operation on Philosopher components...");
            testOk = false;
            try {
                org.omg.Components.EventConsumerBase consumer =
                    dinnerPhilosopherComp1.disconnect_config();
                testOk = true;
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Remove operation on the Philosopher home...");
            testOk = false;
            try {
                this.container.remove_home(dinnerPhilosopherHome);
                testOk = true;
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: NoKeyAvailable");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Remove operation on the Observer home...");
            testOk = false;
            try {
                this.container.remove_home(observerHome);
                testOk = true;
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: NoKeyAvailable");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Remove operation on the Fork home...");
            testOk = false;
            try {
                this.container.remove_home(forkHome);
                testOk = true;
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: NoKeyAvailable");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Remove operation on the Registrator home...");
            testOk = false;
            try {
                this.container.remove_home(registratorHome);
                testOk = true;
            } catch (org.omg.Components.RemoveFailure rf) {
                println("Unexpected exception raised: NoKeyAvailable");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            // remove container
            this.container.remove();

        } catch (Exception e) {
            e.printStackTrace();
            println("Unexpected exception raised: " + e.toString());
            check(false);
        }

    }

}


