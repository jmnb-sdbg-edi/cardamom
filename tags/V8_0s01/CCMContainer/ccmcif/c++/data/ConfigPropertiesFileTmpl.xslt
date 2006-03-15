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
   This template generates one or many CORBA properties file(s).

   @param none
-->
<xsl:template name="config_properties_file">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>

   <xsl:if test="boolean($lifecycleFrameworkNodes/home-impl)">
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

         <!--
            Get the CPF filename for the managed component.
         -->
         <xsl:variable name="managedComponentCPF">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
               <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
               <xsl:with-param name="_defaultFilename" select="$managedComponentNode/@name"/>
               <xsl:with-param name="_fileExtension" select="'.cpf'"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Display a message to the user.
         -->
         <xsl:message>
            <xsl:value-of select="concat('Generating file ', $managedComponentCPF)"/>
         </xsl:message>

         <!--
            Generate the file.
         -->
         <xsl:document method="xml" encoding="utf-8" doctype-system="properties.dtd" href="{$managedComponentCPF}">
            <!--
               Insert the document header.
            -->
            <xsl:call-template name="docHeader">
               <xsl:with-param name="_lang" select="'xml'"/>
            </xsl:call-template>
            <!--
               Insert the content.
            -->
            <xsl:call-template name="config_properties_file.content">
               <xsl:with-param name="_name" select="$managedComponentNode/@name"/>
               <xsl:with-param name="_type" select="'Component'"/>
               
            </xsl:call-template>
         </xsl:document>

         <!--
            Get the CPF filename for the home.
         -->
         <xsl:variable name="homeCPF">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
               <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
               <xsl:with-param name="_defaultFilename" select="$homeName"/>>
               <xsl:with-param name="_fileExtension" select="'.cpf'"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Display a message to the user.
         -->
         <xsl:message>
            <xsl:value-of select="concat('Generating file ', $homeCPF)"/>
         </xsl:message>

         <!--
            Generate the file.
         -->
         <xsl:document method="xml" encoding="utf-8" doctype-system="properties.dtd" href="{$homeCPF}">
            <!--
               Insert the document header.
            -->
            <xsl:call-template name="docHeader">
               <xsl:with-param name="_lang" select="'xml'"/>
            </xsl:call-template>
            <!--
               Insert the content.
            -->
            <xsl:call-template name="config_properties_file.content">
               <xsl:with-param name="_name" select="$homeName"/>
               <xsl:with-param name="_type" select="'Home'"/>
            </xsl:call-template>
         </xsl:document>
         
         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="config_properties_file">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template config_properties_file -->



<!--
   This template generates the content of the file.

   @param _name
-->
<xsl:template name="config_properties_file.content">
   <xsl:param name="_name"/>
   <xsl:param name="_type"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="node" select="key('classByName', $_name)"/>

   <!--
      The code generation starts here.
   -->

   <xsl:element name="properties">
      <xsl:call-template name="findAllInterfaceReadWriteAttributes">
         <xsl:with-param name="_interfaceName" select="$_name"/>
         <xsl:with-param name="_type" select="$_type"/>
         <xsl:with-param name="_template" select="'config_properties_file.content.1'"/>
      </xsl:call-template>
   </xsl:element>

   <!--
      The code generation stops here.
   -->
   
</xsl:template> <!-- end of template config_properties_file.content -->



<!--
   This template tells if an attribute is either a simple, a sequence, a struct or a valuetype.

   @param _typeId   The type id.
