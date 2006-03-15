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


<xsl:stylesheet version="1.1" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text"/>



<!--
   Stylesheet parameters.
   The default values can be overridden by command-line arguments.
-->



<!--
   This parameter defines to template to apply.
-->
<xsl:param name="TEMPLATE"/>

<!--
   This parameter defines the (installed) CDMW IDL path.
-->
<xsl:param name="CDMWIDL"/>

<!--
   This parameter defines the temporary directory for the generation of the XMI document.
   The default directory is set to '/tmp'.
-->
<xsl:param name="TMPDIR" select="'/tmp'"/>

<!--
   This parameter defines the name of the generated XMI document.<br>
   The default value is set to 'generated.xmi'.
-->
<xsl:param name="XMIFILE" select="'generated.xmi'"/>

<!--
   This parameter tells the generator to ouput only either XMI or IDL2.
-->
<xsl:param name="ONLY"/>



<!--
   Global variables.
-->



<!--
   This variable defines the separator in use within XML documents.
-->
<xsl:variable name="xmlSep" select="'.'"/>

<!--
   For use in <b>translate()</b> to lower/upper case a string.
-->
<xsl:variable name="lcase" select="'abcdefghijklmnopqrstuvwxyz-_.'"/>

<!--
   For use in <b>translate()</b> to lower/upper case a string.
-->
<xsl:variable name="ucase" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ-_.'"/>



<!--
   This template returns the last token of a string.

   @param _string      The tokenized string.
   @param _separator   The token separator used in the string.
-->
<xsl:template name="getLastToken">
   <xsl:param name="_string"/>
   <xsl:param name="_separator" select="$xmlSep"/>

   <xsl:variable name="substringAfter" select="substring-after($_string, $_separator)"/>

   <xsl:choose>
      <!--
         Does the string contain any separator ?
         If true and the string does not end with a separator,
         then proceed with the remaining substring.
      -->
      <xsl:when test="contains($_string, $_separator) and string-length($substringAfter) > 0">
         <xsl:call-template name="getLastToken">
            <xsl:with-param name="_string" select="$substringAfter"/>
            <xsl:with-param name="_separator" select="$_separator"/>
         </xsl:call-template>
      </xsl:when>
      <!--
         Recursiveness end statement : string with no separators or string ending with a separator.
      -->
      <xsl:otherwise>
         <xsl:choose>
            <xsl:when test="contains($_string, $_separator) and string-length($substringAfter) = 0">
               <xsl:value-of select="substring-before($_string, $_separator)"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="$_string"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template getLastToken -->



<!--
   Main - This template determines which templates to apply according
   to the template name set by the user.

   @param none
-->
<xsl:template match="/">
   <xsl:variable name="template" select="translate($TEMPLATE, $ucase, $lcase)"/>
   
   <xsl:choose>
      <xsl:when test="$template = 'get_module_to_export'">
         <xsl:call-template name="get_module_to_export"/>
      </xsl:when>
      <xsl:when test="$template = 'get_idl_filenames'">
         <xsl:call-template name="get_idl_filenames"/>
      </xsl:when>
      <xsl:when test="$template = 'get_idl_dirs'">
         <xsl:call-template name="get_idl_dirs">
            <xsl:with-param name="_searchDirs" select="//idl-search-dir"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:when test="$template = 'get_preproc_flags'">
         <xsl:call-template name="get_preproc_flags">
            <xsl:with-param name="_preprocFlags" select="//preprocessing-flags"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:when test="$template = 'has_only_user_factories'">
         <xsl:call-template name="has_only_user_factories"/>
      </xsl:when>
      <xsl:when test="$template = 'has_only_factories_defs'">
         <xsl:call-template name="has_only_factories_defs"/>
      </xsl:when>
      <xsl:when test="$template = 'must_generate_idl2'">
         <xsl:call-template name="must_generate_idl2"/>
      </xsl:when>
      <xsl:when test="$template = 'get_idl2_filename'">
         <xsl:call-template name="get_idl2_filename"/>
      </xsl:when>
      <xsl:when test="$template = 'build_export_command_line_args'">
         <xsl:call-template name="build_export_command_line_args"/>
      </xsl:when>
      <xsl:when test="$template = 'get_idls'">
         <xsl:call-template name="get_idls"/>
      </xsl:when>
   </xsl:choose>
</xsl:template>



<!--
   This template outputs the CORBA module we want to export from IDL to XMI.

   @param none
