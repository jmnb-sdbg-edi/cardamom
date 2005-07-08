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
<xsl:template name="session_component_cpp">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   
   <xsl:if test="boolean($lifecycleFrameworkNodes/home-impl) and $lifecycleFrameworkNodes/home-impl/@category = 'SESSION'">
      <xsl:if test="$_index &lt;= count($lifecycleFrameworkNodes/home-impl)">
         <xsl:variable name="homeImplNode" select="$lifecycleFrameworkNodes[home-impl and position() = $_index]/home-impl"/>

         <!--
            Check if the component is segmented
         -->
         <xsl:variable name="isSegmented">
             <xsl:choose>
                 <xsl:when test="boolean($homeImplNode/@segmented)">
                     <xsl:value-of select="$homeImplNode/@segmented"/>
                 </xsl:when>
                 <xsl:otherwise>
                     <xsl:value-of select="'yes'"/>
                 </xsl:otherwise>
             </xsl:choose>
         </xsl:variable>

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
            <xsl:variable name="componentImplClassname" select="concat('Session', $managedComponentNode/@name, '_impl')"/>

            <!--
               Get the output filename.
            -->
            <xsl:variable name="filename">
               <xsl:call-template name="buildOutputFilename">
                  <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                  <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                  <xsl:with-param name="_defaultFilename" select="$componentImplClassname"/>
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
               <xsl:call-template name="session_component_cpp.content">
                  <xsl:with-param name="_scopedHomeImplClassname">
                     <xsl:call-template name="buildHomeImplClassname">
                        <xsl:with-param name="_homeImplNode" select="$homeImplNode"/>
                     </xsl:call-template>
                  </xsl:with-param>
                  <xsl:with-param name="_componentName" select="$managedComponentNode/@name"/>
                  <xsl:with-param name="_isSegmented" select="$isSegmented"/>
               </xsl:call-template>
            </xsl:document>
         </xsl:if>

         <!--
            Proceed to the next home impl if any.
         -->
         <xsl:call-template name="session_component_cpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template session_component_cpp -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _facetName                 The component name.
-->
<xsl:template name="session_component_cpp.content">
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_componentName"/>
   <xsl:param name="_isSegmented"/>
   

   <!--
      References to some nodes.
   -->
   <xsl:variable name="componentNode" select="key('classByName', $_componentName)"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="cppScopedComponentName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_componentName"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cppComponentScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_componentName"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="componentImplClassname" select="concat('Session', $_componentName, '_impl')"/>
   <xsl:variable name="componentObjRefVar" select="concat($cppComponentScope, $cppSep, 'CCM_', $_componentName, '_var')"/>
   <xsl:variable name="componentObjRefPtr" select="concat($cppComponentScope, $cppSep, 'CCM_', $_componentName, '_ptr')"/>
   <xsl:variable name="generalizations" select="key('generalizationById', $componentNode/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref)"/>
   <xsl:variable name="hasFacets">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMProvides'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasReceptacles">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMUses'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasEmitters">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMEmits'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasPublishers">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMPublishes'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasConsumers">
      <xsl:call-template name="componentElements">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_whatToFind" select="'CCMConsumes'"/>
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
   
   #include "Foundation/orbsupport/CORBA.hpp"
   #include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
   #include "CCMContainer/ccmcommon/CCMUtils.hpp"
   #include "<xsl:value-of select="concat($pathPrefix, $componentImplClassname, '.hpp')"/>"
   #include "Foundation/orbsupport/RefCountLocalObject.hpp"

   <!--
      Include all the contained facets.
   -->
   <xsl:call-template name="findAllComponentFacets">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_component_cpp.content.4'"/>
   </xsl:call-template>

   <!--
      Include all the contained consumers.
   -->
   <xsl:call-template name="findAllComponentConsumers">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_component_cpp.content.24'"/>
   </xsl:call-template>

   #include "CCMContainer/idllib/CdmwCcmCif.skel.hpp"
   #include "<xsl:value-of select="concat($pathPrefix, 'CCM_', $_componentName, '_SessionContext_impl.hpp')"/>"

   namespace {
        
      <xsl:if test="$_isSegmented = 'yes'">
      const char* COMP_REP_ID = "<xsl:value-of select="$componentNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>";
      </xsl:if>
      <xsl:if test="$_isSegmented = 'no'">
      const char* COMP_REP_ID = "IDL:thalesgroup.com/Cdmw_<xsl:value-of select="concat($cppComponentScope, '/', $_componentName, ':1.0')"/>";
      </xsl:if>
      const char* COMP_NAME   = "<xsl:value-of select="$_componentName"/>";

      <!--
         Facets.
      -->
      <xsl:call-template name="findAllComponentFacets">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'session_component_cpp.content.5'"/>
      </xsl:call-template>

      <!--
         Receptacles.
      -->
      <xsl:call-template name="findAllComponentReceptacles">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'session_component_cpp.content.10'"/>
      </xsl:call-template>
      
      <!--
         Publishers.
      -->

      <xsl:call-template name="findAllComponentPublishers">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'session_component_cpp.content.13'"/>
      </xsl:call-template>
      
      <!--
         Emitters.
      -->
      
      <xsl:call-template name="findAllComponentEmitters">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'session_component_cpp.content.14'"/>
      </xsl:call-template>

      <!--
         Consumers.
      -->

      <xsl:call-template name="findAllComponentConsumers">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'session_component_cpp.content.19'"/>
      </xsl:call-template>
      
      /*
       * If a home, when creating a component, returns an EnterpriseComponent, we map it 
       * in an ExecutorLocator for code optimisation purpose. Thus the code is the same
       * for monolithic executor and segmented executor.
       */
      class <xsl:value-of select="$_componentName"/>ExecutorLocator : public CdmwCcmCif::SessionExecutorLocator,
                                    public Cdmw::OrbSupport::RefCountLocalObject
      {
         public:
            <xsl:value-of select="$_componentName"/>ExecutorLocator(<xsl:value-of select="$componentObjRefPtr"/> executor)
               : is_a_session_component(false),
               m_comp_executor(<xsl:value-of select="concat($cppComponentScope, $cppSep, 'CCM_', $_componentName)"/>::_duplicate(executor))
            {
               try 
               {
                  Components::SessionComponent_var session_comp
                     = Components::SessionComponent::_narrow(m_comp_executor.in());
            
                  if (!CORBA::is_nil(session_comp.in()))
                  {
                     is_a_session_component = true;
                  }
               }
               catch (...)
               {
               }
            };

            ~<xsl:value-of select="$_componentName"/>ExecutorLocator() {};

            /*
             * the cif will invoke its obtain_executor operation prior to each invocation to
             * retrieve the implementation for a port. The port name, given in the name
             * parameter, is the same as used in the components interface description 
             * in IDL, or the components name for the main executor. The obtain_executor
             * operation returns a local object reference of the expected type. The 
             * CCMException exception may be raised in case of a system error that 
             * prohibits locating the requested executor.
             */
            //
            // IDL:omg.org/Components/ExecutorLocator/obtain_executor:1.0
            //
            virtual CORBA::Object_ptr obtain_executor(const char* name)
               throw(Components::CCMException,
                     CORBA::SystemException)
            {
               std::string exec_name(name);
               CORBA::Object_var result;

               if (exec_name == "<xsl:value-of select="$_componentName"/>")
               {
                  result = <xsl:value-of select="concat($cppComponentScope, $cppSep, 'CCM_', $_componentName, $cppSep)"/>_duplicate(m_comp_executor.in());
               }
               <xsl:call-template name="findAllComponentFacets">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'session_component_cpp.content.6'"/>
               </xsl:call-template>
               <xsl:call-template name="findAllComponentConsumers">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'session_component_cpp.content.20'"/>
               </xsl:call-template>
               else 
               {
                  throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidExecutorName, CORBA::COMPLETED_NO);
               }

               return result._retn();
            };

            /* 
             * The release_executor operation is called by the cif once the current
             * invocation on an executor that was obtained through the obtain_executor 
             * operation has finished. The locator can thus release any resources that
             * were acquired as part of the obtain_executor operation.
             */
            //
            // IDL:omg.org/Components/ExecutorLocator/release_executor:1.0
            //
            virtual void release_executor(CORBA::Object_ptr exc)
               throw(Components::CCMException,
                     CORBA::SystemException)
            {
            };

            /* 
             * The configuration_complete operation is called to propagate the
             * configuration_complete operation on the CCMObject interface to the
             * component implementation.
             */
            //
            // IDL:omg.org/Components/ExecutorLocator/configuration_complete:1.0
            //
            virtual void configuration_complete()
               throw(Components::InvalidConfiguration,
                     CORBA::SystemException)
            {
               // No equivalent operation for monolithic executor
            };

            //
            // IDL:omg.org/Components/SessionComponent/set_session_context:1.0
            //
            virtual void set_session_context(Components::SessionContext_ptr ctx)
               throw(Components::CCMException,
                     CORBA::SystemException)
            {
               // check if component implements the SessionComponent local interface
               if (is_a_session_component)
               {
                  Components::SessionComponent_var session_comp 
                     = Components::SessionComponent::_narrow(m_comp_executor.in());
                  <![CDATA[session_comp->set_session_context(ctx);]]>
               }
            };

            /* 
             * The ccm_activate operation is called by the container to notify a 
             * session component that it has been made active. The component instance
             * should perform any initialization required prior to operation invocation.
             * The component may raise the CCMException with the SYSTEM_ERROR minor code
             * to indicate a failure caused by a system level error.
             */
            //
            // IDL:omg.org/Components/SessionComponent/ccm_activate:1.0
            //
            virtual void ccm_activate()
               throw(Components::CCMException,
                     CORBA::SystemException)
            {
               // check if component implements the SessionComponent local interface
               if (is_a_session_component)
               {
                  Components::SessionComponent_var session_comp 
                     = Components::SessionComponent::_narrow(m_comp_executor.in());
                  <![CDATA[session_comp->ccm_activate();]]>
               }
            };

            /* 
             * The ccm_passivate operation is called by the container to notify a session
             * component that it has been made inactive. The component instance should
             * release any resources it acquired at activation time.The component may 
             * raise the CCMException with the SYSTEM_ERROR minor code to indicate a 
             * failure caused by a system level error.
             */
            //
            // IDL:omg.org/Components/SessionComponent/ccm_passivate:1.0
            //
            virtual void ccm_passivate()
               throw(Components::CCMException,
                     CORBA::SystemException)
            {
               // check if component implements the SessionComponent local interface
               if (is_a_session_component)
               {
                  Components::SessionComponent_var session_comp 
                     = Components::SessionComponent::_narrow(m_comp_executor.in());
                  <![CDATA[session_comp->ccm_passivate();]]>
               }
            };

            /* 
             * The ccm_remove operation is called by the container when the servant is
             * about to be destroyed. It informs the component that it is about to be
             * destroyed.The component may raise the CCMException with the SYSTEM_ERROR
             * minor code to indicate a failure caused by a system level error.
             */
            //
            // IDL:omg.org/Components/SessionComponent/ccm_remove:1.0
            //
            virtual void ccm_remove()
               throw(Components::CCMException,
                     CORBA::SystemException)
            {
               // check if component implements the SessionComponent local interface
               if (is_a_session_component)
               {
                  Components::SessionComponent_var session_comp 
                     = Components::SessionComponent::_narrow(m_comp_executor.in());
                  <![CDATA[session_comp->ccm_remove();]]>
               }
            };

         private:
            <xsl:value-of select="$_componentName"/>ExecutorLocator(const <xsl:value-of select="$_componentName"/>ExecutorLocator<![CDATA[&);]]>
            void operator=(const <xsl:value-of select="$_componentName"/>ExecutorLocator<![CDATA[&);]]>

            bool is_a_session_component;  // indicates that the component executor inherits
                                          // from the SessionComponent local interface.

            <xsl:value-of select="$componentObjRefVar"/> m_comp_executor; // points on Component monolithic executor.
      };
   };  // end of anonymous namespace

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>


