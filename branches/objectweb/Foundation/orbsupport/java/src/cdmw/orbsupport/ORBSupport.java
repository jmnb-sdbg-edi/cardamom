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

import cdmw.common.Options;
import cdmw.ossupport.OS;

import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.SystemException;
import org.omg.CORBA.BAD_PARAM;

import org.omg.PortableServer.POA;
import org.omg.PortableServer.POAManager;
import org.omg.PortableServer.POAPackage.AdapterAlreadyExists;
import org.omg.PortableServer.POAPackage.InvalidPolicy;
import java.lang.*;

/**
 * This class provides a wrapping of the ORB dependent capability.
 *
 * Features: Thread safe
 *
 */
public class ORBSupport {

    /**
     * Name of the CDMW POA root of all CDMW-specific POAs
     */
    public final static String CDMW_ROOT_POA_NAME = "CDMW_RootPOA";

    /**
     * Minimum port number.
     */
    public final static long MIN_PORT_NUMBER = 0;

    /**
     * Maximum port number.
     */
    public final static long MAX_PORT_NUMBER = 65535;

    /**
     * Implementation of the interface ORBSupportInterface
     */
    private final static ORBSupportInterface orbSupport = new ORBSupportImpl();
    

    /**
     * Creates a new ORB instance, initialised with the
     * specified CDMW StrategyList. This method calls the
     * underlying ORB specific ORBInit, so the semantic is as
     * defined by CORBA.
     *
     * @param args The command line arguments
     * @param strategyList A list of CDMW strategies
     * @param orbIdentifier Optional ORB identifier
     *
     * @return A valid reference to an ORB pseudo-object
     *
     * @exception BAD_PARAM Invalid port number.
     */
    public static org.omg.CORBA.ORB ORBInit(
        String[] args, 
        StrategyList strategyList,
        String orbIdentifier) 
        throws SystemException {
		
	int minor;
        String rootPOAPortOption = OS.getOptionValue(args,
        Options.LOCALISATION_SERVICE_OPTION);
        int rootPOAPortNumber = 0;
        if ( !rootPOAPortOption.equals("no") ) {
            if ( !rootPOAPortOption.equals("yes") ) {
                try {
                    int port = Integer.valueOf(rootPOAPortOption).intValue();
                    if ( port > MIN_PORT_NUMBER && port < MAX_PORT_NUMBER ) {
                        rootPOAPortNumber = port;
                    } else {
                        minor = ExceptionMinorCodes.BAD_PARAMInvalidPortNumber;
                        throw new BAD_PARAM(
                            ExceptionStrings.BAD_PARAM_reasons[minor],
                            minor, CompletionStatus.COMPLETED_NO);
                    }
                }
                catch(NumberFormatException nfe) {
                    minor = ExceptionMinorCodes.BAD_PARAMInvalidPortNumber;
                    throw new BAD_PARAM(
                        ExceptionStrings.BAD_PARAM_reasons[minor],
                        minor, CompletionStatus.COMPLETED_NO);
                }
            } else {
                minor = ExceptionMinorCodes.BAD_PARAMInvalidPortNumber;
                throw new BAD_PARAM(
                    ExceptionStrings.BAD_PARAM_reasons[minor],
                    minor, CompletionStatus.COMPLETED_NO);
            }
	 
        } // else ignore

        String cdmwRootPOAPortOption = OS.getOptionValue(args,
            Options.PROCESS_PORT_OPTION);
        int cdmwRootPOAPortNumber = 0;
        if ( !cdmwRootPOAPortOption.equals("no") ) {
            minor = ExceptionMinorCodes.BAD_PARAMInvalidPortNumber;
            if ( !cdmwRootPOAPortOption.equals("yes") ) {
                try {
                    int port =
                        Integer.valueOf(cdmwRootPOAPortOption).intValue();
                    if ( port > MIN_PORT_NUMBER && port < MAX_PORT_NUMBER ) {
                        cdmwRootPOAPortNumber = port;
                    } else {
                        minor = ExceptionMinorCodes.BAD_PARAMInvalidPortNumber;
                        throw new BAD_PARAM(
                            ExceptionStrings.BAD_PARAM_reasons[minor],
                            minor, CompletionStatus.COMPLETED_NO);
                    }
                }
                catch(NumberFormatException nfe) {
                    minor = ExceptionMinorCodes.BAD_PARAMInvalidPortNumber;
                    throw new BAD_PARAM(
                        ExceptionStrings.BAD_PARAM_reasons[minor],
                        minor, CompletionStatus.COMPLETED_NO);
                }
            } else {
                minor = ExceptionMinorCodes.BAD_PARAMInvalidPortNumber;
                throw new BAD_PARAM(
                    ExceptionStrings.BAD_PARAM_reasons[minor],
                    minor, CompletionStatus.COMPLETED_NO);
            }
        } // else ignore


        return ORBInit(args, strategyList, cdmwRootPOAPortNumber,
                       rootPOAPortNumber, orbIdentifier);

    }
    
