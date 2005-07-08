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
   This template finds all R/W attributes of an interface (Component or Home) 
   as well as those from its superclasses.

   @param _interfaceName   The name of the interface to visit.
   @param _type            The type of interface (Component or Home).
   @param _template        The template to call.
   @param _strategy        The strategy to use when building the interface impl classname.
-->
<xsl:template name="findAllInterfaceReadWriteAttributes">
   <xsl:param name="_interfaceName"/>
   <xsl:param name="_type"/>
   <xsl:param name="_template"/>
   <xsl:param name="_strategy" select="'Session'"/>
   <!--
      Private parameters.
   -->
   <xsl:param name="_interfaceNameSave" select="''"/>
   
   <!--
      References to some nodes.
   -->
   <xsl:variable name="interfaceNode" select="key('classByName', $_interfaceName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="interfaceNameInUse">
      <xsl:choose>
         <xsl:when test="string-length($_interfaceNameSave) > 0">
            <xsl:value-of select="$_interfaceNameSave"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$_interfaceName"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="cppInterfaceScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$interfaceNameInUse"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="interfaceImplClassname" select="concat($_strategy, $interfaceNameInUse, '_impl')"/>
   <xsl:variable name="interfaceObjRefVar" select="concat($cppInterfaceScope, $cppSep, 'CCM_', $interfaceNameInUse, '_var')"/>
   <xsl:variable name="ccmInterfaceId">
      <xsl:choose>
         <xsl:when test="$_type='Component'">
            <xsl:value-of select="key('stereotypeByName', 'CCMComponent')/@xmi.id"/>
         </xsl:when>
         <xsl:when test="$_type='Home'">
            <xsl:value-of select="key('stereotypeByName', 'CCMHome')/@xmi.id"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:message terminate="yes">Unknown type interface</xsl:message>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="readonlyId" select="key('stereotypeByName', 'readonly')/@xmi.id"/>
   <xsl:variable name="readonlyEndId" select="key('stereotypeByName', 'readonlyEnd')/@xmi.id"/>
   <xsl:variable name="umlAttributes" select="$interfaceNode/UML:Namespace.ownedElement/UML:Attribute"/>
   <xsl:variable name="umlAssociations" select="key('associationBySourceId', $interfaceNode/@xmi.id)"/>
   <xsl:variable name="generalizations" select="key('generalizationById', $interfaceNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>

   <xsl:for-each select="$umlAttributes|$umlAssociations">
      <xsl:sort select="@xmi.id"/>
      
      <xsl:variable name="isAssociation" select="boolean(UML:Association.connection)"/>
      <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
      
      <xsl:variable name="selectTypeId">
         <xsl:choose>
            <xsl:when test="$isAssociation">
               <xsl:value-of select="$oppositeEnd/@type"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="@type"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:variable>
      <xsl:variable name="returnType">
         <xsl:call-template name="getType">
            <xsl:with-param name="_id" select="$selectTypeId"/>
            <xsl:with-param name="_kind" select="'return'"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="objRefVarType">
         <xsl:call-template name="getType">
            <xsl:with-param name="_id" select="$selectTypeId"/>
            <xsl:with-param name="_kind" select="'inout'"/>
            <xsl:with-param name="_asObjRefVar" select="true()"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="paramType">
         <xsl:call-template name="getType">
            <xsl:with-param name="_id" select="$selectTypeId"/>
            <xsl:with-param name="_kind" select="'in'"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="paramName">
         <xsl:choose>
            <xsl:when test="$isAssociation">
               <xsl:value-of select="$oppositeEnd/@name"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="@name"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:variable>
      <xsl:variable name="cppOperationName">
         <xsl:call-template name="mapCppReservedWords">
            <xsl:with-param name="_operationName" select="$paramName"/>
         </xsl:call-template>
      </xsl:variable>

      <xsl:choose>
         <xsl:when test="$_template = 'session_component_cpp.content.1'">
            <xsl:call-template name="session_component_cpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.2'">
            <xsl:call-template name="session_component_cpp.content.2">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
               <xsl:with-param name="_paramType" select="$paramType"/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_componentImplClassname" select="$interfaceImplClassname"/>
               <xsl:with-param name="_cppOperationName" select="$cppOperationName"/>
               <xsl:with-param name="_componentObjRefVar" select="$interfaceObjRefVar"/>
               <xsl:with-param name="_objRefVarType" select="$objRefVarType"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.3'">
            <xsl:call-template name="session_component_cpp.content.3">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_hpp.content.4'">
            <xsl:call-template name="session_component_hpp.content.4">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramType" select="$paramType"/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_cppOperationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_hpp.content.5'">
            <xsl:call-template name="session_component_hpp.content.5">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.1'">
            <xsl:call-template name="service_component_cpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.2'">
            <xsl:call-template name="service_component_cpp.content.2">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
               <xsl:with-param name="_paramType" select="$paramType"/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_componentImplClassname" select="$interfaceImplClassname"/>
               <xsl:with-param name="_cppOperationName" select="$cppOperationName"/>
               <xsl:with-param name="_componentObjRefVar" select="$interfaceObjRefVar"/>
               <xsl:with-param name="_objRefVarType" select="$objRefVarType"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.3'">
            <xsl:call-template name="service_component_cpp.content.3">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_hpp.content.4'">
            <xsl:call-template name="service_component_hpp.content.4">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramType" select="$paramType"/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_cppOperationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_hpp.content.5'">
            <xsl:call-template name="service_component_hpp.content.5">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'config_properties_file.content.1'">
            <xsl:call-template name="config_properties_file.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_home_hpp.content.2'">
            <xsl:call-template name="session_home_hpp.content.2">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramType" select="$paramType"/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_cppOperationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_home_cpp.content.2'">
            <xsl:call-template name="session_home_cpp.content.2">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
               <xsl:with-param name="_paramType" select="$paramType"/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_homeImplClassname" select="$interfaceImplClassname"/>
               <xsl:with-param name="_cppOperationName" select="$cppOperationName"/>
               <xsl:with-param name="_homeObjRefVar" select="$interfaceObjRefVar"/>
               <xsl:with-param name="_objRefVarType" select="$objRefVarType"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_home_hpp.content.2'">
            <xsl:call-template name="service_home_hpp.content.2">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramType" select="$paramType"/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_cppOperationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_home_cpp.content.2'">
            <xsl:call-template name="service_home_cpp.content.2">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_readonlyId" select="$readonlyId"/>
               <xsl:with-param name="_readonlyEndId" select="$readonlyEndId"/>
               <xsl:with-param name="_paramName" select="$paramName"/>
               <xsl:with-param name="_paramType" select="$paramType"/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_homeImplClassname" select="$interfaceImplClassname"/>
               <xsl:with-param name="_cppOperationName" select="$cppOperationName"/>
               <xsl:with-param name="_homeObjRefVar" select="$interfaceObjRefVar"/>
               <xsl:with-param name="_objRefVarType" select="$objRefVarType"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:for-each>

   <!--
      Look for inherited attributes.
   -->
   <xsl:for-each select="$generalizations">
      <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:if test="$superClassNode/@stereotype = $ccmInterfaceId">
         <xsl:call-template name="findAllInterfaceReadWriteAttributes">
            <xsl:with-param name="_interfaceName" select="$superClassNode/@name"/>
            <xsl:with-param name="_type" select="$_type"/>
            <xsl:with-param name="_template" select="$_template"/>
            <xsl:with-param name="_strategy" select="$_strategy"/>
            <xsl:with-param name="_interfaceNameSave">
               <xsl:choose>
                  <xsl:when test="string-length($_interfaceNameSave) > 0">
                     <xsl:value-of select="$_interfaceNameSave"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="$_interfaceName"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:if>
   </xsl:for-each>
