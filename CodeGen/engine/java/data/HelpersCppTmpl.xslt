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
   This template go through all the /cdmw/programs/program/services/fault-tolerance/datastores/datastore
   nodes and produces an <b>#include</b> instruction for each datastore.
-->
<xsl:template name="includeOidDataForDataStore">
   <xsl:param name="_datastorenodes"/>
   <xsl:for-each select="$_datastorenodes">
      <xsl:variable name="datastoreIdref" select="@idref"/>
      <xsl:variable name="datastoreTypeNode" select="$DatastoreTypeNodes[@typename = $datastoreIdref]"/>
      <xsl:if test="boolean($datastoreTypeNode/@idl-filename)">
         <xsl:variable name="_idlfilename" select="$datastoreTypeNode/@idl-filename"/>
         <xsl:variable name="_string" select="substring-before($_idlfilename,'.idl')"/>

         <xsl:variable name="pathPrefix">
             <xsl:choose>
                 <xsl:when test="string-length($CSC_NAME) > 0">
                     <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
                 </xsl:when>
                 <xsl:otherwise>
                     <xsl:text></xsl:text>
                 </xsl:otherwise>
             </xsl:choose>
         </xsl:variable>

         <xsl:text>#include "</xsl:text><xsl:value-of select="concat($pathPrefix, $_string)"/>.stub.hpp"<xsl:value-of select="$lf"/>
      </xsl:if>	 
   </xsl:for-each>
</xsl:template>



<!--
   This template go through all the /cdmw/programs/program/services/lifecycle/framework
   nodes and produces an <b>#include</b> instruction for each framework.
   
   @param _framework        A framework.
   @param _hasHomeImplRef   If set to true() then produce #include instructions for home impl.
-->
<xsl:template name="includeFramework">
   <xsl:param name="_framework"/>
   <xsl:param name="_hasHomeImplRef" select="false()"/>
   <xsl:param name="_hasFaultTolerance" select="false()"/>

   <!--
      Get the matching /cdmw/lifecycle-frameworks/lifecycle-framework node.
   -->
   <xsl:variable name="lifecycleFrameworkNode" select="$lifecycleFrameworkNodes[@id = $_framework/@idref]"/>

   <xsl:variable name="isUserDefined" select="boolean($lifecycleFrameworkNode/user-factory-impl)"/>

   <xsl:variable name="pathPrefix">
       <xsl:choose>
           <xsl:when test="string-length($CSC_NAME) > 0">
               <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
           </xsl:when>
           <xsl:otherwise>
               <xsl:text></xsl:text>
           </xsl:otherwise>
       </xsl:choose>
   </xsl:variable>

   <!--
      Determine the filename to include.
   -->
   <xsl:variable name="filename">
      <xsl:call-template name="addFileExtension">
         <xsl:with-param name="_filename">
            <xsl:choose>
               <!--
                  Is it a user-defined factory ?
               -->
               <xsl:when test="$isUserDefined">
                  <xsl:choose>
                     <!--
                        If the user has specified a filename then use it.
                     -->
                     <xsl:when test="boolean($lifecycleFrameworkNode/user-factory-impl/factory-allocator/@filename)">
                        <xsl:value-of select="$lifecycleFrameworkNode/user-factory-impl/factory-allocator/@filename"/>
                     </xsl:when>
                     <!--
                        By default, use the classname as the filename.
                     -->
                     <xsl:otherwise>
                        <xsl:value-of select="$pathPrefix"/>
                        <xsl:call-template name="getLastToken">
                           <xsl:with-param name="_string" select="$lifecycleFrameworkNode/user-factory-impl/factory-allocator/@classname"/>
                           <xsl:with-param name="_separator" select="$xmlSep"/>
                        </xsl:call-template>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:choose>
                     <!--
                        If the user has specified a filename then use it.
                     -->
                     <xsl:when test="boolean($lifecycleFrameworkNode/factory-impl/@filename)">
                        <xsl:value-of select="$lifecycleFrameworkNode/factory-impl/@filename"/>
                     </xsl:when>
                     <xsl:when test="boolean($lifecycleFrameworkNode/home-impl/@filename) and $_hasHomeImplRef">
                        <xsl:value-of select="$lifecycleFrameworkNode/home-impl/@filename"/>
                     </xsl:when>
                     <!--
                        Otherwise, use the factory/framework class name as the default filename.
                     -->
                     <xsl:otherwise>
                        <xsl:value-of select="$pathPrefix"/>
                        <xsl:choose>
                           <xsl:when test="boolean($lifecycleFrameworkNode/factory-impl/@classname)">
                              <xsl:call-template name="getLastToken">
                                 <xsl:with-param name="_string" select="$lifecycleFrameworkNode/factory-impl/@classname"/>
                                 <xsl:with-param name="_separator" select="$xmlSep"/>
                              </xsl:call-template>
                           </xsl:when>
                           <xsl:when test="boolean($lifecycleFrameworkNode/home-impl/@classname) and $_hasHomeImplRef">
                              <xsl:call-template name="getLastToken">
                                 <xsl:with-param name="_string" select="$lifecycleFrameworkNode/home-impl/@classname"/>
                                 <xsl:with-param name="_separator" select="$xmlSep"/>
                              </xsl:call-template>
                           </xsl:when>
                           <xsl:otherwise>
                              <xsl:if test="$_hasHomeImplRef">
                                 <xsl:call-template name="buildHomeImplClassname">
                                    <xsl:with-param name="_homeImplNode" select="$lifecycleFrameworkNode/home-impl"/>
                                    <xsl:with-param name="_scoped" select="false()"/>
                                 </xsl:call-template>
                              </xsl:if>
                           </xsl:otherwise>
                        </xsl:choose>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:with-param>
         <xsl:with-param name="_extension" select="'.hpp'"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      Determine the FT filename to include for Home_impl.
   -->
   <xsl:variable name="FTHomeFilename"
