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

public class PhiloTest extends Testable {

    public static final String PHILOSOPHER_HOME_REP_ID =
        "IDL:acme.com/Cdmw_Dinner/PhilosopherHome:1.0";
    public static final String PHILOSOPHER_HOME_SERVANT_NAME =
        "Cdmw.CCM.CIF.CdmwDinner.SessionPhilosopherHome_impl";
    public static final String PHILOSOPHER_HOME_UID = "PhilosopherHome_1";
    public static final String PHILOSOPHER_HOME_ENTRY_POINT =
        "cdmw.ccm.cif.test.CCMPhilosopherHomeImpl.createCCMPhilosopherHome";

    public static final String OBSERVER_HOME_REP_ID =
        "IDL:acme.com/Cdmw_Dinner/ObserverHome:1.0";
    public static final String OBSERVER_HOME_SERVANT_NAME =
        "Cdmw.CCM.CIF.CdmwDinner.SessionObserverHome_impl";
    public static final String OBSERVER_HOME_UID = "ObserverHome_1";
    public static final String OBSERVER_HOME_ENTRY_POINT =
        "cdmw.ccm.cif.test.CCMObserverHomeImpl.createCCMObserverHome";

    public static final String FORKMANAGER_HOME_REP_ID =
        "IDL:acme.com/Cdmw_Dinner/ForkHome:1.0";
    public static final String FORKMANAGER_HOME_SERVANT_NAME =
        "Cdmw.CCM.CIF.CdmwDinner.SessionForkHome_impl";
    public static final String FORKMANAGER_HOME_UID = "ForkHome_1";
    public static final String FORKMANAGER_HOME_ENTRY_POINT =
        "cdmw.ccm.cif.test.CCMForkHomeImpl.createCCMForkHome";

    public static final String REGISTRATOR_HOME_REP_ID =
        "IDL:acme.com/Cdmw_Dinner/RegistratorHome:1.0";
    public static final String REGISTRATOR_HOME_SERVANT_NAME =
        "Cdmw.CCM.CIF.CdmwDinner.SessionRegistratorHome_impl";
    public static final String REGISTRATOR_HOME_UID = "RegistratorHome_1";
    public static final String REGISTRATOR_HOME_ENTRY_POINT =
        "cdmw.ccm.cif.test.CCMRegistratorHomeImpl.createCCMRegistratorHome";

    public static final String FORK_FACET_NAME = "the_fork";

    public static final String FORK_RIGHT_RECEP_NAME = "right";
    public static final String FORK_LEFT_RECEP_NAME = "left";
    public static final String REGISTER_RECEP_NAME = "register";

    public static final String INFO_CONSUMER_NAME = "info";
    public static final String INFO_PUBLISHER_NAME = "info";
    public static final String CONFIG_EMITTER_NAME = "config";

    public static final String EVENT_STATUSINFO_REP_ID =
        "IDL:acme.com/Dinner/StatusInfo:1.0";
    public static final String EVENT_STATUSINFO_ENTRYPOINT =
        "cdmw.ccm.cif.test.StatusInfoFactoryImpl.createStatusInfoFactory";

    private org.omg.CORBA.ORB orb;

    private org.omg.Components.Deployment.Container container;

    private com.acme.Dinner.StatusInfoConsumer consumer;

