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


package cdmw.trace;

import cdmw.common.Locations;
import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

import cdmw.ossupport.OS;
import cdmw.orbsupport.CORBASystemExceptionWrapper;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import cdmw.platformmngt.PlatformInterface;

import com.thalesgroup.CdmwNamingAndRepository.Repository;

import com.thalesgroup.CdmwTrace.Collector;
import com.thalesgroup.CdmwTrace.CollectorHelper;
import com.thalesgroup.CdmwTrace.TraceProducerPackage.CollectorData;
import com.thalesgroup.CdmwTrace.TraceProducer;

import org.omg.CORBA.CompletionStatus;
import org.omg.PortableServer.POA;


public class InitUtils {


    /**
     * Name of the collector found registered during init
     */
    public final static String COLLECTOR_INI_MNEMONAME = "collector_ini_";


    /**
     * Initialise the CDMW Trace Library.
     *
     * @param tracePOA  The POA to be used for the TraceProducer object
     * @param applicationName The name of the application
     * @param processName The name of the CDMW Process
     * @param flushingTime The flushing time for the Flush Area Manager
     * @param msgThreshold The message threshold for the Flush Area Manager
     * @param nbFlushArea The number of flushing areas
     * for the Flush Area Manager
     * @param sizeFlushArea The size of a flush area for the Flush Area Manager
     * @exception CORBASystemExceptionWrapper Any wrapped CORBA system exception
     * The following CDMW minor codes details the possible reason:
     * <UL><LI><B>INTERNALCdmwRepositoryError</B> when its a CDMW Repository 
     * Error. This may happen if the Trace collectors name domain is not 
     * present in the Repository, or if the ADMIN_ROOT_CONTEXT_ID is not set.
     * <LI><B>INTERNALInvalidTraceCollectorsLocation</B> when the location of
     * TraceCollectors is not valid.
     * <LI><B>INTERNAL</B> when an Internal Error occurs during Init 
     * of Flush Area Manager.
     * <LI><B>BAD_PARAMInvalidTraceProducerLocation</B> when the location of
     * the TraceProducer object is not valid.
     * <LI><B>BAD_PARAMInvalidTraceProducerName</B> when the name of the
     * TraceProducer object is not valid.
     * </UL>
     * @see FlushAreaMngr#init
     */
    public static void initTraceLibrary(POA tracePOA, 
                                        String applicationName, 
                                        String processName, 
                                        int flushingTime,
                                        int msgThreshold,
                                        int nbFlushArea,
                                        int sizeFlushArea, 
                                        java.util.Vector collectorNameList) 
        throws CORBASystemExceptionWrapper {

        // create the initial collector list
        java.util.Vector collList = new java.util.Vector();

        // get the "AdminRootContext" root context.
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
        /* PCR597 ON ECR145
          // set trace collector location under default root context
          // Pattern is :
          //    CDMW/SERVICES/TRACE/COLLECTORS/"
          String collectorDomainStr;
          collectorDomainStr = Locations.CDMW_SERVICES_NAME_DOMAIN + "/";
          collectorDomainStr += Locations.TRACE_COLLECTORS_NAME_DOMAIN;

          // get Collector domain Naming Interface via Repository Interface
          NamingInterface niCollector =
                    RepositoryInterface
                        .getDomainNamingInterface(collectorDomainStr);
        */
          int collectorNb = collectorNameList.size();
          Object[] collNameList = collectorNameList.toArray();
        

        // PCR597 ON ECR-0145: there is only one local collector per node.
        // the pattern is: hostname/SERVICES/TRACE/COLLECTORS/<collector_name>
        // where collector_name is either the first name in collectorNameList
        // or "LocalCollector" if collectorNameList is empty.
          String collectorName = "LocalCollector";
          if (collectorNb > 0)
          {
              collectorName = (String) collNameList[0];
          }

          NamingInterface niCollector = new NamingInterface(adminRoot);
          boolean collectorFound = true;

        /* PCR597 ON ECR145
          for (int collector_ix=0 ; collector_ix < collectorNb ; collector_ix++) {
              collectorFound = true;

              // get collector name
              String collectorName = (String) collNameList[collector_ix];
        */
              Collector collector = null;
              String collectorDomainStr = OS.getHostname() + "/SERVICES/" +
                  Locations.TRACE_COLLECTORS_NAME_DOMAIN + "/" + collectorName;
              try {
                // We call each Collector, and we notify them of our creation
                org.omg.CORBA.Object obj = niCollector.resolve(collectorDomainStr);
                collector = CollectorHelper.narrow(obj);
              } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
                // ignore if collector not registered in repository
                collectorFound = false;
                /* PCR597 ON ECR145
                System.out.println("Trace Collector " + collectorName
                          + " is not found in repository");
                */
                System.out.println("The local trace Collector "
                                   + collectorDomainStr
                                   + " is not found in repository");
              }
              