-->
<xsl:template name="get_module_to_export">
   <xsl:for-each select="/cdmw/factories/factory">
      <xsl:text></xsl:text><xsl:value-of select="@idl-interface"/>
      <xsl:if test="position() != last()">
         <xsl:text>,</xsl:text>
      </xsl:if>
   </xsl:for-each>
   <xsl:text>,</xsl:text>
   <xsl:for-each select="/cdmw/datastore-types/datastore-type">
      <xsl:text></xsl:text><xsl:value-of select="@oid-type"/>
      <xsl:if test="position() != last()">
         <xsl:text>,</xsl:text>
      </xsl:if>
   </xsl:for-each>
   <xsl:text>,</xsl:text>
   <xsl:for-each select="/cdmw/datastore-types/datastore-type">
      <xsl:text></xsl:text><xsl:value-of select="@data-type"/>
      <xsl:if test="position() != last()">
         <xsl:text>,</xsl:text>
      </xsl:if>
   </xsl:for-each>
</xsl:template> <!-- end of template get_module_to_export -->


<!--
   This template returns the IDL filename(s) to include when feeding OpenCCM.

   @param none
-->
<xsl:template name="get_idl_filenames">
   <xsl:for-each select="/cdmw/factories/factory">
      <xsl:choose>
         <xsl:when test="boolean(@idl-filename)">
            <xsl:text></xsl:text><xsl:value-of select="@idl-filename"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:variable name="interfaceName">
               <xsl:call-template name="getLastToken">
                  <xsl:with-param name="_string" select="@idl-interface"/>
                  <xsl:with-param name="_separator" select="$xmlSep"/>
               </xsl:call-template>
            </xsl:variable>
            <xsl:text></xsl:text><xsl:value-of select="concat($interfaceName, '.idl')"/>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:if test="position() != last()">
         <xsl:text> </xsl:text>
      </xsl:if>
   </xsl:for-each>
   <xsl:variable name="existFactories" select="/cdmw/factories/factory"/>
   <xsl:if test="boolean($existFactories)">
      <xsl:variable name="existDatastores" select="/cdmw/datastore-types/datastore-type"/>
      <xsl:if test="boolean($existDatastores)">
         <xsl:text> </xsl:text>
      </xsl:if>
   </xsl:if>
   <xsl:for-each select="/cdmw/datastore-types/datastore-type">
      <xsl:if test="boolean(@idl-filename)">
         <xsl:text></xsl:text><xsl:value-of select="@idl-filename"/>
      </xsl:if>
      <xsl:if test="position() != last()">
         <xsl:text> </xsl:text>
      </xsl:if>
   </xsl:for-each>
</xsl:template> <!-- end of template get_idl_filenames -->


<!--
   This template returns the directories where the IDLs are stored.<br>
   Duplicates are output only once.

   @param _searchDirs   The IDL directories.
-->
<xsl:template name="get_idl_dirs">
   <xsl:param name="_searchDirs"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   <xsl:param name="_dirs" select="''"/>

   <xsl:choose>
      <xsl:when test="$_index &lt;= count($_searchDirs)">
         <xsl:call-template name="get_idl_dirs">
            <xsl:with-param name="_searchDirs" select="$_searchDirs"/>
            <xsl:with-param name="_index" select="$_index + 1"/>
            <xsl:with-param name="_dirs">
               <xsl:choose>
                  <xsl:when test="string-length($_searchDirs[$_index]) = 0 or 
                                  contains($_dirs, concat(' --I', $_searchDirs[$_index], ' '))">
                     <xsl:value-of select="$_dirs"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat($_dirs, ' --I', $_searchDirs[$_index], ' ')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
         <xsl:text></xsl:text><xsl:value-of select="$_dirs"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template get_idl_dirs -->


<!--
   This template returns the preprocessing flags specified in all factories.<br>
   Duplicates are output only once.

   @param _preprocFlags   The preprocessing flags.
-->
<xsl:template name="get_preproc_flags">
   <xsl:param name="_preprocFlags"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   <xsl:param name="_flags" select="''"/>

   <xsl:choose>
      <xsl:when test="$_index &lt;= count($_preprocFlags)">
         <xsl:call-template name="get_preproc_flags">
            <xsl:with-param name="_preprocFlags" select="$_preprocFlags"/>
            <xsl:with-param name="_index" select="$_index + 1"/>
            <xsl:with-param name="_flags">
               <xsl:choose>
                  <xsl:when test="string-length($_preprocFlags[$_index]) = 0 or 
                                  contains($_flags, $_preprocFlags[$_index])">
                     <xsl:value-of select="$_flags"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat($_flags, $_preprocFlags[$_index], ' ')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:with-param>
         </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
         <xsl:text></xsl:text><xsl:value-of select="$_flags"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template get_preproc_flags -->




<!--
   This function returns true() if no factories are to be generated.

   @param none
