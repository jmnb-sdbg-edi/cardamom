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
<xsl:template name="entity_factory_hpp">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   
   <!--
      References to some nodes.
   -->
   <xsl:variable name="nodes" select="$lifecycleFrameworkNodes[factory-impl/@category = 'ENTITY']"/>

   <xsl:if test="boolean($nodes)">
      <xsl:if test="$_index &lt;= count($nodes)">
         <xsl:variable name="lifecycleFrameworkNode" select="$nodes[$_index]"/>

         <xsl:variable name="factoryNode" select="$factoryNodes[@idl-interface = $lifecycleFrameworkNode/@factory-idref]"/>

         <!--
            Get the (scoped) idl interface name.
         -->
         <xsl:variable name="scopedInterfaceName" select="$factoryNode/@idl-interface"/>

         <!--
            Get the factory impl classname.
         -->
         <xsl:variable name="scopedFactoryImplClassname" select="$lifecycleFrameworkNode/factory-impl/@classname"/>
         <xsl:variable name="factoryImplClassname">
            <xsl:call-template name="getLastToken">
               <xsl:with-param name="_string" select="$scopedFactoryImplClassname"/>
               <xsl:with-param name="_separator" select="$xmlSep"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the (scoped) servant provider classname.
         -->
         <xsl:variable name="scopedServantProviderClassname" select="$factoryNode/servant-provider/@classname"/>

         <!--
            Get the output filename.
         -->
         <xsl:variable name="filename">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$lifecycleFrameworkNode/factory-impl/@directory"/>
               <xsl:with-param name="_userFilename" select="$lifecycleFrameworkNode/factory-impl/@filename"/>
               <xsl:with-param name="_defaultFilename" select="$factoryImplClassname"/>
               <xsl:with-param name="_fileExtension" select="'.hpp'"/>
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
               Insert #ifndef #define instructions.
            -->
            <xsl:call-template name="writeDefine">
               <xsl:with-param name="_prefix" select="'RESOURCES'"/>
               <xsl:with-param name="_classname" select="$factoryImplClassname"/>
            </xsl:call-template>

            <!--
               Insert the content.
            -->
            <xsl:call-template name="entity_factory_hpp.content">
               <xsl:with-param name="_scopedInterfaceName" select="$scopedInterfaceName"/>
               <xsl:with-param name="_interfaceFilename">
                  <xsl:if test="string-length($CSC_NAME) > 0">
                     <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
                  </xsl:if>
                  <xsl:choose>
                     <xsl:when test="boolean($factoryNode/@idl-filename)">
                        <xsl:value-of select="substring-before($factoryNode/@idl-filename, '.idl')"/>
                     </xsl:when>
                     <xsl:otherwise>
                        <xsl:call-template name="getLastToken">
                           <xsl:with-param name="_string" select="$scopedInterfaceName"/>
                           <xsl:with-param name="_separator" select="$xmlSep"/>
                        </xsl:call-template>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:with-param>
               <xsl:with-param name="_servantProviderClassname">
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="$scopedServantProviderClassname"/>
                     <xsl:with-param name="_separator" select="$xmlSep"/>
                  </xsl:call-template>
               </xsl:with-param>
               <xsl:with-param name="_scopedServantProviderClassname" select="$scopedServantProviderClassname"/>
               <xsl:with-param name="_factoryImplClassname" select="$factoryImplClassname"/>
               <xsl:with-param name="_scopedFactoryImplClassname" select="$scopedFactoryImplClassname"/>
            </xsl:call-template>

            <!--
               Insert #endif instructions.
            -->
            <xsl:call-template name="writeDefine">
               <xsl:with-param name="_prefix" select="'RESOURCES'"/>
               <xsl:with-param name="_classname" select="$factoryImplClassname"/>
               <xsl:with-param name="_endif" select="true()"/>
            </xsl:call-template>
         </xsl:document>

         <!--
            Proceed to the next item if any.
         -->
         <xsl:call-template name="entity_factory_hpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template entity_factory_hpp -->



<!--
   This template generates the content of the file.

   @param _scopedInterfaceName              The scoped named interface name.
   @param _interfaceFilename                The named interface filename.
   @param _servantProviderClassname         The servant provider classname.
   @param _scopedServantProviderClassname   The scoped servant provider classname.
   @param _factoryImplClassname             The factory impl classname.
   @param _scopedFactoryImplClassname       The scoped factory impl classname.
