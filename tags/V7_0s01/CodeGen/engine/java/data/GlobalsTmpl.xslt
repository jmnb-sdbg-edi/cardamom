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



<!--
   Global variables.
-->



<!--
   This variable defines the name of the application.
-->
<xsl:variable name="appName" select="'CDMW Code Generator'"/>

<!--
   This variable defines the version of the application.
-->
<xsl:variable name="appVersion" select="'1.0'"/>

<!--
   This variable defines the namespace/package separator
   used in XML documents.
-->
<xsl:variable name="xmlSep" select="'.'"/>

<!--
   This variable defines the C++ namespace separator.
-->
<xsl:variable name="cppSep" select="'::'"/>

<!--
   This variable defines the ADA package separator.
-->
<xsl:variable name="adaSep" select="'.'"/>

<!--
   This variable defines the JAVA package separator.
-->
<xsl:variable name="javaSep" select="'.'"/>

<!--
   This variable defines the pragma prefix 
   used in CDMW IDL files.
-->
<xsl:variable name="idlPragmaPrefix" select="'thalesgroup.com'"/>

<!--
   This variable defines the line feed character.
-->
<xsl:variable name="lf">
   <xsl:choose>
      <xsl:when test="translate($OS, $ucase, $lcase) = 'dos'">
         <xsl:text>&#xD;&#xA;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
         <xsl:text>&#xA;</xsl:text>
      </xsl:otherwise>
   </xsl:choose>
</xsl:variable>

<!--
   This variable defines the file separator to use when building paths.
-->
<xsl:variable name="fileSep">
   <xsl:choose>
      <xsl:when test="translate($OS, $ucase, $lcase) = 'dos'">
         <xsl:value-of select="'\'"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="'/'"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:variable>

<!--
   This variable is a reference to the /cdmw node.
-->
<xsl:variable name="cdmwNode" select="document($CODEGEN_CONF)/cdmw"/>

<!--
   This variable is a reference to the /cdmw/factories/factory node(s).
-->
<xsl:variable name="factoryNodes" select="$cdmwNode/factories/factory"/>

<!--
   This variable is a reference to the /cdmw/lifecycle-frameworks child node(s).
-->
<xsl:variable name="lifecycleFrameworkNodes" select="$cdmwNode/lifecycle-frameworks/lifecycle-framework"/>

<!--
   This variable is a reference to the /cdmw/datastore-types child node(s).
-->
<xsl:variable name="DatastoreTypeNodes" select="$cdmwNode/datastore-types/datastore-type"/>

<!--
   This variable is the xmi.id value of the Cdmw::LifeCycle::Key.
-->
<xsl:variable name="keyNodeId" select="key('classByName', 'Key')/@xmi.id"/>

<!--
   This variable is the xmi.id value of the CdmwLifeCycle::AlreadyExistant.
-->
<xsl:variable name="alreadyExistentId" select="'CdmwLifeCycle::AlreadyExistant'"/>

<!--
   This variable is the xmi.id value of the CdmwLifeCycle::InvalidName.
-->
<xsl:variable name="invalidNameId" select="'CdmwLifeCycle::InvalidName'"/>

<!--
   This variable stores the IDL3 filename.
-->
<xsl:variable name="idl3Filename">
   <xsl:variable name="homeFactoryNode" select="$factoryNodes[@kind = 'HOME']"/>   
   <xsl:choose>
      <xsl:when test="boolean($homeFactoryNode/@idl-filename)">
         <xsl:value-of select="$homeFactoryNode/@idl-filename"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:variable name="interfaceName">
            <xsl:call-template name="getLastToken">
               <xsl:with-param name="_string" select="$homeFactoryNode/@idl-interface"/>
               <xsl:with-param name="_separator" select="$xmlSep"/>
            </xsl:call-template>
         </xsl:variable>
         <xsl:value-of select="concat($interfaceName, '.idl')"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:variable>


</xsl:stylesheet>


