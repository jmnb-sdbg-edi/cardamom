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


package cdmw.resources;

import cdmw.common.Assert;
import cdmw.common.BadParameterException;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.PolicyList;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.lifecycle.RetainObjectDeactivator;
import cdmw.lifecycle.POAThreadingPolicy;
import cdmw.lifecycle.ServantProviderBase;
import cdmw.lifecycle.ObjectUtil;

import com.thalesgroup.CF.DataType;
import com.thalesgroup.CF.Resource;
import com.thalesgroup.CF.Device;
import com.thalesgroup.CF.Application;
import com.thalesgroup.CF.ResourceHelper;
import com.thalesgroup.CF.DeviceHelper;
import com.thalesgroup.CF.ApplicationHelper;

import com.thalesgroup.CF.InvalidProperties;
import com.thalesgroup.CF.CreationError;
import com.thalesgroup.CF.InvalidResourceNumber;
import com.thalesgroup.CF.CreateApplicationError;
import com.thalesgroup.CF.DevicePackage.InvalidLocation;
import com.thalesgroup.CFFactories.AnonymousResourceFactoryPOA;

/**
 * This class provides an implementation of AnonymousResourceFactoryPOA
 * with a session policy
 *
 */
public class SessionResourceFactoryImpl extends AnonymousResourceFactoryPOA {

    /**
     * Maximum number of retries on receiving TRANSIENT exception
     * for registering objects.
     */
    public final static int MAX_RETRIES = 1;

    private org.omg.CORBA.ORB orb;

    private org.omg.PortableServer.POA poa;

    private org.omg.PortableServer.POA objectsPOA;

    private String factoryName;

    private RetainObjectDeactivator objectDeactivator;

    private ResourceServantProvider servantProvider;

    // ----------------------------------------------------------------------
    // Resource Servant activator
    // ----------------------------------------------------------------------

    public class TheServantActivator
        extends org.omg.CORBA.LocalObject
        implements org.omg.PortableServer.ServantActivator {

        private ServantProviderBase servantProvider;

        public TheServantActivator(ServantProviderBase provider) {
            this.servantProvider = provider;
            Assert.check(this.servantProvider != null);
        }

        public org.omg.PortableServer.Servant incarnate(
            byte[] oid,
            org.omg.PortableServer.POA poa)
            throws org.omg.PortableServer.ForwardRequest {

            Assert.check(this.servantProvider != null);

            // TODO: Begin for test only
            String s = new String(oid);
            System.out.println("\t[SESSION]===> Incarnating " + s);

            return servantProvider.lookupServant(oid);
        }

        public void etherealize(
            byte[] oid,
            org.omg.PortableServer.POA poa,
            org.omg.PortableServer.Servant servant,
            boolean cleanupInProgress,
            boolean remainingActivations) {

            Assert.check(this.servantProvider != null);

            // TODO: Begin for test only
            String s = new String(oid);
            System.out.println("\t[SESSION]===> Etherealizing " + s);

            // TODO: End for test only
            if (!remainingActivations) {
                this.servantProvider.releaseServant(oid, poa, servant);
            }
        }

    }

