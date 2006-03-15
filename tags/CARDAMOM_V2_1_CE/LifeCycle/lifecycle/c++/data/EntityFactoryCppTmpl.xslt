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
<xsl:template name="entity_factory_cpp">
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
         <xsl:variable name="servantProviderNode" select="$factoryNode/servant-provider"/>
         <xsl:variable name="servantProviderAllocatorNode" select="$lifecycleFrameworkNode/factory-impl/servant-provider-allocator"/>

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
         <xsl:variable name="scopedServantProviderClassname" select="$servantProviderNode/@classname"/>

         <!--
            Get the (scoped) user servant provider allocator classname.
         -->
         <xsl:variable name="scopedUserServantProviderAllocatorClassname" select="$servantProviderAllocatorNode/@classname"/>

         <!--
            Get the user servant provider allocator filename.
         -->
         <xsl:variable name="userServantProviderAllocatorFilename">
            <xsl:call-template name="addFileExtension">
               <xsl:with-param name="_filename">
                  <xsl:choose>
                     <xsl:when test="boolean($servantProviderAllocatorNode/@filename)">
                        <xsl:value-of select="$servantProviderAllocatorNode/@filename"/>
                     </xsl:when>
                     <xsl:otherwise>
                        <xsl:call-template name="getLastToken">
                           <xsl:with-param name="_string" select="$scopedUserServantProviderAllocatorClassname"/>
                           <xsl:with-param name="_separator" select="$xmlSep"/>
                        </xsl:call-template>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:with-param>
               <xsl:with-param name="_extension" select="'.hpp'"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the output filename.
         -->
         <xsl:variable name="filename">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$lifecycleFrameworkNode/factory-impl/@directory"/>
               <xsl:with-param name="_userFilename" select="$lifecycleFrameworkNode/factory-impl/@filename"/>
               <xsl:with-param name="_defaultFilename" select="$factoryImplClassname"/>
               <xsl:with-param name="_fileExtension" select="'.cpp'"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the servant provider filename.
         -->
         <xsl:variable name="servantProviderFilename">
            <xsl:call-template name="addFileExtension">
               <xsl:with-param name="_filename">
                  <xsl:variable name="spFilename">
                     <xsl:choose>
                        <xsl:when test="boolean($servantProviderNode/@filename)">
                           <xsl:value-of select="$servantProviderNode/@filename"/>
                        </xsl:when>
                        <xsl:otherwise>
                           <xsl:call-template name="getLastToken">
                              <xsl:with-param name="_string" select="$scopedServantProviderClassname"/>
                              <xsl:with-param name="_separator" select="$xmlSep"/>
                           </xsl:call-template>
                        </xsl:otherwise>
                     </xsl:choose>
                  </xsl:variable>
                  <xsl:choose>
                     <xsl:when test="boolean($servantProviderNode/@directory)">
                        <xsl:value-of select="concat($servantProviderNode/@directory, $fileSep, $spFilename)"/>
                     </xsl:when>
                     <xsl:otherwise>
                        <xsl:if test="string-length($CSC_NAME) > 0">
                           <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
                        </xsl:if>
                        <xsl:value-of select="$spFilename"/>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:with-param>
               <xsl:with-param name="_extension" select="'.hpp'"/>
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
            <xsl:call-template name="entity_factory_cpp.content">
               <xsl:with-param name="_interfaceName">
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="$scopedInterfaceName"/>
                     <xsl:with-param name="_separator" select="$xmlSep"/>
                  </xsl:call-template>
               </xsl:with-param>
               <xsl:with-param name="_headerFactoryFilename">
                  <xsl:if test="string-length($CSC_NAME) > 0">
                     <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
                  </xsl:if>
                  <xsl:call-template name="replaceCharsInString">
                     <xsl:with-param name="_stringIn">
                        <xsl:call-template name="getLastToken">
                           <xsl:with-param name="_string" select="$filename"/>
                           <xsl:with-param name="_separator" select="$fileSep"/>
                        </xsl:call-template>
                     </xsl:with-param>
                     <xsl:with-param name="_charsIn" select="'.cpp'"/>
                     <xsl:with-param name="_charsOut" select="'.hpp'"/>
                  </xsl:call-template>
               </xsl:with-param>
               <xsl:with-param name="_scopedServantProviderClassname" select="$scopedServantProviderClassname"/>
               <xsl:with-param name="_servantProviderFilename" select="$servantProviderFilename"/>
               <xsl:with-param name="_scopedUserServantProviderAllocatorClassname" select="$scopedUserServantProviderAllocatorClassname"/>
               <xsl:with-param name="_userServantProviderAllocatorFilename" select="$userServantProviderAllocatorFilename"/>
               <xsl:with-param name="_factoryImplClassname" select="$factoryImplClassname"/>
               <xsl:with-param name="_scopedFactoryImplClassname" select="$scopedFactoryImplClassname"/>
               <xsl:with-param name="_isMultithread" select="boolean($lifecycleFrameworkNode/factory-impl/@threading = 'MULTITHREAD')"/>
            </xsl:call-template>
         </xsl:document>

         <!--
            Proceed to the next item if any.
         -->
         <xsl:call-template name="entity_factory_cpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template entity_factory_cpp -->



