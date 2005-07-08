<?xml version="1.0" encoding="utf-8"?>
<!-- ===================================================================== -->
<!--
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
-->
<!-- ===================================================================== -->


<xsl:stylesheet version="1.1" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">



<!--
   Main - This template fetches the necessary data from inputs
   to generate a file.

   @param _programNode   A reference to a node /cdmw/programs/program.
-->

<xsl:template name="cdmw_interface_java">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>

   <xsl:variable name="programNodes" select="$cdmwNode/programs/program"/>

   <xsl:if test="boolean($programNodes)">
      <xsl:if test="$_index &lt;= count($programNodes)">
         <xsl:variable name="programNode" select="$programNodes[$_index]"/>

         <xsl:variable name="classname">
            <xsl:choose>
               <xsl:when test="boolean($programNode/@classname)">
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="$programNode/@classname"/>
                     <xsl:with-param name="_separator" select="$xmlSep"/>
                  </xsl:call-template>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="$programNode/@filename"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:variable>
         
         <xsl:variable name="package">
            <xsl:choose>
               <xsl:when test="boolean($programNode/@classname)">
                  <xsl:call-template name="getScope">
                     <xsl:with-param name="_name" select="$programNode/@classname"/>
                     <xsl:with-param name="_lang" select="'java'"/>
                  </xsl:call-template>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="'cdmw.generated'"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:variable>
      
         <!--
            Get the output filename.
         -->
         <xsl:variable name="filename">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$programNode/@directory"/>
               <xsl:with-param name="_userFilename" select="$classname"/>
               <xsl:with-param name="_defaultFilename" select="'CdmwInterface'"/>
               <xsl:with-param name="_fileExtension" select="'.java'"/>
            </xsl:call-template>
         </xsl:variable>
      
         <!--
            Display a message to the user.
         -->
         <xsl:message>
            <xsl:value-of select="concat('Generating file ', $filename)"/>
         </xsl:message>
      
         <!--
            Generate the file.
         -->
         <xsl:document method="text" href="{$filename}">
            <!--
               Insert the document header.
            -->
            <xsl:call-template name="docHeader">
               <xsl:with-param name="_lang" select="'java'"/>
            </xsl:call-template>
            <!--
               Insert the content.
            -->
            <xsl:call-template name="cdmw_interface_java.content">
               <xsl:with-param name="_programNode" select="$programNode"/>
               <xsl:with-param name="_javaClassname" select="$classname"/>
               <xsl:with-param name="_cdmwInterfacePackage" select="$package"/>
            </xsl:call-template>
         </xsl:document>
      
         <!--
            Proceed to the next node.
         -->
         <xsl:call-template name="cdmw_interface_java">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template cdmw_interface_java -->



<!--
   This template generates the content of the file.

   @param _programNode   A reference to a node /cdmw/programs/program.
-->

<xsl:template name="cdmw_interface_java.content">
   <xsl:param name="_programNode"/>
   <xsl:param name="_javaClassname"/>
   <xsl:param name="_cdmwInterfacePackage"/>
   
   <!--
      References to some nodes.
   -->
   <xsl:variable name="servicesNode" select="$_programNode/services"/>
   <xsl:variable name="frameworkNodes" select="$servicesNode/lifecycle/framework"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="hasRepositoryInterface" select="boolean($servicesNode/repository-interface)"/>
   <xsl:variable name="hasPerformance" select="boolean($servicesNode/performance)"/>
   <xsl:variable name="hasTrace" select="boolean($servicesNode/trace)"/>
   <xsl:variable name="hasTime" select="boolean($servicesNode/time)"/>
   <xsl:variable name="hasLifecycle" select="boolean($servicesNode/lifecycle)"/>
   <xsl:variable name="hasHomeImpl">
      <xsl:call-template name="containsHomeImpl">
         <xsl:with-param name="_programNode" select="$_programNode"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasEventPorts">
      <xsl:call-template name="containsEventPorts">
         <xsl:with-param name="_programNode" select="$_programNode"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      The code generation starts here.
   -->

<xsl:if test="string-length($_cdmwInterfacePackage)>0">
package <xsl:value-of select="$_cdmwInterfacePackage"/>;
</xsl:if>

import cdmw.common.Assert;
import cdmw.common.BadOrderException;
import cdmw.common.BadParameterException;
import cdmw.common.InternalErrorException;

import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.orbsupport.CORBASystemExceptionWrapper;

import cdmw.namingandrepository.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;
import com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT;
import com.thalesgroup.CdmwNamingAndRepository.RepositoryPackage.NoRootContext;

<xsl:if test="$hasTrace">
import cdmw.trace.FlushAreaMngr;
import cdmw.trace.TraceDomains;
import cdmw.trace.TraceLevels;
import cdmw.trace.Stream;
</xsl:if>

<xsl:if test="$hasPerformance">
import cdmw.performance.PerformanceMngr;
</xsl:if>

import cdmw.cdmwinit.InitUtils;
import cdmw.cdmwinit.ProcessControl;
import cdmw.cdmwinit.DefaultProcessControl;
import cdmw.cdmwinit.CdmwProcessImpl;
import cdmw.cdmwinit.XMLErrorException;
import cdmw.cdmwinit.CdmwInitConfiguration;
import cdmw.cdmwinit.ConfigurationException;

import cdmw.lifecycle.POAThreadingPolicy;
import cdmw.lifecycle.ThreadPoolPolicy;
import cdmw.lifecycle.ThreadPerRequestPolicy;
import cdmw.lifecycle.SingleThreadPolicy;
import cdmw.lifecycle.AnonymousObjectsFactoryInitUtil;
import cdmw.lifecycle.NamedObjectsFactoryInitUtil;
   
   <xsl:if test="$hasHomeImpl='true'">
      import cdmw.ccm.container.HomeAllocator;
      import cdmw.ccm.container.HomeAllocatorRegistry;
      import cdmw.common.Locations;
   </xsl:if>