</xsl:template> <!-- end of template findAllInterfaceReadWriteAttributes -->



<!--
   This template finds all the facets contained in a component as well as
   those from its parents.

   @param _componentName   The name of the component to visit.
   @param _template        The template to call.
   @param _strategy        The strategy to use when building the component impl classname.
-->
<xsl:template name="findAllComponentFacets">
   <xsl:param name="_componentName"/>
   <xsl:param name="_template"/>
   <xsl:param name="_isSegmented" select="'yes'"/>
   <xsl:param name="_strategy" select="'Session'"/>
   <!--
      Private parameters.
   -->
   <xsl:param name="_componentNameSave" select="''"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="componentNameInUse">
      <xsl:choose>
         <xsl:when test="string-length($_componentNameSave) > 0">
            <xsl:value-of select="$_componentNameSave"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$_componentName"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="componentImplClassname" select="concat($_strategy, $componentNameInUse, '_impl')"/>
   <xsl:variable name="ccmProvidesId" select="key('stereotypeByName', 'CCMProvides')/@xmi.id"/>
   <xsl:variable name="providesNodes" select="key('associationByStereotypeId', $ccmProvidesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmComponentId" select="key('stereotypeByName', 'CCMComponent')/@xmi.id"/>
   <xsl:variable name="generalizations" select="key('generalizationById', $componentNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>

   <xsl:for-each select="$providesNodes">
      <xsl:sort select="@xmi.id"/>
         
      <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
      <xsl:variable name="facetNode" select="key('classById', $oppositeEnd/@type)"/>
      <xsl:variable name="facetName" select="$facetNode/@name"/>

      <xsl:choose>
         <xsl:when test="$_template = 'session_component_cpp.content.4'">
            <xsl:call-template name="session_component_cpp.content.4">
               <xsl:with-param name="_facetName" select="$facetNode/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.5'">
            <xsl:call-template name="session_component_cpp.content.5">
               <xsl:with-param name="_facetNode" select="$facetNode"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.6'">
            <xsl:call-template name="session_component_cpp.content.6">
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_isSegmented" select="$_isSegmented"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.7'">
            <xsl:call-template name="session_component_cpp.content.7">
               <xsl:with-param name="_facetTypeName" select="$facetNode/@name"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.8'">
            <xsl:call-template name="session_component_cpp.content.8">
               <xsl:with-param name="_facetTypeName" select="$facetNode/@name"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
               <xsl:with-param name="_isSegmented" select="$_isSegmented"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.9'">
            <xsl:call-template name="session_component_cpp.content.9">
               <xsl:with-param name="_facetTypeName" select="$facetNode/@name"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_hpp.content.1'">
            <xsl:call-template name="session_component_hpp.content.1">
               <xsl:with-param name="_facetName" select="$facetNode/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_hpp.content.2'">
            <xsl:call-template name="session_component_hpp.content.2">
               <xsl:with-param name="_facetTypeName" select="$facetNode/@name"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_isSegmented" select="$_isSegmented"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.4'">
            <xsl:call-template name="service_component_cpp.content.4">
               <xsl:with-param name="_facetName" select="$facetNode/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.5'">
            <xsl:call-template name="service_component_cpp.content.5">
               <xsl:with-param name="_facetNode" select="$facetNode"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.6'">
            <xsl:call-template name="service_component_cpp.content.6">
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.7'">
            <xsl:call-template name="service_component_cpp.content.7">
               <xsl:with-param name="_facetTypeName" select="$facetNode/@name"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.8'">
            <xsl:call-template name="service_component_cpp.content.8">
               <xsl:with-param name="_facetTypeName" select="$facetNode/@name"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.9'">
            <xsl:call-template name="service_component_cpp.content.9">
               <xsl:with-param name="_facetTypeName" select="$facetNode/@name"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_hpp.content.1'">
            <xsl:call-template name="service_component_hpp.content.1">
               <xsl:with-param name="_facetName" select="$facetNode/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_hpp.content.2'">
            <xsl:call-template name="service_component_hpp.content.2">
               <xsl:with-param name="_facetTypeName" select="$facetNode/@name"/>
               <xsl:with-param name="_facetName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:for-each>

   <!--
      Look for inherited facets.
   -->
   <xsl:for-each select="$generalizations">
      <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:if test="$superClassNode/@stereotype = $ccmComponentId">
         <xsl:call-template name="findAllComponentFacets">
            <xsl:with-param name="_componentName" select="$superClassNode/@name"/>
            <xsl:with-param name="_template" select="$_template"/>
            <xsl:with-param name="_isSegmented" select="$_isSegmented"/>
            <xsl:with-param name="_strategy" select="$_strategy"/>
            <xsl:with-param name="_componentNameSave">
               <xsl:choose>
                  <xsl:when test="string-length($_componentNameSave) > 0">
                     <xsl:value-of select="$_componentNameSave"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="$_componentName"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:if>
   </xsl:for-each>
