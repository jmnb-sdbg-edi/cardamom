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
   Main - This templates fetches the necessary data from inputs
   to generate a file.

   @param none
-->
<xsl:template name="entity_factory_java">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   
   <!--
      References to some nodes.
   -->
   <xsl:variable name="nodes" select="$lifecycleFrameworkNodes[factory-impl/@category = 'ENTITY']"/>

   <xsl:if test="boolean($nodes)">
      <xsl:if test="$_index &lt;= count($nodes)">
         <xsl:variable name="lifecycleFrameworkNode" select="$nodes[$_index]"/>

         <xsl:variable name="factoryNode" select="$factoryNodes[@idl-interface = $lifecycleFrameworkNode/@factory-idref]"/>
         <xsl:variable name="servantProviderNode" select="$factoryNode/servant-provider"/>
         <xsl:variable name="servantProviderAllocatorNode" select="$lifecycleFrameworkNode/factory-impl/servant-provider-allocator"/>

         <!--
            Get the (scoped) idl interface name.
         -->
         <xsl:variable name="scopedInterfaceName" select="$factoryNode/@idl-interface"/>

         <!--
            Get the factory impl classname.
         -->
         <xsl:variable name="scopedFactoryImplClassname" select="$lifecycleFrameworkNode/factory-impl/@classname"/>
         <xsl:variable name="factoryImplClassname">
            <xsl:call-template name="getLastToken">
               <xsl:with-param name="_string" select="$scopedFactoryImplClassname"/>
               <xsl:with-param name="_separator" select="$xmlSep"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the factory impl allocator classname.
            By convention a factory allocator classname is the name of the factory + "Allocator" 
            TODO: Use a user-provided name rather than a naming convention!
         -->
         <xsl:variable name="factoryImplAllocatorClassname" select="concat($factoryImplClassname, 'Allocator')"/>
         
         <!--
            Get the (scoped) servant provider classname.
         -->
         <xsl:variable name="scopedServantProviderClassname" select="$servantProviderNode/@classname"/>

         <!--
            Get the (scoped) user servant provider allocator classname.
         -->
         <xsl:variable name="scopedUserServantProviderAllocatorClassname" select="$servantProviderAllocatorNode/@classname"/>

         <!--
            Get the user servant provider allocator filename.
         -->
         <xsl:variable name="userServantProviderAllocatorFilename">
            <xsl:call-template name="addFileExtension">
               <xsl:with-param name="_filename">
                  <xsl:choose>
                     <xsl:when test="boolean($servantProviderAllocatorNode/@filename)">
                        <xsl:value-of select="$servantProviderAllocatorNode/@filename"/>
                     </xsl:when>
                     <xsl:otherwise>
                        <xsl:call-template name="getLastToken">
                           <xsl:with-param name="_string" select="$scopedUserServantProviderAllocatorClassname"/>
                           <xsl:with-param name="_separator" select="$xmlSep"/>
                        </xsl:call-template>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:with-param>
               <xsl:with-param name="_extension" select="'.java'"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the output filename.
         -->
         <xsl:variable name="filename">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$lifecycleFrameworkNode/factory-impl/@directory"/>
               <!-- in Java, the filename must match the classname in order to compile -->
               <xsl:with-param name="_userFilename" select="$factoryImplClassname"/>
               <xsl:with-param name="_defaultFilename" select="'EntityResourceFactoryImpl'"/>
               <xsl:with-param name="_fileExtension" select="'.java'"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the output filename for factory allocator.
         -->
         <xsl:variable name="allocatorFilename">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$lifecycleFrameworkNode/factory-impl/@directory"/>
                <!-- in Java, the filename must match the classname in order to compile -->
               <xsl:with-param name="_userFilename" select="$factoryImplAllocatorClassname"/>
               <xsl:with-param name="_defaultFilename" select="'UnMngEntityResourceFactoryImplAllocator'"/>
               <xsl:with-param name="_fileExtension" select="'.java'"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the servant provider filename.
         -->
         <xsl:variable name="servantProviderFilename">
            <xsl:call-template name="addFileExtension">
               <xsl:with-param name="_filename">
                  <xsl:variable name="spFilename">
                     <xsl:choose>
                        <xsl:when test="boolean($servantProviderNode/@filename)">
                           <xsl:value-of select="$servantProviderNode/@filename"/>
                        </xsl:when>
                        <xsl:otherwise>
                           <xsl:call-template name="getLastToken">
                              <xsl:with-param name="_string" select="$scopedServantProviderClassname"/>
                              <xsl:with-param name="_separator" select="$xmlSep"/>
                           </xsl:call-template>
                        </xsl:otherwise>
                     </xsl:choose>
                  </xsl:variable>
                  <xsl:choose>
                     <xsl:when test="boolean($servantProviderNode/@directory)">
                        <xsl:value-of select="concat($servantProviderNode/@directory, $fileSep, $spFilename)"/>
                     </xsl:when>
                     <xsl:otherwise>
                        <xsl:value-of select="$spFilename"/>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:with-param>
               <xsl:with-param name="_extension" select="'.java'"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Display a message to the user.
         -->
         <xsl:message>
            <xsl:value-of select="concat('Generating file ', $filename)"/>
         </xsl:message>

         <!--
            Generate file.
         -->
         <xsl:document method="text" href="{$filename}">
            <!--
               Insert document header.
            -->
            <xsl:call-template name="docHeader">
               <xsl:with-param name="_lang" select="'java'"/>
            </xsl:call-template>

            <!--
               Insert the content.
            -->
            <xsl:call-template name="entity_factory_java.content">
               <xsl:with-param name="_interfaceName">
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="$scopedInterfaceName"/>
                     <xsl:with-param name="_separator" select="$xmlSep"/>
                  </xsl:call-template>
               </xsl:with-param>
               <xsl:with-param name="_scopedServantProviderClassname" select="$scopedServantProviderClassname"/>
               <xsl:with-param name="_servantProviderFilename" select="$servantProviderFilename"/>
               <xsl:with-param name="_scopedUserServantProviderAllocatorClassname" select="$scopedUserServantProviderAllocatorClassname"/>
               <xsl:with-param name="_userServantProviderAllocatorFilename" select="$userServantProviderAllocatorFilename"/>
               <xsl:with-param name="_factoryImplClassname" select="$factoryImplClassname"/>
               <xsl:with-param name="_scopedFactoryImplClassname" select="$scopedFactoryImplClassname"/>
               <xsl:with-param name="_isMultithread" select="boolean($lifecycleFrameworkNode/factory-impl/@threading = 'MULTITHREAD')"/>
            </xsl:call-template>
         </xsl:document>

         <!--
            Display a message to the user.
         -->
         <xsl:message>
            <xsl:value-of select="concat('Generating file ', $allocatorFilename)"/>
         </xsl:message>

         <!--
            Generate factory allocator file.
         -->
         <xsl:document method="text" href="{$allocatorFilename}">
            <!--
               Insert document header.
            -->
            <xsl:call-template name="docHeader">
               <xsl:with-param name="_lang" select="'java'"/>
            </xsl:call-template>

            <!--
               Insert the content.
            -->
            <xsl:call-template name="entity_factory_allocator_java.content">
               <xsl:with-param name="_factoryImplAllocatorClassname" select="$factoryImplAllocatorClassname"/>
               <xsl:with-param name="_scopedFactoryImplClassname" select="$scopedFactoryImplClassname"/>
            </xsl:call-template>
         </xsl:document>

         <!--
            Proceed to the next item if any.
         -->
         <xsl:call-template name="entity_factory_java">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template entity_factory_java.xslt -->