>
      <xsl:call-template name="addFileExtension">
         <xsl:with-param name="_filename">
            <xsl:value-of select="concat($pathPrefix, 'FT')"/>
            <xsl:choose>
               <xsl:when test="boolean($lifecycleFrameworkNode/factory-impl/@classname)">
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="$lifecycleFrameworkNode/factory-impl/@classname"/>
                     <xsl:with-param name="_separator" select="$xmlSep"/>
                  </xsl:call-template>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:if test="$_hasHomeImplRef">
                     <xsl:call-template name="buildHomeImplClassname">
                        <xsl:with-param name="_homeImplNode" select="$lifecycleFrameworkNode/home-impl"/>
                        <xsl:with-param name="_scoped" select="false()"/>
                     </xsl:call-template>
                  </xsl:if>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:with-param>
         <xsl:with-param name="_extension" select="'.hpp'"/>
      </xsl:call-template>
   </xsl:variable>


   <!--
      Generate the instruction.
   -->
   <xsl:value-of select="$lf"/>
   <xsl:choose>
      <xsl:when test="$_hasHomeImplRef='false' and not($isUserDefined)">
         <xsl:text>#include "</xsl:text><xsl:value-of select="concat(substring-before($filename, '.'), '_allocator.hpp')"/><xsl:text>"</xsl:text>
      </xsl:when>
      <xsl:otherwise>
         <xsl:text>#include "</xsl:text><xsl:value-of select="$filename"/><xsl:text>"</xsl:text>
         <xsl:if test="$_hasFaultTolerance">
            <xsl:value-of select="$lf"/>
            <xsl:text>#include "</xsl:text><xsl:value-of select="$FTHomeFilename"/><xsl:text>"</xsl:text>
         </xsl:if>
      </xsl:otherwise>
   </xsl:choose>
   <xsl:value-of select="$lf"/>
</xsl:template> <!-- end of template includeFramework -->



<!--
   This template generates <b>#ifndef, #define, #endif</b> instructions for .hpp files.<br>
   The default behaviour is set to generate #ifndef, #define.

   @param _prefix      A string prefix.
   @param _classname   A class name.
   @param _endif       If true() then write #endif else write #ifndef, #define.
-->
<xsl:template name="writeDefine">
   <xsl:param name="_prefix"/>
   <xsl:param name="_classname"/>
   <xsl:param name="_endif" select="false()"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="defineString" select="concat('__INCL_', translate($_prefix, $lcase, $ucase), '_', translate($_classname, $lcase, $ucase), '_HPP__')"/>

   <xsl:choose>
      <xsl:when test="$_endif">
         <xsl:choose>
            <xsl:when test="string-length($_prefix) = 0 and string-length($_classname) = 0">
               <xsl:value-of select="concat($lf, '#endif')"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="concat($lf, '#endif // ', $defineString, $lf)"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="concat($lf, '#ifndef ', $defineString, $lf)"/>
         <xsl:value-of select="concat('#define ', $defineString, $lf)"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template writeDefine -->



<!--
   This template opens cascaded namespaces instructions from a scope.

   @param _scope                  The scope to use.
   @param _separator              The token separator.
   @param _lastTokenIsNamespace   Consider the last token of the scope as a namespace.
                                  (The default is set to false as most of the time, the
                                   user will pass the scoped implementation classname of
                                   a factory or a home to this template)
-->
<xsl:template name="openNamespace">
   <xsl:param name="_scope"/>
   <xsl:param name="_separator" select="$xmlSep"/>
   <xsl:param name="_lastTokenIsNamespace" select="false()"/>

   <xsl:if test="string-length($_scope) > 0">
      <xsl:if test="not(starts-with($_scope, $_separator))">
         <xsl:choose>
            <xsl:when test="contains($_scope, $_separator)">
               <xsl:value-of select="concat($lf, 'namespace ', substring-before($_scope, $_separator), ' {', $lf)"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:if test="$_lastTokenIsNamespace">
                  <xsl:value-of select="concat($lf, 'namespace ', $_scope, ' {', $lf)"/>
               </xsl:if>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:if>
      <xsl:call-template name="openNamespace">
         <xsl:with-param name="_scope" select="substring-after($_scope, $_separator)"/>
         <xsl:with-param name="_separator" select="$_separator"/>
         <xsl:with-param name="_lastTokenIsNamespace" select="$_lastTokenIsNamespace"/>
      </xsl:call-template>
   </xsl:if>
