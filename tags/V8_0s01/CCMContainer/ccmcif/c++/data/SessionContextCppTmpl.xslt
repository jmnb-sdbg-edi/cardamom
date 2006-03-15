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
<xsl:template name="session_context_cpp">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>

   <xsl:if test="boolean($lifecycleFrameworkNodes/home-impl) and $lifecycleFrameworkNodes/home-impl/@category = 'SESSION'">
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
            <xsl:variable name="contextImplClassname" select="concat('CCM_', $managedComponentNode/@name, '_SessionContext_impl')"/>

            <!--
               Get the output filename.
            -->
            <xsl:variable name="filename">
               <xsl:call-template name="buildOutputFilename">
                  <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                  <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                  <xsl:with-param name="_defaultFilename" select="$contextImplClassname"/>
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
               Generate the file.
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
               <xsl:call-template name="session_context_cpp.content">
                  <xsl:with-param name="_contextImplClassname" select="$contextImplClassname"/>
                  <xsl:with-param name="_homeName" select="$homeName" />
                  <xsl:with-param name="_scopedHomeImplClassname">
                     <xsl:call-template name="buildHomeImplClassname">
                        <xsl:with-param name="_homeImplNode" select="$homeImplNode"/>
                     </xsl:call-template>
                  </xsl:with-param>
                  <xsl:with-param name="_componentName" select="$managedComponentNode/@name"/>
               </xsl:call-template>
            </xsl:document>
         </xsl:if>

         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="session_context_cpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template session_context_cpp -->



<!--
   This template generates the content of the file.

   @param _contextImplClassname      The session context impl classname.
   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _componentName             The component name.
-->
<xsl:template name="session_context_cpp.content">
   <xsl:param name="_contextImplClassname"/>
   <xsl:param name="_homeName"/>
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_componentName"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="cppComponentScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_componentName"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppHomeScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_homeName"/>
      </xsl:call-template>
   </xsl:variable>

   <!-- PCR-0049 -->
   <xsl:variable name="eventTypes">
      <xsl:variable name="eventTypes_">
         <xsl:call-template name="findAllComponentEmitters">
            <xsl:with-param name="_componentName" select="$_componentName" />
            <xsl:with-param name="_template" select="'session_context_cpp.content.3'" />
         </xsl:call-template>

         <xsl:call-template name="findAllComponentPublishers">
            <xsl:with-param name="_componentName" select="$_componentName" />
            <xsl:with-param name="_template" select="'session_context_cpp.content.3'" />
         </xsl:call-template>
      </xsl:variable>

      <xsl:call-template name="removeDuplicateTokens">
         <xsl:with-param name="_string" select="$eventTypes_" />
      </xsl:call-template>
   </xsl:variable>

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

   <!--
      The code generation starts here.
   -->

   <![CDATA[
   #include <Foundation/orbsupport/CORBA.hpp>
   #include <Foundation/orbsupport/Codec.hpp>
   #include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
   ]]>
   #include "<xsl:value-of select="concat($pathPrefix, $_contextImplClassname)"/>.hpp"

   <!-- PCR-0049 -->
  <xsl:call-template name="session_context_cpp.content.4">
     <xsl:with-param name="_eventTypes" select="$eventTypes"/>
  </xsl:call-template>

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="concat('Cdmw::CCM::CIF::Cdmw',$cppHomeScope)"/>
      <xsl:with-param name="_separator" select="$cppSep"/>
      <xsl:with-param name="_lastTokenIsNamespace" select="true()"/>
   </xsl:call-template>

   <xsl:value-of select="concat($lf, $_contextImplClassname, $cppSep, $_contextImplClassname, $lf)"/>
         (Cdmw::CCM::CIF::Context*          ctx,
          ::Cdmw::CCM::CIF::CCMObject_impl* component)
      throw (CORBA::SystemException)
         : SessionContext_impl(<![CDATA[ctx->get_ccm_context()]]>),
           m_component(component)
   {
   }

   <xsl:value-of select="concat($lf, $_contextImplClassname, $cppSep, '~', $_contextImplClassname, '()', $lf)"/>
   {
   }

   <!--
      Receptacles.
   -->

   <xsl:call-template name="findAllComponentReceptacles">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_context_cpp.content.1'"/>
   </xsl:call-template>

   <!--
      Emitters.
   -->

   <xsl:call-template name="findAllComponentEmitters">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_context_cpp.content.2'"/>
   </xsl:call-template>

   <!--
      Publishers.
   -->

   <xsl:call-template name="findAllComponentPublishers">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_context_cpp.content.2'"/>
   </xsl:call-template>

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="concat('Cdmw::CCM::CIF::Cdmw',$cppHomeScope)"/>
      <xsl:with-param name="_separator" select="$cppSep"/>
      <xsl:with-param name="_lastTokenIsNamespace" select="true()"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template session_context_cpp.content -->