public class <xsl:value-of select="$_javaClassname"/> {

    public static void cdmwInit(
        org.omg.CORBA.ORB orb,
        String[] args,
        ProcessControl ctrl)
        throws org.omg.CORBA.SystemException {

        Assert.check(orb != null);

        try {

            // Get the XML init filename. This may raise BAD_PARAM or INTERNAL
            String xmlFile = InitUtils.getXmlInitialisationFile(args);

            // Get the XML documents validation option. This may raise INTERNAL
            boolean validateXml = InitUtils.getXmlValidationOption(args);

            // Parsing XML
            CdmwInitConfiguration xmlData = null;
            try {
                CdmwInitConfiguration init =
                    CdmwInitConfiguration.readConfiguration(
                        xmlFile,
                        validateXml);
                xmlData = init;
            } catch (InternalErrorException iee) {
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (ConfigurationException ce) {
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            // *****************************************************************
            // *****************  CDMW root POA and servant manager ***********
            // *****************************************************************

            // Create the CDMW root POA. It has its own POA Manager
            // May raise a org.omg.CORBA.SystemException.
            org.omg.PortableServer.POA cdmwRootPOA =
                InitUtils.createCdmwRootPOA(orb);

            org.omg.PortableServer.POAManager poaManager =
                cdmwRootPOA.the_POAManager();

            // Initialize the server so that incoming requests are
            // handled from this point on
            // May raise org.omg.PortableServer.POAManager.AdapterInactive
            // or org.omg.CORBA.SystemException
            try {
                poaManager.activate(); // TODO: maybe move it ?
            } catch (
                org.omg.PortableServer.POAManagerPackage.AdapterInactive ai) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALORBAdapterInactive,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            // *****************************************************************
            // *****************  INIT platforminterface object  **************
            // *****************************************************************

            // Did we receive a ProcessControl callback object?
            ProcessControl processCtrl;
            if (ctrl != null)
                processCtrl = ctrl;
            else
                processCtrl = new DefaultProcessControl();            
            
            // CdmwProcessImpl() will acquire a reference on ProcessControl 
            // object and will automatically release it at object's destruction.
            CdmwProcessImpl processImpl =
                new CdmwProcessImpl(
                    orb,
                    InitUtils.getRootPOA(orb),
                    cdmwRootPOA,
                    processCtrl);
            com.thalesgroup.CdmwPlatformMngt.Process initProcess =
                processImpl._this(orb);
            InitUtils.initPlatformInterface(orb, args, initProcess);
            //  From now on, a platform supervisor may initiate a call to 
            //  process.init() method. 
            //  process.init() call is blocked with a condition until CDMW
            //  init has completed?

            // Get the process name.
            // This may raise org.omg.CORBA.BAD_INV_ORDER
            String processName = InitUtils.getCdmwProcessName();

            // Get the process name.
            // This may raise org.omg.CORBA.BAD_INV_ORDER
            String applicationName = InitUtils.getCdmwApplicationName();

            // Retrieve Repository object reference.
            // This may raise org.omg.CORBA.BAD_PARAM
            com.thalesgroup.CdmwNamingAndRepository.Repository repository =
                InitUtils.getCdmwRepository();

      <xsl:if test="$hasRepositoryInterface">
         <![CDATA[
            // *****************************************************************
            // *****************  INIT of RepositoryInterface ******************
            // *****************************************************************
            String defaultDomaineName = "";

            try {
                defaultDomaineName =
                    xmlData.getServiceAttributeValue(
                        "repository-interface",
                        "default-domain-name");
            } catch (BadParameterException bpe) {
                processImpl.setCdmwInitExceptionRaised();
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (XMLErrorException xmle) {
                processImpl.setCdmwInitExceptionRaised();
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            try {
                RepositoryInterface.init(defaultDomaineName, repository);
            } catch (
                com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
                processImpl.setCdmwInitExceptionRaised();
                // Default NameDomain is not a NameDomain!
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMNameDomainNotFound,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
                processImpl.setCdmwInitExceptionRaised();
                // Default NameDomain name is ill-formed!
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidDefaultDomainName,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
         ]]>
      </xsl:if>

      <xsl:if test="$hasTrace">
         <![CDATA[
            // *****************************************************************
            // *********         Init The CDMW Trace Library          *********
            // *****************************************************************

            String res;
            int flushingTime = 0;
            int nbFlushArea = 0;
            int sizeFlushArea = 0;
            String collectorName;
            java.util.Vector collectorNameList = new java.util.Vector();

            try {
                try {
                    flushingTime = Integer.parseInt(
                        xmlData.getServiceAttributeValue(
                            "trace",
                            "flushing-time"));
                    nbFlushArea = Integer.parseInt(
                        xmlData.getServiceAttributeValue(
                            "trace",
                            "flush-area-nb"));
                    sizeFlushArea = Integer.parseInt(
                        xmlData.getServiceAttributeValue(
                            "trace",
                            "flush-area-sz"));
                } catch (BadParameterException bpe) {
                    processImpl.setCdmwInitExceptionRaised();
                    throw new org.omg.CORBA.BAD_PARAM(
                        ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }

                try {
                    collectorName =
                        xmlData.getServiceAttributeValue(
                            "trace",
                            "collector-name");
                    collectorNameList.add(collectorName);
                } catch (BadParameterException bpe) {
                    // do nothing : parameter is optional
                }

            } catch (XMLErrorException xee) {
                processImpl.setCdmwInitExceptionRaised();
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (NumberFormatException nfe) {
                processImpl.setCdmwInitExceptionRaised();
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            // May raise org.omg.CORBA.SystemException
            cdmw.trace.InitUtils.initTraceLibrary(
                cdmwRootPOA,
                applicationName,
                processName,
                flushingTime,
                nbFlushArea,
                sizeFlushArea,
                collectorNameList);

            // active the trace flushing to trace collector
            FlushAreaMngr.getInstance().activateFlushing();
         ]]>
      </xsl:if>

      <xsl:if test="$hasPerformance">
         <![CDATA[
            // *****************************************************************
            // *********       Init The CDMW Performance Library      *********
            // *****************************************************************

            String performanceFileName = null;
            try {
                performanceFileName =
                    xmlData.getServiceAttributeValue(
                        "performance",
                        "result-file-name");
            } catch (BadParameterException bpe) {
                processImpl.setCdmwInitExceptionRaised();
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (XMLErrorException xee) {
                processImpl.setCdmwInitExceptionRaised();
                throw new org.omg.CORBA.BAD_PARAM(
                    ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            // May raise org.omg.CORBA.SystemException
            if (performanceFileName != null) {
                cdmw.performance.InitUtils.initPerformanceLibrary(
                    cdmwRootPOA,
                    performanceFileName);
            }
         ]]>
      </xsl:if>
      
      <xsl:if test="$hasTime">
         <![CDATA[
            // *****************************************************************
            // *********         Init The CDMW Time Service           *********
            // *****************************************************************
            
            // definition of Corbaname URL :
            //    Note : Time Service port is set to 2001
            String timeServiceCorbaname =
                "corbaname::1.2@localhost:2001/NameService#COS/TimeService/TimeService";
            String timerEventServiceCorbaname =
                "corbaname::1.2@localhost:2001/NameService#COS/TimeService/TimerEventHandler";

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
                    // to be reached by resolve_initial_reference orb API
                    cdmw.orbsupport.ORBSupport.registerInitialReference(
                        orb,
                        timeServiceName, 
                        timeServiceObj);
                } catch(Exception e) {
                    // Do nothing (ignore : time service is not started)
                    // TODO: Add trace logs here
                }

                org.omg.CORBA.Object timerEventServiceObj = null;

                try {
                    timerEventServiceObj = orb.string_to_object(
                        timerEventServiceCorbaname);
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
                        cdmw.orbsupport.ORBSupport.registerInitialReference(
                            orb, 
                            timerEventServiceName, 
                            timerEventServiceObj);
                    } catch(Exception e) {
                        // Do nothing (ignore : time service is not started)
                        // TODO: Add trace logs here
                    }
                }
            }
         ]]>
      </xsl:if>

      <xsl:if test="$hasLifecycle">
         <![CDATA[
            // *****************************************************************
            // *********  Create CDMW Factories POA (persistent POA)  *********
            // *****************************************************************
            // Raises org.omg.CORBA.SystemException
            org.omg.PortableServer.POA factoriesPOA =
                InitUtils.createCdmwLifeCyclePOA(cdmwRootPOA);

            // Create CDMW factories

            org.omg.CosNaming.NamingContext ncRootContext = null;
            try {
                ncRootContext = 
                    repository.resolve_root_context(DEFAULT_ROOT_CONTEXT.value);
            } catch (NoRootContext nrc) {
                processImpl.setCdmwInitExceptionRaised();
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            } catch (org.omg.CORBA.SystemException se) {
                processImpl.setCdmwInitExceptionRaised();
                throw se;
            }

            // May raise a org.omg.CORBA.SystemException
            NamingInterface rootContext = new NamingInterface(ncRootContext);
         ]]>

      <xsl:if test="$hasHomeImpl='true'">
         // Register Valuetype factories
         cdmw.ccm.cif.InitUtils.registerValuetypeFactories(orb);
         cdmw.ccm.container.InitUtils.registerValuetypeFactories(orb);
      </xsl:if>

      String fullNameDomainName = "";
      String fullFactoryName = "";
      POAThreadingPolicy threadingPolicy = null;
      int i = 0;
      <xsl:for-each select="$frameworkNodes">
         <xsl:variable name="frameworkIdref" select="@idref"/>

         <!--
            Get the matching node /cdmw/lifecycle-frameworks/lifecycle-framework.
         -->
         <xsl:variable name="lifecycleFrameworkNode" select="$lifecycleFrameworkNodes[@id = $frameworkIdref]"/>
         <xsl:variable name="isHomeImpl" select="boolean($lifecycleFrameworkNode/home-impl)"/>

         <!--
            Get the factory classname.
         -->
         <!--
         <xsl:variable name="factoryClassname" select="$lifecycleFrameworkNode/*/@classname"/>
         -->
         <xsl:variable name="factoryClassname">
            <xsl:choose>
               <xsl:when test="$isHomeImpl">
                  <xsl:call-template name="buildHomeImplClassname">
                     <xsl:with-param name="_homeImplNode" select="$lifecycleFrameworkNode/home-impl"/>
                  </xsl:call-template>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="$lifecycleFrameworkNode/*/@classname"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:variable>

         <!--
            Format the class name for output.
         -->
         <xsl:variable name="javaFactoryClassname">
            <xsl:call-template name="replaceCharsInString">
               <xsl:with-param name="_stringIn" select="$factoryClassname"/>
               <xsl:with-param name="_charsIn" select="$xmlSep"/>
               <xsl:with-param name="_charsOut" select="$javaSep"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the strategy.
         -->
         <xsl:variable name="strategy">
            <xsl:variable name="category" select="$lifecycleFrameworkNode/*/@category"/>
            <xsl:choose>
               <xsl:when test="boolean($category)">
                  <xsl:value-of select="$category"/>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="'USER_DEFINED'"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:variable>

         <!--
            Register the home allocator.
         -->
         <xsl:if test="$isHomeImpl">
            // Register <xsl:value-of select="$javaFactoryClassname"/> HomeAllocator
            HomeAllocatorRegistry.register("<xsl:value-of select="$javaFactoryClassname"/>",
                                           new HomeAllocator("<xsl:value-of select="$javaFactoryClassname"/>"));
         </xsl:if>
         
         <!--
            Strategy selection.
         -->
         <xsl:choose>
            <!--
               ENTITY.
            -->
            <xsl:when test="$strategy = 'ENTITY'">
               <xsl:choose>
                  <xsl:when test="$isHomeImpl">
                     <!--
                        TODO: Add component server code here!
                     -->
                  </xsl:when>
                  <xsl:otherwise>
                     // *****************************************************************
                     // ****** <xsl:value-of select="$javaFactoryClassname"/>  ******
                     // ***************************************************************** 
                     fullNameDomainName = "";
                     fullFactoryName = "";
                     threadingPolicy = null;
                     i = 0;

                     <xsl:if test="@occurrences > 0">
                        try {
                            fullNameDomainName =
                                xmlData.getFrameworkAttributeValue(
                                    "<xsl:value-of select="$frameworkIdref"/>",
                                    i,
                                    "domain-name");
                            fullFactoryName =
                                xmlData.getFrameworkAttributeValue(
                                    "<xsl:value-of select="$frameworkIdref"/>",
                                    i,
                                    "full-factory-name");
                            // TODO: If fullNameDomainName and fullFactoryName are empty
                            // use default name domain
                   
                        <xsl:choose>
                           <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                              threadingPolicy = new SingleThreadPolicy();
                           </xsl:when>
                           <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                              String[] values =
                                  xmlData.getFrameworkMultithreadAttribute(
                                      "<xsl:value-of select="$frameworkIdref"/>",
                                      i);
                              String xmlThreadingPolicy = values[0];
                              String xmlThreadingAttribute = values[1];
                              String xmlThreadingAttributeValue = values[2];
                              
                              if (!xmlThreadingPolicy.equals("serialize")) {
                                  threadingPolicy = new SingleThreadPolicy();
                              } else if (!xmlThreadingPolicy.equals("thread-per-request")) {
                                  threadingPolicy = new ThreadPerRequestPolicy();
                              } else if (!xmlThreadingPolicy.equals("thread-pool")) {
                                  int threadPoolSize =
                                      Integer.valueOf(xmlThreadingAttributeValue).intValue();
                                  threadingPolicy = new ThreadPoolPolicy(threadPoolSize);
                              }
                           </xsl:when>
                        </xsl:choose>
                        
                        <![CDATA[
                           } catch (NumberFormatException nfe) {
                               processImpl.setCdmwInitExceptionRaised();
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           } catch (XMLErrorException xee) {
                               processImpl.setCdmwInitExceptionRaised();
                               // TODO: Add trace logs here
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           } catch (BadParameterException bpe) {
                               processImpl.setCdmwInitExceptionRaised();
                               // TODO: Add trace logs here
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           }
                        ]]>
                        
                        try {
                            // Get the factory Allocator
                            // TODO: Use a user-provided name rather than a naming convention!
                            <xsl:value-of select="$javaFactoryClassname"/>Allocator allocator = 
                                <xsl:value-of select="$javaFactoryClassname"/>Allocator.theAllocator;
                        
                            NamedObjectsFactoryInitUtil
                                .setupNamedObjectFactory(
                                    allocator,
                                    orb,
                                    factoriesPOA,
                                    repository,
                                    rootContext,
                                    fullNameDomainName,
                                    fullFactoryName,
                                    "", // TODO:
                                    threadingPolicy,
                                    args);

                        <![CDATA[
                        } catch (BadParameterException bpe) {
                            processImpl.setCdmwInitExceptionRaised();
                            throw new org.omg.CORBA.BAD_PARAM(
                                ExceptionMinorCodes.BAD_PARAMLifeCycleFrameworkInit,
                                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                        } catch (InternalErrorException iie) {
                            processImpl.setCdmwInitExceptionRaised();
                            throw new org.omg.CORBA.INTERNAL(
                                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                        }
                        ]]>
                     </xsl:if>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <!--
               UNMANAGED_ENTITY.
            -->
            <xsl:when test="$strategy = 'UNMANAGED_ENTITY'">
               // *****************************************************************
               // ****** <xsl:value-of select="$javaFactoryClassname"/>  ******
               // *****************************************************************
               fullNameDomainName = "";
               fullFactoryName = "";
               threadingPolicy = null;
               i = 0;

               <xsl:if test="@occurrences > 0">
                  try {
                      fullNameDomainName =
                          xmlData.getFrameworkAttributeValue(
                              "<xsl:value-of select="$frameworkIdref"/>",
                              i,
                              "domain-name");
                      fullFactoryName =
                          xmlData.getFrameworkAttributeValue(
                              "<xsl:value-of select="$frameworkIdref"/>",
                              i,
                              "full-factory-name");
                      // TODO: If fullNameDomainName and fullFactoryName are empty
                      // use default name domain
             
                  <xsl:choose>
                     <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                        threadingPolicy = new SingleThreadPolicy();
                     </xsl:when>
                     <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                        String[] values =
                            xmlData.getFrameworkMultithreadAttribute(
                                "<xsl:value-of select="$frameworkIdref"/>",
                                i);
                        String xmlThreadingPolicy = values[0];
                        String xmlThreadingAttribute = values[1];
                        String xmlThreadingAttributeValue = values[2];
                        
                        if (!xmlThreadingPolicy.equals("serialize")) {
                            threadingPolicy = new SingleThreadPolicy();
                        } else if (!xmlThreadingPolicy.equals("thread-per-request")) {
                            threadingPolicy = new ThreadPerRequestPolicy();
                        } else if (!xmlThreadingPolicy.equals("thread-pool")) {
                            int threadPoolSize =
                                Integer.valueOf(xmlThreadingAttributeValue).intValue();
                            threadingPolicy = new ThreadPoolPolicy(threadPoolSize);
                        }
                     </xsl:when>
                  </xsl:choose>
                  
                  <![CDATA[
                     } catch (NumberFormatException nfe) {
                         processImpl.setCdmwInitExceptionRaised();
                         throw new org.omg.CORBA.BAD_PARAM(
                             ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                     } catch (XMLErrorException xee) {
                         processImpl.setCdmwInitExceptionRaised();
                         // TODO: Add trace logs here
                         throw new org.omg.CORBA.BAD_PARAM(
                             ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                     } catch (BadParameterException bpe) {
                         processImpl.setCdmwInitExceptionRaised();
                         // TODO: Add trace logs here
                         throw new org.omg.CORBA.BAD_PARAM(
                             ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                     }
                  ]]>
                  
                  try {
                      // Get the factory Allocator
                      // TODO: Use a user-provided name rather than a naming convention!
                      <xsl:value-of select="$javaFactoryClassname"/>Allocator allocator = 
                          <xsl:value-of select="$javaFactoryClassname"/>Allocator.theAllocator;
                        
                      NamedObjectsFactoryInitUtil
                          .setupNamedObjectFactory(
                              allocator,
                              orb,
                              factoriesPOA,
                              repository,
                              rootContext,
                              fullNameDomainName,
                              fullFactoryName,
                              "", // TODO:
                              threadingPolicy,
                              args);

                  <![CDATA[
                  } catch (BadParameterException bpe) {
                      processImpl.setCdmwInitExceptionRaised();
                      throw new org.omg.CORBA.BAD_PARAM(
                          ExceptionMinorCodes.BAD_PARAMLifeCycleFrameworkInit,
                          org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                  } catch (InternalErrorException iie) {
                      processImpl.setCdmwInitExceptionRaised();
                      throw new org.omg.CORBA.INTERNAL(
                          ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                          org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                  }
                  ]]>
               </xsl:if>
            </xsl:when>
            <!--
               SESSION.
            -->
            <xsl:when test="$strategy = 'SESSION'">
               <xsl:choose>
                  <xsl:when test="$isHomeImpl">
                     <!--
                        TODO: Add component server code here!
                     -->
                  </xsl:when>
                  <xsl:otherwise>
                     // *****************************************************************
                     // ****** <xsl:value-of select="$javaFactoryClassname"/>  ******
                     // *****************************************************************
                     fullFactoryName = "";
                     threadingPolicy = null;
                     i = 0;
                     
                     <xsl:if test="@occurrences > 0">
                        try {
                            fullFactoryName =
                                xmlData.getFrameworkAttributeValue(
                                    "<xsl:value-of select="$frameworkIdref"/>",
                                    i,
                                    "full-factory-name");
                                                  
                        <xsl:choose>
                           <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                              threadingPolicy = new SingleThreadPolicy();
                           </xsl:when>
                           <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                              String[] values =
                                  xmlData.getFrameworkMultithreadAttribute(
                                      "<xsl:value-of select="$frameworkIdref"/>",
                                      i);
                              String xmlThreadingPolicy = values[0];
                              String xmlThreadingAttribute = values[1];
                              String xmlThreadingAttributeValue = values[2];
                              
                              if (!xmlThreadingPolicy.equals("serialize")) {
                                  threadingPolicy = new SingleThreadPolicy();
                              } else if (!xmlThreadingPolicy.equals("thread-per-request")) {
                                  threadingPolicy = new ThreadPerRequestPolicy();
                              } else if (!xmlThreadingPolicy.equals("thread-pool")) {
                                  int threadPoolSize =
                                      Integer.parseInt(xmlThreadingAttributeValue);
                                  threadingPolicy = new ThreadPoolPolicy(threadPoolSize);
                              }
                           </xsl:when>
                        </xsl:choose>

                        <![CDATA[
                           } catch (NumberFormatException nfe) {
                               processImpl.setCdmwInitExceptionRaised();
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           } catch (XMLErrorException xee) {
                               processImpl.setCdmwInitExceptionRaised();
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           } catch (BadParameterException bpe) {
                               processImpl.setCdmwInitExceptionRaised();
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           }
                        ]]>

                        try {
                            // Get the factory Allocator
                            // TODO: Use a user-provided name rather than a naming convention!
                            <xsl:value-of select="$javaFactoryClassname"/>Allocator allocator = 
                                <xsl:value-of select="$javaFactoryClassname"/>Allocator.theAllocator;
                        
                            AnonymousObjectsFactoryInitUtil
                                .setupAnonymousObjectFactory(
                                    allocator,
                                    orb,
                                    factoriesPOA,
                                    repository,
                                    rootContext,
                                    fullFactoryName,
                                    "", // TODO:
                                    threadingPolicy, 
                                    args);
                         <![CDATA[
                           } catch (BadParameterException bpe) {
                               processImpl.setCdmwInitExceptionRaised();
                               // TODO: Add trace logs here
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMLifeCycleFrameworkInit,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           } catch (InternalErrorException iie) {
                               processImpl.setCdmwInitExceptionRaised();
                               // TODO: Add trace logs here
                               throw new org.omg.CORBA.INTERNAL(
                                   ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           }
                         ]]>
                     </xsl:if>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <!--
               SERVICE.
            -->
            <xsl:when test="$strategy = 'SERVICE'">
               <xsl:choose>
                  <xsl:when test="$isHomeImpl">
                     <!--
                        TODO: Add component server code here!
                     -->
                  </xsl:when>
                  <xsl:otherwise>
                     // *****************************************************************
                     // ****** <xsl:value-of select="$javaFactoryClassname"/>  ******
                     // *****************************************************************
                     fullFactoryName = "";
                     threadingPolicy = null;
                     i = 0;
                     
                     <xsl:if test="@occurrences > 0">
                        try {
                            fullFactoryName =
                                xmlData.getFrameworkAttributeValue(
                                    "<xsl:value-of select="$frameworkIdref"/>",
                                    i,
                                    "full-factory-name");
                                                  
                        <xsl:choose>
                           <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                              threadingPolicy = new SingleThreadPolicy();
                           </xsl:when>
                           <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                              String[] values =
                                  xmlData.getFrameworkMultithreadAttribute(
                                      "<xsl:value-of select="$frameworkIdref"/>",
                                      i);
                              String xmlThreadingPolicy = values[0];
                              String xmlThreadingAttribute = values[1];
                              String xmlThreadingAttributeValue = values[2];
                              
                              if (!xmlThreadingPolicy.equals("serialize")) {
                                  threadingPolicy = new SingleThreadPolicy();
                              } else if (!xmlThreadingPolicy.equals("thread-per-request")) {
                                  threadingPolicy = new ThreadPerRequestPolicy();
                              } else if (!xmlThreadingPolicy.equals("thread-pool")) {
                                  int threadPoolSize =
                                      Integer.parseInt(xmlThreadingAttributeValue);
                                  threadingPolicy = new ThreadPoolPolicy(threadPoolSize);
                              }
                           </xsl:when>
                        </xsl:choose>

                        <![CDATA[
                           } catch (NumberFormatException nfe) {
                               processImpl.setCdmwInitExceptionRaised();
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           } catch (XMLErrorException xee) {
                               processImpl.setCdmwInitExceptionRaised();
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           } catch (BadParameterException bpe) {
                               processImpl.setCdmwInitExceptionRaised();
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           }
                        ]]>

                        try {
                            // Get the factory Allocator
                            // TODO: Use a user-provided name rather than a naming convention!
                            <xsl:value-of select="$javaFactoryClassname"/>Allocator allocator = 
                                <xsl:value-of select="$javaFactoryClassname"/>Allocator.theAllocator;
                        
                            AnonymousObjectsFactoryInitUtil
                                .setupAnonymousObjectFactory(
                                    allocator,
                                    orb,
                                    factoriesPOA,
                                    repository,
                                    rootContext,
                                    fullFactoryName,
                                    "", // TODO:
                                    threadingPolicy, 
                                    args);
                         <![CDATA[
                           } catch (BadParameterException bpe) {
                               processImpl.setCdmwInitExceptionRaised();
                               // TODO: Add trace logs here
                               throw new org.omg.CORBA.BAD_PARAM(
                                   ExceptionMinorCodes.BAD_PARAMLifeCycleFrameworkInit,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           } catch (InternalErrorException iie) {
                               processImpl.setCdmwInitExceptionRaised();
                               // TODO: Add trace logs here
                               throw new org.omg.CORBA.INTERNAL(
                                   ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                           }
                         ]]>
                     </xsl:if>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$strategy = 'PROCESS'">
               <!--
                  TODO: Add component server code here!
               -->
            </xsl:when>
            <!--
               USER_DEFINED.
            -->
            <xsl:when test="$strategy = 'USER_DEFINED'">
               <xsl:variable name="isNamed" select="$factoryNodes[@idl-interface = $lifecycleFrameworkNode/@factory-idref]/@kind = 'NAMED'"/>
               <xsl:variable name="namedString">
                  <xsl:choose>
                     <xsl:when test="$isNamed">
                        <xsl:text>Named</xsl:text>
                     </xsl:when>
                     <xsl:otherwise>
                        <xsl:text>Anonymous</xsl:text>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:variable>
               <xsl:variable name="javaFactoryAllocatorClassname">
                  <xsl:call-template name="replaceCharsInString">
                     <xsl:with-param name="_stringIn" select="$lifecycleFrameworkNode/user-factory-impl/factory-allocator/@classname"/>
                     <xsl:with-param name="_charsIn" select="$xmlSep"/>
                     <xsl:with-param name="_charsOut" select="$javaSep"/>
                  </xsl:call-template>
               </xsl:variable>

               // *****************************************************************
               // ******                User defined factory                 ******
               // *****************************************************************
               fullFactoryName = "";;
               <xsl:if test="$isNamed">fullNameDomainName= "";</xsl:if>
               threadingPolicy = null;
               i = 0;

               <xsl:if test="@occurrences > 0">
                  try {
                  <xsl:choose>
                     <xsl:when test="$isNamed">
                        fullNameDomainName =
                          xmlData.getFrameworkAttributeValue(
                              "<xsl:value-of select="$frameworkIdref"/>",
                              i,
                              "domain-name");
                        fullFactoryName =
                          xmlData.getFrameworkAttributeValue(
                              "<xsl:value-of select="$frameworkIdref"/>",
                              i,
                              "full-factory-name");
                        // TODO: If fullNameDomainName and fullFactoryName are empty
                        // use default name domain
                     </xsl:when>
                     <xsl:otherwise>
                        fullFactoryName =
                          xmlData.getFrameworkAttributeValue(
                              "<xsl:value-of select="$frameworkIdref"/>",
                              i,
                              "full-factory-name");
                     </xsl:otherwise>
                  </xsl:choose>
                  
                  <xsl:choose>
                     <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                        threadingPolicy = new SingleThreadPolicy();
                     </xsl:when>
                     <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                        String[] values =
                            xmlData.getFrameworkMultithreadAttribute(
                                "<xsl:value-of select="$frameworkIdref"/>",
                                i);
                        String xmlThreadingPolicy = values[0];
                        String xmlThreadingAttribute = values[1];
                        String xmlThreadingAttributeValue = values[2];
                        
                        if (!xmlThreadingPolicy.equals("serialize")) {
                            threadingPolicy = new SingleThreadPolicy();
                        } else if (!xmlThreadingPolicy.equals("thread-per-request")) {
                            threadingPolicy = new ThreadPerRequestPolicy();
                        } else if (!xmlThreadingPolicy.equals("thread-pool")) {
                            int threadPoolSize =
                                Integer.valueOf(xmlThreadingAttributeValue).intValue();
                            threadingPolicy = new ThreadPoolPolicy(threadPoolSize);
                        }
                     </xsl:when>
                  </xsl:choose>

                  <![CDATA[
                     } catch (NumberFormatException nfe) {
                         processImpl.setCdmwInitExceptionRaised();
                         throw new org.omg.CORBA.BAD_PARAM(
                             ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                     } catch (XMLErrorException xee) {
                         processImpl.setCdmwInitExceptionRaised();
                         throw new org.omg.CORBA.BAD_PARAM(
                             ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                     } catch (BadParameterException bpe) {
                         processImpl.setCdmwInitExceptionRaised();
                         throw new org.omg.CORBA.BAD_PARAM(
                             ExceptionMinorCodes.BAD_PARAMInvalidXMLInitData,
                             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                     }
                  ]]>

                  try {
                      // Get the factory Allocator
                      // TODO: Use a user-provided name rather than a naming convention!
                      <xsl:value-of select="$javaFactoryAllocatorClassname"/> allocator = 
                          <xsl:value-of select="$javaFactoryAllocatorClassname"/>.theAllocator;
                        
                      <xsl:value-of select="$namedString"/>ObjectsFactoryInitUtil
                          .setup<xsl:value-of select="$namedString"/>ObjectFactory(
                              allocator,
                              orb,
                              factoriesPOA,
                              repository,
                              rootContext,
                              <xsl:if test="$isNamed">fullNameDomainName,</xsl:if>
                              fullFactoryName,
                              "", // TODO:
                              threadingPolicy, 
                              args);
                  <![CDATA[
                     } catch (BadParameterException bpe) {
                         processImpl.setCdmwInitExceptionRaised();
                         throw new org.omg.CORBA.BAD_PARAM(
                             ExceptionMinorCodes.BAD_PARAMLifeCycleFrameworkInit,
                             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                     } catch (InternalErrorException iie) {
                         processImpl.setCdmwInitExceptionRaised();
                         throw new org.omg.CORBA.INTERNAL(
                             ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                     }
                  ]]>
               </xsl:if>
            </xsl:when>
         </xsl:choose>
      </xsl:for-each>
      
     <xsl:if test="$hasHomeImpl='true'">
        <![CDATA[  
         // Get a Default Event Channel Factory
         com.thalesgroup.CdmwEvent.EventChannelFactory eventChannelFactory = null;
         ]]>

         <xsl:if test="$hasEventPorts = 'true'">
            <![CDATA[
            try {
                // Retrieve reference to the Default Event Channel factory 
                String evtFactoriesNd = 
                    Locations.CDMW_EVENT_SERVICE_NAME_DOMAIN + "/FACTORIES";

                NamingInterface evtNi = 
                    RepositoryInterface.getDomainNamingInterface(evtFactoriesNd);
           
                String EVENT_CHANNEL_FACTORY_NAME = "DefaultEventChannelFactory";

                eventChannelFactory = com.thalesgroup.CdmwEvent.EventChannelFactoryHelper.narrow(
                   evtNi.resolve(EVENT_CHANNEL_FACTORY_NAME));
            } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
                throw new CORBASystemExceptionWrapper(
                    new org.omg.CORBA.BAD_PARAM(
                        ExceptionMinorCodes.BAD_PARAMNameDomainNotFound,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO),
                    Locations.CDMW_EVENT_SERVICE_NAME_DOMAIN + "/FACTORIES is not a NameDomain!");
            } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
                throw new CORBASystemExceptionWrapper(
                    new org.omg.CORBA.INTERNAL(
                        ExceptionMinorCodes.INTERNAL,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO));
            } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
                throw new CORBASystemExceptionWrapper(
                    new org.omg.CORBA.INTERNAL(
                        ExceptionMinorCodes.INTERNAL,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO));
            } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
                throw new CORBASystemExceptionWrapper(
                    new org.omg.CORBA.INTERNAL(
                        ExceptionMinorCodes.INTERNAL,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO));
            } catch (org.omg.CosNaming.NamingContextPackage.InvalidName in) {
                throw new CORBASystemExceptionWrapper(
                    new org.omg.CORBA.INTERNAL(
                        ExceptionMinorCodes.INTERNAL,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO));
            } catch (org.omg.CORBA.SystemException se) {
                throw new CORBASystemExceptionWrapper(se);
            }
            ]]>
         </xsl:if>
     
         <![CDATA[  
         // Create the ComponentServer
         try {
             com.thalesgroup.CdmwCcmComponentServer.ComponentServer theComponentServer = 
                 cdmw.ccm.componentserver.InitUtils.createComponentServer(
                     orb,
                     cdmwRootPOA,
                     eventChannelFactory,
                     processName,
                     applicationName,
                     args);
             // Register it as cdmw.common.locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN/
             //     processName@applicationName.ComponentServer
             String id = processName + "@" + applicationName;
             String kind = "ComponentServer";
             org.omg.CosNaming.NameComponent[] name = new org.omg.CosNaming.NameComponent[1];
             name[0] = new org.omg.CosNaming.NameComponent();
             name[0].id   = id;
             name[0].kind = kind;

             String key = NamingInterface.toString(name);

             com.thalesgroup.CdmwNamingAndRepository.NameDomain domain =
                 RepositoryInterface.getDomain(Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN);

             try {
                 domain.register_new_factory(key, theComponentServer);
             } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
                 // Re-register it again.
                 domain.release_name(key);
                 domain.register_new_factory(key, theComponentServer);
             } 
         } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
             throw new CORBASystemExceptionWrapper(
                 new org.omg.CORBA.BAD_PARAM(
                     ExceptionMinorCodes.BAD_PARAMNameDomainNotFound,
                     org.omg.CORBA.CompletionStatus.COMPLETED_NO),
                 Locations.ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN + " is not a NameDomain!");
         } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
             throw new CORBASystemExceptionWrapper(
                 new org.omg.CORBA.INTERNAL(
                     ExceptionMinorCodes.INTERNAL,
                     org.omg.CORBA.CompletionStatus.COMPLETED_NO));
         } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
             throw new CORBASystemExceptionWrapper(
                 new org.omg.CORBA.INTERNAL(
                     ExceptionMinorCodes.INTERNAL,
                     org.omg.CORBA.CompletionStatus.COMPLETED_NO));
         } catch(cdmw.namingandrepository.InvalidNameException ine) {
             throw new CORBASystemExceptionWrapper(
                 new org.omg.CORBA.INTERNAL(
                     ExceptionMinorCodes.INTERNAL,
                     org.omg.CORBA.CompletionStatus.COMPLETED_NO));
         } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.NotRegistered nr) {
             throw new CORBASystemExceptionWrapper(
                 new org.omg.CORBA.INTERNAL(
                     ExceptionMinorCodes.INTERNAL,
                     org.omg.CORBA.CompletionStatus.COMPLETED_NO));
         } catch (org.omg.CORBA.SystemException se) {
             throw new CORBASystemExceptionWrapper(se);
         }
         ]]>
      </xsl:if>
   </xsl:if>
   
   <![CDATA[
      // Discard requests until run order comes from the platform management
      try {
          poaManager.discard_requests(false);
      } catch (
          org.omg.PortableServer.POAManagerPackage.AdapterInactive ai) {
          // TODO: Add trace logs here
          throw new org.omg.CORBA.INTERNAL(
              ExceptionMinorCodes.INTERNALORBAdapterInactive,
              org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
   
      // advise processImpl that cdmwInit is done
      processImpl.setCdmwInitDone();
    ]]>

      } catch (CORBASystemExceptionWrapper csew) {
   <xsl:choose>
      <xsl:when test="$hasTrace">
      <![CDATA[
          String domain = TraceDomains.CDMW_INIT;
          short level = TraceLevels.FTL;

          if (FlushAreaMngr.isToBeTraced(domain, level)) {
              Stream stream = Stream.getInstance();
              // filename and line number are avalaible in J2SDK 1.4
              stream.setLine((short) 0);
              stream.setFile("unknown");
              stream.setUserDomain(domain);
              stream.setUserLevel(level);
              stream.doFlush();
          } else { // In case there is a problem in the trace library itself!
              System.err.println(
                  domain + " : Fatal Error : " + csew.toString());
          }
      ]]>
      </xsl:when>
      <xsl:otherwise>
      <![CDATA[
          System.err.println(
              "cdmwInit: Fatal Error: " + csew.toString());
      ]]>
      </xsl:otherwise>
   </xsl:choose>
   <![CDATA[
          csew.raiseSystemException();
      }
       
   }

   // CDMW Factory objects are deactivated at ORB shutdown.
   public static void cdmwCleanup(org.omg.CORBA.ORB orb) {
      Assert.check(orb != null);

      // Release all static object references held by RepositoryInterface
      RepositoryInterface.finish();
   ]]>

      <xsl:if test="$hasHomeImpl='true'">

         // Unregister HomeAllocators
         <xsl:for-each select="$frameworkNodes">
            <xsl:variable name="frameworkIdref" select="@idref"/>

            <!--
               Get the matching node /cdmw/lifecycle-frameworks/lifecycle-framework.
            -->
            <xsl:variable name="lifecycleFrameworkNode" select="$lifecycleFrameworkNodes[@id = $frameworkIdref]"/>
            <xsl:variable name="isHomeImpl" select="boolean($lifecycleFrameworkNode/home-impl)"/>

            <xsl:if test="$isHomeImpl">
               <xsl:variable name="factoryClassname">
                  <xsl:call-template name="buildHomeImplClassname">
                     <xsl:with-param name="_homeImplNode" select="$lifecycleFrameworkNode/home-impl"/>
                  </xsl:call-template>
               </xsl:variable>

               HomeAllocatorRegistry.unregister("<xsl:value-of select="$factoryClassname"/>");
            </xsl:if>
         </xsl:for-each>
      </xsl:if>

   <xsl:if test="$hasTrace">
      // Free all allocated memory and stop the trace service
      FlushAreaMngr.cleanup();
   </xsl:if>

   <xsl:if test="$hasPerformance">
      // Free all allocated memory and stop the performance service
      try {
         PerformanceMngr.cleanup();
      } catch (BadParameterException bpe) {
         // Do nothing (ignore)
         // TODO: Add trace logs here
      }
   </xsl:if>
   }

}

   <!--
      The code generation ends here.
   -->

</xsl:template> <!-- end of template cdmw_interface_java.content -->



</xsl:stylesheet>


