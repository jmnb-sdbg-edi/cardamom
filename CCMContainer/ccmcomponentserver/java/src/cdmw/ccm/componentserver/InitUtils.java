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


package cdmw.ccm.componentserver;

import cdmw.common.Options;

import cdmw.ossupport.OS;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.PolicyList;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.orbsupport.CORBASystemExceptionWrapper;

import com.thalesgroup.CdmwCcmComponentServer.ComponentServer;

/**
 * This class provides helper functions to be used
 * by the CDMW generated CdmwInterface.java file.
 *
 * @see cdmw.cdmwinit.CdmwInterface#cdmwInit
 * @see cdmw.cdmwinit.CdmwInterface#cdmwCleanup
 *
 */
public class InitUtils {

    /**
     * Name of the POA root of CDMW ComponentServer
     */
    public static String COMPONENT_SERVER_POA_NAME = "ComponentServerPOA";

    public static boolean getHomeRegistrationOption(String[] args) {

        boolean defHomeRegistration = false;
        String homeRegOption =
            OS.getOptionValue(args, Options.DEFAULT_HOME_REGISTRATION_OPTION);

        if (homeRegOption.equals("yes") || !homeRegOption.equals("no")) {
            defHomeRegistration = true;
        }

        return defHomeRegistration;
    }

    /**
     * Method createComponentServer.
     *
     * @param orb A reference to the ORB pseudo-object
     * @param parent A reference to the CdmwRootPOA pseudo-object
     * @param eventChannelFactory
     * @param processName
     * @param applicationName
     * @param args
     * @return A reference to a CDMW ComponentServer object
     * @throws CORBASystemExceptionWrapper
     */
    public static ComponentServer createComponentServer(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory,
        String processName,
        String applicationName,
        String[] args)
        throws CORBASystemExceptionWrapper {

        try {
            ComponentServer theComponentServer = null;

            // A) Create a set of ConfigValues.
            org.omg.Components.ConfigValue[] cfgCompserver =
                new org.omg.Components.ConfigValue[0];

            // B) Create A ComponentServerPOA (with Retain and Single-thread policicies)
            // to be used to activate the ComponentServer and Container objects.
            // Lifespan policy            = TRANSIENT
            // Id Assignment policy       = SYSTEM_ID
            // Id Uniqueness policy       = UNIQUE_ID
            // Implicit Activation policy = IMPLICIT_ACTIVATION
            // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY
            // Servant Retention policy   = RETAIN;
            // Thread Policy              = SINGLE_THREAD_MODEL
            //
            org.omg.PortableServer.POAManager poaManager =
                parent.the_POAManager();
            org.omg.CORBA.Policy[] policies = new org.omg.CORBA.Policy[7];
            policies[0] =
                parent.create_id_assignment_policy(
                    org.omg.PortableServer.IdAssignmentPolicyValue.SYSTEM_ID);
            policies[1] =
                parent.create_lifespan_policy(
                    org.omg.PortableServer.LifespanPolicyValue.TRANSIENT);
            policies[2] =
                parent.create_servant_retention_policy(
                    org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN);
            policies[3] =
                parent.create_id_uniqueness_policy(
                    org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID);
            policies[4] =
                parent.create_request_processing_policy(
                    org.omg.PortableServer.RequestProcessingPolicyValue.USE_ACTIVE_OBJECT_MAP_ONLY);
            policies[5] =
                parent.create_implicit_activation_policy(
                    org.omg.PortableServer.ImplicitActivationPolicyValue.IMPLICIT_ACTIVATION);
            policies[6] =
                parent.create_thread_policy(
                    org.omg.PortableServer.ThreadPolicyValue.SINGLE_THREAD_MODEL);

            StrategyList poaStrategies = new StrategyList();

            org.omg.PortableServer.POA csPoa =
                ORBSupport.createPOA(
                    parent,
                    COMPONENT_SERVER_POA_NAME,
                    poaManager,
                    new PolicyList(policies),
                    poaStrategies);

            // C) Create the ComponentServer servant
            org.omg.Components.HomeRegistration homeRegistration = null;

            if (getHomeRegistrationOption(args)) {
                //
                // Get naming service
                //
                try {
                    org.omg.CosNaming.NamingContext nc = null;
                    org.omg.CORBA.Object obj =
                        orb.resolve_initial_references("NameService");
                    if (obj == null) {
                        System.err.println(
                            "NameService is a nil object reference");
                    } else {
                        nc = org.omg.CosNaming.NamingContextHelper.narrow(obj);
                        if (nc == null) {
                            System.err.println(
                                "NameService is not a NamingContext object reference");
                        } else {
                            homeRegistration = new DefaultHomeRegistration(nc);
                        }
                    }
                } catch (org.omg.CORBA.ORBPackage.InvalidName in) {
                    System.err.println("Can't resolve `NameService'");
                }
            } else {
                homeRegistration = new StandardHomeRegistration();
            }

            ComponentServerImpl cs =
                new ComponentServerImpl(
                    orb,
                    csPoa,
                    homeRegistration,
                    eventChannelFactory,
                    processName,
                    applicationName,
                    cfgCompserver);

            // D) Activate the ComponentServer
            theComponentServer = cs._this(orb);

            return theComponentServer;
        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            // A component server is a singleton!
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO));
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO));
        } catch (org.omg.CORBA.SystemException se) {
            throw new CORBASystemExceptionWrapper(se);
        }

    }

}