#ifdef CDMW_USE_FAULTTOLERANCE
    // FT Activation Handler for the component
    class Session<xsl:value-of select="$_componentName"/>FTActivationHandler_impl :
         virtual public ::CdmwFT::Location::ActivationHandler,
         virtual public Cdmw::OrbSupport::RefCountLocalObject
    {
    public:
        Session<xsl:value-of select="$_componentName"/>FTActivationHandler_impl(Session<xsl:value-of select="$_componentName"/>_impl* component_servant)
            : m_component_servant(component_servant)
        {
        }

        //
        // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/activate:1.0
        //
        virtual void activate()
            throw(CORBA::SystemException)
        {
            // Check component has its configuration complete 
            if (<![CDATA[m_component_servant->is_configured()]]>) {
                if (<![CDATA[!m_component_servant->m_is_activated]]>) {
                    // component not already activated
                    Components::SessionComponent_var session_locator =
                        <![CDATA[m_component_servant->m_session_locator;]]>
                    <![CDATA[session_locator->ccm_activate();]]>

                    <![CDATA[m_component_servant->m_is_activated = true;]]>
                } else {
                     PRINT_ERROR("Component already activated!");
                     throw CORBA::INTERNAL(OrbSupport::INTERNALAlreadyActivated,
                                           CORBA::COMPLETED_NO);
                 }
            } else {
                 PRINT_ERROR("Component has its configuration complete!");
                 throw CORBA::BAD_INV_ORDER(OrbSupport::BAD_INV_ORDERComponentNotConfigured,
                                            CORBA::COMPLETED_NO);
            }
        }

        //
        // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/deactivate:1.0
        //
        virtual void deactivate()
            throw(CORBA::SystemException)
        {
            if (<![CDATA[m_component_servant->m_is_activated]]>) {
                // component is activated, passivate it!
                Components::SessionComponent_var session_locator =
                    <![CDATA[m_component_servant->m_session_locator;]]>
                <![CDATA[session_locator->ccm_passivate();]]>

                <![CDATA[m_component_servant->m_is_activated = false;]]>
            }
        }

    private:
        Session<xsl:value-of select="$_componentName"/>FTActivationHandler_impl(const Session<xsl:value-of select="$_componentName"/>FTActivationHandler_impl<![CDATA[&);]]>
        Session<xsl:value-of select="$_componentName"/>FTActivationHandler_impl<![CDATA[&]]> operator=(const Session<xsl:value-of select="$_componentName"/>FTActivationHandler_impl<![CDATA[&);]]>

        Session<xsl:value-of select="$_componentName"/>_impl* m_component_servant;

    };
#endif


   // Constructor
   <xsl:value-of select="concat($componentImplClassname, $cppSep, $componentImplClassname)"/>(const std::string comp_oid,
                         CdmwCcmContainer::CCM2Context_ptr ctx,
                         CORBA::Object_ptr                 comp_ref,
                         Components::EnterpriseComponent*  executor,
#ifdef CDMW_USE_FAULTTOLERANCE
                         ::FT::ObjectGroup_ptr             group_ref,
#endif
                         bool is_created_by_factory_operation)
      throw (Components::CCMException,
             CORBA::SystemException)
      <xsl:if test="$_isSegmented = 'yes'">
      : CCMObject_impl(comp_oid, ctx, COMP_REP_ID, comp_ref),
      </xsl:if>
      <xsl:if test="$_isSegmented = 'no'">
      : CCMObject_impl(comp_oid, ctx, COMP_REP_ID, comp_ref, true),
      </xsl:if>
        m_is_activated(false), 
#ifdef CDMW_USE_FAULTTOLERANCE
        m_group_ref(::FT::ObjectGroup::_duplicate(group_ref)),
        m_activation_handler(::CdmwFT::Location::ActivationHandler::_nil()),
        m_activation_manager(::CdmwFT::Location::ActivationManager::_nil()),
        m_activation_handler_id(0L),