</xsl:template> <!-- end of template findAllComponentFacets -->



<!--
   This template finds all the receptacles contained in a component as well as
   those from its parents.

   @param _componentName   The name of the component to visit.
   @param _template        The template to call.
   @param _strategy        The strategy to use when building the component impl classname.
-->
<xsl:template name="findAllComponentReceptacles">
   <xsl:param name="_componentName"/>
   <xsl:param name="_template"/>
   <xsl:param name="_strategy" select="'Session'"/>
   <!--
      Private parameters.
   -->
   <xsl:param name="_componentNameSave" select="''"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="componentNameInUse">
      <xsl:choose>
         <xsl:when test="string-length($_componentNameSave) > 0">
            <xsl:value-of select="$_componentNameSave"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$_componentName"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="componentImplClassname" select="concat($_strategy, $componentNameInUse, '_impl')"/>
   <xsl:variable name="contextImplClassname" select="concat('CCM_', $componentNameInUse, '_', $_strategy, 'Context_impl')"/>
   <xsl:variable name="ccmUsesId" select="key('stereotypeByName', 'CCMUses')/@xmi.id"/>
   <xsl:variable name="usesNodes" select="key('associationByStereotypeId', $ccmUsesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmComponentId" select="key('stereotypeByName', 'CCMComponent')/@xmi.id"/>
   <xsl:variable name="generalizations" select="key('generalizationById', $componentNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>
   
   <xsl:for-each select="$usesNodes">
      <xsl:sort select="@xmi.id"/>
         
      <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
      <xsl:variable name="receptacleNode" select="key('classById', $oppositeEnd/@type)"/>
      <xsl:variable name="receptacleName" select="$receptacleNode/@name"/>

      <xsl:choose>
         <xsl:when test="$_template = 'session_component_cpp.content.10'">
            <xsl:call-template name="session_component_cpp.content.10">
               <xsl:with-param name="_receptacleNode" select="$receptacleNode"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.11'">
            <xsl:call-template name="session_component_cpp.content.11">
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.12'">
            <xsl:call-template name="session_component_cpp.content.12">
               <xsl:with-param name="_receptacleTypeName" select="$receptacleNode/@name"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_hpp.content.3'">
            <xsl:call-template name="session_component_hpp.content.3">
               <xsl:with-param name="_receptacleTypeName" select="$receptacleNode/@name"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_context_cpp.content.1'">
            <xsl:call-template name="session_context_cpp.content.1">
               <xsl:with-param name="_receptacleTypeName" select="$receptacleNode/@name"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_contextImplClassname" select="$contextImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_context_hpp.content.1'">
            <xsl:call-template name="session_context_hpp.content.1">
               <xsl:with-param name="_receptacleTypeName" select="$receptacleNode/@name"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.10'">
            <xsl:call-template name="service_component_cpp.content.10">
               <xsl:with-param name="_receptacleNode" select="$receptacleNode"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.11'">
            <xsl:call-template name="service_component_cpp.content.11">
               <xsl:with-param name="_receptacleTypeName" select="$receptacleNode/@name"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.12'">
            <xsl:call-template name="service_component_cpp.content.12">
               <xsl:with-param name="_receptacleTypeName" select="$receptacleNode/@name"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_hpp.content.3'">
            <xsl:call-template name="service_component_hpp.content.3">
               <xsl:with-param name="_receptacleTypeName" select="$receptacleNode/@name"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_context_cpp.content.1'">
            <xsl:call-template name="service_context_cpp.content.1">
               <xsl:with-param name="_receptacleTypeName" select="$receptacleNode/@name"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_contextImplClassname" select="$contextImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_context_hpp.content.1'">
            <xsl:call-template name="service_context_hpp.content.1">
               <xsl:with-param name="_receptacleTypeName" select="$receptacleNode/@name"/>
               <xsl:with-param name="_receptacleName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:for-each>

   <!--
      Look for inherited receptacles.
   -->
   <xsl:for-each select="$generalizations">
      <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:if test="$superClassNode/@stereotype = $ccmComponentId">
         <xsl:call-template name="findAllComponentReceptacles">
            <xsl:with-param name="_componentName" select="$superClassNode/@name"/>
            <xsl:with-param name="_template" select="$_template"/>
            <xsl:with-param name="_strategy" select="$_strategy"/>
            <xsl:with-param name="_componentNameSave">
               <xsl:choose>
                  <xsl:when test="string-length($_componentNameSave) > 0">
                     <xsl:value-of select="$_componentNameSave"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="$_componentName"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:if>
   </xsl:for-each>
