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

import cdmw.orbsupport.PolicyList;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.lifecycle.RetainObjectDeactivator;
import cdmw.lifecycle.POAThreadingPolicy;
import cdmw.lifecycle.ServantProviderBase;
import cdmw.lifecycle.ObjectUtil;
import cdmw.lifecycle.ObjectRegistrationGuard;

import com.thalesgroup.CF.DataType;
import com.thalesgroup.CF.Resource;
import com.thalesgroup.CF.Device;
import com.thalesgroup.CF.Application;
import com.thalesgroup.CF.ResourceHelper;
import com.thalesgroup.CF.DeviceHelper;
import com.thalesgroup.CF.ApplicationHelper;

import com.thalesgroup.CF.InvalidProperties;
import com.thalesgroup.CF.CreationError;
import com.thalesgroup.CF.CreateApplicationError;
import com.thalesgroup.CF.InvalidResourceNumber;
import com.thalesgroup.CF.DevicePackage.InvalidLocation;
import com.thalesgroup.CFFactories.ResourceFactoryPOA;

/**
 * This class provides an implementation of ResourceFactoryPOA
 * with an unmanaged entity policy
 *
 */
public class UnMngEntityResourceFactoryImpl extends ResourceFactoryPOA {

    /**
     * Maximum number of retries on receiving TRANSIENT exception
     * for registering objects.
     */
    public static final int MAX_RETRIES = 1;

    private org.omg.CORBA.ORB orb;

    private org.omg.PortableServer.POA poa;

    private org.omg.PortableServer.POA objectsPOA;

    private com.thalesgroup.CdmwNamingAndRepository.NameDomain domain;

