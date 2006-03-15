<?xml version="1.0" encoding="utf-8"?>
<!-- ===================================================================== -->
<!--
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
-->
<!-- ===================================================================== -->


<xsl:stylesheet version="1.1"
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   xmlns:UML="org.omg/UML1.3">



<!--
   Main - This template fetches the necessary data from inputs
   to generate a file.

   @param none
-->
<xsl:template name="service_component_java">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   
   <xsl:if test="boolean($lifecycleFrameworkNodes/home-impl) and $lifecycleFrameworkNodes/home-impl/@category = 'SERVICE'">
      <xsl:if test="$_index &lt;= count($lifecycleFrameworkNodes/home-impl)">
         <xsl:variable name="homeImplNode" select="$lifecycleFrameworkNodes[home-impl and position() = $_index]/home-impl"/>

         <!--
            Get the managed component.
         -->
         <xsl:variable name="homeName">
            <xsl:call-template name="getLastToken">
               <xsl:with-param name="_string" select="$homeImplNode/../@factory-idref"/>
               <xsl:with-param name="_separator" select="$xmlSep"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:variable name="managedComponentId">
            <xsl:call-template name="getManagedComponentId">
               <xsl:with-param name="_homeName" select="$homeName"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:variable name="managedComponentNode" select="key('classById', $managedComponentId)"/>

         <xsl:if test="boolean($managedComponentNode)">
            <xsl:variable name="componentImplClassname" select="concat('Service', $managedComponentNode/@name, 'Impl')"/>

            <!--
               Get the output filename.
            -->
            <xsl:variable name="filename">
               <xsl:call-template name="buildOutputFilename">
                  <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                  <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                  <xsl:with-param name="_defaultFilename" select="$componentImplClassname"/>
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
               <xsl:call-template name="service_component_java.content">
                  <xsl:with-param name="_scopedHomeImplClassname">
                     <xsl:call-template name="buildHomeImplClassname">
                        <xsl:with-param name="_homeImplNode" select="$homeImplNode"/>
                     </xsl:call-template>
                  </xsl:with-param>
                  <xsl:with-param name="_componentName" select="$managedComponentNode/@name"/>
               </xsl:call-template>
            </xsl:document>
         </xsl:if>

         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="service_component_java">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_component_java.content -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _facetName                 The component name.
-->
<xsl:template name="service_component_java.content">
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_componentName"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="javaScopedComponentName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_componentName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="javaComponentScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_componentName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="componentImplClassname" select="concat('Service', $_componentName, 'Impl')"/>
   <xsl:variable name="componentObjRefVar" select="concat($javaComponentScope, $javaSep, 'CCM_', $_componentName)"/>
   <xsl:variable name="hasFacets">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMProvides'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasReceptacles">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMUses'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasEmitters">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMEmits'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasPublishers">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMPublishes'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasConsumers">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMConsumes'"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      The code generation starts here.
   -->

<xsl:variable name="package">
   <xsl:call-template name="getScope">
      <xsl:with-param name="_name" select="$_scopedHomeImplClassname"/>
      <xsl:with-param name="_lang" select="'java'"/>
   </xsl:call-template>
</xsl:variable>
package <xsl:value-of select="$package"/>;

import cdmw.common.AlreadyDoneException;
import cdmw.common.InternalErrorException;
import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.ccm.cif.CCMObjectImpl;

   /**
    * Service component
    */

