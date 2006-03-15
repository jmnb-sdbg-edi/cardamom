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
   This template finds all the operations of an interface as well as those
   from its parents.

   @param _interfaceName          The name of the interface to visit.
   @param _template               The template to call.
   @param _isNamed                true = named factory, false = anonymous factory.
   @param _isServantProvider      
   @param _factoryImplClassname   The factory implementation class name.
-->
<xsl:template name="findAllFactoryOperations">
   <xsl:param name="_interfaceName"/>
   <xsl:param name="_template"/>
   <xsl:param name="_isNamed"/>
   <xsl:param name="_isServantProvider"/>
   <xsl:param name="_factoryImplClassname"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="interfaceNode" select="key('classByName', $_interfaceName)"/>

   <!--
      Miscellaneous variables.
   -->

   <xsl:variable name="generalizations" select="key('generalizationById', $interfaceNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>
   <xsl:variable name="interfaceId" select="key('stereotypeByName', 'CORBAInterface')/@xmi.id"/>
   <xsl:variable name="containedOps"  select="key('operations', $interfaceNode/@xmi.id)"/>

   <xsl:for-each select="$containedOps">
      <xsl:if test="not(boolean($_isServantProvider))">
         <!--
            Check whether the first parameter of the operation is of type CdmwLifeCycle::Key.
            Note : the first parameter of an operation is UML:Parameter[2] 
                   because UML:Parameter[1] is the return type.
         -->
         <xsl:choose>
            <xsl:when test="$_isNamed = 'true'">
               <xsl:if test="not(UML:BehavioralFeature.parameter/UML:Parameter[2]/@type = $keyNodeId)">
                  <xsl:message terminate="yes">
                     <xsl:value-of select="concat($lf, 'FAILURE : The interface ', $_interfaceName, 
                        ' contains operations with a missing CdmwLifeCycle::Key parameter.', $lf,
                        'Are you using a suitable IDL interface to generate this named factory ?', $lf,
                        'The generation of the factory ', $_factoryImplClassname, ' is aborted.', $lf)"/>
                  </xsl:message>
               </xsl:if>
            </xsl:when>
            <xsl:otherwise>
               <xsl:if test="UML:BehavioralFeature.parameter/UML:Parameter[2]/@type = $keyNodeId">
                  <xsl:message terminate="yes">
                     <xsl:value-of select="concat($lf, 'FAILURE : The interface ', $_interfaceName, 
                        ' contains operations with a CdmwLifeCycle::Key parameter.', $lf,
                        'Are you using a suitable IDL interface to generate this anonymous factory ?', $lf,
                        'The generation of the factory ', $_factoryImplClassname, ' is aborted.', $lf)"/>
                  </xsl:message>
               </xsl:if>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:if>

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
         Get the operation name.
      -->
      <xsl:variable name="cppOperationName">
         <xsl:call-template name="mapCppReservedWords">
            <xsl:with-param name="_operationName" select="@name"/>
         </xsl:call-template>
      </xsl:variable>

      <xsl:choose>
         <xsl:when test="$_template = 'entity_factory_hpp.content.1'">
            <xsl:call-template name="entity_factory_hpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'entity_factory_cpp.content.1'">
            <xsl:call-template name="entity_factory_cpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
               <xsl:with-param name="_factoryImplClassname" select="$_factoryImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'unmanaged_entity_factory_hpp.content.1'">
            <xsl:call-template name="unmanaged_entity_factory_hpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'unmanaged_entity_factory_cpp.content.1'">
            <xsl:call-template name="unmanaged_entity_factory_cpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
               <xsl:with-param name="_factoryImplClassname" select="$_factoryImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_factory_hpp.content.1'">
            <xsl:call-template name="service_factory_hpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_factory_cpp.content.1'">
            <xsl:call-template name="service_factory_cpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
               <xsl:with-param name="_factoryImplClassname" select="$_factoryImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_factory_hpp.content.1'">
            <xsl:call-template name="session_factory_hpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_factory_cpp.content.1'">
            <xsl:call-template name="session_factory_cpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
               <xsl:with-param name="_factoryImplClassname" select="$_factoryImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'servant_provider_hpp.content.1'">
            <xsl:call-template name="servant_provider_hpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:for-each>

   <!--
      Look for inherited operations.
   -->
   <xsl:for-each select="$generalizations">
      <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:if test="$superClassNode/@stereotype = $interfaceId and not($superClassNode/@name = 'FactoryBase')">
         <xsl:call-template name="findAllFactoryOperations">
            <xsl:with-param name="_interfaceName" select="$superClassNode/@name"/>
            <xsl:with-param name="_template" select="$_template"/>
            <xsl:with-param name="_isNamed" select="$_isNamed"/>
            <xsl:with-param name="_isServantProvider" select="$_isServantProvider"/>
            <xsl:with-param name="_factoryImplClassname" select="$_factoryImplClassname"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:for-each>
</xsl:template>



</xsl:stylesheet>


