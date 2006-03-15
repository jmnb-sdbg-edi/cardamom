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


#ifndef INCL_ORBSUPPORT_EXCEPTION_MINOR_CODES_HPP
#define INCL_ORBSUPPORT_EXCEPTION_MINOR_CODES_HPP

#include "Foundation/orbsupport/CORBA.hpp"

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{
    /**
    * Namespace for CDMW OrbSupport types and data.
    */

    namespace OrbSupport
    {

        /**
        *Purpose:
        *<p>    Definition of Minor codes for CORBA system exceptions raised by
        * CDMW Runtime.
        *
        *Features:
        *<p> [Thread safety, exception safety, performance]
        *    [if no pertinent write none ]
        *
        *@see  [if necessary] 
        *@link [if necessary] 
        *
        */


        // CDMW VMCID (Vendor Minor Codeset ID)
        const CORBA::ULong CDMW_VMCID = 0x54480000;  //THxxxx


        /**
        * CDMW specific Minor Codes for CORBA::BAD_INV_ORDER system exception
        */
        enum BAD_INV_ORDERMinorCodes {
            BAD_INV_ORDER = 0 | CDMW_VMCID,
            BAD_INV_ORDERInitNotCalled          = 1  | CDMW_VMCID,
            BAD_INV_ORDERInitAlreadyDone        = 2  | CDMW_VMCID,
            BAD_INV_ORDERAckNotDone             = 3  | CDMW_VMCID,
            BAD_INV_ORDERIteratorPresent        = 4  | CDMW_VMCID,
            BAD_INV_ORDERLocationAlreadyPresent = 5  | CDMW_VMCID,
            BAD_INV_ORDERIteratorDestroyed      = 6  | CDMW_VMCID,
            BAD_INV_ORDERComponentNotConfigured = 7  | CDMW_VMCID,
            BAD_INV_ORDERNullLocationPtr        = 8  | CDMW_VMCID,
            BAD_INV_ORDERNullObjectGroupPtr     = 9  | CDMW_VMCID,
            BAD_INV_ORDERObjectGroupNotEmpty    = 10 | CDMW_VMCID
        };


        // WARNING!
        // WHENEVER YOU ADD A MINOR CODE, DO NOT FORGET ADDING CORRESPONDING ENTRY
        // IN ExceptionStrings.cpp#XXXX_reasons[]
        // COMMON PITFALL: BEWARE OF ENDING COMMA AS A MISSING COMMA WILL CONCATENATE
        // ADJACENT STRING LITERALS!!!

        /**
        * CDMW specific Minor Codes for CORBA::BAD_PARAM system exception
        */
        enum BAD_PARAMMinorCodes {
            BAD_PARAM = 0 | CDMW_VMCID,
            BAD_PARAMNilObjectReference = 1 | CDMW_VMCID,
            BAD_PARAMInvalidApplicationAgentReference = 2 | CDMW_VMCID,
            BAD_PARAMInvalidRepositoryReference = 3 | CDMW_VMCID,
            BAD_PARAMApplicationAgentReferenceMissing = 4 | CDMW_VMCID,
            BAD_PARAMRepositoryNotFound = 5 | CDMW_VMCID,
            BAD_PARAMProcessNameMissing = 6 | CDMW_VMCID,
            BAD_PARAMUnknownProcessName = 7 | CDMW_VMCID,
            BAD_PARAMInvalidProcessName = 8 | CDMW_VMCID,
            BAD_PARAMCannotGetRepositoryReference = 9 | CDMW_VMCID,
            BAD_PARAMNameDomainNotFound = 10 | CDMW_VMCID,
            BAD_PARAMInvalidDefaultDomainName = 11 | CDMW_VMCID,
            BAD_PARAMLifeCycleFrameworkInit = 12 | CDMW_VMCID,
            BAD_PARAMLifeCycleFrameworkPOAExists = 13 | CDMW_VMCID,
            BAD_PARAMInvalidServiceName = 14 | CDMW_VMCID,
            BAD_PARAMInvalidTraceProducerLocation = 15 | CDMW_VMCID,
            BAD_PARAMInvalidTraceProducerName = 16 | CDMW_VMCID,
            BAD_PARAMMissingORBThreadingStrategy = 17 | CDMW_VMCID,
            BAD_PARAMPOAMutltiThreadStrategyRequired = 18 | CDMW_VMCID,
            BAD_PARAMUnknownPOAType = 19 | CDMW_VMCID,
            BAD_PARAMCorbalocBindingAlreadyExists = 20 | CDMW_VMCID,
            BAD_PARAMCorbalocBindingNotFound = 21 | CDMW_VMCID,
            BAD_PARAMProcessXMLFileNameMissing = 22 | CDMW_VMCID,
            BAD_PARAMInvalidXMLInitData = 23 | CDMW_VMCID,
            BAD_PARAMInvalidPortNumber = 24 | CDMW_VMCID,
            BAD_PARAMCdmwRootPOAExists = 25 | CDMW_VMCID,
            BAD_PARAMInvalidPerformanceAdminLocation = 26 | CDMW_VMCID,
            BAD_PARAMInvalidPerformanceAdminName = 27 | CDMW_VMCID,
            BAD_PARAMInvalidChannelName = 28 | CDMW_VMCID,
            BAD_PARAMInvalidHomeExecutor = 29 | CDMW_VMCID,
            BAD_PARAMWrongContextType = 30 | CDMW_VMCID,
            BAD_PARAMUnknownHomeReference = 31 | CDMW_VMCID,
            BAD_PARAMUnknownComponentReference = 32 | CDMW_VMCID,
            BAD_PARAMUnknownContainerReference = 33 | CDMW_VMCID,
            BAD_PARAMWrongValuetypeInstallerType = 34 | CDMW_VMCID,
            BAD_PARAMHomeNotRegistered = 35 | CDMW_VMCID,
            BAD_PARAMProcessCallbackReferenceMissing = 36 | CDMW_VMCID,
            BAD_PARAMBadEventType = 37 | CDMW_VMCID,
            BAD_PARAMInvalidExecutorName = 38 | CDMW_VMCID,
            BAD_PARAMInvalidComponentExecutor = 39 | CDMW_VMCID,
            BAD_PARAMInvalidLocation = 40 | CDMW_VMCID,
            BAD_PARAMInvalidConfigValues = 41 | CDMW_VMCID,
            BAD_PARAMIncompatibleMonitoringInterval = 42 | CDMW_VMCID,
            BAD_PARAMIncompatibleMonitoringTimeout = 43 | CDMW_VMCID
        };


        /**
        * CDMW specific Minor Codes for CORBA::INTERNAL system exception
        */
        enum INTERNALMinorCodes {
            INTERNAL                               =  0 | CDMW_VMCID,
            INTERNALCannotReachRepository          =  1 | CDMW_VMCID,
            INTERNALCdmwRepositoryNotExists        =  2 | CDMW_VMCID,
            INTERNALCdmwApplicationAgentNotExists  =  3 | CDMW_VMCID,
            INTERNALCdmwRepositoryError            =  4 | CDMW_VMCID,
            INTERNALServantProviderError           =  5 | CDMW_VMCID,
            INTERNALLifeCycleFrameworkError        =  6 | CDMW_VMCID,
            INTERNALInvalidTraceProducerLocation   =  7 | CDMW_VMCID,
            INTERNALInvalidTraceCollectorsLocation =  8 | CDMW_VMCID,
            INTERNALORBAdapterInactive             =  9 | CDMW_VMCID,
            INTERNALXMLLibraryError                = 10 | CDMW_VMCID,
            INTERNALCCMCIFError                    = 11 | CDMW_VMCID,
            INTERNALInvalidExecutor                = 12 | CDMW_VMCID,
            INTERNALFaultToleranceError            = 13 | CDMW_VMCID,
            INTERNALNoCodecFactory                 = 14 | CDMW_VMCID,
            INTERNALLocationAlreadyPresent         = 15 | CDMW_VMCID,
            INTERNALDataStoreNotFound              = 16 | CDMW_VMCID,
            INTERNALInvalidChangeSequence          = 17 | CDMW_VMCID,
            INTERNALAlreadyActivated               = 18 | CDMW_VMCID,
            INTERNALLocationCtxtNotFound           = 19 | CDMW_VMCID,
            INTERNALObjGroupCtxtNotFound           = 20 | CDMW_VMCID,
            INTERNALSMUndefinedState               = 21 | CDMW_VMCID,
            INTERNALSMUndefinedTransition          = 22 | CDMW_VMCID,
            INTERNALSMIndexOutOfBounds             = 23 | CDMW_VMCID,
            INTERNALSMUnexpectedException          = 24 | CDMW_VMCID,
            INTERNALLocationInfoNotFound           = 25 | CDMW_VMCID,
            INTERNALObjectGroupDataNotFound        = 26 | CDMW_VMCID,
            INTERNALIncompleteLocationInfo         = 27 | CDMW_VMCID,
            INTERNALUnexpectedLocInfoState         = 28 | CDMW_VMCID,
            INTERNALNullObjGroupCtxtPtr            = 29 | CDMW_VMCID,
            INTERNALLoadBalancingError             = 30 | CDMW_VMCID,
            INTERNALComponentInvalidName           = 31 | CDMW_VMCID,
            INTERNALNoChannel                      = 32 | CDMW_VMCID,
            INTERNALNoContext                      = 33 | CDMW_VMCID,

        };


        /**
        * CDMW specific Minor Codes for CORBA::NO_IMPLEMENT system exception
        */
        enum NO_IMPLEMENTMinorCodes {
            NO_IMPLEMENT = 0 | CDMW_VMCID,
            NO_IMPLEMENTNotYetImplemented = 1 | CDMW_VMCID,
            NO_IMPLEMENTComponentKindNotSupported = 2 | CDMW_VMCID,
            NO_IMPLEMENTTransactionsNotSupported = 3 | CDMW_VMCID,
            NO_IMPLEMENTSecurityNotSupported = 4 | CDMW_VMCID
        };


        /**
        * CDMW specific Minor Codes for CORBA::NO_MEMORY system exception
        */
        enum NO_MEMORYMinorCodes {
            NO_MEMORY = 0 | CDMW_VMCID
        };


        /**
        * CDMW specific Minor Codes for CORBA::NO_PERMISSION system exception
        */
        enum NO_PERMISSIONMinorCodes {
            NO_PERMISSION = 0 | CDMW_VMCID
        };


        /**
        * CDMW specific Minor Codes for CORBA::NO_RESOURCES system exception
        */
        enum NO_RESOURCESMinorCodes {
            NO_RESOURCES = 0 | CDMW_VMCID
        };


        /**
        * CDMW specific Minor Codes for CORBA::BAD_OPERATION system exception
        */
        enum BAD_OPERATIONMinorCodes {
            BAD_OPERATION = 0 | CDMW_VMCID
        };


        /**
        * CDMW specific Minor Codes for CORBA::OBJECT_NOT_EXIST system exception
        */
        enum OBJECT_NOT_EXISTMinorCodes {
            OBJECT_NOT_EXIST = 0 | CDMW_VMCID,
            OBJECT_NOT_EXISTInvalidExecutor = 1 | CDMW_VMCID,
            OBJECT_NOT_EXISTObjectRemoved = 2 | CDMW_VMCID,
            OBJECT_NOT_EXISTInvalidObjectId = 3 | CDMW_VMCID,
            OBJECT_NOT_EXISTExecutorCreateFailure = 4 | CDMW_VMCID,
            OBJECT_NOT_EXISTCCMObjectNotFound = 5 | CDMW_VMCID,
        };

        /**
        * CDMW specific Minor Codes for CORBA::INV_OBJREF system exception
        */
        enum INV_OBJREFMinorCodes {
            INV_OBJREF = 0 | CDMW_VMCID,
            INV_OBJREFInvalidObjectGroupRefVersion = 1 | CDMW_VMCID
        };

        /**
        * CDMW specific Minor Codes for CORBA::TRANSIENT system exception
        */
        enum TRANSIENTMinorCodes {
            TRANSIENT = 0 | CDMW_VMCID,
            TRANSIENTCannotReachRepository = 1 | CDMW_VMCID,
            TRANSIENTCannotReachApplicationAgent = 2 | CDMW_VMCID,
            TRANSIENTCannotReachEventServer = 3 | CDMW_VMCID,
            TRANSIENTCannotReachNameService = 4 | CDMW_VMCID
        };


    } // End namespace OrbSupport
} // End namespace Cdmw

#endif // INCL_ORBSUPPORT_EXCEPTION_MINOR_CODES_HPP