-->
<xsl:template name="cpf_attributeKind">
   <xsl:param name="_typeId"/>

   <xsl:variable name="stereotypeName" select="key('stereotypeById', $_typeId)/@name"/>
   <xsl:if test="$stereotypeName = 'CORBAboolean' or $stereotypeName = 'CORBAchar' or $stereotypeName = 'CORBAdouble' or
                 $stereotypeName = 'CORBAfloat' or $stereotypeName = 'CORBAshort' or $stereotypeName = 'CORBAlong' or
                 $stereotypeName = 'CORBAoctet' or $stereotypeName = 'CORBAstring' or $stereotypeName = 'CORBAunsignedlong' or
                 $stereotypeName = 'CORBAunsignedshort'">
      <xsl:text>simple</xsl:text>
   </xsl:if>

   <xsl:variable name="classNode" select="key('classById', $_typeId)"/>
   <xsl:if test="boolean($classNode)">
      <xsl:variable name="classStereotypeName" select="key('stereotypeById', $classNode/@stereotype)/@name"/>

      <xsl:choose>
         <xsl:when test="$classStereotypeName = 'CORBAInterface' or $classStereotypeName = 'CORBAEnum'">
            <xsl:text>simple</xsl:text>
         </xsl:when>
         <xsl:when test="$classStereotypeName = 'CORBASequence'">
            <xsl:text>sequence</xsl:text>
         </xsl:when>
         <xsl:when test="$classStereotypeName = 'CORBAStruct'">
            <xsl:text>struct</xsl:text>
         </xsl:when>
         <xsl:when test="$classStereotypeName = 'CORBAValue'">
            <xsl:text>valuetype</xsl:text>
         </xsl:when>
      </xsl:choose>
   </xsl:if>
</xsl:template> <!-- end of template cpf_attributeKind -->



<!--
   This template describes an element using an appropriate template.

   @param _name     The name of the element.
   @param _typeId   The type id.
-->
<xsl:template name="cpf_describe_element">
   <xsl:param name="_name"/>
   <xsl:param name="_typeId"/>

   <xsl:variable name="kind">
      <xsl:call-template name="cpf_attributeKind">
         <xsl:with-param name="_typeId" select="$_typeId"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:choose>
      <xsl:when test="$kind = 'simple'">
         <xsl:call-template name="cpf_describe_simple">
            <xsl:with-param name="_name" select="$_name"/>
            <xsl:with-param name="_typeId" select="$_typeId"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:when test="$kind = 'sequence'">
         <xsl:call-template name="cpf_describe_sequence">
            <xsl:with-param name="_name" select="$_name"/>
            <xsl:with-param name="_typeId" select="$_typeId"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:when test="$kind = 'struct'">
         <xsl:call-template name="cpf_describe_struct">
            <xsl:with-param name="_name" select="$_name"/>
            <xsl:with-param name="_typeId" select="$_typeId"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:when test="$kind = 'valuetype'">
         <xsl:call-template name="cpf_describe_valuetype">
            <xsl:with-param name="_name" select="$_name"/>
            <xsl:with-param name="_typeId" select="$_typeId"/>
         </xsl:call-template>
      </xsl:when>
   </xsl:choose>
</xsl:template> <!-- end of template cpf_describe_element -->



<!--
   This templates returns a simple type string for a given stereotype.

   @param _stereotypeName   The name of the stereotype.
-->
<xsl:template name="cpf_simpleTypeString">
   <xsl:param name="_stereotypeName"/>
   
   <xsl:choose>
      <xsl:when test="$_stereotypeName = 'CORBAboolean'">
         <xsl:text>boolean</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAchar'">
         <xsl:text>char</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAdouble'">
         <xsl:text>double</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAfloat'">
         <xsl:text>float</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAshort'">
         <xsl:text>short</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAlong'">
         <xsl:text>long</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAInterface'">
         <xsl:text>objref</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAoctet'">
         <xsl:text>octet</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAstring' or $_stereotypeName = 'CORBAEnum'">
         <xsl:text>string</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAunsignedlong'">
         <xsl:text>ulong</xsl:text>
      </xsl:when>
      <xsl:when test="$_stereotypeName = 'CORBAunsignedshort'">
         <xsl:text>ushort</xsl:text>
      </xsl:when>
   </xsl:choose>
</xsl:template> <!-- end of template cpf_simpleTypeString -->



<!--
   This template describes a simple type.

   @param _name     The name of the element.
   @param _typeId   The type id.
