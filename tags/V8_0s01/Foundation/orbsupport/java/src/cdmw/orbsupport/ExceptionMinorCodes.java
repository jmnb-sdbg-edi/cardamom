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


package cdmw.orbsupport;

/**
 * Definition of Minor codes for CORBA system exceptions raised by
 * CDMW Runtime.
 *
 */
public class ExceptionMinorCodes {

    /**
     * CDMW VMCID (Vendor Minor Codeset ID)
     */
    public static final int CDMW_VMCID = 0x54480000;
    
    /**
     *  VMCID (Vendor Minor Codeset ID) mask
     */
    public static final int VMCID_MASK = 0xfffff000;
    public static final int MINOR_CODE_MASK = 0x00000fff;

    /**
     * CDMW specific Minor Codes for org.omg.CORBA.BAD_INV_ORDER system exception
     */
    public static final int BAD_INV_ORDER                 = 0 | CDMW_VMCID;
    public static final int BAD_INV_ORDERInitNotCalled    = 1 | CDMW_VMCID;
    public static final int BAD_INV_ORDERInitAlreadyDone  = 2 | CDMW_VMCID;
    public static final int BAD_INV_ORDERAckNotDone       = 3 | CDMW_VMCID;

    // WARNING!
    // WHENEVER YOU ADD A MINOR CODE, DO NOT FORGET ADDING CORRESPONDING ENTRY
    // IN ExceptionStrings.java

    /**
     * CDMW specific Minor Codes for org.omg.CORBA.BAD_PARAM system exception
     */
    public static final int BAD_PARAM                                 = 0 | CDMW_VMCID;
    public static final int BAD_PARAMNilObjectReference               = 1 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidApplicationAgentReference = 2 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidRepositoryReference       = 3 | CDMW_VMCID;
    public static final int BAD_PARAMApplicationAgentReferenceMissing = 4 | CDMW_VMCID;
    public static final int BAD_PARAMRepositoryNotFound               = 5 | CDMW_VMCID;
    public static final int BAD_PARAMProcessNameMissing               = 6 | CDMW_VMCID;
    public static final int BAD_PARAMUnknownProcessName               = 7 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidProcessName               = 8 | CDMW_VMCID;
    public static final int BAD_PARAMCannotGetRepositoryReference     = 9 | CDMW_VMCID;
    public static final int BAD_PARAMNameDomainNotFound               = 10 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidDefaultDomainName         = 11 | CDMW_VMCID;
    public static final int BAD_PARAMLifeCycleFrameworkInit           = 12 | CDMW_VMCID;
    public static final int BAD_PARAMLifeCycleFrameworkPOAExists      = 13 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidServiceName               = 14 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidTraceProducerLocation     = 15 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidTraceProducerName         = 16 | CDMW_VMCID;
    public static final int BAD_PARAMMissingORBThreadingStrategy      = 17 | CDMW_VMCID;
    public static final int BAD_PARAMPOAMutltiThreadStrategyRequired  = 18 | CDMW_VMCID;
    public static final int BAD_PARAMUnknownPOAType                   = 19 | CDMW_VMCID;
    public static final int BAD_PARAMCorbalocBindingAlreadyExists     = 20 | CDMW_VMCID;
    public static final int BAD_PARAMCorbalocBindingNotFound          = 21 | CDMW_VMCID;
    public static final int BAD_PARAMProcessXMLFileNameMissing        = 22 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidXMLInitData               = 23 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidPortNumber                = 24 | CDMW_VMCID;
    public static final int BAD_PARAMCdmwRootPOAExists               = 25 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidPerformanceAdminLocation  = 26 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidPerformanceAdminName      = 27 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidChannelName               = 28 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidHomeExecutor              = 29 | CDMW_VMCID;
    public static final int BAD_PARAMWrongContextType                 = 30 | CDMW_VMCID;
    public static final int BAD_PARAMUnknownHomeReference             = 31 | CDMW_VMCID;
    public static final int BAD_PARAMUnknownComponentReference        = 32 | CDMW_VMCID;
    public static final int BAD_PARAMUnknownContainerReference        = 33 | CDMW_VMCID;
    public static final int BAD_PARAMWrongValuetypeInstallerType      = 34 | CDMW_VMCID;
    public static final int BAD_PARAMHomeNotRegistered                = 35 | CDMW_VMCID;
    public static final int BAD_PARAMProcessCallbackReferenceMissing  = 36 | CDMW_VMCID;
    public static final int BAD_PARAMBadEventType                     = 37 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidExecutorName              = 38 | CDMW_VMCID;
    public static final int BAD_PARAMInvalidComponentExecutor         = 39 | CDMW_VMCID;

