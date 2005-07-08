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


package cdmw.orbsupport;

import cdmw.ossupport.Mutex;
import cdmw.common.Assert;
import cdmw.common.AssertionFailedException;

import org.omg.CORBA.ORB;
import org.omg.CORBA.SystemException;
import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.BAD_PARAM;
import org.omg.CORBA.BAD_INV_ORDER;
import org.omg.CORBA.INTERNAL;
import org.omg.CORBA.ORBPackage.InvalidName;

import com.ooc.OB.BootManager;
import com.ooc.OB.BootManagerHelper;
import com.ooc.OB.BootManagerPackage.AlreadyExists;
import com.ooc.OB.BootManagerPackage.NotFound;

import com.ooc.OB.InvalidThreadPool;
import com.ooc.OB.DispatchStrategy;
import com.ooc.OB.DispatchStrategyFactory;
import com.ooc.OB.DispatchStrategyFactoryHelper;

import com.ooc.OBPortableServer.POA;
import com.ooc.OBPortableServer.POAHelper;
import com.ooc.OBPortableServer.POAManager;
import com.ooc.OBPortableServer.POAManagerFactory;
import com.ooc.OBPortableServer.POAManagerFactoryHelper;
import com.ooc.OBPortableServer.POAManagerFactoryPackage.POAManagerAlreadyExists;