    /**
     * Calls the underlying ORB specific ORBInit, so the semantic is as
     * defined by CORBA.
     *
     * @param args The command line arguments
     * @param strategyList A list of CDMW strategies
     *
     * @return A valid reference to an ORB pseudo-object
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     */
    public static org.omg.CORBA.ORB ORBInit(
        String[] args, StrategyList strategyList) throws SystemException {
        return ORBInit(args, strategyList, "");
    }
    
    /**
     * Calls the underlying ORB specific ORBInit, so the semantic is as
     * defined by CORBA.
     *
     * @param args The command line arguments
     * @param strategyList A list of CDMW strategies
     * @param cdmwRootPOAPort specific port for the Cdmw root POA
     * @param rootPOAPort specific port for the root POA
     * @param orbIdentifier Optional ORB identifier
     *
     * @return A valid reference to an ORB pseudo-object
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     */
    public static org.omg.CORBA.ORB ORBInit(
        String[] args,
        StrategyList strategyList,
        int cdmwRootPOAPort,
        int rootPOAPort,
        String orbIdentifier)
        throws org.omg.CORBA.SystemException {
    
        return orbSupport.ORBInit(
            args, 
            strategyList, 
            cdmwRootPOAPort, 
            rootPOAPort,
            orbIdentifier);
            
    }

    /**
     * Creates a POA child of the specified parentPOA. The
     *  created POA is initialised with the specified policies
     *  and with others Policy, created according to the
     *  specified CDMW StrategyList.
     *
     * @param parentPOA The parent POA of the POA to be created
     * @param adapterName Name of the new POA
     * @param aPOAManager Manager of the POA
     * @param policies List of policies to use to create the POA
     * @param strategyList List of CDMW Strategies
     *
     * @return A reference to the created POA
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     * @exception AdapterAlreadyExists A POA with the same name
     * is already created by parentPOA.
     * @exception InvalidPolicy Supplied policies are not compatible.
     *
     * @see StrategyList
     */
    public static POA createPOA(POA parentPOA,
        String adapterName, POAManager aPOAManager,
        PolicyList policies,StrategyList strategyList)
        throws SystemException, AdapterAlreadyExists, InvalidPolicy {

        return orbSupport.createPOA(parentPOA,adapterName,aPOAManager,
                                    policies,strategyList);

    }
    
    /**
     * Creates a POA child of the specified parentPOA. The
     *  created POA is initialised with the specified policies
     *  and with others Policy, created according to the
     *  specified CDMW StrategyList.
     *
     * @param parentPOA The parent POA of the POA to be created
     * @param adapterName Name of the new POA
     * @param aPOAManager Manager of the POA
     * @param policies List of policies to use to create the POA
     * @param strategyList List of CDMW Strategies
     *
     * @return A reference to the created POA
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     * @exception AdapterAlreadyExists A POA with the same name
     * is already created by parentPOA.
     * @exception InvalidPolicy Supplied policies are not compatible.
     *
     * @see StrategyList
     */
    public static POA createPOA(POA parentPOA,
        String adapterName, POAManager aPOAManager,
        org.omg.CORBA.Policy[] policies,StrategyList strategyList)
        throws SystemException, AdapterAlreadyExists, InvalidPolicy {

        return orbSupport.createPOA(parentPOA,adapterName,aPOAManager,
                                    new PolicyList(policies),strategyList);

    }