<!--
   This template generates the content of the file.

   @param _interfaceName                                 The named interface name.
   @param _scopedServantProviderClassname                The scoped servant provider classname.
   @param _servantProviderFilename                       The servant provider filename.
   @param _scopedUserServantProviderAllocatorClassname   The scoped user servant provider allocator classname.
   @param _userServantProviderAllocatorFilename          The user servant provider allocator filename.
   @param _factoryImplClassname                          The factory impl classname.
   @param _scopedFactoryImplClassname                    The scoped factory impl classname.
   @param _isMultithread                                 Multithread flag.
-->
<xsl:template name="entity_factory_java.content">
   <xsl:param name="_interfaceName"/>
   <xsl:param name="_scopedServantProviderClassname"/>
   <xsl:param name="_servantProviderFilename"/>
   <xsl:param name="_scopedUserServantProviderAllocatorClassname"/>
   <xsl:param name="_userServantProviderAllocatorFilename"/>
   <xsl:param name="_factoryImplClassname"/>
   <xsl:param name="_scopedFactoryImplClassname"/>
   <xsl:param name="_isMultithread"/>
   
   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="javaScopedServantProviderClassname">
      <xsl:value-of select="$_scopedServantProviderClassname"/>
   </xsl:variable>
   <xsl:variable name="javaScopedUserServantProviderAllocatorClassname">
      <xsl:value-of select="$_scopedUserServantProviderAllocatorClassname"/>
   </xsl:variable>
   <xsl:variable name="javaFactoryImplClassname">
      <xsl:value-of select="$_factoryImplClassname"/>
   </xsl:variable>

   <!--
      The code generation starts here.
   -->

