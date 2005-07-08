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
<xsl:template name="ccmcif_consumer_java">
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
               <xsl:variable name="consumerImplClassname" select="concat($consumerClassname, 'Impl')"/>
               
               <!--
                  Get the output filename.
               -->
               <xsl:variable name="filename">
                  <xsl:call-template name="buildOutputFilename">
                     <xsl:with-param name="_userDirectory" select="$homeImplNode/@directory"/>
                     <xsl:with-param name="_userFilename" select="$homeImplNode/@filename"/>
                     <xsl:with-param name="_defaultFilename" select="$consumerImplClassname"/>
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
                  Generate file.
               -->
               <xsl:document method="text" href="{$filename}">
                  <!--
                     Insert document header.
                  -->
                  <xsl:call-template name="docHeader">
                     <xsl:with-param name="_lang" select="'java'"/>
                  </xsl:call-template>

                  <!--
                     Insert the content.
                  -->
                  <xsl:call-template name="ccmcif_consumer_java.content">
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
         <xsl:call-template name="ccmcif_consumer_java">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template ccmcif_consumer_java -->



<!--
   This template generates the content of the file.

   @param _scopedHomeImplClassname   The scoped home impl classname.
   @param _consumerTypeName          The consumer type.
   @param _consumerClassname         The consumer class name.
   @param _consumerVarName           The name of the consumer as declared in IDL.
   @param _componentName             The name of the component where the consumer is declared.
-->
<xsl:template name="ccmcif_consumer_java.content">
   <xsl:param name="_scopedHomeImplClassname"/>
   <xsl:param name="_consumerTypeName"/>
   <xsl:param name="_consumerClassname"/>
   <xsl:param name="_consumerVarName"/>
   <xsl:param name="_componentName"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="consumerImplClassname" select="concat($_consumerTypeName, 'ConsumerImpl')"/>
   <xsl:variable name="javaScopedConsumerTypeName">
      <xsl:call-template name="getScopedName">
         <xsl:with-param name="_name" select="$_consumerTypeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="javaConsumerScope">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$javaScopedConsumerTypeName"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="javaScopedConsumerPOAName">
      <xsl:variable name="newScopedConsumerName">
         <xsl:call-template name="prefixModuleName">
            <xsl:with-param name="_prefix" select="'Cdmw_'"/>
            <xsl:with-param name="_idlPrefix" select="$idlPragmaPrefix"/>
            <xsl:with-param name="_scopedName" select="$javaScopedConsumerTypeName"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($newScopedConsumerName, 'ConsumerPOA')"/>
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

<xsl:variable name="package">
   <xsl:call-template name="getScope">
      <xsl:with-param name="_name" select="$_scopedHomeImplClassname"/>
      <xsl:with-param name="_lang" select="'java'"/>
   </xsl:call-template>
</xsl:variable>
package <xsl:value-of select="$package"/>;

import cdmw.orbsupport.ExceptionMinorCodes;
import cdmw.ccm.cif.LifeCycleObjectBase;

