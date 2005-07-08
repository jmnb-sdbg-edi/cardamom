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


package cdmw.cdmwinit;

import cdmw.common.Assert;
import cdmw.common.Options;
import cdmw.common.Locations;
import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

import cdmw.ossupport.OS;
import cdmw.orbsupport.CORBASystemExceptionWrapper;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.PolicyList;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.lifecycle.ThreadPoolPolicy;
import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import cdmw.platformmngt.ServiceNames;
import cdmw.platformmngt.PlatformInterface;

import com.thalesgroup.CdmwPlatformMngt.ProcessCallback;
import com.thalesgroup.CdmwPlatformMngt.ProcessCallbackHelper;
import com.thalesgroup.CdmwNamingAndRepository.Repository;
import com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper;

import org.omg.CORBA.Policy;
import org.omg.CORBA.CompletionStatus;

import org.omg.PortableServer.POA;
import org.omg.PortableServer.Servant;


/**
 * This class provides helper functions to be used by the CDMW generated
 * CdmwInterface file.
 *
 * @see CdmwInterface
 *
 */
public class InitUtils {

    /**
     * Name of the POA root of CDMW LifeCycle framework
     */
    public final static String FACTORIES_POA_NAME = "FactoriesPOA";
    
    /**
     * Default Thread pool size of the CDMW factories POA (above)
     */
    public final static int FACTORIES_POA_THREAD_POOL_SIZE = 5;

    /**
     * Default Thread pool size of the CDMW root POA
     */
    public final static int CDMW_ROOT_POA_THREAD_POOL_SIZE = 5;


    /**
     * CDMW Factories Servant Activator
     */
    static class CDMWInitServantActivator 
        extends org.omg.CORBA.LocalObject
        implements org.omg.PortableServer.ServantActivator {

        /**
         * This method should not be called.
         *
         * @param oid
         * @param poa
         */
        public Servant incarnate(byte[] oid, POA poa)
            throws org.omg.PortableServer.ForwardRequest {
                // Should not happen (at this stage!)
                Assert.check(false);
                return null; // TODO: Maybe raise org.omg.CORBA.TRANSIENT()
        }

        /**
         * The main purpose of this operation is to provide a hook
         * that will be called by the ORB during its shutdown phase
         * to etherealize CDMW factories objects.
         *
         * @param oid
         * @param poa
         * @param servant
         * @param cleanup_in_progress
         * @param remaining_activations
         */
        public void
        etherealize(byte[] oid,
                    POA poa,
                    Servant servant,
                    boolean cleanup_in_progress,
                    boolean remaining_activations) {
            // nothing to do in Java
        }
    }