/**
 * This class implements the OrbSupportInterface required
 * capability for the OrBacus ORB
 *
 * Features:
 * Thread safe.
 *
 * @see ORBSupportInterface
 *
 */
 class ORBSupportImpl implements ORBSupportInterface {

    /**
     * Name of the CDMW POA Manager
     */
    public final static String CDMW_POA_MANAGER_NAME = "CDMW_POA_MANAGER";

    /**
     * True if the ORBInit method has been already called
     */
    private static boolean initCalled = false;

    /**
     * Mutex
     */
    private static Mutex mutex = new Mutex();

    /**
     * Dispatch Strategy factory
     */
    private static DispatchStrategyFactory dispatchStrategyFactory;

    /**
     * CDMW POA Manager
     */
    private static POAManager cdmwPOAManager;

    /**
     * Add to the PolicyList the Policy required
     * to create a POA with a thread pool
     *
     * @param policies List of policies to be updated
     * to create a POA with thread pool
     * @param obPOA The Orbacus Parent POA
     * @param dispatchStrategyFactory A reference
     * to the Dispatch Strategy factory
     * @param nbThreads The size of the thread pool
     *
     * @exception InvalidThreadPool If the thread pool is invalid.
     */
    private static void addPolicyThreadPool(PolicyList policies, POA obPOA,
        DispatchStrategyFactory dispatchStrategyFactory, int nbThreads)
        throws InvalidThreadPool {

        // We create a thread pool of the right size
        int threadPoolId =
            dispatchStrategyFactory.create_thread_pool(nbThreads);

        // And we create the associated ORBacus strategy
        DispatchStrategy dsThreadPool =
            dispatchStrategyFactory.create_thread_pool_strategy(threadPoolId);

        // We insert our stategy in the list
        policies.insert( obPOA.create_dispatch_strategy_policy(dsThreadPool));

    }

    /**
     * Add to the PolicyList the Policy required
     * to create a POA with a thread per request
     *
     * @param policies List of policies to be updated
     * to create a POA with thread pool
     * @param obPOA The Orbacus Parent POA
     * @param dispatchStrategyFactory A reference to the Dispatch Strategy factory
     */
    private static void addPolicyThreadPerRequest(PolicyList policies,
        POA obPOA, DispatchStrategyFactory dispatchStrategyFactory) {

        // We create the strategy one thread per request
        DispatchStrategy dsThreaded =
            dispatchStrategyFactory.create_thread_per_request_strategy();

        policies.insert(obPOA.create_dispatch_strategy_policy(dsThreaded));

    }

    /**
     * Add to the PolicyList Policies required
     * to create a POA matching the CDMW StrategyList
     *
     * @param policies List of policy to be updated
     * @param obPOA The Orbacus Parent POA
     * @param strategyList CDMW StrategyList specifying the POA behavior
     *
     * @exception BAD_INV_ORDER The ORB_init operation was never called.
     * @exception InvalidThreadPool The thread pool is invalid.
     */
    private static void transformStrategyIntoPolicy(PolicyList policies,
        POA obPOA, StrategyList strategyList)
        throws SystemException, InvalidThreadPool {

        if (!initCalled) {
            int minor = ExceptionMinorCodes.BAD_INV_ORDERInitNotCalled;
            throw new
                BAD_INV_ORDER(ExceptionStrings.BAD_INV_ORDER_reasons[minor],
                minor, CompletionStatus.COMPLETED_NO);
        }

        // WARNING: here we do not check for policy consistency,
        // it must have been done in checkPOAPoliciesMatchORBPolicies

        // Do we use a thread pool for the POA ?
        if ( strategyList.isPOAThreadPool() ) {
            // Number of threads for the thread pool
            int nbThreads = strategyList.getNbPOAThreads();
            addPolicyThreadPool( policies, obPOA,
                                    dispatchStrategyFactory, nbThreads );
        }

        // Do we use one thread per request ?
        if ( strategyList.isPOAThreadPerRequest() ) {
            addPolicyThreadPerRequest( policies, obPOA,
                                    dispatchStrategyFactory );
        }
    }

    /**
     * Initialises the ORB
     *
     * @param args The command line arguments
     * @param strategyList A list of CDMW strategies
     * @param orbIdentifier Optional ORB identifier
     *
     * @return A valid reference to an ORB pseudo-object
     *
     * @exception BAD_PARAM Invalid parameter. The following CDMW minor codes
     * detail the reason:
     * <UL>
     * <LI><B>BAD_PARAMMissingORBThreadingStrategy</B>
     * when the multithread strategy is not set for the ORB, or
     * <LI><B>BAD_PARAMPOAMutltiThreadStrategyRequired</B>
     * when the required POA multithread is not set.
     * </UL>
     * @exception INTERNAL  The CDMW <B>INTERNAL</B> minor code indicates an
     * internal error occured within OrbSupport library.
     *
     * @see ORBSupport#ORBInit
     */
    public ORB ORBInit(String[] args, StrategyList strategyList,
        int cdmwRootPOAPort, int rootPOAPort, String orbIdentifier)
        throws SystemException {

        org.omg.CORBA.ORB theORB = null;

        // At this time is ORB SHALL be multi-threaded
        if ( !strategyList.isORBThreaded() ) {
            int minor =
                ExceptionMinorCodes.BAD_PARAMMissingORBThreadingStrategy;
            throw new BAD_PARAM(ExceptionStrings.BAD_PARAM_reasons[minor],
                                minor, CompletionStatus.COMPLETED_NO);
        }

        // We create the set of properties required by orbacus
        java.util.Properties props = System.getProperties();
        props.put("org.omg.CORBA.ORBClass", "com.ooc.CORBA.ORB");
        props.put("org.omg.CORBA.ORBSingletonClass",
                    "com.ooc.CORBA.ORBSingleton");

        args = com.ooc.CORBA.ORB.ParseArgs(args, props, null);

        // The ORB is multi-threaded
        props.setProperty("ooc.orb.conc_model", "threaded");


        // Threading strategies
        // --------------------------------------------------------------------

        // Do we use a thread pool for the POAs ?
        if ( strategyList.isPOAThreadPool() ) {

            // The number of thread to use
            int nbThreads = strategyList.getNbPOAThreads();

            // Must be checked by the StrategyList
            Assert.check( nbThreads > 0 );

            props.setProperty("ooc.orb.oa.conc_model", "thread_pool");
            props.setProperty("ooc.orb.oa.thread_pool", String.valueOf(nbThreads));

        // Else we use a multi-threaded POA
        } else if ( strategyList.isPOAThreadPerRequest() ) {

            props.setProperty("ooc.orb.oa.conc_model", "thread_per_request");

        } else if ( strategyList.isPOAThreadPerConnection() ) {

            props.setProperty("ooc.orb.oa.conc_model", "thread_per_client");

        // Else the user hasn't set a strategy for the POA
        } else {
            int minor =
                ExceptionMinorCodes.BAD_PARAMPOAMutltiThreadStrategyRequired;
            throw new BAD_PARAM(ExceptionStrings.BAD_PARAM_reasons[minor],
                                minor, CompletionStatus.COMPLETED_NO);
        }

        // IIOP strategies
        // --------------------------------------------------------------------

        // Do we use a specific port for the root POA ?
        if (rootPOAPort != 0) {
            props.setProperty( "ooc.orb.oa.endpoint",
                "iiop --numeric --port " + String.valueOf(rootPOAPort) );
        } else {
            props.setProperty( "ooc.orb.oa.endpoint", "iiop --numeric");
        }

        // Do we use a specific port for the Cdmw root POA ?
        if (cdmwRootPOAPort != 0) {
            String propertyKeyBase = "ooc.orb.poamanager.";
            propertyKeyBase  += CDMW_POA_MANAGER_NAME;

            String propertyKey = propertyKeyBase + ".endpoint";
            props.setProperty( propertyKey, "iiop --numeric --port "
                + String.valueOf(cdmwRootPOAPort) );
        }

        // --------------------------------------------------------------------
        // --------------------------------------------------------------------


        theORB = ORB.init(args, props);
        // FIXME : orbIdentifier is never used (comes from the C++ version)

        // Set Dispatch Strategy Factory
        // Use Double-checked locking
        if (!initCalled) {

            try {
            mutex.lock();

            if (!initCalled) {
                org.omg.CORBA.Object obj = theORB.resolve_initial_references(
                    "DispatchStrategyFactory");
                dispatchStrategyFactory  =
                    DispatchStrategyFactoryHelper.narrow(obj);

                obj = theORB.resolve_initial_references("POAManagerFactory");
                POAManagerFactory factory =
                    POAManagerFactoryHelper.narrow(obj);
                cdmwPOAManager =
                    factory.create_poa_manager(CDMW_POA_MANAGER_NAME);
                
                initCalled = true;
            }

            } catch(InvalidName in) {
                int minor = ExceptionMinorCodes.INTERNAL;
                throw new INTERNAL(ExceptionStrings.INTERNAL_reasons[minor],
                    minor,CompletionStatus.COMPLETED_NO);
            } catch(InterruptedException ie) {
                int minor = ExceptionMinorCodes.INTERNAL;
                throw new INTERNAL(ExceptionStrings.INTERNAL_reasons[minor],
                    minor,CompletionStatus.COMPLETED_NO);
            } catch(POAManagerAlreadyExists poamae) {
                int minor = ExceptionMinorCodes.INTERNAL;
                throw new INTERNAL(ExceptionStrings.INTERNAL_reasons[minor],
                    minor,CompletionStatus.COMPLETED_NO);
            } catch(com.ooc.OCI.InvalidParam ip) {
                int minor = ExceptionMinorCodes.INTERNAL;
                throw new INTERNAL(ExceptionStrings.INTERNAL_reasons[minor],
                    minor,CompletionStatus.COMPLETED_NO);
            }
            finally {
                mutex.unlock();
            }
        }

        return theORB;

    }


    /**
     * Creates a new POA
     *
     * @param parentPOA The parent POA of the POA to be created
     * @param adapterName Name of the new POA
     * @param aPOAManager Manager of the POA
     * @param policies List of policies to use to create the POA
     * @param strategyList List of CDMW Strategies
     *
     * @return A reference to the created POA
     *
     * @exception AdapterAlreadyExists A POA with the same
     * name is already created by parentPOA.
     * @exception InvalidPolicy Supplied policies are not
     * compatible.
     * @exception BAD_PARAM Invalid parameter. The following CDMW minor codes
     * detail the reason:
     * <UL>
     * <LI><B>BAD_PARAM</B> when a strategy transformation failed, or
     * <LI><B>BAD_PARAMNilObjectReference</B> when a null object reference
     * is supplied for as parentPOA, or
     * <LI><B>BAD_PARAMUnknownPOAType</B> when the POA is not created by the
     * underlying ORB.
     * </UL>
     *
     * @see ORBSupport#createPOA
     */
    public org.omg.PortableServer.POA createPOA(
        org.omg.PortableServer.POA parentPOA, String adapterName,
        org.omg.PortableServer.POAManager aPOAManager, PolicyList policies,
        StrategyList strategyList) throws SystemException,
            org.omg.PortableServer.POAPackage.AdapterAlreadyExists,
            org.omg.PortableServer.POAPackage.InvalidPolicy {

        // Check that parentPOA is not a nil object reference
        if ( parentPOA == null ) {
            int minor = ExceptionMinorCodes.BAD_PARAMNilObjectReference;
            throw new BAD_PARAM(ExceptionStrings.BAD_PARAM_reasons[minor],
                minor,CompletionStatus.COMPLETED_NO);
        }

        // We use the proprietary OrbAcus POA
        POA obParentPOA = POAHelper.narrow(parentPOA);

        // The POA may be not an ORBacus POA
        if( obParentPOA == null ) {
            int minor = ExceptionMinorCodes.BAD_PARAMUnknownPOAType;
            throw new BAD_PARAM(ExceptionStrings.BAD_PARAM_reasons[minor],
                minor, CompletionStatus.COMPLETED_NO);
        }

        // We make a copy of the user policies
        PolicyList obPolicies = policies.copy();

        // Each Cdmw Strategy is transform into the ORBacus proprietary Policy
        try {
            transformStrategyIntoPolicy(obPolicies, obParentPOA, strategyList);
        } catch (InvalidThreadPool itp) {
            int minor = ExceptionMinorCodes.BAD_PARAM;
            throw new BAD_PARAM(ExceptionStrings.BAD_PARAM_reasons[minor],
                minor, CompletionStatus.COMPLETED_NO);
        }

        POAManager thePOAManager = (POAManager) aPOAManager;
        // Is it the CDMW POA Manager?
        if ( thePOAManager == null
            && adapterName.equals(ORBSupport.CDMW_ROOT_POA_NAME) ) {
            thePOAManager = cdmwPOAManager;
        }

        // We have inserted our proprietary policies, we create the POA
        return obParentPOA.create_POA( adapterName, thePOAManager,
            obPolicies.getList() );

    }

    /**
     * Attach a CORBA object to a corbaloc reachable reference
     *
     * @param orb The ORB pseudo-object
     * @param corbalocName Name to be assigned to reach the object
     * @param object Object reference to bind to the corbalocName
     *
     * @exception BAD_PARAM Invalid parameter. The CDMW
     * <B>BAD_PARAMCorbalocBindingAlreadyExists</B> minor code indicates that
     * an object reference has already been bound to the specified name.
     * @exception INTERNAL When the Boot Manager can't be initialized
     *
     * @see ORBSupport#bindObjectToCorbaloc
     */
    public void bindObjectToCorbaloc(ORB orb, String corbalocName,
        org.omg.CORBA.Object o) throws SystemException {

        // The Id must be defined by us
        byte[] oid = corbalocName.getBytes();

        try {
            org.omg.CORBA.Object bmgrObj =
                orb.resolve_initial_references("BootManager");

            BootManager bootManager = BootManagerHelper.narrow(bmgrObj);

            // Are we not using a specific version of Orbacus ?
            Assert.check( bootManager != null );

            // We register our object
            bootManager.add_binding(oid, o);
        } catch (AlreadyExists ae) {
            int minor =
                ExceptionMinorCodes.BAD_PARAMCorbalocBindingAlreadyExists;
            throw new BAD_PARAM(ExceptionStrings.BAD_PARAM_reasons[minor],
                minor, CompletionStatus.COMPLETED_NO);
        } catch(InvalidName in) {
            int minor = ExceptionMinorCodes.INTERNAL;
            throw new INTERNAL(ExceptionStrings.INTERNAL_reasons[minor],
                minor,CompletionStatus.COMPLETED_NO);
        }

    }

    /**
     * Remove a corbaloc binding
     *
     * @param orb The ORB pseudo-object
     * @param corbalocName Name of the binding to be removed.
     *
     * @exception BAD_PARAM Invalid parameter. The CDMW
     * <B>BAD_PARAMCorbalocBindingNotFound</B> minor code indicates that
     * there is no object reference bound to the specified name.
     * @exception INTERNAL When the Boot Manager can't be initialized
     *
     * @see ORBSupport#removeCorbalocBinding
     */
    public void removeCorbalocBinding(ORB orb, String corbalocName)
        throws SystemException {

        // The Id must be defined by us
        byte[] oid = corbalocName.getBytes();

        try {
            org.omg.CORBA.Object bmgrObj =
                orb.resolve_initial_references("BootManager");

            BootManager bootManager = BootManagerHelper.narrow(bmgrObj);

            // Are we not using a specific version of Orbacus ?
            Assert.check( bootManager != null );

            // We register our object
            bootManager.remove_binding( oid );
        } catch (NotFound nf) {
            int minor =
                ExceptionMinorCodes.BAD_PARAMCorbalocBindingNotFound;
            throw new BAD_PARAM(ExceptionStrings.BAD_PARAM_reasons[minor],
                minor, CompletionStatus.COMPLETED_NO);
        } catch(InvalidName in) {
            int minor = ExceptionMinorCodes.INTERNAL;
            throw new INTERNAL(ExceptionStrings.INTERNAL_reasons[minor],
                minor,CompletionStatus.COMPLETED_NO);
        }

    }

    /**
     * Register the object reference as specified service
     * to be reachable with "resolveInitialReference" API.
     *
     * @param orb The ORB pseudo-object
     * @param serviceName Name to be assigned as service to reach the object
     * @param object Object reference to associate to service name
     *
     * @see ORBSupport#registerInitialReference
     */
    public void registerInitialReference(ORB orb,String serviceName,
        org.omg.CORBA.Object object) throws SystemException {

        try {
          ((com.ooc.CORBA.ORB) orb).register_initial_reference(serviceName, object);
        } catch(InvalidName in) {
            int minor =
                ExceptionMinorCodes.BAD_PARAMInvalidServiceName;
            throw new BAD_PARAM(ExceptionStrings.BAD_PARAM_reasons[minor],
                minor, CompletionStatus.COMPLETED_NO);
        }

    }

    /**
     * Clean up the orb
     *
     * @param orb The ORB pseudo-object
     * @exception BAD_INV_ORDER 
     */
    public void ORBCleanup(org.omg.CORBA.ORB orb) 
        throws org.omg.CORBA.BAD_INV_ORDER {
        // do nothing
    }

}

