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
<xsl:template name="service_home_cpp">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>

   <xsl:if test="boolean($lifecycleFrameworkNodes/home-impl) and $lifecycleFrameworkNodes/home-impl/@category = 'SERVICE'">
      <xsl:if test="$_index &lt;= count($lifecycleFrameworkNodes/home-impl)">
         <xsl:variable name="homeImplNode" select="$lifecycleFrameworkNodes[home-impl and position() = $_index]/home-impl"/>

         <!--
            Get the home impl class name.
         -->
         <xsl:variable name="scopedHomeImplClassname">
            <xsl:call-template name="buildHomeImplClassname">
               <xsl:with-param name="_homeImplNode" select="$homeImplNode"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:variable name="homeImplClassname">
            <xsl:call-template name="getLastToken">
               <xsl:with-param name="_string" select="$scopedHomeImplClassname"/>
               <xsl:with-param name="_separator" select="$xmlSep"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the output filename.
         -->
         <xsl:variable name="filename">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
               <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
               <xsl:with-param name="_defaultFilename" select="$homeImplClassname"/>
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
               Insert the document header.
            -->
            <xsl:call-template name="docHeader">
               <xsl:with-param name="_lang" select="'cpp'"/>
            </xsl:call-template>
            <!--
               Insert the content.
            -->
            <xsl:call-template name="service_home_cpp.content">
               <xsl:with-param name="_scopedHomeImplClassname" select="$scopedHomeImplClassname"/>
               <xsl:with-param name="_homeName">
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="$homeImplNode/../@factory-idref"/>
                     <xsl:with-param name="_separator" select="$xmlSep"/>
                  </xsl:call-template>
               </xsl:with-param>
            </xsl:call-template>
         </xsl:document>

         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="service_home_cpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_home_cpp -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _homeName                  The home name.