</xsl:template> <!-- end of template findAllComponentReceptacles -->



<!--
   This template finds all the emitters contained in a component as well as
   those from its parents.

   @param _componentName   The name of the component to visit.
   @param _template        The template to call.
   @param _strategy        The strategy to use when building the component impl classname.
-->
<xsl:template name="findAllComponentEmitters">
   <xsl:param name="_componentName"/>
   <xsl:param name="_template"/>
   <xsl:param name="_strategy" select="'Session'"/>
   <!--
      Private parameters.
   -->
   <xsl:param name="_componentNameSave" select="''"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="componentNameInUse">
      <xsl:choose>
         <xsl:when test="string-length($_componentNameSave) > 0">
            <xsl:value-of select="$_componentNameSave"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$_componentName"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="componentImplClassname" select="concat($_strategy, $componentNameInUse, '_impl')"/>
   <xsl:variable name="contextImplClassname" select="concat('CCM_', $componentNameInUse, '_', $_strategy, 'Context_impl')"/>
   <xsl:variable name="ccmEmitsId" select="key('stereotypeByName', 'CCMEmits')/@xmi.id"/>
   <xsl:variable name="emitsNodes" select="key('associationByStereotypeId', $ccmEmitsId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmComponentId" select="key('stereotypeByName', 'CCMComponent')/@xmi.id"/>
   <xsl:variable name="generalizations" select="key('generalizationById', $componentNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>
   
   <xsl:for-each select="$emitsNodes">
      <xsl:sort select="@xmi.id"/>
         
      <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
      <xsl:variable name="emitterNode" select="key('classById', $oppositeEnd/@type)"/>
      <xsl:variable name="emitterName" select="$emitterNode/@name"/>

      <xsl:choose>
         <xsl:when test="$_template = 'session_context_hpp.content.2'">
            <xsl:call-template name="session_context_hpp.content.2">
               <xsl:with-param name="_eventTypeName" select="$emitterName"/>
               <xsl:with-param name="_eventName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_hpp.content.8'">
            <xsl:call-template name="session_component_hpp.content.8">
               <xsl:with-param name="_emitterTypeName" select="$emitterName"/>
               <xsl:with-param name="_emitterName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_context_cpp.content.2'">
            <xsl:call-template name="session_context_cpp.content.2">
               <xsl:with-param name="_eventTypeName" select="$emitterName"/>
               <xsl:with-param name="_eventName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_contextImplClassname" select="$contextImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.14'">
            <xsl:call-template name="session_component_cpp.content.14">
               <xsl:with-param name="_emitterNode" select="$emitterNode"/>
               <xsl:with-param name="_emitterName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.16'">
            <xsl:call-template name="session_component_cpp.content.16">
               <xsl:with-param name="_emitterName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.18'">
            <xsl:call-template name="session_component_cpp.content.18">
               <xsl:with-param name="_emitterTypeName" select="$emitterName"/>
               <xsl:with-param name="_emitterName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_context_hpp.content.2'">
            <xsl:call-template name="service_context_hpp.content.2">
               <xsl:with-param name="_eventTypeName" select="$emitterName"/>
               <xsl:with-param name="_eventName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_hpp.content.8'">
            <xsl:call-template name="service_component_hpp.content.8">
               <xsl:with-param name="_emitterTypeName" select="$emitterName"/>
               <xsl:with-param name="_emitterName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_context_cpp.content.2'">
            <xsl:call-template name="service_context_cpp.content.2">
               <xsl:with-param name="_eventTypeName" select="$emitterName"/>
               <xsl:with-param name="_eventName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_contextImplClassname" select="$contextImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.14'">
            <xsl:call-template name="service_component_cpp.content.14">
               <xsl:with-param name="_emitterNode" select="$emitterNode"/>
               <xsl:with-param name="_emitterName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.16'">
            <xsl:call-template name="service_component_cpp.content.16">
               <xsl:with-param name="_emitterName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.18'">
            <xsl:call-template name="service_component_cpp.content.18">
               <xsl:with-param name="_emitterTypeName" select="$emitterName"/>
               <xsl:with-param name="_emitterName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:for-each>

   <!--
      Look for inherited receptacles.
   -->
   <xsl:for-each select="$generalizations">
      <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:if test="$superClassNode/@stereotype = $ccmComponentId">
         <xsl:call-template name="findAllComponentEmitters">
            <xsl:with-param name="_componentName" select="$superClassNode/@name"/>
            <xsl:with-param name="_template" select="$_template"/>
            <xsl:with-param name="_strategy" select="$_strategy"/>
            <xsl:with-param name="_componentNameSave">
               <xsl:choose>
                  <xsl:when test="string-length($_componentNameSave) > 0">
                     <xsl:value-of select="$_componentNameSave"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="$_componentName"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:if>
   </xsl:for-each>
