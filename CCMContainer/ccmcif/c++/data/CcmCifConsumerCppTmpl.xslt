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
<xsl:template name="ccmcif_consumer_cpp">
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
               Find all the consumers.
            -->
            <xsl:variable name="ccmConsumesId" select="key('stereotypeByName', 'CCMConsumes')/@xmi.id"/>
            <xsl:variable name="consumers" select="key('associationByStereotypeId', $ccmConsumesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $managedComponentNode/@xmi.id]"/>
         
            <xsl:for-each select="$consumers">
               <xsl:sort select="@xmi.id"/>

               <xsl:variable name="consumerTypeName" select="key('classById', UML:Association.connection/UML:AssociationEnd[2]/@type)/@name"/>
               <xsl:variable name="consumerClassname" select="concat($consumerTypeName, 'Consumer')"/>
               <xsl:variable name="consumerImplClassname" select="concat($consumerClassname, '_impl')"/>
               
               <!--
                  Get the output filename.
               -->
               <xsl:variable name="filename">
                  <xsl:call-template name="buildOutputFilename">
                     <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                     <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                     <xsl:with-param name="_defaultFilename" select="$consumerImplClassname"/>
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
                  <xsl:call-template name="ccmcif_consumer_cpp.content">
                     <xsl:with-param name="_scopedHomeImplClassname">
                        <xsl:call-template name="buildHomeImplClassname">
                           <xsl:with-param name="_homeImplNode" select="$homeImplNode"/>
                        </xsl:call-template>
                     </xsl:with-param>
                     <xsl:with-param name="_consumerTypeName" select="$consumerTypeName"/>
                     <xsl:with-param name="_consumerClassname" select="$consumerClassname"/>
                     <xsl:with-param name="_consumerVarName" select="UML:Association.connection/UML:AssociationEnd[2]/@name"/>
                     <xsl:with-param name="_componentName" select="$managedComponentNode/@name"/>
                  </xsl:call-template>
               </xsl:document>
            </xsl:for-each>
         </xsl:if>

         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="ccmcif_consumer_cpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template ccmcif_consumer_cpp -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _consumerTypeName          The consumer type.
   @param _consumerClassname         The consumer class name.
   @param _consumerVarName           The name of the consumer as declared in IDL.
   @param _componentName             The name of the component where the consumer is declared.
