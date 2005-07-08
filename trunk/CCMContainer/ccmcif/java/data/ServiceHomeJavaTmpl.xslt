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
<xsl:template name="service_home_java">
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
            Generate the file.
         -->
         <xsl:document method="text" href="{$filename}">
            <!--
               Insert the document header.
            -->
            <xsl:call-template name="docHeader">
               <xsl:with-param name="_lang" select="'java'"/>
            </xsl:call-template>
            <!--
               Insert the content.
            -->
            <xsl:call-template name="service_home_java.content">
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
         <xsl:call-template name="service_home_java">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template service_home_java.content -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _homeName                  The home name.
-->
<xsl:template name="service_home_java.content">
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
   <xsl:variable name="javaHomeScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$_homeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="managedComponentId">
      <xsl:variable name="ccmManagesId" select="key('stereotypeByName', 'CCMManages')/@xmi.id"/>
      <xsl:variable name="homeNodeId" select="key('classByName', $_homeName)/@xmi.id"/>
      <xsl:variable name="managesNode" select="key('associationByStereotypeId', $ccmManagesId)[UML:Association.connection/UML:AssociationEnd[1]/@type = $homeNodeId]"/>
      <xsl:value-of select="$managesNode/UML:Association.connection/UML:AssociationEnd[2]/@type"/>
   </xsl:variable>
   <xsl:variable name="scopedManagedComponentName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="key('classById', $managedComponentId)/@name"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:variable name="javaScopedHomeOperationsName">
      <xsl:variable name="tempName">
         <xsl:call-template name="prefixModuleName">
            <xsl:with-param name="_prefix" select="'Cdmw_'"/>
            <xsl:with-param name="_idlPrefix" select="$idlPragmaPrefix"/>
            <xsl:with-param name="_scopedName" select="concat($javaHomeScope, $javaSep, $_homeName)"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($tempName, 'Operations')"/>
   </xsl:variable>
   
   <!--
      References to some nodes.
   -->
   <xsl:variable name="managedComponentNode" select="key('classById', $managedComponentId)"/>
   <xsl:variable name="homeNode" select="key('classByName', $_homeName)"/>

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

import cdmw.ccm.cif.CCMHomeImpl;
import cdmw.ccm.cif.HomeConfigurationImpl;