#endif
        m_is_created_by_factory_operation(is_created_by_factory_operation)
      <xsl:call-template name="findAllInterfaceReadWriteAttributes">
         <xsl:with-param name="_interfaceName" select="$_componentName"/>
         <xsl:with-param name="_type" select="'Component'"/>
         <xsl:with-param name="_template" select="'session_component_cpp.content.1'"/>
      </xsl:call-template>
   {
      PRINT_INFO("<xsl:value-of select="$componentImplClassname"/> constructor called for oid: " <![CDATA[<< comp_oid);]]>
      
      // Check if the home returns an executor locator
      m_executor_locator = Components::ExecutorLocator::_narrow(executor);

      if (CORBA::is_nil(m_executor_locator.in()))
      {
         // executor is not an executor locator
         // map monolithic executor to executor locator
         <xsl:value-of select="$componentObjRefVar"/> comp_exec = <xsl:value-of select="concat($cppComponentScope, $cppSep, 'CCM_',  $_componentName)"/>::_narrow(executor);
        
         if (CORBA::is_nil(comp_exec.in()))
         {
            // This is to avoid Assertion failure in debug mode as internal ref count
            // equals 1 (CORBA C++ mapping oblige!)
            _set_ref_count(0);

            PRINT_ERROR("Invalid Component Executor");
            throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidComponentExecutor,
                                   CORBA::COMPLETED_NO);
         }
         m_executor_locator = new <xsl:value-of select="$_componentName"/>ExecutorLocator(comp_exec.in());

         m_session_locator = Components::SessionComponent::_narrow(m_executor_locator.in());
        
         if (CORBA::is_nil(m_session_locator.in()))
         {
             PRINT_ERROR("Executor Locator should be a session component!");
             throw CORBA::INTERNAL(OrbSupport::INTERNALInvalidExecutor,
                                   CORBA::COMPLETED_NO);
         }
      }
      else
      {
         Components::SessionComponent_var m_session_locator 
            = Components::SessionComponent::_narrow(m_executor_locator.in());
        
         if (CORBA::is_nil(m_session_locator.in()))
         {
             PRINT_ERROR("Executor Locator should be a session component!");
             throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMInvalidComponentExecutor,
                                   CORBA::COMPLETED_NO);
         }
      }


#ifdef CDMW_USE_FAULTTOLERANCE
      if (!CORBA::is_nil(m_group_ref.in()))
      {
         // component is replicated, it shall be activated by FT framework
         // To do so, we shall create a FT ActivationHandler
         m_activation_handler = new Session<xsl:value-of select="$_componentName"/>FTActivationHandler_impl(this);

<![CDATA[         
         //m_activation_handler = sctx->create_oid();

         CORBA::Object_var am_ref = 
            m_context->resolve_initial_references("FTActivationManager");
         m_activation_manager = 
            CdmwFT::Location::ActivationManager::_narrow(am_ref.in());
         m_activation_handler_id = 
            m_activation_manager->register_activation_handler(m_group_ref.in(),
                                                              m_activation_handler.in());
]]>
      }
