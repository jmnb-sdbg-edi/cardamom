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
   This template converts CORBA IDL types into ADA types.

   @param _paramKind    The parameter kind [in (default), out, inout, return]
   @param _stereotype   The CORBA stereotype.
   @param _scopedName   The scoped name.
-->

<xsl:template name="mapToADA">
   <xsl:param name="_paramKind" select="'in'"/>
   <xsl:param name="_stereotype"/>
   <xsl:param name="_scopedName"/>

   <xsl:variable name="kind" select="translate($_paramKind, $ucase, $lcase)"/>

   <xsl:choose>
      <!--
         IN
      -->
      <xsl:when test="$kind = 'in'">
         <xsl:choose>
            <xsl:when test="$_stereotype = 'CORBAshort'">
               <xsl:value-of select="'CORBA.Short'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlong'">
               <xsl:value-of select="'CORBA.Long'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlonglong'">
               <xsl:value-of select="'CORBA.Long_Long'"/>
            </xsl:when>
             <xsl:when test="$_stereotype = 'CORBAunsignedshort'">
               <xsl:value-of select="'CORBA.Unsigned_Short'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlong'">
               <xsl:value-of select="'CORBA.Unsigned_Long'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlonglong'">
               <xsl:value-of select="'CORBA.Unsigned_Long_Long'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAfloat'">
               <xsl:value-of select="'CORBA.Float'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAdouble'">
               <xsl:value-of select="'CORBA.Double'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlongdouble'">
               <xsl:value-of select="'CORBA.Long_Double'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAchar'">
               <xsl:value-of select="'CORBA.Char'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAwchar'">
               <xsl:value-of select="'CORBA.Wchar'"/>
            </xsl:when>
             <xsl:when test="$_stereotype = 'CORBAboolean'">
               <xsl:value-of select="'CORBA.Boolean'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAoctet'">
               <xsl:value-of select="'CORBA.Octet'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAstring'">
               <xsl:value-of select="'CORBA.String'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAany'">
               <xsl:value-of select="Any"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAArray'">
               <!-- TODO -->
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAEnum'">
               <!-- TODO -->
            </xsl:when>
            <xsl:otherwise>
               <xsl:value-of select="$_scopedName"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <!--
         OUT
      -->
      <xsl:when test="$kind = 'out'">
      </xsl:when>
      <!--
         INOUT
      -->
      <xsl:when test="$kind = 'inout'">
      </xsl:when>
      <!--
         RETURN
      -->
      <xsl:when test="$kind = 'return'">
         <xsl:value-of select="concat($_scopedName, $adaSep, 'Ref')"/>
      </xsl:when>
   </xsl:choose>
</xsl:template> <!-- end of template mapToADA -->



</xsl:stylesheet>