public class <xsl:value-of select="$consumerImplClassname"/> 
    extends <xsl:value-of select="$javaScopedConsumerPOAName"/> {
    
    private org.omg.Components.ExecutorLocator executorLocator;

    private String sinkName;

    private org.omg.PortableServer.Servant component;
    
    private LifeCycleObjectBase lifecycleObjectBase;
    
    public <xsl:value-of select="$consumerImplClassname"/>(
        org.omg.Components.ExecutorLocator execLocator, 
        String name,
        org.omg.Components.CCM2Context ctx,
        org.omg.PortableServer.Servant component) {

        this.lifecycleObjectBase = new LifeCycleObjectBase(ctx);
        this.executorLocator = execLocator;
        this.sinkName = name;
        this.component = component;

    }
         
    public void push_<xsl:value-of select="concat($_consumerTypeName, '(', $javaScopedConsumerTypeName, ' the', $_consumerTypeName, ')')"/> {

        try {
            org.omg.CORBA.Object obj = this.executorLocator.obtain_executor(this.sinkName);
     
            if (obj == null) {
                throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                    ExceptionMinorCodes.OBJECT_NOT_EXIST, 
                    org.omg.CORBA.CompletionStatus.COMPLETED_NO);
            }
    
            try {
               // For monolithic component, push operation is implemented in <xsl:value-of select="$_componentName"/>
               // component instead of the consumer executor for segmented component
               <xsl:value-of select="concat($javaConsumerScope, $javaSep, 'CCM_', $_componentName, ' ', $lowercaseComponentName, 'Exec = ', $javaConsumerScope, $javaSep, 'CCM_', $_componentName, 'Helper.narrow(obj);')"/>
               <xsl:value-of select="$lowercaseComponentName"/>Exec.push_<xsl:value-of select="concat($_consumerVarName, '(the', $_consumerTypeName,');')"/>
            } catch(org.omg.CORBA.BAD_PARAM bp1) {
                try {
                   // Component may segmented
                   <xsl:value-of select="concat($javaConsumerScope, $javaSep, 'CCM_', $_consumerClassname, ' consumerExec = ', $javaConsumerScope, $javaSep, 'CCM_', $_consumerClassname, 'Helper.narrow(obj);')"/>
                   // push event to the consumer executor
                   consumerExec.push(the<xsl:value-of select="$_consumerTypeName"/>);
                } catch(org.omg.CORBA.BAD_PARAM bp2) {
                   throw new org.omg.CORBA.OBJECT_NOT_EXIST(
                       ExceptionMinorCodes.OBJECT_NOT_EXIST, 
                       org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }
            }

            this.executorLocator.release_executor(obj);
        } catch(org.omg.Components.CCMException ccme) {
            throw new org.omg.CORBA.INTERNAL(
                ExceptionMinorCodes.INTERNALCCMCIFError, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
        
    }

    public void push_event(org.omg.Components.EventBase evt)
        throws org.omg.Components.BadEventType {
        
        try {
            <xsl:value-of select="$javaScopedConsumerTypeName"/> infoEvt =
               (<xsl:value-of select="$javaScopedConsumerTypeName"/>) evt;
            this.push_<xsl:value-of select="$_consumerTypeName"/>(infoEvt);
        } catch(ClassCastException cce) {
            throw new org.omg.Components.BadEventType();
        }
        
    }

    public void push(org.omg.CORBA.Any data)
        throws org.omg.CosEventComm.Disconnected {

        <xsl:value-of select="$javaScopedConsumerTypeName"/> event = 
            <xsl:value-of select="$javaScopedConsumerTypeName"/>Helper.read(data.create_input_stream());

        try {
            this.push_<xsl:value-of select="$_consumerTypeName"/>(event);
        } catch (org.omg.CORBA.SystemException se) {
            throw se;
        }

    }

    public void disconnect_push_consumer() {}
    
    // methods from LifeCycleObjectBase to delegate operations
    
    public org.omg.CosLifeCycle.LifeCycleObject copy(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws
            org.omg.CosLifeCycle.NoFactory,
            org.omg.CosLifeCycle.NotCopyable,
            org.omg.CosLifeCycle.InvalidCriteria,
            org.omg.CosLifeCycle.CannotMeetCriteria {
        return this.lifecycleObjectBase.copy(there, the_criteria);
    }
    
    public void move(
        org.omg.CosLifeCycle.FactoryFinder there,
        org.omg.CosLifeCycle.NVP[] the_criteria)
        throws
            org.omg.CosLifeCycle.NoFactory,
            org.omg.CosLifeCycle.NotMovable,
            org.omg.CosLifeCycle.InvalidCriteria,
            org.omg.CosLifeCycle.CannotMeetCriteria {
        this.lifecycleObjectBase.move(there, the_criteria);
    }

    public void remove() throws org.omg.CosLifeCycle.NotRemovable {
        this.lifecycleObjectBase.remove();
    }

}

   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template ccmcif_consumer_java.content -->



</xsl:stylesheet>