</xsl:template> <!-- end of template findAllComponentEmitters -->



<!--
   This template finds all the publishers contained in a component as well as
   those from its parents.

   @param _componentName   The name of the component to visit.
   @param _template        The template to call.
   @param _strategy        The strategy to use when building the component impl classname.
-->
<xsl:template name="findAllComponentPublishers">
   <xsl:param name="_componentName"/>
   <xsl:param name="_template"/>
   <xsl:param name="_strategy" select="'Session'"/>
   <!--
      Private parameters.
   -->
   <xsl:param name="_componentNameSave" select="''"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="componentNameInUse">
      <xsl:choose>
         <xsl:when test="string-length($_componentNameSave) > 0">
            <xsl:value-of select="$_componentNameSave"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$_componentName"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="componentImplClassname" select="concat($_strategy, $componentNameInUse, '_impl')"/>
   <xsl:variable name="contextImplClassname" select="concat('CCM_', $componentNameInUse, '_', $_strategy, 'Context_impl')"/>
   <xsl:variable name="ccmPublishesId" select="key('stereotypeByName', 'CCMPublishes')/@xmi.id"/>
   <xsl:variable name="publishesNodes" select="key('associationByStereotypeId', $ccmPublishesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmComponentId" select="key('stereotypeByName', 'CCMComponent')/@xmi.id"/>
   <xsl:variable name="generalizations" select="key('generalizationById', $componentNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>
   
   <xsl:for-each select="$publishesNodes">
      <xsl:sort select="@xmi.id"/>
         
      <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
      <xsl:variable name="publisherNode" select="key('classById', $oppositeEnd/@type)"/>
      <xsl:variable name="publisherName" select="$publisherNode/@name"/>

      <xsl:choose>
         <xsl:when test="$_template = 'session_context_hpp.content.2'">
            <xsl:call-template name="session_context_hpp.content.2">
               <xsl:with-param name="_eventTypeName" select="$publisherName"/>
               <xsl:with-param name="_eventName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_hpp.content.7'">
            <xsl:call-template name="session_component_hpp.content.7">
               <xsl:with-param name="_publisherTypeName" select="$publisherName"/>
               <xsl:with-param name="_publisherName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_context_cpp.content.2'">
            <xsl:call-template name="session_context_cpp.content.2">
               <xsl:with-param name="_eventTypeName" select="$publisherName"/>
               <xsl:with-param name="_eventName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_contextImplClassname" select="$contextImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.13'">
            <xsl:call-template name="session_component_cpp.content.13">
               <xsl:with-param name="_publisherNode" select="$publisherNode"/>
               <xsl:with-param name="_publisherName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.15'">
            <xsl:call-template name="session_component_cpp.content.15">
               <xsl:with-param name="_publisherName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.17'">
            <xsl:call-template name="session_component_cpp.content.17">
               <xsl:with-param name="_publisherTypeName" select="$publisherName"/>
               <xsl:with-param name="_publisherName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_context_hpp.content.2'">
            <xsl:call-template name="service_context_hpp.content.2">
               <xsl:with-param name="_eventTypeName" select="$publisherName"/>
               <xsl:with-param name="_eventName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_hpp.content.7'">
            <xsl:call-template name="service_component_hpp.content.7">
               <xsl:with-param name="_publisherTypeName" select="$publisherName"/>
               <xsl:with-param name="_publisherName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_context_cpp.content.2'">
            <xsl:call-template name="service_context_cpp.content.2">
               <xsl:with-param name="_eventTypeName" select="$publisherName"/>
               <xsl:with-param name="_eventName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_contextImplClassname" select="$contextImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.13'">
            <xsl:call-template name="service_component_cpp.content.13">
               <xsl:with-param name="_publisherNode" select="$publisherNode"/>
               <xsl:with-param name="_publisherName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.15'">
            <xsl:call-template name="service_component_cpp.content.15">
               <xsl:with-param name="_publisherName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.17'">
            <xsl:call-template name="service_component_cpp.content.17">
               <xsl:with-param name="_publisherTypeName" select="$publisherName"/>
               <xsl:with-param name="_publisherName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:for-each>

   <!--
      Look for inherited receptacles.
   -->
   <xsl:for-each select="$generalizations">
      <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:if test="$superClassNode/@stereotype = $ccmComponentId">
         <xsl:call-template name="findAllComponentPublishers">
            <xsl:with-param name="_componentName" select="$superClassNode/@name"/>
            <xsl:with-param name="_template" select="$_template"/>
            <xsl:with-param name="_strategy" select="$_strategy"/>
            <xsl:with-param name="_componentNameSave">
               <xsl:choose>
                  <xsl:when test="string-length($_componentNameSave) > 0">
                     <xsl:value-of select="$_componentNameSave"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="$_componentName"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:if>
   </xsl:for-each>