<!--
-->
<xsl:template name="session_context_cpp.content.1">
   <xsl:param name="_receptacleTypeName"/>
   <xsl:param name="_receptacleName"/>
   <xsl:param name="_contextImplClassname"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_receptacleTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:value-of select="concat($lf, $scopedReturnType, '_ptr', $lf,
                                $_contextImplClassname, $cppSep, 'get_connection_', $_receptacleName, '()', $lf)"/>
      throw(CORBA::SystemException)
   {
      Components::ConnectionDescriptions_var comp_desc;

      try
      {
         comp_desc = 
            <xsl:value-of select="concat('m_component->get_connections(&quot;', $_receptacleName, '&quot;);')"/>
      }
      catch (const <![CDATA[Components::InvalidName&]]>)
      {
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, 
                               CORBA::COMPLETED_NO);   
      }
      catch (const <![CDATA[CORBA::SystemException&]]> )
      {
         throw;
      }

      <xsl:value-of select="concat($scopedReturnType, '_var')"/> recep;
      if (<![CDATA[comp_desc->]]>length() == 1)
      {
         <![CDATA[CORBA::Object_var obj = CORBA::Object::_duplicate(comp_desc->operator[](0)->objref());]]>

         recep = <xsl:value-of select="$scopedReturnType"/>::_narrow(obj.in());
      }
      else
      {
         recep = <xsl:value-of select="$scopedReturnType"/>::_nil();
      }

      return recep._retn();
   }
</xsl:template> <!-- end of template session_context_cpp.content.1 -->



<!--
-->
<xsl:template name="session_context_cpp.content.2">
   <xsl:param name="_eventTypeName"/>
   <xsl:param name="_eventName"/>
   <xsl:param name="_contextImplClassname"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_eventTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <!-- PCR-0049 -->
   <xsl:variable name="eventScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_eventTypeName"/>
      </xsl:call-template>
   </xsl:variable>


   <xsl:value-of select="concat($lf, 'void', $lf, $_contextImplClassname, $cppSep, 'push_', $_eventName, '(', $scopedReturnType, '* event', ')',
                                $lf, 'throw(CORBA::SystemException)', $lf)"/>
   {
      // insert event into an any
      /* PCR-0049
      CORBA::Any data;
      <![CDATA[data <<= event;]]>
      */
      Cdmw::OrbSupport::Codec<xsl:value-of select="concat('&lt;', $scopedReturnType, '&gt;')"/> codec;
      CORBA::OctetSeq_var encoded_data = codec.encode(event);
      CORBA::Any data;
      <![CDATA[data <<= encoded_data;]]>

      // find proxy to push event
      CosEventComm::PushConsumer_var proxy_consumer =
         <![CDATA[m_component->get_proxy_consumer]]>("<xsl:value-of select="$_eventName"/>");

      if (CORBA::is_nil(proxy_consumer.in()))
      {
          throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }

      try
      {
          <![CDATA[proxy_consumer->push(data);]]>
      }
      catch (<![CDATA[const CosEventComm::Disconnected &]]>)
      {
          throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
   }
</xsl:template> <!-- end of template session_context_cpp.content.2 -->


<!-- PCR-0049 -->
<!--
-->
<!-- TAO-JacORB interop workaround -->
<xsl:template name="session_context_cpp.content.3">
   <xsl:param name="_eventTypeName" />

   <xsl:variable name="scopedEventType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_eventTypeName" />
      </xsl:call-template>
   </xsl:variable>

   <xsl:text></xsl:text><xsl:value-of select="$scopedEventType"/><xsl:text> </xsl:text>
</xsl:template> <!-- end of template session_context_cpp.content.3 -->


<!-- PCR-0049 -->
<!--
-->
<xsl:template name="session_context_cpp.content.4">
   <xsl:param name="_eventTypes" />
   <xsl:param name="_sep" select="' '" />

   <xsl:if test="string-length($_eventTypes) > 0">
      <xsl:choose>
         <xsl:when test="starts-with($_eventTypes, $_sep)">
            <xsl:call-template name="session_context_cpp.content.4">
               <xsl:with-param name="_eventTypes"
                              select="substring-after($_eventTypes, $_sep)" />
            </xsl:call-template>
         </xsl:when>
         <xsl:otherwise>
            <xsl:choose>
               <xsl:when test="contains($_eventTypes, $_sep)">
                  <xsl:value-of select="concat('DEFINE_VALUETYPE_DATA_TRAITS(',
                                             substring-before($_eventTypes, $_sep),
                                             ')', $lf)" />

                  <xsl:call-template name="session_context_cpp.content.4">
                     <xsl:with-param name="_eventTypes"
                                    select="substring-after($_eventTypes, $_sep)" />
                  </xsl:call-template>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="concat('DEFINE_VALUETYPE_DATA_TRAITS(',
                                               $_eventTypes, ')', $lf)" />
               </xsl:otherwise>
            </xsl:choose>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:if>
</xsl:template> <!-- end of template session_context_cpp.content.4 -->


</xsl:stylesheet>