<xsl:variable name="package">
   <xsl:call-template name="getScope">
      <xsl:with-param name="_name" select="$javaScopedServantProviderClassname"/>
      <xsl:with-param name="_lang" select="'java'"/>
   </xsl:call-template>
</xsl:variable>

package <xsl:value-of select="$package"/>;

import cdmw.common.Assert;
import cdmw.common.Locations;
import cdmw.common.BadParameterException;

import cdmw.orbsupport.ORBSupport;
import cdmw.orbsupport.StrategyList;
import cdmw.orbsupport.PolicyList;
import cdmw.orbsupport.ExceptionMinorCodes;

import cdmw.commonsvcs.naming.NamingInterface;
import cdmw.namingandrepository.RepositoryInterface;

import cdmw.lifecycle.RetainObjectDeactivator;
import cdmw.lifecycle.EntityCleanupNotifier;
import cdmw.lifecycle.POAThreadingPolicy;
import cdmw.lifecycle.ObjectRegistrationGuard;
import cdmw.lifecycle.ServantProviderBase;
import cdmw.lifecycle.ObjectUtil;

   <xsl:variable name="interfaceScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_interfaceName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>

public class <xsl:value-of select="$javaFactoryImplClassname"/>
    extends <xsl:value-of select="concat($interfaceScope, $javaSep, $_interfaceName)"/>POA {

    /**
     * Maximum number of retries on receiving TRANSIENT exception
     * for registering objects.
     */
    public final static int MAX_RETRIES = 1;

    /**
     * PropertySet Name
     */
    public final static String ENTITY_PROPERTY_SET_NAME = "PropertySet.object";

    private org.omg.CORBA.ORB orb;

    private org.omg.PortableServer.POA poa;

    private org.omg.PortableServer.POA objectsPOA;

    private com.thalesgroup.CdmwNamingAndRepository.NameDomain domain;

    private String domainName;

    private String factoryName;

    private RetainObjectDeactivator objectDeactivator;

    private <xsl:value-of select="$javaScopedServantProviderClassname"/> servantProvider;

    private EntityCleanupNotifier cleanupNotifier;
    
    // ----------------------------------------------------------------------
    // Resource Servant activator
    // ----------------------------------------------------------------------

    public class TheServantActivator
        extends org.omg.CORBA.LocalObject
        implements org.omg.PortableServer.ServantActivator {

        private ServantProviderBase servantProvider;

        public TheServantActivator(ServantProviderBase provider) {
            this.servantProvider = provider;
            Assert.check(this.servantProvider != null);
        }

      <![CDATA[
        public org.omg.PortableServer.Servant incarnate(
            byte[] oid,
            org.omg.PortableServer.POA poa)
            throws org.omg.PortableServer.ForwardRequest {
            Assert.check(this.servantProvider != null);
            return servantProvider.lookupServant(oid);
        }

        public void etherealize(
            byte[] oid,
            org.omg.PortableServer.POA poa,
            org.omg.PortableServer.Servant servant,
            boolean cleanupInProgress,
            boolean remainingActivations) {
            Assert.check(this.servantProvider != null);
            if (!remainingActivations) {
                this.servantProvider.releaseServant(oid, poa, servant);
            }
        }
      ]]>
    }

    /**
     * Constructor
     *
     * @param orb
     * @param parent
     * @param nameDomain
     * @param nameDomainName
     * @param factoryName
     * @param threadingPolicy
     * @param args
     *
     * @exception BadParameterException
     * @exception org.omg.CORBA.SystemException
     */
    public <xsl:value-of select="$javaFactoryImplClassname"/>(
        org.omg.CORBA.ORB orb,
        org.omg.PortableServer.POA parent,
        com.thalesgroup.CdmwNamingAndRepository.NameDomain nameDomain,
        String nameDomainName,
        String factoryName,
        POAThreadingPolicy threadingPolicy,
        String[] args)
        throws BadParameterException, org.omg.CORBA.SystemException {
   <![CDATA[
        this.orb = orb;
        this.poa = parent;
        this.objectsPOA = null;
        this.domain = nameDomain;
        this.domainName = nameDomainName;
        this.factoryName = factoryName;
        this.objectDeactivator = new RetainObjectDeactivator(orb);
        this.servantProvider = null;
        this.cleanupNotifier = null;

        //
        // Create POA for use by the factory
        //
        // Entity Life cycle framework use the following POA policies
        //
        // Lifespan policy            = PERSISTENT
        // Id Assignment policy       = USER_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = NO_IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_SERVANT_MANAGER;
        // Servant Retention policy   = RETAIN;
        //

        org.omg.PortableServer.POAManager poaManager =
            this.poa.the_POAManager();
        PolicyList policies = new PolicyList();
        policies.insert(
            parent.create_id_assignment_policy(
                org.omg.PortableServer.IdAssignmentPolicyValue.USER_ID));
        policies.insert(
            parent.create_lifespan_policy(
                org.omg.PortableServer.LifespanPolicyValue.PERSISTENT));
        policies.insert(
            parent.create_servant_retention_policy(
                org.omg.PortableServer.ServantRetentionPolicyValue.RETAIN));
        policies.insert(
            parent.create_id_uniqueness_policy(
                org.omg.PortableServer.IdUniquenessPolicyValue.UNIQUE_ID));
        policies.insert(
            parent.create_request_processing_policy(
                org.omg.PortableServer.RequestProcessingPolicyValue.USE_SERVANT_MANAGER));
        policies.insert(
            parent.create_implicit_activation_policy(
                org.omg.PortableServer.ImplicitActivationPolicyValue.NO_IMPLICIT_ACTIVATION));

        StrategyList poaStrategies = new StrategyList();
   ]]>

   <xsl:choose>
      <!--
         MULTITHREAD.
      -->
      <xsl:when test="$_isMultithread">
        // Strategies with MULTITHREAD policy will accept Single Thread and
        // Multi Thread strategies. However, SERIALIZE policy will only allow
        // Single Thread policy.
        threadingPolicy.appendToStrategyList(poaStrategies);
      </xsl:when>
      <!--
         SERIALIZE.
      -->
      <xsl:otherwise>
         <![CDATA[
            // For Single Thread policy, first check validity of 'threading_policy' using 
            // the code below : Begin Threading policy check
    
            // Check Multi-Threading Policy
            try {
               cdmw.lifecycle.SingleThreadPolicy singleThread
                  = (cdmw.lifecycle.SingleThreadPolicy) threadingPolicy;
               singleThread.appendToStrategyList(poaStrategies);
            } catch (ClassCastException cce) {
               // Wrong policy
               String parameter = "cdmw.lifecycle.POAThreadingPolicy(" + factoryName + ")";
               throw new BadParameterException(parameter, threadingPolicy.getClass().getName());
            }
            // End Threading policy check
         ]]>
      </xsl:otherwise>
   </xsl:choose>
   
        try {
            objectsPOA =
                ORBSupport.createPOA(
                    poa,
                    factoryName,
                    poaManager,
                    policies,
                    poaStrategies);
            //
            // Create and install servant provider
            //
            <xsl:value-of select="$javaScopedUserServantProviderAllocatorClassname"/> allocator =
                new  <xsl:value-of select="$javaScopedUserServantProviderAllocatorClassname"/>();
            cdmw.lifecycle.ServantProviderBase srvprvd = 
                allocator.allocate(orb,
                                   objectsPOA,
                                   objectDeactivator,
                                   args);
            Assert.check(srvprvd != null);
            try {
                servantProvider = (<xsl:value-of select="$javaScopedServantProviderClassname"/>) srvprvd;
            } catch (ClassCastException e) {
                // Shouldn't happen since  <xsl:value-of select="$javaScopedUserServantProviderAllocatorClassname"/>.allocate()
                // must return a <xsl:value-of select="$javaScopedServantProviderClassname"/>
                Assert.check(false);
            }

            // Hand Shaking with the Servant Provider
            // IDs should be unique
            if (servantProvider.mayIncarnateMultipleObjects()) {
                String parameter = "ObjectId Uniqueness(" + factoryName + ")";
                throw new BadParameterException(parameter);
            }

            cleanupNotifier = new EntityCleanupNotifier(domain);

            // Get the property set "CDMW/SERVICES/PropertySet.object"
            // (CDMW Developers should not rely on this functionality as this
            // is only needed for persisting EntityCleanupNotifier objects.
            // This may change in future CDMW versions)
            org.omg.CosPropertyService.PropertySet propertySet = null;
            try {
                NamingInterface sni =
                    RepositoryInterface.getDomainNamingInterface(
                        Locations.CDMW_SERVICES_NAME_DOMAIN);

                org.omg.CORBA.Object obj =
                    sni.resolve(ENTITY_PROPERTY_SET_NAME);
                propertySet =
                    org.omg.CosPropertyService.PropertySetHelper.narrow(obj);

            } catch (
                com.thalesgroup.CdmwNamingAndRepository.NoNameDomain nnd) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_YES);
            } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_YES);
            } catch (org.omg.CosNaming.NamingContextPackage.NotFound nf) {
                // TODO: Echo warning
            } catch (org.omg.CosNaming.NamingContextPackage.CannotProceed cp) {
                // TODO: Echo warning
            } catch (org.omg.CosNaming.NamingContextPackage.InvalidName invn) {
                // TODO: Echo warning
            }

            if (!cleanupNotifier.init(propertySet, factoryName)) {
                // TODO: Echo warning
            }

            servantProvider.setCleanupNotifier(cleanupNotifier);

            //
            // Create and install servant activator
            //
            objectsPOA.set_servant_manager(
                new TheServantActivator(servantProvider));

        } catch (BadParameterException bpe) {
            throw bpe;
        } catch (org.omg.PortableServer.POAPackage.AdapterAlreadyExists aae) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        } catch (org.omg.PortableServer.POAPackage.InvalidPolicy ip) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        } catch (org.omg.PortableServer.POAPackage.WrongPolicy ip) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALLifeCycleFrameworkError,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        }

    }

    /**
     * Returns the default POA.
     *
     * @return The default POA.
     */
    public org.omg.PortableServer.POA _default_POA() {
        return this.poa;
    }

   <!--
      Create the operation declarations.
   -->
   <xsl:call-template name="findAllJavaFactoryOperations">
      <xsl:with-param name="_interfaceName" select="$_interfaceName"/>
      <xsl:with-param name="_template" select="'entity_factory_java.content.1'"/>
      <xsl:with-param name="_isNamed" select="true()"/>
      <xsl:with-param name="_factoryImplClassname" select="$_factoryImplClassname"/>
   </xsl:call-template>
    
    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
    //
    /**
     * Tests if the servant provider supports an object
     * represented by its repositoryId
     *
     * @param repositoryId
     * @return True if the servant provider supports the object otherwise false.
     */
    public boolean supports(String repositoryId) {
        return this.servantProvider.supports(repositoryId);
    }

    //
    // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
    //
    /**
     * Returns the supported objects by the servant provider
     *
     * @return List of the supported objects as a String table
     */
    public String[] get_supported_objects() {
        return this.servantProvider.getSupportedObjects();
    }
    
    /**
     * Register the name objectKey within the NameDomain. This
     * will check validity of the name and whether it is already in use.
     *
     * @param objectKey
     * @exception com.thalesgroup.CdmwLifeCycle.AlreadyExistant
     * @exception org.omg.CORBA.SystemException
     * @exception com.thalesgroup.CdmwLifeCycle.InvalidName
     * @return Registration ID as a org.omg.CORBA.Any object
     */
    private org.omg.CORBA.Any registerName(String objectKey)
        throws
            com.thalesgroup.CdmwLifeCycle.AlreadyExistant,
            org.omg.CORBA.SystemException,
            com.thalesgroup.CdmwLifeCycle.InvalidName {

        org.omg.CORBA.Any regId;
        try {
            regId = domain.new_name(objectKey);
        } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
            // Add more information to help the caller in debuging
            String reason = domainName + "/" + objectKey;
            throw new com.thalesgroup.CdmwLifeCycle.AlreadyExistant(reason);
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName in) {
            throw new com.thalesgroup.CdmwLifeCycle.InvalidName(in.reason);
        } catch (org.omg.CORBA.OBJECT_NOT_EXIST one) {
            // Oops! CDMW Repository has been destroyed
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALCdmwRepositoryNotExists,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        } catch (org.omg.CORBA.SystemException se) {
            // Cannot reach the CDMW Repository at the moment, retry later!
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        return regId;

    }

    /**
     * Register (name,object) within the repository. The name has already been
     * registered, so this should succeed. In case of a TRANSIENT system
     * exception, there will be a retry max_retries times.
     * If max_retries == 0, then this will retry forever.
     *
     * @param regId
     * @param theObject
     * @param maxRetries
     * @exception org.omg.CORBA.SystemException
     */
    private void registerObject(
        org.omg.CORBA.Any regId,
        org.omg.CORBA.Object theObject,
        int maxRetries)
        throws org.omg.CORBA.SystemException {
<![CDATA[
        boolean forever = (maxRetries <= 0);
        int counter = maxRetries;

        do {
            try {
                domain.register_object(regId, theObject);
                break;
            } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.AlreadyExists ae) {
                // Registration will be cancelled by the ObjectRegistrationGuard
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_YES);
            } catch (com.thalesgroup.CdmwNamingAndRepository.NameDomainPackage.InvalidRegistration ir) {
                // Registration will be cancelled by the ObjectRegistrationGuard
                throw new org.omg.CORBA.INTERNAL(
                    ExceptionMinorCodes.INTERNALCdmwRepositoryError,
                    org.omg.CORBA.CompletionStatus.COMPLETED_YES);
            } catch (org.omg.CORBA.TRANSIENT t) {
                // Retry
            } catch (org.omg.CORBA.SystemException se) {
                // Cancel registration ????
                throw se;
            }
        } while (forever || (--counter > 0));

        if (!forever && (counter <= 0)) {
            throw new org.omg.CORBA.TRANSIENT(
                ExceptionMinorCodes.TRANSIENTCannotReachRepository,
                org.omg.CORBA.CompletionStatus.COMPLETED_YES);
        }

    }

}
]]>
   <!--
      The code generation stops here.
   -->
   