#endif

      m_context_executor = new CCM_<xsl:value-of select="$_componentName"/>_SessionContext_impl(ctx, this);

      <xsl:call-template name="findAllComponentFacets">
         <xsl:with-param name="_componentName" select="$_componentName"/>
         <xsl:with-param name="_template" select="'session_component_cpp.content.7'"/>
      </xsl:call-template>

      <xsl:if test="$hasReceptacles = 'true'">
         try
         {
            <xsl:call-template name="findAllComponentReceptacles">
               <xsl:with-param name="_componentName" select="$_componentName"/>
               <xsl:with-param name="_template" select="'session_component_cpp.content.11'"/>
            </xsl:call-template>
         }
         catch (const <![CDATA[AlreadyDoneException&]]>)
         {
            // This is to avoid Assertion failure in debug mode as internal ref count
            // equals 1 (CORBA C++ mapping oblige!)
            _set_ref_count(0);

            PRINT_ERROR("Receptacle already declared!");
            // Don't propagate while its probably a code generation error!
         }
         catch (const <![CDATA[InternalErrorException&]]>)
         {
            // This is to avoid Assertion failure in debug mode as internal ref count
            // equals 1 (CORBA C++ mapping oblige!)
            _set_ref_count(0);
        
            PRINT_ERROR("Internal exception raised when declaring receptacle!");
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
         }
         catch (const <![CDATA[CORBA::SystemException& e]]>)
         {
            // This is to avoid Assertion failure in debug mode as internal ref count
            // equals 1 (CORBA C++ mapping oblige!)
            _set_ref_count(0);
        
            PRINT_ERROR("System exception raised when declaring receptacle!" <![CDATA[<< e]]>);
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
         }
      </xsl:if>

      <xsl:if test="$hasEmitters = 'true' or $hasPublishers = 'true'">
         try
         {
            <xsl:if test="$hasPublishers = 'true'">
               <xsl:call-template name="findAllComponentPublishers">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'session_component_cpp.content.15'"/>
               </xsl:call-template>
            </xsl:if>
            <xsl:if test="$hasEmitters = 'true'">
               <xsl:call-template name="findAllComponentEmitters">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'session_component_cpp.content.16'"/>
               </xsl:call-template>
            </xsl:if>
         }
         catch (const <![CDATA[AlreadyDoneException&]]>)
         {
            // This is to avoid Assertion failure in debug mode as internal ref count
            // equals 1 (CORBA C++ mapping oblige!)
            _set_ref_count(0);

            PRINT_ERROR("Event source already declared!");
            // Don't propagate while its probably a code generation error!
         }
         catch (const <![CDATA[CORBA::SystemException& e]]>)
         {
            // This is to avoid Assertion failure in debug mode as internal ref count
            // equals 1 (CORBA C++ mapping oblige!)
            _set_ref_count(0);
        
            PRINT_ERROR("System exception raised when declaring event source!" <![CDATA[<< e]]>);
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
         }
      </xsl:if>

      <xsl:if test="$hasConsumers = 'true'">
         try
         {
            <xsl:call-template name="findAllComponentConsumers">
               <xsl:with-param name="_componentName" select="$_componentName"/>
               <xsl:with-param name="_template" select="'session_component_cpp.content.21'"/>
            </xsl:call-template>
         }
         catch (const <![CDATA[AlreadyDoneException&]]>)
         {
            // This is to avoid Assertion failure in debug mode as internal ref count
            // equals 1 (CORBA C++ mapping oblige!)
            _set_ref_count(0);

            PRINT_ERROR("Consumer already declared!");
            // Don't propagate while its probably a code generation error!
         }
         catch (const <![CDATA[InternalErrorException&]]>)
         {
            // This is to avoid Assertion failure in debug mode as internal ref count
            // equals 1 (CORBA C++ mapping oblige!)
            _set_ref_count(0);
        
            PRINT_ERROR("Internal exception raised when declaring consumer!");
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
         }
         catch (const <![CDATA[CORBA::SystemException& e]]>)
         {
            // This is to avoid Assertion failure in debug mode as internal ref count
            // equals 1 (CORBA C++ mapping oblige!)
            _set_ref_count(0);
        
            PRINT_ERROR("System exception raised when declaring consumer!" <![CDATA[<< e]]>);
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
         }
      </xsl:if>

      PRINT_INFO("<xsl:value-of select="$componentImplClassname"/> constructor end");
   }

   <xsl:value-of select="concat($componentImplClassname, $cppSep, '~', $componentImplClassname, '()')"/>
   {
      PRINT_INFO("<xsl:value-of select="$componentImplClassname"/> destructor called!");

#ifdef CDMW_USE_FAULTTOLERANCE
      try
      {
          <![CDATA[m_activation_manager->unregister_activation_handler(m_activation_handler_id);]]>
      }
      catch (...)
      {
      }
#endif

      if (m_is_activated)
      {
          <![CDATA[m_session_locator->ccm_passivate();]]>
          m_is_activated = false;
      }
   }


   /**
    * Component attributes
    */
   <xsl:call-template name="findAllInterfaceReadWriteAttributes">
      <xsl:with-param name="_interfaceName" select="$_componentName"/>
      <xsl:with-param name="_type" select="'Component'"/>
      <xsl:with-param name="_template" select="'session_component_cpp.content.2'"/>
   </xsl:call-template>

   /**
    * Supported interfaces
    */

   <!--
      Declare supported interfaces operations.
   -->
   <xsl:for-each select="$generalizations">
      <xsl:variable name="interfaceNode" select="key('classById', UML:Generalization.supertype/UML:GeneralizableElement/@xmi.idref)"/>
      <xsl:variable name="containedOps"  select="key('operations', $interfaceNode/@xmi.id)"/>

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
            <xsl:call-template name="getType">
               <xsl:with-param name="_id" select="UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'return']/@type"/>
               <xsl:with-param name="_kind" select="'inout'"/>
               <xsl:with-param name="_asObjRefVar" select="true()"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Operation signature.
         -->
         <xsl:variable name="cppOperationName">
            <xsl:call-template name="mapCppReservedWords">
               <xsl:with-param name="_operationName" select="@name"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:value-of select="concat($returnType, $lf, $componentImplClassname, $cppSep, $cppOperationName, $lf)"/>
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
            if (is_removed())
            {
                throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                              CORBA::COMPLETED_NO); 
            }

            <xsl:value-of select="$componentObjRefVar"/> comp_exec = obtain_component_executor();
            <xsl:if test="string-length($objRefVarType) > 0">
                <xsl:value-of select="concat($objRefVarType, ' result = ')"/>
            </xsl:if>
            <![CDATA[comp_exec->]]><xsl:value-of select="$cppOperationName"/>(
               <xsl:for-each select="UML:BehavioralFeature.parameter/UML:Parameter[@kind != 'return']">
                  <xsl:value-of select="@name"/>
                  <xsl:if test="position() != last()">
                     <xsl:text>, </xsl:text>
                  </xsl:if>
               </xsl:for-each>
            );

            try
            {
                <![CDATA[m_executor_locator->release_executor(comp_exec.in());]]>
            }
            catch (const <![CDATA[Components::CCMException&]]> )
            {
                throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                      CORBA::COMPLETED_NO);
            }

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
      </xsl:for-each>
   </xsl:for-each>

   /**
    * Equivalent IDL methods
    */

   // Facets

   <xsl:call-template name="findAllComponentFacets">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_component_cpp.content.8'"/>
      <xsl:with-param name="_isSegmented" select="$_isSegmented"/>
   </xsl:call-template>

   // Receptacles

   <xsl:call-template name="findAllComponentReceptacles">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_component_cpp.content.12'"/>
   </xsl:call-template>

   // Publishers

   <xsl:call-template name="findAllComponentPublishers">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_component_cpp.content.17'"/>
   </xsl:call-template>
   
   // Emitters

   <xsl:call-template name="findAllComponentEmitters">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_component_cpp.content.18'"/>
   </xsl:call-template>
   
   // Consumers
   
   <xsl:if test="$_isSegmented = 'no'">
      <xsl:if test="$hasConsumers = 'true'">
         void
         <xsl:value-of select="$componentImplClassname"/>::push_event(Components::EventBase * evt)
            throw (Components::BadEventType, CORBA::SystemException)
         {
            if (is_removed())
              {
                throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                              CORBA::COMPLETED_NO); 
            }
   
            bool bad_evt_type_found = true;
            
            <xsl:call-template name="findAllComponentConsumers">
               <xsl:with-param name="_componentName" select="$_componentName"/>
               <xsl:with-param name="_template" select="'session_component_cpp.content.25'"/>
            </xsl:call-template>
            
            if (bad_evt_type_found)
            {
               throw Components::BadEventType();
            }
         }
   
         void
         <xsl:value-of select="$componentImplClassname"/>::push(const <![CDATA[CORBA::Any& data]]>)
            throw (CosEventComm::Disconnected, 
                   CORBA::SystemException)
         {
            if (is_removed())
            {
                throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                              CORBA::COMPLETED_NO); 
            }
   
            bool bad_evt_type_found = true;
            
            <xsl:call-template name="findAllComponentConsumers">
               <xsl:with-param name="_componentName" select="$_componentName"/>
               <xsl:with-param name="_template" select="'session_component_cpp.content.26'"/>
            </xsl:call-template>
            
            if (bad_evt_type_found)
            {
                throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMBadEventType, CORBA::COMPLETED_NO);
            }
         }
   
         void
         <xsl:value-of select="$componentImplClassname"/>::disconnect_push_consumer()
         throw (CORBA::SystemException)
         {
             if (is_removed())
             {
                 throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                               CORBA::COMPLETED_NO); 
             }
   
         }
   
      </xsl:if>
   </xsl:if>

   <xsl:call-template name="findAllComponentConsumers">
      <xsl:with-param name="_componentName" select="$_componentName"/>
      <xsl:with-param name="_template" select="'session_component_cpp.content.22'"/>
      <xsl:with-param name="_isSegmented" select="$_isSegmented"/>
   </xsl:call-template>

   void
   <xsl:value-of select="$componentImplClassname"/>::configuration_complete()
      throw(Components::InvalidConfiguration,
            CORBA::SystemException)
   {
      if (is_removed())
      {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      if (m_configured == false)
      {
         if (!m_is_created_by_factory_operation)
         {
            <xsl:call-template name="findAllInterfaceReadWriteAttributes">
               <xsl:with-param name="_interfaceName" select="$_componentName"/>
               <xsl:with-param name="_type" select="'Component'"/>
               <xsl:with-param name="_template" select="'session_component_cpp.content.3'"/>
            </xsl:call-template>
         }
         
         CCMObject_impl::configuration_complete();
        
         try
         {
            <![CDATA[
            m_session_locator->set_session_context(m_context_executor.in());
            m_executor_locator->configuration_complete();

#ifdef CDMW_USE_FAULTTOLERANCE
            if (CORBA::is_nil(m_group_ref.in()))
            {
               // component is not replicated
               m_session_locator->ccm_activate();
            
               // no error occurs component is activated
               m_is_activated = true;
            }
            // else the Component will be activated by FT framework with the help
            // of FTActivationHandler (see above)
#else
            m_session_locator->ccm_activate();
            
            // no error occurs component is activated
            m_is_activated = true;
#endif
            ]]>
         }
         catch (const <![CDATA[Components::CCMException& e]]>)
         {
            <![CDATA[
            PRINT_ERROR("Components::CCMException raised, reason " << e);
            ]]>
            throw CORBA::INTERNAL(OrbSupport::INTERNAL,
                                  CORBA::COMPLETED_NO);
         }
      }
   }

   <xsl:value-of select="concat($lf, $cppComponentScope, $cppSep, 'CCM_', $_componentName, '*')"/>
   <xsl:value-of select="concat($lf, $componentImplClassname)"/>::obtain_component_executor()
      throw (CORBA::SystemException)
   {
      if (is_removed())
      {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
        }

      try
      {
         <![CDATA[CORBA::Object_var obj = m_executor_locator->obtain_executor(COMP_NAME);]]>

         if (CORBA::is_nil(obj.in()))
         {
            throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXISTInvalidExecutor,
                                  CORBA::COMPLETED_NO);
         }

         <xsl:value-of select="$componentObjRefVar"/> comp_exec = <xsl:value-of select="concat($cppComponentScope, $cppSep, 'CCM_', $_componentName)"/>::_narrow(obj.in());
    
         if (CORBA::is_nil(comp_exec.in()))
         {
            throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXISTInvalidExecutor,
                                  CORBA::COMPLETED_NO);
         }
    
         return comp_exec._retn();
      }
      catch (const <![CDATA[Components::CCMException& e]]>)
      {
         throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXISTInvalidExecutor,
                               CORBA::COMPLETED_NO);
      }
      catch (const <![CDATA[CORBA::BAD_PARAM& e]]>)
      {
         PRINT_ERROR(e);
         throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXISTInvalidExecutor,
                               CORBA::COMPLETED_NO);
      }
   }

   <![CDATA[
   /**
    * Purpose:
    * <p>This returns the servant for the facet/consumer specified by facet_name.
    * This operation is implemented by the well typed component servant.
    *
    * @param facet_name The facet name
    *
    * @return the facet servant.
    */
   ]]>
   <xsl:choose>
      <xsl:when test="$hasFacets = 'true' or $hasConsumers = 'true'">
         PortableServer::Servant
         <xsl:value-of select="$componentImplClassname"/>::get_facet_servant(const char* facet_name) 
            throw (CORBA::SystemException)
         {
            if (is_removed())
            {
                throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                              CORBA::COMPLETED_NO); 
            }

            std::string facet = facet_name;
            PortableServer::ServantBase_var facet_servant;
          
            <xsl:if test="$hasFacets = 'true'">
               <xsl:call-template name="findAllComponentFacets">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'session_component_cpp.content.9'"/>
               </xsl:call-template>
            </xsl:if>
            <xsl:if test="$hasConsumers = 'true'">
               <xsl:call-template name="findAllComponentConsumers">
                  <xsl:with-param name="_componentName" select="$_componentName"/>
                  <xsl:with-param name="_template" select="'session_component_cpp.content.23'"/>
               </xsl:call-template>
            </xsl:if>
            {
               // No facet/consumer with this name define for that component
               throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                             CORBA::COMPLETED_NO); 
            }
          
            return facet_servant._retn();
         }
      </xsl:when>
      <xsl:otherwise>
         PortableServer::Servant
         <xsl:value-of select="$componentImplClassname"/>::get_facet_servant(const char* facet_name) 
            throw (CORBA::SystemException)
         {
            if (is_removed())
            {
                throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                              CORBA::COMPLETED_NO); 
            }

            std::string facet = facet_name;
            PortableServer::ServantBase_var facet_servant;
        
            // No facet with this name define for that component
            throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                          CORBA::COMPLETED_NO); 
        
            return facet_servant._retn();
         }
      </xsl:otherwise>
   </xsl:choose>

   // Call ccm_passivate and ccm_remove on executor
   void 
   <xsl:value-of select="$componentImplClassname"/>::advise_executor_of_remove_component()
      throw (CORBA::SystemException)
   {
      if (is_removed())
      {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
        }

      try
      {
         <![CDATA[
         if (m_is_activated)
         {
             m_session_locator->ccm_passivate();
             m_is_activated = false;
         }
         m_session_locator->ccm_remove();
         ]]>
      }
      catch (const <![CDATA[Components::CCMException& e]]>)
      {
         <![CDATA[
         PRINT_ERROR("Components::CCMException raised, reason " << e);
         ]]>
         throw CORBA::INTERNAL(OrbSupport::INTERNAL,
                               CORBA::COMPLETED_NO);
      }
   }

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$_scopedHomeImplClassname"/>
   </xsl:call-template>

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template session_component_cpp.content -->