    public PhiloTest(
        org.omg.CORBA.ORB orb,
        String containerIOR,
        String nonCdmwConsumerIOR) {

        this.name = "cdmw.ccm.cif.PhiloTest";
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

            obj = this.orb.string_to_object(nonCdmwConsumerIOR);

            if (obj == null) {
                println("TEST FAILED : exception raised during test initialisation!");
            } else {
                this.consumer =
                    com.acme.Dinner.StatusInfoConsumerHelper.narrow(obj);
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
        setNbOfRequestedTestOK(52);

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
        org.omg.Components.EventConsumerBase infoConsumer = null;

        org.omg.Components.CCMHome forkHome = null;
        com.acme.Dinner.ForkHome dinnerForkHome = null;
        org.omg.Components.CCMObject ccmForkComp = null;
        com.acme.Dinner.ForkManager dinnerForkComp1 = null;
        com.acme.Dinner.ForkManager dinnerForkComp2 = null;
        com.acme.Dinner.ForkManager dinnerForkComp3 = null;
        com.acme.Dinner.ForkManager dinnerForkComp4 = null;
        org.omg.CORBA.Object forkFacetObj1 = null;
        org.omg.CORBA.Object forkFacetObj2 = null;
        org.omg.CORBA.Object forkFacetObj3 = null;
        org.omg.CORBA.Object forkFacetObj4 = null;

        org.omg.Components.CCMHome registratorHome = null;
        com.acme.Dinner.RegistratorHome dinnerRegistratorHome = null;
        org.omg.Components.CCMObject ccmRegistratorComp = null;
        com.acme.Dinner.Registrator dinnerRegistratorComp = null;

        org.omg.CORBA.Object statusinfoConsumerObj = null;

        org.omg.Components.Cookie recep_ck = null;
        org.omg.Components.Cookie ck1 = null;
        org.omg.Components.Cookie ck2 = null;
        org.omg.Components.Cookie ck2_bis = null;
        org.omg.Components.Cookie ck3 = null;
        org.omg.Components.Cookie ck4 = null;
        org.omg.Components.Cookie ck4_bis = null;

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
                println("Unexpected exception raised: Components::Deployment::InstallationFailure");
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Check Philosopher component is same using same_component() operation");
            testOk = false;
            try {
                boolean result1 =
                    dinnerPhilosopherComp2.same_component(
                        dinnerPhilosopherComp3);
                boolean result2 =
                    dinnerPhilosopherComp3.same_component(
                        dinnerPhilosopherComp3);

                if (result1 == true) {
                    println("ERROR: dinnerPhilosopherComp2 is not the same component!");
                } else {
                    if (result2 == false) {
                        println("ERROR: dinnerPhilosopherComp3 is the same component!");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Testing Navigation operation on ForkManager1 component...");
            println("Get_all_facets operation on ForkManager component...");
            testOk = false;
            try {
                org.omg.Components.FacetDescription[] facetDesc =
                    dinnerForkComp1.get_all_facets();

                if (facetDesc.length != 1) {
                    println("ERROR: Unexpected number of facet found for ForkManager component");
                } else {
                    forkFacetObj1 = facetDesc[0].facet_ref;
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_named_facets operation on ForkManager2 component...");
            testOk = false;
            try {
                String[] facetNameList = new String[1];
                facetNameList[0] = FORK_FACET_NAME;
                org.omg.Components.FacetDescription[] facetDesc =
                    dinnerForkComp2.get_named_facets(facetNameList);

                if (facetDesc.length != 1) {
                    println("ERROR: Unexpected number of facet found for Philosopher component");
                } else {
                    forkFacetObj2 = facetDesc[0].facet_ref;
                    testOk = true;
                }
            } catch (org.omg.Components.InvalidName ine) {
                println("Unexpected exception raised: InvalidName");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("provide_facet operation on ForkManager3 and ForkManager4 component...");
            testOk = false;
            try {
                forkFacetObj3 = dinnerForkComp3.provide_facet(FORK_FACET_NAME);
                forkFacetObj4 = dinnerForkComp4.provide_facet(FORK_FACET_NAME);

                if (forkFacetObj3 == null) {
                    println("ERROR: No facet found for ForkManager3 component");
                } else {
                    if (forkFacetObj4 == null) {
                        println("ERROR: No facet found for ForkManager4 component");
                    } else {
                        testOk = true;
                    }
                }
            } catch (org.omg.Components.InvalidName ine) {
                println("Unexpected exception raised: InvalidName");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Testing Receptacle operation on Philosopher component...");
            println("Get_all_receptacles operation on Philosopher component...");
            testOk = false;
            try {
                org.omg.Components.ReceptacleDescription[] recepDesc =
                    dinnerPhilosopherComp1.get_all_receptacles();

                if (recepDesc.length != 3) {
                    println("ERROR: Unexpected number of receptacle found for Philosopher component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Connect operation on Philosopher components to ForkManager...");
            testOk = false;
            try {
                recep_ck =
                    dinnerPhilosopherComp1.connect(
                        FORK_RIGHT_RECEP_NAME,
                        forkFacetObj1);
                recep_ck =
                    dinnerPhilosopherComp1.connect(
                        FORK_LEFT_RECEP_NAME,
                        forkFacetObj2);
                recep_ck =
                    dinnerPhilosopherComp2.connect(
                        FORK_RIGHT_RECEP_NAME,
                        forkFacetObj2);
                recep_ck =
                    dinnerPhilosopherComp2.connect(
                        FORK_LEFT_RECEP_NAME,
                        forkFacetObj3);
                recep_ck =
                    dinnerPhilosopherComp3.connect(
                        FORK_RIGHT_RECEP_NAME,
                        forkFacetObj3);
                recep_ck =
                    dinnerPhilosopherComp3.connect(
                        FORK_LEFT_RECEP_NAME,
                        forkFacetObj4);
                recep_ck =
                    dinnerPhilosopherComp4.connect(
                        FORK_RIGHT_RECEP_NAME,
                        forkFacetObj4);
                recep_ck =
                    dinnerPhilosopherComp4.connect(
                        FORK_LEFT_RECEP_NAME,
                        forkFacetObj1);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Expected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Connect operation again on Philosopher components to ForkManager...");
            testOk = false;
            try {
                org.omg.Components.Cookie ck =
                    dinnerPhilosopherComp1.connect(
                        FORK_RIGHT_RECEP_NAME,
                        forkFacetObj1);
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Expected exception raised: AlreadyConnected");
                testOk = true;
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Connect operation on Philosopher components to Registrator...");
            testOk = false;
            try {
                org.omg.Components.Cookie ck =
                    dinnerPhilosopherComp1.connect(
                        REGISTER_RECEP_NAME,
                        dinnerRegistratorComp);
                ck =
                    dinnerPhilosopherComp2.connect(
                        REGISTER_RECEP_NAME,
                        dinnerRegistratorComp);
                ck =
                    dinnerPhilosopherComp3.connect(
                        REGISTER_RECEP_NAME,
                        dinnerRegistratorComp);
                ck =
                    dinnerPhilosopherComp4.connect(
                        REGISTER_RECEP_NAME,
                        dinnerRegistratorComp);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
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
            } catch (org.omg.Components.InvalidName ine) {
                println("Unexpected exception raised: InvalidName");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_consumers operation on Observer component...");
            testOk = false;
            try {
                infoConsumer =
                    dinnerObserverComp.get_consumer(INFO_CONSUMER_NAME);

                if (infoConsumer == null) {
                    println("ERROR: no consumer found for Observer component");
                } else {
                    testOk = true;
                }
            } catch (org.omg.Components.InvalidName ine) {
                println("ERROR: InvalidName exception raised!");
                testOk = false;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Subscribe operation on Philosopher components from Observer...");
            testOk = false;
            try {
                ck1 =
                    dinnerPhilosopherComp1.subscribe(
                        INFO_PUBLISHER_NAME,
                        infoConsumer);
                ck2 =
                    dinnerPhilosopherComp2.subscribe(
                        INFO_PUBLISHER_NAME,
                        infoConsumer);
                ck3 =
                    dinnerPhilosopherComp3.subscribe(
                        INFO_PUBLISHER_NAME,
                        infoConsumer);
                ck4 =
                    dinnerPhilosopherComp4.subscribe(
                        INFO_PUBLISHER_NAME,
                        infoConsumer);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Subscribe operation on Philosopher components from a non cdmw consumer...");
            testOk = false;
            try {
                ck4_bis =
                    dinnerPhilosopherComp4.subscribe(
                        INFO_PUBLISHER_NAME,
                        this.consumer);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Try to unsubscribe operation on Philosopher1 components with invalid cookie...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.unsubscribe(
                    INFO_PUBLISHER_NAME,
                    ck2);
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Expected exception raised: InvalidConnection");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Try to unsubscribe operation on Philosopher1 components with invalid name...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.unsubscribe(
                    CONFIG_EMITTER_NAME,
                    ck1);
            } catch (org.omg.Components.InvalidName in) {
                println("Expected exception raised: InvalidName");
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Unsubscribe operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.unsubscribe(
                    INFO_PUBLISHER_NAME,
                    ck1);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("try to subscribe operation on Philosopher1 components with invalid name...");
            testOk = false;
            try {
                ck1 =
                    dinnerPhilosopherComp1.subscribe(
                        CONFIG_EMITTER_NAME,
                        infoConsumer);
            } catch (org.omg.Components.InvalidName in) {
                println("Expected exception raised: InvalidName");
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Subscribe operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                ck1 =
                    dinnerPhilosopherComp1.subscribe(
                        INFO_PUBLISHER_NAME,
                        infoConsumer);
                ck2_bis =
                    dinnerPhilosopherComp2.subscribe(
                        INFO_PUBLISHER_NAME,
                        infoConsumer);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.ExceededConnectionLimit ecl) {
                println("Unexpected exception raised: ExceededConnectionLimit");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Try to subscribe operation on Philosopher1 components with invalid name...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_consumer(
                    INFO_PUBLISHER_NAME,
                    infoConsumer);
            } catch (org.omg.Components.InvalidName in) {
                println("Expected exception raised: InvalidName");
                testOk = true;
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Connect_consumer operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_consumer(
                    CONFIG_EMITTER_NAME,
                    infoConsumer);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Connect_consumer operation on Philosopher3 components from a non Cdmw Consumer...");
            testOk = false;
            try {
                dinnerPhilosopherComp3.connect_consumer(
                    CONFIG_EMITTER_NAME,
                    this.consumer);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Try to connect_consumer again operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_consumer(
                    CONFIG_EMITTER_NAME,
                    infoConsumer);
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Expected exception raised: AlreadyConnected");
                testOk = true;
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Disconnect_consumer operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                org.omg.Components.EventConsumerBase consumer =
                    dinnerPhilosopherComp1.disconnect_consumer(
                        CONFIG_EMITTER_NAME);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Disconnect_consumer operation again on Philosopher1 components from Observer...");
            testOk = false;
            try {
                org.omg.Components.EventConsumerBase consumer =
                    dinnerPhilosopherComp1.disconnect_consumer(
                        CONFIG_EMITTER_NAME);
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.NoConnection nc) {
                println("Expected exception raised: NoConnection");
                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Try to connect_consumer again operation on Philosopher1 components from Observer...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.connect_consumer(
                    CONFIG_EMITTER_NAME,
                    infoConsumer);

                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.AlreadyConnected ac) {
                println("Unexpected exception raised: AlreadyConnected");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Get_ccmHome operation on Observer component...");
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("ConfigurationComplete operation on Philosopher components...");
            testOk = false;
            try {
                println("ConfigurationComplete operation on Philosopher1 components...");
                dinnerPhilosopherComp1.configuration_complete();

                OS.sleep(timescale * 1000);

                println("ConfigurationComplete operation on Philosopher2 components...");
                dinnerPhilosopherComp2.configuration_complete();

                OS.sleep(timescale * 1000);

                println("ConfigurationComplete operation on Philosopher3 components...");
                dinnerPhilosopherComp3.configuration_complete();

                OS.sleep(timescale * 1000);

                println("ConfigurationComplete operation on Philosopher4 components...");
                dinnerPhilosopherComp4.configuration_complete();
                OS.sleep(timescale * 1000);

                testOk = true;
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Unexpected exception raised: InvalidConfiguration");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
            }
            check(testOk);

            println("ConfigurationComplete operation on Observer component...");
            testOk = false;
            try {
                dinnerObserverComp.configuration_complete();
                testOk = true;
            } catch (org.omg.Components.InvalidConfiguration ic) {
                println("Unexpected exception raised: InvalidConfiguration");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
            }
            check(testOk);

            println("Get name of Philosopher component1...");
            testOk = false;
            try {
                String philosopherName = dinnerPhilosopherComp1.name();
                println("Philosopher1 name is " + philosopherName);

                philosopherName = dinnerPhilosopherComp2.name();
                println("Philosopher2 name is " + philosopherName);

                philosopherName = dinnerPhilosopherComp3.name();
                println("Philosopher3 name is " + philosopherName);

                philosopherName = dinnerPhilosopherComp4.name();
                println("Philosopher4 name is " + philosopherName);

                testOk = true;
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
            }
            check(testOk);

            OS.sleep(timescale * 30000);

            println("Disconnect operation on Philosopher component...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.disconnect(FORK_LEFT_RECEP_NAME, null);
                dinnerPhilosopherComp2.disconnect(FORK_LEFT_RECEP_NAME, null);
                dinnerPhilosopherComp3.disconnect(FORK_LEFT_RECEP_NAME, null);
                dinnerPhilosopherComp4.disconnect(FORK_LEFT_RECEP_NAME, null);
                dinnerPhilosopherComp1.disconnect(FORK_RIGHT_RECEP_NAME, null);
                dinnerPhilosopherComp2.disconnect(FORK_RIGHT_RECEP_NAME, null);
                dinnerPhilosopherComp3.disconnect(FORK_RIGHT_RECEP_NAME, null);
                dinnerPhilosopherComp4.disconnect(FORK_RIGHT_RECEP_NAME, null);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.CookieRequired cr) {
                println("Unexpected exception raised: CookieRequired");
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Disconnect operation on Philosopher components...");
            testOk = false;
            try {
                dinnerPhilosopherComp1.disconnect(REGISTER_RECEP_NAME, null);
                dinnerPhilosopherComp2.disconnect(REGISTER_RECEP_NAME, null);
                dinnerPhilosopherComp3.disconnect(REGISTER_RECEP_NAME, null);
                dinnerPhilosopherComp4.disconnect(REGISTER_RECEP_NAME, null);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.Components.CookieRequired cr) {
                println("Unexpected exception raised: CookieRequired");
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Unsubscribe operation on Philosopher components...");
            testOk = false;
            try {
                println("Unsubscribe operation with ck1...");
                dinnerPhilosopherComp1.unsubscribe(INFO_PUBLISHER_NAME, ck1);
                println("Unsubscribe operation with ck2...");
                dinnerPhilosopherComp2.unsubscribe(INFO_PUBLISHER_NAME, ck2);
                println("Unsubscribe operation with ck2_bis...");
                dinnerPhilosopherComp2.unsubscribe(
                    INFO_PUBLISHER_NAME,
                    ck2_bis);
                println("Unsubscribe operation with ck3...");
                dinnerPhilosopherComp3.unsubscribe(INFO_PUBLISHER_NAME, ck3);
                println("Unsubscribe operation with ck4...");
                dinnerPhilosopherComp4.unsubscribe(INFO_PUBLISHER_NAME, ck4);
                println("Unsubscribe operation with ck4_bis...");
                dinnerPhilosopherComp4.unsubscribe(
                    INFO_PUBLISHER_NAME,
                    ck4_bis);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.InvalidConnection ic) {
                println("Unexpected exception raised: InvalidConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Disconnect_consumer operation on Philosopher components...");
            testOk = false;
            try {
                org.omg.Components.EventConsumerBase consumer =
                    dinnerPhilosopherComp1.disconnect_consumer(
                        CONFIG_EMITTER_NAME);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

            println("Disconnect_consumer operation on Philosopher3 components from Observer...");
            testOk = false;
            try {
                org.omg.Components.EventConsumerBase consumer =
                    dinnerPhilosopherComp3.disconnect_consumer(
                        CONFIG_EMITTER_NAME);
                testOk = true;
            } catch (org.omg.Components.InvalidName in) {
                println("Unexpected exception raised: InvalidName");
            } catch (org.omg.Components.NoConnection nc) {
                println("Unexpected exception raised: NoConnection");
            } catch (org.omg.CORBA.SystemException se) {
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
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
                println("Unexpected system exception raised: " + se.toString());
                testOk = false;
            }
            check(testOk);

        } catch (Exception e) {
            e.printStackTrace();
            println("Unexpected exception raised: " + e.toString());
            check(false);
        }

    }

}


