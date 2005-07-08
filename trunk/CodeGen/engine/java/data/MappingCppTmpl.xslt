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
   This template converts CORBA IDL types into C++ types.

   @param _paramKind    The parameter kind [in (default), out, inout, return]
   @param _stereotype   The CORBA stereotype.
   @param _scopedName   The scoped name.
   @param _asObjRefVar  If true, returns the object reference variable form instead.
   @param _isVariable   If true, the type is a variable size type.
-->
<xsl:template name="mapToCPP">
   <xsl:param name="_paramKind" select="'in'"/>
   <xsl:param name="_stereotype" select="'CORBAInterface'"/>
   <xsl:param name="_scopedName"/>
   <xsl:param name="_asObjRefVar" select="false()"/>
   <xsl:param name="_isVariable" select="false()"/>

   <xsl:variable name="kind" select="translate($_paramKind, $ucase, $lcase)"/>

   <xsl:choose>
      <!--
         IN.
      -->
      <xsl:when test="$kind = 'in'">
         <xsl:choose>
            <xsl:when test="$_stereotype = 'CORBAshort'">
               <xsl:value-of select="'CORBA::Short'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlong'">
               <xsl:value-of select="'CORBA::Long'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlonglong'">
               <xsl:value-of select="'CORBA::LongLong'"/>
            </xsl:when>
             <xsl:when test="$_stereotype = 'CORBAunsignedshort'">
               <xsl:value-of select="'CORBA::UShort'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlong'">
               <xsl:value-of select="'CORBA::ULong'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlonglong'">
               <xsl:value-of select="'CORBA::ULongLong'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAfloat'">
               <xsl:value-of select="'CORBA::Float'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAdouble'">
               <xsl:value-of select="'CORBA::Double'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlongdouble'">
               <xsl:value-of select="'CORBA::LongDouble'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAboolean'">
               <xsl:value-of select="'CORBA::Boolean'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAchar'">
               <xsl:value-of select="'CORBA::Char'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAwchar'">
               <xsl:value-of select="'CORBA::WChar'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAoctet'">
               <xsl:value-of select="'CORBA::Octet'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAEnum'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAInterface' or $_stereotype = 'CCMComponent' or $_stereotype = 'CCMHome'">
               <xsl:value-of select="concat($_scopedName, '_ptr')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAStruct' or $_stereotype = 'CORBAUnion' or $_stereotype = 'CORBASequence'">
               <xsl:value-of select="concat('const ', $_scopedName, '&amp;')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAstring'">
               <xsl:value-of select="'const char*'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAwstring'">
               <xsl:value-of select="'const CORBA::WChar*'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAArray'">
               <xsl:value-of select="concat('const ', $_scopedName)"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAany'">
               <xsl:value-of select="concat('const CORBA::Any', '&amp;')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAFixed'">
               <xsl:value-of select="concat('const ', $_scopedName, '&amp;')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAValue'">
               <xsl:value-of select="concat($_scopedName, '*')"/>
            </xsl:when>
         </xsl:choose>
      </xsl:when>
      <!--
         OUT.
      -->
      <xsl:when test="$kind = 'out'">
         <xsl:choose>
            <xsl:when test="$_stereotype = 'CORBAshort'">
               <xsl:value-of select="concat('CORBA::Short', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlong'">
               <xsl:value-of select="concat('CORBA::Long', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlonglong'">
               <xsl:value-of select="concat('CORBA::LongLong', '_out')"/>
            </xsl:when>
             <xsl:when test="$_stereotype = 'CORBAunsignedshort'">
               <xsl:value-of select="concat('CORBA::UShort', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlong'">
               <xsl:value-of select="concat('CORBA::ULong', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlonglong'">
               <xsl:value-of select="concat('CORBA::ULongLong', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAfloat'">
               <xsl:value-of select="concat('CORBA::Float', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAdouble'">
               <xsl:value-of select="concat('CORBA::Double', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlongdouble'">
               <xsl:value-of select="concat('CORBA::LongDouble', '_out')"/>
            </xsl:when>
             <xsl:when test="$_stereotype = 'CORBAboolean'">
               <xsl:value-of select="concat('CORBA::Boolean', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAchar'">
               <xsl:value-of select="concat('CORBA::Char', '_out')"/>
            </xsl:when>
             <xsl:when test="$_stereotype = 'CORBAwchar'">
               <xsl:value-of select="concat('CORBA::WChar', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAoctet'">
               <xsl:value-of select="concat('CORBA::Octet', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAEnum'">
               <xsl:value-of select="concat($_scopedName, '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAInterface' or $_stereotype = 'CCMComponent' or $_stereotype = 'CCMHome'">
               <xsl:value-of select="concat($_scopedName, '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAStruct' or $_stereotype = 'CORBAUnion'">
	       <xsl:value-of select="concat($_scopedName, '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBASequence'">
               <xsl:value-of select="concat($_scopedName, '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAstring'">
               <xsl:value-of select="concat('CORBA::String', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAwstring'">
               <xsl:value-of select="concat('CORBA::WString', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAArray'">
               <xsl:value-of select="concat($_scopedName, '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAany'">
               <xsl:value-of select="concat('CORBA::Any', '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAFixed'">
               <xsl:value-of select="concat($_scopedName, '_out')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAValue'">
               <xsl:value-of select="concat($_scopedName, '_out')"/>
            </xsl:when>
         </xsl:choose>
      </xsl:when>
      <!--
         INOUT
      -->
      <xsl:when test="$kind = 'inout'">
         <xsl:choose>
            <xsl:when test="$_stereotype = 'CORBAshort'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::Short', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::Short', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlong'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::Long', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::Long', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlonglong'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::LongLong', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::LongLong', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedshort'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::UShort', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::UShort', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlong'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::ULong', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::ULong', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlonglong'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::ULongLong', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::ULongLong', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAfloat'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::Float', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::Float', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAdouble'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::Double', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::Double', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlongdouble'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::LongDouble', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::LongDouble', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAboolean'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::Boolean', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::Boolean', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAchar'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::Char', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::Char', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAwchar'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::WChar', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::WChar', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAoctet'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat('const CORBA::Octet', '&amp;')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::Octet', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAEnum'">
               <xsl:value-of select="concat($_scopedName, '&amp;')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAInterface' or $_stereotype = 'CCMComponent' or $_stereotype = 'CCMHome'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat($_scopedName, '_var')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat($_scopedName, '_ptr', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAStruct' or $_stereotype = 'CORBAUnion'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat($_scopedName, '_var')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat($_scopedName, '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBASequence'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat($_scopedName, '_var')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat($_scopedName, '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAstring'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="'CORBA::String_var'"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('char*', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAwstring'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="'CORBA::WString_var'"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat('CORBA::WChar*', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAArray'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat($_scopedName, '_var')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="$_scopedName"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAany'">
               <xsl:value-of select="concat('CORBA::Any', '&amp;')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAFixed'">
               <xsl:value-of select="concat($_scopedName, '&amp;')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAValue'">
               <xsl:choose>
                  <xsl:when test="$_asObjRefVar = 'true'">
                     <xsl:value-of select="concat($_scopedName, '_var')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="concat($_scopedName, '*', '&amp;')"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
         </xsl:choose>
      </xsl:when>
      <!--
         RETURN
      -->
      <xsl:when test="$kind = 'return'">
         <xsl:choose>
            <xsl:when test="$_stereotype = 'void'">
               <xsl:value-of select="'void'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAshort'">
               <xsl:value-of select="'CORBA::Short'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlong'">
               <xsl:value-of select="'CORBA::Long'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlonglong'">
               <xsl:value-of select="'CORBA::LongLong'"/>
            </xsl:when>
             <xsl:when test="$_stereotype = 'CORBAunsignedshort'">
               <xsl:value-of select="'CORBA::UShort'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlong'">
               <xsl:value-of select="'CORBA::ULong'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAunsignedlonglong'">
               <xsl:value-of select="'CORBA::ULongLong'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAfloat'">
               <xsl:value-of select="'CORBA::Float'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAdouble'">
               <xsl:value-of select="'CORBA::Double'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAlongdouble'">
               <xsl:value-of select="'CORBA::LongDouble'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAboolean'">
               <xsl:value-of select="'CORBA::Boolean'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAchar'">
               <xsl:value-of select="'CORBA::Char'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAwchar'">
               <xsl:value-of select="'CORBA::WChar'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAoctet'">
               <xsl:value-of select="'CORBA::Octet'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAEnum'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAInterface' or $_stereotype = 'CCMComponent' or $_stereotype = 'CCMHome'">
               <xsl:value-of select="concat($_scopedName, '_ptr')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAStruct' or $_stereotype = 'CORBAUnion'">
               <xsl:choose>
                  <xsl:when test="$_isVariable = 'true'">
                     <xsl:value-of select="concat($_scopedName, '*')"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <xsl:value-of select="$_scopedName"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBASequence'">
               <xsl:value-of select="concat($_scopedName, '*')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAstring'">
               <xsl:value-of select="'char*'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAwstring'">
               <xsl:value-of select="'CORBA::WChar*'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAArray'">
               <xsl:value-of select="concat($_scopedName, '_slice*')"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAany'">
               <xsl:value-of select="'CORBA::Any*'"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAFixed'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype = 'CORBAValue'">
               <xsl:value-of select="concat($_scopedName, '*')"/>
            </xsl:when>
         </xsl:choose>
      </xsl:when>
   </xsl:choose>
