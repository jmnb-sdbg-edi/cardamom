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
<xsl:template name="service_component_hpp">
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
            <xsl:variable name="componentImplClassname" select="concat('Service', $managedComponentNode/@name, '_impl')"/>

            <!--
               Get the output filename.
            -->
            <xsl:variable name="filename">
               <xsl:call-template name="buildOutputFilename">
                  <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                  <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                  <xsl:with-param name="_defaultFilename" select="$componentImplClassname"/>
                  <xsl:with-param name="_fileExtension" select="'.hpp'"/>
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
                  <xsl:with-param name="_lang" select="'cpp'"/>
               </xsl:call-template>
               <!--
                  Insert the #ifndef #define instructions.
               -->
               <xsl:call-template name="writeDefine">
                  <xsl:with-param name="_prefix" select="'CCMCIF'"/>
                  <xsl:with-param name="_classname" select="$componentImplClassname"/>
               </xsl:call-template>
               <!--
                  Insert the content.
               -->
               <xsl:call-template name="service_component_hpp.content">
                  <xsl:with-param name="_scopedHomeImplClassname">
                     <xsl:call-template name="buildHomeImplClassname">
                        <xsl:with-param name="_homeImplNode" select="$homeImplNode"/>
                     </xsl:call-template>
                  </xsl:with-param>
                  <xsl:with-param name="_componentName" select="$managedComponentNode/@name"/>
               </xsl:call-template>
               <!--
                  Insert the #endif instruction.
               -->
               <xsl:call-template name="writeDefine">
                  <xsl:with-param name="_prefix" select="'CCMCIF'"/>
                  <xsl:with-param name="_classname" select="$componentImplClassname"/>
                  <xsl:with-param name="_endif" select="true()"/>
               </xsl:call-template>
            </xsl:document>
         </xsl:if>

         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="service_component_hpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_component_hpp -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _facetName                 The component name.
