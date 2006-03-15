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
<xsl:template name="service_home_hpp">
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
               Insert the #ifndef #define instructions.
            -->
            <xsl:call-template name="writeDefine">
               <xsl:with-param name="_prefix" select="'CCMCIF'"/>
               <xsl:with-param name="_classname" select="$homeImplClassname"/>
            </xsl:call-template>
            <!--
               Insert the content.
            -->
            <xsl:call-template name="service_home_hpp.content">
               <xsl:with-param name="_scopedHomeImplClassname" select="$scopedHomeImplClassname"/>
               <xsl:with-param name="_homeName">
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="$homeImplNode/../@factory-idref"/>
                     <xsl:with-param name="_separator" select="$xmlSep"/>
                  </xsl:call-template>
               </xsl:with-param>
            </xsl:call-template>
            <!--
               Insert the #endif instruction.
            -->
            <xsl:call-template name="writeDefine">
               <xsl:with-param name="_prefix" select="'CCMCIF'"/>
               <xsl:with-param name="_classname" select="$homeImplClassname"/>
               <xsl:with-param name="_endif" select="true()"/>
            </xsl:call-template>
         </xsl:document>

         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="service_home_hpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_home_hpp -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _homeName                  The home name.
-->
<xsl:template name="service_home_hpp.content">
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
   <xsl:variable name="scopedManagedComponentName">
      <xsl:variable name="ccmManagesId" select="key('stereotypeByName', 'CCMManages')/@xmi.id"/>
      <xsl:variable name="homeNodeId" select="key('classByName', $_homeName)/@xmi.id"/>
      <xsl:variable name="managesNode" select="key('associationByStereotypeId', $ccmManagesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $homeNodeId]"/>
      <xsl:variable name="managedComponentId" select="$managesNode/UML:Association.connection/UML:AssociationEnd[2]/@type"/>
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="key('classById', $managedComponentId)/@name"/>
      </xsl:call-template>
   </xsl:variable>

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
   
   #include "<xsl:value-of select="concat($pathPrefix, 'Cdmw_', substring-before($idl3Filename, '.idl') , '_cif')"/>.skel.hpp"
   #include "CCMContainer/ccmcif/CCMHome_impl.hpp"
   #include "CCMContainer/ccmcif/HomeConfiguration_impl.hpp"
   #include "CCMContainer/ccmcontainer/HomeAllocator.hpp"

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   class <xsl:value-of select="$homeImplClassname"/> : virtual public POA_Cdmw_<xsl:value-of select="concat($cppHomeScope, $cppSep, $_homeName, ',')"/>
                                                       public CCMHome_impl,
                                                       virtual public HomeConfiguration_impl
   {
      friend class Container::HomeAllocator<![CDATA[<]]><xsl:value-of select="$homeImplClassname"/><![CDATA[>;]]>

      public:
         // Destructor
         ~<xsl:value-of select="$homeImplClassname"/>();

         /**
          * Home attributes
          */

         <xsl:call-template name="findAllInterfaceReadWriteAttributes">
            <xsl:with-param name="_interfaceName" select="$_homeName"/>
            <xsl:with-param name="_type" select="'Home'"/>
            <xsl:with-param name="_template" select="'service_home_hpp.content.2'"/>
         </xsl:call-template>

         /**
          * Explicit interface
          */

         <xsl:call-template name="findAllHomeFactoryOperations">
            <xsl:with-param name="_homeName" select="$_homeName"/>
            <xsl:with-param name="_template" select="'service_home_hpp.content.1'"/>
            <xsl:with-param name="_strategy" select="'Service'"/>
         </xsl:call-template>

         /**
          * Implicit interface
          */

         virtual <xsl:value-of select="$scopedManagedComponentName"/>_ptr
         create()
            throw(Components::CreateFailure,
                  CORBA::SystemException);

         // Keyless home operation

         virtual Components::CCMObject_ptr
         create_component()
            throw(Components::CreateFailure,
                  CORBA::SystemException);

         <![CDATA[
            /**
             * Purpose:
             * <p>This method acts as a hand shaking between the Servant Provider
             * and the generated Factory implementation. It indicates whether
             * a Servant provided by this Servant Provider may incarnate multiple
             * objects (with ObjectIds provided by this Servant Provider)
             *
             * @return <B>true</B> if a servant may incarnate multiple objects,
             * <B>false</B> otherwise.
             */
            virtual bool may_incarnate_multiple_objects() const throw()
            {
               return false;
            }

            virtual void remove()
                throw(CosLifeCycle::NotRemovable,
                      CORBA::SystemException);
         ]]>

      protected:
         // Constructor
         <xsl:value-of select="$homeImplClassname"/>(Components::HomeExecutorBase_ptr home_exec,
                                                     const <![CDATA[Cdmw::LifeCycle::ObjectDeactivator&]]> deactivator)
            throw(CORBA::SystemException);

         <![CDATA[
            /**
             * This returns an array containing repository IDs of all objects
             * supported by the current ServantProvider.
             * <B>NOTA:</B> Repository IDs should be statically allocated; thus derived classes
             * should not delete return pointer(s).
             */
            virtual const char ** get_ids() const throw();
         ]]>

         <![CDATA[
            /**
             * This returns the number of repository Ids supported by the current ServantProvider.
             * <B>NOTA:</B> This should correspond to the number of null terminated strings
             * returned by get_ids() operation.
             */
            virtual CORBA::Long get_number_of_ids() const throw();
         ]]>

         <![CDATA[
            /**
             * Purpose:
             * <p>
             *   This creates new component servant.
             *   This shall be implemented by the well typed home servant.
             *
             * @param ctx      The component context
             * @param executor The component executor
             *
             * @return The new component servant.
             * @exception Components::CreateFailure if a problem occured when creating executor.
             * @exception Any CORBA Exception
             *
             */
            virtual PortableServer::Servant
            create_component_servant(const std::string                  comp_oid,
                                     CdmwCcmContainer::CCM2Context_ptr ctx,
                                     Components::EnterpriseComponent*   executor)
               throw(Components::CreateFailure,
                     CORBA::SystemException);
         ]]>

      private:
         <xsl:value-of select="$homeImplClassname"/>(const <xsl:value-of select="$homeImplClassname"/><![CDATA[&);]]>
         void operator=(const <xsl:value-of select="$homeImplClassname"/><![CDATA[&);]]>

         // Home executor
         <xsl:value-of select="concat($cppHomeScope, $cppSep, 'CCM_', $_homeName, '_var')"/> m_home_executor;
   };

   };

   }; // end of namespace CIF

   namespace Container
   {
      <![CDATA[template <>]]>
      PortableServer::Servant
      HomeAllocator<![CDATA[<]]><xsl:value-of select="concat('CIF::Cdmw', $cppHomeScope, $cppSep, $homeImplClassname)"/><![CDATA[>::allocate]]>
                  (Components::HomeExecutorBase_ptr            home_executor,
                   const <![CDATA[Cdmw::LifeCycle::ObjectDeactivator &]]> deactivator)
         throw(CORBA::SystemException,
               std::bad_alloc)
      {
         return new <xsl:value-of select="concat('CIF::Cdmw', $cppHomeScope, $cppSep, $homeImplClassname)"/>(home_executor, deactivator);
      }
   }; // end of namespace Container

   }; // end of namespace CCM

   }; // end of namespace Cdmw

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template service_home_hpp.content -->



<!--
-->
<xsl:template name="service_home_hpp.content.1">
   <xsl:param name="_node"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_operationName"/>

   // Factory operation

   <xsl:value-of select="concat($lf, 'virtual ', $_returnType, $lf, $_operationName, $lf)"/>
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
      throw (Components::CreateFailure,
             CORBA::SystemException);
</xsl:template> <!-- end of template service_home_hpp.content.1 -->


<xsl:template name="service_home_hpp.content.2">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramType"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_cppOperationName"/>

   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:value-of select="concat($lf, 'virtual ', $_returnType, ' ', $_cppOperationName, '()', $lf, 'throw(CORBA::SystemException);', $lf)"/>

      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         <xsl:value-of select="concat($lf, 'virtual void ', $_cppOperationName, '(', $_paramType, ')', $lf, 'throw(CORBA::SystemException);', $lf)"/>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_home_hpp.content.2 -->



</xsl:stylesheet>


