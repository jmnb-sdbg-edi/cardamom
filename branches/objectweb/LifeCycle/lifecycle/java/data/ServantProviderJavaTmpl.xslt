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
<xsl:template name="servant_provider_java">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>

   <xsl:if test="boolean($factoryNodes)">
      <xsl:if test="$_index &lt;= count($factoryNodes)">
         <xsl:variable name="factoryNode" select="$factoryNodes[$_index]"/>

         <!--
            References to some nodes.
         -->
         <xsl:variable name="servantProviderNode" select="$factoryNode/servant-provider"/>

         <xsl:if test="boolean($servantProviderNode)">
            <!--
               Get the servant provider classname.
            -->
            <xsl:variable name="scopedServantProviderClassname" select="$servantProviderNode/@classname"/>
            <xsl:variable name="servantProviderClassname">
               <xsl:call-template name="getLastToken">
                  <xsl:with-param name="_string" select="$scopedServantProviderClassname"/>
                  <xsl:with-param name="_separator" select="$xmlSep"/>
               </xsl:call-template>
            </xsl:variable>
         
            <!--
               Get the output filename.
            -->
            <xsl:variable name="filename">
               <xsl:call-template name="buildOutputFilename">
                  <xsl:with-param name="_userDirectory" select="$servantProviderNode/@directory"/>
                  <!-- in Java, the filename must match the classname in order to compile -->
                  <xsl:with-param name="_userFilename" select="$servantProviderClassname"/>
                  <xsl:with-param name="_defaultFilename" select="$servantProviderClassname"/>
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
               <xsl:call-template name="servant_provider_java.content">
                  <xsl:with-param name="_interfaceName">
                     <xsl:call-template name="getLastToken">
                        <xsl:with-param name="_string" select="$factoryNode/@idl-interface"/>
                        <xsl:with-param name="_separator" select="$xmlSep"/>
                     </xsl:call-template>
                  </xsl:with-param>
                  <xsl:with-param name="_servantProviderClassname" select="$servantProviderClassname"/>
                  <xsl:with-param name="_scopedServantProviderClassname" select="$scopedServantProviderClassname"/>
               </xsl:call-template>
            </xsl:document>
         </xsl:if>

         <!--
            Proceed to the next item in idlNodes if any.
         -->
         <xsl:call-template name="servant_provider_java">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template servant_provider_java -->



<!--
   This template generates the content of the file.

   @param _interfaceName                    The idl interface to visit.
   @param _servantProviderClassname         The servant provider class name.
   @param _scopedServantProviderClassname   The scoped servant provider class name.
-->

<xsl:template name="servant_provider_java.content">
   <xsl:param name="_interfaceName"/>
   <xsl:param name="_servantProviderClassname"/>
   <xsl:param name="_scopedServantProviderClassname"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="interfaceNode" select="key('classByName', $_interfaceName)"/>
   
   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="javaScopedServantProviderClassname">
      <xsl:value-of select="$_scopedServantProviderClassname"/>
   </xsl:variable>
   
   <!--
      The code generation starts here.
   -->
   
   <xsl:variable name="package">
      <xsl:call-template name="getScope">
         <xsl:with-param name="_name" select="$javaScopedServantProviderClassname"/>
         <xsl:with-param name="_lang" select="'java'"/>
      </xsl:call-template>
   </xsl:variable>

package <xsl:value-of select="$package"/>;

public abstract class <xsl:value-of select="$_servantProviderClassname"/>
    extends cdmw.lifecycle.ServantProviderBase {

    /**
     * IDs
     */
    private final static String[] ids = {
         <xsl:call-template name="getObjectList">
            <xsl:with-param name="_nodeName" select="$_interfaceName"/>
            <xsl:with-param name="_lang" select="'java'"/>
         </xsl:call-template>
    };
    
    <!--
        Create the operations definitions.
    -->
         <xsl:call-template name="findAllJavaFactoryOperations">
            <xsl:with-param name="_interfaceName" select="$_interfaceName"/>
            <xsl:with-param name="_template" select="'servant_provider_java.content.1'"/>
            <xsl:with-param name="_isServantProvider" select="true()"/>
         </xsl:call-template>
   
    /**
     * Returns the list of IDs
     *
     * @return The list of IDs as a string table
     */
    protected String[] getIds() {
        return ids;
    }

    /**
     * Returns the number of IDs
     *
     * @return The number of IDs
     */
    protected int getNumberOfIds() {
        return ids.length;
    }

}
   <!--
      The code generation stops here.
   -->

</xsl:template> <!-- end of template servant_provider_java.content -->


<xsl:template name="servant_provider_java.content.1">
    <xsl:param name="_node"/>
    <xsl:param name="_operationName"/>

      <xsl:value-of select="concat('public abstract byte[] ', $_operationName, '(', $lf, '        ')"/>
            <!--
               IN parameters.
            -->
            <xsl:for-each select="$_node/UML:BehavioralFeature.parameter/UML:Parameter[@kind = 'in' and not(@type = $keyNodeId)]">
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
                  <xsl:value-of select="concat(',', $lf, '        ')"/>
               </xsl:if>
            </xsl:for-each>
         <xsl:value-of select="concat(')', $lf, '        throws', $lf)"/>
         <!--
            Exceptions.
         -->
         <xsl:for-each select="$_node/UML:ModelElement.taggedValue/UML:TaggedValue">
            <xsl:if test="(@tag = 'raises') and not(@value = $alreadyExistentId) and not(@value = $invalidNameId)">
               <xsl:variable name="scopedException">
                  <xsl:call-template name="getScopedException">
                     <xsl:with-param name="_exception" select="@value"/>
                  </xsl:call-template>
               </xsl:variable>
               <xsl:value-of select="concat('            ', $scopedException, ',', $lf)"/>
            </xsl:if>
         </xsl:for-each>
        <xsl:value-of select="concat('            org.omg.CORBA.SystemException;', $lf, $lf, '    ')"/>

</xsl:template>


</xsl:stylesheet>