-->
<xsl:template name="service_component_hpp.content">
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_componentName"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="cppScopedComponentName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_componentName"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppComponentScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_componentName"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      The code generation starts here.
   -->

   <xsl:variable name="pathPrefix">
       <xsl:choose>
           <xsl:when test="string-length($CSC_NAME) > 0">
               <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
           </xsl:when>
           <xsl:otherwise>
               <xsl:value-of select="''"/>
           </xsl:otherwise>
       </xsl:choose>
   </xsl:variable>
   
   #include "<xsl:value-of select="concat($pathPrefix, 'Cdmw_', substring-before($idl3Filename, '.idl') , '_cif')"/>.skel.hpp"
   #include "CCMContainer/ccmcif/CCMObject_impl.hpp"

   <!--
      Include all the contained facets.
   -->
   <xsl:call-template name="findAllComponentFacets">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'service_component_hpp.content.1'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   /**
    * Service component
    */

   class Service<xsl:value-of select="$_componentName"/>_impl : virtual public POA_<xsl:value-of select="$cppScopedComponentName"/>,
                                                                virtual public CCMObject_impl
   {
      public:
         Service<xsl:value-of select="$_componentName"/>_impl(const <![CDATA[std::string&]]> comp_oid,
                  CdmwCcmContainer::CCM2Context_ptr ctx,
                  Components::EnterpriseComponent* executor,
                  bool is_created_by_factory_operation)
            throw (Components::CCMException,
                   CORBA::SystemException);

         ~Service<xsl:value-of select="$_componentName"/>_impl();

         /**
          * Component attributes
          */

         <xsl:call-template name="findAllInterfaceReadWriteAttributes">
            <xsl:with-param name="_interfaceName" select="$_componentName"/>
            <xsl:with-param name="_type" select="'Component'"/>
            <xsl:with-param name="_template" select="'service_component_hpp.content.4'"/>
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
                  </xsl:call-template>
               </xsl:variable>

               <!--
                  Operation signature.
               -->
               <xsl:variable name="cppOperationName">
                  <xsl:call-template name="mapCppReservedWords">
                     <xsl:with-param name="_operationName" select="@name"/>
                  </xsl:call-template>
               </xsl:variable>
               <xsl:value-of select="concat('virtual ', $returnType, $lf, $cppOperationName, $lf)"/>
                  (
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
                  )
               throw (
                  <!--
                     Exceptions.
                  -->
                  <xsl:for-each select="UML:ModelElement.taggedValue/UML:TaggedValue">
                     <xsl:if test="@tag = 'raises'">
                        <xsl:value-of select="@value"/>,
                     </xsl:if>
                  </xsl:for-each>
                  CORBA::SystemException
               );
            </xsl:for-each>
         </xsl:for-each>

         /**
          * Equivalent IDL methods
          */

         // Facets

         <xsl:call-template name="findAllComponentFacets">
            <xsl:with-param name="_componentName" select="$_componentName"/>
            <xsl:with-param name="_template" select="'service_component_hpp.content.2'"/>
            <xsl:with-param name="_strategy" select="'Service'"/>
         </xsl:call-template>

         // Receptacles

         <xsl:call-template name="findAllComponentReceptacles">
            <xsl:with-param name="_componentName" select="$_componentName"/>
            <xsl:with-param name="_template" select="'service_component_hpp.content.3'"/>
            <xsl:with-param name="_strategy" select="'Service'"/>
         </xsl:call-template>

         // Consumers

         <xsl:call-template name="findAllComponentConsumers">
            <xsl:with-param name="_componentName" select="$_componentName"/>
            <xsl:with-param name="_template" select="'service_component_hpp.content.6'"/>
            <xsl:with-param name="_strategy" select="'Service'"/>
         </xsl:call-template>

         // Publishers

         <xsl:call-template name="findAllComponentPublishers">
            <xsl:with-param name="_componentName" select="$_componentName"/>
            <xsl:with-param name="_template" select="'service_component_hpp.content.7'"/>
            <xsl:with-param name="_strategy" select="'Service'"/>
         </xsl:call-template>
         
         // Emitters

         <xsl:call-template name="findAllComponentEmitters">
            <xsl:with-param name="_componentName" select="$_componentName"/>
            <xsl:with-param name="_template" select="'service_component_hpp.content.8'"/>
            <xsl:with-param name="_strategy" select="'Service'"/>
         </xsl:call-template>


         /**
          * Inherited CCMObject interface implemented by CCMObject_impl
          */

         virtual void
         configuration_complete()
            throw(Components::InvalidConfiguration,
                  CORBA::SystemException);

         /**
          * Inherited Events interface implemented by CCMObject_impl
          */

         <![CDATA[
            /**
             * Purpose:
             * <p>This returns the servant for the facet specified by facet_name.
             * This operation is implemented by the well typed component servant.
             *
             * @param facet_name The facet name
             *
             * @return the facet servant.
             */
            virtual PortableServer::Servant
            get_facet_servant(const std::string& facet_name)
               throw (CORBA::SystemException);
         ]]>

      protected:
         // Return the component executor
         <xsl:value-of select="$cppComponentScope"/>::CCM_<xsl:value-of select="$_componentName"/>_ptr
         obtain_component_executor()
            throw (CORBA::SystemException);

         // Call ccm_passivate and ccm_remove on executor
         void advise_executor_of_remove_component()
           throw (CORBA::SystemException);

      private:
         Service<xsl:value-of select="$_componentName"/>_impl(const Service<xsl:value-of select="$_componentName"/>_impl<![CDATA[&);]]>
         void operator=(const Service<xsl:value-of select="$_componentName"/>_impl<![CDATA[&);]]>

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

         // <xsl:value-of select="$_componentName"/> executor locator used to get component exec and facet exec
         Components::ExecutorLocator_var   m_executor_locator;

         // <xsl:value-of select="$_componentName"/> context executor
         Cdmw_<xsl:value-of select="$cppComponentScope"/>::CCM_<xsl:value-of select="$_componentName"/>_ServiceContext_var  m_context_executor;
         
         // Indicates if current component has been created via a factory operation
         bool m_is_created_by_factory_operation;

         <xsl:call-template name="findAllInterfaceReadWriteAttributes">
            <xsl:with-param name="_interfaceName" select="$_componentName"/>
            <xsl:with-param name="_type" select="'Component'"/>
            <xsl:with-param name="_template" select="'service_component_hpp.content.5'"/>
            <xsl:with-param name="_strategy" select="'Service'"/>
         </xsl:call-template>
   };

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template service_component_hpp.content -->



<!--
-->
<xsl:template name="service_component_hpp.content.1">
   <xsl:param name="_facetName"/>
   
   <xsl:variable name="pathPrefix">
       <xsl:choose>
           <xsl:when test="string-length($CSC_NAME) > 0">
               <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
           </xsl:when>
           <xsl:otherwise>
               <xsl:value-of select="''"/>
           </xsl:otherwise>
       </xsl:choose>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, '#include &quot;', $pathPrefix, $_facetName, '_impl.hpp&quot;')"/>