    /**
     * desc
     * 
     * @param orb  A reference to the ORB pseudo-object
     * @param args Arguments passed to the program
     * @param proc The process object the platform management has to manage
     * @exception CORBASystemExceptionWrapper
     */ 
    public static void initPlatformInterface(org.omg.CORBA.ORB orb, 
        String[] args, com.thalesgroup.CdmwPlatformMngt.Process proc) 
        throws CORBASystemExceptionWrapper {
        
        try {
            PlatformInterface.setup(orb, args);
        
            // TODO: Add appropriate minor code
            if (proc == null) {
                throw new CORBASystemExceptionWrapper(
                    new org.omg.CORBA.BAD_PARAM(
                        ExceptionMinorCodes.BAD_PARAM, 
                        CompletionStatus.COMPLETED_NO),
                    "nil Process object reference");
            }
        
            // Acknowledge the creation of the process
            PlatformInterface.acknowledgeCreation(proc);
        } catch (BadOrderException boe) {
            // TODO: Add appropriate minor code
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_INV_ORDER(
                    ExceptionMinorCodes.BAD_INV_ORDER, 
                    CompletionStatus.COMPLETED_NO),
                boe.what());
            //        CompletionStatus.COMPLETED_NO);
        } catch (BadParameterException bpe) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAM, 
                    CompletionStatus.COMPLETED_NO),
                bpe.what());
            // TODO: Add appropriate minor code
        } catch(org.omg.CORBA.SystemException se) {
            throw new CORBASystemExceptionWrapper(se, se.getMessage());
        }
        
    }
    
    /**
     * Retrieves the CDMW ProcessCallback's object reference from
     * Program command line arguments.
     *
     * @param orb  A reference to the ORB pseudo-object
     * @param args Arguments passed to the program
     * @return A reference to the CDMW ProcessCallback
     * @exception CORBASystemExceptionWrapper A wrapped CORBA system exception
     * The following CDMW minor codes detail the reason:
     * <UL><LI><B>BAD_PARAM</B> when the Process Callback's reference 
     * is missing from the command line arguments; and
     * <LI><B>BAD_PARAM</B> when the input IOR is nil 
     * or does not denote an Process Callback.</UL>
     */
    public static ProcessCallback
        getCdmwProcessCallback(org.omg.CORBA.ORB orb, String[] args)
        throws CORBASystemExceptionWrapper {

        // Get Reference of the Process Callback
        String agentIor
            = OS.getOptionValue(args,Options.CALLBACK_IOR_OPTION);
        ProcessCallback processCallback = null;

        if ( !agentIor.equals("no") && !agentIor.equals("yes") ) {
            try {
                org.omg.CORBA.Object obj = orb.string_to_object(agentIor);
                processCallback = ProcessCallbackHelper.narrow(obj);
            } catch (Exception e) {
                // Cannot proceed with no agent Ior
                throw new CORBASystemExceptionWrapper(
                    new org.omg.CORBA.BAD_PARAM(
                        // TODO: minor code to be defined
                        ExceptionMinorCodes.BAD_PARAM,
                        CompletionStatus.COMPLETED_NO),
                    e.getMessage());
            }
        } else {
            // Cannot proceed with no processCallback IOR
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    // TODO: minor code to be defined
                    ExceptionMinorCodes.BAD_PARAM,
                    CompletionStatus.COMPLETED_NO),
                "Cannot proceed with no processCallback IOR");
        }
        if (processCallback==null) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    // TODO: minor code to be defined
                    ExceptionMinorCodes.BAD_PARAM,
                    CompletionStatus.COMPLETED_NO),
                "nil processCallback object reference");
        }
        return processCallback;

    }
    
    /**
     * Returns the reference of the RootPOA pseudo object.
     *
     * @param orb A reference to the ORB pseudo-object
     * @return A reference to the RootPOA pseudo object
     * @exception CORBASystemExceptionWrapper Any wrapped CORBA system exception
     */
    public static POA getRootPOA(org.omg.CORBA.ORB orb) 
        throws CORBASystemExceptionWrapper {

        Assert.check(orb!=null);
        POA root = null;

        try {
            // Resolve the Root POA
            org.omg.CORBA.Object obj
                = orb.resolve_initial_references("RootPOA");
            root = org.omg.PortableServer.POAHelper.narrow(obj);
        } catch (org.omg.CORBA.ORBPackage.InvalidName in) {
            // Raised by resolve_initial_references("RootPOA")
            // shouldn't happen
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL,
                    CompletionStatus.COMPLETED_NO),
                in.getMessage());
        } catch(org.omg.CORBA.SystemException se) {
            throw new CORBASystemExceptionWrapper(se, se.getMessage());
        }

        return root;

    }

    /**
     * Returns the reference of the CDMW upper most POA, parent of all
     * CDMW-specific POAs.
     *
     * @param orb  A reference to the ORB pseudo-object
     * @return A reference to the CDMW RootPOA pseudo object
     * @exception CORBASystemExceptionWrapper Any wrapped CORBA system exception
     */
    public static POA createCdmwRootPOA(org.omg.CORBA.ORB orb) 
        throws CORBASystemExceptionWrapper {

        //
        // Create the CDMW root POA with its own POA manager.
        // Use Default POA policies:
        //
        // Lifespan policy            = TRANSIENT
        // Id Assignment policy       = SYSTEM_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY
        // Servant Retention policy   = RETAIN;
        // Thread Policy              = ORB_CTRL_MODEL;
        //

        // Resolve the Root POA. May raise a CORBA::SystemException.
        POA root = getRootPOA(orb);

        PolicyList policies = new PolicyList();
        policies.insert(root.create_id_assignment_policy(
            org.omg.PortableServer.IdAssignmentPolicyValue.SYSTEM_ID));
        policies.insert(root.create_lifespan_policy(
            org.omg.PortableServer.LifespanPolicyValue.TRANSIENT));
        policies.insert(root.create_servant_retention_policy(
            org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN));
        policies.insert(root.create_id_uniqueness_policy(
            org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID));
        policies.insert(root.create_request_processing_policy(
            org.omg.PortableServer.RequestProcessingPolicyValue.
                USE_ACTIVE_OBJECT_MAP_ONLY));
        policies.insert(root.create_implicit_activation_policy(
            org.omg.PortableServer.ImplicitActivationPolicyValue.
                IMPLICIT_ACTIVATION));
        policies.insert(root.create_thread_policy(
            org.omg.PortableServer.ThreadPolicyValue.ORB_CTRL_MODEL));

        StrategyList poaStrategies = new StrategyList();
        ThreadPoolPolicy threading
            = new ThreadPoolPolicy(CDMW_ROOT_POA_THREAD_POOL_SIZE);
        try {
            threading.appendToStrategyList(poaStrategies);
        } catch(BadParameterException bpe) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAM,
                    CompletionStatus.COMPLETED_NO),
                bpe.what());
        }

        POA cdmwRootPOA = null;
        try {
            cdmwRootPOA = ORBSupport.createPOA(root, 
                ORBSupport.CDMW_ROOT_POA_NAME, null, policies, poaStrategies);
        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMCdmwRootPOAExists,
                    CompletionStatus.COMPLETED_NO),
                aae.getMessage());
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL,
                    CompletionStatus.COMPLETED_NO),
                ip.getMessage());
        } catch(org.omg.CORBA.SystemException se) {
            throw new CORBASystemExceptionWrapper(se, se.getMessage());
        }

        return cdmwRootPOA;

    }

    /**
     * Gets the CDMW Naming and Repository's object reference from
     * the CDMW Application Agent.
     *
     * @return A reference to the CDMW Naming and Repository
     * @exception CORBASystemExceptionWrapper
     * The following CDMW minor codes details tha reason:
     * <UL><LI><B>BAD_PARAMRepositoryNotFound</B> when there is 
     * no CDMW Naming and Repository registered for the current process 
     * within the Process Callback;
     * <LI><B>BAD_PARAMUnknownProcessName</B> when the process name
     * is not known by the Process Callback;
     * <LI><B>BAD_PARAMCannotGetRepositoryReference</B> when an error occurred 
     * during retrieval of the object reference from the Process Callback; and
     * <LI><B>BAD_PARAMInvalidRepositoryReference</B> when the object reference
     * received from the Application Agent is either nil or does not denote 
     * an CDMW Naming and Repository service.</UL>
     */
    public static Repository getCdmwRepository() 
        throws CORBASystemExceptionWrapper {

        // Retreive Repository object reference from the application agent
        Repository rep = null;
        try {
            // May throw ServiceNotFound, ProcessNotFound
            org.omg.CORBA.Object obj = PlatformInterface.getService(
                ServiceNames.NAMING_AND_REPOSITORY_SERVICE);
            rep = RepositoryHelper.narrow(obj);
        } catch(BadOrderException boe) {
            // TODO: minor code to be defined
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_INV_ORDER(
                    ExceptionMinorCodes.BAD_INV_ORDER,
                    CompletionStatus.COMPLETED_NO),
                boe.what());
        } catch(BadParameterException bpe) {
            // TODO: minor code to be defined
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAM,
                    CompletionStatus.COMPLETED_NO),
                bpe.what());
        } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceNotFound snf) {
            // Cannot Proceed with no CDMW Repository Ior
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMRepositoryNotFound,
                    CompletionStatus.COMPLETED_NO),
                snf.getMessage());
        } catch (com.thalesgroup.CdmwPlatformMngtService.ServiceUnavailable su) {
            // Cannot Proceed with no CDMW Repository Ior
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMRepositoryNotFound,
                    CompletionStatus.COMPLETED_NO),
                su.getMessage());
        } catch (org.omg.CORBA.SystemException se) {
            // Cannot Proceed with no CDMW Repository Ior
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMCannotGetRepositoryReference,
                    CompletionStatus.COMPLETED_NO),
                "Received: " + se.toString());
        }
        if (rep==null) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidRepositoryReference,
                    CompletionStatus.COMPLETED_NO),
                "nil repository object reference");
        }
        return rep;

    }

    /**
     * Retrieves the CDMW Process name from Program command line arguments.
     *
     * @param args The arguments passed to the program
     * @return The name of current CDMW Process
     * @exception CORBASystemExceptionWrapper
     */
    public static String getCdmwProcessName()
        throws CORBASystemExceptionWrapper {

        try {
            // Get the Process Name
            return PlatformInterface.getProcessName();
        } catch (BadOrderException boe) {
            // Cannot Proceed with no process name
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMProcessNameMissing,
                    CompletionStatus.COMPLETED_NO),
                boe.what());
        }

    }
    
    /**
     * Retrieves the CDMW Appcalition name from the PlatformInterface.
     *
     * @return The name of current CDMW Application
     * @exception CORBASystemExceptionWrapper
     */ 
    public static String getCdmwApplicationName()
        throws CORBASystemExceptionWrapper {

        try {
            // Get the Application Name
            return PlatformInterface.getApplicationName();
        } catch (BadOrderException boe) {
            // Cannot Proceed with no application name
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAM,
                    CompletionStatus.COMPLETED_NO),
                boe.what());
        }

    }


    /**
     * Retrieves the CDMW Process XML initialisation file name
     * from Program command line arguments.
     *
     * @param args The arguments passed to the program
     * @return The name of current CDMW Process XML initialisation file name
     * @exception CORBASystemExceptionWrapper Invalid parameter.
     * The following CDMW minor codes details tha reason:
     * <B>BAD_PARAMProcessXMLFileNameMissing</B> when the CDMW Process XML 
     * initialisation file name is missing from the command line arguments
     */
    public static String getXmlInitialisationFile(String[] args)
        throws CORBASystemExceptionWrapper {

        // Get the XML file Name
        String filename = OS.getOptionValue(args,
            Options.PROCESS_XML_FILE_OPTION);

        if ( filename.equals("no") || filename.equals("yes") ) {
            // Cannot proceed with no file name
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMProcessXMLFileNameMissing,
                    CompletionStatus.COMPLETED_NO),
                "Cannot proceed with no file name");
        }

        return filename;

    }

    /**
     * Retrieves the CDMW Process XML initialisation file validation
     * option. It instructs the XML parser whether to validate
     * the XML document or not.
     * This will return <B>true</B> when XML document is validated whenever
     * VALIDATE_XML_OPTION appears in the argument list.
     *
     * @param args The arguments passed to the program
     * @return <B>true</B> if the XML document has to be validated
     * by the XML Parser, <B>false</B> otherwise.
     */
    public static boolean getXmlValidationOption(String[] args) {

        // XML document is validated whenever VALIDATE_XML_OPTION appears 
        // in the argument list.
        boolean validate = false;

        // Get the XML file Name
        String validateXmlOption =
            OS.getOptionValue( args, Options.VALIDATE_XML_OPTION);

        if ( validateXmlOption.equals("yes")
            || !validateXmlOption.equals("no") ) {
            validate = true;
        }

        return validate;

    }



    /**
     * Creates the CDMW LifeCycleFramework POA.
     *
     * @param parent  A reference to the Parent POA
     * @return A reference to the LifeCycleFramework Portable Object Adapter
     * @exception CORBASystemExceptionWrapper
     * @exception CORBA::BAD_PARAM Invalid parameter.
     * The following CDMW minor codes details the reason:
     * <UL><LI><B>BAD_PARAMLifeCycleFrameworkPOAExists</B> indicates that a POA
     * with the same name as the LifeCycle POA already exists.
     * <LI><B>INTERNALLifeCycleFrameworkError</B> indicates that the LifeCycle 
     * framework has choosen a wrong policy for its POA.
     * </UL>
     */
    public static POA createCdmwLifeCyclePOA(POA parent)
        throws CORBASystemExceptionWrapper {

        Assert.check(parent!=null);

        POA factoriesPOA = null;

        // Create CDMW Factories POA (persistent POA)
        try {

            org.omg.PortableServer.POAManager poaManager 
                = parent.the_POAManager();

            PolicyList policies = new PolicyList();
            policies.insert(parent.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID));
            policies.insert(parent.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.PERSISTENT));
            policies.insert(parent.create_servant_retention_policy(
                org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN));
            policies.insert(parent.create_id_uniqueness_policy(
                org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID));
            policies.insert(parent.create_request_processing_policy(
                org.omg.PortableServer.RequestProcessingPolicyValue.
                    USE_SERVANT_MANAGER));
            policies.insert(parent.create_implicit_activation_policy(
                org.omg.PortableServer.ImplicitActivationPolicyValue.
                    NO_IMPLICIT_ACTIVATION));

            StrategyList poaStrategies = new StrategyList();
            ThreadPoolPolicy threading
                = new ThreadPoolPolicy(FACTORIES_POA_THREAD_POOL_SIZE);
            try {
                threading.appendToStrategyList(poaStrategies);
            } catch(BadParameterException bpe) {
                throw new CORBASystemExceptionWrapper(
                    new org.omg.CORBA.BAD_PARAM(
                        ExceptionMinorCodes.BAD_PARAM,
                        CompletionStatus.COMPLETED_NO),
                    bpe.what());
            }

            factoriesPOA = ORBSupport.createPOA(
                parent,FACTORIES_POA_NAME,poaManager,policies,poaStrategies);
            //
            // Create and install servant activator
            //
            /*org.omg.PortableServer.ServantActivator theActivator 
                = (org.omg.PortableServer.ServantActivator) new CDMWInitServantActivator();
                */
            factoriesPOA.set_servant_manager(new CDMWInitServantActivator());

        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            // Raised by create_POA
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMLifeCycleFrameworkPOAExists,
                    CompletionStatus.COMPLETED_NO),
                aae.getMessage());
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            // Raised by create_POA
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    CompletionStatus.COMPLETED_NO),
                ip.getMessage());
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy wp) {
            // Raised by set_servant_manager
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                    CompletionStatus.COMPLETED_NO),
                wp.getMessage());
        } catch(org.omg.CORBA.SystemException se) {
            throw new CORBASystemExceptionWrapper(se, se.getMessage());
        }

        return factoriesPOA;

    }

}

