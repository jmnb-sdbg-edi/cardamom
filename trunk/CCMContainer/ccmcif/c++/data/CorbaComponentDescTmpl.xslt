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
   This template generates one or many CORBA component descriptor(s).

   @param none
-->
<xsl:template name="corba_component_desc">
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
         <xsl:variable name="homeId" select="key('classByName', $homeName)/@xmi.id"/>
         <xsl:variable name="ccmManagesId" select="key('stereotypeByName', 'CCMManages')/@xmi.id"/>
         <xsl:variable name="ccmManagesAssociation" select="key('associationBySourceId', $homeId)[@stereotype = $ccmManagesId]"/>
         <xsl:variable name="ccmComponentId" select="key('stereotypeByName', 'CCMComponent')/@xmi.id"/>
         <xsl:variable name="managedComponentId" select="$ccmManagesAssociation/UML:Association.connection/UML:AssociationEnd[@stereotype = $ccmComponentId]/@type"/>
         <xsl:variable name="managedComponentNode" select="key('classById', $managedComponentId)"/>

         <!--
            Get the output filename.
         -->
         <xsl:variable name="filename">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
               <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
               <xsl:with-param name="_defaultFilename" select="$homeName"/>
               <xsl:with-param name="_fileExtension" select="'.ccd'"/>
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
         <xsl:document method="xml" encoding="utf-8" doctype-system="corbacomponent.dtd" href="{$filename}">
            <!--
               Insert the document header.
            -->
            <xsl:call-template name="docHeader">
               <xsl:with-param name="_lang" select="'xml'"/>
            </xsl:call-template>
            <!--
               Insert the content.
            -->
            <xsl:call-template name="corba_component_desc.content">
               <xsl:with-param name="_componentName" select="$managedComponentNode/@name"/>
               <xsl:with-param name="_componentKind" select="translate($homeImplNode/@category, $ucase, $lcase)"/>
               <xsl:with-param name="_homeName" select="$homeName"/>
               <xsl:with-param name="_homeImplClassname">
                  <xsl:call-template name="buildHomeImplClassname">
                     <xsl:with-param name="_homeImplNode" select="$homeImplNode"/>
                  </xsl:call-template>
               </xsl:with-param>
            </xsl:call-template>
         </xsl:document>

         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="corba_component_desc">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template corba_component_desc -->



<!--
   This template generates the content of the file.

   @param _componentName
   @param _componentKind
   @param _homeName
   @param _homeImplClassname
-->
<xsl:template name="corba_component_desc.content">
   <xsl:param name="_componentName"/>
   <xsl:param name="_componentKind"/>
   <xsl:param name="_homeName"/>
   <xsl:param name="_homeImplClassname"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>
   <xsl:variable name="homeNode" select="key('classByName', $_homeName)"/>

   <!--
      The code generation starts here.
   -->

   <xsl:element name="corbacomponent">
      <!--
         ELEMENT : corbaversion.
      -->
      <xsl:element name="corbaversion">
         <xsl:text>3.0</xsl:text>
      </xsl:element>
      <!--
         ELEMENT : componentrepid.
      -->
      <xsl:element name="componentrepid">
         <!--
            Attributes.
         -->
         <xsl:attribute name="repid">
            <xsl:value-of select="$componentNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
         </xsl:attribute>
      </xsl:element>
      <!--
         ELEMENT : homerepid.
      -->
      <xsl:element name="homerepid">
         <!--
            Attributes.
         -->
         <xsl:attribute name="repid">
            <xsl:value-of select="$homeNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
         </xsl:attribute>
      </xsl:element>
      <!--
         ELEMENT : componentKind.
      -->
      <xsl:element name="componentkind">
         <!--
            Child elements.
         -->
         <xsl:element name="{$_componentKind}">
            <xsl:element name="servant">
               <xsl:attribute name="lifetime">
                  <xsl:text>container</xsl:text>
               </xsl:attribute>
            </xsl:element>
         </xsl:element>
      </xsl:element>
      <!--
         ELEMENT : threading.
      -->
      <xsl:element name="threading">
         <!--
            Attributes.
         -->
         <xsl:attribute name="policy">
            <xsl:text>multithread</xsl:text>
         </xsl:attribute>
      </xsl:element>
      <!--
         ELEMENT : configurationcomplete.
      -->
      <xsl:element name="configurationcomplete">
         <!--
            Attributes.
         -->
         <xsl:attribute name="set">
            <xsl:text>true</xsl:text>
         </xsl:attribute>
      </xsl:element>

      <!--
         ELEMENT : homefeatures.
      -->
      <xsl:call-template name="ccd_homefeatures">
         <xsl:with-param name="_homeName" select="$_homeName"/>
      </xsl:call-template>

      <!--
         ELEMENT : componentfeatures.
      -->
      <xsl:call-template name="ccd_componentfeatures">
         <xsl:with-param name="_componentName" select="$_componentName"/>
      </xsl:call-template>

      <!--
         ELEMENT : interface.
      -->
      <xsl:call-template name="ccd_findInterfacesUsedInComponents">
         <xsl:with-param name="_componentName" select="$_componentName"/>
      </xsl:call-template>
   </xsl:element>

   <!--
      The code generation stops here.
   -->
   