-->
<xsl:template name="cpf_describe_simple">
   <xsl:param name="_name"/>
   <xsl:param name="_typeId"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="stereotypeName" select="key('stereotypeById', $_typeId)/@name"/>

   <!--
      ELEMENT : simple.
   -->
   <xsl:element name="simple">
      <!--
         Attributes.
      -->
      <xsl:if test="string-length($_name) > 0">
         <xsl:attribute name="name">
            <xsl:value-of select="$_name"/>
         </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="type">
         <xsl:call-template name="cpf_simpleTypeString">
            <xsl:with-param name="_stereotypeName">
               <xsl:choose>
                  <xsl:when test="string-length($stereotypeName) > 0">
                     <xsl:value-of select="$stereotypeName"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="key('stereotypeById', key('classById', $_typeId)/@stereotype)/@name"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:attribute>
      <!--
         Child elements.
      -->
      <xsl:element name="value"/>
   </xsl:element>
</xsl:template> <!-- end of template cpf_describe_simple -->



<!--
   This template describes a sequence type.

   @param _name     The name of the element.
   @param _typeId   The type id.
-->
<xsl:template name="cpf_describe_sequence">
   <xsl:param name="_name"/>
   <xsl:param name="_typeId"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="classNode" select="key('classById', $_typeId)"/>
   <xsl:variable name="seqElemAssoc" select="key('associationBySourceId', $classNode/@xmi.id)"/>
   <xsl:variable name="elemTypeId" select="$seqElemAssoc/UML:Association.connection/UML:AssociationEnd[2]/@type"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="upperBoundValue" select="$seqElemAssoc/UML:Association.connection/UML:AssociationEnd//UML:TaggedValue[@tag = 'constraintUpperValue']/@value"/>
   <xsl:variable name="sequenceElementType">
      <xsl:variable name="simpleTypeNode" select="key('stereotypeById', $elemTypeId)"/>
      <xsl:choose>
         <xsl:when test="boolean($simpleTypeNode)">
            <xsl:call-template name="cpf_simpleTypeString">
               <xsl:with-param name="_stereotypeName" select="$simpleTypeNode/@name"/>
            </xsl:call-template>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="key('classById', $elemTypeId)/@name"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="elemTypeString">
      <xsl:choose>
         <xsl:when test="$upperBoundValue = '*'">
            <xsl:value-of select="concat('sequence&lt;', $sequenceElementType,'&gt;')"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="concat('sequence&lt;', $sequenceElementType, ',', $upperBoundValue, '&gt;')"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>

   <!--
      ELEMENT : sequence.
   -->
   <xsl:element name="sequence">
      <!--
         Attributes.
      -->
      <xsl:if test="string-length($_name) > 0">
         <xsl:attribute name="name">
            <xsl:value-of select="$_name"/>
         </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="type">
         <xsl:value-of select="$elemTypeString"/>
      </xsl:attribute>
      <!--
         Child elements.
      -->
      <xsl:choose>
         <!--
            This test prevents the template to loop indefinitely.
            If it's a recursive sequence then we stop at depth 2.
         -->
         <xsl:when test="$elemTypeId = $_typeId">
            <xsl:element name="sequence">
               <xsl:attribute name="type">
                  <xsl:value-of select="$elemTypeString"/>
               </xsl:attribute>
            </xsl:element>
         </xsl:when>
         <xsl:otherwise>
            <xsl:call-template name="cpf_describe_element">
               <xsl:with-param name="_typeId" select="$elemTypeId"/>
            </xsl:call-template>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:element>
</xsl:template> <!-- end of template cpf_describe_sequence -->



<!--
   This template describes a struct type.

   @param _name     The name of the element.
   @param _typeId   The type id.