    /**
     * CDMW specific Minor Codes for link org.omg.CORBA.INTERNAL system exception
     */
    public static final int INTERNAL                               = 0 | CDMW_VMCID;
    public static final int INTERNALCannotReachRepository          = 1 | CDMW_VMCID;
    public static final int INTERNALCdmwRepositoryNotExists       = 2 | CDMW_VMCID;
    public static final int INTERNALCdmwApplicationAgentNotExists = 3 | CDMW_VMCID;
    public static final int INTERNALCdmwRepositoryError           = 4 | CDMW_VMCID;
    public static final int INTERNALServantProviderError           = 5 | CDMW_VMCID;
    public static final int INTERNALLifeCycleFrameworkError        = 6 | CDMW_VMCID;
    public static final int INTERNALInvalidTraceProducerLocation   = 7 | CDMW_VMCID;
    public static final int INTERNALInvalidTraceCollectorsLocation = 8 | CDMW_VMCID;
    public static final int INTERNALORBAdapterInactive             = 9 | CDMW_VMCID;
    public static final int INTERNALXMLLibraryError                = 10 | CDMW_VMCID;
    public static final int INTERNALCCMCIFError                    = 11 | CDMW_VMCID;
    public static final int INTERNALInvalidExecutor                = 12 | CDMW_VMCID;

    /**
     * CDMW specific Minor Codes for org.omg.CORBA.NO_IMPLEMENT system exception
     */
    public static final int NO_IMPLEMENT                           = 0 | CDMW_VMCID;
    public static final int NO_IMPLEMENTNotYetImplemented          = 1 | CDMW_VMCID;
    public static final int NO_IMPLEMENTComponentKindNotSupported  = 2 | CDMW_VMCID;
    public static final int NO_IMPLEMENTTransactionsNotSupported   = 3 | CDMW_VMCID;
    public static final int NO_IMPLEMENTSecurityNotSupported       = 4 | CDMW_VMCID;

    /**
     * CDMW specific Minor Codes for org.omg.CORBA.NO_MEMORY system exception
     */
    public static final int NO_MEMORY                              = 0 | CDMW_VMCID;

    /**
     * CDMW specific Minor Codes for org.omg.CORBA.NO_PERMISSION system exception
     */
    public static final int NO_PERMISSION                          = 0 | CDMW_VMCID;

    /**
     * CDMW specific Minor Codes for org.omg.CORBA.NO_RESOURCES system exception
     */
    public static final int NO_RESOURCES                           = 0 | CDMW_VMCID;

    /**
     * CDMW specific Minor Codes for org.omg.CORBA.OBJECT_NOT_EXIST system exception
     */
    public static final int OBJECT_NOT_EXIST                       = 0 | CDMW_VMCID;
    public static final int OBJECT_NOT_EXISTInvalidExecutor        = 1 | CDMW_VMCID;

    /**
     * CDMW specific Minor Codes for org.omg.CORBA.TRANSIENT system exception
     */
    public static final int TRANSIENT                              = 0 | CDMW_VMCID;
    public static final int TRANSIENTCannotReachRepository         = 1 | CDMW_VMCID;
    public static final int TRANSIENTCannotReachApplicationAgent   = 2 | CDMW_VMCID;
    public static final int TRANSIENTCannotReachEventServer        = 3 | CDMW_VMCID;
    public static final int TRANSIENTCannotReachNameService        = 4 | CDMW_VMCID;

}