<!--
-->
<xsl:template name="session_component_cpp.content.1">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramName"/>

   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         <xsl:value-of select="concat($lf, ', m_is_', $_paramName, '_configured(false)')"/>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template session_component_cpp.content.1 -->



<!--
-->
<xsl:template name="session_component_cpp.content.2">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramName"/>
   <xsl:param name="_paramType"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_componentImplClassname"/>
   <xsl:param name="_cppOperationName"/>
   <xsl:param name="_componentObjRefVar"/>
   <xsl:param name="_objRefVarType"/>

   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:value-of select="concat($lf, $_returnType, $lf, $_componentImplClassname, $cppSep, $_cppOperationName, '()', $lf)"/>
         throw(CORBA::SystemException)
      {
         if (is_removed())
         {
             throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                           CORBA::COMPLETED_NO); 
           }

         <xsl:value-of select="$_componentObjRefVar"/> comp_exec = obtain_component_executor();
         <xsl:value-of select="$_objRefVarType"/> result = <![CDATA[comp_exec->]]><xsl:value-of select="$_cppOperationName"/>();

         try
         {
             <![CDATA[m_executor_locator->release_executor(comp_exec.in());]]>
         }
         catch (const <![CDATA[Components::CCMException&]]> )
         {
             throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                   CORBA::COMPLETED_NO);
         }

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
         <xsl:value-of select="concat($_componentImplClassname, $cppSep, $_cppOperationName, '(', $_paramType, ' ', $_paramName, ')')"/>
            throw(CORBA::SystemException)
         {
             if (is_removed())
             {
                 throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                               CORBA::COMPLETED_NO); 
             }

             <xsl:value-of select="$_componentObjRefVar"/> comp_exec = obtain_component_executor();
             <![CDATA[comp_exec->]]><xsl:value-of select="concat($_cppOperationName, '(', $_paramName, ');')"/>

             try
             {
                 <![CDATA[m_executor_locator->release_executor(comp_exec.in());]]>
             }
             catch (const <![CDATA[Components::CCMException&]]> )
             {
                 throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                       CORBA::COMPLETED_NO);
             }

             // set boolean to true (needed for configuration_completed)
             m_is_<xsl:value-of select="$_paramName"/>_configured = true;
         }
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template session_component_cpp.content.2 -->



<!--
-->
<xsl:template name="session_component_cpp.content.3">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramName"/>

   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         // check if the attribute have been initialised only if not created by a factory op
         if (!m_is_<xsl:value-of select="$_paramName"/>_configured)
         {
            // component is not well configured
            PRINT_ERROR("Attribut <xsl:value-of select="$_paramName"/> not initialised!");
            throw Components::InvalidConfiguration();
         }
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template session_component_cpp.content.3 -->



<!--
-->
<xsl:template name="session_component_cpp.content.4">
   <xsl:param name="_facetName"/>
   
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

  <xsl:value-of select="concat($lf, '#include &quot;', $pathPrefix, $_facetName, '_impl.hpp&quot;')"/>
</xsl:template> <!-- end of template session_component_cpp.content.4 -->



<!--
-->
<xsl:template name="session_component_cpp.content.5">
   <xsl:param name="_facetNode"/>
   <xsl:param name="_facetName"/>

   <xsl:variable name="uppercaseFacetName" select="translate($_facetName, $lcase, $ucase)"/>
   
   <xsl:value-of select="concat($lf, 'const char* FACET_', $uppercaseFacetName, '_NAME')"/> = "<xsl:value-of select="$_facetName"/>";
   <xsl:value-of select="concat($lf, 'const char* FACET_', $uppercaseFacetName, '_REP_ID')"/> = "<xsl:value-of select="$_facetNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>";
</xsl:template> <!-- end of template session_component_cpp.content.5 -->



<!--
-->
<xsl:template name="session_component_cpp.content.6">
   <xsl:param name="_facetName"/>
   
   else if (exec_name == "<xsl:value-of select="$_facetName"/>")
   {
      <![CDATA[result = m_comp_executor->]]><xsl:value-of select="concat('get_', $_facetName, '();')"/>
   }
</xsl:template> <!-- end of template session_component_cpp.content.6 -->



<!--
-->
<xsl:template name="session_component_cpp.content.7">
   <xsl:param name="_facetTypeName"/>
   <xsl:param name="_facetName"/>
   
   <xsl:variable name="uppercaseFacetName" select="translate($_facetName, $lcase, $ucase)"/>

   try
   {
      CORBA::Object_var <xsl:value-of select="$_facetName"/>_facet
         = declare_facet(comp_oid.c_str(), 
                         FACET_<xsl:value-of select="$uppercaseFacetName"/>_NAME, 
                         FACET_<xsl:value-of select="$uppercaseFacetName"/>_REP_ID);
                         
      if (CORBA::is_nil(<xsl:value-of select="$_facetName"/>_facet.in()))
      {
         // This is to avoid Assertion failure in debug mode as internal ref count
         // equals 1 (CORBA C++ mapping oblige!)
         _set_ref_count(0);

         PRINT_ERROR("Internal exception raised when declaring facet <xsl:value-of select="$_facetName"/>!");
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
      }
   }
   catch (const <![CDATA[AlreadyDoneException&]]>)
   {
      // This is to avoid Assertion failure in debug mode as internal ref count
      // equals 1 (CORBA C++ mapping oblige!)
      _set_ref_count(0);
     
      PRINT_ERROR("Facet for_clients already declared!");
      // Don't propagate while its probably a code generation error!
   }
   catch (const <![CDATA[InternalErrorException&]]>)
   {
      // This is to avoid Assertion failure in debug mode as internal ref count
      // equals 1 (CORBA C++ mapping oblige!)
      _set_ref_count(0);

      PRINT_ERROR("Internal exception raised when declaring facet <xsl:value-of select="$_facetTypeName"/>!");
      throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
   }
</xsl:template> <!-- end of template session_component_cpp.content.7 -->