-->
<xsl:template name="has_only_user_factories">
   <xsl:variable name="lifecycleFrameworkNodes" select="/cdmw/lifecycle-frameworks/lifecycle-framework"/>
   <xsl:choose>
      <xsl:when test="boolean($lifecycleFrameworkNodes/factory-impl) or
                      boolean($lifecycleFrameworkNodes/home-impl)">
         <xsl:value-of select="false()"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="true()"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template has_only_user_factories -->



<!--
   This function returns true() when /cdmw/factories contains at least one child element
   while /cdmw/lifecycle-frameworks and /cdmw/programs are empty.

   @param none
-->
<xsl:template name="has_only_factories_defs">
   <xsl:choose>
      <xsl:when test="count(/cdmw/factories/factory) > 0 and
                      count(/cdmw/lifecycle-frameworks/lifecycle-framework) = 0 and
                      count(/cdmw/programs/program) = 0">
         <xsl:value-of select="true()"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="false()"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template has_only_factories_defs -->



<!--
   This function tells the code generator whether it should generate
   IDL2 from IDL3.

   @param none
-->
<xsl:template name="must_generate_idl2">
   <xsl:choose>
      <xsl:when test="boolean(/cdmw/factories/factory[@kind = 'HOME'])">
         <xsl:value-of select="true()"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="false()"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template must_generate_idl2 -->



<!--
   This template builds an IDL2 filename from the IDL3 filename.

   @param none
-->
<xsl:template name="get_idl2_filename">
   <xsl:variable name="homeFactoryNode" select="/cdmw/factories/factory[@kind = 'HOME']"/>   
   <xsl:choose>
      <xsl:when test="boolean($homeFactoryNode/@idl-filename)">
         <xsl:text></xsl:text><xsl:value-of select="concat(substring-before($homeFactoryNode/@idl-filename, '.idl'), '_cif.idl')"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:variable name="interfaceName">
            <xsl:call-template name="getLastToken">
               <xsl:with-param name="_string" select="$homeFactoryNode/@idl-interface"/>
               <xsl:with-param name="_separator" select="$xmlSep"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:value-of select="concat($interfaceName, '_cif.idl')"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template get_idl2_filename -->



<!--
   This template returns a list of all the IDLs referred in the XML document.

   @param none
-->
<xsl:template name="get_idls">
   <xsl:for-each select="/cdmw/factories/factory">
      <xsl:variable name="idl_basename">
         <xsl:choose>
            <xsl:when test="boolean(@idl-filename)">
               <xsl:value-of select="@idl-filename"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:variable name="interfaceName">
                  <xsl:call-template name="getLastToken">
                     <xsl:with-param name="_string" select="@idl-interface"/>
                     <xsl:with-param name="_separator" select="$xmlSep"/>
                  </xsl:call-template>
               </xsl:variable>
               <xsl:value-of select="concat($interfaceName, '.idl')"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:variable>
      <xsl:choose>
         <xsl:when test="count(idl-search-dir) > 0">
            <xsl:for-each select="idl-search-dir">
               <xsl:variable name="dirname" select="."/>
               <xsl:text></xsl:text><xsl:value-of select="concat($dirname, '/', $idl_basename, ' ')"/>
            </xsl:for-each>
         </xsl:when>
         <xsl:otherwise>
            <xsl:for-each select="//idl-search-dir">
               <xsl:variable name="dirname" select="."/>
               <xsl:text></xsl:text><xsl:value-of select="concat($dirname, '/', $idl_basename, ' ')"/>
            </xsl:for-each>
         </xsl:otherwise>
      </xsl:choose>
      <xsl:if test="position() != last()">
         <xsl:text> </xsl:text>
      </xsl:if>
   </xsl:for-each>
</xsl:template>



<!--
   This template builds the command line arguments that export either XMI or IDL2 or both.

   @param none