    private String domainName;

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
            System.out.println("\t[UNMANAGED ENTITY]===> Incarnating " + s);

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
            System.out.println("\t[UNMANAGED ENTITY]===> Etherealizing " + s);

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
     * @param nameDomain
     * @param nameDomainName
     * @param factoryName
     * @param threadingPolicy
     * @param args
     *
     * @exception BadParameterException
     * @exception SystemException
     */
    public UnMngEntityResourceFactoryImpl(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        com.thalesgroup.CdmwNamingAndRepository.NameDomain nameDomain,
        String nameDomainName,
        String factoryName,
        POAThreadingPolicy threadingPolicy,
        String[] args)
        throws BadParameterException, org.omg.CORBA.SystemException {

        this.orb = orb;
        this.poa = parent;
        this.objectsPOA = null;
        this.domain = nameDomain;
        this.domainName = nameDomainName;
        this.factoryName = factoryName;
        this.objectDeactivator = new RetainObjectDeactivator(orb);
        this.servantProvider = null;

        //
        // Create POA for use by the factory
        //
        // Entity Life cycle framework use the following POA policies
        //
        // Lifespan policy            = PERSISTENT
        // Id Assignment policy       = USER_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = NO_IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_SERVANT_MANAGER;
        // Servant Retention policy   = RETAIN;
        //
        org.omg.PortableServer.POAManager poaManager =
            this.poa.the_POAManager();
        PolicyList policies = new PolicyList();
        policies.insert(
            parent.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID));
        policies.insert(
            parent.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.PERSISTENT));
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
                throw new BadParameterException(parameter);
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
        return poa;
    }

    //
    // IDL:thalesgroup.com/CFFactories/ResourceFactory/create_resource:1.0
    //
    /**
     * Method create_resource.
     * @param objectKey
     * @param resourceNumber
     * @param qualifiers
     * @return Resource
     * @throws SystemException
     * @throws InvalidName
     * @throws AlreadyExistant
     * @throws InvalidResourceNumber
     * @throws InvalidProperties
     * @throws CreationError
     */
    public Resource create_resource(
        String objectKey,
        short resourceNumber,
        DataType[] qualifiers)
        throws
            org.omg.CORBA.SystemException,
            com.thalesgroup.CdmwLifeCycle.InvalidName,
            com.thalesgroup.CdmwLifeCycle.AlreadyExistant,
            InvalidResourceNumber,
            InvalidProperties,
            CreationError {

        Resource ref = null;
        // [I] Register the name object_key within the NameDomain. This
        // will check validity of the name and whether it is already in use.
        org.omg.CORBA.Any regId = registerName(objectKey);
        // Object's name has been registered successfuly. From now on, it should be
        // unregistered if an exception has to be rethrown
        ObjectRegistrationGuard unreg =
            new ObjectRegistrationGuard(domain, objectKey);
        try {
            // [II] Acquire oid from user for object_key
            // Operation may throw user exceptions that will re-thrown to caller
            byte[] oid =
                servantProvider.createResource(resourceNumber, qualifiers);
            // [III] Activate the object immediately :
            //       [a] Acquire a servant from user for odi
            //       [b] Activate the object (using a servant activator)
            //       [c] Create the object reference
            //       [d] Assert the object is really of the expected type
            org.omg.CORBA.Object obj =
                ObjectUtil.activateObject(objectsPOA, oid, servantProvider);
            ref = ResourceHelper.narrow(obj);
            // [IV] Register (name,object) within the repository.
            // In case of an exception ObjectRegistrationGuard will try
            // to unregister the name, even if there a high probability
            // that this may not work.
            registerObject(regId, ref, MAX_RETRIES);
            // [V] Return the object reference
            // Everything is OK, so we "do not rollback" the registration
            unreg.cancel();
        } catch (InvalidResourceNumber irn) {
            throw irn;
        } catch (InvalidProperties ip) {
            throw ip;
        } catch (CreationError ce) {
            throw ce;
        } catch (org.omg.CORBA.SystemException se) {
            throw se;
        } finally {
            unreg.destroy();
        }

        return ref;

    }

    //
    // IDL:thalesgroup.com/CFFactories/ResourceFactory/create_device:1.0
    //
    /**
     * Method create_device.
     * @param objectKey
     * @param label
     * @return Device
     * @throws AlreadyExistant
     * @throws InvalidName
     * @throws InvalidLocation
     * @throws SystemException
     */
    public Device create_device(String objectKey, String label)
        throws
            com.thalesgroup.CdmwLifeCycle.AlreadyExistant,
            com.thalesgroup.CdmwLifeCycle.InvalidName,
            InvalidLocation,
            org.omg.CORBA.SystemException {

        Device ref = null;
        // [I] Register the name object_key within the NameDomain. This
        // will check validity of the name and whether it is already in use.
        org.omg.CORBA.Any regId = registerName(objectKey);
        // Object's name has been registered successfuly. From now on,
        // it should be unregistered if an exception has to be rethrown
        ObjectRegistrationGuard unreg =
            new ObjectRegistrationGuard(domain, objectKey);
        try {
            // [II] Acquire oid from user for object_key
            // Operation may throw user exceptions that will rethrown to caller
            byte[] oid = this.servantProvider.createDevice(label);
            // [III] Activate the object immediately :
            //       [a] Acquire a servant from user for odi
            //       [b] Activate the object (using a servant activator)
            //       [c] Create the object reference
            //       [d] Assert the object is really of the expected type
            org.omg.CORBA.Object obj =
                ObjectUtil.activateObject(objectsPOA, oid, servantProvider);
            ref = DeviceHelper.narrow(obj);
            // [IV] Register (name,object) within the repository.
            // In case of an exception ObjectRegistrationGuard will try
            // to unregister the name, even if there a high probability
            // that this may not work.
            registerObject(regId, ref, MAX_RETRIES);
            // [V] Return the object reference
            // Everything is OK, so we "do not rollback" the registration
            unreg.cancel();
        } catch (InvalidLocation il) {
            throw il;
        } catch (org.omg.CORBA.SystemException se) {
            throw se;
        } finally {
            unreg.destroy();
        }

        return ref;

    }

    //
    // IDL:thalesgroup.com/CFFactories/ResourceFactory/create_application:1.0
    //
    /**
     * Method create_application.
     * @param objectKey
     * @param name
     * @return Application
     * @throws InvalidName
     * @throws AlreadyExistant
     * @throws SystemException
     * @throws CreateApplicationError
     */
    public Application create_application(String objectKey, String name)
        throws
            com.thalesgroup.CdmwLifeCycle.InvalidName,
            com.thalesgroup.CdmwLifeCycle.AlreadyExistant,
            org.omg.CORBA.SystemException,
            CreateApplicationError {

        Application ref = null;
        // [I] Register the name object_key within the NameDomain. This
        // will check validity of the name and whether it is already in use.
        org.omg.CORBA.Any regId = registerName(objectKey);
        // Object's name has been registered successfuly. From now on, it should be
        // unregistered if an exception has to be rethrown
        ObjectRegistrationGuard unreg =
            new ObjectRegistrationGuard(domain, objectKey);
        try {
            // [II] Acquire oid from user for object_key
            // Operation may throw user exceptions that will rethrown to caller
            byte[] oid = servantProvider.createApplication(name);
            // [III] Activate the object immediately :
            //       [a] Acquire a servant from user for odi
            //       [b] Activate the object (using a servant activator)
            //       [c] Create the object reference
            //       [d] Assert the object is really of the expected type
            org.omg.CORBA.Object obj =
                ObjectUtil.activateObject(objectsPOA, oid, servantProvider);
            ref = ApplicationHelper.narrow(obj);
            // [IV] Register (name,object) within the repository.
            // In case of an exception ObjectRegistrationGuard will try
            // to unregister the name, even if there a high probability
            // that this may not work.
            registerObject(regId, ref, MAX_RETRIES);
            // [V] Return the object reference
            // Everything is OK, so we "do not rollback" the registration
            unreg.cancel();
        } catch (CreateApplicationError cae) {
            throw cae;
        } catch (org.omg.CORBA.SystemException se) {
            throw se;
        } finally {
            unreg.destroy();
        }

        return ref;

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
        return this.servantProvider.supports(repositoryId);
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

    /**
     * Register the name objectKey within the NameDomain. This
     * will check validity of the name and whether it is already in use.
     *
     * @param objectKey
     * @exception com.thalesgroup.CdmwLifeCycle.AlreadyExistant
     * @exception com.thalesgroup.CdmwLifeCycle.InvalidName
     * @exception org.omg.CORBA.SystemException
     * @return Registration ID as a Any object
     */
    private org.omg.CORBA.Any registerName(String objectKey)
        throws
            com.thalesgroup.CdmwLifeCycle.AlreadyExistant,
            com.thalesgroup.CdmwLifeCycle.InvalidName,
            org.omg.CORBA.SystemException {

        org.omg.CORBA.Any regId;
        try {
            System.out.println("objectKey: " + objectKey);
            regId = domain.new_name(objectKey);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
            // Add more information to help the caller for debuging
            String reason = domainName + "/" + objectKey;
            throw new com.thalesgroup.CdmwLifeCycle.AlreadyExistant(reason);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            throw new com.thalesgroup.CdmwLifeCycle.InvalidName(in.reason);
        } catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
            // Oops! CDMW Repository has been destroyed
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALCdmwRepositoryNotExists,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CORBA.SystemException se) {
            // Cannot reach the CDMW Repository at the moment, retry later!
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        return regId;

    }

    /**
     * Register (name,object) within the repository. The name has already been
     * registered, so this should succeed. In case of a TRANSIENT system
     * exception, there will be a retry max_retries times.
     * If max_retries == 0, then this will do retry forever.
     *
     * @param regId
     * @param theObject
     * @param maxRetries
     * @exception org.omg.CORBA.SystemException
     */
    private void registerObject(
        org.omg.CORBA.Any regId,
        org.omg.CORBA.Object theObject,
        int maxRetries)
        throws org.omg.CORBA.SystemException {

        boolean forever = (maxRetries <= 0);
        int counter = maxRetries;

        do {
            try {
                domain.register_object(regId, theObject);
                break;
            } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
                // Registration will be cancelled by the ObjectRegistrationGuard
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_YES);
            } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.InvalidRegistration ir) {
                // Registration will be cancelled by the ObjectRegistrationGuard
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_YES);
            } catch (org.omg.CORBA.TRANSIENT t) {
                // Retry
            } catch (org.omg.CORBA.SystemException se) {
                // Cancel registration ????
                throw se;
            }
        } while (forever || (--counter > 0));

        if (!forever && (counter <= 0)) {
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        }

    }

}

