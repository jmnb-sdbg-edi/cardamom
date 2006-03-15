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
   This templates prefixes the module name of a scoped classname.
   Optionaly a new IDL pragma prefix can be set.
   
   @param _prefix      Name used to prefix the module.
   @param _idlPrefix   IDL prefix can also be set. Optional.
   @param _scopedName  The scoped classname to change.

-->
<xsl:template name="prefixModuleName">
   <xsl:param name="_prefix"/>
   <xsl:param name="_idlPrefix"/>
   <xsl:param name="_scopedName"/>
   
   <xsl:variable name="className">
      <xsl:call-template name="getLastToken">
         <xsl:with-param name="_string" select="$_scopedName"/>
         <xsl:with-param name="_separator" select="$javaSep"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:variable name="moduleName">
      <xsl:value-of select="key('moduleById', key('classByName', $className)/@namespace)/@name"/>
   </xsl:variable>

   <xsl:variable name="beforeModuleName">
      <xsl:value-of select="substring-before($_scopedName, $moduleName)"/>
   </xsl:variable>
   
   <xsl:variable name="afterModuleName">
      <xsl:value-of select="substring-after($_scopedName, $moduleName)"/>
   </xsl:variable>
   
   <xsl:choose>
      <xsl:when test="string-length($_idlPrefix)>0">
         <xsl:variable name="javaIdlPrefix">
            <xsl:call-template name="reverseList">
               <xsl:with-param name="_list" select="$_idlPrefix"/>
               <xsl:with-param name="_separator" select="$javaSep"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:value-of select="concat($javaIdlPrefix, $javaSep, $_prefix, $moduleName, $afterModuleName)"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="concat($beforeModuleName, $_prefix, $moduleName, $afterModuleName)"/>
      </xsl:otherwise>
   </xsl:choose>

</xsl:template>



<!--
   This templates prefixes the class name of a scoped classname.
   
   @param _prefix      Name used to prefix the classname.
   @param _scopedName  The scoped classname to change.

-->
<xsl:template name="prefixClassName">
   <xsl:param name="_prefix"/>
   <xsl:param name="_scopedName"/>
   
   <xsl:variable name="className">
      <xsl:call-template name="getLastToken">
         <xsl:with-param name="_string" select="$_scopedName"/>
         <xsl:with-param name="_separator" select="$javaSep"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:variable name="scope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_scopedName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($scope, $javaSep, $_prefix, $className)"/>

</xsl:template>



<!--
   This templates returns the scoped name of an exception.
   
   @param _exception   The name of the exception.

-->
<xsl:template name="getScopedException">
   <xsl:param name="_exception"/>

   <xsl:variable name="javaException">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_exception"/>
         <xsl:with-param name="_charsIn" select="$cppSep"/>
         <xsl:with-param name="_charsOut" select="$javaSep"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:variable name="exceptionName">
      <xsl:call-template name="getLastToken">
         <xsl:with-param name="_string" select="$javaException"/>
         <xsl:with-param name="_separator" select="$javaSep"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:variable name="scope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$javaException"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <!--
      if the exception is defined in the XMI file, 
      let's build the scoped name
   -->
   <xsl:variable name="scopedName">
      <xsl:if test="boolean(key('exceptionByName', $exceptionName))">
         <xsl:call-template name="getScopedName">
            <xsl:with-param name="_name" select="$exceptionName"/>
            <xsl:with-param name="_lang" select="'java'"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:variable>
   
   <xsl:choose>
      <!--
         it may happen that a class with the same name is defined in the XMI file
         so we check if the built scoped name contains at least the original scope
      -->
      <xsl:when test="contains($scopedName, $scope)">
         <xsl:value-of select="$scopedName"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:variable name="prefix">
            <xsl:call-template name="reverseList">
               <xsl:with-param name="_list" select="$idlPragmaPrefix"/>
               <xsl:with-param name="_separator" select="$javaSep"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:variable name="count">
            <xsl:call-template name="countTokens">
               <xsl:with-param name="_string" select="$javaException"/>
               <xsl:with-param name="_separator" select="$javaSep"/>
            </xsl:call-template>
         </xsl:variable>
         <!--
            if the class is defined inside an interface
            we must add 'Package' to the end of the interface name
         -->
         <xsl:variable name="javaScope">
            <xsl:choose>
               <xsl:when test="$count > 2">
                  <xsl:value-of select="concat($scope, 'Package')"/>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="$scope"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:variable>
         <xsl:value-of select="concat($prefix, $javaSep, $javaScope, $javaSep, $exceptionName)"/>
      </xsl:otherwise>
   </xsl:choose> 
   
</xsl:template> <!-- end of template getScopedException -->



</xsl:stylesheet>


