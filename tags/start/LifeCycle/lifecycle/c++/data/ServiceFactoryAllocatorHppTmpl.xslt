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
<xsl:template name="service_factory_allocator_hpp">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="nodes" select="$lifecycleFrameworkNodes[factory-impl/@category = 'SERVICE']"/>

   <xsl:if test="boolean($nodes)">
      <xsl:if test="$_index &lt;= count($nodes)">
         <xsl:variable name="lifecycleFrameworkNode" select="$nodes[$_index]"/>

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
            Get the output filename.
         -->
         <xsl:variable name="factoryFileName">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$lifecycleFrameworkNode/factory-impl/@directory"/>
               <xsl:with-param name="_userFilename" select="$lifecycleFrameworkNode/factory-impl/@filename"/>
               <xsl:with-param name="_defaultFilename" select="$factoryImplClassname"/>
               <xsl:with-param name="_fileExtension" select="''"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:variable name="filename" select="concat($factoryFileName, '_allocator.hpp')"/>

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
               <xsl:with-param name="_lang" select="'cpp'"/>
            </xsl:call-template>

            <!--
               Insert the content.
            -->
            <xsl:call-template name="service_factory_allocator_hpp.content">
               <xsl:with-param name="_factoryImplClassname" select="$factoryImplClassname"/>
               <xsl:with-param name="_headerFactoryAllocatorName">
                  <xsl:if test="string-length($CSC_NAME) > 0">
                     <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
                  </xsl:if>
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="$factoryFileName"/>
                     <xsl:with-param name="_separator" select="$fileSep"/>
                  </xsl:call-template>
               </xsl:with-param>
               <xsl:with-param name="_scopedFactoryImplClassname" select="$scopedFactoryImplClassname"/>
            </xsl:call-template>
         </xsl:document>

         <!--
            Proceed to the next item if any.
         -->
         <xsl:call-template name="service_factory_allocator_hpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template>



<!--
   This template generates the content of the file.*

   @param _factoryImplClassname       The factory impl classname.
   @param _headerFactoryAllocatorName The factory allocator header file name.
   @param _scopedFactoryImplClassname The scoped factory impl classname.
-->
<xsl:template name="service_factory_allocator_hpp.content">
   <xsl:param name="_factoryImplClassname"/>
   <xsl:param name="_headerFactoryAllocatorName"/>
   <xsl:param name="_scopedFactoryImplClassname"/>

   <!--
      The code generation starts here.
   -->

   #include "LifeCycle/lifecycle/FactoryAllocator.hpp"
   #include "<xsl:value-of select="$_headerFactoryAllocatorName"/>.hpp"

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedFactoryImplClassname"/>
   </xsl:call-template>

   // TODO: Use a user-provided name rather than a naming convention!
   <![CDATA[typedef Cdmw::LifeCycle::AnonymousObjectsFactoryAllocator<]]><xsl:value-of select="$_factoryImplClassname"/><![CDATA[>]]>
      <xsl:value-of select="$_factoryImplClassname"/>_Allocator;

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedFactoryImplClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->


</xsl:template>


</xsl:stylesheet>