              if (collectorFound) {
                // create collector mnemonic name
                String mnemoName = COLLECTOR_INI_MNEMONAME 
                    // PCR597 ON ECR145 + String.valueOf(collector_ix);
                    + String.valueOf(0);

                // create collector data structure
                CollectorData collectorData = new CollectorData();
                collectorData.the_collectorMnemoName = mnemoName;
                collectorData.the_collectorObjName = collectorName;
                collectorData.the_collector = collector;

                // insert collector data in list
                collList.add(collectorData);
              }
          //PCR597 ON ECR145}
              /* PCR597 ON ECR145
        } catch(com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    CompletionStatus.COMPLETED_NO),
                nnd.getMessage());
        } catch(com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALInvalidTraceCollectorsLocation,
                    CompletionStatus.COMPLETED_NO),
                in.getMessage());
              */
        } catch(org.omg.CosNaming.NamingContextPackage.InvalidName in) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALInvalidTraceCollectorsLocation,
                    CompletionStatus.COMPLETED_NO),
                in.getMessage());
        } catch(org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALInvalidTraceCollectorsLocation,
                    CompletionStatus.COMPLETED_NO),
                cp.getMessage());
        } catch(org.omg.CORBA.SystemException se) {
            throw new CORBASystemExceptionWrapper(se, se.getMessage());
        }

        // init of FlushAreaMngr
        CollectorData[] collectorList = new CollectorData[collList.size()];
        java.util.Enumeration e = collList.elements();
        int i = 0;
        for(; e.hasMoreElements() ;) {
            collectorList[i++] = (CollectorData) e.nextElement();
        }
        TraceProducer traceProducer = null;

        try {
            traceProducer = FlushAreaMngr.init(
                                tracePOA,
                                collectorList,
                                com.thalesgroup.CdmwTrace.ALL_COMPONENT_NAMES.value, // ECR-0123
                                com.thalesgroup.CdmwTrace.ALL_DOMAINS.value, 
                                com.thalesgroup.CdmwTrace.ALL_VALUES.value,
                                applicationName,
                                processName,
                                flushingTime,
                                nbFlushArea,
                                sizeFlushArea,
                                FlushAreaMngr.DEFAULT_AREA_NB_MESSAGES,
                                msgThreshold);

        } catch (BadParameterException bpe) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAM,
                    CompletionStatus.COMPLETED_NO),
                bpe.what());
        } catch (InternalErrorException iee) {
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNAL,
                    CompletionStatus.COMPLETED_NO),
                iee.what());
        }
        /* PCR597 ON ECR145
        // Get "AdminRootContext" root context and then
        // register TraceProducer object
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
        */

        try {
            
            // Build the name of the Trace Producer object
            // Pattern is :
            //    hostname/appliName/processName/TRACE/TraceProducer
            String traceProducerName = OS.getHostname()
                + "/" + applicationName
                + "/" + processName + "/TRACE/TraceProducer";

            NamingInterface ni = new NamingInterface(adminRoot);

            // If already registered, force its registration
            ni.bind(traceProducerName, traceProducer, true);
        } catch(org.omg.CosNaming.NamingContextPackage.NotFound nf) {
            // Should not happen!
            throw new CORBASystemExceptionWrapper(
                new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidTraceProducerLocation,
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
                    ExceptionMinorCodes.BAD_PARAMInvalidTraceProducerName,
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