-->
<xsl:template name="cpf_describe_struct">
   <xsl:param name="_name"/>
   <xsl:param name="_typeId"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="classNode" select="key('classById', $_typeId)"/>

   <!--
      ELEMENT : struct.
   -->
   <xsl:element name="struct">
      <!--
         Attributes.
      -->
      <xsl:if test="string-length($_name) > 0">
         <xsl:attribute name="name">
            <xsl:value-of select="$_name"/>
         </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="type">
         <xsl:value-of select="$classNode/@name"/>
      </xsl:attribute>
      <!--
         Child elements.
      -->
      <xsl:for-each select="$classNode/UML:Namespace.ownedElement/UML:Attribute|key('associationBySourceId', $classNode/@xmi.id)">
         <!--
            Sort the elements using xmi.id as primary sort key as we want to generate
            the child elements in the IDL order.
         -->
         <xsl:sort select="@xmi.id"/>

         <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
         
         <xsl:choose>
            <!--
               This test prevents the template to loop indefinitely.
               If it's a recursive struct then we stop at depth 2.
            -->
            <xsl:when test="$oppositeEnd/@type = $_typeId or @type = $_typeId">
               <xsl:element name="struct">
                  <xsl:if test="string-length($oppositeEnd/@name) > 0 or string-length(@name) > 0">
                     <xsl:attribute name="name">
                        <xsl:choose>
                           <xsl:when test="string-length(@name) > 0">
                              <xsl:value-of select="@name"/>
                           </xsl:when>
                           <xsl:otherwise>
                              <xsl:value-of select="$oppositeEnd/@name"/>
                           </xsl:otherwise>
                        </xsl:choose>
                     </xsl:attribute>
                  </xsl:if>
                  <xsl:attribute name="type">
                     <xsl:value-of select="$classNode/@name"/>
                  </xsl:attribute>
               </xsl:element>
            </xsl:when>
            <xsl:otherwise>
               <xsl:choose>
                  <xsl:when test="boolean(UML:Association.connection)">
                     <xsl:call-template name="cpf_describe_element">
                        <xsl:with-param name="_name" select="$oppositeEnd/@name"/>
                        <xsl:with-param name="_typeId" select="$oppositeEnd/@type"/>
                     </xsl:call-template>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:call-template name="cpf_describe_element">
                        <xsl:with-param name="_name" select="@name"/>
                        <xsl:with-param name="_typeId" select="@type"/>
                     </xsl:call-template>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:for-each>
   </xsl:element>
</xsl:template> <!-- end of template cpf_describe_struct -->



<!--
   This template describes a valuetype type.

   @param _name     The name of the element.
   @param _typeId   The type id.
-->
<xsl:template name="cpf_describe_valuetype">
   <xsl:param name="_name"/>
   <xsl:param name="_typeId"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="classNode" select="key('classById', $_typeId)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="pk">
      <xsl:call-template name="valuetypeIsPrimaryKey">
         <xsl:with-param name="_typeId" select="$classNode/@xmi.id"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="generalizations" select="key('generalizationByChildId', $classNode/@xmi.id)"/>

   <!--
      ELEMENT : valuetype.
   -->
   <xsl:element name="valuetype">
      <!--
         Attributes.
      -->
      <xsl:if test="string-length($_name) > 0">
         <xsl:attribute name="name">
            <xsl:value-of select="$_name"/>
         </xsl:attribute>
      </xsl:if>
      <xsl:attribute name="type">
         <xsl:value-of select="$classNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
      </xsl:attribute>
      <xsl:attribute name="primarykey">
         <xsl:value-of select="$pk"/>
      </xsl:attribute>
      <!--
         Child elements.
      -->
      <xsl:call-template name="cpf_describe_valuetype_attributes">
         <xsl:with-param name="_nodes" select="$classNode/UML:Namespace.ownedElement/UML:Attribute[@visibility = 'public']|key('associationBySourceId', $classNode/@xmi.id)[@visibility = 'public']"/>
         <xsl:with-param name="_typeId" select="$_typeId"/>
      </xsl:call-template>
      <!--
         Inherited attributes.
      -->
      <xsl:variable name="valueId" select="key('stereotypeByName', 'CORBAValue')/@xmi.id"/>
      <xsl:for-each select="$generalizations">
         <xsl:variable name="parentNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>

         <xsl:if test="$parentNode/@stereotype = $valueId">
            <xsl:call-template name="cpf_describe_valuetype_attributes">
               <xsl:with-param name="_nodes" select="$parentNode/UML:Namespace.ownedElement/UML:Attribute[@visibility = 'public']|key('associationBySourceId', $parentNode/@xmi.id)[@visibility = 'public']"/>
               <xsl:with-param name="_typeId" select="$parentNode/@xmi.id"/>
            </xsl:call-template>
         </xsl:if>
      </xsl:for-each>
   </xsl:element>