-->
<xsl:template name="entity_factory_hpp.content">
   <xsl:param name="_scopedInterfaceName"/>
   <xsl:param name="_interfaceFilename"/>
   <xsl:param name="_servantProviderClassname"/>
   <xsl:param name="_scopedServantProviderClassname"/>
   <xsl:param name="_factoryImplClassname"/>
   <xsl:param name="_scopedFactoryImplClassname"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="interfaceName">
      <xsl:call-template name="getLastToken">
         <xsl:with-param name="_string" select="$_scopedInterfaceName"/>
         <xsl:with-param name="_separator" select="$xmlSep"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppScopedInterfaceName">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_scopedInterfaceName"/>
         <xsl:with-param name="_charsIn" select="$xmlSep"/>
         <xsl:with-param name="_charsOut" select="$cppSep"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppScopedServantProviderClassname">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_scopedServantProviderClassname"/>
         <xsl:with-param name="_charsIn" select="$xmlSep"/>
         <xsl:with-param name="_charsOut" select="$cppSep"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="interfaceNode" select="key('classByName', $interfaceName)"/>

   <!--
      The code generation starts here.
   -->

   #include "Foundation/orbsupport/CORBA.hpp"
   #include "Foundation/common/Exception.hpp"
   #include "<xsl:value-of select="$_interfaceFilename"/>.skel.hpp"
   #include "LifeCycle/idllib/CdmwLifeCycle.stub.hpp"
   #include "LifeCycle/lifecycle/RetainObjectDeactivator.hpp"
   #include "LifeCycle/lifecycle/EntityCleanupNotifier.hpp"
   #include "Foundation/orbsupport/POAThreadingPolicy.hpp"
   #include "Foundation/orbsupport/RefCountServantBase.hpp"

   <![CDATA[
      #include <string>
      #include <memory>
   ]]>

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedServantProviderClassname"/>
   </xsl:call-template>

   // Forward declarations
   class <xsl:value-of select="$_servantProviderClassname"/>;

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedServantProviderClassname"/>
   </xsl:call-template>

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedFactoryImplClassname"/>
   </xsl:call-template>

   class <xsl:value-of select="$_factoryImplClassname"/> :
      public virtual POA_<xsl:value-of select="$cppScopedInterfaceName"/>,
      public virtual Cdmw::OrbSupport::RefCountServantBase
   {
      public:
         <xsl:value-of select="$_factoryImplClassname"/>
         <![CDATA[
            (CORBA::ORB_ptr                           orb,
             PortableServer::POA_ptr                  parent,
             CdmwLifeCycle::ObjectRegistration_ptr   name_domain,
             const std::string &                      name_domain_name,
             const std::string &                      factory_name,
             Cdmw::OrbSupport::POAThreadingPolicy &   threading_policy,
             CosPropertyService::PropertySet_ptr      property_set,
             int & argc, char** argv)
            throw(Cdmw::OutOfMemoryException,
                  Cdmw::BadParameterException,
                  Cdmw::InternalErrorException,
                  CORBA::SystemException);
         ]]>

         virtual
         ~<xsl:value-of select="$_factoryImplClassname"/> ()
            throw();

         virtual PortableServer::POA_ptr _default_POA() {
            return PortableServer::POA::_duplicate(m_poa.in());
         }

         <!--
            Create the operation declarations.
         -->
         <xsl:call-template name="findAllFactoryOperations">
            <xsl:with-param name="_interfaceName" select="$interfaceName"/>
            <xsl:with-param name="_template" select="'entity_factory_hpp.content.1'"/>
            <xsl:with-param name="_isNamed" select="true()"/>
            <xsl:with-param name="_factoryImplClassname" select="$_factoryImplClassname"/>
         </xsl:call-template>

         //
         // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
         //
         virtual CORBA::Boolean supports(const char* repository_id)
            throw(CORBA::SystemException);

         //
         // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
         //
         virtual CdmwLifeCycle::RepositoryIdSeq* get_supported_objects()
            throw(CORBA::SystemException);

      protected:
         <xsl:value-of select="$_factoryImplClassname"/> ()
            throw(CORBA::SystemException);

      private:
      <![CDATA[
        // Register (name,object) within the repository. The name has already been
        // registered, so this should succeed. In case of a TRANSIENT system exception
        // There will be a retry max_retries times. if max_retries == 0, then this will do
        // retry forever.
        void
        register_object
            (CORBA::Object_ptr                                           the_object,
             const char*                                                 object_name,
             signed long                                                 max_retries)
            throw(CdmwLifeCycle::AlreadyExistant,
                  CdmwLifeCycle::InvalidName,
                  CORBA::SystemException);
      ]]>

         // Hide copy constructor/assignment operator
         <xsl:value-of select="$_factoryImplClassname"/> (const <xsl:value-of select="$_factoryImplClassname"/><![CDATA[ & rhs)]]>
            throw();

         <xsl:value-of select="$_factoryImplClassname"/><![CDATA[ &]]>
         operator=(const <xsl:value-of select="$_factoryImplClassname"/><![CDATA[ & rhs)]]>
            throw();

         /**
          * [Attribute description]
          */
         CORBA::ORB_var                            m_orb;
         PortableServer::POA_var                   m_poa;
         PortableServer::POA_var                   m_objects_poa;
         CdmwLifeCycle::ObjectRegistration_var    m_domain;
         std::string                               m_domain_name;
         std::string                               m_factory_name;
         Cdmw::LifeCycle::RetainObjectDeactivator m_object_deactivator;

         <xsl:value-of select="$cppScopedServantProviderClassname"/>* m_servant_provider;
         <![CDATA[std::auto_ptr<Cdmw::LifeCycle::EntityCleanupNotifier> m_cleanup_notifier;]]>

   }; // End class <xsl:value-of select="concat($_factoryImplClassname, $lf)"/>

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedFactoryImplClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->
</xsl:template> <!-- end of template entity_factory_hpp.content -->



<!--
-->
<xsl:template name="entity_factory_hpp.content.1">
   <xsl:param name="_node"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_operationName"/>

   <xsl:value-of select="concat($lf, $_returnType, $lf, $_operationName, $lf)"/>
      (
         <!--
            IN parameters.
         -->
         <xsl:for-each select="$_node/UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'in']">
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
      <xsl:for-each select="$_node/UML:ModelElement.taggedValue/UML:TaggedValue">
         <xsl:if test="@tag = 'raises'">
            <xsl:value-of select="@value"/>,
         </xsl:if>
      </xsl:for-each>
      CORBA::SystemException
   );
</xsl:template>



</xsl:stylesheet>