-->
<xsl:template name="ccmcif_consumer_cpp.content">
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerClassname"/>
   <xsl:param name="_consumerVarName"/>
   <xsl:param name="_componentName"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="consumerImplClassname" select="concat($_consumerClassname, '_impl')"/>
   <xsl:variable name="cppScopedConsumerTypeName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_consumerTypeName"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppScopedConsumerClassname" select="concat($cppScopedConsumerTypeName, 'Consumer')"/>
   <xsl:variable name="consumerNamespace">
      <xsl:variable name="homeImplClassname">
         <xsl:call-template name="getLastToken">
            <xsl:with-param name="_string" select="$_scopedHomeImplClassname"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:call-template name="getLastToken">
         <xsl:with-param name="_string" select="substring-before($_scopedHomeImplClassname, concat($xmlSep, $homeImplClassname))"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppConsumerScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_consumerTypeName"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="lowercaseComponentName" select="translate($_componentName, $ucase, $lcase)"/>
   <xsl:variable name="lowercaseConsumerTypeName" select="translate($_consumerTypeName, $ucase, $lcase)"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="consumerNode" select="key('classByName', $_consumerTypeName)"/>

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
   #include "<xsl:value-of select="concat($pathPrefix, $consumerImplClassname, '.hpp')"/>"
   
   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   <xsl:value-of select="concat($consumerImplClassname, $cppSep, $consumerImplClassname)"/>(Components::ExecutorLocator_ptr exec_locator,
                        const char* name,
                        Components::CCM2Context_ptr ctx,
                        PortableServer::Servant     component)
      : LifeCycleObjectBase(ctx),
        m_executor_locator(Components::ExecutorLocator::_duplicate(exec_locator)),
        m_sink_name(name),
        m_component(component)
   {
      // incrementation refcount of component in order to avoid destruction
      // of the component before the destruction of the facet
      <![CDATA[m_component->_add_ref();]]>
   }

   <xsl:value-of select="concat($consumerImplClassname, $cppSep, '~', $consumerImplClassname, '() throw()')"/>
   {
      // no remove_ref on component while it's done in var destructor
      PRINT_INFO("<xsl:value-of select="$_consumerClassname"/> destructor called");
   }

   void
   <xsl:value-of select="concat($consumerImplClassname, $cppSep, 'push_', $_consumerTypeName, 
      '(', $cppScopedConsumerTypeName, '* the_', $lowercaseConsumerTypeName, ')')"/>
      throw (CORBA::SystemException)
   {
       try
        {
         <![CDATA[CORBA::Object_var obj = m_executor_locator->obtain_executor(m_sink_name.c_str());]]>
    
         if (CORBA::is_nil(obj.in()))
         {
            throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                          CORBA::COMPLETED_NO);
         }
    
         // For monolithic component, push operation is implemented in <xsl:value-of select="$_componentName"/> component
         // instead of the consumer executor for segmented component
         <xsl:value-of select="concat($cppConsumerScope, $cppSep, 'CCM_', $_componentName, '_var ', $lowercaseComponentName, '_exec = ', 
                                      $cppConsumerScope, $cppSep, 'CCM_', $_componentName, $cppSep, '_narrow(obj.in());')"/>

         if (CORBA::is_nil(<xsl:value-of select="concat($lowercaseComponentName, '_exec.in()')"/>))
         {
            // Component may segmented
            <xsl:value-of select="concat($cppConsumerScope, $cppSep, 'CCM_', $_consumerClassname, '_var consumer_exec = ',
                                         $cppConsumerScope, $cppSep, 'CCM_', $_consumerClassname, $cppSep, '_narrow(obj.in());')"/>
        
            if (CORBA::is_nil(consumer_exec.in()))
            {
               throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                             CORBA::COMPLETED_NO);
            }
            else
            {
               // push event to the consumer executor
               <![CDATA[consumer_exec->push]]>(the_<xsl:value-of select="$lowercaseConsumerTypeName"/>);   
            }
         }
         else
         {
            <xsl:value-of select="$lowercaseComponentName"/><![CDATA[_exec->push_]]><xsl:value-of select="concat($_consumerVarName, '(the_', $lowercaseConsumerTypeName,');')"/>
         }
    
         <![CDATA[m_executor_locator->release_executor(obj.in());]]>
      }
        catch (const <![CDATA[Components::CCMException&]]> )
        {
            throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                               CORBA::COMPLETED_NO);
        }
   }

   void 
   <xsl:value-of select="$consumerImplClassname"/>::push_event(Components::EventBase * evt)
      throw (Components::BadEventType, CORBA::SystemException)
   {
      <xsl:value-of select="$cppScopedConsumerTypeName"/>_var info_evt 
         = <xsl:value-of select="$cppScopedConsumerTypeName"/>::_downcast(evt);
      CORBA::add_ref(info_evt.in());

      if (!info_evt.in())
      {
         throw Components::BadEventType();
      }
      else
      {
         <![CDATA[this->push_]]><xsl:value-of select="$_consumerTypeName"/>(info_evt.in());
      }
   }

   void 
   <xsl:value-of select="$consumerImplClassname"/>::push(const <![CDATA[CORBA::Any& data]]>)
      throw (CosEventComm::Disconnected, 
             CORBA::SystemException)
   {
      <xsl:value-of select="$cppScopedConsumerTypeName"/>_var event;
      <![CDATA[data >>= event;]]>
      CORBA::add_ref(event.in());

      try
      {
         <![CDATA[this->push_]]><xsl:value-of select="$_consumerTypeName"/>(event.in());
      }
      catch (const <![CDATA[CORBA::SystemException&]]>)
      {
         throw;
      }
   }

   void
   <xsl:value-of select="$consumerImplClassname"/>::disconnect_push_consumer()
      throw (CORBA::SystemException)
   {
   }
   
   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template ccmcif_consumer_cpp.content -->



</xsl:stylesheet>