</xsl:template> <!-- end of template corba_component_desc.content -->



<!--
   This template generates as much homefeatures elements as necessary.

   @param _homeName   The name of the home to visit.
-->
<xsl:template name="ccd_homefeatures">
   <xsl:param name="_homeName"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_firstVisitedHome" select="''"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="homeNode" select="key('classByName', $_homeName)"/>

   <!--
      Look for the home generalization(s). May contain either the supported interfaces and/or the base home.
   -->
   <xsl:variable name="homeBaseClassGeneralization" select="key('generalizationByChildId', $homeNode/@xmi.id)"/>

   <!--
      Find out if this home has a base home.
   -->
   <xsl:variable name="ccmHomeId" select="key('stereotypeByName', 'CCMHome')/@xmi.id"/>
   <xsl:variable name="baseHomeId">
      <xsl:for-each select="$homeBaseClassGeneralization">
         <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
         <xsl:if test="$superClassNode/@stereotype = $ccmHomeId">
            <xsl:value-of select="UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref"/>
         </xsl:if>
      </xsl:for-each>
   </xsl:variable>
   <xsl:variable name="baseHomeNode" select="key('classById', $baseHomeId)"/>

   <!--
      ELEMENT : homefeatures.
   -->
   <xsl:element name="homefeatures">
      <!--
         Attributes.
      -->
      <xsl:attribute name="name">
         <xsl:value-of select="$homeNode/@name"/>
      </xsl:attribute>
      <xsl:attribute name="repid">
         <xsl:value-of select="$homeNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
      </xsl:attribute>
      <!--
         Child elements.
      -->
      <!--
         Base component.
      -->
      <xsl:if test="boolean($baseHomeNode)">
         <xsl:element name="inheritshome">
            <xsl:attribute name="repid">
               <xsl:value-of select="$baseHomeNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
            </xsl:attribute>
         </xsl:element>
      </xsl:if>
      
      <xsl:variable name="scopedHomeName">
         <xsl:call-template name="getScopedName">
            <xsl:with-param name="_name" select="$_homeName"/>
            <xsl:with-param name="_lang" select="'ada'"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="homeImplNode" select="$lifecycleFrameworkNodes[@factory-idref = $scopedHomeName]/home-impl"/>
      <xsl:if test="boolean($homeImplNode) and string-length($_firstVisitedHome) = 0">
         <xsl:element name="extension">
            <xsl:attribute name="class">
               <xsl:text>HOME_SERVANT_CLASSNAME</xsl:text>
            </xsl:attribute>
            <xsl:attribute name="origin">
               <xsl:text>Cdmw</xsl:text>
            </xsl:attribute>
            <xsl:call-template name="buildHomeImplClassname">
               <xsl:with-param name="_homeImplNode" select="$homeImplNode"/>
            </xsl:call-template>
         </xsl:element>
      </xsl:if>
   </xsl:element>

   <!--
      Visit the base class' super class.
   -->
   <xsl:if test="boolean($baseHomeNode)">
      <xsl:call-template name="ccd_homefeatures">
         <xsl:with-param name="_homeName" select="$baseHomeNode/@name"/>
         <xsl:with-param name="_firstVisitedHome">
            <xsl:choose>
               <xsl:when test="string-length($_firstVisitedHome) = 0">
                  <xsl:value-of select="$_homeName"/>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="$_firstVisitedHome"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:with-param>
      </xsl:call-template>
   </xsl:if>
