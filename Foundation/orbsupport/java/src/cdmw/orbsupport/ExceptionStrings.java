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

/**
 * Definition of detail messages for CORBA system exceptions raised by
 * CDMW Runtime.
 *
 */
public class ExceptionStrings {

    /**
     * Detailed reasons in case of a BAD_INV_ORDER exception.
     */
    public static final String[] BAD_INV_ORDER_reasons = {
        // --> BAD_INV_ORDER
        "Routine invocation out of order",
        // --> BAD_INV_ORDERInitNotCalled
        "INIT not called",
        // --> BAD_INV_ORDERInitAlreadyDone
        "INIT Already done",
        // --> BAD_INV_ORDERAckNotDone
        "PlatformManagement Acknowledgement has not been performed"
    };

    /**
     * Detailed reasons in case of a BAD_PARAM exception.
     */
    public static final String[] BAD_PARAM_reasons = {
        // --> BAD_PARAM
        "Incorrect/illegal parameter",
        // --> BAD_PARAMNilObjectReference
        "Nil Object Reference",
        // --> BAD_PARAMInvalidApplicationAgentReference
        "Invalid ApplicationAgent's reference",
        // --> BAD_PARAMInvalidRepositoryReference
        "Invalid Repository reference",
        // --> BAD_PARAMApplicationAgentReferenceMissing
        "ApplicationAgent's reference is missing",
        // --> BAD_PARAMRepositoryNotFound
        "CDMW Repository not found",
        // --> BAD_PARAMProcessNameMissing
        "CDMW process name is missing",
        // --> BAD_PARAMUnknownProcessName
        "CDMW process name unknown",
        // --> BAD_PARAMInvalidProcessName
        "CDMW process name not valid",
        // --> BAD_PARAMCannotGetRepositoryReference
        "Cannot get object reference of the CDMW Repository",
        // --> BAD_PARAMNameDomainNotFound
        "Name domain not found",
        // --> BAD_PARAMInvalidDefaultDomainName
        "Invalid default domain name",
        // --> BAD_PARAMLifeCycleFrameworkInit
        "Bad parameter for init of the LifeCycle framework",
        // --> BAD_PARAMLifeCycleFrameworkPOAExists
        "A POA with the same name as the LifeCycle POA already exists",
        // --> BAD_PARAMInvalidServiceName
        "Name of the CDMW service is not valid",
        // --> BAD_PARAMInvalidTraceProducerLocation
        "Location of the TraceProducer object is not valid",
        // --> BAD_PARAMInvalidTraceProducerName
        "Fully qualified name of TraceProducer object is not valid",
        // --> BAD_PARAMMissingORBThreadingStrategy
        "The threading strategy for the ORB is missing",
        // --> BAD_PARAMPOAMutltiThreadStrategyRequired
        "A multi-thread strategy is required",
        // --> BAD_PARAMUnknownPOAType
        "Unexpected POA type - Not tested by CDMW",
        // --> BAD_PARAMCorbalocBindingAlreadyExists
        "Requested Corbaloc binding already exists",
        // --> BAD_PARAMCorbalocBindingNotFound
        "Requested Corbaloc binding is not found",
        // --> BAD_PARAMProcessXMLFileNameMissing
        "CDMW process XML initialisation file name is missing",
        // --> BAD_PARAMInvalidXMLInitData
        "Invalid XML initialisation data",
        // --> BAD_PARAMInvalidPortNumber
        "Invalid value for port number",
        // --> BAD_PARAMCdmwRootPOAExists
        "A POA with the same name as the CDMW Root POA already exists",
        // --> BAD_PARAMInvalidPerformanceAdminLocation
        "Location of the PerformanceAdmin object is not valid",
        // --> BAD_PARAMInvalidPerformanceAdminName
        "Fully qualified name of PerformanceAdmin object is not valid",
        // --> BAD_PARAMInvalidEventChannelName
        "Event channel name is not valid",
        // --> BAD_PARAMInvalidHomeExecutor
        "Home executor type is not valid",
        // --> BAD_PARAMWrongContextType
        "Context is not of the expected type",
        // --> BAD_PARAMUnknownHomeReference
        "Home reference not created by this container",
        // --> BAD_PARAMUnknownComponentReference
        "Component reference not created by this container",
        // --> BAD_PARAMUnknownContainerReference
        "Container reference not created by this component server",
        // --> BAD_PARAMWrongValuetypeInstallerType
        "Valuetype Installer Implementation is not of the expected type",
        // --> BAD_PARAMHomeNotRegistered
        "Component Home not registered within the HomeRegistration object",
        // --> BAD_PARAMProcessCallbackReferenceMissing
        "ProcessCalback object reference is missing",
        // --> BAD_PARAMBadEventType
        "Event sent is not of the expected type",
        // --> BAD_PARAMInvalidExecutorName
        "Executor name is not valid",
        // --> BAD_PARAMInvalidComponentExecutor
        "Component executor is not valid"
    };