<!--
   This template generates the content of the file.

   @param _interfaceName                                 The named interface name.
   @param _headerFactoryFilename                         The header filename.
   @param _scopedServantProviderClassname                The scoped servant provider classname.
   @param _servantProviderFilename                       The servant provider filename.
   @param _scopedUserServantProviderAllocatorClassname   The scoped user servant provider allocator classname.
   @param _userServantProviderAllocatorFilename          The user servant provider allocator filename.
   @param _factoryImplClassname                          The factory impl classname.
   @param _scopedFactoryImplClassname                    The scoped factory impl classname.
   @param _isMultithread                                 Multithread flag.
-->
<xsl:template name="entity_factory_cpp.content">
   <xsl:param name="_interfaceName"/>
   <xsl:param name="_headerFactoryFilename"/>
   <xsl:param name="_scopedServantProviderClassname"/>
   <xsl:param name="_servantProviderFilename"/>
   <xsl:param name="_scopedUserServantProviderAllocatorClassname"/>
   <xsl:param name="_userServantProviderAllocatorFilename"/>
   <xsl:param name="_factoryImplClassname"/>
   <xsl:param name="_scopedFactoryImplClassname"/>
   <xsl:param name="_isMultithread"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="cppScopedServantProviderClassname">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_scopedServantProviderClassname"/>
         <xsl:with-param name="_charsIn" select="$xmlSep"/>
         <xsl:with-param name="_charsOut" select="$cppSep"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppScopedUserServantProviderAllocatorClassname">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_scopedUserServantProviderAllocatorClassname"/>
         <xsl:with-param name="_charsIn" select="$xmlSep"/>
         <xsl:with-param name="_charsOut" select="$cppSep"/>
      </xsl:call-template>
   </xsl:variable>

   <!--
      The code generation starts here.
   -->

   #include "<xsl:value-of select="$_headerFactoryFilename"/>"
   #include "LifeCycle/lifecycle/ServantProviderAllocator.hpp"
   #include "<xsl:value-of select="$_servantProviderFilename"/>"
   #include "<xsl:value-of select="$_userServantProviderAllocatorFilename"/>"
   #include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
   #include "Foundation/orbsupport/OrbSupport.hpp"
   #include "Foundation/orbsupport/RefCountLocalObject.hpp"
   #include "LifeCycle/lifecycle/ObjectUtil.hpp"
   #include "Foundation/common/Assert.hpp"
   #include "Foundation/orbsupport/StrategyList.hpp"

   <![CDATA[#include <typeinfo>]]>

   namespace
   {
      // Maximum number of retries on receiving CORBA::TRANSIENT exception
      // for registering objects.
      const signed long MAX_RETRIES = 1;

      // PropertSet Name
      const char* ENTITY_PROPERTY_SET_NAME = "PropertySet.object";

      // ----------------------------------------------------------------------
      // Resource Servant activator
      // ----------------------------------------------------------------------

      class TheServantActivator  : 
         public virtual PortableServer::ServantActivator,
         public virtual Cdmw::OrbSupport::RefCountLocalObject
      {
	      public:
	         TheServantActivator (Cdmw::LifeCycle::ServantProviderBase * provider)
               : m_servant_provider(provider) {
               CDMW_ASSERT(provider != 0);
            }
            ~TheServantActivator ()	{
               <xsl:value-of select="$cppScopedUserServantProviderAllocatorClassname"/>::TheAllocator.dispose(m_servant_provider);
               m_servant_provider = 0;
            }
	
         <![CDATA[
            virtual PortableServer::Servant 
            incarnate(const PortableServer::ObjectId& oid, 
                      PortableServer::POA_ptr poa)
               throw(PortableServer::ForwardRequest, CORBA::SystemException) {
               CDMW_ASSERT(m_servant_provider != 0);    
               return m_servant_provider->lookup_servant(oid);
            }
	
            virtual void 
            etherealize(const PortableServer::ObjectId& oid,
                        PortableServer::POA_ptr poa,
                        PortableServer::Servant servant,
                        CORBA::Boolean cleanup_in_progress,
                        CORBA::Boolean remaining_activations)
               throw(CORBA::SystemException) {
               CDMW_ASSERT(m_servant_provider != 0);
               if (!remaining_activations) {
                  m_servant_provider -> release_servant (oid, poa, servant);
               }
            }
         ]]>

         private:        
            Cdmw::LifeCycle::ServantProviderBase * m_servant_provider;
      };  
   };

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedFactoryImplClassname"/>
   </xsl:call-template>

   <xsl:value-of select="$_factoryImplClassname"/>::<xsl:value-of select="$_factoryImplClassname"/>(
   <![CDATA[
         CORBA::ORB_ptr                           orb, 
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
            CORBA::SystemException)
      : m_orb(CORBA::ORB::_duplicate(orb)), 
        m_poa(PortableServer::POA::_duplicate(parent)),
        m_objects_poa(PortableServer::POA::_nil()),
        m_domain(CdmwLifeCycle::ObjectRegistration::_duplicate(name_domain)),
        m_domain_name(name_domain_name),
        m_factory_name(factory_name),
        m_object_deactivator(orb),
        m_servant_provider(0),
        m_cleanup_notifier(0)
   {
      //
      // Create POA for use by the factory
      //
      // Entity Life cycle framework use the following POA policies
      //
      // Lifespan policy            = PERSISTENT
      // Id Assignment policy       = USER_ID
      // Id Uniqueness policy       = UNIQUE_ID
      // Implicit Activation policy = NO_IMPLICIT_ACTIVATION
      // Request Processing policy  = USE_SERVANT_MANAGER;
      // Servant Retention policy   = RETAIN;
      // 

      PortableServer::POAManager_var poaManager = parent -> the_POAManager();
      CORBA::PolicyList policies;
      policies.length(7);
      policies[0] =
         parent -> create_id_assignment_policy(PortableServer::USER_ID);
      policies[1] =
         parent -> create_lifespan_policy(PortableServer::PERSISTENT);
      policies[2] =
         parent -> create_servant_retention_policy(PortableServer::RETAIN);
      policies[3] =
         parent -> create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
      policies[4] = parent ->
         create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);
      policies[5] = parent ->
         create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);

      Cdmw::OrbSupport::StrategyList poaStrategies;
   ]]>

   <xsl:choose>
      <!--
         MULTITHREAD.
      -->
      <xsl:when test="$_isMultithread">
         <![CDATA[
         // Strategies with MULTITHREAD policy will accept Single Thread and
         // Multi Thread strategies. However, SERIALIZE policy will only allow
         // Single Thread policy.
         Cdmw::OrbSupport::SingleThreadPolicy * single_thread
            = dynamic_cast< Cdmw::OrbSupport::SingleThreadPolicy *>(&threading_policy);
         if (single_thread == NULL)
         {
            // Multi Thread strategies
            policies[6] = parent ->
               create_thread_policy(PortableServer::ORB_CTRL_MODEL);
         }
         else
         {
            policies[6] = parent ->
               create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
         }

         threading_policy.append_to_StrategyList(poaStrategies);
         ]]>
      </xsl:when>
      <!--
         SERIALIZE.
      -->
      <xsl:otherwise>
         <![CDATA[
            // For Single Thread policy, first check validity of 'threading_policy' using 
            // the code bellow :
            // Begin Threading policy check
    
            policies[6] = parent ->
                create_thread_policy(PortableServer::SINGLE_THREAD_MODEL);
            // Check Multi-Threading Policy
            {
               using namespace Cdmw::OrbSupport;
               try {
                  SingleThreadPolicy & single_thread
                     = dynamic_cast<SingleThreadPolicy &>(threading_policy);
                  single_thread.append_to_StrategyList(poaStrategies);
               } catch (const std::bad_cast &) {
                  // Wrong policy
                  std::string parameter("Cdmw::OrbSupport::POAThreadingPolicy");
                  parameter += "(";
                  parameter += factory_name;
                  parameter += ")";
                  CDMW_THROW2( BadParameterException, parameter, typeid(threading_policy).name() );
               }
            }
            // End Threading policy check
         ]]>
      </xsl:otherwise>
   </xsl:choose>

   try
   {
	   // BadParameterException, InternalErrorException
	   m_objects_poa = Cdmw::OrbSupport::OrbSupport::create_POA(m_poa.in(),
                                                                m_factory_name.c_str(),
                                                                poaManager.in(), 
                                                                policies,
                                                                poaStrategies);
        
      //
      // Create and install servant provider
      //
      Cdmw::LifeCycle::ServantProviderBase * srvprvd 
         = <xsl:value-of select="$cppScopedUserServantProviderAllocatorClassname"/>::TheAllocator.allocate
            (orb,
             m_objects_poa.in(),
             m_object_deactivator,
             argc, argv);
      <![CDATA[m_servant_provider = dynamic_cast<]]><xsl:value-of select="$cppScopedServantProviderClassname"/><![CDATA[*>(srvprvd);]]>
      CDMW_ASSERT(m_servant_provider != NULL);

      <![CDATA[
         // Hand Shaking with the Servant Provider
         // IDs shall be unique
         if (m_servant_provider->may_incarnate_multiple_objects())
         {
            std::string parameter("ObjectId Uniqueness");
            parameter += "(";
            parameter += factory_name;
            parameter += ")";
            CDMW_THROW2( BadParameterException, parameter, "MULTIPLE_ID");
         }
    
         std::auto_ptr< Cdmw::LifeCycle::EntityCleanupNotifier> notifier
            (new Cdmw::LifeCycle::EntityCleanupNotifier(m_domain.in()));
         m_cleanup_notifier = notifier;

         if (!m_cleanup_notifier->init(property_set, factory_name)) {
               // TODO: Echo warning
         }

         m_servant_provider->set_cleanup_notifier(m_cleanup_notifier.get());

         //
         // Create and install servant activator
         //
         PortableServer::ServantActivator_var the_activator
            = new TheServantActivator(m_servant_provider);
         m_objects_poa -> set_servant_manager(the_activator.in());
    
      } catch (const std::bad_alloc & ) {
	      // This is to avoid Assertion failure in debug mode as internal ref count
         // equals 1 (CORBA C++ mapping oblige!)
         _set_ref_count(0);
         CDMW_THROW(Cdmw::OutOfMemoryException);
      } catch (const BadParameterException & ) {
         // This is to avoid Assertion failure in debug mode as internal ref count
         // equals 1 (CORBA C++ mapping oblige!)
         _set_ref_count(0);
         throw;
      } catch (const Cdmw::InternalErrorException & ) {
         // This is to avoid Assertion failure in debug mode as internal ref count
         // equals 1 (CORBA C++ mapping oblige!)
         _set_ref_count(0);
         throw;	
      } catch (const PortableServer::POA::AdapterAlreadyExists &) {
         // This is to avoid Assertion failure in debug mode as internal ref count
         // equals 1 (CORBA C++ mapping oblige!)
         _set_ref_count(0);
         throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                               CORBA::COMPLETED_YES);
      } catch (const PortableServer::POA::InvalidPolicy &) {
         // This is to avoid Assertion failure in debug mode as internal ref count
         // equals 1 (CORBA C++ mapping oblige!)
         _set_ref_count(0);
         throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                               CORBA::COMPLETED_YES);
      } catch (const CORBA::SystemException &) {
         // This is to avoid Assertion failure in debug mode as internal ref count
         // equals 1 (CORBA C++ mapping oblige!)
         _set_ref_count(0);
         throw; // rethrow
      }
      ]]>
   }

   <xsl:value-of select="$_factoryImplClassname"/>::~<xsl:value-of select="$_factoryImplClassname"/>()
      throw()
   {
      if (!m_cleanup_notifier->finish()) {
         // TODO: Echo warning
      }

      // <xsl:value-of select="$cppScopedUserServantProviderAllocatorClassname"/>::TheAllocator.dispose(m_servant_provider);

      // m_objects_poa and m_servant_activator references will be released by their destructors;
      // and the associated POA and ServantActivator implementations will be destroyed following
      // the destruction of the parent POA.
   }

   <!--
      Create the operation declarations.
   -->
   <xsl:call-template name="findAllFactoryOperations">
      <xsl:with-param name="_interfaceName" select="$_interfaceName"/>
      <xsl:with-param name="_template" select="'entity_factory_cpp.content.1'"/>
      <xsl:with-param name="_isNamed" select="true()"/>
      <xsl:with-param name="_factoryImplClassname" select="$_factoryImplClassname"/>
   </xsl:call-template>

   //
   // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
   //
   CORBA::Boolean 
   <xsl:value-of select="$_factoryImplClassname"/>::supports(const char* repository_id)
      throw(CORBA::SystemException)
   {
      <![CDATA[return CORBA::Boolean(m_servant_provider->supports(repository_id));]]>
   }

   //
   // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
   //
   CdmwLifeCycle::RepositoryIdSeq* 
   <xsl:value-of select="$_factoryImplClassname"/>::get_supported_objects()
      throw(CORBA::SystemException)
   {
      return m_servant_provider->get_supported_objects();
   }


   // Register (name,object) within the repository. 
   // In case of a TRANSIENT system exception
   // There will be a retry max_retries times. if max_retries == 0, then this will do
   // retry forever.
   void
   <xsl:value-of select="$_factoryImplClassname"/>::register_object
   <![CDATA[
         (CORBA::Object_ptr                                           the_object,
          const char*                                                 object_name,
          signed long                                                 max_retries)
       throw(CdmwLifeCycle::AlreadyExistant,
             CdmwLifeCycle::InvalidName,
             CORBA::SystemException)
   {
      bool forever = (max_retries <= 0);

      signed long counter = max_retries;

      do {
         try {
            m_domain->register_object (the_object, object_name);
            break;
         } catch (const CosNaming::NamingContext::AlreadyBound &) {
            CdmwLifeCycle::AlreadyExistant ex;
            // Add more information to help the caller in debuging
            std::string reason = m_domain_name + "/" + object_name;
            ex.name = CORBA::string_dup(reason.c_str());
            throw ex;
         } catch (const CosNaming::NamingContext::InvalidName & e) {
   	    // no "reason" field available
	    throw CdmwLifeCycle::InvalidName();
         } catch (const CORBA::TRANSIENT &) {
            // Retry
         } catch (const CORBA::SystemException &) {
            // Cancel registration ????
            throw;
         }
      } while (forever || (--counter > 0) );

      if (!forever && (counter <= 0))
         throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                                CORBA::COMPLETED_YES);
   ]]>
   }
   
   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedFactoryImplClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->
   