    /**
     * Constructor
     *
     * @param orb
     * @param parent
     * @param factoryName
     * @param threadingPolicy
     * @param args
    
     * @exception BadParameterException
     * @exception org.omg.CORBA.SystemException
     */
    public SessionResourceFactoryImpl(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        String factoryName,
        POAThreadingPolicy threadingPolicy,
        String[] args)
        throws BadParameterException, org.omg.CORBA.SystemException {

        this.orb = orb;
        this.poa = parent;
        this.objectsPOA = null;
        this.factoryName = factoryName;
        this.objectDeactivator = new RetainObjectDeactivator(orb);
        this.servantProvider = null;

        //
        // Create POA for use by the factory
        //
        // Service Life cycle framework use the following POA policies
        //
        // Lifespan policy            = TRANSIENT
        // Id Assignment policy       = USER_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = NO_IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_SERVANT_MANAGER;
        // Servant Retention policy   = RETAIN;
        //
        org.omg.PortableServer.POAManager poaManager = parent.the_POAManager();
        PolicyList policies = new PolicyList();
        policies.insert(
            parent.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID));
        policies.insert(
            parent.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.TRANSIENT));
        policies.insert(
            parent.create_servant_retention_policy(
                org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN));
        policies.insert(
            parent.create_id_uniqueness_policy(
                org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID));
        policies.insert(
            parent.create_request_processing_policy(
                org.omg.PortableServer.RequestProcessingPolicyValue.USE_SERVANT_MANAGER));
        policies.insert(
            parent.create_implicit_activation_policy(
                org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION));

        StrategyList poaStrategies = new StrategyList();
        // Strategies with MULTITHREAD policy will accept Single Thread and
        // Multi Thread strategies. However, SERIALIZE policy will only allow
        // Single Thread policy.
        threadingPolicy.appendToStrategyList(poaStrategies);

        try {
            objectsPOA =
                ORBSupport.createPOA(
                    poa,
                    factoryName,
                    poaManager,
                    policies,
                    poaStrategies);

            //
            // Create and install servant provider
            //
            cdmw.resources.USPAllocator allocator =
                new cdmw.resources.USPAllocator();
            cdmw.lifecycle.ServantProviderBase srvprvd = 
                allocator.allocate(orb,
                                   objectsPOA,
                                   objectDeactivator,
                                   args);
            Assert.check(srvprvd != null);
            try {
                servantProvider = (cdmw.resources.ResourceServantProvider) srvprvd;
            } catch (ClassCastException e) {
                // Shouldn't happen since cdmw.resources.USPAllocator.allocate()
                // must return a cdmw.resources.ResourceServantProvider
                Assert.check(false);
            }

            // Hand Shaking with the Servant Provider
            // IDs shall be unique
            if (servantProvider.mayIncarnateMultipleObjects()) {
                String parameter = "ObjectId Uniqueness(" + factoryName + ")";
                throw new BadParameterException();
            }

            //
            // Create and install servant activator
            //
            objectsPOA.set_servant_manager(new TheServantActivator(servantProvider));

        } catch (BadParameterException bpe) {
            throw bpe;
        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        }

    }

    /**
     * Returns the default POA.
     *
     * @return The default POA.
     */
    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

    //
    // IDL:thalesgroup.com/CFFactories/AnonymousResourceFactory/create_resource:1.0
    //
    /**
     * Method create_resource.
     * @param resourceNumber
     * @param qualifiers
     * @return Resource
     * @throws InvalidResourceNumber
     * @throws InvalidProperties
     * @throws CreationError
     */
    public Resource create_resource(
        short resourceNumber,
        DataType[] qualifiers)
        throws InvalidResourceNumber, InvalidProperties, CreationError {

        // [I] Acquire oid from user
        // Operation may throw user exceptions that will re-thrown to caller
        byte[] oid = servantProvider.createResource(resourceNumber, qualifiers);
        // [II] Create its object reference
        org.omg.CORBA.Object obj =
            ObjectUtil.createObjectReference(objectsPOA, oid, servantProvider);
        return ResourceHelper.narrow(obj);

    }

    //
    // IDL:thalesgroup.com/CFFactories/AnonymousResourceFactory/create_device:1.0
    //
    /**
     * Method create_device.
     * @param label
     * @return Device
     * @throws InvalidLocation
     */
    public Device create_device(String label) throws InvalidLocation {

        // [I] Acquire oid from user
        // Operation may throw user exceptions that will re-thrown to caller
        byte[] oid = this.servantProvider.createDevice(label);
        // [II] Create its object reference
        org.omg.CORBA.Object obj =
            ObjectUtil.createObjectReference(objectsPOA, oid, servantProvider);
        return DeviceHelper.narrow(obj);

    }

    //
    // IDL:thalesgroup.com/CFFactories/AnonymousResourceFactory/create_application:1.0
    //
    /**
     * Method create_application.
     * @param name
     * @return Application
     * @throws CreateApplicationError
     */
    public Application create_application(String name)
        throws CreateApplicationError {

        // [I] Acquire oid from user
        // Operation may throw user exceptions that will re-thrown to caller
        byte[] oid = this.servantProvider.createApplication(name);
        // [II] Create its object reference
        org.omg.CORBA.Object obj =
            ObjectUtil.createObjectReference(objectsPOA, oid, servantProvider);
        return ApplicationHelper.narrow(obj);
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    /**
     * Tests if the servant provider supports an object
     * represented by its repositoryId
     * Supports the following objects:
     *     IDL:thalesgroup.com/CF/Resource:1.0
     *     IDL:thalesgroup.com/CF/Device:1.0
     *     IDL:thalesgroup.com/CF/Application:1.0
     *
     * @param repositoryId
     * @return True if the servant provider supports the object otherwise false.
     */
    public boolean supports(String repositoryId) {
        return servantProvider.supports(repositoryId);
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    /**
     * Returns the supported objects by the servant provider
     *
     * @return List of the supported objects as a String table
     */
    public String[] get_supported_objects() {
        return this.servantProvider.getSupportedObjects();
    }

}