</xsl:template> <!-- end of template ccd_homefeatures -->



<!--
   This template generates as much componentfeatures elements as necessary.

   @param _componentName   The name of the component to visit.
-->
<xsl:template name="ccd_componentfeatures">
   <xsl:param name="_componentName"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_facettagCounter" select="0"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Look for the component generalization(s). May contain either the supported interfaces and/or the base component.
   -->
   <xsl:variable name="componentBaseClassGeneralization" select="key('generalizationByChildId', $componentNode/@xmi.id)"/>

   <!--
      Find out if this component has a base component.
   -->
   <xsl:variable name="ccmComponentId" select="key('stereotypeByName', 'CCMComponent')/@xmi.id"/>
   <xsl:variable name="baseComponentId">
      <xsl:for-each select="$componentBaseClassGeneralization">
         <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
         <xsl:if test="$superClassNode/@stereotype = $ccmComponentId">
            <xsl:value-of select="UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref"/>
         </xsl:if>
      </xsl:for-each>
   </xsl:variable>
   <xsl:variable name="baseComponentNode" select="key('classById', $baseComponentId)"/>

   <!--
      Find ports.
   -->
   <xsl:variable name="ccmProvidesId" select="key('stereotypeByName', 'CCMProvides')/@xmi.id"/>
   <xsl:variable name="providesNodes" select="key('associationByStereotypeId', $ccmProvidesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmUsesId" select="key('stereotypeByName', 'CCMUses')/@xmi.id"/>
   <xsl:variable name="usesNodes" select="key('associationByStereotypeId', $ccmUsesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmEmitsId" select="key('stereotypeByName', 'CCMEmits')/@xmi.id"/>
   <xsl:variable name="emitsNodes" select="key('associationByStereotypeId', $ccmEmitsId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmPublishesId" select="key('stereotypeByName', 'CCMPublishes')/@xmi.id"/>
   <xsl:variable name="publishesNodes" select="key('associationByStereotypeId', $ccmPublishesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmConsumesId" select="key('stereotypeByName', 'CCMConsumes')/@xmi.id"/>
   <xsl:variable name="consumesNodes" select="key('associationByStereotypeId', $ccmConsumesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>

   <!--
      ELEMENT : componentfeatures.
   -->
   <xsl:element name="componentfeatures">
      <!--
         Attributes.
      -->
      <xsl:attribute name="name">
         <xsl:value-of select="$componentNode/@name"/>
      </xsl:attribute>
      <xsl:attribute name="repid">
         <xsl:value-of select="$componentNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
      </xsl:attribute>
      <!--
         Child elements.
      -->
      <!--
         Base component.
      -->
      <xsl:if test="boolean($baseComponentNode)">
         <xsl:element name="inheritscomponent">
            <xsl:attribute name="repid">
               <xsl:value-of select="$baseComponentNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
            </xsl:attribute>
         </xsl:element>
      </xsl:if>
      <!--
         Supported interfaces.
      -->
      <xsl:for-each select="$componentBaseClassGeneralization">
         <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
         <xsl:if test="not($superClassNode/@stereotype = $ccmComponentId)">
            <xsl:element name="supportsinterface">
               <xsl:attribute name="repid">
                  <xsl:value-of select="$superClassNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
               </xsl:attribute>
            </xsl:element>
         </xsl:if>
      </xsl:for-each>
      <!--
         ELEMENT : ports.
      -->
      <xsl:element name="ports">
         <!--
            Provides.
         -->
         <xsl:for-each select="$providesNodes">
            <xsl:element name="provides">
               <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
               <xsl:attribute name="providesname">
                  <xsl:value-of select="$oppositeEnd/@name"/>
               </xsl:attribute>
               <xsl:attribute name="repid">
                  <xsl:value-of select="key('classById', $oppositeEnd/@type)/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
               </xsl:attribute>
               <xsl:attribute name="facettag">
                  <xsl:value-of select="$_facettagCounter + position()"/>
               </xsl:attribute>
            </xsl:element>
         </xsl:for-each>
         <!--
            Uses.
         -->
         <xsl:for-each select="$usesNodes">
            <xsl:element name="uses">
               <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
               <xsl:attribute name="usesname">
                  <xsl:value-of select="$oppositeEnd/@name"/>
               </xsl:attribute>
               <xsl:attribute name="repid">
                  <xsl:value-of select="key('classById', $oppositeEnd/@type)/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
               </xsl:attribute>
            </xsl:element>
         </xsl:for-each>
         <!--
            Emits.
         -->
         <xsl:for-each select="$emitsNodes">
            <xsl:element name="emits">
               <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
               <xsl:attribute name="emitsname">
                  <xsl:value-of select="$oppositeEnd/@name"/>
               </xsl:attribute>
               <xsl:attribute name="eventtype">
                  <xsl:value-of select="key('classById', $oppositeEnd/@type)/@name"/>
               </xsl:attribute>
               <xsl:element name="eventpolicy">
                  <xsl:attribute name="policy">
                     <xsl:text>default</xsl:text>
                  </xsl:attribute>
               </xsl:element>
            </xsl:element>
         </xsl:for-each>
         <!--
            Publishes.
         -->
         <xsl:for-each select="$publishesNodes">
            <xsl:element name="publishes">
               <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
               <xsl:attribute name="publishesname">
                  <xsl:value-of select="$oppositeEnd/@name"/>
               </xsl:attribute>
               <xsl:attribute name="eventtype">
                  <xsl:value-of select="key('classById', $oppositeEnd/@type)/@name"/>
               </xsl:attribute>
               <xsl:element name="eventpolicy">
                  <xsl:attribute name="policy">
                     <xsl:text>default</xsl:text>
                  </xsl:attribute>
               </xsl:element>
            </xsl:element>
         </xsl:for-each>
         <!--
            Consumes.
         -->
         <xsl:for-each select="$consumesNodes">
            <xsl:element name="consumes">
               <xsl:variable name="oppositeEnd" select="UML:Association.connection/UML:AssociationEnd[2]"/>
               <xsl:attribute name="consumesname">
                  <xsl:value-of select="$oppositeEnd/@name"/>
               </xsl:attribute>
               <xsl:attribute name="eventtype">
                  <xsl:value-of select="key('classById', $oppositeEnd/@type)/@name"/>
               </xsl:attribute>
               <xsl:element name="eventpolicy">
                  <xsl:attribute name="policy">
                     <xsl:text>default</xsl:text>
                  </xsl:attribute>
               </xsl:element>
            </xsl:element>
         </xsl:for-each>
      </xsl:element>
   </xsl:element>

   <!--
      Visit the base class.
   -->
   <xsl:if test="boolean($baseComponentNode)">
      <xsl:call-template name="ccd_componentfeatures">
         <xsl:with-param name="_componentName" select="$baseComponentNode/@name"/>
         <xsl:with-param name="_facettagCounter" select="$_facettagCounter + count($providesNodes)"/>
      </xsl:call-template>
   </xsl:if>