</xsl:template> <!-- end of template entity_factory_cpp.content -->



<!--
-->
<xsl:template name="entity_factory_cpp.content.1">
   <xsl:param name="_node"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_operationName"/>
   <xsl:param name="_factoryImplClassname"/>

   <xsl:value-of select="concat($lf, $_returnType, $lf, $_factoryImplClassname, $cppSep, $_operationName, $lf)"/>
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
   )
   {
      // [II] Acquire oid from user for object_key
      // Operation may throw user exceptions that will re-thrown to caller
      PortableServer::ObjectId_var oid 
         <![CDATA[= m_servant_provider->]]><xsl:value-of select="$_operationName"/> (
         <xsl:for-each select="$_node/UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'in' and not(@type = $keyNodeId)]">
            <xsl:value-of select="@name"/>
            <xsl:if test="position() != last()">
               <xsl:text>, </xsl:text>
            </xsl:if>
         </xsl:for-each>
         );
        
      // [III] Activate the object immediately :
      //       [a] Acquire a servant from user for odi
      //       [b] Activate the object (using a servant activator)
      //       [c] Create the object reference
      //       [d] Assert the object is really of the expected type
      <![CDATA[typedef Cdmw::LifeCycle::ObjectUtil<]]><xsl:value-of select="substring-before($_returnType, '_ptr')"/><![CDATA[>]]> Utils;
      PortableServer::Servant servant = Utils::lookup_servant
         (oid.in(), m_servant_provider);
      <xsl:value-of select="substring-before($_returnType, '_ptr')"/>_var ref;
      try {
            ref = Utils::activate_object(m_objects_poa.in(),oid.in(),servant);   
      // [IV] Register (name,object) within the repository.
            register_object(ref.in(), object_key, MAX_RETRIES);
      } catch (...) {
            <![CDATA[m_servant_provider -> release_servant (oid.in(), m_objects_poa.in(), servant);]]>
	    throw;
      }

      // [VI] Insert entry to the cleanup notifier if object is removable
      CosLifeCycle::LifeCycleObject_var removable = CosLifeCycle::LifeCycleObject::_narrow(ref.in());
      if (!CORBA::is_nil(removable.in())) {
         <![CDATA[m_cleanup_notifier->insert_entry(oid.in(), object_key);]]>
      }

      return ref._retn();
   }
</xsl:template>



</xsl:stylesheet>