</xsl:template> <!-- end of template entity_factory_java.content -->


<!--
   This template generates the content of the file.

   @param _factoryImplAllocatorClassname                 The factory allocator classname.
   @param _scopedFactoryImplClassname                    The factory classname with full scope.
-->
<xsl:template name="entity_factory_allocator_java.content">
   <xsl:param name="_factoryImplAllocatorClassname"/>
   <xsl:param name="_scopedFactoryImplClassname"/>

   <!--
      The code generation starts here.
   -->
<xsl:variable name="package">
   <xsl:call-template name="getScope">
      <xsl:with-param name="_name" select="$_scopedFactoryImplClassname"/>
      <xsl:with-param name="_lang" select="'java'"/>
   </xsl:call-template>
</xsl:variable>

package <xsl:value-of select="$package"/>;


import cdmw.lifecycle.NamedObjectsFactoryAllocator;


public class <xsl:value-of select="$_factoryImplAllocatorClassname"/> 
    extends NamedObjectsFactoryAllocator
{

    public <xsl:value-of select="$_factoryImplAllocatorClassname"/>() {
        super("<xsl:value-of select="$_scopedFactoryImplClassname"/>");
    }

    public static final <xsl:value-of select="$_factoryImplAllocatorClassname"/> theAllocator =
        new <xsl:value-of select="$_factoryImplAllocatorClassname"/>();

}