</xsl:template> <!-- end of template ccd_componentfeatures -->



<!--
   This template finds all the interfaces used in the component
   as well as in the base component and in the supported interfaces.

   @param _componentName   The name of the component to visit.
-->
<xsl:template name="ccd_findInterfacesUsedInComponents">
   <xsl:param name="_componentName"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Look for the component generalization(s). May contain either the supported interfaces and/or the base component.
   -->
   <xsl:variable name="componentBaseClassGeneralization" select="key('generalizationByChildId', $componentNode/@xmi.id)"/>

   <!--
      Find out if this component has a base component.
   -->
   <xsl:variable name="ccmComponentId" select="key('stereotypeByName', 'CCMComponent')/@xmi.id"/>
   <xsl:variable name="baseComponentId">
      <xsl:for-each select="$componentBaseClassGeneralization">
         <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
         <xsl:if test="$superClassNode/@stereotype = $ccmComponentId">
            <xsl:value-of select="UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref"/>
         </xsl:if>
      </xsl:for-each>
   </xsl:variable>
   <xsl:variable name="baseComponentNode" select="key('classById', $baseComponentId)"/>

   <!--
      Supported interfaces.
   -->
   <xsl:for-each select="$componentBaseClassGeneralization">
      <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:if test="not($superClassNode/@stereotype = $ccmComponentId)">
         <xsl:call-template name="ccd_visitInterface">
            <xsl:with-param name="_interfaceName" select="$superClassNode/@name"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:for-each>
   <xsl:variable name="visitedSupportedInterfaces">
      <xsl:for-each select="$componentBaseClassGeneralization">
         <xsl:variable name="superClassNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
         <xsl:if test="not($superClassNode/@stereotype = $ccmComponentId)">
            <xsl:value-of select="$superClassNode/@name"/>
         </xsl:if>
      </xsl:for-each>
   </xsl:variable>
   
   <!--
      Ports.
   -->

   <xsl:variable name="ccmProvidesId" select="key('stereotypeByName', 'CCMProvides')/@xmi.id"/>
   <xsl:variable name="providesNodes" select="key('associationByStereotypeId', $ccmProvidesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>
   <xsl:variable name="ccmUsesId" select="key('stereotypeByName', 'CCMUses')/@xmi.id"/>
   <xsl:variable name="usesNodes" select="key('associationByStereotypeId', $ccmUsesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $componentNode/@xmi.id]"/>

   <xsl:call-template name="ccd_visitPorts">
      <xsl:with-param name="_nodes" select="$providesNodes|$usesNodes"/>
      <xsl:with-param name="_visitedInterfaces" select="$visitedSupportedInterfaces"/>
   </xsl:call-template>

   <!--
      Visit the base class.
   -->
   <xsl:if test="boolean($baseComponentNode)">
      <xsl:call-template name="ccd_findInterfacesUsedInComponents">
         <xsl:with-param name="_componentName" select="$baseComponentNode/@name"/>
      </xsl:call-template>
   </xsl:if>
