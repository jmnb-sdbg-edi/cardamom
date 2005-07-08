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


package cdmw.ccm.container;

import cdmw.common.Assert;
import cdmw.ossupport.OS;
import cdmw.ossupport.Timeval;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.PolicyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.lifecycle.ServantProviderBase;
import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.InvalidNameException;

import cdmw.ccm.common.CCMUtils;

import com.thalesgroup.CdmwDeployment.ServantLifetimeValue;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValue;

/**
 * Implementation of the Session2Context internal interface for use with
 * Session container type.
 */
public class Session2ContextImpl
    extends org.omg.CORBA.LocalObject
    implements com.thalesgroup.CdmwCcmContainer.Session2Context {

    private org.omg.PortableServer.Current current = null;

    private org.omg.PortableServer.POA poa = null;

    private org.omg.Components.CCMHome home;

    private org.omg.Components.HomeRegistration homeRegistration;

    private
        com.thalesgroup.CdmwDeployment.ServantLifetimeValue servantLifetime;

    private com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory;

    private String processName;

    private String applicationName;

    // **********************************************************************
    // A SERVANT ACTIVATOR
    // **********************************************************************

    public class TheServantActivator
        extends org.omg.CORBA.LocalObject
        implements org.omg.PortableServer.ServantActivator {

        private ComponentServantProviderInterface servantProvider;

        public TheServantActivator(ComponentServantProviderInterface provider) {

            this.servantProvider = provider;
            Assert.check(provider != null);
            try {
                org.omg.PortableServer.Servant servant
                    = (org.omg.PortableServer.Servant) provider;
            } catch(ClassCastException cce) {
                Assert.check(false);
            }

        }

        public org.omg.PortableServer.Servant incarnate(
            byte[] oid,
            org.omg.PortableServer.POA poa)
            throws org.omg.PortableServer.ForwardRequest {
            Assert.check(servantProvider != null);
            return servantProvider.lookupServant(oid);
        }

        public void etherealize(
            byte[] oid,
            org.omg.PortableServer.POA poa,
            org.omg.PortableServer.Servant servant,
            boolean cleanupInProgress,
            boolean remainingActivations) {
            Assert.check(servantProvider != null);
            if(!remainingActivations) {
                servantProvider.releaseServant(oid, poa, servant);
            }
        }

    }

    // **********************************************************************
    // A SERVANT LOCATOR
    // **********************************************************************

    public class TheServantLocator
        extends org.omg.CORBA.LocalObject
        implements org.omg.PortableServer.ServantLocator {

        private ComponentServantProviderInterface servantProvider;

        public TheServantLocator(ComponentServantProviderInterface provider) {
            this.servantProvider = provider;
            Assert.check(provider != null);
            try {
                org.omg.PortableServer.Servant servant
                    = (org.omg.PortableServer.Servant) provider;
            } catch(ClassCastException cce) {
                Assert.check(false);
            }
        }

        public org.omg.PortableServer.Servant preinvoke(
            byte[] oid,
            org.omg.PortableServer.POA poa,
            String operation,
            org.omg.PortableServer.ServantLocatorPackage.CookieHolder theCookie)
            throws org.omg.PortableServer.ForwardRequest {
            Assert.check(servantProvider != null);
            return servantProvider.lookupServant(oid);
        }

        public void postinvoke(
            byte[] oid,
            org.omg.PortableServer.POA poa,
            String operation,
            Object theCookie,
            org.omg.PortableServer.Servant theServant) {
            Assert.check(servantProvider != null);
            servantProvider.releaseServant(oid, poa, theServant);
        }

    }

    /**
     * Constructor.
     *
     * @param orb A reference to the ORB pseudo-object
     * @param parent A parent POA to the home instance-specific POA.
     * @param home A reference to the component's home
     * @param homeId Unique Identifier to a home
     * within the context of its container.
     * @param servantProvider A component/facet servant provider.
     * @param servantLifetime Servant Lifetime
     * @param threadingPolicy Threading Policy
     * @param homeRegistration A reference to a HomeRegistration for use
     * to register component homes so they may be located by the HomeFinder.
     */
    public Session2ContextImpl(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        org.omg.Components.CCMHome home,
        String homeId,
        ComponentServantProviderInterface servantProvider,
        com.thalesgroup.CdmwDeployment.ServantLifetimeValue servantLifetime,
        com.thalesgroup.CdmwDeployment.ThreadingPolicyValue threadingPolicy,
        org.omg.Components.HomeRegistration homeRegistration,
        com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory,
        String processName,
        String applicationName) {

        this.home = home;
        this.homeRegistration = homeRegistration;
        this.servantLifetime = servantLifetime;
        this.eventChannelFactory = eventChannelFactory;
        this.processName = processName;
        this.applicationName = applicationName;

        Assert.check(orb != null);
        try {
            org.omg.CORBA.Object obj =
                orb.resolve_initial_references("POACurrent");
            this.current = org.omg.PortableServer.CurrentHelper.narrow(obj);

            // A Context object is created at home installation.
            // This will setup a whole lifecycle framework
            // for components and components' facets managed by this home.
            //
            // A lifecycle framework contains the following actors:
            //   A - *) An object factory ( the home created by the container)
            //   B - *) An object adapter
            //   C - *) A servant manager
            //   D - *) A servant provider
            //
            // A) Create the object adapter
            // POA policies depend on the component_kind, lifetime_policy,
            // and threading_policy

            org.omg.PortableServer.POAManager poaManager =
                parent.the_POAManager();
            org.omg.CORBA.Policy[] policies = new org.omg.CORBA.Policy[7];
            policies[0] = parent.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID);
            policies[1] = parent.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.TRANSIENT);
            if (servantLifetime.value() == ServantLifetimeValue.COMPONENT_LIFETIME.value() ||
                servantLifetime.value() == ServantLifetimeValue.CONTAINER_LIFETIME.value()) {
                policies[2] = parent.create_servant_retention_policy(
                    org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN);
            } else if (servantLifetime.value() == ServantLifetimeValue.METHOD_LIFETIME.value()) {
                policies[2] = parent.create_servant_retention_policy(
                    org.omg.PortableServer.ServantRetentionPolicyValue.NON_RETAIN);
            } else {
                // TRANSACTION_LIFETIME servant lifetime policy is not supported!
                throw new org.omg.CORBA.NO_IMPLEMENT(
                    ExceptionMinorCodes.NO_IMPLEMENTTransactionsNotSupported,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
            policies[3] = parent.create_id_uniqueness_policy(
                org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID);
            policies[4] = parent.create_request_processing_policy(
                org.omg.PortableServer.RequestProcessingPolicyValue.USE_SERVANT_MANAGER);
            policies[5] = parent.create_implicit_activation_policy(
                org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION);
            if (threadingPolicy.value() == ThreadingPolicyValue.SERIALIZE.value()) {
                policies[6] = parent.create_thread_policy(
                    org.omg.PortableServer.ThreadPolicyValue.SINGLE_THREAD_MODEL);
            } else {
                policies[6] = parent.create_thread_policy(
                    org.omg.PortableServer.ThreadPolicyValue.ORB_CTRL_MODEL);
            }

            StrategyList poaStrategies = new StrategyList();
            this.poa = ORBSupport.createPOA(
                parent,
                homeId,
                poaManager,
                new PolicyList(policies),
                poaStrategies);

            //
            // Create and install servant activator
            //
            org.omg.PortableServer.ServantManager theServantManager = null;
            if (servantLifetime.value() == ServantLifetimeValue.METHOD_LIFETIME.value()) {
                theServantManager = new TheServantLocator(servantProvider);
            } else {
                theServantManager = new TheServantActivator(servantProvider);
            }
            this.poa.set_servant_manager(theServantManager);
        } catch (Exception e) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

    }

    /**
     * Destructor
     *
     */
    public void destroy() {
        // Delete home's POA
        try {
            this.poa.destroy(true, false);
            this.poa = null;
        } catch (Exception e) {}
    }

    //--- From CCMContext -----------------------------------------------//
    //
    // IDL:omg.org/Components/CCMContext/get_caller_principal:1.0
    //
    /**
     * The get_caller_principal operation obtains the CORBA security
     * credentials in effect for the caller. Security on the server is
     * primarily controlled by the security policy in the deployment descriptor
     * for this component. The component may use this operation to
     * determine the credentials associated with the current client invocation.
     *
     * <p/><B>NOTA:</B> This is NOT implemented in this version.
     *
     * @return A reference to the caller pricipal.
     */
    public org.omg.SecurityLevel2.Credentials get_caller_principal() {
        throw new org.omg.CORBA.NO_IMPLEMENT(
            ExceptionMinorCodes.NO_IMPLEMENTSecurityNotSupported,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    }

    //
    // IDL:omg.org/Components/CCMContext/get_CCM_home:1.0
    //
    /**
     * The get_CCM_home operation is used to obtain a reference
     * to the home interface.
     *
     * @return A reference to the component's home
     */
    public org.omg.Components.CCMHome get_CCM_home() {
        return this.home;
    }

    //
    // IDL:omg.org/Components/CCMContext/get_rollback_only:1.0
    //
    /**
     * The get_rollback_only operation is used by a component to test
     * if the current transaction has been marked for rollback.
     * When get_rollback_only is issued by a component, it results
     * in a org.omg.CosTransaction.Current.get_status being issued to
     * the CORBA transaction service and the status value returned being tested
     * for the MARKED_ROLLBACK state.
     *
     * <p/><B>NOTA:</B> This is NOT implemented in this version.
     *
     * @return <B>TRUE</B> if the transaction has been marked for rollback,
     * otherwise it returns <B>FALSE</B>.
     */
    public boolean get_rollback_only() {
        throw new org.omg.CORBA.NO_IMPLEMENT(
            ExceptionMinorCodes.NO_IMPLEMENTTransactionsNotSupported,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    }

    //
    // IDL:omg.org/Components/CCMContext/get_user_transaction:1.0
    //
    /**
     * The get_user_transaction operation is used to access the
     * org.omg.Transaction.UserTransaction interface. The UserTransaction
     * interface is used to implement self-managed transactions.
     *
     * <p/><B>NOTA:</B> This is NOT implemented in this version.
     *
     * @return A reference to the UserTransaction object
     */
    public org.omg.Components.Transaction.UserTransaction
        get_user_transaction() {
        throw new org.omg.CORBA.NO_IMPLEMENT(
            ExceptionMinorCodes.NO_IMPLEMENTTransactionsNotSupported,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    }

    //
    // IDL:omg.org/Components/CCMContext/is_caller_in_role:1.0
    //
    /**
     * The is_caller_in_role operation is used by the CORBA component
     * to compare the current credentials to the credentials defined
     * by the role parameter.
     *
     * <p/><B>NOTA:</B> This is NOT implemented in this version.
     *
     * @return <B>TRUE</B> if the current credentials match the credentials
     * defined by the role parameter; <B>FALSE</B> otherwise.
     */
    public boolean is_caller_in_role(String role) {
        throw new org.omg.CORBA.NO_IMPLEMENT(
            ExceptionMinorCodes.NO_IMPLEMENTSecurityNotSupported,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    }

    //
    // IDL:omg.org/Components/CCMContext/set_rollback_only:1.0
    //
    /**
     * The set_rollback_only operation is used by a component to mark
     * an existing transaction for abnormal termination.
     * When set_rollback_only is issued by a component, it results in a
     * org.omg.CosTransaction.Current.rollback_only being issued
     * to the CORBA transaction service.
     *
     * <p/><B>NOTA:</B> This is NOT implemented in this version.
     */
    public void set_rollback_only() {
        throw new org.omg.CORBA.NO_IMPLEMENT(
            ExceptionMinorCodes.NO_IMPLEMENTTransactionsNotSupported,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
    }


    //--- From CCM2Context ----------------------------------------------//
    //
    // IDL:omg.org/Components/CCM2Context/get_home_registration:1.0
    //
    /**
     * The get_home_registration operation is used to obtain a reference
     * to the HomeRegistration interface. The HomeRegistration is used
     * to register component homes so they may be located by the HomeFinder.
     *
     * @return A reference to the HomeRegistration object.
     */
    public org.omg.Components.HomeRegistration get_home_registration() {
        return this.homeRegistration;
    }

    //
    // IDL:omg.org/Components/CCM2Context/req_passivate:1.0
    //
    /**
     * The req_passivate operation is used by the component to inform
     * the container that it wishes to be passivated
     * when its current operation completes. The component must have
     * a servant lifetime policy of component or container.
     *
     * @exception org.omg.Components.PolicyMismatch
     * Raised if the servant lifetime policy is neither component nor container.
     */
    public void req_passivate() throws org.omg.Components.PolicyMismatch {
        if (servantLifetime.value() != ServantLifetimeValue.COMPONENT_LIFETIME.value() &&
            servantLifetime.value() != ServantLifetimeValue.CONTAINER_LIFETIME.value()) {
            throw new org.omg.Components.PolicyMismatch();
        }

        try {
            org.omg.PortableServer.POA currentPoa = this.current.get_POA();
            byte[] oid = this.current.get_object_id();
            System.err.println("---> req_passivate() current '" 
                + currentPoa.the_name() + "'");
            currentPoa.deactivate_object(oid);
        } catch (org.omg.PortableServer.CurrentPackage.NoContext nc) {
            throw new org.omg.Components.PolicyMismatch();
            // TODO: Raise an issue in the components FTF.
            // throw new org.omg.Component.IllegalState();
        } catch (org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
            // should not happen!
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            throw new org.omg.Components.PolicyMismatch();
        } catch (org.omg.CORBA.SystemException se) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    }

    //
    // IDL:omg.org/Components/CCM2Context/get_persistence:1.0
    //
    /**
     * The get_persistence operation provides the component access
     * to a persistence framework provided by an implementation
     * of the CORBA persistence state service.
     *
     * <p/><B>NOTA:</B> This raises an
     * org.omg.Components.PersistenceNotAvailable exception
     *
     * @param catalog_type_id Identifier of the CatalogBase
     *
     * @return A reference to the org.omg.CosPersistentState.CatalogBase
     * which serves as an index to the available storage homes.
     *
     * @exception org.omg.Components.PersistenceNotAvailable
     * Raised if the CatalogBase identified by
     * <I>catalog_type_id</I> is not available on this container.
     */
    public org.omg.CosPersistentState.CatalogBase
        get_persistence(String catalog_type_id)
        throws org.omg.Components.PersistenceNotAvailable {
        throw new org.omg.Components.PersistenceNotAvailable();
    }

    //--- From SessionContext -------------------------------------------//
    //
    // IDL:omg.org/Components/SessionContext/get_CCM_object:1.0
    //
    /**
     * The get_CCM_object operation is used to get the reference used
     * to invoke the component. For basic components, this will always
     * be the component reference. For extended components, this will
     * be a specific facet reference.
     *
     * @return The object reference used to invoke the component.
     *
     * @exception org.omg.Components.IllegalState
     * Raised if this operation is issued outside of
     * the scope of a callback operation.
     */
    public org.omg.CORBA.Object get_CCM_object()
        throws org.omg.Components.IllegalState {
        try {
            org.omg.PortableServer.POA currentPoa = this.current.get_POA();
            byte[] oid = this.current.get_object_id();
            System.err.println("---> get_CCM_object() current '" 
                + currentPoa.the_name() + "'");
            return currentPoa.id_to_reference(oid);
        } catch (org.omg.PortableServer.CurrentPackage.NoContext nc) {
            throw new org.omg.Components.IllegalState();
        } catch (org.omg.PortableServer.POAPackage.ObjectNotActive ona) {
            // should not happen!
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            // TODO/FIXME: Check for NON_RETAIN servant retention policy
            // should not happen!
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CORBA.SystemException se) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    }


    //--- From Session2Context ------------------------------------------//
    //
    // IDL:omg.org/Components/Session2Context/create_ref:1.0
    //
    /**
     * The create_ref operation is used to create a reference to be exported
     * to clients to invoke operations. The RepositoryId must match
     * the RepositoryId of the component itself, one of its bases,
     * one of its supported interfaces, or one of its facets.
     *
     * @param repid Identifies the RepositoryId associated with the interface
     * for which a reference is being created.
     *
     * @return A newly created object reference.
     */
    public org.omg.CORBA.Object create_ref(String repid) {
        byte[] oid = createObjectId();
        System.err.println("---> create_ref() using poa '" 
            + this.poa.the_name() + "'");
	try {
        return this.poa.create_reference_with_id(oid, repid);
        } catch ( Exception wp){
		return null;
	}
    }

    //
    // IDL:omg.org/Components/Session2Context/create_ref_from_oid:1.0
    //
    /**
     * The create_ref_from_oid operation is used to create a reference
     * to be exported to clients which includes information provided
     * by the component which it can use on subsequent operation requests.
     *
     * @param oid Identifies the ObjectIdSeq to be encapsulated
     * in the reference.
     * @param repid Identifies the RepositoryId associated with the interface
     * for which a reference is being created.
     *
     * @return A newly created object reference.
     */
    public org.omg.CORBA.Object create_ref_from_oid(byte[] oid, String repid) 
	{

        System.err.println("---> create_ref_from_oid() using poa '" 
            + this.poa.the_name() + "'");
	try {
		 return this.poa.create_reference_with_id(oid, repid);
       } catch ( Exception wp){
		return null;
	}
//return obj;
    }

    //
    // IDL:omg.org/Components/Session2Context/get_oid_from_ref:1.0
    //
    /**
     * The get_oid_from_ref operation is used by the component
     * to extract the oid encapsulated in the reference. This operation must be
     * called within an operation invocation.
     *
     * @param objref Specifies the reference which contains the oid.
     *
     * @return the oid encapsulated in the reference
     *
     * @exception org.omg.Components.IllegalState
     * Raise if the operation is not called within an operation invocation.
     * @exception org.omg.Components.BadComponentReference
     * Raised with the WRONG_CONTAINER minor code
     * if the reference was not created by this container.
     */
    public byte[] get_oid_from_ref(org.omg.CORBA.Object objref)
        throws org.omg.Components.IllegalState,
                org.omg.Components.BadComponentReference {
        // The get_oid_from_ref operation is used by the component to extract
        // the oid encapsulated in the reference. The objref parameter
        // specifies the reference which contains the oid. This operation must
        // be called within an operation invocation. If not the IllegalState
        // exception shall be raised. If the reference was not created by this
        // container, the BadComponentReference with the WRONG_CONTAINER minor
        // code is raised.
        try {
            org.omg.PortableServer.POA currentPoa = this.current.get_POA();
            System.err.println("---> get_oid_from_ref() using poa '" + 
                this.poa.the_name() + "'");
            System.err.println("---> get_oid_from_ref() current '" + 
                currentPoa.the_name() + "'");
            return this.poa.reference_to_id(objref);
        } catch (org.omg.PortableServer.CurrentPackage.NoContext nc) {
            throw new org.omg.Components.IllegalState();
        } catch (org.omg.PortableServer.POAPackage.WrongAdapter wa) {
            throw new org.omg.Components.BadComponentReference(
                org.omg.Components.BadComponentReferenceReason.WRONG_CONTAINER);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            // should not happen!
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CORBA.SystemException se) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNAL,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    }

    //
    // IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/obtain_event_channel_for_emitter:1.0
    //
    public org.omg.CosEventChannelAdmin.EventChannel
        obtain_event_channel_for_emitter(
            org.omg.Components.PortDescription port)
        throws com.thalesgroup.CdmwCcmContainer.NoChannel {
        if (this.eventChannelFactory != null) {
            try {
                return createEventChannel(
                    this.eventChannelFactory,
                    this.processName,
                    this.applicationName,
                    port);
            } catch (org.omg.Components.CreateFailure cf) {
                throw
                    new com.thalesgroup.CdmwCcmContainer.NoChannel(cf.reason);
            }
        } else {
            throw new com.thalesgroup.CdmwCcmContainer.NoChannel(
                com.thalesgroup.CdmwDeployment.NO_EVENT_CHANNEL_FACTORY.value);
        }
    }

    //
    // IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/create_event_channel_for_publisher:1.0
    //
    public org.omg.CosEventChannelAdmin.EventChannel
        create_event_channel_for_publisher(
            org.omg.Components.PortDescription port)
        throws org.omg.Components.CreateFailure {
        if (this.eventChannelFactory != null) {
            return createEventChannel(
                this.eventChannelFactory,
                this.processName,
                this.applicationName,
                port);
        } else {
            throw new org.omg.Components.CreateFailure(
                com.thalesgroup.CdmwDeployment.NO_EVENT_CHANNEL_FACTORY.value);
        }
    }

    /**
     * Creates an Event Channel object
     * through the CDMW's Event Channel Factory.
     *
     * @param evtFactory The Event Channel Factory.
     * @param processName The name of the process.
     * @param applicationName The name of the application.
     * @param port A PortDescription object
     * @return The newly created EventChannel
     */
    public org.omg.CosEventChannelAdmin.EventChannel createEventChannel(
        com.thalesgroup.CdmwEvent.EventChannelFactory evtFactory,
        String processName,
        String applicationName,
        org.omg.Components.PortDescription port)
        throws org.omg.Components.CreateFailure {

        Assert.check(evtFactory != null);
        Assert.check(port != null);

        try {
            String portId = CCMUtils.createUniqueIdentifier(port.name, 3);

            org.omg.CosNaming.NameComponent[] name =
                new org.omg.CosNaming.NameComponent[1];
            name[0] = new org.omg.CosNaming.NameComponent();
            name[0].id  = portId + "@" + processName + "@" + applicationName;
            name[0].kind = "EventChannel";

            return
                evtFactory.create_event_channel(NamingInterface.toString(name));
        } catch (com.thalesgroup.CdmwLifeCycle.AlreadyExistant ae) {
            throw new org.omg.Components.CreateFailure(
                com.thalesgroup.CdmwDeployment.EVENT_CHANNEL_ALREADY_EXISTS.value);
        } catch (com.thalesgroup.CdmwLifeCycle.InvalidName in) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (com.thalesgroup.CdmwEvent.CreationError ce) {
            throw new org.omg.Components.CreateFailure(
                com.thalesgroup.CdmwDeployment.CREATE_FAILURE.value);
        } catch(InvalidNameException ine) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

    }

    /**
     * Generates an ObjectId for use in create_ref()
     *
     * @return an newly created ObjectId
     */
    private byte[] createObjectId() {
        int counter = CCMUtils.createIdentifier();

        Timeval tv = OS.getTime();
        String str =
            tv.getSeconds() + ":" + tv.getMicroseconds() + ":" + counter;

        return str.getBytes();
    }

    
    
    //
    // IDL:thalesgroup.com/CdmwCcmContainer/CCM2Context/create_event_channel_for_publisher:1.0
    //
    public org.omg.CORBA.Object resolve_initial_references(String identifier)
        throws com.thalesgroup.CdmwCcmContainer.CCM2ContextPackage.InvalidName
    {
        // Deprecated and only used for CCM with FT in C++
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }
    
    
    
}

