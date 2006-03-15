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


package cdmw.performance;


import cdmw.common.Locations;
import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;

import cdmw.ossupport.OS;
import cdmw.orbsupport.CORBASystemExceptionWrapper;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import cdmw.platformmngt.PlatformInterface;

import com.thalesgroup.CdmwNamingAndRepository.Repository;

import com.thalesgroup.CdmwPerformanceAdmin.PerformanceAdmin;

import org.omg.CORBA.CompletionStatus;

import org.omg.PortableServer.POA;


public class InitUtils {

    
    /**
     * Initialise the CDMW Performance Library.
     * 
     * @param performancePOA The POA to be used by Performance Library
     * @param performanceFilename The performance file name to store results
     * @exception CORBASystemExceptionWrapper Any wrapped CORBA system exception
     * The following CDMW minor codes details the reason:
     * <UL><LI><B>INTERNALCdmwRepositoryError</B> when its a CDMW Repository 
     * Error. This may happen if the Performance admin name domain is not 
     * present in the Repository, or if the ADMIN_ROOT_CONTEXT_ID is not set.
     * <LI><B>INTERNAL</B> when an Internal Error occurs during Init of 
     * PerformanceMngr.
     * <LI><B>BAD_PARAMInvalidPerformanceAdminLocation</B> when 
     * the location of the PerformanceMngr object is not valid.
     * <LI><B>BAD_PARAMInvalidPerformanceAdminName</B> when the name of the
     * PerformanceMngr object is not valid.
     * </UL>
     * @see PerformanceMngr#init
     */
    public static void initPerformanceLibrary (
        POA performancePOA, String performanceFilename) 
        throws CORBASystemExceptionWrapper {
        
        // init of PerformanceMngr
        PerformanceAdmin perfoManager = null;
        try {
            perfoManager = PerformanceMngr.init(performancePOA, 
                                                performanceFilename);
        } catch (BadParameterException bpe) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAM, 
                    CompletionStatus.COMPLETED_NO),
                bpe.what());
        }
        
        // Get "AdminRootContext" root context and then
        // register PerformanceMngr object
        org.omg.CosNaming.NamingContext adminRoot = null;
        try {
            Repository rep = RepositoryInterface.getRepository();
            adminRoot = rep.resolve_root_context(Locations.ADMIN_ROOT_CONTEXT_ID);
        } catch (com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext nrc) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    CompletionStatus.COMPLETED_NO),
                nrc.getMessage());
        } catch(org.omg.CORBA.SystemException se) {
            throw new CORBASystemExceptionWrapper(se, se.getMessage());
        }
        
        try {
            // Build the name of the Performance Manager object
            // Pattern is :
            //    hostname/appliName/processName/PERFORMANCE/PerformanceAdmin
            String perfoManagerName = OS.getHostname()
                + "/" + PlatformInterface.getApplicationName()
                + "/" + PlatformInterface.getProcessName() 
                + "/PERFORMANCE/PerformanceAdmin";
        
            NamingInterface ni = new NamingInterface(adminRoot);
            // If already registered, force its registration
            ni.bind(perfoManagerName, perfoManager, true);
        } catch(BadOrderException boe) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAM,
                    CompletionStatus.COMPLETED_NO),
                boe.what());
        } catch(org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            // Should not happen!
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidPerformanceAdminLocation,
                    CompletionStatus.COMPLETED_NO),
                nf.getMessage());
        } catch(org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            // Should not happen!
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    CompletionStatus.COMPLETED_NO),
                cp.getMessage());
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidPerformanceAdminName,
                    CompletionStatus.COMPLETED_NO),
                in.getMessage());
        } catch(org.omg.CosNaming.NamingContextPackage.AlreadyBound ab) {
            // Should not happen because force is set to true!
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    CompletionStatus.COMPLETED_NO),
                ab.getMessage());
        } catch(org.omg.CORBA.SystemException se) {
            throw new CORBASystemExceptionWrapper(se, se.getMessage());
        }
        
    }

}

