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
<xsl:template name="service_context_cpp">
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
            <xsl:variable name="contextImplClassname" select="concat('CCM_', $managedComponentNode/@name, '_ServiceContext_impl')"/>

            <!--
               Get the output filename.
            -->
            <xsl:variable name="filename">
               <xsl:call-template name="buildOutputFilename">
                  <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                  <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                  <xsl:with-param name="_defaultFilename" select="$contextImplClassname"/>
                  <xsl:with-param name="_fileExtension" select="'.cpp'"/>
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
                  Insert document header.
               -->
               <xsl:call-template name="docHeader">
                  <xsl:with-param name="_lang" select="'cpp'"/>
               </xsl:call-template>
               <!--
                  Insert the content.
               -->
               <xsl:call-template name="service_context_cpp.content">
                  <xsl:with-param name="_contextImplClassname" select="$contextImplClassname"/>
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
         <xsl:call-template name="service_context_cpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_context_cpp -->



<!--
   This template generates the content of the file.

   @param _contextImplClassname      The service context impl classname.
   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _componentName             The component name.
-->
<xsl:template name="service_context_cpp.content">
   <xsl:param name="_contextImplClassname"/>
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_componentName"/>

   <!--
      Miscellaneous variables.
   -->
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
   
   #include "Foundation/orbsupport/CORBA.hpp"
   #include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
   #include "<xsl:value-of select="concat($pathPrefix, $_contextImplClassname)"/>.hpp"
   #include "<xsl:value-of select="$pathPrefix"/>Service<xsl:value-of select="$_componentName"/>_impl.hpp"
   
   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   <xsl:value-of select="concat($lf, $_contextImplClassname, $cppSep, $_contextImplClassname, $lf)"/>
         (Components::CCMContext_ptr ctx,
          Service<xsl:value-of select="$_componentName"/>_impl*        component)
      throw (CORBA::SystemException)
         : ServiceContext_impl(ctx),
           m_component(component)
   {
   }

   <xsl:value-of select="concat($lf, $_contextImplClassname, $cppSep, '~', $_contextImplClassname, '()', $lf)"/>
   {
   }

   <!--
      Receptacles.
   -->

   <xsl:call-template name="findAllComponentReceptacles">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'service_context_cpp.content.1'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

   <!--
      Emitters.
   -->
   
   <xsl:call-template name="findAllComponentEmitters">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'service_context_cpp.content.2'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

   <!--
      Publishers.
   -->

   <xsl:call-template name="findAllComponentPublishers">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'service_context_cpp.content.2'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>
   
   <!--
      The code generation stops here.
   -->
   
</xsl:template> <!-- end of template service_context_cpp.content -->



<!--
-->
<xsl:template name="service_context_cpp.content.1">
   <xsl:param name="_receptacleTypeName"/>
   <xsl:param name="_receptacleName"/>
   <xsl:param name="_contextImplClassname"/>
   
   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_receptacleTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:value-of select="concat($lf, $scopedReturnType, '_ptr', $lf,
                                $_contextImplClassname, $cppSep, 'get_connection_', $_receptacleName, '()', $lf)"/>
      throw(CORBA::SystemException)
   {
      <xsl:value-of select="concat($scopedReturnType, '_var')"/> recep = 
         <![CDATA[m_component->get_connection_]]><xsl:value-of select="$_receptacleName"/>();
      return recep._retn();
   }
</xsl:template> <!-- end of template service_context_cpp.content.1 -->



<!--
-->
<xsl:template name="service_context_cpp.content.2">
   <xsl:param name="_eventTypeName"/>
   <xsl:param name="_eventName"/>
   <xsl:param name="_contextImplClassname"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_eventTypeName"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'void', $lf, $_contextImplClassname, $cppSep, 'push_', $_eventName, '(', $scopedReturnType, '* event', ')',
                                $lf, 'throw(CORBA::SystemException)', $lf)"/>
   {
      // insert event into an any
      CORBA::Any data;
      <![CDATA[data <<= event;]]>

      // find proxy to push event
      CosEventComm::PushConsumer_var proxy_consumer =
         <![CDATA[m_component->get_proxy_consumer]]>("<xsl:value-of select="$_eventName"/>");

      if (CORBA::is_nil(proxy_consumer))
      {
          throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }

      try
      {
          <![CDATA[proxy_consumer->push(data);]]>
      }
      catch (<![CDATA[const CosEventComm::Disconnected &]]>)
      {
          throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
   }
</xsl:template> <!-- end of template service_context_cpp.content.2 -->



</xsl:stylesheet>