</xsl:template> <!-- end of template ccd_findInterfacesUsedInComponents -->



<!--
   This template visits the facets and receptacles.

   @param _nodes   The ports to visit.
-->
<xsl:template name="ccd_visitPorts">
   <xsl:param name="_nodes"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   <xsl:param name="_visitedInterfaces" select="''"/>

   <xsl:if test="$_index &lt;= count($_nodes)">
      <xsl:variable name="interfaceName" select="key('classById', $_nodes[$_index]/UML:Association.connection/UML:AssociationEnd[2]/@type)/@name"/>

      <!--
         Visit a port only if it has not been visited already.
      -->
      <xsl:if test="not(contains($_visitedInterfaces, $interfaceName))">
         <xsl:call-template name="ccd_visitInterface">
            <xsl:with-param name="_interfaceName" select="$interfaceName"/>
         </xsl:call-template>
      </xsl:if>

      <!--
         Next port.
      -->
      <xsl:call-template name="ccd_visitPorts">
         <xsl:with-param name="_nodes" select="$_nodes"/>
         <xsl:with-param name="_index" select="$_index + 1"/>
         <xsl:with-param name="_visitedInterfaces" select="concat($_visitedInterfaces, ' ', $interfaceName)"/>
      </xsl:call-template>
   </xsl:if>
</xsl:template>



<!--
   This template visits an interface and all its base interfaces.

   @param _interfaceName   The name of the interface to visit.
-->
<xsl:template name="ccd_visitInterface">
   <xsl:param name="_interfaceName"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="interfaceNode" select="key('classByName', $_interfaceName)"/>

   <!--
      Look for base interfaces.
   -->
   <xsl:variable name="interfaceGeneralizations" select="key('generalizationById', $interfaceNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>

   <!--
      ELEMENT : interface.
   -->
   <xsl:element name="interface">
      <!--
         Attributes.
      -->
      <xsl:attribute name="name">
         <xsl:value-of select="$interfaceNode/@name"/>
      </xsl:attribute>
      <xsl:attribute name="repid">
         <xsl:value-of select="$interfaceNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
      </xsl:attribute>
      <!--
         Child elements.
      -->
      <xsl:for-each select="$interfaceGeneralizations">
         <xsl:element name="inheritsinterface">
            <xsl:attribute name="repid">
               <xsl:value-of select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
            </xsl:attribute>
         </xsl:element>
      </xsl:for-each>
   </xsl:element>

   <!--
      Visit the base interfaces (if any).
   -->
   <xsl:for-each select="$interfaceGeneralizations">
      <xsl:call-template name="ccd_visitInterface">
         <xsl:with-param name="_interfaceName" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)/@name"/>
      </xsl:call-template>
   </xsl:for-each>
</xsl:template> <!-- end of template ccd_visitInterface -->



</xsl:stylesheet>