<!--
-->
<xsl:template name="session_component_cpp.content.8">
   <xsl:param name="_facetTypeName"/>
   <xsl:param name="_facetName"/>
   <xsl:param name="_componentImplClassname"/>
   <xsl:param name="_isSegmented"/>
   
   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_facetTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:variable name="cppFacetScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_facetTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:value-of select="concat($lf, $scopedReturnType, '_ptr')"/>
   <xsl:value-of select="concat($lf, $_componentImplClassname, $cppSep, 'provide_', $_facetName, '()',
                                $lf, 'throw(CORBA::SystemException)', $lf)"/>
   {
      if (is_removed())
       {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      CORBA::Object_var obj;

      try 
      {
         obj = CCMObject_impl::provide_facet(CORBA::string_dup("<xsl:value-of select="$_facetName"/>"));
      }
      catch (const <![CDATA[Components::InvalidName &]]>)
      {
         throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST,
                                       CORBA::COMPLETED_NO);
      }

      if (CORBA::is_nil(obj.in()))
      {
         throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                       CORBA::COMPLETED_NO);
      }

      <xsl:value-of select="concat($scopedReturnType, '_var')"/> facet_ref = <xsl:value-of select="$scopedReturnType"/>::_narrow(obj.in());
       
      if (CORBA::is_nil(facet_ref.in()))
      {
         throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                       CORBA::COMPLETED_NO);
      }
       
      return facet_ref._retn();
   }

   <xsl:if test="$_isSegmented = 'no'">
      <!--
               facet operation code for monolithic component
       -->
      <xsl:variable name="facetNode" select="key('classByName', $_facetTypeName)"/>
      
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
                  <xsl:with-param name="_kind" select="'inout'"/>
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
         <xsl:value-of select="concat($lf, $returnType, $lf, $_componentImplClassname, $cppSep, $cppOperationName, $lf)"/>
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
              if (is_removed())
              {
                  throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                                CORBA::COMPLETED_NO); 
                }
   
              try
               {
                <![CDATA[CORBA::Object_var obj = m_executor_locator->obtain_executor("]]><xsl:value-of select="$_facetName"/>");
   
               if (CORBA::is_nil(obj.in()))
               {
                  throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                                CORBA::COMPLETED_NO);
               }
   
               <xsl:value-of select="concat($cppFacetScope, $cppSep, 'CCM_', $_facetTypeName, '_var')"/> facet_exec = <xsl:value-of select="concat($cppFacetScope, $cppSep, 'CCM_', $_facetTypeName)"/>::_narrow(obj.in());
       
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
   </xsl:if>
</xsl:template> <!-- end of template session_component_cpp.content.8 -->



<!--
-->
<xsl:template name="session_component_cpp.content.9">
   <xsl:param name="_facetTypeName"/>
   <xsl:param name="_facetName"/>
   
   <xsl:variable name="uppercaseFacetName" select="translate($_facetName, $lcase, $ucase)"/>

   if (facet == FACET_<xsl:value-of select="$uppercaseFacetName"/>_NAME)
   {
      facet_servant 
         = new <xsl:value-of select="concat($_facetTypeName, '_impl')"/>(m_executor_locator.in(), 
                   FACET_<xsl:value-of select="$uppercaseFacetName"/>_NAME,
                   m_context.in(),
                   m_comp_ref.in(),
                   this);
   }
   else
</xsl:template> <!-- end of template session_component_cpp.content.9 -->



<!--
-->
<xsl:template name="session_component_cpp.content.10">
   <xsl:param name="_receptacleNode"/>
   <xsl:param name="_receptacleName"/>

   <xsl:variable name="uppercaseReceptacleName" select="translate($_receptacleName, $lcase, $ucase)"/>
   
   <xsl:value-of select="concat($lf, 'const char* RECEP_', $uppercaseReceptacleName, '_NAME')"/> = "<xsl:value-of select="$_receptacleName"/>";
   <xsl:value-of select="concat($lf, 'const char* RECEP_', $uppercaseReceptacleName, '_REP_ID')"/> = "<xsl:value-of select="$_receptacleNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>";
</xsl:template> <!-- end of template session_component_cpp.content.10 -->



<!--
-->
<xsl:template name="session_component_cpp.content.11">
   <xsl:param name="_receptacleName"/>
   
   <xsl:variable name="uppercaseReceptacleName" select="translate($_receptacleName, $lcase, $ucase)"/>

   declare_receptacle(<xsl:value-of select="concat('RECEP_', $uppercaseReceptacleName,'_NAME, RECEP_', $uppercaseReceptacleName,'_REP_ID')"/>);
</xsl:template> <!-- end of template session_component_cpp.content.11 -->



<!--
-->
<xsl:template name="session_component_cpp.content.12">
   <xsl:param name="_receptacleTypeName"/>
   <xsl:param name="_receptacleName"/>
   <xsl:param name="_componentImplClassname"/>
   
   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_receptacleTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:variable name="uppercaseReceptacleName" select="translate($_receptacleName, $lcase, $ucase)"/>

   <xsl:value-of select="concat($lf, 'void', $lf, $_componentImplClassname, $cppSep, 'connect_', $_receptacleName, '(', $scopedReturnType, '_ptr conxn)', $lf)"/>
      throw(Components::AlreadyConnected,
            Components::InvalidConnection,
            CORBA::SystemException)
   {
        if (is_removed())
      {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      try
      {
          CCMObject_impl::connect(RECEP_<xsl:value-of select="$uppercaseReceptacleName"/>_NAME, conxn);
      }
      catch (const <![CDATA[Components::InvalidName &]]>)
      {
          PRINT_ERROR("Invalid name for receptacle connection!");
          throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                CORBA::COMPLETED_NO);   
      }
      catch (const <![CDATA[Components::AlreadyConnected &]]>)
      {
          throw;
      }
      catch (const <![CDATA[Components::InvalidConnection &]]>)
      {
          throw;
      }
      catch (const <![CDATA[Components::ExceededConnectionLimit &]]>)
      {
          PRINT_ERROR("Exceeded connection limit reached for receptacle connection!");
          throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                CORBA::COMPLETED_NO);   
      }
      catch (const <![CDATA[CORBA::SystemException &]]>)
      {
          throw;
      }
   }

   <xsl:value-of select="concat($lf, $scopedReturnType, '_ptr', $lf, $_componentImplClassname, $cppSep, 'disconnect_', $_receptacleName, '()', $lf)"/>
      throw(Components::NoConnection,
            CORBA::SystemException)
   {
      if (is_removed())
      {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      try
      {
          <xsl:value-of select="concat($scopedReturnType, '_var')"/> result = get_connection_<xsl:value-of select="$_receptacleName"/>();
          CCMObject_impl::disconnect(RECEP_<xsl:value-of select="$uppercaseReceptacleName"/>_NAME, 0);

          return result._retn();
      }
      catch (const <![CDATA[Components::InvalidName &]]>)
      {
          PRINT_ERROR("Invalid name for receptacle disconnection!");
          throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                CORBA::COMPLETED_NO);   
      }
      catch (const <![CDATA[Components::NoConnection &]]>)
      {
          throw;
      }
      catch (const <![CDATA[Components::InvalidConnection &]]>)
      {
          PRINT_ERROR("Invalid connection for receptacle disconnection!");
          throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                CORBA::COMPLETED_NO);   
      }
      catch (const <![CDATA[Components::CookieRequired &]]>)
      {
          PRINT_ERROR("Cookie required for receptacle disconnection!");
          throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                CORBA::COMPLETED_NO);   
      }
      catch (const <![CDATA[CORBA::SystemException &]]>)
      {
          throw;
      }
   }

   <xsl:value-of select="concat($lf, $scopedReturnType, '_ptr', $lf, $_componentImplClassname, $cppSep, 'get_connection_', $_receptacleName, '()', $lf)"/>
      throw(CORBA::SystemException)
   {
      if (is_removed())
        {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      Components::ConnectionDescriptions_var comp_desc;

      try
      {
         comp_desc = CCMObject_impl::get_connections(RECEP_<xsl:value-of select="$uppercaseReceptacleName"/>_NAME);
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
      
      <xsl:value-of select="concat($scopedReturnType, '_var')"/> result;
      
      if (<![CDATA[comp_desc->]]>length() == 1)
      {
         <![CDATA[CORBA::Object_var obj = CORBA::Object::_duplicate(comp_desc->operator[](0)->objref());]]>

         result = <xsl:value-of select="$scopedReturnType"/>::_narrow(obj.in());
      }
      else
      {
         result = <xsl:value-of select="$scopedReturnType"/>::_nil();
      }
 
      return result._retn();
   }
</xsl:template> <!-- end of template session_component_cpp.content.12 -->



<!--
-->
<xsl:template name="session_component_cpp.content.13">
   <xsl:param name="_publisherNode"/>
   <xsl:param name="_publisherName"/>

   <xsl:variable name="uppercasePublisherName" select="translate($_publisherName, $lcase, $ucase)"/>
   <xsl:variable name="repId">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_publisherNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
         <xsl:with-param name="_charsIn" select="':1.0'"/>
         <xsl:with-param name="_charsOut" select="'Consumer:1.0'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'const char* PUBLISHER_', $uppercasePublisherName, '_NAME')"/> = "<xsl:value-of select="$_publisherName"/>";
   <xsl:value-of select="concat($lf, 'const char* PUBLISHER_', $uppercasePublisherName, '_REP_ID')"/> = "<xsl:value-of select="$repId"/>";
</xsl:template> <!-- end of template session_component_cpp.content.13 -->



<!--
-->
<xsl:template name="session_component_cpp.content.14">
   <xsl:param name="_emitterNode"/>
   <xsl:param name="_emitterName"/>

   <xsl:variable name="uppercaseEmitterName" select="translate($_emitterName, $lcase, $ucase)"/>
   <xsl:variable name="repId">
      <xsl:call-template name="replaceCharsInString">
         <xsl:with-param name="_stringIn" select="$_emitterNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>
         <xsl:with-param name="_charsIn" select="':1.0'"/>
         <xsl:with-param name="_charsOut" select="'Consumer:1.0'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'const char* EMITTER_', $uppercaseEmitterName, '_NAME')"/> = "<xsl:value-of select="$_emitterName"/>";
   <xsl:value-of select="concat($lf, 'const char* EMITTER_', $uppercaseEmitterName, '_REP_ID')"/> = "<xsl:value-of select="$repId"/>";
</xsl:template> <!-- end of template session_component_cpp.content.14 -->



<!--
-->
<xsl:template name="session_component_cpp.content.15">
   <xsl:param name="_publisherName"/>
   
   <xsl:variable name="uppercasePublisherName" select="translate($_publisherName, $lcase, $ucase)"/>

   declare_publisher(<xsl:value-of select="concat('PUBLISHER_', $uppercasePublisherName,'_NAME, PUBLISHER_', $uppercasePublisherName,'_REP_ID')"/>);
</xsl:template> <!-- end of template session_component_cpp.content.15 -->



<!--
-->
<xsl:template name="session_component_cpp.content.16">
   <xsl:param name="_emitterName"/>
   
   <xsl:variable name="uppercaseEmitterName" select="translate($_emitterName, $lcase, $ucase)"/>

   declare_emitter(<xsl:value-of select="concat('EMITTER_', $uppercaseEmitterName,'_NAME, EMITTER_', $uppercaseEmitterName,'_REP_ID')"/>);
</xsl:template> <!-- end of template session_component_cpp.content.16 -->



<!--
-->
<xsl:template name="session_component_cpp.content.17">
   <xsl:param name="_publisherTypeName"/>
   <xsl:param name="_publisherName"/>
   <xsl:param name="_componentImplClassname"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_publisherTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:variable name="uppercasePublisherName" select="translate($_publisherName, $lcase, $ucase)"/>

   <xsl:value-of select="concat($lf, 'Components::Cookie*',
                                $lf, $_componentImplClassname, $cppSep, 'subscribe_', $_publisherName, '(', $scopedReturnType, 'Consumer_ptr consumer)')"/>
      throw(Components::ExceededConnectionLimit, CORBA::SystemException)
   {
      if (is_removed())
       {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      Components::Cookie_var result;
      
      try
      {
          result = CCMObject_impl::subscribe(PUBLISHER_<xsl:value-of select="$uppercasePublisherName"/>_NAME,
                                             consumer);
      }
      catch (const <![CDATA[Components::InvalidName &]]>)
      {
          PRINT_ERROR("Invalid name for publisher subscription!");
          throw CORBA::INTERNAL(OrbSupport::INTERNAL, 
                                CORBA::COMPLETED_NO);   
      }
      catch (const <![CDATA[Components::InvalidConnection &]]>)
      {
          // should never appear!
          throw CORBA::INTERNAL(OrbSupport::INTERNAL, 
                                CORBA::COMPLETED_NO);   
      }
      catch (const <![CDATA[Components::ExceededConnectionLimit &]]>)
      {
          throw;
      }
      catch (const <![CDATA[CORBA::SystemException&]]>)
      {
          throw;
      }
      
      return result._retn();
   }

   <xsl:value-of select="concat($lf, $scopedReturnType, 'Consumer_ptr',
                                $lf, $_componentImplClassname, $cppSep, 'unsubscribe_', $_publisherName, '(Components::Cookie* ck)')"/>
      throw(Components::InvalidConnection, CORBA::SystemException)
   {
      if (is_removed())
       {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      if (!ck)
         throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
       
      <xsl:value-of select="$scopedReturnType"/>Consumer_var result = <xsl:value-of select="$scopedReturnType"/>Consumer::_nil();
      try
      {
         Components::EventConsumerBase_var consumer 
            = CCMObject_impl::get_consumer(PUBLISHER_<xsl:value-of select="$uppercasePublisherName"/>_NAME, ck);

         result = <xsl:value-of select="$scopedReturnType"/>Consumer::_narrow(consumer.in());

         CCMObject_impl::unsubscribe(PUBLISHER_<xsl:value-of select="$uppercasePublisherName"/>_NAME, ck);
      }
      catch (const <![CDATA[Components::InvalidName&]]>)
      {
         // internal error should not appear
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, 
                               CORBA::COMPLETED_NO);   
      }
      catch (const <![CDATA[Components::InvalidConnection&]]>)
      {
         throw;
      }
      catch (const <![CDATA[CORBA::SystemException&]]>)
      {
         throw;
      }

      return result._retn();
   }
</xsl:template> <!-- end of template session_component_cpp.content.17 -->



<!--
-->
<xsl:template name="session_component_cpp.content.18">
   <xsl:param name="_emitterTypeName"/>
   <xsl:param name="_emitterName"/>
   <xsl:param name="_componentImplClassname"/>

   <xsl:variable name="scopedReturnType">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_emitterTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   <xsl:variable name="uppercaseEmitterName" select="translate($_emitterName, $lcase, $ucase)"/>

   <xsl:value-of select="concat($lf, 'void',
                                $lf, $_componentImplClassname, $cppSep, 'connect_', $_emitterName, '(', $scopedReturnType, 'Consumer_ptr consumer)')"/>
      throw(Components::AlreadyConnected, CORBA::SystemException)
   {
      if (is_removed())
      {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      try
      {
         CCMObject_impl::connect_consumer(EMITTER_<xsl:value-of select="$uppercaseEmitterName"/>_NAME, consumer);
      }
      catch (const <![CDATA[Components::InvalidName&]]>)
      {
         // can not appear!
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, 
                               CORBA::COMPLETED_NO);
      }
      catch (const <![CDATA[Components::InvalidConnection&]]>)
      {
         // can not appear!
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, 
                               CORBA::COMPLETED_NO);
      } 
      catch (const <![CDATA[Components::AlreadyConnected&]]>)
      {
         throw;
      }
      catch (const <![CDATA[CORBA::SystemException&]]>)
      {
         throw;
      } 
   }

   <xsl:value-of select="concat($lf, $scopedReturnType, 'Consumer_ptr',
                                $lf, $_componentImplClassname, $cppSep, 'disconnect_', $_emitterName, '()')"/>
      throw(Components::NoConnection, CORBA::SystemException)
   {
      if (is_removed())
      {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      try
      {
         Components::EventConsumerBase_var consumer = 
            CCMObject_impl::disconnect_consumer(EMITTER_<xsl:value-of select="$uppercaseEmitterName"/>_NAME);

         <xsl:value-of select="$scopedReturnType"/>Consumer_var result = 
            <xsl:value-of select="$scopedReturnType"/>Consumer::_narrow(consumer.in());

         if (CORBA::is_nil(result.in()))
         {
            // can not appear!
            throw CORBA::INTERNAL(OrbSupport::INTERNAL, 
                                  CORBA::COMPLETED_NO);
         }

         return result._retn();
      }
      catch (const <![CDATA[Components::InvalidName&]]>)
      {
         // can not appear!
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, 
                               CORBA::COMPLETED_NO);
      }
      catch (const <![CDATA[Components::InvalidConnection&]]>)
      {
         // can not appear!
         throw CORBA::INTERNAL(OrbSupport::INTERNAL, 
                               CORBA::COMPLETED_NO);
      } 
      catch (const <![CDATA[Components::AlreadyConnected&]]>)
      {
         throw;
      }
      catch (const <![CDATA[CORBA::SystemException&]]>)
      {
         throw;
      } 
   }