public class Service<xsl:value-of select="$_componentName"/>Impl extends CCMObjectImpl
    implements <xsl:value-of select="$javaScopedComponentName"/>Operations {

    public static final String COMP_REP_ID = "<xsl:value-of select="$componentNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>";
    public static final String COMP_NAME   = "<xsl:value-of select="$_componentName"/>";

      <!--
         Facets.
      -->
      <xsl:call-template name="findAllJavaComponentFacets">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'service_component_java.content.9'"/>
         <xsl:with-param name="_strategy" select="'Service'"/>
      </xsl:call-template>

      <!--
         Receptacles.
      -->
      <xsl:call-template name="findAllJavaComponentReceptacles">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'service_component_java.content.13'"/>
         <xsl:with-param name="_strategy" select="'Service'"/>
      </xsl:call-template>
      
      <!--
         Publishers.
      -->

      <xsl:call-template name="findAllJavaComponentPublishers">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'service_component_java.content.15'"/>
         <xsl:with-param name="_strategy" select="'Service'"/>
      </xsl:call-template>
      
      <!--
         Emitters.
      -->
      
      <xsl:call-template name="findAllJavaComponentEmitters">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'service_component_java.content.16'"/>
         <xsl:with-param name="_strategy" select="'Service'"/>
      </xsl:call-template>

      <!--
         Consumers.
      -->

      <xsl:call-template name="findAllJavaComponentConsumers">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'service_component_java.content.19'"/>
         <xsl:with-param name="_strategy" select="'Service'"/>
      </xsl:call-template>

    /**
     * If a home, when creating a component, returns an EnterpriseComponent, we map it 
     * in an ExecutorLocator for code optimisation purpose. Thus the code is the same
     * for monolithic executor and segmented executor.
     */
    private class <xsl:value-of select="$_componentName"/>ExecutorLocator 
        extends org.omg.CORBA.LocalObject
        implements com.thalesgroup.CdmwCcmCif.ServiceExecutorLocator {

        /**
         * indicates that the component executor inherits from the ServiceComponent local interface.
         */
        private boolean isServiceComponent = false;

        /**
         * points on Component monolithic executor.
         */
        private <xsl:value-of select="$componentObjRefVar"/> compExecutor;

        public <xsl:value-of select="$_componentName"/>ExecutorLocator(<xsl:value-of select="$componentObjRefVar"/> executor) {

            this.compExecutor = executor;
            try  {
                org.omg.Components.ServiceComponent serviceComp
                    = org.omg.Components.ServiceComponentHelper.narrow(this.compExecutor);
            
                if (serviceComp != null) {
                    isServiceComponent = true;
                }
            } catch (Exception e) {}
        }

         /**
          * the cif will invoke its obtain_executor operation prior to each invocation to
          * retrieve the implementation for a port. The port name, given in the name
          * parameter, is the same as used in the components interface description 
          * in IDL, or the components name for the main executor. The obtain_executor
          * operation returns a local object reference of the expected type. The 
          * CCMException exception may be raised in case of a system error that 
          * prohibits locating the requested executor.
          */
         //
         // IDL:omg.org/Components/ExecutorLocator/obtain_executor:1.0
         //
         public org.omg.CORBA.Object obtain_executor(String name)
            throws org.omg.Components.CCMException {

            String execName = name;
            org.omg.CORBA.Object result = null;

            if (execName.equals("<xsl:value-of select="$_componentName"/>")) {
               result = this.compExecutor;
            }
            <xsl:call-template name="findAllJavaComponentFacets">
               <xsl:with-param name="_componentName" select="$_componentName"/>
               <xsl:with-param name="_template" select="'service_component_java.content.10'"/>
               <xsl:with-param name="_strategy" select="'Service'"/>
            </xsl:call-template>
            <xsl:call-template name="findAllJavaComponentConsumers">
               <xsl:with-param name="_componentName" select="$_componentName"/>
               <xsl:with-param name="_template" select="'service_component_java.content.20'"/>
               <xsl:with-param name="_strategy" select="'Service'"/>
            </xsl:call-template>
            else {
               throw new org.omg.CORBA.BAD_PARAM(
                   ExceptionMinorCodes.BAD_PARAMInvalidExecutorName, 
                   org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }

            return result;
         }

         /** 
          * The release_executor operation is called by the cif once the current
          * invocation on an executor that was obtained through the obtain_executor 
          * operation has finished. The locator can thus release any resources that
          * were acquired as part of the obtain_executor operation.
          */
         //
         // IDL:omg.org/Components/ExecutorLocator/release_executor:1.0
         //
         public void release_executor(org.omg.CORBA.Object exc) 
            throws org.omg.Components.CCMException {}

         /** 
          * The configuration_complete operation is called to propagate the
          * configuration_complete operation on the CCMObject interface to the
          * component implementation.
          */
         //
         // IDL:omg.org/Components/ExecutorLocator/configuration_complete:1.0
         //
         public void configuration_complete() 
            throws org.omg.Components.InvalidConfiguration {
            // No equivalent operation for monolithic executor
         }

         //
         // IDL:omg.org/Components/ServiceComponent/set_service_context:1.0
         //
         public void set_service_context(org.omg.Components.ServiceContext ctx)
            throws org.omg.Components.CCMException {
            // check if component implements the ServiceComponent local interface
            if (isServiceComponent) {
               org.omg.Components.ServiceComponent serviceComp 
                  = org.omg.Components.ServiceComponentHelper.narrow(this.compExecutor);
               serviceComp.set_service_context(ctx);
            }
         }

         /** 
          * The ccm_activate operation is called by the container to notify a 
          * service component that it has been made active. The component instance
          * should perform any initialization required prior to operation invocation.
          * The component may raise the CCMException with the SYSTEM_ERROR minor code
          * to indicate a failure caused by a system level error.
          */
         //
         // IDL:omg.org/Components/ServiceComponent/ccm_activate:1.0
         //
         public void ccm_activate() throws org.omg.Components.CCMException {
            // check if component implements the ServiceComponent local interface
            if (isServiceComponent) {
               org.omg.Components.ServiceComponent serviceComp 
                  = org.omg.Components.ServiceComponentHelper.narrow(this.compExecutor);
               serviceComp.ccm_activate();
            }
         }

         /** 
          * The ccm_passivate operation is called by the container to notify a service
          * component that it has been made inactive. The component instance should
          * release any resources it acquired at activation time.The component may 
          * raise the CCMException with the SYSTEM_ERROR minor code to indicate a 
          * failure caused by a system level error.
          */
         //
         // IDL:omg.org/Components/ServiceComponent/ccm_passivate:1.0
         //
         public void ccm_passivate() throws org.omg.Components.CCMException {
            // check if component implements the ServiceComponent local interface
            if (isServiceComponent) {
               org.omg.Components.ServiceComponent serviceComp 
                  = org.omg.Components.ServiceComponentHelper.narrow(this.compExecutor);
               serviceComp.ccm_passivate();
            }
         }

         /**
          * The ccm_remove operation is called by the container when the servant is
          * about to be destroyed. It informs the component that it is about to be
          * destroyed.The component may raise the CCMException with the SYSTEM_ERROR
          * minor code to indicate a failure caused by a system level error.
          */
         //
         // IDL:omg.org/Components/ServiceComponent/ccm_remove:1.0
         //
         public void ccm_remove() throws org.omg.Components.CCMException {
            // check if component implements the ServiceComponent local interface
            if (isServiceComponent) {
               org.omg.Components.ServiceComponent serviceComp 
                  = org.omg.Components.ServiceComponentHelper.narrow(this.compExecutor);
               serviceComp.ccm_remove();
            }
         }
     }

    /**
     * Attributes
     */
   
    /**
     * If a home, in creating a component, returns an ExecutorLocator, the cif
     * will invoke its obtain_executor operation prior to each invocation to
     * retrieve the implementation for a port. If the home returns an
     * EnterpriseComponent, we encapsulate it in an executor locator in order to
     * use same code.
     *
     */
   
    /**
     * <xsl:value-of select="$_componentName"/> executor locator used to get component exec and facet exec
     */
    private org.omg.Components.ExecutorLocator executorLocator;
    
    /**
     * executor locator must a service component to be advise 
     * of its activation, passivation and removal
     */
    private org.omg.Components.ServiceComponent serviceLocator;
   
   <xsl:variable name="contextExecutorClassName">
      <xsl:variable name="tmpClassName">
         <xsl:call-template name="prefixClassName">
            <xsl:with-param name="_prefix" select="'CCM_'"/>
            <xsl:with-param name="_scopedName">
               <xsl:call-template name="prefixModuleName">
                  <xsl:with-param name="_prefix" select="'Cdmw_'"/>
                  <xsl:with-param name="_idlPrefix" select="$idlPragmaPrefix"/>
                  <xsl:with-param name="_scopedName" select="$javaScopedComponentName"/>
               </xsl:call-template>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($tmpClassName, '_ServiceContext')"/>
    </xsl:variable>
    /**
     * <xsl:value-of select="$_componentName"/> context executor
     */
    private <xsl:value-of select="$contextExecutorClassName"/> contextExecutor;
   
    /**
     * Indicates if current component has been created via a factory operation
     */
    private boolean isCreatedByFactoryOperation;
   
   <xsl:call-template name="findAllJavaInterfaceReadWriteAttributes">
      <xsl:with-param name="_interfaceName" select="$_componentName"/>
      <xsl:with-param name="_type" select="'Component'"/>
      <xsl:with-param name="_template" select="'service_component_java.content.4'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

    /**
     * Constructor
     */
    public Service<xsl:value-of select="$_componentName"/>Impl(
        String compOid,
        com.thalesgroup.CdmwCcmContainer.CCM2Context ctx,
        org.omg.Components.EnterpriseComponent executor,
        boolean isCreatedByFactoryOperation)
        throws org.omg.Components.CCMException {

        super(compOid, ctx, COMP_REP_ID);
        this.isCreatedByFactoryOperation = isCreatedByFactoryOperation;

        System.out.println("<xsl:value-of select="$componentImplClassname"/> constructor called for oid: " + compOid);
      
        // Check if the home returns an executor locator
        try {
            this.executorLocator = org.omg.Components.ExecutorLocatorHelper.narrow(executor);
            this.serviceLocator = org.omg.Components.ServiceComponentHelper.narrow(this.executorLocator);

            if (this.serviceLocator == null) {
   			    System.err.println("Executor Locator should be a service component!");
   			    throw new org.omg.CORBA.BAD_PARAM(
                  ExceptionMinorCodes.BAD_PARAMInvalidComponentExecutor,
                  org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
        } catch(org.omg.CORBA.BAD_PARAM bp) {
             // executor is not an executor locator
             // map monolithic executor to executor locator
             <xsl:value-of select="$componentObjRefVar"/> compExec = <xsl:value-of select="concat($javaComponentScope,    $javaSep, 'CCM_',  $_componentName)"/>Helper.narrow(executor);
           
             if (compExec == null) {
                 System.err.println("Invalid Component Executor");
                 throw new org.omg.CORBA.BAD_PARAM(
                     ExceptionMinorCodes.BAD_PARAMInvalidComponentExecutor,
   			           org.omg.CORBA.CompletionStatus.COMPLETED_NO);
             }
             this.executorLocator = new <xsl:value-of select="$_componentName"/>ExecutorLocator(compExec);
             this.serviceLocator = org.omg.Components.ServiceComponentHelper.narrow(this.executorLocator);
         
             if (this.serviceLocator == null) {
                 System.err.println("Executor Locator should be a service component!");
                 throw new org.omg.CORBA.INTERNAL(
                     ExceptionMinorCodes.INTERNALInvalidExecutor,
                     org.omg.CORBA.CompletionStatus.COMPLETED_NO);
		        }
        }

        this.contextExecutor = new CCM_<xsl:value-of select="$_componentName"/>_ServiceContextImpl(ctx, this);

      <xsl:call-template name="findAllJavaComponentFacets">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'service_component_java.content.11'"/>
         <xsl:with-param name="_strategy" select="'Service'"/>
      </xsl:call-template>

      <xsl:if test="$hasReceptacles = 'true'">
         try {
            <xsl:call-template name="findAllJavaComponentReceptacles">
               <xsl:with-param name="_componentName" select="$_componentName"/>
               <xsl:with-param name="_template" select="'service_component_java.content.14'"/>
               <xsl:with-param name="_strategy" select="'Service'"/>
            </xsl:call-template>
         } catch (AlreadyDoneException ade) {
            System.err.println("Receptacle already declared!");
            // Don't propagate while its probably a code generation error!
         <!-- //TODO: remove this catch statement if not needed
         } catch (InternalErrorException iee) {
            System.err.println("Internal exception raised when declaring receptacle!");
            throw new org.omg.CORBA.INTERNAL(
               ExceptionMinorCodes.INTERNAL, 
               org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         -->
         } catch (org.omg.CORBA.SystemException se) {
            System.err.println("System exception raised when declaring receptacle!" + se.toString());
            throw new org.omg.CORBA.INTERNAL(
               ExceptionMinorCodes.INTERNAL, 
               org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         }
      </xsl:if>

      <xsl:if test="$hasEmitters = 'true' or $hasPublishers = 'true'">
         try {
            <xsl:if test="$hasPublishers = 'true'">
               <xsl:call-template name="findAllJavaComponentPublishers">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'service_component_java.content.17'"/>
                  <xsl:with-param name="_strategy" select="'Service'"/>
               </xsl:call-template>
            </xsl:if>
            <xsl:if test="$hasEmitters = 'true'">
               <xsl:call-template name="findAllJavaComponentEmitters">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'service_component_java.content.18'"/>
                  <xsl:with-param name="_strategy" select="'Service'"/>
               </xsl:call-template>
            </xsl:if>
         } catch (AlreadyDoneException ade) {
            System.err.println("Event source already declared!");
            // Don't propagate while its probably a code generation error!
         } catch (org.omg.CORBA.SystemException se) {
            System.err.println("System exception raised when declaring event source!");
            System.err.println(se.toString());
            throw new org.omg.CORBA.INTERNAL(
               ExceptionMinorCodes.INTERNAL, 
               org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         }
      </xsl:if>

      <xsl:if test="$hasConsumers = 'true'">
         try {
            <xsl:call-template name="findAllJavaComponentConsumers">
               <xsl:with-param name="_componentName" select="$_componentName"/>
               <xsl:with-param name="_template" select="'service_component_java.content.21'"/>
               <xsl:with-param name="_strategy" select="'Service'"/>
            </xsl:call-template>
         } catch (AlreadyDoneException ade) {
            System.err.println("Consumer already declared!");
            // Don't propagate while its probably a code generation error!
         } catch (InternalErrorException iee) {
            System.err.println("Internal exception raised when declaring consumer!");
            throw new org.omg.CORBA.INTERNAL(
               ExceptionMinorCodes.INTERNAL, 
               org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         } catch (org.omg.CORBA.SystemException se) {
            System.err.println("System exception raised when declaring consumer!" + se.toString());
            throw new org.omg.CORBA.INTERNAL(
               ExceptionMinorCodes.INTERNAL, 
               org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         }
      </xsl:if>

      System.out.println("<xsl:value-of select="$componentImplClassname"/> constructor end");

    }

    /**
     * Component attributes
     */

   <xsl:call-template name="findAllJavaInterfaceReadWriteAttributes">
      <xsl:with-param name="_interfaceName" select="$_componentName"/>
      <xsl:with-param name="_type" select="'Component'"/>
      <xsl:with-param name="_template" select="'service_component_java.content.3'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

    /**
     * Supported interfaces
     */

   <!--
      Declare supported interfaces operations.
   -->
   <xsl:variable name="generalizations" select="key('generalizationById', $componentNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>
   <xsl:for-each select="$generalizations">
      <xsl:sort select="@xmi.id"/>
         
      <xsl:variable name="interfaceNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:variable name="containedOps"  select="key('operations', $interfaceNode/@xmi.id)"/>

      <xsl:for-each select="$containedOps">
         <!--
            Get the operation return type.
         -->
         <xsl:variable name="returnType">
            <xsl:call-template name="getType">
               <xsl:with-param name="_id" select="UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'return']/@type"/>
               <xsl:with-param name="_kind" select="'return'"/>
               <xsl:with-param name="_lang" select="'java'"/>
            </xsl:call-template>
         </xsl:variable>


         <!--
            Operation signature.
         -->
         <xsl:variable name="javaOperationName">
            <xsl:call-template name="mapJavaReservedWords">
               <xsl:with-param name="_operationName" select="@name"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:value-of select="concat('public ', $returnType, $lf, $javaOperationName, '(', $lf)"/>
               <!--
                  IN, OUT and INOUT parameters.
               -->
               <xsl:for-each select="UML:BehavioralFeature.parameter/UML:Parameter[@kind != 'return']">
                  <!--
                     Get the parameter type.
                  -->
                  <xsl:variable name="paramType">
                     <xsl:call-template name="getParameterType">
                        <xsl:with-param name="_paramNode" select="."/>
                        <xsl:with-param name="_lang" select="'java'"/>
                     </xsl:call-template>
                  </xsl:variable>
                  <xsl:value-of select="concat($paramType, ' ', @name)"/>
                  <!--
                     Write out comma if necessary.
                  -->
                  <xsl:if test="position() != last()">
                     <xsl:value-of select="concat(',', $lf)"/>
                  </xsl:if>
               </xsl:for-each>
            <xsl:value-of select="'    )'"/>
            <!--
               Exceptions.
            -->
            <xsl:if test="count(UML:ModelElement.taggedValue/UML:TaggedValue[@tag='raises']) > 0">
               <xsl:value-of select="concat(' throws', $lf)"/>
            </xsl:if>
            <xsl:for-each select="UML:ModelElement.taggedValue/UML:TaggedValue">
               <xsl:if test="@tag = 'raises'">
                  <xsl:variable name="scopedException">
                     <xsl:call-template name="getScopedException">
                        <xsl:with-param name="_exception" select="@value"/>
                     </xsl:call-template>
                  </xsl:variable>
                  <xsl:value-of select="concat('        ', $scopedException)"/>
                  <xsl:if test="position() != last()">
                     <xsl:value-of select="concat(',', $lf)"/>
                  </xsl:if>
               </xsl:if>
            </xsl:for-each>
            <xsl:value-of select="concat(' {', $lf)"/>
            
            <xsl:value-of select="$componentObjRefVar"/> compExec = obtain_component_executor();
            <xsl:if test="not($returnType = 'void')">
               <xsl:value-of select="concat($returnType, ' result = ')"/>
            </xsl:if> 
            compExec.<xsl:value-of select="$javaOperationName"/>(
            <xsl:for-each select="UML:BehavioralFeature.parameter/UML:Parameter[@kind != 'return']">
               <xsl:value-of select="@name"/>
               <xsl:if test="position() != last()">
                  <xsl:text>, </xsl:text>
               </xsl:if>
            </xsl:for-each>
            );            

            try {
                this.executorLocator.release_executor(compExec);
            } catch(org.omg.Components.CCMException ccme) {
               throw new org.omg.CORBA.INTERNAL(
                  ExceptionMinorCodes.INTERNALCCMCIFError,
                  org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
            <xsl:if test="not($returnType = 'void')">
            return result;
            </xsl:if>
         }
      </xsl:for-each>
   </xsl:for-each>

   /**
    * Equivalent IDL methods
    */

   // Facets

   <xsl:call-template name="findAllJavaComponentFacets">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'service_component_java.content.1'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

   // Receptacles

   <xsl:call-template name="findAllJavaComponentReceptacles">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'service_component_java.content.2'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

   // Consumers

   <xsl:call-template name="findAllJavaComponentConsumers">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'service_component_java.content.5'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

   // Publishers

   <xsl:call-template name="findAllJavaComponentPublishers">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'service_component_java.content.6'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>
   
   // Emitters

   <xsl:call-template name="findAllJavaComponentEmitters">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'service_component_java.content.7'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

   /**
    * Inherited CCMObject interface implemented by CCMObjectImpl
    */

   public void configuration_complete()
      throws org.omg.Components.InvalidConfiguration {
      
      if (this.configured == false) {
         if (!this.isCreatedByFactoryOperation) {
            <xsl:call-template name="findAllJavaInterfaceReadWriteAttributes">
               <xsl:with-param name="_interfaceName" select="$_componentName"/>
               <xsl:with-param name="_type" select="'Component'"/>
               <xsl:with-param name="_template" select="'service_component_java.content.8'"/>
               <xsl:with-param name="_strategy" select="'Service'"/>
            </xsl:call-template>
         }
         
         super.configuration_complete();
        
         org.omg.Components.ServiceComponent serviceLocator 
            = org.omg.Components.ServiceComponentHelper.narrow(this.executorLocator);
        
         try {
             serviceLocator.set_service_context(this.contextExecutor);
             this.executorLocator.configuration_complete();
             serviceLocator.ccm_activate();
         } catch(org.omg.Components.CCMException ccme) {
             System.err.println("CCMException raised, reason: " + ccme.toString());
             throw new org.omg.CORBA.INTERNAL(
                 ExceptionMinorCodes.INTERNAL,
                 org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         }

      }
      
   }

   /**
    * Inherited Events interface implemented by CCMObjectImpl
    */

   <![CDATA[
      /**
       * This returns the servant for the facet specified by facetName.
       * This operation is implemented by the well typed component servant.
       *
       * @param facetName The facet name
       *
       * @return the facet servant.
       */
   ]]>
   <xsl:choose>
      <xsl:when test="$hasFacets = 'true' or $hasConsumers = 'true'">
         public org.omg.PortableServer.Servant get_facet_servant(String facetName) {
            String facet = facetName;
            org.omg.PortableServer.Servant facetServant = null;
          
            <xsl:if test="$hasFacets = 'true'">
               <xsl:call-template name="findAllJavaComponentFacets">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'service_component_java.content.12'"/>
                  <xsl:with-param name="_strategy" select="'Service'"/>
               </xsl:call-template>
            </xsl:if>
            <xsl:if test="$hasConsumers = 'true'">
               <xsl:call-template name="findAllJavaComponentConsumers">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'service_component_java.content.22'"/>
                  <xsl:with-param name="_strategy" select="'Service'"/>
               </xsl:call-template>
            </xsl:if>
            {
               // No facet/consumer with this name define for that component
               throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                  ExceptionMinorCodes.OBJECT_NOT_EXIST,
                  org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
          
            return facetServant;
         }
      </xsl:when>
      <xsl:otherwise>
         public org.omg.PortableServer.Servant get_facet_servant(String facetName) {
            // No facet with this name define for that component
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
               ExceptionMinorCodes.OBJECT_NOT_EXIST,
               org.omg.CORBA.CompletionStatus.COMPLETED_NO); 
         }
      </xsl:otherwise>
   </xsl:choose>

   // Return the component executor
   protected <xsl:value-of select="$javaComponentScope"/>.CCM_<xsl:value-of select="$_componentName"/>
   obtain_component_executor() {
      org.omg.CORBA.Object obj = null;
      try {
         obj = this.executorLocator.obtain_executor(COMP_NAME);
         
         if (obj == null) {
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                ExceptionMinorCodes.OBJECT_NOT_EXISTInvalidExecutor,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         }

         <xsl:value-of select="$componentObjRefVar"/> compExec = <xsl:value-of select="concat($javaComponentScope, $javaSep, 'CCM_', $_componentName)"/>Helper.narrow(obj);

         if (compExec == null) {
            throw new org.omg.CORBA.OBJECT_NOT_EXIST(
               ExceptionMinorCodes.OBJECT_NOT_EXISTInvalidExecutor, 
               org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         }

         return compExec;
      } catch(org.omg.Components.CCMException ccme) {
         System.err.println("CCMException raised, reason: " + ccme.toString());
         throw new org.omg.CORBA.OBJECT_NOT_EXIST(
            ExceptionMinorCodes.OBJECT_NOT_EXISTInvalidExecutor,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch(org.omg.CORBA.BAD_PARAM bp) {
         System.err.println(bp.toString());
         throw new org.omg.CORBA.OBJECT_NOT_EXIST(
            ExceptionMinorCodes.OBJECT_NOT_EXISTInvalidExecutor,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
   }

   // Call ccm_passivate and ccm_remove on executor
   protected void advise_executor_of_remove_component() {
      org.omg.Components.ServiceComponent serviceLocator 
         = org.omg.Components.ServiceComponentHelper.narrow(this.executorLocator);

      try {
         serviceLocator.ccm_passivate();
         serviceLocator.ccm_remove();
      } catch(org.omg.Components.CCMException ccme) {
         System.err.println("CCMException raised, reason: " + ccme.toString());
         throw new org.omg.CORBA.INTERNAL(
            ExceptionMinorCodes.INTERNAL,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
   }

}

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template service_component_java.content -->



<!--
-->
<xsl:template name="service_component_java.content.1">
   <xsl:param name="_facetTypeName"/>
   <xsl:param name="_facetName"/>
   
   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_facetTypeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'public ', $scopedReturnType, ' provide_', $_facetName, '()', $lf, '{', $lf)"/>
      org.omg.CORBA.Object obj;

      try {
         obj = super.provide_facet("<xsl:value-of select="$_facetName"/>");
      } catch (org.omg.Components.InvalidName in) {
         throw new org.omg.CORBA.OBJECT_NOT_EXIST(
            ExceptionMinorCodes.OBJECT_NOT_EXIST,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }

      if (obj == null) {
         throw new org.omg.CORBA.OBJECT_NOT_EXIST(
            ExceptionMinorCodes.OBJECT_NOT_EXIST, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }

      <xsl:value-of select="$scopedReturnType"/> facetRef = <xsl:value-of select="$scopedReturnType"/>Helper.narrow(obj);
       
      if (facetRef == null) {
         throw new org.omg.CORBA.OBJECT_NOT_EXIST(
            ExceptionMinorCodes.OBJECT_NOT_EXIST, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
       
      return facetRef;
   }
</xsl:template> <!-- end of template service_component_java.content.1 -->



<!--
-->
<xsl:template name="service_component_java.content.2">
   <xsl:param name="_receptacleTypeName"/>
   <xsl:param name="_receptacleName"/>
   
   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_receptacleTypeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="uppercaseReceptacleName" select="translate($_receptacleName, $lcase, $ucase)"/>
   
   <xsl:value-of select="concat($lf, 'public void connect_',  $_receptacleName, '(', $scopedReturnType, ' conxn)', 
      $lf, 'throws org.omg.Components.AlreadyConnected, org.omg.Components.InvalidConnection {', $lf)"/>
      try {
          super.connect(RECEP_<xsl:value-of select="$uppercaseReceptacleName"/>_NAME, conxn);
      } catch(org.omg.Components.InvalidName in) {
         throw new org.omg.CORBA.INTERNAL(
             ExceptionMinorCodes.INTERNALCCMCIFError,
             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch(org.omg.Components.ExceededConnectionLimit ecl) {
         throw new org.omg.CORBA.INTERNAL(
             ExceptionMinorCodes.INTERNALCCMCIFError,
             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
   }
   
   <xsl:value-of select="concat($lf, 'public ', $scopedReturnType, ' disconnect_', $_receptacleName, '()',
      $lf, 'throws org.omg.Components.NoConnection {', $lf)"/>
      <xsl:value-of select="$scopedReturnType"/> result = get_connection_<xsl:value-of select="$_receptacleName"/>();
      try {
          super.disconnect(RECEP_<xsl:value-of select="$uppercaseReceptacleName"/>_NAME, null);
      } catch(org.omg.Components.InvalidName in) {
         throw new org.omg.CORBA.INTERNAL(
             ExceptionMinorCodes.INTERNALCCMCIFError,
             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch(org.omg.Components.InvalidConnection ic) {
         throw new org.omg.CORBA.INTERNAL(
             ExceptionMinorCodes.INTERNALCCMCIFError,
             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch(org.omg.Components.CookieRequired cr) {
         throw new org.omg.CORBA.INTERNAL(
             ExceptionMinorCodes.INTERNALCCMCIFError,
             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
      return result;
   }
   
   <xsl:value-of select="concat($lf, 'public ', $scopedReturnType, ' get_connection_', $_receptacleName, '() {', $lf)"/>
      org.omg.Components.ConnectionDescription[] compDesc = null;

      try {
         compDesc = super.get_connections(RECEP_<xsl:value-of select="$uppercaseReceptacleName"/>_NAME);
      } catch (org.omg.Components.InvalidName in) {
         throw new org.omg.CORBA.INTERNAL(
            ExceptionMinorCodes.INTERNAL, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch (org.omg.CORBA.SystemException se) {
         throw se;
      }
      
      <xsl:value-of select="$scopedReturnType"/> result = null;
      if (compDesc.length == 1) {
         org.omg.CORBA.Object obj = compDesc[0].objref;
         result = <xsl:value-of select="$scopedReturnType"/>Helper.narrow(obj);
      }
      
      return result;
   }

</xsl:template> <!-- end of template service_component_java.content.2 -->



<!--
-->
<xsl:template name="service_component_java.content.3">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramName"/>
   <xsl:param name="_paramType"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_javaOperationName"/>
   <xsl:param name="_componentObjRefVar"/>
   <xsl:param name="_objRefVarType"/>

   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:value-of select="concat($lf, 'public ', $_returnType, ' ', $_javaOperationName, '() {', $lf)"/>
         <xsl:value-of select="$_componentObjRefVar"/> compExec = obtain_component_executor();
         <xsl:value-of select="$_objRefVarType"/> result = compExec.<xsl:value-of select="$_javaOperationName"/>();

         try {
             this.executorLocator.release_executor(compExec);
         } catch(org.omg.Components.CCMException ccme) {
             throw new org.omg.CORBA.INTERNAL(
                 ExceptionMinorCodes.INTERNALCCMCIFError,
                 org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         }
         return result;
      }

      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         <xsl:value-of select="concat($lf, 'public void ', $_javaOperationName, '(', $_paramType, ' ', $_paramName, ') {', $lf)"/>
            <xsl:value-of select="$_componentObjRefVar"/> compExec = obtain_component_executor();
             compExec.<xsl:value-of select="concat($_javaOperationName, '(', $_paramName, ');')"/>

             try {
                 this.executorLocator.release_executor(compExec);
             } catch(org.omg.Components.CCMException ccme) {
                 throw new org.omg.CORBA.INTERNAL(
                     ExceptionMinorCodes.INTERNALCCMCIFError,
                     org.omg.CORBA.CompletionStatus.COMPLETED_NO);
             }

             // set boolean to true (needed for configuration_completed)
             this.is<xsl:value-of select="$_paramName"/>Configured = true;
         }
      </xsl:if>
   </xsl:if>
   
</xsl:template> <!-- end of template service_component_java.content.3 -->



<!--
-->
<xsl:template name="service_component_java.content.4">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramName"/>
   
   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         <xsl:value-of select="concat($lf, 'private boolean is', $_paramName, 'Configured = false;', $lf)"/>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_component_java.content.4 -->



<!--
-->
<xsl:template name="service_component_java.content.5">
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerName"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_consumerTypeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:value-of select="concat($lf, 'public ', $scopedReturnType, 'Consumer',
                                $lf, 'get_consumer_', $_consumerName, '()',
                                $lf, '   {', $lf)"/>

      org.omg.CORBA.Object obj = null;

      try {
         obj = super.get_consumer(CONSUMER_<xsl:value-of select="translate($_consumerName, $lcase, $ucase)"/>_NAME);
      } catch (org.omg.Components.InvalidName in) {
         throw new org.omg.CORBA.OBJECT_NOT_EXIST(
            ExceptionMinorCodes.OBJECT_NOT_EXIST,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }

      if (obj == null) {
         throw new org.omg.CORBA.OBJECT_NOT_EXIST(
            ExceptionMinorCodes.OBJECT_NOT_EXIST, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }

      <xsl:value-of select="concat($scopedReturnType, 'Consumer consumerRef = ',
                                   $scopedReturnType, 'ConsumerHelper.narrow(obj);')"/>

      if (consumerRef == null) {
         throw new org.omg.CORBA.OBJECT_NOT_EXIST(
            ExceptionMinorCodes.OBJECT_NOT_EXIST, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
       
      return consumerRef;
   }
</xsl:template> <!-- end of template service_component_java.content.5 -->



<!--
-->
<xsl:template name="service_component_java.content.6">
   <xsl:param name="_publisherTypeName"/>
   <xsl:param name="_publisherName"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_publisherTypeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="uppercasePublisherName" select="translate($_publisherName, $lcase, $ucase)"/>

   <xsl:value-of select="concat($lf, 'public org.omg.Components.Cookie subscribe_', $_publisherName, '(', $scopedReturnType, 'Consumer consumer)', $lf, 'throws org.omg.Components.ExceededConnectionLimit {', $lf)"/>
      org.omg.Components.Cookie result = null;
      
      try {
          result = super.subscribe(PUBLISHER_<xsl:value-of select="$uppercasePublisherName"/>_NAME, consumer);
      } catch (org.omg.Components.InvalidName in) {
          System.err.println("Invalid name for publisher subscription!");
          throw new org.omg.CORBA.INTERNAL(
             ExceptionMinorCodes.INTERNAL, 
             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch (org.omg.Components.InvalidConnection ic) {
          // should never appear!
          throw new org.omg.CORBA.INTERNAL(
             ExceptionMinorCodes.INTERNAL, 
             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch (org.omg.Components.ExceededConnectionLimit ecl) {
          throw ecl;
      } catch (org.omg.CORBA.SystemException se) {
          throw se;
      }
      
      return result;
   }

   <xsl:value-of select="concat($lf, 'public ', $scopedReturnType, 'Consumer unsubscribe_', $_publisherName, '(org.omg.Components.Cookie ck)', $lf, 'throws org.omg.Components.InvalidConnection {', $lf)"/>
      if (ck == null){
         throw new org.omg.CORBA.BAD_PARAM(
            ExceptionMinorCodes.BAD_PARAMNilObjectReference, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
       
      <xsl:value-of select="$scopedReturnType"/>Consumer result = null;
      try {
         org.omg.Components.EventConsumerBase consumer 
            = super.get_consumer(PUBLISHER_<xsl:value-of select="$uppercasePublisherName"/>_NAME, ck);

         result = <xsl:value-of select="$scopedReturnType"/>ConsumerHelper.narrow(consumer);

         super.unsubscribe(PUBLISHER_<xsl:value-of select="$uppercasePublisherName"/>_NAME, ck);
      } catch (org.omg.Components.InvalidName in) {
         // internal error should not appear
         throw new org.omg.CORBA.INTERNAL(
            ExceptionMinorCodes.INTERNAL, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch (org.omg.Components.InvalidConnection ic) {
         throw ic;
      } catch (org.omg.CORBA.SystemException se) {
         throw se;
      }

      return result;
   }
</xsl:template> <!-- end of template service_component_java.content.6 -->



<!--
-->
<xsl:template name="service_component_java.content.7">
   <xsl:param name="_emitterTypeName"/>
   <xsl:param name="_emitterName"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_emitterTypeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="uppercaseEmitterName" select="translate($_emitterName, $lcase, $ucase)"/>

   <xsl:value-of select="concat($lf, 'public void connect_', $_emitterName, '(', $scopedReturnType, 'Consumer consumer)',
                                $lf, 'throws org.omg.Components.AlreadyConnected {', $lf)"/>
      try {
         super.connect_consumer(EMITTER_<xsl:value-of select="$uppercaseEmitterName"/>_NAME, consumer);
      } catch (org.omg.Components.InvalidName in) {
         // can not appear!
         throw new org.omg.CORBA.INTERNAL(
            ExceptionMinorCodes.INTERNAL, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch (org.omg.Components.InvalidConnection ic) {
         // can not appear!
         throw new org.omg.CORBA.INTERNAL(
            ExceptionMinorCodes.INTERNAL, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      } catch (org.omg.CORBA.SystemException se) {
         throw se;
      } 
   }

   <xsl:value-of select="concat($lf, 'public ', $scopedReturnType, 'Consumer disconnect_', $_emitterName, '()',
                                $lf, 'throws org.omg.Components.NoConnection {', $lf)"/>
      try {
         org.omg.Components.EventConsumerBase consumer = 
            super.disconnect_consumer(EMITTER_<xsl:value-of select="$uppercaseEmitterName"/>_NAME);

         <xsl:value-of select="$scopedReturnType"/>Consumer result = 
            <xsl:value-of select="$scopedReturnType"/>ConsumerHelper.narrow(consumer);

         if (result == null) {
            // can not appear!
            throw new org.omg.CORBA.INTERNAL(
               ExceptionMinorCodes.INTERNAL, 
               org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         }

         return result;
      } catch (org.omg.Components.InvalidName in) {
         // can not appear!
         throw new org.omg.CORBA.INTERNAL(
            ExceptionMinorCodes.INTERNAL, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      <!-- //TODO: remove this catch statement if not needed
      } catch (org.omg.Components.InvalidConnection ic) {
         // can not appear!
         throw new org.omg.CORBA.INTERNAL(
            ExceptionMinorCodes.INTERNAL, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      -->
      } catch (org.omg.CORBA.SystemException se) {
         throw se;
      } 
   }

</xsl:template> <!-- end of template service_component_java.content.7 -->



<!--
-->
<xsl:template name="service_component_java.content.8">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramName"/>

   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         // check if the attribute have been initialised only if not created by a factory op
         if (!this.is<xsl:value-of select="$_paramName"/>Configured) {
            // component is not well configured
            System.err.println("Attribut <xsl:value-of select="$_paramName"/> not initialised!");
            throw new org.omg.Components.InvalidConfiguration();
         }
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_component_java.content.8 -->



<!--
-->
<xsl:template name="service_component_java.content.9">
   <xsl:param name="_facetNode"/>
   <xsl:param name="_facetName"/>

   <xsl:variable name="uppercaseFacetName" select="translate($_facetName, $lcase, $ucase)"/>
   
   <xsl:value-of select="concat($lf, 'public static final String FACET_', $uppercaseFacetName, '_NAME')"/> = "<xsl:value-of select="$_facetName"/>";
   <xsl:value-of select="concat($lf, 'public static final String FACET_', $uppercaseFacetName, '_REP_ID')"/> = "<xsl:value-of select="$_facetNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>";
</xsl:template> <!-- end of template service_component_java.content.9 -->



<!--
-->
<xsl:template name="service_component_java.content.10">
   <xsl:param name="_facetName"/>
   
   else if (execName.equals("<xsl:value-of select="$_facetName"/>")) {
      result = this.compExecutor.<xsl:value-of select="concat('get_', $_facetName, '();')"/>
   }
</xsl:template> <!-- end of template service_component_java.content.10 -->



<!--
-->
<xsl:template name="service_component_java.content.11">
   <xsl:param name="_facetTypeName"/>
   <xsl:param name="_facetName"/>
   
   <xsl:variable name="uppercaseFacetName" select="translate($_facetName, $lcase, $ucase)"/>

   try {
      org.omg.CORBA.Object <xsl:value-of select="$_facetName"/>_facet
         = declare_facet(compOid, 
                         FACET_<xsl:value-of select="$uppercaseFacetName"/>_NAME, 
                         FACET_<xsl:value-of select="$uppercaseFacetName"/>_REP_ID);
                         
      if (<xsl:value-of select="$_facetName"/>_facet == null) {
         System.err.println("Internal exception raised when declaring facet <xsl:value-of select="$_facetName"/>!");
         throw new org.omg.CORBA.INTERNAL(
            ExceptionMinorCodes.INTERNAL, 
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
   } catch (AlreadyDoneException ade) {
      System.err.println("Facet for_clients already declared!");
      // Don't propagate while its probably a code generation error!
   } catch (InternalErrorException iee) {
      System.err.println("Internal exception raised when declaring facet <xsl:value-of select="$_facetTypeName"/>!");
      throw new org.omg.CORBA.INTERNAL(
         ExceptionMinorCodes.INTERNAL, 
         org.omg.CORBA.CompletionStatus.COMPLETED_NO);
   }
</xsl:template> <!-- end of template service_component_java.content.11 -->



<!--
-->
<xsl:template name="service_component_java.content.12">
   <xsl:param name="_facetTypeName"/>
   <xsl:param name="_facetName"/>
   
   <xsl:variable name="uppercaseFacetName" select="translate($_facetName, $lcase, $ucase)"/>

   if (facet.equals(FACET_<xsl:value-of select="$uppercaseFacetName"/>_NAME)) {
      facetServant = new <xsl:value-of select="concat($_facetTypeName, 'Impl')"/>(
         this.executorLocator, 
         FACET_<xsl:value-of select="$uppercaseFacetName"/>_NAME,
         this.context,
         this);
   }
   else
</xsl:template> <!-- end of template service_component_java.content.12 -->



<!--
-->
<xsl:template name="service_component_java.content.13">
   <xsl:param name="_receptacleNode"/>
   <xsl:param name="_receptacleName"/>

   <xsl:variable name="uppercaseReceptacleName" select="translate($_receptacleName, $lcase, $ucase)"/>
   
   <xsl:value-of select="concat($lf, 'public static final String RECEP_', $uppercaseReceptacleName, '_NAME')"/> = "<xsl:value-of select="$_receptacleName"/>";
   <xsl:value-of select="concat($lf, 'public static final String RECEP_', $uppercaseReceptacleName, '_REP_ID')"/> = "<xsl:value-of select="$_receptacleNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>";
</xsl:template> <!-- end of template service_component_java.content.13 -->



<!--
-->
<xsl:template name="service_component_java.content.14">
   <xsl:param name="_receptacleName"/>
   
   <xsl:variable name="uppercaseReceptacleName" select="translate($_receptacleName, $lcase, $ucase)"/>

   declare_receptacle(<xsl:value-of select="concat('RECEP_', $uppercaseReceptacleName,'_NAME, RECEP_', $uppercaseReceptacleName,'_REP_ID')"/>);
</xsl:template> <!-- end of template service_component_java.content.14 -->



<!--
-->
<xsl:template name="service_component_java.content.15">
   <xsl:param name="_publisherNode"/>
   <xsl:param name="_publisherName"/>

   <xsl:variable name="uppercasePublisherName" select="translate($_publisherName, $lcase, $ucase)"/>
   <xsl:variable name="repId">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_publisherNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
         <xsl:with-param name="_charsIn" select="':1.0'"/>
         <xsl:with-param name="_charsOut" select="'Consumer:1.0'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'public static final String PUBLISHER_', $uppercasePublisherName, '_NAME')"/> = "<xsl:value-of select="$_publisherName"/>";
   <xsl:value-of select="concat($lf, 'public static final String PUBLISHER_', $uppercasePublisherName, '_REP_ID')"/> = "<xsl:value-of select="$repId"/>";
</xsl:template> <!-- end of template service_component_java.content.15 -->



<!--
-->
<xsl:template name="service_component_java.content.16">
   <xsl:param name="_emitterNode"/>
   <xsl:param name="_emitterName"/>

   <xsl:variable name="uppercaseEmitterName" select="translate($_emitterName, $lcase, $ucase)"/>
   <xsl:variable name="repId">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_emitterNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
         <xsl:with-param name="_charsIn" select="':1.0'"/>
         <xsl:with-param name="_charsOut" select="'Consumer:1.0'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'public static final String EMITTER_', $uppercaseEmitterName, '_NAME')"/> = "<xsl:value-of select="$_emitterName"/>";
   <xsl:value-of select="concat($lf, 'public static final String EMITTER_', $uppercaseEmitterName, '_REP_ID')"/> = "<xsl:value-of select="$repId"/>";
</xsl:template> <!-- end of template service_component_java.content.16 -->



<!--
-->
<xsl:template name="service_component_java.content.17">
   <xsl:param name="_publisherName"/>
   
   <xsl:variable name="uppercasePublisherName" select="translate($_publisherName, $lcase, $ucase)"/>

   declare_publisher(<xsl:value-of select="concat('PUBLISHER_', $uppercasePublisherName,'_NAME, PUBLISHER_', $uppercasePublisherName,'_REP_ID')"/>);
</xsl:template> <!-- end of template service_component_java.content.17 -->



<!--
-->
<xsl:template name="service_component_java.content.18">
   <xsl:param name="_emitterName"/>
   
   <xsl:variable name="uppercaseEmitterName" select="translate($_emitterName, $lcase, $ucase)"/>

   declare_emitter(<xsl:value-of select="concat('EMITTER_', $uppercaseEmitterName,'_NAME, EMITTER_', $uppercaseEmitterName,'_REP_ID')"/>);
</xsl:template> <!-- end of template service_component_java.content.18 -->



<!--
-->
<xsl:template name="service_component_java.content.19">
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerName"/>

   <xsl:variable name="uppercaseConsumerName" select="translate($_consumerName, $lcase, $ucase)"/>
   <xsl:variable name="repId">
      <xsl:variable name="partialRepId">
         <xsl:call-template name="replaceCharsInString">
            <xsl:with-param name="_stringIn">
               <xsl:call-template name="getScopedName">
                  <xsl:with-param name="_name" select="$_consumerTypeName"/>
               </xsl:call-template>
            </xsl:with-param>
            <xsl:with-param name="_charsIn" select="$javaSep"/>
            <xsl:with-param name="_charsOut" select="'/'"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat('IDL:', $idlPragmaPrefix, '/Cdmw_', $partialRepId, 'Consumer:1.0')"/>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'public static final String CONSUMER_', $uppercaseConsumerName, '_NAME')"/> = "<xsl:value-of select="$_consumerName"/>";
   <xsl:value-of select="concat($lf, 'public static final String CONSUMER_', $uppercaseConsumerName, '_REP_ID')"/> = "<xsl:value-of select="$repId"/>";
</xsl:template> <!-- end of template service_component_java.content.19 -->



<!--
-->
<xsl:template name="service_component_java.content.20">
   <xsl:param name="_consumerName"/>
   <xsl:param name="_componentName"/>

   <xsl:variable name="javaComponentScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_componentName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   
   else if (execName.equals(<xsl:value-of select="concat('CONSUMER_', translate($_consumerName, $lcase, $ucase), '_NAME')"/>)) {
      result = this.compExecutor;
   }
</xsl:template> <!-- end of template service_component_java.content.20 -->



<!--
-->
<xsl:template name="service_component_java.content.21">
   <xsl:param name="_consumerName"/>
   
   <xsl:variable name="uppercaseConsumerName" select="translate($_consumerName, $lcase, $ucase)"/>

   declare_consumer(compOid, <xsl:value-of select="concat('CONSUMER_', $uppercaseConsumerName,'_NAME, CONSUMER_', $uppercaseConsumerName,'_REP_ID')"/>);
</xsl:template> <!-- end of template service_component_java.content.21 -->



<!--
-->
<xsl:template name="service_component_java.content.22">
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerName"/>
   
   <xsl:variable name="uppercaseConsumerName" select="translate($_consumerName, $lcase, $ucase)"/>

   if (facet.equals(CONSUMER_<xsl:value-of select="$uppercaseConsumerName"/>_NAME)) {
      facetServant = new <xsl:value-of select="concat($_consumerTypeName, 'ConsumerImpl')"/>(
         this.executorLocator, 
         CONSUMER_<xsl:value-of select="$uppercaseConsumerName"/>_NAME,
         this.context,
         this);
   }
   else
</xsl:template> <!-- end of template service_component_java.content.22 -->



</xsl:stylesheet>