</xsl:template> <!-- end of template findAllComponentPublishers -->



<!--
   This template finds all the consumers contained in a component as well as
   those from its parents.

   @param _componentName   The name of the component to visit.
   @param _template        The template to call.
   @param _strategy        The strategy to use when building the component impl classname.
-->
<xsl:template name="findAllComponentConsumers">
   <xsl:param name="_componentName"/>
   <xsl:param name="_template"/>
   <xsl:param name="_isSegmented" select="'yes'"/>
   <xsl:param name="_strategy" select="'Session'"/>
   <!--
      Private parameters.
   -->
   <xsl:param name="_componentNameSave" select="''"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="componentNameInUse">
      <xsl:choose>
         <xsl:when test="string-length($_componentNameSave) > 0">
            <xsl:value-of select="$_componentNameSave"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$_componentName"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="componentImplClassname" select="concat($_strategy, $componentNameInUse, '_impl')"/>
   <xsl:variable name="ccmConsumesId" select="key('stereotypeByName', 'CCMConsumes')/@xmi.id"/>
   <xsl:variable name="consumesNodes" select="key('associationByStereotypeId', $ccmConsumesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmComponentId" select="key('stereotypeByName', 'CCMComponent')/@xmi.id"/>
   <xsl:variable name="generalizations" select="key('generalizationById', $componentNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>
   
   <xsl:for-each select="$consumesNodes">
      <xsl:sort select="@xmi.id"/>
         
      <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
      <xsl:variable name="consumerNode" select="key('classById', $oppositeEnd/@type)"/>
      <xsl:variable name="consumerName" select="$consumerNode/@name"/>

      <xsl:choose>
         <xsl:when test="$_template = 'session_component_hpp.content.6'">
            <xsl:call-template name="session_component_hpp.content.6">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_isSegmented" select="$_isSegmented"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.19'">
            <xsl:call-template name="session_component_cpp.content.19">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.20'">
            <xsl:call-template name="session_component_cpp.content.20">
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentName" select="$componentNameInUse"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.21'">
            <xsl:call-template name="session_component_cpp.content.21">
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.22'">
            <xsl:call-template name="session_component_cpp.content.22">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentName" select="$componentNameInUse"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
               <xsl:with-param name="_isSegmented" select="$_isSegmented"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.23'">
            <xsl:call-template name="session_component_cpp.content.23">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.24'">
            <xsl:call-template name="session_component_cpp.content.24">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_component_cpp.content.25'">
            <xsl:call-template name="session_component_cpp.content.25">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
            </xsl:call-template>
         </xsl:when>
			<xsl:when test="$_template = 'session_component_cpp.content.26'">
            <xsl:call-template name="session_component_cpp.content.26">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_hpp.content.6'">
            <xsl:call-template name="service_component_hpp.content.6">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.19'">
            <xsl:call-template name="service_component_cpp.content.19">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.20'">
            <xsl:call-template name="service_component_cpp.content.20">
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentName" select="$componentNameInUse"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.21'">
            <xsl:call-template name="service_component_cpp.content.21">
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.22'">
            <xsl:call-template name="service_component_cpp.content.22">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_componentImplClassname" select="$componentImplClassname"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.23'">
            <xsl:call-template name="service_component_cpp.content.23">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
               <xsl:with-param name="_consumerName" select="$oppositeEnd/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_component_cpp.content.24'">
            <xsl:call-template name="service_component_cpp.content.24">
               <xsl:with-param name="_consumerTypeName" select="$consumerName"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:for-each>

   <!--
      Look for inherited receptacles.
   -->
   <xsl:for-each select="$generalizations">
      <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:if test="$superClassNode/@stereotype = $ccmComponentId">
         <xsl:call-template name="findAllComponentConsumers">
            <xsl:with-param name="_componentName" select="$superClassNode/@name"/>
            <xsl:with-param name="_template" select="$_template"/>
            <xsl:with-param name="_strategy" select="$_strategy"/>
            <xsl:with-param name="_componentNameSave">
               <xsl:choose>
                  <xsl:when test="string-length($_componentNameSave) > 0">
                     <xsl:value-of select="$_componentNameSave"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="$_componentName"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:if>
   </xsl:for-each>
