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

package cdmw.event.test;

import cdmw.common.Assert;
import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.namingandrepository.RepositoryInterface;

import cdmw.cdmwinit.InitUtils;
import cdmw.cdmwinit.CdmwInitConfiguration;
import cdmw.cdmwinit.XMLErrorException;
import cdmw.cdmwinit.ProcessControl;
import cdmw.cdmwinit.CdmwProcessImpl;

import org.omg.CORBA.CompletionStatus;
import org.omg.CORBA.SystemException;

public class ServerCdmwInterface {

    // TODO: Add exception specifications
    public static void cdmwInit(org.omg.CORBA.ORB orb, String[] args,
        ProcessControl ctrl) throws SystemException {

        Assert.check(orb!=null);
        boolean traceEnabled = false;

        try {

        // Get the XML init filename.
        // This may raise org.omg.CORBA.BAD_PARAM or org.omg.CORBA.INTERNAL
        String xmlFile = InitUtils.getXmlInitialisationFile(args);

        // Get the XML documents validation option.
        // This may raise org.omg.CORBA.BAD_PARAM or org.omg.CORBA.INTERNAL
        boolean validateXml = InitUtils.getXmlValidationOption(args);

        //Parsing xml
        CdmwInitConfiguration xmlData = null;
        try {
            CdmwInitConfiguration init
                = CdmwInitConfiguration.readConfiguration(xmlFile,validateXml);
            xmlData = init;
        } catch (cdmw.cdmwinit.ConfigurationException ce) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    CompletionStatus.COMPLETED_NO);
        } catch(InternalErrorException iie){
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALXMLLibraryError,
                    CompletionStatus.COMPLETED_NO);
        }

        // *****************************************************************
        // *****************  CDMW root POA and servant manager ***********
        // *****************************************************************

        // Create the CDMW root POA. It has its own POA Manager
        // May raise a org.omg.CORBA.SystemException.
        org.omg.PortableServer.POA cdmwRootPOA
            = InitUtils.createCdmwRootPOA(orb);

        org.omg.PortableServer.POAManager poaManager
            = cdmwRootPOA.the_POAManager();

        // Initialize the server so that incoming requests are
        // handled from this point on
        // May raise org.omg.PortableServer.POAManager.AdapterInactive
        // or org.omg.CORBA.SystemException
        try {
            poaManager.activate(); // TODO: maybe move it ?
        } catch (org.omg.PortableServer.POAManagerPackage.AdapterInactive ai) {
            // TODO: Add trace logs here
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALORBAdapterInactive,
                    CompletionStatus.COMPLETED_NO);
        }

        // *****************************************************************
        // *****************  INIT platforminterface object  **************
        // *****************************************************************

        CdmwProcessImpl processImpl = new CdmwProcessImpl(
            orb, InitUtils.getRootPOA(orb), cdmwRootPOA, ctrl);
        com.thalesgroup.CdmwPlatformMngt.ProcessDelegate initProcess
            = processImpl._this(orb);
        InitUtils.initPlatformInterface(orb,args,initProcess);
        // TODO: (NOTA)
        //  From now on, a platform supervisor may initiate a call to
        //  process->init() method.
        //  - Shall we disable this until init of CDMW Services has completed?
        //    We need acknowledge creation to get the process name...etc.
        //  - Shall we complete init of CDMW services in process->init()?
        //  - Shall we just block process->init() call until CDMW init has
        //    completed?
        //  - Shall we pospone activation of the POA manager until CDMW init has
        //    completed?

        // Retrieve Repository object reference.
        // This may raise org.omg.CORBA.BAD_PARAM
        com.thalesgroup.CdmwNamingAndRepository.Repository repository
            = InitUtils.getCdmwRepository();

        // *****************************************************************
        // *****************  INIT of RepositoryInterface ******************
        // *****************************************************************
        String defaultDomaineName= "";
        try {
            defaultDomaineName
                = xmlData.getServiceAttributeValue("repository-interface",
                                                    "default-domain-name");
        } catch (BadParameterException bpe) {
            processImpl.setCdmwInitExceptionRaised();
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    CompletionStatus.COMPLETED_NO);
        } catch (cdmw.cdmwinit.XMLErrorException xee) {
            processImpl.setCdmwInitExceptionRaised();
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    CompletionStatus.COMPLETED_NO);
        }

        try {
            RepositoryInterface.init(defaultDomaineName,repository);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
            processImpl.setCdmwInitExceptionRaised();
            // Default NameDomain is not a NameDomain!
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMNameDomainNotFound,
                    CompletionStatus.COMPLETED_NO);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            processImpl.setCdmwInitExceptionRaised();
            // Default NameDomain name is ill-formed!
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidDefaultDomainName,
                    CompletionStatus.COMPLETED_NO);
        }

        // *****************************************************************
        // *********         Init The CDMW Trace Library          *********
        // *****************************************************************

        String res;
        int flushingTime = 0;
        int msgThreshold = 0;
        int nbFlushArea = 0;
        int sizeFlushArea = 0;
        boolean initTrace = false;
        String collectorName;
        java.util.Vector collectorNameList = new java.util.Vector();

        // Get the process name.
        // This may raise org.omg.CORBA.BAD_INV_ORDER
        String applicationName = InitUtils.getCdmwApplicationName();

        // Get the process name.
        // This may raise org.omg.CORBA.BAD_INV_ORDER
        String processName = InitUtils.getCdmwProcessName();

        try {
            try {
                flushingTime = Integer.valueOf(
                    xmlData.getServiceAttributeValue("trace", "flushing-time"))
                        .intValue();
                nbFlushArea = Integer.valueOf(
                    xmlData.getServiceAttributeValue("trace", "flush-area-nb"))
                        .intValue();
                sizeFlushArea = Integer.valueOf(
                    xmlData.getServiceAttributeValue("trace", "flush-area-sz"))
                        .intValue();
                traceEnabled = true;
            } catch (BadParameterException bpe) {
                // do not throw an exception : this service is optional
            }

            try {
                msgThreshold = Integer.parseInt(
                        xmlData.getServiceAttributeValue(
                            "trace",
                            "msg-threshold"));
                collectorName = xmlData.getServiceAttributeValue("trace", "collector-name");
                collectorNameList.add(collectorName);
            } catch (BadParameterException bpe) {
                // do nothing : parameter is optional
            }

        } catch (XMLErrorException xee) {
            processImpl.setCdmwInitExceptionRaised();
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    CompletionStatus.COMPLETED_NO);
        } catch (NumberFormatException nfe) {
            processImpl.setCdmwInitExceptionRaised();
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    CompletionStatus.COMPLETED_NO);
        }

        if (traceEnabled) {
            // May raise org.omg.CORBA.SystemException
            cdmw.trace.InitUtils.initTraceLibrary(
                cdmwRootPOA,
                applicationName,
                processName,
                flushingTime,
                msgThreshold,
                nbFlushArea,
                sizeFlushArea,
                collectorNameList);
            // active the trace flushing to trace collector
            cdmw.trace.FlushAreaMngr.getInstance().activateFlushing();
        }

        // *****************************************************************
        // *********         Init The CDMW Performance Library    *********
        // *****************************************************************

        String performanceFileName = null;
        try {
            performanceFileName = xmlData.getServiceAttributeValue("performance","result-file-name");
        } catch (BadParameterException bpe) {
            // do not throw an exception : this service is optional
        } catch (XMLErrorException xee) {
            processImpl.setCdmwInitExceptionRaised();
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    CompletionStatus.COMPLETED_NO);
        }

        // May raise org.omg.CORBA.SystemException
        if (performanceFileName != null) {
            cdmw.performance.InitUtils.initPerformanceLibrary(cdmwRootPOA, performanceFileName);
            System.out.println("Initializing performance lib...........ok.");
        }

        // *****************************************************************
        // *********         Init The CDMW Time Service           *********
        // *****************************************************************

        // definition of Corbaname URL :
        //    Note : Time Service port is set to 2001
        String timeServiceCorbaname =
            "corbaname::1.@localhost:2001/NameService#COS/TimeService/TimeService";
        String timerEventServiceCorbaname =
            "corbaname::1.@localhost:2001/NameService#COS/TimeService/TimerEventHandler";

        // definitions of services
        String timeServiceName = "TimeService";
        String timerEventServiceName = "TimerEventService";

        org.omg.CORBA.Object timeServiceObj = null;

        try {
            timeServiceObj = orb.string_to_object(timeServiceCorbaname);
        } catch(Exception e) {
            // Do nothing (ignore : time service is not started)
            // TODO: Add trace logs here
        }

        if (timeServiceObj == null) {
            // Do nothing (ignore : time service is not started)
            // TODO: Add trace logs here
        } else {
            try {
                // register TimeService as service for ORB
                // to be reached by resolve_initial_reference orb api
                ORBSupport.registerInitialReference(
                    orb, timeServiceName, timeServiceObj);
            } catch(Exception e) {
                // Do nothing (ignore : time service is not started)
                // TODO: Add trace logs here
            }
        }

        org.omg.CORBA.Object timerEventServiceObj = null;

        try {
            timerEventServiceObj
                = orb.string_to_object(timerEventServiceCorbaname);
        } catch(Exception e) {
            // Do nothing (ignore : time service is not started)
            // TODO: Add trace logs here
        }

        if (timerEventServiceObj == null) {
            // Do nothing (ignore : time service is not started)
            // TODO: Add trace logs here
        } else {
            try {
                // register TimeService as service for ORB
                // to be reached by resolve_initial_reference orb api
                ORBSupport.registerInitialReference(
                    orb, timerEventServiceName, timerEventServiceObj);
            } catch(Exception e) {
                // Do nothing (ignore : time service is not started)
                // TODO: Add trace logs here
            }
        }


        // Discard requests until run order comes from the platform management
        try {
            poaManager.discard_requests(false);
        } catch (org.omg.PortableServer.POAManagerPackage.AdapterInactive ai) {
            // TODO: Add trace logs here
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALORBAdapterInactive,
                    CompletionStatus.COMPLETED_NO);
        }

        // advise processImpl that cdmwInit is done
        processImpl.setCdmwInitDone();

        } catch (cdmw.orbsupport.CORBASystemExceptionWrapper csew) {

            if(traceEnabled) {

                String domain = cdmw.trace.TraceDomains.CDMW_INIT;
                short level = cdmw.trace.TraceLevels.FTL;

                if (cdmw.trace.FlushAreaMngr.isToBeTraced(domain, level)) {
                    cdmw.trace.Stream stream = cdmw.trace.Stream.getInstance();
                    // filename and line number are avalaible in J2SDK 1.4
                    stream.setLine((short)0);
                    stream.setFile("unknown");
                    stream.setUserDomain(domain);
                    stream.setUserLevel(level);
                    stream.doFlush();
                } else { // In case there is a problem in the trace library itself!
                    System.out.println(domain + " : Fatal Error : " + csew.toString());
                }

            } else {
                System.out.println("cdmwInit: Fatal Error: " + csew.toString());
            }
            csew.raiseSystemException();

        }

    }


    // CDMW Factory objects will be deactivated at ORB shutdown.
    public static void cdmwCleanup(org.omg.CORBA.ORB orb) {
        Assert.check(orb!=null);

        // Release all static object references held by RepositoryInterface
        RepositoryInterface.finish();

        // Free all allocated memory and stop the trace service
        cdmw.trace.FlushAreaMngr.cleanup();

    }

}