</xsl:template> <!-- end of template mapToCPP -->



<!--
   This template maps operation name that matches C++ reserved words.

   @param _opName   The operation name.
-->
<xsl:template name="mapCppReservedWords">
   <xsl:param name="_operationName"/>

   <xsl:choose>
      <xsl:when test="$_operationName = 'and' or $_operationName = 'and_eq' or $_operationName = 'asm' or $_operationName = 'auto' or $_operationName = 'bitand' or $_operationName = 'bitor' or
                      $_operationName = 'bool' or $_operationName = 'break' or $_operationName = 'case' or $_operationName = 'catch' or $_operationName = 'char' or $_operationName = 'class' or
                      $_operationName = 'compl' or $_operationName = 'const' or $_operationName = 'const_cast' or $_operationName = 'continue' or $_operationName = 'default' or $_operationName = 'delete' or
                      $_operationName = 'do' or $_operationName = 'double' or $_operationName = 'dynamic_cast' or $_operationName = 'else' or $_operationName = 'enum' or $_operationName = 'explicit' or
                      $_operationName = 'export' or $_operationName = 'extern' or $_operationName = 'false' or $_operationName = 'float' or $_operationName = 'for' or $_operationName = 'friend' or
                      $_operationName = 'goto' or $_operationName = 'if' or $_operationName = 'inline' or $_operationName = 'int' or $_operationName = 'long' or $_operationName = 'mutable' or
                      $_operationName = 'namespace' or $_operationName = 'new' or $_operationName = 'not' or $_operationName = 'not_eq' or $_operationName = 'operator' or $_operationName = 'or' or
                      $_operationName = 'or_eq' or $_operationName = 'private' or $_operationName = 'protected' or $_operationName = 'public' or $_operationName = 'register' or $_operationName = 'reinterpret_cast' or
                      $_operationName = 'return' or $_operationName = 'short' or $_operationName = 'signed' or $_operationName = 'sizeof' or $_operationName = 'static' or $_operationName = 'static_cast' or
                      $_operationName = 'struct' or $_operationName = 'switch' or $_operationName = 'template' or $_operationName = 'this' or $_operationName = 'throw' or $_operationName = 'true' or
                      $_operationName = 'try' or $_operationName = 'typedef' or $_operationName = 'typeid' or $_operationName = 'typename' or $_operationName = 'union' or $_operationName = 'unsigned' or
                      $_operationName = 'using' or $_operationName = 'virtual' or $_operationName = 'void' or $_operationName = 'volatile' or $_operationName = 'wchar_t' or $_operationName = 'while' or
                      $_operationName = 'xor' or $_operationName = 'xor_eq'">
         <xsl:value-of select="concat('_cxx_', $_operationName)"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="$_operationName"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template mapCppReservedWords -->