</xsl:template> <!-- end of template entity_factory_allocator_java.content -->



<xsl:template name="entity_factory_java.content.1">
    <xsl:param name="_node"/>
    <xsl:param name="_returnType"/>
    <xsl:param name="_operationName"/>
    <xsl:param name="_factoryImplClassname"/>

    <xsl:value-of select="concat('public ', $_returnType, ' ', $_operationName, '(', $lf, '        ')"/>

            <!--
               IN parameters.
            -->
            <xsl:for-each select="$_node/UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'in']">
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
                  <xsl:value-of select="concat(',', $lf, '        ')"/>
               </xsl:if>
            </xsl:for-each>
         <xsl:value-of select="concat(')', $lf, '        throws', $lf)"/>
         <!--
            Exceptions.
         -->
         <xsl:for-each select="$_node/UML:ModelElement.taggedValue/UML:TaggedValue">
            <xsl:if test="@tag = 'raises'">
               <xsl:variable name="scopedException">
                  <xsl:call-template name="getScopedException">
                     <xsl:with-param name="_exception" select="@value"/>
                  </xsl:call-template>
               </xsl:variable>
               <xsl:value-of select="concat('            ', $scopedException, ',', $lf)"/>
            </xsl:if>
         </xsl:for-each>
        <xsl:value-of select="concat('            org.omg.CORBA.SystemException {', $lf, $lf, '        ')"/>

        <xsl:value-of select="$_returnType"/> ref = null;
        byte[] oid;
        // [I] Register the name object_key within the NameDomain. This
        // will check validity of the name and whether it is already in use.
        org.omg.CORBA.Any regId = registerName(object_key);
        // Object's name has been registered successfuly. From now on,
        // it should be unregistered if an exception has to be rethrown
        ObjectRegistrationGuard unreg =
            new ObjectRegistrationGuard(domain, object_key);
        try {
            // [II] Acquire oid from user for object_key
            // Operation may throw user exceptions that will rethrown to caller
            oid = servantProvider.<xsl:value-of select="@name"/>(
            <xsl:for-each select="$_node/UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'in' and not(@type = $keyNodeId)]">
               <xsl:value-of select="@name"/>
               <xsl:if test="position() != last()">
                  <xsl:text>, </xsl:text>
               </xsl:if>
            </xsl:for-each>
            );
            // [III] Activate the object immediately :
            //       [a] Acquire a servant from user for oid
            //       [b] Activate the object (using a servant activator)
            //       [c] Create the object reference
            //       [d] Assert the object is really of the expected type
            org.omg.CORBA.Object obj =
                ObjectUtil.activateObject(objectsPOA, oid, servantProvider);
            ref = <xsl:value-of select="concat($_returnType, 'Helper')"/>.narrow(obj);
            // [IV] Register (name,object) within the repository. In case of
            // an exception ObjectRegistrationGuard will try to unregister
            // the name, even if there a high probability
            // that this may not work.
            registerObject(regId, ref, MAX_RETRIES);
            // [V] Return the object reference
            // Everything is OK, so we "do not rollback" the registration
            unreg.cancel();
        } finally {
            unreg.destroy();
        }

        // [VI] Insert entry to the cleanup notifier if object is removable
        try {
            org.omg.CosLifeCycle.LifeCycleObject removable =
                org.omg.CosLifeCycle.LifeCycleObjectHelper.narrow(ref);
            if (removable != null) {
                cleanupNotifier.insertEntry(oid, object_key);
            }
        } catch (org.omg.CORBA.BAD_PARAM bp) {}

        return ref;

    }
    

</xsl:template>



</xsl:stylesheet>


