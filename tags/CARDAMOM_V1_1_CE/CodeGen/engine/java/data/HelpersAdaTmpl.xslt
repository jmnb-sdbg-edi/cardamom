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
   This template builds a file name compatible with gnat.

   @param _name   The string to build the file name from.
-->

<xsl:template name="gnatFilename">
   <xsl:param name="_name"/>

   <xsl:choose>
      <xsl:when test="contains($_name, $adaSep)">
         <xsl:value-of select="concat(translate(substring-before($_name, $adaSep), $ucase, $lcase), '-')"/>
         <xsl:call-template name="gnatFilename">
            <xsl:with-param name="_name" select="substring-after($_name, $adaSep)"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="translate($_name, $ucase, $lcase)"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template gnatFilename -->



<!--
   This template formats an operation name using the following rules :
   <ul>
      <li> the first character of a word is uppercased,
      <li> the following characters are in lowercase
   </ul>
   We assume that each word is separated with an underscore.
   <p>
   Example : 'class_name' becomes 'Class_Name'.

   @param _string   The string to format.
-->

<xsl:template name="adaOperationName">
   <xsl:param name="_string"/>

   <xsl:choose>
      <xsl:when test="contains($_string, '_')">
         <!--
            Get the first word.
         -->
         <xsl:variable name="word" select="substring-before($_string, '_')"/>
         
         <!--
            Uppercase the first character.
         -->
         <xsl:value-of select="concat(translate(substring($word, 1, 1), $lcase, $ucase), substring($word, 2), '_')"/>
         
         <!--
            Repeat the process with the remaining string.
         -->
         <xsl:call-template name="adaOperationName">
            <xsl:with-param name="_string" select="substring-after($_string, '_')"/>
         </xsl:call-template>
      </xsl:when>
      <!--
         There is no underscore in $_string,
         we just need to uppercase the first character of _string.
      -->
      <xsl:otherwise>
         <xsl:value-of select="concat(translate(substring($_string, 1, 1), $lcase, $ucase), substring($_string, 2))"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template adaOperationName -->



<!--
   This template replaces the tag <@TypesPackages> used in the
   previous code generator.

   @param _interfaceName   The name of the interface to visit.
-->
<xsl:template name="adaTypesPackages">
   <xsl:param name="_interfaceName"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="interfaceNode" select="key('classByName', $_interfaceName)"/>
   
   <!--
      Find what modules we need to visit.
   -->
   <xsl:variable name="packages">
      <xsl:call-template name="adaTypesPackages_visitOps">
         <xsl:with-param name="_operations" select="key('operations', $interfaceNode/@xmi.id)"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      Generate instructions.
   -->
   <xsl:call-template name="adaTypesPackages_print">
      <xsl:with-param name="_packages" select="$packages"/>
   </xsl:call-template>
</xsl:template> <!-- end of template adaTypesPackages -->



<!--
   This template prints out ADA instructions for the packages.

   @param _packages    The packages to use.
   @param _separator   The token separator.
-->
<xsl:template name="adaTypesPackages_print">
   <xsl:param name="_packages" select="''"/>
   <xsl:param name="_separator" select="' '"/>
   
   <xsl:if test="string-length($_packages) > 0">
      <xsl:choose>
         <xsl:when test="contains($_packages, $_separator)">
            <xsl:variable name="firstToken" select="substring-before($_packages, $_separator)"/>
            <xsl:if test="string-length($firstToken) > 0 and $firstToken != $_separator">
               <xsl:value-of select="concat($lf, 'with ', $firstToken, ' ;')"/>
            </xsl:if>
            <!--
               Proceed to the next token.
            -->
            <xsl:call-template name="adaTypesPackages_print">
               <xsl:with-param name="_packages" select="substring-after($_packages, $_separator)"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="concat($lf, 'with ', $_packages, ' ;')"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:if>
</xsl:template> <!-- end of template adaTypesPackages_print -->



<!--
   This template visits a list of operations of an interface
   and returns a list of packages.

   @param _operations   The operations to visit.
-->
<xsl:template name="adaTypesPackages_visitOps">
   <xsl:param name="_operations"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   <xsl:param name="_packages" select="''"/>

   <xsl:if test="boolean($_operations)">
      <xsl:choose>
         <xsl:when test="$_index &lt;= count($_operations)">
            <xsl:variable name="operationNode" select="$_operations[$_index]"/>
            
            <xsl:variable name="packages">
               <xsl:call-template name="adaTypesPackages_visitParams">
                  <xsl:with-param name="_params" select="$operationNode/UML:BehavioralFeature.parameter/UML:Parameter"/>
               </xsl:call-template>
            </xsl:variable>

            <!--
               Proceed to the next operation.
            -->
            <xsl:call-template name="adaTypesPackages_visitOps">
               <xsl:with-param name="_operations" select="$_operations"/>
               <xsl:with-param name="_index" select="$_index + 1"/>
               <xsl:with-param name="_packages">
                  <xsl:choose>
                     <xsl:when test="string-length($packages) > 0">
                        <xsl:call-template name="mergeLists">
                           <xsl:with-param name="_list1" select="$_packages"/>
                           <xsl:with-param name="_list2" select="$packages"/>
                        </xsl:call-template>
                     </xsl:when>
                     <xsl:otherwise>
                        <xsl:value-of select="$_packages"/>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:with-param>
            </xsl:call-template>
         </xsl:when>
         <xsl:otherwise>
            <!--
               Return.
            -->
            <xsl:if test="string-length($_packages) > 0">
               <xsl:value-of select="$_packages"/>
            </xsl:if>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:if>
</xsl:template> <!-- end of template adaTypesPackages_visitOps -->



<!--
   This template visits a list of parameters of an operation
   and returns a list of packages.

   @param _params   The parameters to visit.
-->
<xsl:template name="adaTypesPackages_visitParams">
   <xsl:param name="_params"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   <xsl:param name="_packages" select="''"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="interfaceId" select="key('stereotypeByName', 'CORBAInterface')/@xmi.id"/>

   <xsl:if test="boolean($_params)">
      <xsl:choose>
         <xsl:when test="$_index &lt;= count($_params)">
            <xsl:variable name="paramNode" select="$_params[$_index]"/>     
            <xsl:variable name="paramTypeNode" select="key('classById', $paramNode/@type)"/>
            
            <xsl:variable name="package">
               <xsl:if test="boolean($paramTypeNode) and ($paramTypeNode/@stereotype = $interfaceId)">
                  <xsl:call-template name="getScopedName">
                     <xsl:with-param name="_name" select="$paramTypeNode/@name"/>
                     <xsl:with-param name="_lang" select="'ada'"/>
                  </xsl:call-template>
               </xsl:if>
            </xsl:variable>

            <!--
               Proceed to the next param.
            -->
            <xsl:call-template name="adaTypesPackages_visitParams">
               <xsl:with-param name="_params" select="$_params"/>
               <xsl:with-param name="_index" select="$_index + 1"/>
               <xsl:with-param name="_packages">
                  <xsl:call-template name="addTokenToList">
                     <xsl:with-param name="_list" select="$_packages"/>
                     <xsl:with-param name="_token" select="$package"/>
                  </xsl:call-template>
               </xsl:with-param>
            </xsl:call-template>
         </xsl:when>
         <xsl:otherwise>
            <!--
               Return.
            -->
            <xsl:if test="string-length($_packages) > 0">
               <xsl:value-of select="$_packages"/>
            </xsl:if>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:if>
</xsl:template> <!-- end of template adaTypesPackages_visitParams -->



</xsl:stylesheet>


