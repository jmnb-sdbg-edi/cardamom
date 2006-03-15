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
<xsl:template name="ccmcif_facet_java">
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

         <xsl:if test="boolean($managedComponentNode)">
            <!--
               Find all the facets.
            -->
            <xsl:variable name="ccmProvidesId" select="key('stereotypeByName', 'CCMProvides')/@xmi.id"/>
            <xsl:variable name="facets" select="key('associationByStereotypeId', $ccmProvidesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $managedComponentNode/@xmi.id]"/>
         
            <xsl:for-each select="$facets">
               <xsl:sort select="@xmi.id"/>

               <xsl:variable name="facetName" select="key('classById', UML:Association.connection/UML:AssociationEnd[2]/@type)/@name"/>
               <xsl:variable name="facetImplClassname" select="concat($facetName, 'Impl')"/>
               
               <!--
                  Get the output filename.
               -->
               <xsl:variable name="filename">
                  <xsl:call-template name="buildOutputFilename">
                     <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                     <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                     <xsl:with-param name="_defaultFilename" select="$facetImplClassname"/>
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
                  Generate file.
               -->
               <xsl:document method="text" href="{$filename}">
                  <!--
                     Insert document header.
                  -->
                  <xsl:call-template name="docHeader">
                     <xsl:with-param name="_lang" select="'java'"/>
                  </xsl:call-template>

                  <!--
                     Insert the content.
                  -->
                  <xsl:call-template name="ccmcif_facet_java.content">
                     <xsl:with-param name="_scopedHomeImplClassname">
                        <xsl:call-template name="buildHomeImplClassname">
                           <xsl:with-param name="_homeImplNode" select="$homeImplNode"/>
                        </xsl:call-template>
                     </xsl:with-param>
                     <xsl:with-param name="_facetName" select="$facetName"/>
                  </xsl:call-template>

               </xsl:document>
            </xsl:for-each>
         </xsl:if>

         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="ccmcif_facet_java">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template ccmcif_facet_java.content -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _facetName                 The facet name.
-->
<xsl:template name="ccmcif_facet_java.content">
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_facetName"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="javaScopedFacetName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_facetName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="javaFacetScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$javaScopedFacetName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="facetNamespace">
      <xsl:variable name="homeImplClassname">
         <xsl:call-template name="getLastToken">
            <xsl:with-param name="_string" select="$_scopedHomeImplClassname"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:call-template name="getLastToken">
         <xsl:with-param name="_string" select="substring-before($_scopedHomeImplClassname, concat($xmlSep, $homeImplClassname))"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="javaScopedFacetPOAName">
      <xsl:variable name="newScopedFacetName">
         <xsl:call-template name="prefixModuleName">
            <xsl:with-param name="_prefix" select="'Cdmw_'"/>
            <xsl:with-param name="_idlPrefix" select="$idlPragmaPrefix"/>
            <xsl:with-param name="_scopedName" select="$javaScopedFacetName"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($newScopedFacetName, 'POA')"/>
   </xsl:variable>


   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="facetImplClassname" select="concat($_facetName, 'Impl')"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="facetNode" select="key('classByName', $_facetName)"/>

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
import cdmw.ccm.cif.LifeCycleObjectBase;