</xsl:template> <!-- end of template session_component_cpp.content.18 -->



<!--
-->
<xsl:template name="session_component_cpp.content.19">
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerName"/>

   <xsl:variable name="uppercaseConsumerName" select="translate($_consumerName, $lcase, $ucase)"/>
   <xsl:variable name="repId">
      <xsl:variable name="partialRepId">
         <xsl:call-template name="replaceCharsInString">
            <xsl:with-param name="_stringIn">
               <xsl:call-template name="getScopedName">
                  <xsl:with-param name="_name" select="$_consumerTypeName"/>
               </xsl:call-template>
            </xsl:with-param>
            <xsl:with-param name="_charsIn" select="$cppSep"/>
            <xsl:with-param name="_charsOut" select="'/'"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat('IDL:', $idlPragmaPrefix, '/Cdmw_', $partialRepId, 'Consumer:1.0')"/>
   </xsl:variable>
   
   <xsl:value-of select="concat($lf, 'const char* CONSUMER_', $uppercaseConsumerName, '_NAME')"/> = "<xsl:value-of select="$_consumerName"/>";
   <xsl:value-of select="concat($lf, 'const char* CONSUMER_', $uppercaseConsumerName, '_REP_ID')"/> = "<xsl:value-of select="$repId"/>";
</xsl:template> <!-- end of template session_component_cpp.content.19 -->