</xsl:template> <!-- end of template openNamespace -->



<!--
   This template closes namespaces as opposite to the previous template.

   @param _scope                  The scope to use.
   @param _separator              The token separator.
   @param _lastTokenIsNamespace   Consider the last token of the scope as a namespace.
                                  (The default is set to false as most of the time, the
                                   user will pass the scoped implementation classname of
                                   a factory or a home to this template)
-->
<xsl:template name="closeNamespace">
   <xsl:param name="_scope"/>
   <xsl:param name="_separator" select="$xmlSep"/>
   <xsl:param name="_lastTokenIsNamespace" select="false()"/>

   <xsl:if test="string-length($_scope) > 0">
      <xsl:variable name="lastNamespace">
         <xsl:call-template name="getLastToken">
            <xsl:with-param name="_string" select="$_scope"/>
            <xsl:with-param name="_separator" select="$_separator"/>
         </xsl:call-template>
      </xsl:variable>

      <xsl:choose>
         <xsl:when test="not($_lastTokenIsNamespace)">
            <xsl:call-template name="closeNamespace">
               <xsl:with-param name="_scope" select="substring-before($_scope, concat($_separator, $lastNamespace))"/>
               <xsl:with-param name="_separator" select="$_separator"/>
               <xsl:with-param name="_lastTokenIsNamespace" select="true()"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="string-length($lastNamespace) > 0">
            <xsl:value-of select="concat($lf, '} // end of namespace ', $lastNamespace, $lf)"/>
            <xsl:call-template name="closeNamespace">
               <xsl:with-param name="_scope" select="substring-before($_scope, concat($_separator, $lastNamespace))"/>
               <xsl:with-param name="_separator" select="$_separator"/>
               <xsl:with-param name="_lastTokenIsNamespace" select="true()"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:if>
</xsl:template> <!-- end of template closeNamespace -->



<!--
   This template goes through each operation contained within the interface
   and finds the container of the return type.

   @param _interfaceNode   The idl interface to visit.
-->
<xsl:template name="findStubToInclude">
   <xsl:param name="_interfaceNode"/>

   <!--
      Get all the operations contained within this interface.
   -->
   <xsl:variable name="containedOps" select="$_interfaceNode/UML:Namespace.ownedElement/UML:Operation"/>

   <!--
      Get the containers of all the return types.
      There may have duplicates.
   -->
   <xsl:variable name="modules">
      <xsl:for-each select="$containedOps">
         <xsl:value-of select="key('moduleById', key('classById', UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'return']/@type)/@namespace)/@name"/>
         <xsl:text> </xsl:text>
      </xsl:for-each>
   </xsl:variable>

   <!--
      Removes the duplicates.
   -->
   <xsl:call-template name="removeDuplicateTokens">
      <xsl:with-param name="_string" select="$modules"/>
   </xsl:call-template>
</xsl:template> <!-- end of template findStubToInclude -->



<!--
   This template generates an #include "module.stub.hpp" instruction
   for each module name in _string.

   @param _string   The module names.
-->
<xsl:template name="printIncludeIdls">
   <xsl:param name="_string"/>

   <xsl:variable name="pathPrefix">
       <xsl:choose>
           <xsl:when test="string-length($CSC_NAME) > 0">
               <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
           </xsl:when>
           <xsl:otherwise>
               <xsl:text></xsl:text>
           </xsl:otherwise>
       </xsl:choose>
   </xsl:variable>

   <xsl:choose>
      <xsl:when test="contains($_string, ' ')">
         <xsl:variable name="firstRepositoryId" select="substring-before($_string, ' ')"/>
         <xsl:if test="string-length($firstRepositoryId) > 0">
            <xsl:text>#include "</xsl:text><xsl:value-of select="concat($pathPrefix, $firstToken)"/>.stub.hpp"<xsl:value-of select="$lf"/>
         </xsl:if>
         <!--
            Do the job again with the remaining module names.
         -->
         <xsl:call-template name="printIncludeIdls">
            <xsl:with-param name="_string" select="substring-after($_string, ' ')"/>
         </xsl:call-template>
      </xsl:when>
      <!--
         No more space character found : end of recursiveness.
      -->
      <xsl:otherwise>
         <xsl:text>#include "</xsl:text><xsl:value-of select="concat($pathPrefix, $_string)"/>.stub.hpp"<xsl:value-of select="$lf"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template printIncludeIdls -->



<!--
   This template finds the idl to include and generates adequate instructions.

   @param _interfaceName   The idl interface name to visit.
-->
<xsl:template name="includeIdls">
   <xsl:param name="_interfaceName"/>

   <xsl:call-template name="printIncludeIdls">
      <xsl:with-param name="_string">
         <xsl:call-template name="findStubToInclude">
            <xsl:with-param name="_interfaceNode" select="key('classByName', $_interfaceName)"/>
         </xsl:call-template>
      </xsl:with-param>
   </xsl:call-template>
</xsl:template> <!-- end of template includeIdls -->

</xsl:stylesheet>