public class <xsl:value-of select="$facetImplClassname"/> 
    extends <xsl:value-of select="$javaScopedFacetPOAName"/> {

    private org.omg.Components.ExecutorLocator executorLocator;
    
    private String facetName;

    private org.omg.PortableServer.Servant component;
    
    private LifeCycleObjectBase lifecycleObjectBase;

    public <xsl:value-of select="$facetImplClassname"/>(
        org.omg.Components.ExecutorLocator execLocator, 
        String name,
        org.omg.Components.CCM2Context ctx,
        org.omg.PortableServer.Servant component) {

        this.lifecycleObjectBase = new LifeCycleObjectBase(ctx);
        this.executorLocator = execLocator;
        this.facetName = name;
        this.component = component;
        
    }

   <!--
      Create operation declarations.
   -->
   <xsl:variable name="containedOps"  select="key('operations', $facetNode/@xmi.id)"/>
   <xsl:for-each select="$containedOps">
      <!--
         Get the operation return type.
      -->
      <xsl:variable name="returnType">
         <xsl:call-template name="getType">
            <xsl:with-param name="_id" select="UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'return']/@type"/>
            <xsl:with-param name="_kind" select="'return'"/>
            <xsl:with-param name="_lang" select="'java'"/>
         </xsl:call-template>
      </xsl:variable>


      <!--
         Operation signature.
      -->
      <xsl:variable name="javaOperationName">
         <xsl:call-template name="mapJavaReservedWords">
            <xsl:with-param name="_operationName" select="@name"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($lf, '    public ', $returnType, ' ', $javaOperationName, '(', $lf)"/>
            <!--
               IN, OUT and INOUT parameters.
            -->
            <xsl:for-each select="UML:BehavioralFeature.parameter/UML:Parameter[@kind != 'return']">
               <!--
                  Get the parameter type.
               -->
               <xsl:variable name="paramType">
                  <xsl:call-template name="getParameterType">
                     <xsl:with-param name="_paramNode" select="."/>
                     <xsl:with-param name="_lang" select="'java'"/>
                  </xsl:call-template>
               </xsl:variable>
               <xsl:value-of select="concat($paramType, ' ', @name)"/>
               <!--
                  Write out comma if necessary.
               -->
               <xsl:if test="position() != last()">
                  <xsl:value-of select="concat(',', $lf)"/>
               </xsl:if>
            </xsl:for-each>
         <xsl:value-of select="'    )'"/>
         <!--
            Exceptions.
         -->
         <xsl:if test="count(UML:ModelElement.taggedValue/UML:TaggedValue[@tag='raises']) > 0">
            <xsl:value-of select="concat(' throws', $lf)"/>
         </xsl:if>
         <xsl:for-each select="UML:ModelElement.taggedValue/UML:TaggedValue">
            <xsl:if test="@tag = 'raises'">
               <xsl:variable name="scopedException">
                  <xsl:call-template name="getScopedException">
                     <xsl:with-param name="_exception" select="@value"/>
                  </xsl:call-template>
               </xsl:variable>
               <xsl:value-of select="concat('        ', $scopedException)"/>
               <xsl:if test="position() != last()">
                  <xsl:value-of select="concat(',', $lf)"/>
               </xsl:if>
            </xsl:if>
         </xsl:for-each>
         <xsl:value-of select="concat(' {', $lf)"/>

        try {
            org.omg.CORBA.Object obj = this.executorLocator.obtain_executor(this.facetName);

            if (obj == null) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST, 
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
   
            <xsl:value-of select="concat($javaFacetScope, $javaSep, 'CCM_', $_facetName)"/> facetExec = <xsl:value-of    select="concat($javaFacetScope, $javaSep, 'CCM_', $_facetName)"/>Helper.narrow(obj);
   
            if (facetExec == null) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST, 
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
       
            <xsl:if test="not($returnType = 'void')">
               <xsl:value-of select="concat($returnType, ' result =')"/>
            </xsl:if>
            facetExec.<xsl:value-of select="$javaOperationName"/>(
               <xsl:for-each select="UML:BehavioralFeature.parameter/UML:Parameter[@kind != 'return']">
                  <xsl:value-of select="@name"/>
                  <xsl:if test="position() != last()">
                     <xsl:text>, </xsl:text>
                  </xsl:if>
               </xsl:for-each>
            );

            this.executorLocator.release_executor(facetExec);

	    <xsl:if test="not($returnType = 'void')">
            return result;
            </xsl:if>
        } catch(org.omg.Components.CCMException ccme) {
           throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALCCMCIFError, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }


    }
   </xsl:for-each>
   
   // methods from LifeCycleObjectBase to delegate operations
    
    public org.omg.CosLifeCycle.LifeCycleObject copy(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws
            org.omg.CosLifeCycle.NoFactory,
            org.omg.CosLifeCycle.NotCopyable,
            org.omg.CosLifeCycle.InvalidCriteria,
            org.omg.CosLifeCycle.CannotMeetCriteria {
        return this.lifecycleObjectBase.copy(there, the_criteria);
    }
    
    public void move(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws
            org.omg.CosLifeCycle.NoFactory,
            org.omg.CosLifeCycle.NotMovable,
            org.omg.CosLifeCycle.InvalidCriteria,
            org.omg.CosLifeCycle.CannotMeetCriteria {
        this.lifecycleObjectBase.move(there, the_criteria);
    }

    public void remove() throws org.omg.CosLifeCycle.NotRemovable {
        this.lifecycleObjectBase.remove();
    }

}

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template ccmcif_facet_java.content -->



</xsl:stylesheet>