<!--
-->
<xsl:template name="session_component_cpp.content.20">
   <xsl:param name="_consumerName"/>
   <xsl:param name="_componentName"/>

   <xsl:variable name="cppComponentScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_componentName"/>
      </xsl:call-template>
   </xsl:variable>
   
   else if (exec_name == <xsl:value-of select="concat('CONSUMER_', translate($_consumerName, $lcase, $ucase), '_NAME')"/>)
   {
      result = <xsl:value-of select="concat($cppComponentScope, $cppSep, 'CCM_', $_componentName)"/>::_duplicate(m_comp_executor.in());
   }
</xsl:template> <!-- end of template session_component_cpp.content.20 -->



<!--
-->
<xsl:template name="session_component_cpp.content.21">
   <xsl:param name="_consumerName"/>
   
   <xsl:variable name="uppercaseConsumerName" select="translate($_consumerName, $lcase, $ucase)"/>

   declare_consumer(comp_oid.c_str(), <xsl:value-of select="concat('CONSUMER_', $uppercaseConsumerName,'_NAME, CONSUMER_', $uppercaseConsumerName,'_REP_ID')"/>);
</xsl:template> <!-- end of template session_component_cpp.content.21 -->



<!--
-->
<xsl:template name="session_component_cpp.content.22">
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerName"/>
   <xsl:param name="_componentName"/>
   <xsl:param name="_componentImplClassname"/>
   <xsl:param name="_isSegmented"/>

   <xsl:variable name="cppScopedConsumerTypeName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_consumerTypeName"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:if test="$_isSegmented = 'no'">
      <xsl:variable name="cppConsumerScope">
         <xsl:call-template name="getScope">
            <xsl:with-param name="_name" select="$_consumerTypeName"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:variable name="lowercaseComponentName" select="translate($_componentName, $ucase, $lcase)"/>
      <xsl:variable name="consumerClassname" select="concat($_consumerTypeName, 'Consumer')"/>
      <xsl:variable name="lowercaseConsumerTypeName" select="translate($_consumerTypeName, $ucase, $lcase)"/>
      
   
   
      void
      <xsl:value-of select="concat($_componentImplClassname, $cppSep, 'push_', $_consumerTypeName, '(', $cppScopedConsumerTypeName, '* the_', $lowercaseConsumerTypeName, ')')"/>
         throw (CORBA::SystemException)
      {
          if (is_removed())
          {
              throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                            CORBA::COMPLETED_NO); 
          }
   
          try
          {
   
          <![CDATA[CORBA::Object_var obj = m_executor_locator->obtain_executor("]]><xsl:value-of select="$_consumerName"/>");
   
          if (CORBA::is_nil(obj.in()))
          {
              throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                            CORBA::COMPLETED_NO);
          }
          <xsl:value-of select="concat($cppConsumerScope, $cppSep, 'CCM_', $_componentName, '_var ', $lowercaseComponentName, '_exec = ', 
                                         $cppConsumerScope, $cppSep, 'CCM_', $_componentName, $cppSep, '_narrow(obj.in());')"/>
   
           if (CORBA::is_nil(<xsl:value-of select="concat($lowercaseComponentName, '_exec.in()')"/>))
           {
               // Component may segmented
               <xsl:value-of select="concat($cppConsumerScope, $cppSep, 'CCM_', $consumerClassname, '_var consumer_exec = ',
                                            $cppConsumerScope, $cppSep, 'CCM_', $consumerClassname, $cppSep, '_narrow(obj.in());')"/>
           
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
               <xsl:value-of select="$lowercaseComponentName"/><![CDATA[_exec->push_]]><xsl:value-of select="concat($_consumerName, '(the_', $lowercaseConsumerTypeName,');')"/>
           }
       
            <![CDATA[m_executor_locator->release_executor(obj.in());]]>
         }
         catch (const <![CDATA[Components::CCMException&]]> )
         {
            throw CORBA::INTERNAL(OrbSupport::INTERNALCCMCIFError, 
                                  CORBA::COMPLETED_NO);
         }
      }
   </xsl:if>

   <xsl:value-of select="concat($lf, $cppScopedConsumerTypeName, 'Consumer_ptr',
                                $lf, $_componentImplClassname, $cppSep, 'get_consumer_', $_consumerName, '()')"/>
      throw(CORBA::SystemException)
   {
      if (is_removed())
      {
          throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST,
                                        CORBA::COMPLETED_NO); 
      }

      CORBA::Object_var obj;

      try 
      {
         obj = CCMObject_impl::get_consumer(CONSUMER_<xsl:value-of select="translate($_consumerName, $lcase, $ucase)"/>_NAME);
      }
      catch (const <![CDATA[Components::InvalidName &]]>)
      {
         throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST,
                                       CORBA::COMPLETED_NO);
      }

      if (CORBA::is_nil(obj.in()))
      {
         throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                       CORBA::COMPLETED_NO);
      }

      <xsl:value-of select="concat($cppScopedConsumerTypeName, 'Consumer_var consumer_ref = ',
                                   $cppScopedConsumerTypeName, 'Consumer', $cppSep, '_narrow(obj.in());')"/>
       
      if (CORBA::is_nil(consumer_ref.in()))
      {
         throw CORBA::OBJECT_NOT_EXIST(OrbSupport::OBJECT_NOT_EXIST, 
                                       CORBA::COMPLETED_NO);
      }
       
      return consumer_ref._retn();
   } 
</xsl:template> <!-- end of template session_component_cpp.content.22 -->



<!--
-->
<xsl:template name="session_component_cpp.content.23">
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerName"/>
   
   <xsl:variable name="uppercaseConsumerName" select="translate($_consumerName, $lcase, $ucase)"/>

   if (facet == CONSUMER_<xsl:value-of select="$uppercaseConsumerName"/>_NAME)
   {
      facet_servant 
         = new <xsl:value-of select="concat($_consumerTypeName, 'Consumer_impl')"/>(m_executor_locator.in(), 
                   CONSUMER_<xsl:value-of select="$uppercaseConsumerName"/>_NAME,
                   m_context.in(),
                   this);
   }
   else
</xsl:template> <!-- end of template session_component_cpp.content.23 -->



<!--
-->
<xsl:template name="session_component_cpp.content.24">
   <xsl:param name="_consumerTypeName"/>
   
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
   
   <xsl:value-of select="concat($lf, '#include &quot;', $pathPrefix, $_consumerTypeName, 'Consumer_impl.hpp&quot;')"/>
</xsl:template> <!-- end of template session_component_cpp.content.24 -->

<!-- Following template is only used for monolithic component -->
<xsl:template name="session_component_cpp.content.25">
   <xsl:param name="_consumerTypeName"/>

   <xsl:variable name="cppScopedConsumerTypeName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_consumerTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   if (bad_evt_type_found)
   {
      <xsl:value-of select="$cppScopedConsumerTypeName"/>_var info_evt 
         = <xsl:value-of select="$cppScopedConsumerTypeName"/>::_downcast(evt);
      CORBA::add_ref(info_evt.in());

      if (info_evt.in())
      {
         bad_evt_type_found = false;
         <![CDATA[this->push_]]><xsl:value-of select="$_consumerTypeName"/>(info_evt.in());
      }
   }
   
</xsl:template> <!-- end of template session_component_cpp.content.25 -->


<!-- Following template is only used for monolithic component -->
<xsl:template name="session_component_cpp.content.26">
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerName"/>
   <xsl:param name="_componentImplClassname"/>

   <xsl:variable name="cppScopedConsumerTypeName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_consumerTypeName"/>
      </xsl:call-template>
   </xsl:variable>

   if (bad_evt_type_found)
   {
      <xsl:value-of select="$cppScopedConsumerTypeName"/>_var event;
      <![CDATA[data >>= event;]]>
      CORBA::add_ref(event.in());

      if (event.in())
      {
          bad_evt_type_found = false;
          try
          {
              <![CDATA[this->push_]]><xsl:value-of select="$_consumerTypeName"/>(event.in());
          }
          catch (const <![CDATA[CORBA::SystemException&]]>)
          {
              throw;
          }
      }
   }
</xsl:template> <!-- end of template session_component_cpp.content.26 -->



</xsl:stylesheet>


