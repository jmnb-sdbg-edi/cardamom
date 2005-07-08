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
<xsl:template name="ccmcif_facet_cpp">
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
               <xsl:variable name="facetImplClassname" select="concat($facetName, '_impl')"/>
               
               <!--
                  Get the output filename.
               -->
               <xsl:variable name="filename">
                  <xsl:call-template name="buildOutputFilename">
                     <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                     <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                     <xsl:with-param name="_defaultFilename" select="$facetImplClassname"/>
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
                  <xsl:call-template name="ccmcif_facet_cpp.content">
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
         <xsl:call-template name="ccmcif_facet_cpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template ccmcif_facet_cpp -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _facetName                 The facet name.
-->
<xsl:template name="ccmcif_facet_cpp.content">
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_facetName"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="cppScopedFacetName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_facetName"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppFacetScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_facetName"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="facetImplClassname" select="concat($_facetName, '_impl')"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="facetNode" select="key('classByName', $_facetName)"/>

   <!--
      The code generation starts here.
   -->

   <xsl:variable name="pathPrefix">
       <xsl:choose>
           <xsl:when test="string-length($CSC_NAME) > 0">
               <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
           </xsl:when>
           <xsl:otherwise>
               <xsl:value-of select="''"/>
           </xsl:otherwise>
       </xsl:choose>
   </xsl:variable>
   
   <![CDATA[
   #include <Foundation/orbsupport/CORBA.hpp>
   #include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
   #include <CCMContainer/ccmcommon/CCMUtils.hpp>
   ]]>
   #include "<xsl:value-of select="concat($pathPrefix, $facetImplClassname, '.hpp')"/>"

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   <xsl:value-of select="concat($facetImplClassname, $cppSep, $facetImplClassname)"/>(Components::ExecutorLocator_ptr exec_locator, 
                        const char* name,
                        Components::CCM2Context_ptr ctx,
                        CORBA::Object_ptr           comp_ref,
                        PortableServer::Servant     component)
      : LifeCycleObjectBase(ctx),
        m_executor_locator(Components::ExecutorLocator::_duplicate(exec_locator)),
        m_facet_name(name),
        m_comp_ref(CORBA::Object::_duplicate(comp_ref)),
        m_component(component)
   {
      // incrementation refcount of component in order to avoid destruction
      // of the component before the destruction of the facet
      <![CDATA[m_component->_add_ref();]]>
   }

   <xsl:value-of select="concat($facetImplClassname, $cppSep, '~', $facetImplClassname, '() throw()')"/>
   {
      // no remove_ref on component while it's done in var destructor
      PRINT_INFO("<xsl:value-of select="$_facetName"/> destructor called");
   }

   CORBA::Object_ptr
   <xsl:value-of select="$facetImplClassname"/>::_get_component()
   {
      <![CDATA[return CORBA::Object::_duplicate(m_comp_ref.in());]]>
   }

   <xsl:variable name="containedOps"  select="key('operations', $facetNode/@xmi.id)"/>
   <xsl:for-each select="$containedOps">
      <!--
         Get the operation return type.
      -->
      <xsl:variable name="returnType">
         <xsl:call-template name="getType">
            <xsl:with-param name="_id" select="UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'return']/@type"/>
            <xsl:with-param name="_kind" select="'return'"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="objRefVarType">
         <xsl:if test="$returnType != 'void'">
            <xsl:call-template name="getType">
               <xsl:with-param name="_id" select="UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'return']/@type"/>
               <xsl:with-param name="_kind" select="'return'"/>
               <xsl:with-param name="_asObjRefVar" select="true()"/>
            </xsl:call-template>
         </xsl:if>
      </xsl:variable>
      
      <!--
         Get the operation name.
      -->
      <xsl:variable name="cppOperationName">
         <xsl:call-template name="mapCppReservedWords">
            <xsl:with-param name="_operationName" select="@name"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($lf, $returnType, $lf, $facetImplClassname, $cppSep, $cppOperationName, $lf)"/>
         (
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
         )
      throw (
         <!--
            Exceptions.
         -->
         <xsl:for-each select="UML:ModelElement.taggedValue/UML:TaggedValue">
            <xsl:if test="@tag = 'raises'">
               <xsl:value-of select="@value"/>,
            </xsl:if>
         </xsl:for-each>
         CORBA::SystemException
      )
      {
           try
            {
             <![CDATA[CORBA::Object_var obj = m_executor_locator->obtain_executor(m_facet_name.c_str());]]>

            if (CORBA::is_nil(obj.in()))
            {
               throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                             CORBA::COMPLETED_NO);
            }

            <xsl:value-of select="concat($cppFacetScope, $cppSep, 'CCM_', $_facetName, '_var')"/> facet_exec = <xsl:value-of select="concat($cppFacetScope, $cppSep, 'CCM_', $_facetName)"/>::_narrow(obj.in());
    
            if (CORBA::is_nil(facet_exec.in()))
            {
               throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                             CORBA::COMPLETED_NO);
            }
    
            <xsl:if test="string-length($objRefVarType) > 0">
                <xsl:value-of select="concat($objRefVarType, ' result =')"/>
            </xsl:if>
            <![CDATA[facet_exec->]]><xsl:value-of select="$cppOperationName"/>
               (
                  <xsl:for-each select="UML:BehavioralFeature.parameter/UML:Parameter[@kind != 'return']">
                     <xsl:value-of select="@name"/>
                     <xsl:if test="position() != last()">
                        <xsl:text>, </xsl:text>
                     </xsl:if>
                  </xsl:for-each>
               );

            <![CDATA[m_executor_locator->release_executor(facet_exec.in());]]>

            <xsl:if test="string-length($objRefVarType) > 0">
               <xsl:choose>
                  <xsl:when test="contains($objRefVarType, '_var')">
                     return result._retn();
                  </xsl:when>
                  <xsl:otherwise>
                     return result;
                  </xsl:otherwise>
                </xsl:choose>
            </xsl:if>
         }
            catch (const <![CDATA[Components::CCMException&]]> )
            {
               throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                  CORBA::COMPLETED_NO);
            }
       }
   </xsl:for-each>

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template ccmcif_facet_cpp.content -->



</xsl:stylesheet>


