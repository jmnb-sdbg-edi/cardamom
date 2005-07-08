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


<xsl:stylesheet version="1.1"
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   xmlns:UML="org.omg/UML1.3">



<!--
   Main - This template fetches the necessary data from inputs
   to generate a file.

   @param none
-->
<xsl:template name="service_context_java">
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
            <xsl:variable name="contextImplClassname" select="concat('CCM_', $managedComponentNode/@name, '_ServiceContextImpl')"/>

            <!--
               Get the output filename.
            -->
            <xsl:variable name="filename">
               <xsl:call-template name="buildOutputFilename">
                  <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                  <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                  <xsl:with-param name="_defaultFilename" select="$contextImplClassname"/>
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
               <xsl:call-template name="service_context_java.content">
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
         <xsl:call-template name="service_context_java">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_context_java -->



<!--
   This template generates the content of the file.

   @param _contextImplClassname      The service context impl classname.
   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _componentName             The component name.
-->
<xsl:template name="service_context_java.content">
   <xsl:param name="_contextImplClassname"/>
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_componentName"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="javaComponentScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_componentName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="ccmServiceContextClassName">
      <xsl:variable name="tmpClassName">
         <xsl:call-template name="prefixClassName">
            <xsl:with-param name="_prefix" select="'CCM_'"/>
            <xsl:with-param name="_scopedName">
               <xsl:call-template name="prefixModuleName">
                  <xsl:with-param name="_prefix" select="'Cdmw_'"/>
                  <xsl:with-param name="_idlPrefix" select="$idlPragmaPrefix"/>
                  <xsl:with-param name="_scopedName" select="concat($javaComponentScope, $javaSep, $_componentName)"/>
               </xsl:call-template>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($tmpClassName, '_ServiceContext')"/>
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

import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.ccm.cif.ServiceContextImpl;

public class <xsl:value-of select="$_contextImplClassname"/> extends ServiceContextImpl
   implements Cdmw_<xsl:value-of select="$ccmServiceContextClassName"/> {

    /**
     * Component servant implementation
     */
    private Service<xsl:value-of select="$_componentName"/>Impl component;

    public <xsl:value-of select="$_contextImplClassname"/>(org.omg.Components.CCMContext ctx,
        Service<xsl:value-of select="$_componentName"/>Impl component) {
        super(ctx);
        this.component = component;
    }

         <!--
            Receptacles.
         -->
   
         <xsl:call-template name="findAllJavaComponentReceptacles">
            <xsl:with-param name="_componentName" select="$_componentName"/>
            <xsl:with-param name="_template" select="'service_context_java.content.1'"/>
         </xsl:call-template>

         <!--
            Emitters.
         -->
         
         <xsl:call-template name="findAllJavaComponentEmitters">
            <xsl:with-param name="_componentName" select="$_componentName"/>
            <xsl:with-param name="_template" select="'service_context_java.content.2'"/>
         </xsl:call-template>

         <!--
            Publishers.
         -->

         <xsl:call-template name="findAllJavaComponentPublishers">
            <xsl:with-param name="_componentName" select="$_componentName"/>
            <xsl:with-param name="_template" select="'service_context_java.content.2'"/>
         </xsl:call-template>

}

   <!--
      The code generation stops here.
   -->
   
</xsl:template> <!-- end of template service_context_java.content -->



<!--
-->
<xsl:template name="service_context_java.content.1">
   <xsl:param name="_receptacleTypeName"/>
   <xsl:param name="_receptacleName"/>
   
   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_receptacleTypeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'public ', $scopedReturnType, ' get_connection_', $_receptacleName, '() {', $lf)"/>
   <xsl:value-of select="$scopedReturnType"/> recep = 
         this.component.get_connection_<xsl:value-of select="$_receptacleName"/>();
      return recep;
   }
</xsl:template> <!-- end of template service_context_java.content.1 -->



<!--
-->
<xsl:template name="service_context_java.content.2">
   <xsl:param name="_eventTypeName"/>
   <xsl:param name="_eventName"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_eventTypeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:value-of select="concat($lf, 'public void push_', $_eventName, '(', $scopedReturnType, ' event) {', $lf)"/>
      // insert event into an any
      org.omg.CORBA.Any data = org.omg.CORBA.ORB.init().create_any();
      <xsl:value-of select="$scopedReturnType"/>Helper.insert(data, event);

      // find proxy to push event
      org.omg.CosEventComm.PushConsumer proxyConsumer =
         this.component.get_proxy_consumer("<xsl:value-of select="$_eventName"/>");

      if (proxyConsumer == null) {
          throw new org.omg.CORBA.INTERNAL(
             ExceptionMinorCodes.INTERNAL, 
             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }

      try {
          proxyConsumer.push(data);
      } catch (org.omg.CosEventComm.Disconnected d) {
          throw new org.omg.CORBA.INTERNAL(
             ExceptionMinorCodes.INTERNAL, 
             org.omg.CORBA.CompletionStatus.COMPLETED_NO);
      }
   }
</xsl:template> <!-- end of template service_context_java.content.2 -->



</xsl:stylesheet>