</xsl:template> <!-- end of template findAllComponentConsumers -->



<!--
   This template finds all the factory operations of a home as well as those
   from its parents.

   @param _homeName   The name of the home to visit.
   @param _template   The template to call.
   @param _strategy   The strategy to use when building the component impl classname.
-->
<xsl:template name="findAllHomeFactoryOperations">
   <xsl:param name="_homeName"/>
   <xsl:param name="_template"/>
   <xsl:param name="_strategy" select="'Session'"/>
   <!--
      Private parameters.
   -->
   <xsl:param name="_homeNameSave" select="''"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="homeNode" select="key('classByName', $_homeName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="homeNameInUse">
      <xsl:choose>
         <xsl:when test="string-length($_homeNameSave) > 0">
            <xsl:value-of select="$_homeNameSave"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$_homeName"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="homeImplClassname" select="concat($_strategy, $homeNameInUse, '_impl')"/>
   <xsl:variable name="ccmHomeId" select="key('stereotypeByName', 'CCMHome')/@xmi.id"/>
   <xsl:variable name="ccmHomeFactoryId" select="key('stereotypeByName', 'CCMHomeFactory')/@xmi.id"/>
   <xsl:variable name="homeFactoryOps" select="$homeNode/UML:Namespace.ownedElement/UML:Operation[@stereotype = $ccmHomeFactoryId]"/>
   <xsl:variable name="generalizations" select="key('generalizationById', $homeNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>
   <xsl:variable name="managedComponentId">
      <xsl:variable name="ccmManagesId" select="key('stereotypeByName', 'CCMManages')/@xmi.id"/>
      <xsl:variable name="homeNodeId" select="key('classByName', $homeNameInUse)/@xmi.id"/>
      <xsl:variable name="managesNode" select="key('associationByStereotypeId', $ccmManagesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $homeNodeId]"/>
      <xsl:value-of select="$managesNode/UML:Association.connection/UML:AssociationEnd[2]/@type"/>
   </xsl:variable>
   <xsl:variable name="managedComponentNode" select="key('classById', $managedComponentId)"/>
   <xsl:variable name="scopedManagedComponentName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$managedComponentNode/@name"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:for-each select="$homeFactoryOps">
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
         <xsl:when test="$_template = 'session_home_hpp.content.1'">
            <xsl:call-template name="session_home_hpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'session_home_cpp.content.1'">
            <xsl:call-template name="session_home_cpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
               <xsl:with-param name="_homeImplClassname" select="$homeImplClassname"/>
               <xsl:with-param name="_managedComponentName" select="$managedComponentNode/@name"/>
               <xsl:with-param name="_scopedManagedComponentName" select="$scopedManagedComponentName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_home_hpp.content.1'">
            <xsl:call-template name="service_home_hpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:when test="$_template = 'service_home_cpp.content.1'">
            <xsl:call-template name="service_home_cpp.content.1">
               <xsl:with-param name="_node" select="."/>
               <xsl:with-param name="_returnType" select="$returnType"/>
               <xsl:with-param name="_operationName" select="$cppOperationName"/>
               <xsl:with-param name="_homeImplClassname" select="$homeImplClassname"/>
               <xsl:with-param name="_managedComponentName" select="$managedComponentNode/@name"/>
               <xsl:with-param name="_scopedManagedComponentName" select="$scopedManagedComponentName"/>
            </xsl:call-template>
         </xsl:when>
      </xsl:choose>
   </xsl:for-each>

   <!--
      Look for inherited factory operations.
   -->
   <xsl:for-each select="$generalizations">
      <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:if test="$superClassNode/@stereotype = $ccmHomeId">
         <xsl:call-template name="findAllHomeFactoryOperations">
            <xsl:with-param name="_homeName" select="$superClassNode/@name"/>
            <xsl:with-param name="_template" select="$_template"/>
            <xsl:with-param name="_strategy" select="$_strategy"/>
            <xsl:with-param name="_homeNameSave">
               <xsl:choose>
                  <xsl:when test="string-length($_homeNameSave) > 0">
                     <xsl:value-of select="$_homeNameSave"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="$_homeName"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:if>
   </xsl:for-each>
</xsl:template> <!-- end of template findAllHomeFactoryOperations -->



</xsl:stylesheet>