-->
<xsl:template name="build_export_command_line_args">
   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="noNeedToExportXmi">
      <xsl:call-template name="has_only_user_factories"/>
   </xsl:variable>
   <xsl:variable name="moduleName">
      <xsl:call-template name="get_module_to_export"/>
   </xsl:variable>
   <xsl:variable name="IDL2Name">
      <xsl:call-template name="get_idl2_filename"/>
   </xsl:variable>
   <xsl:variable name="idlSearchDirs">
      <xsl:variable name="fetchedIdlSearchDirs">
         <xsl:call-template name="get_idl_dirs">
            <xsl:with-param name="_searchDirs" select="//idl-search-dir"/>
         </xsl:call-template>
      </xsl:variable>
      <xsl:choose>
         <xsl:when test="string-length($CDMWIDL) > 0">
            <xsl:value-of select="concat('--I', $CDMWIDL, ' ', $fetchedIdlSearchDirs)"/>
         </xsl:when>
         <xsl:otherwise>
            <xsl:value-of select="$fetchedIdlSearchDirs"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>
   <xsl:variable name="hasOnlyFactoriesDefs">
      <xsl:call-template name="has_only_factories_defs"/>
   </xsl:variable>
   <xsl:variable name="requiresComponentsIDL">
      <xsl:call-template name="includeComponentsIDL"/>
   </xsl:variable>
   <xsl:variable name="requiresDataStore">
      <xsl:call-template name="usedDataStore"/>
   </xsl:variable>

   <!--
      Build the command line.
   -->
   <xsl:text></xsl:text><xsl:value-of select="concat('--obj=', $moduleName, ' --tmp=', $TMPDIR, ' ')"/>
   <xsl:if test="$hasOnlyFactoriesDefs = 'false' or $requiresDataStore = 'true'">
      <xsl:if test="($noNeedToExportXmi = 'false' or $requiresDataStore = 'true' )and not($ONLY = 'IDL2' or $ONLY = 'idl2')">
         <xsl:text></xsl:text><xsl:value-of select="concat('--xmi=', $XMIFILE, ' ')"/>
      </xsl:if>
   </xsl:if>
   <xsl:if test="$requiresComponentsIDL = 'true' and not($ONLY = 'XMI' or $ONLY = 'xmi')">
      <xsl:text></xsl:text><xsl:value-of select="concat('--idl=', $TMPDIR, '/', $IDL2Name, ' ')"/>
   </xsl:if>
   <xsl:if test="$requiresComponentsIDL = 'false'">
      <xsl:text>--noccm </xsl:text>
   </xsl:if>
   <xsl:if test="$requiresDataStore = 'true'">
      <xsl:text>--requiresdatastore </xsl:text>
   </xsl:if>

   <xsl:text> </xsl:text><xsl:value-of select="$idlSearchDirs"/>
</xsl:template> <!-- end of template build_export_command_line_args -->


<!--
   This template returns true() if datastore is used 

   @param none
-->
<xsl:template name="usedDataStore">
   <xsl:variable name="datastoreTypeNodes" select="/cdmw/datastore-types/datastore-type"/>
   <xsl:choose>
      <xsl:when test="boolean($datastoreTypeNodes)">
         <xsl:value-of select="true()"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="false()"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template usedDataStore -->


<!--
   This template returns true() if CORBA CCM is involved.

   @param none
-->
<xsl:template name="includeComponentsIDL">
   <!--
      References to some nodes.
   -->
   <xsl:variable name="frameworkNodes" select="/cdmw/programs/program/services/lifecycle/framework"/>
   <xsl:variable name="lifecycleFrameworkNodes" select="/cdmw/lifecycle-frameworks/lifecycle-framework"/>

   <xsl:choose>
      <!--
         Case 1 : no lifecycle-frameworks nor programs have been defined.
      -->
      <xsl:when test="not(boolean($frameworkNodes)) and not(boolean($lifecycleFrameworkNodes))">
         <xsl:variable name="containsHomeFactories">
            <xsl:call-template name="must_generate_idl2"/>
         </xsl:variable>
         <xsl:choose>
            <xsl:when test="$containsHomeFactories = 'true'">
               <xsl:value-of select="true()"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="false()"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <!--
         Case 2 : programs do not embed any frameworks and then all the factories
         are going to be generated (default behavior of the code generator). 
         Check if any of the factory requires CCM, if so then return true().
      -->
      <xsl:when test="not(boolean($frameworkNodes))">
         <xsl:variable name="lifecycleFrameworksRequireCCM">
            <xsl:for-each select="$lifecycleFrameworkNodes">
               <xsl:choose>
                  <xsl:when test="boolean(home-impl)">
                     <xsl:value-of select="true()"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="false()"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:for-each>
         </xsl:variable>
         <xsl:choose>
            <xsl:when test="contains($lifecycleFrameworksRequireCCM, 'true')">
               <xsl:value-of select="true()"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="false()"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <!--
         Case 3 : programs embed framework(s) : check only the embedded frameworks.
      -->
      <xsl:otherwise>
         <xsl:variable name="frameworksRequireCCM">
            <xsl:for-each select="$frameworkNodes">
               <xsl:variable name="frameworkNode" select="."/>
               <xsl:variable name="lifecycleFrameworkNode" select="$lifecycleFrameworkNodes[@id = $frameworkNode/@idref]"/>
               <xsl:choose>
                  <xsl:when test="$lifecycleFrameworkNode/home-impl">
                     <xsl:value-of select="true()"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="false()"/>
                  </xsl:otherwise>
               </xsl:choose> 
            </xsl:for-each>
         </xsl:variable>
         <xsl:choose>
            <xsl:when test="contains($frameworksRequireCCM, 'true')">
               <xsl:value-of select="true()"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="false()"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template>



</xsl:stylesheet>