public class <xsl:value-of select="$homeImplClassname"/> extends CCMHomeImpl
   implements <xsl:value-of select="$javaScopedHomeOperationsName"/> {

         <xsl:variable name="cdmwHomeRepId">
            <xsl:call-template name="replaceCharsInString">
               <xsl:with-param name="_stringIn" select="$homeNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag =    'typeid']/@value"/>
               <xsl:with-param name="_charsIn" select="$javaHomeScope"/>
               <xsl:with-param name="_charsOut" select="concat('Cdmw_', $javaHomeScope)"/>
            </xsl:call-template>
         </xsl:variable>
         public static final String HOME_REP_ID = "<xsl:value-of select="$cdmwHomeRepId"/>";
         public static final String COMP_REP_ID = "<xsl:value-of  select="$managedComponentNode/UML:ModelElement.taggedValue/UML:TaggedValue[@tag = 'typeid']/@value"/>";

         public static final String[] ids = {
            <xsl:call-template name="getObjectList">
               <xsl:with-param name="_nodeName" select="$managedComponentNode/@name"/>
            </xsl:call-template>
            "IDL:omg.org/Components/Events:1.0",
            "IDL:omg.org/Components/Receptacles:1.0",
            "IDL:omg.org/Components/Navigation:1.0",
            "IDL:omg.org/Components/CCMObject:1.0"
         };

         /**
          * private class used to delegate methods from HomeConfigurationImpl
          */
         private class HomeConfigurationImplDelegate extends HomeConfigurationImpl {
            
            public org.omg.Components.IRObject get_component_def() { return null;}
            
            public org.omg.Components.IRObject get_home_def() { return null;}
            
            public void remove_component(org.omg.Components.CCMObject comp) 
               throws org.omg.Components.RemoveFailure {}
         
         }
         
         /**
          * delegate instance
          */
         private HomeConfigurationImplDelegate delegate = new HomeConfigurationImplDelegate();

         /**
          * Home executor
          */
         private <xsl:value-of select="concat($javaHomeScope, $javaSep, 'CCM_', $_homeName)"/> homeExecutor;
         
         /**
          * Constructor
          */
         protected <xsl:value-of select="$homeImplClassname"/>(
            org.omg.Components.HomeExecutorBase homeExec,
            cdmw.lifecycle.ObjectDeactivator deactivator) {
            super(HOME_REP_ID, COMP_REP_ID, deactivator);
            this.homeExecutor = <xsl:value-of select="concat($javaHomeScope, $javaSep, 'CCM_', $_homeName)"/>Helper.narrow(homeExec);
            if (this.homeExecutor == null) {
               throw new org.omg.CORBA.BAD_PARAM(
                  ExceptionMinorCodes.BAD_PARAMInvalidHomeExecutor, 
                  org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
         }

         /**
          * Attributes
          */
         <xsl:call-template name="findAllJavaInterfaceReadWriteAttributes">
            <xsl:with-param name="_interfaceName" select="$_homeName"/>
            <xsl:with-param name="_type" select="'Home'"/>
            <xsl:with-param name="_template" select="'service_home_java.content.2'"/>
            <xsl:with-param name="_strategy" select="'Service'"/>
         </xsl:call-template>

         /**
          * Explicit interface
          */

         <xsl:call-template name="findAllJavaHomeFactoryOperations">
            <xsl:with-param name="_homeName" select="$_homeName"/>
            <xsl:with-param name="_template" select="'service_home_java.content.1'"/>
         </xsl:call-template>

         /**
          * Implicit interface
          */

         public <xsl:value-of select="$scopedManagedComponentName"/> create()
            throws org.omg.Components.CreateFailure {

            org.omg.CORBA.Object obj = create_component_ref();
            <xsl:value-of select="$scopedManagedComponentName"/> result = <xsl:value-of  select="$scopedManagedComponentName"/>Helper.narrow(obj);
      
            if (result == null) {
               System.err.println("Created reference is not a <xsl:value-of select="$managedComponentNode/@name"/>!");
               throw new org.omg.CORBA.INTERNAL(
                  ExceptionMinorCodes.INTERNAL, 
                  org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
      
            if (this.configurator != null) {
                try {
                    // a configurator exist!
                    this.configurator.configure(result);
                } catch (org.omg.Components.WrongComponentType wct) {
                  throw new org.omg.Components.CreateFailure(
                     com.thalesgroup.CdmwDeployment.WRONG_COMPONENT_KIND.value);
                }
            }

            return result;

         }

         // Keyless home operation

         public org.omg.Components.CCMObject create_component()
            throws org.omg.Components.CreateFailure {
            return create();
         }

         <![CDATA[
            /**
             * This method acts as a hand shaking between the Servant Provider
             * and the generated Factory implementation. It indicates whether
             * a Servant provided by this Servant Provider may incarnate multiple
             * objects (with ObjectIds provided by this Servant Provider)
             *
             * @return <B>true</B> if a servant may incarnate multiple objects,
             * <B>false</B> otherwise.
             */
            public boolean mayIncarnateMultipleObjects() {
               return false;
            }

            public void remove() throws org.omg.CosLifeCycle.NotRemovable {
               this.homeExecutor = null;
               super.remove();
            }
         ]]>

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
            protected String[] getIds() {
               return ids;
            }
         ]]>

         <![CDATA[
            /**
             * This returns the number of repository Ids supported by the current ServantProvider.
             * <B>NOTA:</B> This should correspond to the number of null terminated strings
             * returned by getIds() operation.
             */
            protected int getNumberOfIds() {
               return ids.length;
            }
         ]]>

         /**
          * This creates new component servant.
          * This shall be implemented by the well typed home servant.
          *
          * @param ctx      The component context
          * @param executor The component executor
          *
          * @return The new component servant.
          * @exception Components::CreateFailure if a problem occured when creating executor.
          */
         protected org.omg.PortableServer.Servant create_component_servant(
            String compOid,
            com.thalesgroup.CdmwCcmContainer.CCM2Context ctx,
            org.omg.Components.EnterpriseComponent executor)
            throws org.omg.Components.CreateFailure {
         
               // Check if an executor has been created at ref creation
               // that is a factory operation has been used to create ref.
               org.omg.Components.EnterpriseComponent compExec = executor;
               boolean isCreatedByFactoryOperation = false;
         
               if (compExec == null) {
                  // Create executor
                  try {
                     compExec = this.homeExecutor.create();
                  } catch (org.omg.Components.CCMException ccme) {
                     System.err.println("Error occured during component executor creation!");
                     throw new org.omg.Components.CreateFailure(
                        com.thalesgroup.CdmwDeployment.CREATE_FAILURE.value);
                  }
                  
                  if (compExec == null) {
                     System.err.println("Component executor is nil!");
                     throw new org.omg.Components.CreateFailure(
                        com.thalesgroup.CdmwDeployment.INVALID_COMPONENT_EXECUTOR.value);
                  }
               } else {
                  isCreatedByFactoryOperation = true;
               }
         
               try {
                  <xsl:value-of select="concat('Service', $managedComponentNode/@name, 'Impl')"/> delegate =
                     new <xsl:value-of select="concat('Service', $managedComponentNode/@name, 'Impl')"/>(compOid, ctx, compExec, isCreatedByFactoryOperation);
                  return new <xsl:value-of select="$scopedManagedComponentName"/>POATie(delegate);
               } catch(org.omg.Components.CCMException ccme) {
                  System.err.println("Error occured during component servant creation!");
                  throw new org.omg.Components.CreateFailure(
                      com.thalesgroup.CdmwDeployment.CREATE_FAILURE.value);
               }
         
         }
         
         protected cdmw.ccm.cif.CCMObjectImpl getCCMObjectImpl(
             org.omg.PortableServer.Servant servant) {

             try {
                 <xsl:value-of select="$scopedManagedComponentName"/>POATie tie =
                     (<xsl:value-of select="$scopedManagedComponentName"/>POATie) servant;
                 return (<xsl:value-of select="concat('Service', $managedComponentNode/@name, 'Impl')"/>) tie._delegate();
             } catch(ClassCastException cce) {
                 return null;
             }

         }

         public static org.omg.PortableServer.Servant allocate(
            org.omg.Components.HomeExecutorBase homeExecutor,
            cdmw.lifecycle.ObjectDeactivator deactivator) {
            <xsl:value-of select="$homeImplClassname"/> delegate = 
                new <xsl:value-of select="$homeImplClassname"/>(homeExecutor, deactivator);
            <xsl:variable name="javaScopedHomePOATieName">
               <xsl:variable name="tempName">
                  <xsl:call-template name="prefixModuleName">
                     <xsl:with-param name="_prefix" select="'Cdmw_'"/>
                     <xsl:with-param name="_idlPrefix" select="$idlPragmaPrefix"/>
                     <xsl:with-param name="_scopedName" select="concat($javaHomeScope, $javaSep, $_homeName)"/>
                  </xsl:call-template>
               </xsl:variable>
               <xsl:value-of select="concat($tempName, 'POATie')"/>
            </xsl:variable>
            return new <xsl:value-of select="$javaScopedHomePOATieName"/>(delegate);
         }
         
         public static org.omg.PortableServer.Servant getDelegate(
            org.omg.PortableServer.Servant servant) {
            
            try {
                <xsl:value-of select="$javaScopedHomePOATieName"/> tie =
                    (<xsl:value-of select="$javaScopedHomePOATieName"/>) servant;
                return (<xsl:value-of select="$homeImplClassname"/>) tie._delegate();
            } catch(ClassCastException cce) {
                return null;
            }
            
         }
         
         //
         // methods from HomeConfigurationImpl
         //
         
         protected org.omg.Components.Configurator configurator = null;
         
         public void set_configurator(org.omg.Components.Configurator cfg) {
            this.delegate.set_configurator(cfg);
            this.configurator = cfg;
         }
         
         public void set_configuration_values(org.omg.Components.ConfigValue[] config) {
            this.delegate.set_configuration_values(config);
         }
         
         public void complete_component_configuration(boolean b) {
            this.delegate.complete_component_configuration(b);
         }
         
         public void disable_home_configuration() {
            this.delegate.disable_home_configuration();
         }
         
         public org.omg.Components.Configurator getConfigurator() {
            return this.configurator;
         }
         
         public org.omg.Components.ConfigValue[] getConfigValues() {
            return this.delegate.getConfigValues();
         }
         
         public boolean isCompleteCompConfEnable() {
            return this.delegate.isCompleteCompConfEnable();
         }

}

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template service_home_java.content -->



<xsl:template name="service_home_java.content.1">
   <xsl:param name="_node"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_operationName"/>
   <xsl:param name="_homeImplClassname"/>
   <xsl:param name="_managedComponentName"/>
   <xsl:param name="_scopedManagedComponentName"/>

   // Factory operation

   <xsl:value-of select="concat($lf, 'public ', $_returnType, $lf, $_operationName, '(', $lf)"/>
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
      ) throws org.omg.Components.CreateFailure {
         // Create executor
         org.omg.Components.EnterpriseComponent compExec = null;
         
         <!--try {-->
            compExec = this.homeExecutor.<xsl:value-of select="$_operationName"/> (
               <xsl:for-each select="$_node/UML:BehavioralFeature.parameter/UML:Parameter[@kind != 'return']">
                  <xsl:value-of select="@name"/>
                  <xsl:if test="position() != last()">
                     <xsl:text>, </xsl:text>
                  </xsl:if>
               </xsl:for-each>
               );
         <!-- //TODO: remove this catch statement if not needed
         } catch (org.omg.Components.CCMException ccme) {
            System.err.println("Error occured during component executor creation!");
            throw ccme;
         }
         -->
          
         if (compExec == null) {
            System.err.println("Component executor is nil!");
            throw new org.omg.Components.CreateFailure(
               com.thalesgroup.CdmwDeployment.INVALID_COMPONENT_EXECUTOR.value);
         }
          
         org.omg.CORBA.Object obj = create_component_ref(compExec);
         <xsl:value-of select="$_scopedManagedComponentName"/> result = 
            <xsl:value-of select="$_scopedManagedComponentName"/>Helper.narrow(obj);
          
         if (result == null) {
            System.err.println("Created reference is not a <xsl:value-of select="$_managedComponentName"/>!");
            throw new org.omg.CORBA.INTERNAL(
               ExceptionMinorCodes.INTERNAL, 
               org.omg.CORBA.CompletionStatus.COMPLETED_NO);
         }

         return result;
      }
</xsl:template> <!-- end of template service_home_java.content.1 -->


<!--
-->
<xsl:template name="service_home_java.content.2">
   <xsl:param name="_node"/>
   <xsl:param name="_readonlyId"/>
   <xsl:param name="_readonlyEndId"/>
   <xsl:param name="_paramName"/>
   <xsl:param name="_paramType"/>
   <xsl:param name="_returnType"/>
   <xsl:param name="_javaOperationName"/>
   <xsl:param name="_homeObjRefVar"/>
   <xsl:param name="_objRefVarType"/>

   <xsl:if test="not(boolean($_node/@stereotype)) or ($_node/@stereotype = $_readonlyEndId) or ($_node/@stereotype = $_readonlyId)">
      <xsl:value-of select="concat($lf, 'public ', $_returnType, ' ', $_javaOperationName, '() {', $lf)"/>
         <xsl:value-of select="$_objRefVarType"/> result = homeExecutor.<xsl:value-of select="$_javaOperationName"/>();

         return result;
      }

      <xsl:if test="not($_node/@stereotype = $_readonlyEndId or $_node/@stereotype = $_readonlyId)">
         <xsl:value-of select="concat($lf, 'public void ', $_javaOperationName, '(', $_paramType, ' ', $_paramName, ') {', $lf)"/>
             homeExecutor.<xsl:value-of select="concat($_javaOperationName, '(', $_paramName, ');')"/>
         }
      </xsl:if>
   </xsl:if>
   
</xsl:template> <!-- end of template service_home_java.content.2 -->



</xsl:stylesheet>