<!--
   This template determines whether the specified type is a fixed or variable size type.

   @param _typeId   The type xmi.id.
-->
<xsl:template name="isVariableSizeTypeCPP">
   <xsl:param name="_typeId"/>

   <xsl:variable name="searchResult">
      <!--
         Is it a CORBA primitive type ?
      -->
      <xsl:variable name="stereotypeNode" select="key('stereotypeById', $_typeId)"/>
      <xsl:choose>
         <!-- YES -->
         <xsl:when test="boolean($stereotypeNode)">
            <xsl:call-template name="isVariableSizeTypeCPP_sub">
               <xsl:with-param name="_stereotypeName" select="$stereotypeNode/@name"/>
            </xsl:call-template>
         </xsl:when>
         <!-- NO -->
         <xsl:otherwise>
            <xsl:variable name="node" select="key('classById', $_typeId)"/>
            <xsl:choose>
               <!--
                  CORBA alias.
               -->
               <xsl:when test="key('stereotypeById', $node/@stereotype)/@name = 'CORBATypedef'">
                  <xsl:call-template name="isVariableSizeTypeCPP">
                     <xsl:with-param name="_typeId">
                        <xsl:call-template name="aliasOriginalTypeId">
                           <xsl:with-param name="_generalization" select="$node/UML:GeneralizableElement.generalization/UML:Generalization/@xmi.idref"/>
                        </xsl:call-template>
                     </xsl:with-param>
                  </xsl:call-template>
               </xsl:when>
               <!--
                  CORBA struct/union.
               -->
               <xsl:when test="key('stereotypeById', $node/@stereotype)/@name = 'CORBAStruct' or
                               key('stereotypeById', $node/@stereotype)/@name = 'CORBAUnion'">
                  <xsl:for-each select="$node/UML:Namespace.ownedElement/UML:Attribute">
                     <xsl:call-template name="isVariableSizeTypeCPP">
                        <xsl:with-param name="_typeId" select="@type"/>
                     </xsl:call-template>
                  </xsl:for-each>
                  <xsl:for-each select="key('associationBySourceId', $node/@xmi.id)">
                     <xsl:call-template name="isVariableSizeTypeCPP">
                        <xsl:with-param name="_typeId" select="UML:Association.connection/UML:AssociationEnd[2]/@type"/>
                     </xsl:call-template>
                  </xsl:for-each>
               </xsl:when>
               <!--
                  CORBA array or CORBA anonymous array.
               -->
               <xsl:when test="key('stereotypeById', $node/@stereotype)/@name = 'CORBAArray' or
                               key('stereotypeById', $node/@stereotype)/@name = 'CORBAAnonymousArray'">
                  <xsl:call-template name="isVariableSizeTypeCPP">
                     <xsl:with-param name="_typeId" select="key('associationBySourceId', $node/@xmi.id)/UML:Association.connection/UML:AssociationEnd[2]/@type"/>
                  </xsl:call-template>
               </xsl:when>
               <!--
                  Others.
               -->
               <xsl:otherwise>
                  <xsl:call-template name="isVariableSizeTypeCPP_sub">
                     <xsl:with-param name="_stereotypeName" select="key('stereotypeById', $node/@stereotype)/@name"/>
                  </xsl:call-template>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>

   <xsl:choose>
      <xsl:when test="contains($searchResult, 'true')">
         <xsl:value-of select="true()"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="false()"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template isVariableSizeTypeCPP -->



<!--
   This template returns true if the CORBA type is a variable size type.

   @param _stereotypeName   The stereotype name.
-->
<xsl:template name="isVariableSizeTypeCPP_sub">
   <xsl:param name="_stereotypeName"/>

   <xsl:choose>
      <xsl:when test="$_stereotypeName = 'CORBAany' or
                      $_stereotypeName = 'CORBATypeCode' or
                      $_stereotypeName = 'CORBAPrincipal' or
                      $_stereotypeName = 'CORBAObject' or
                      $_stereotypeName = 'CORBAstring' or
                      $_stereotypeName = 'CORBAwstring' or
                      $_stereotypeName = 'CORBASequence' or
                      $_stereotypeName = 'CORBAException' or
                      $_stereotypeName = 'CORBAValue'">
         <xsl:value-of select="true()"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="false()"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template isVariableSizeTypeCPP_sub -->



</xsl:stylesheet>