-->
<xsl:template name="service_home_cpp.content">
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_homeName"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="homeImplClassname">
      <xsl:call-template name="getLastToken">
         <xsl:with-param name="_string" select="$_scopedHomeImplClassname"/>
         <xsl:with-param name="_separator" select="$xmlSep"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppHomeScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_homeName"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="managedComponentId">
      <xsl:variable name="ccmManagesId" select="key('stereotypeByName', 'CCMManages')/@xmi.id"/>
      <xsl:variable name="homeNodeId" select="key('classByName', $_homeName)/@xmi.id"/>
      <xsl:variable name="managesNode" select="key('associationByStereotypeId', $ccmManagesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $homeNodeId]"/>
      <xsl:value-of select="$managesNode/UML:Association.connection/UML:AssociationEnd[2]/@type"/>
   </xsl:variable>
   <xsl:variable name="managedComponentNode" select="key('classById', $managedComponentId)"/>
   <xsl:variable name="scopedManagedComponentName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$managedComponentNode/@name"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="homeNode" select="key('classByName', $_homeName)"/>

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
   
   #include "Foundation/orbsupport/CORBA.hpp"
   #include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

   #include "CCMContainer/ccmcommon/CCMUtils.hpp"
   #include "CCMContainer/idllib/CdmwDeployment.stub.hpp"

   #include "<xsl:value-of select="$pathPrefix"/>Service<xsl:value-of select="$_homeName"/>_impl.hpp"
   #include "<xsl:value-of select="$pathPrefix"/>Service<xsl:value-of select="$managedComponentNode/@name"/>_impl.hpp"

   namespace {
      <xsl:variable name="cdmwHomeRepId">
         <xsl:call-template name="replaceCharsInString">
            <xsl:with-param name="_stringIn" select="$homeNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
            <xsl:with-param name="_charsIn" select="$cppHomeScope"/>
            <xsl:with-param name="_charsOut" select="concat('Cdmw_', $cppHomeScope)"/>
         </xsl:call-template>
      </xsl:variable>
      const char* HOME_REP_ID = "<xsl:value-of select="$cdmwHomeRepId"/>";
      const char* COMP_REP_ID = "<xsl:value-of select="$managedComponentNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>";
   };  // End of anonymous namespace

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   // Constructor 
   <xsl:value-of select="concat($homeImplClassname, $cppSep, $homeImplClassname)"/>
         (Components::HomeExecutorBase_ptr home_exec, 
          const <![CDATA[Cdmw::LifeCycle::ObjectDeactivator&]]> deactivator)
      throw(CORBA::SystemException)
         : CCMHome_impl(HOME_REP_ID, COMP_REP_ID, deactivator)
   {
      try {
         m_home_executor = <xsl:value-of select="concat($cppHomeScope, $cppSep, 'CCM_', $_homeName)"/>::_narrow(home_exec);

         if (CORBA::is_nil(m_home_executor))
         {
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidHomeExecutor, 
                                   CORBA::COMPLETED_NO);
         }
      }
      catch (const <![CDATA[CORBA::Exception &]]>)
      {
         // This is to avoid Assertion failure in debug mode as internal ref count
         // equals 1 (CORBA C++ mapping oblige!)
         _set_ref_count(0);
         throw;
      }
   }

   // Destructor
   <xsl:value-of select="concat($homeImplClassname, $cppSep, '~', $homeImplClassname, '()')"/>
   {
   }

   /**
    * Home attributes
    */

   <xsl:call-template name="findAllInterfaceReadWriteAttributes">
      <xsl:with-param name="_interfaceName" select="$_homeName"/>
      <xsl:with-param name="_type" select="'Home'"/>
      <xsl:with-param name="_template" select="'service_home_cpp.content.2'"/>
   </xsl:call-template>

    /**
    * Explicit interface
    */

   <xsl:call-template name="findAllHomeFactoryOperations">
      <xsl:with-param name="_homeName" select="$_homeName"/>
      <xsl:with-param name="_template" select="'service_home_cpp.content.1'"/>
      <xsl:with-param name="_strategy" select="'Service'"/>
   </xsl:call-template>

   //
   // Implicit interface
   //

   <xsl:value-of select="concat($scopedManagedComponentName, '_ptr', $lf)"/>
   <xsl:value-of select="$homeImplClassname"/>::create()
      throw(Components::CreateFailure,
            CORBA::SystemException)
   {
      CORBA::Object_var obj = create_component_ref();
      <xsl:value-of select="concat($scopedManagedComponentName, '_var')"/> result = <xsl:value-of select="$scopedManagedComponentName"/>::_narrow(obj);

      if (CORBA::is_nil(result))
      {
         PRINT_ERROR("Created reference is not a <xsl:value-of select="$managedComponentNode/@name"/>!");
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
 
      if (!CORBA::is_nil(m_configurator))
      {
          try
          {
              // a configurator exist!
              m_configurator->configure(result.in());
          }
          catch (const <![CDATA[Components::WrongComponentType&]]> )
          {
            throw Components::CreateFailure(CdmwDeployment::WRONG_COMPONENT_KIND);
          }
      }
    
      return result._retn();
   }

   //
   // IDL:omg.org/Components/KeylessCCMHome/create_component:1.0
   //
   Components::CCMObject_ptr
   <xsl:value-of select="$homeImplClassname"/>::create_component()
      throw(Components::CreateFailure,
            CORBA::SystemException)
   {
      return create();
   }

   //
   // IDL:omg.org/CosLifeCycle/LifeCycleObject/remove:1.0
   //
   void
   <xsl:value-of select="$homeImplClassname"/>::remove()
      throw(CosLifeCycle::NotRemovable,
            CORBA::SystemException)
   {
      m_home_executor = <xsl:value-of select="concat($cppHomeScope, $cppSep, 'CCM_', $_homeName, $cppSep, '_nil()')"/>;
      CCMHome_impl::remove();
   }

   //
   // ServantProvider abstract operation implementation
   //

   <![CDATA[
   /**
    * This returns an array containing repository IDs of all objects
    * supported by the current ServantProvider.
    * <B>NOTA:</B> Repository IDs should be statically allocated; thus derived classes
    * should not delete return pointer(s).
    */
   ]]>
   const char ** 
   <xsl:value-of select="$homeImplClassname"/>::get_ids() const
      throw()
   {
      static const char* ids[] = {
         "IDL:omg.org/Components/Events:1.0",
         "IDL:omg.org/Components/Receptacles:1.0",
         "IDL:omg.org/Components/Navigation:1.0",
         "IDL:omg.org/Components/CCMObject:1.0",
         <xsl:call-template name="getObjectList">
            <xsl:with-param name="_nodeName" select="$managedComponentNode/@name"/>
         </xsl:call-template>
         0
      };

      return ids;
   }

   <![CDATA[
   /**
    * This returns the number of repository Ids supported by the current ServantProvider.
    * <B>NOTA:</B> This should correspond to the number of null terminated strings
    * returned by get_ids() operation.
    */
   ]]>
   CORBA::Long 
   <xsl:value-of select="$homeImplClassname"/>::get_number_of_ids() const
      throw()
   {
      <xsl:variable name="countObjIds">
         <xsl:call-template name="getObjectList">
            <xsl:with-param name="_nodeName" select="$managedComponentNode/@name"/>
            <xsl:with-param name="_count" select="true()"/>
         </xsl:call-template>
      </xsl:variable>
   
      static const CORBA::Long nb = <xsl:value-of select="$countObjIds + 4"/>;

      return nb;
   }

   //
   // CdmwCCMHome abstract operation implementation
   //

   <![CDATA[
   /**
    * Purpose:
    * <p>
    *   This creates new component servant.
    *   This shall be implemented by the well typed home servant.
    *
    * @param ctx      The component context
    * @param executor The component executor. It can null if component has been
    *                 created with a standard factory operation.
    * 
    * @return The new component servant.
    */
   ]]>
   PortableServer::Servant 
   <xsl:value-of select="$homeImplClassname"/>::create_component_servant
         (const std::string                  comp_oid,
          CdmwCcmContainer::CCM2Context_ptr ctx,
          Components::EnterpriseComponent*   executor)
      throw(Components::CreateFailure,
            CORBA::SystemException)
   {
      // Check if an executor has been created at ref creation
      // that is a factory operation has been used to create ref.
      Components::EnterpriseComponent_var comp_exec 
         = Components::EnterpriseComponent::_duplicate(executor);
      bool is_created_by_factory_operation = false;

      if (CORBA::is_nil(comp_exec))
      {
         // Create executor
         try
         {
            <![CDATA[comp_exec = m_home_executor->create();]]>
         }
         catch (const <![CDATA[Components::CCMException&]]>)
         {
            PRINT_ERROR("Error occured during component executor creation!");
            throw;
         }
         
         if (CORBA::is_nil(comp_exec))
         {
            PRINT_ERROR("Component executor is nil!");
            throw Components::CreateFailure(CdmwDeployment::INVALID_COMPONENT_EXECUTOR);
         }
      }
      else
      {
         is_created_by_factory_operation = true;
      }

      return new <xsl:value-of select="concat('Service', $managedComponentNode/@name, '_impl')"/>(comp_oid, ctx, comp_exec, is_created_by_factory_operation);
   }

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template service_home_cpp.content -->



<!--
-->
<xsl:template name="service_home_cpp.content.1">
   <xsl:param name="_node"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_operationName"/>
   <xsl:param name="_homeImplClassname"/>
   <xsl:param name="_managedComponentName"/>
   <xsl:param name="_scopedManagedComponentName"/>

   // Factory operation

   <xsl:value-of select="concat($lf, $_returnType, $lf, $_homeImplClassname, $cppSep, $_operationName, $lf)"/>
      (
         <!--
            IN, OUT and INOUT parameters.
         -->
         <xsl:for-each select="$_node/UML:BehavioralFeature.parameter/UML:Parameter[@kind != 'return']">
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
      throw(Components::CreateFailure,
            CORBA::SystemException)
   {
      // Create executor
      Components::EnterpriseComponent_var comp_exec;
      
      try
      {
         comp_exec = <![CDATA[m_home_executor->]]><xsl:value-of select="$_operationName"/> (
            <xsl:for-each select="$_node/UML:BehavioralFeature.parameter/UML:Parameter[@kind != 'return']">
               <xsl:value-of select="@name"/>
               <xsl:if test="position() != last()">
                  <xsl:text>, </xsl:text>
               </xsl:if>
            </xsl:for-each>
            );
      }
      catch (const <![CDATA[Components::CCMException&]]>)
      {
         PRINT_ERROR("Error occured during component executor creation!");
         throw;
      }
       
      if (CORBA::is_nil(comp_exec))
      {
         PRINT_ERROR("Component executor is nil!");
         throw Components::CreateFailure(CdmwDeployment::INVALID_COMPONENT_EXECUTOR);
      }
       
      CORBA::Object_var obj = create_component_ref(comp_exec.in());
      <xsl:value-of select="concat($_scopedManagedComponentName, '_var')"/> result = <xsl:value-of select="$_scopedManagedComponentName"/>::_narrow(obj);
       
      if (CORBA::is_nil(result))
      {
         PRINT_ERROR("Created reference is not a <xsl:value-of select="$_managedComponentName"/>!");
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }

      return result._retn();
   }
</xsl:template> <!-- end of template service_home_cpp.content.1 -->



<!--
-->
<xsl:template name="service_home_cpp.content.2">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramName"/>
   <xsl:param name="_paramType"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_homeImplClassname"/>
   <xsl:param name="_cppOperationName"/>
   <xsl:param name="_homeObjRefVar"/>
   <xsl:param name="_objRefVarType"/>

   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:value-of select="concat($lf, $_returnType, $lf, $_homeImplClassname, $cppSep, $_cppOperationName, '()', $lf)"/>
         throw(CORBA::SystemException)
      {
         <xsl:value-of select="$_objRefVarType"/> result = <![CDATA[m_home_executor->]]><xsl:value-of select="$_cppOperationName"/>();

         <xsl:if test="string-length($_objRefVarType) > 0">
             <xsl:choose>
                 <xsl:when test="contains($_objRefVarType, '_var')">
                     return result._retn();
                 </xsl:when>
                 <xsl:otherwise>
                     return result;
                 </xsl:otherwise>
             </xsl:choose>
         </xsl:if>
      }

      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         void 
         <xsl:value-of select="concat($_homeImplClassname, $cppSep, $_cppOperationName, '(', $_paramType, ' ', $_paramName, ')')"/>
            throw(CORBA::SystemException)
         {
             <![CDATA[m_home_executor->]]><xsl:value-of select="concat($_cppOperationName, '(', $_paramName, ');')"/>
         }
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_home_cpp.content.2 -->

</xsl:stylesheet>


