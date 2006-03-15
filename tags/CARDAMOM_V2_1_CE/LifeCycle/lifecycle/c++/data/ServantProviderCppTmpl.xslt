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
<xsl:template name="servant_provider_cpp">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>

   <xsl:if test="boolean($factoryNodes)">
      <xsl:if test="$_index &lt;= count($factoryNodes)">
         <xsl:variable name="factoryNode" select="$factoryNodes[$_index]"/>

         <!--
            References to some nodes.
         -->
         <xsl:variable name="servantProviderNode" select="$factoryNode/servant-provider"/>

         <xsl:if test="boolean($servantProviderNode)">
            <!--
               Get the servant provider classname.
            -->
            <xsl:variable name="scopedServantProviderClassname" select="$servantProviderNode/@classname"/>
            <xsl:variable name="servantProviderClassname">
               <xsl:call-template name="getLastToken">
                  <xsl:with-param name="_string" select="$scopedServantProviderClassname"/>
                  <xsl:with-param name="_separator" select="$xmlSep"/>
               </xsl:call-template>
            </xsl:variable>
         
            <!--
               Get the output filename.
            -->
            <xsl:variable name="filename">
               <xsl:call-template name="buildOutputFilename">
                  <xsl:with-param name="_userDirectory" select="$servantProviderNode/@directory"/>
                  <xsl:with-param name="_userFilename" select="$servantProviderNode/@filename"/>
                  <xsl:with-param name="_defaultFilename" select="$servantProviderClassname"/>
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
                  Insert the document header.
               -->
               <xsl:call-template name="docHeader">
                  <xsl:with-param name="_lang" select="'cpp'"/>
               </xsl:call-template>
               <!--
                  Insert the content.
               -->
               <xsl:call-template name="servant_provider_cpp.content">
                  <xsl:with-param name="_headerFilename">
                     <xsl:if test="string-length($CSC_NAME) > 0">
                        <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
                     </xsl:if>
                     <xsl:call-template name="getLastToken">
                        <xsl:with-param name="_string">
                           <xsl:call-template name="replaceCharsInString">
                              <xsl:with-param name="_stringIn" select="$filename"/>
                              <xsl:with-param name="_charsIn" select="'.cpp'"/>
                              <xsl:with-param name="_charsOut" select="'.hpp'"/>
                           </xsl:call-template>
                        </xsl:with-param>
                        <xsl:with-param name="_separator" select="$fileSep"/>
                     </xsl:call-template>
                  </xsl:with-param>
                  <xsl:with-param name="_interfaceName">
                     <xsl:call-template name="getLastToken">
                        <xsl:with-param name="_string" select="$factoryNode/@idl-interface"/>
                        <xsl:with-param name="_separator" select="$xmlSep"/>
                     </xsl:call-template>
                  </xsl:with-param>
                  <xsl:with-param name="_servantProviderClassname" select="$servantProviderClassname"/>
                  <xsl:with-param name="_scopedServantProviderClassname" select="$scopedServantProviderClassname"/>
               </xsl:call-template>
            </xsl:document>
         </xsl:if>

         <!--
            Proceed to the next item in idlNodes if any.
         -->
         <xsl:call-template name="servant_provider_cpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template servant_provider_cpp -->



<!--
   This template generates the content of the file.

   @param _headerFilename                   The header filename.
   @param _interfaceName                    The idl interface to visit.
   @param _servantProviderClassname         The servant provider class name.
   @param _scopedServantProviderClassname   The scoped servant provider class name.
-->

<xsl:template name="servant_provider_cpp.content">
   <xsl:param name="_headerFilename"/>
   <xsl:param name="_interfaceName"/>
   <xsl:param name="_servantProviderClassname"/>
   <xsl:param name="_scopedServantProviderClassname"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="interfaceNode" select="key('classByName', $_interfaceName)"/>
   
   <!--
      The code generation starts here.
   -->

   #include "<xsl:value-of select="$_headerFilename"/>"

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedServantProviderClassname"/>
   </xsl:call-template>

   <xsl:value-of select="concat($_servantProviderClassname, $cppSep, $_servantProviderClassname)"/>()
      throw()
   {
   }

   <xsl:value-of select="concat($_servantProviderClassname, $cppSep, '~', $_servantProviderClassname)"/>()
      throw()
   {
   }

   const char**
   <xsl:value-of select="$_servantProviderClassname"/>::get_ids() const throw()
   {
      static const char* ids[] = {
         <xsl:call-template name="getObjectList">
            <xsl:with-param name="_nodeName" select="$_interfaceName"/>
         </xsl:call-template>
         0
      };
      return ids;
   }

   CORBA::Long
   <xsl:value-of select="$_servantProviderClassname"/>::get_number_of_ids() const throw()
   {
      static const CORBA::Long nb = <xsl:call-template name="getObjectList">
                                       <xsl:with-param name="_nodeName" select="$_interfaceName"/>
                                       <xsl:with-param name="_count" select="true()"/>
                                    </xsl:call-template>;
      return nb;
   }

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedServantProviderClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->
   
</xsl:template> <!-- end of template servant_provider_cpp.content -->



</xsl:stylesheet>