    /**
     * Binds the specified object reference
     * to a name reachable by corbaloc URLs.
     *
     * @param orb The ORB pseudo-object
     * @param corbalocName Name to be assigned to reach the object
     * @param o Object reference to bind to the corbalocName
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     */
    public static void bindObjectToCorbaloc(
        org.omg.CORBA.ORB orb, 
        String corbalocName,
        org.omg.CORBA.Object o) 
        throws SystemException {
        orbSupport.bindObjectToCorbaloc(orb,corbalocName,o);
    }

    /**
     * Removes a corbaloc binding.
     *
     * @param orb The ORB pseudo-object
     * @param corbalocName Name of the binding to be removed.
     *
     * @exception SystemException Any CORBA system exception that
     * may be raised by the underlying ORB.
     */
    public static void removeCorbalocBinding(
        org.omg.CORBA.ORB orb, 
        String corbalocName)
        throws SystemException {
        orbSupport.removeCorbalocBinding(orb,corbalocName);
    }
    
    /**
     * Register the object reference as specified service 
     * to be reachable with "resolveInitialReference" API.
     * 
     * @param orb The ORB pseudo-object
     * @param serviceName Name to be assigned as service to reach the object
     * @param object Object reference to associate to service name
     *
     * @exception BAD_PARAM Invalid parameter. The CDMW
     * <B>BAD_PARAMCorbalocBindingAlreadyExists</B> minor code indicates that
     * an object reference has already been bound to the specified name.
     */ 
    public static void registerInitialReference(
        org.omg.CORBA.ORB orb,
        String serviceName, 
        org.omg.CORBA.Object object) 
        throws SystemException {
        orbSupport.registerInitialReference(orb,serviceName,object);
    }
    
    /**
     * Translates an Exception into a string in
     * a human readable format.
     * 
     * @param e The CORBA exception in question
     * @return A string in a human readable format
     */ 
    public static String exceptionToString(Exception e) {
        
        if (e instanceof org.omg.CORBA.SystemException) {
            return systemExceptionToString((org.omg.CORBA.SystemException) e);
        } else {
            return e.toString();
        }
        
    }

    /**
     * Translates an org.omg.CORBA.SystemException into a string 
     * in a human readable format.
     * 
     * @param ex The CORBA system exception in question
     * @return A string in a human readable format
     */ 
    private static String 
        systemExceptionToString(org.omg.CORBA.SystemException se) {
        
        if ((se.minor & ExceptionMinorCodes.VMCID_MASK) 
            == ExceptionMinorCodes.CDMW_VMCID) {
            
            int minor = se.minor & ExceptionMinorCodes.MINOR_CODE_MASK;

            // This is a CDMW system exception.
            if (se instanceof org.omg.CORBA.BAD_INV_ORDER) {
                return ExceptionStrings.BAD_INV_ORDER_reasons[minor];
            } else if(se instanceof org.omg.CORBA.BAD_PARAM) {
                return ExceptionStrings.BAD_INV_ORDER_reasons[minor];
            } else if(se instanceof org.omg.CORBA.INTERNAL) {
                return ExceptionStrings.BAD_PARAM_reasons[minor];
            } else if(se instanceof org.omg.CORBA.NO_IMPLEMENT) {
                return ExceptionStrings.NO_IMPLEMENT_reasons[minor];
            } else if(se instanceof org.omg.CORBA.NO_MEMORY) {
                return ExceptionStrings.NO_MEMORY_reasons[minor];
            } else if(se instanceof org.omg.CORBA.NO_PERMISSION) {
                return ExceptionStrings.NO_PERMISSION_reasons[minor];
            } else if(se instanceof org.omg.CORBA.NO_RESOURCES) {
                return ExceptionStrings.NO_RESOURCES_reasons[minor];
            } else if(se instanceof org.omg.CORBA.OBJECT_NOT_EXIST) {
                return ExceptionStrings.OBJECT_NOT_EXIST_reasons[minor];
            } else if(se instanceof org.omg.CORBA.TRANSIENT) {
                return ExceptionStrings.TRANSIENT_reasons[minor];
            } else {
                return "";
            }

        } else {
            return se.toString();
        }
        
    }
    
    /**
     * Clean up the orb
     *
     * @param orb The ORB pseudo-object
     * @exception BAD_INV_ORDER 
     */
    public static void ORBCleanup(org.omg.CORBA.ORB orb) 
        throws org.omg.CORBA.BAD_INV_ORDER {
        orbSupport.ORBCleanup(orb);
    }

}