</xsl:template> <!-- end of template service_component_hpp.content.1 -->



<!--
-->
<xsl:template name="service_component_hpp.content.2">
   <xsl:param name="_facetTypeName"/>
   <xsl:param name="_facetName"/>
   
   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_facetTypeName"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'virtual ', $scopedReturnType, '_ptr provide_', $_facetName, '()', 
                                $lf, 'throw(CORBA::SystemException);', $lf)"/>
</xsl:template> <!-- end of template service_component_hpp.content.2 -->



<!--
-->
<xsl:template name="service_component_hpp.content.3">
   <xsl:param name="_receptacleTypeName"/>
   <xsl:param name="_receptacleName"/>
   
   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_receptacleTypeName"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'virtual void connect_',  $_receptacleName, '(', $scopedReturnType, '_ptr conxn)', 
                                $lf, 'throw(Components::AlreadyConnected, Components::InvalidConnection, CORBA::SystemException);', $lf)"/>
   <xsl:value-of select="concat($lf, 'virtual ', $scopedReturnType, '_ptr disconnect_', $_receptacleName, '()',
                                $lf, 'throw(Components::NoConnection, CORBA::SystemException);', $lf)"/>
   <xsl:value-of select="concat($lf, 'virtual ', $scopedReturnType, '_ptr get_connection_', $_receptacleName, '()', 
                                $lf, 'throw(CORBA::SystemException);', $lf)"/>
</xsl:template> <!-- end of template service_component_hpp.content.3 -->



<!--
-->
<xsl:template name="service_component_hpp.content.4">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramType"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_cppOperationName"/>

   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:value-of select="concat($lf, 'virtual ', $_returnType, ' ', $_cppOperationName, '()', $lf, 'throw(CORBA::SystemException);', $lf)"/>

      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         <xsl:value-of select="concat($lf, 'virtual void ', $_cppOperationName, '(', $_paramType, ')', $lf, 'throw(CORBA::SystemException);', $lf)"/>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_component_hpp.content.4 -->



<!--
-->
<xsl:template name="service_component_hpp.content.5">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramName"/>
   
   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         <xsl:value-of select="concat($lf, 'bool m_is_', $_paramName, '_configured;', $lf)"/>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_component_hpp.content.5 -->



<!--
-->
<xsl:template name="service_component_hpp.content.6">
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerName"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_consumerTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:value-of select="concat($lf, 'virtual ', $scopedReturnType, 'Consumer_ptr',
                                $lf, 'get_consumer_', $_consumerName, '()',
                                $lf, '   throw(CORBA::SystemException);', $lf)"/>
</xsl:template> <!-- end of template service_component_hpp.content.6 -->



<!--
-->
<xsl:template name="service_component_hpp.content.7">
   <xsl:param name="_publisherTypeName"/>
   <xsl:param name="_publisherName"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_publisherTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:value-of select="concat($lf, 'virtual Components::Cookie* subscribe_', $_publisherName, '(', $scopedReturnType, 'Consumer_ptr consumer)',
                                $lf, 'throw(Components::ExceededConnectionLimit, CORBA::SystemException);', $lf)"/>

   <xsl:value-of select="concat($lf, 'virtual ', $scopedReturnType, 'Consumer_ptr unsubscribe_', $_publisherName, '(Components::Cookie* ck)',
                                $lf, 'throw(Components::InvalidConnection, CORBA::SystemException);', $lf)"/>
</xsl:template> <!-- end of template service_component_hpp.content.7 -->



<!--
-->
<xsl:template name="service_component_hpp.content.8">
   <xsl:param name="_emitterTypeName"/>
   <xsl:param name="_emitterName"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_emitterTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:value-of select="concat($lf, 'virtual void connect_', $_emitterName, '(', $scopedReturnType, 'Consumer_ptr consumer)',
                                $lf, 'throw(::Components::AlreadyConnected, CORBA::SystemException);', $lf)"/>

   <xsl:value-of select="concat($lf, 'virtual ', $scopedReturnType, 'Consumer_ptr disconnect_', $_emitterName, '()',
                                $lf, 'throw(Components::NoConnection, CORBA::SystemException);', $lf)"/>
</xsl:template> <!-- end of template service_component_hpp.content.8 -->



</xsl:stylesheet>