    /**
     * Detailed reasons in case of a INTERNAL exception.
     */
    public static final String[] INTERNAL_reasons = {
        // --> INTERNAL
        "Internal failure",
        // --> INTERNALCannotReachRepository
        "Cannot reach the CDMW Repository",
        // --> INTERNALCdmwRepositoryNotExists
        "CDMW Repository object does not exist",
        // --> INTERNALCdmwApplicationAgentNotExists
        "CDMW ApplicationAgent object does not exist",
        // --> INTERNALCdmwRepositoryError
        "CDMW Repository Error",
        // --> INTERNALServantProviderError
        "Failure in User Servant Provider",
        // --> INTERNALLifeCycleFrameworkError
        "Failure in the LifeCycle framework",
        // --> INTERNALInvalidTraceProducerLocation
        "Location of TraceProducer object is not valid",
        // --> INTERNALInvalidTraceCollectorsLocation
        "Location of TraceCollectors is not valid",
        // --> INTERNALORBAdapterInactive
        "POA Manager is in the 'inactive' state",
        // --> INTERNALXMLLibraryError
        "Failure in XML library",
        // --> INTERNALCCMCIFError
        "Failure in CCM CIF",
        // --> INTERNALInvalidExecutor
        "Executor is not as expected"
    };

    /**
     * Detailed reasons in case of a NO_IMPLEMENT exception.
     */
    public static final String[] NO_IMPLEMENT_reasons = {
        // --> NO_IMPLEMENT
        "Not implemented",
        // --> NO_IMPLEMENTNotYetImplemented
        "Not yet implemented",
        // --> NO_IMPLEMENTComponentKindNotSupported
        "Component Kind not supported",
        // --> NO_IMPLEMENTTransactionsNotSupported
        "CORBA Transactions not supported",
        // --> NO_IMPLEMENTSecurityNotSupported
        "CORBA Security not supported"
    };

    /**
     * Detailed reasons in case of a NO_MEMORY exception.
     */
    public static final String[] NO_MEMORY_reasons = {
        // --> NO_MEMORY
        "Run out of memory"
    };

    /**
     * Detailed reasons in case of a NO_PERMISSION exception.
     */
    public static final String[] NO_PERMISSION_reasons = {
        // --> NO_PERMISSION
        "No permission for this operation"
    };

    /**
     * Detailed reasons in case of a NO_RESOURCES exception.
     */
    public static final String[] NO_RESOURCES_reasons = {
        // --> NO_RESOURCES
        "Run out of resources"
    };

    /**
     * Detailed reasons in case of a OBJECT_NOT_EXIST exception.
     */
    public static final String[] OBJECT_NOT_EXIST_reasons = {
        // --> OBJECT_NOT_EXIST
        "Object does not exist",
        // --> OBJECT_NOT_EXISTInvalidExecutor
        "Executor does not exist"
    };

    /**
     * Detailed reasons in case of a TRANSIENT exception.
     */
    public static final String[] TRANSIENT_reasons = {
        // --> TRANSIENT
        "Cannot reach the object now",
        // --> TRANSIENTCannotReachRepository
        "Cannot reach the CDMW Repository now",
        // --> TRANSIENTCannotReachApplicationAgent
        "Cannot reach the CDMW ApplicationAgent now",
        // --> TRANSIENTCannotReachEventServer
        "Cannot reach the server implementing the CORBA Event Service now",
        // --> TRANSIENTCannotReachNameService
        "Cannot reach the server implementing the CORBA Naming Service now"
    };


}

