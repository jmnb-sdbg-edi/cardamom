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
<xsl:template name="entity_factory_allocator_hpp">
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
            Get the output filename.
         -->
         <xsl:variable name="factoryFileName">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$lifecycleFrameworkNode/factory-impl/@directory"/>
               <xsl:with-param name="_userFilename" select="$lifecycleFrameworkNode/factory-impl/@filename"/>
               <xsl:with-param name="_defaultFilename" select="$factoryImplClassname"/>
               <xsl:with-param name="_fileExtension" select="''"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:variable name="filename" select="concat($factoryFileName, '_allocator.hpp')"/>

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
            <xsl:call-template name="entity_factory_allocator_hpp.content">
               <xsl:with-param name="_factoryImplClassname" select="$factoryImplClassname"/>
               <xsl:with-param name="_headerFactoryAllocatorName">
                  <xsl:if test="string-length($CSC_NAME) > 0">
                     <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
                  </xsl:if>
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="$factoryFileName"/>
                     <xsl:with-param name="_separator" select="$fileSep"/>
                  </xsl:call-template>
               </xsl:with-param>
               <xsl:with-param name="_scopedFactoryImplClassname" select="$scopedFactoryImplClassname"/>
            </xsl:call-template>
         </xsl:document>

         <!--
            Proceed to the next item if any.
         -->
         <xsl:call-template name="entity_factory_allocator_hpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template>



<!--
   This template generates the content of the file.*

   @param _factoryImplClassname       The factory impl classname.
   @param _headerFactoryAllocatorName The factory allocator header file name.
   @param _scopedFactoryImplClassname The scoped factory impl classname.
-->
<xsl:template name="entity_factory_allocator_hpp.content">
   <xsl:param name="_factoryImplClassname"/>
   <xsl:param name="_headerFactoryAllocatorName"/>
   <xsl:param name="_scopedFactoryImplClassname"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="cppScopedFactoryImplClassname">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_scopedFactoryImplClassname"/>
         <xsl:with-param name="_charsIn" select="$xmlSep"/>
         <xsl:with-param name="_charsOut" select="$cppSep"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      The code generation starts here.
   -->

   #include "LifeCycle/lifecycle/FactoryAllocator.hpp"
   #include "Foundation/common/Locations.hpp"
   #include "Repository/naminginterface/NamingUtil.hpp"
   #include "Repository/repositoryinterface/RepositoryInterface.hpp"
   #include "<xsl:value-of select="$_headerFactoryAllocatorName"/>.hpp"


   namespace
   {
       // PropertSet Name
       const char* ENTITY_PROPERTY_SET_NAME = "PropertySet.object";
   };


   <![CDATA[
   namespace Cdmw
   {
   namespace LifeCycle
   {
   template <>
   PortableServer::Servant
   ]]>
   <![CDATA[NamedObjectsFactoryAllocator<]]><xsl:value-of select="$cppScopedFactoryImplClassname"/><![CDATA[>::allocate]]>
   <![CDATA[
               (CORBA::ORB_ptr                           orb,
                PortableServer::POA_ptr                  parent,
                CdmwLifeCycle::ObjectRegistration_ptr   name_domain,
                const std::string &                      name_domain_name,
                const std::string &                      factory_name,
                Cdmw::OrbSupport::POAThreadingPolicy &                     threading_policy,
                int & argc, char** argv)
           throw(Cdmw::OutOfMemoryException,
                 Cdmw::BadParameterException,
                 Cdmw::InternalErrorException,
                 CORBA::SystemException)
       {

           // Get the property set "CDMW/SERVICES/PropertySet.object"
           // (CDMW Developpers should not rely on this functionalilty as this is only
           // needed for persisting EntityCleanupNotifier objects. This may change in
           // future CDMW versions)
           CosPropertyService::PropertySet_var property_set
             = CosPropertyService::PropertySet::_nil();
           try {
             using Cdmw::NamingAndRepository::RepositoryInterface;
             Cdmw::NamingAndRepository::NamingInterface sni
               = RepositoryInterface::get_domain_naming_interface
               (Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN);

             typedef Cdmw::NamingAndRepository::NamingUtil<CosPropertyService::PropertySet>
               NamingUtil;
             property_set = NamingUtil::resolve_name(sni,ENTITY_PROPERTY_SET_NAME);

           } catch (const CdmwNamingAndRepository::NoNameDomain & ) {
             throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryError,
                                   CORBA::COMPLETED_YES);
           } catch (const CdmwNamingAndRepository::InvalidName & ) {
             throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryError,
                                   CORBA::COMPLETED_YES);
           } catch (const CosNaming::NamingContext::NotFound & ) {
             // TODO: Echo warning
           } catch (const CORBA::SystemException & ) {
             throw;
           } catch (...) {
             throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                   CORBA::COMPLETED_YES);
           }

           try {
               return new ]]><xsl:value-of select="$cppScopedFactoryImplClassname"/>
   <![CDATA[
                           (orb,
                            parent,
                            name_domain,
                            name_domain_name,
                            factory_name,
                            threading_policy,
                            property_set.in(),
                            argc,
                            argv);
           } catch (const std::bad_alloc &) {
               CDMW_THROW(Cdmw::OutOfMemoryException);
           } catch (const Cdmw::OutOfMemoryException &) {
            throw;
           } catch (const Cdmw::BadParameterException &) {
               throw;
           } catch (const Cdmw::InternalErrorException &) {
               throw;
           } catch (const CORBA::SystemException & e) {
               throw;
           }
       }

   }; // End namespace LifeCycle
   }; // End namespace Cdmw
   ]]>

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedFactoryImplClassname"/>
   </xsl:call-template>

   // TODO: Use a user-provided name rather than a naming convention!
   <![CDATA[typedef Cdmw::LifeCycle::NamedObjectsFactoryAllocator<]]><xsl:value-of select="$_factoryImplClassname"/><![CDATA[>]]>
      <xsl:value-of select="$_factoryImplClassname"/>_Allocator;

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedFactoryImplClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->


</xsl:template>


</xsl:stylesheet>