</xsl:template> <!-- end of template cpf_describe_valuetype -->



<!--
   This template describes the attributes of a value type.

   @param _nodes    The attributes to visit.
   @param _typeId   The type id.
-->
<xsl:template name="cpf_describe_valuetype_attributes">
   <xsl:param name="_nodes"/>
   <xsl:param name="_typeId"/>

   <xsl:for-each select="$_nodes">
      <!--
         Sort the elements using xmi.id as primary sort key as we want to generate
         the child elements in the IDL order.
      -->
      <xsl:sort select="@xmi.id"/>
      
      <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>

      <xsl:choose>
         <!--
            This test prevents the template to loop indefinitely.
            If it's a recursive valuetype then we stop at depth 2.
         -->
         <xsl:when test="$oppositeEnd/@type = $_typeId or @type = $_typeId">
            <xsl:element name="valuetype">
               <xsl:if test="string-length($oppositeEnd/@name) > 0 or string-length(@name) > 0">
                  <xsl:attribute name="name">
                     <xsl:choose>
                        <xsl:when test="string-length(@name) > 0">
                           <xsl:value-of select="@name"/>
                        </xsl:when>
                        <xsl:otherwise>
                           <xsl:value-of select="$oppositeEnd/@name"/>
                        </xsl:otherwise>
                     </xsl:choose>
                  </xsl:attribute>
               </xsl:if>
               <xsl:attribute name="type">
                  <xsl:value-of select="$classNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
               </xsl:attribute>
               <xsl:attribute name="primarykey">
                  <xsl:value-of select="$pk"/>
               </xsl:attribute>
            </xsl:element>
         </xsl:when>
         <xsl:otherwise>
            <xsl:choose>
               <xsl:when test="boolean(UML:Association.connection)">
                  <xsl:call-template name="cpf_describe_element">
                     <xsl:with-param name="_name" select="$oppositeEnd/@name"/>
                     <xsl:with-param name="_typeId" select="$oppositeEnd/@type"/>
                  </xsl:call-template>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:call-template name="cpf_describe_element">
                     <xsl:with-param name="_name" select="@name"/>
                     <xsl:with-param name="_typeId" select="@type"/>
                  </xsl:call-template>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:for-each>
</xsl:template> <!-- end of template cpf_describe_valuetype_attributes -->



<!--
-->
<xsl:template name="config_properties_file.content.1">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyEndId"/>
   
   <xsl:choose>
      <xsl:when test="boolean($_node/UML:Association.connection)">
         <xsl:if test="not(boolean($_node/@stereotype) or ($_node/@stereotype = $_readonlyEndId))">
            <xsl:variable name="oppositeEnd" select="$_node/UML:Association.connection/UML:AssociationEnd[2]"/>
            <xsl:call-template name="cpf_describe_element">
               <xsl:with-param name="_name" select="$oppositeEnd/@name"/>
               <xsl:with-param name="_typeId" select="$oppositeEnd/@type"/>
            </xsl:call-template>
         </xsl:if>
      </xsl:when>
      <xsl:otherwise>
         <xsl:if test="not(boolean($_node/@stereotype))">
            <xsl:call-template name="cpf_describe_element">
               <xsl:with-param name="_name" select="$_node/@name"/>
               <xsl:with-param name="_typeId" select="$_node/@type"/>
            </xsl:call-template>
         </xsl:if>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template config_properties_file.content.1 -->



</xsl:stylesheet>


