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


#include "orbsupport/ExceptionStrings.hpp"


namespace Cdmw
{

    namespace OrbSupport
    {

        const char* ExceptionStrings::BAD_INV_ORDER_reasons[] =
            {
                // --> BAD_INV_ORDER
                "Routine invocation out of order",
                // --> BAD_INV_ORDERInitNotCalled
                "INIT not called",
                // --> BAD_INV_ORDERInitAlreadyDone
                "INIT Already done",
                // --> BAD_INV_ORDERAckNotDone
                "PlatformManagement Acknowledgement has not been performed",
                // --> BAD_INV_ORDERIteratorPresent
                "Iterator ready exists for the supplied location",
                // --> BAD_INV_ORDERLocationAlreadyPresent
                "The supplied location is already present",
                // --> BAD_INV_ORDERIteratorDestroyed
                "Operation invoked on a destroyed iterator",
                // --> BAD_INV_ORDERComponentNotConfigured
                "CCMComponent is not completly configured",
                // --> BAD_INV_ORDERNullLocationPtr
                "Attempt to associate an Object to a NULL Location",
                // --> BAD_INV_ORDERNullObjectGroupPtr
                "Attempt to associate an Object to a NULL ObjectGroup",
                // --> BAD_INV_ORDERObjectGroupNotEmpty
                "The ObjectGroup is not empty",
            };

        const char* ExceptionStrings::BAD_PARAM_reasons[] =
            {
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
                "Component executor is not valid",
                // --> BAD_PARAMInvalidLocation
                "Location is not valid (length 0)",
                // --> BAD_PARAMInvalidConfigValues
                "ConfigValues are not valid",
                // --> BAD_PARAMIncompatibleMonitoringInterval
                "Incompatible monitoring interval",
                // --> BAD_PARAMIncompatibleMonitoringTimeout
                "Incompatible monitoring timeout"
            };

        const char* ExceptionStrings::INTERNAL_reasons[] =
            {
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
                "Executor is not as expected",
                // --> INTERNALFaultToleranceError
                "CDMW Fault Tolerance Error",
                // --> NoCodecFactory 
                "No Codec Factory",
                // --> INTERNALLocationAlreadyPresent
                "Iterator ready exists for the supplied location",
                // --> INTERNALDataStoreNotFound
                "DataStore Id not found",
                // --> INTERNALInvalidChangeSequence
                "Sequence of state changes is not valid",
                // --> INTERNALAlreadyActivated
                "Already activated",
                // --> INTERNALLocationCtxtNotFound
                "Could not find the Location context",
                // --> INTERNALObjGroupCtxtNotFound
                "Could not find the ObjectGroup context",
                // --> INTERNALSMUndefinedState
                "Undefined FSM state",
                // --> INTERNALSMUndefinedTransition
                "Undefined FSM transition",
                // --> INTERNALSMIndexOutOfBounds
                "FSM index out of bounds",
                // --> INTERNALSMUnexpectedException
                "FSM Unexpected Exception",
                // --> INTERNALLocationInfoNotFound
                "LocationInfo not found",
                // --> INTERNALObjectGroupDataNotFound
                "ObjectGroupData not found",
                // --> INTERNALIncompleteLocationInfo
                "The LocationInfo is not fully initialized",
                // --> INTERNALUnexpectedLocInfoState
                "The LocationInfo has not the expected stateness",
                // --> INTERNALNullObjGroupCtxtPtr
                "The ObjectGroupCtxt has been found but is a NULL pointer",
                // --> INTERNALLoadBalancingError
                "CDMW Load Balancing Error",
                // --> INTERNALComponentInvalidName
                "Component Invalid Name",
                // --> INTERNALNoChannel
                "No Channel created",
                // --> INTERNALNoContext
                "No Context",
            };


        const char* ExceptionStrings::NO_IMPLEMENT_reasons[] =
            {
                // --> NO_IMPLEMENT
                "Not implemented",
                // --> NO_IMPLEMENTNotYetImplemented
                "Not yet implemented",
                // --> NO_IMPLEMENTComponentKindNotSupported
                "Component Kind not supported",
                // --> NO_IMPLEMENTTransactionsNotSupported
                "CORBA Transactions not supported",
                // --> NO_IMPLEMENTSecurityNotSupported
                "CORBA Security not supported",
            };

        const char* ExceptionStrings::NO_MEMORY_reasons[] =
            {
                // --> NO_MEMORY
                "Run out of memory"
            };

        const char* ExceptionStrings::NO_PERMISSION_reasons[] =
            {
                // --> NO_PERMISSION
                "No permission for this operation"
            };

        const char* ExceptionStrings::NO_RESOURCES_reasons[] =
            {
                // --> NO_RESOURCES
                "Run out of resources"
            };

        const char* ExceptionStrings::BAD_OPERATION_reasons[] =
            {
                // --> BAD_OPERATION
                "invalid operation"
            };

        const char* ExceptionStrings::OBJECT_NOT_EXIST_reasons[] =
            {
                // --> OBJECT_NOT_EXIST
                "Object does not exist",
                // --> OBJECT_NOT_EXISTInvalidExecutor
                "Executor does not exist",
                // --> OBJECT_NOT_EXISTObjectRemoved
                "Object is removed",
                // --> OBJECT_NOT_EXISTInvalidObjectId
                "Invalid Object Id",
                // --> OBJECT_NOT_EXISTExecutorCreateFailure
                "Executor Creation Failure",
                // --> OBJECT_NOT_EXISTCCMObjectNotFound
                "CCM Object Not Found",
                
            };

        const char* ExceptionStrings::INV_OBJREF_reasons[] =
            {
                // --> INV_OBJREF
                "Invalid object reference",
                // --> INV_OBJREFInvalidObjectGroupRefVersion
                "Invalid object group reference version",

            };

        const char* ExceptionStrings::TRANSIENT_reasons[] =
            {
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



    } // End namespace OrbSupport
} // End namespace Cdmw

