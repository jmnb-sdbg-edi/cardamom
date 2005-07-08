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
   This template converts CORBA IDL types into JAVA types.

   @param _stereotype   The CORBA stereotype.
   @param _xmiId        Reference to the IDL element defined in the XMI file.
-->

<xsl:template name="mapToJAVA">
   <xsl:param name="_stereotype" select="'CORBAInterface'"/>
   <xsl:param name="_paramKind" select="'in'"/>
   <xsl:param name="_xmiId"/>
   
   <xsl:variable name="_scopedName">
      <!-- beware of anonymous classes ! -->
      <xsl:if test="boolean(key('classById', $_xmiId)/@name)">
         <xsl:call-template name="getScopedName">
            <xsl:with-param name="_name" select="key('classById', $_xmiId)/@name"/>
            <xsl:with-param name="_lang" select="'java'"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:variable>

   <xsl:variable name="kind" select="translate($_paramKind, $ucase, $lcase)"/>

   <xsl:choose>
      <!--
         IN.
      -->
      <xsl:when test="$kind = 'in'">
         <xsl:choose>
	    <xsl:when test="$_stereotype='CORBAshort'">
	       <xsl:value-of select="'short'"/>
            </xsl:when>
	    <xsl:when test="$_stereotype='CORBAlong'">
	       <xsl:value-of select="'int'"/>
	    </xsl:when>
            <xsl:when test="$_stereotype='CORBAlonglong'">
            <xsl:value-of select="'long'"/>
	       </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedshort'">
               <xsl:value-of select="'short'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedlong'">
               <xsl:value-of select="'int'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedlonglong'">
               <xsl:value-of select="'long'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAfloat'">
               <xsl:value-of select="'float'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAdouble'">
               <xsl:value-of select="'double'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAlongdouble'">
               <xsl:message>ERROR : CORBAlongdouble is not supported at this time.</xsl:message>
               <xsl:message>(Ref : JAVA Language Mapping Specification (99-07-53), paragraph 1.4.1.1)</xsl:message>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAchar'">
               <xsl:value-of select="'char'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAwchar'">
               <xsl:value-of select="'char'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAboolean'">
               <xsl:value-of select="'boolean'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAoctet'">
               <xsl:value-of select="'byte'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAstring'">
               <xsl:value-of select="'String'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAwstring'">
               <xsl:value-of select="'String'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAfixed'">
               <xsl:value-of select="'java.math.BigDecimal'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBASequence'">
               <xsl:call-template name="getJavaScopedTypeName">
                  <xsl:with-param name="_xmiId" select="$_xmiId"/>
               </xsl:call-template>         
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAArray'">
               <xsl:call-template name="getJavaScopedTypeName">
                  <xsl:with-param name="_xmiId" select="$_xmiId"/>
               </xsl:call-template>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAany'">
               <xsl:value-of select="'org.omg.CORBA.Any'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAEnum'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAUnion'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAStruct'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAValue'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAInterface'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='void'">
               <xsl:value-of select="'void'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CCMComponent'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CCMEvent'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:message>WARNING : stereotype <xsl:value-of select="$_stereotype"/> not supported.</xsl:message>
               <xsl:value-of select="$_scopedName"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <!--
         OUT.
      -->
      <xsl:when test="$kind = 'out'">
         <xsl:choose>
	    <xsl:when test="$_stereotype='CORBAshort'">
	       <xsl:value-of select="'org.omg.CORBA.ShortHolder'"/>
            </xsl:when>
	    <xsl:when test="$_stereotype='CORBAlong'">
	       <xsl:value-of select="'org.omg.CORBA.IntHolder'"/>
	    </xsl:when>
            <xsl:when test="$_stereotype='CORBAlonglong'">
            <xsl:value-of select="'org.omg.CORBA.LongHolder'"/>
	       </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedshort'">
               <xsl:value-of select="'org.omg.CORBA.ShortHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedlong'">
               <xsl:value-of select="'org.omg.CORBA.IntHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedlonglong'">
               <xsl:value-of select="'org.omg.CORBA.LongHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAfloat'">
               <xsl:value-of select="'org.omg.CORBA.FloatHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAdouble'">
               <xsl:value-of select="'org.omg.CORBA.DoubleHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAlongdouble'">
               <xsl:message>ERROR : CORBAlongdouble is not supported at this time.</xsl:message>
               <xsl:message>(Ref : JAVA Language Mapping Specification (99-07-53), paragraph 1.4.1.1)</xsl:message>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAchar'">
               <xsl:value-of select="'org.omg.CORBA.CharHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAwchar'">
               <xsl:value-of select="'org.omg.CORBA.CharHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAboolean'">
               <xsl:value-of select="'org.omg.CORBA.BooleanHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAoctet'">
               <xsl:value-of select="'org.omg.CORBA.ByteHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAstring'">
               <xsl:value-of select="'org.omg.CORBA.StringHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAwstring'">
               <xsl:value-of select="'org.omg.CORBA.StringHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAfixed'">
               <xsl:value-of select="'java.math.BigDecimal'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBASequence'">
                <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAArray'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAany'">
               <xsl:value-of select="'org.omg.CORBA.AnyHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAEnum'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAUnion'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAStruct'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAValue'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAInterface'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='void'">
               <xsl:value-of select="'void'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CCMComponent'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CCMEvent'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:message>WARNING : stereotype <xsl:value-of select="$_stereotype"/> not supported.</xsl:message>
               <xsl:value-of select="$_scopedName"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <!--
         INOUT.
      -->
      <xsl:when test="$kind = 'inout'">
         <xsl:choose>
	    <xsl:when test="$_stereotype='CORBAshort'">
	       <xsl:value-of select="'org.omg.CORBA.ShortHolder'"/>
            </xsl:when>
	    <xsl:when test="$_stereotype='CORBAlong'">
	       <xsl:value-of select="'org.omg.CORBA.IntHolder'"/>
	    </xsl:when>
            <xsl:when test="$_stereotype='CORBAlonglong'">
            <xsl:value-of select="'org.omg.CORBA.LongHolder'"/>
	       </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedshort'">
               <xsl:value-of select="'org.omg.CORBA.ShortHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedlong'">
               <xsl:value-of select="'org.omg.CORBA.IntHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedlonglong'">
               <xsl:value-of select="'org.omg.CORBA.LongHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAfloat'">
               <xsl:value-of select="'org.omg.CORBA.FloatHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAdouble'">
               <xsl:value-of select="'org.omg.CORBA.DoubleHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAlongdouble'">
               <xsl:message>ERROR : CORBAlongdouble is not supported at this time.</xsl:message>
               <xsl:message>(Ref : JAVA Language Mapping Specification (99-07-53), paragraph 1.4.1.1)</xsl:message>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAchar'">
               <xsl:value-of select="'org.omg.CORBA.CharHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAwchar'">
               <xsl:value-of select="'org.omg.CORBA.CharHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAboolean'">
               <xsl:value-of select="'org.omg.CORBA.BooleanHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAoctet'">
               <xsl:value-of select="'org.omg.CORBA.ByteHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAstring'">
               <xsl:value-of select="'org.omg.CORBA.StringHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAwstring'">
               <xsl:value-of select="'org.omg.CORBA.StringHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAfixed'">
               <xsl:value-of select="'java.math.BigDecimal'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBASequence'">
                <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAArray'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAany'">
               <xsl:value-of select="'org.omg.CORBA.AnyHolder'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAEnum'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAUnion'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAStruct'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAValue'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAInterface'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='void'">
               <xsl:value-of select="'void'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CCMComponent'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CCMEvent'">
               <xsl:value-of select="concat($_scopedName, 'Holder')"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:message>WARNING : stereotype <xsl:value-of select="$_stereotype"/> not supported.</xsl:message>
               <xsl:value-of select="$_scopedName"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
      <!--
         RETURN.
      -->
      <xsl:when test="$kind = 'return'">
         <xsl:choose>
	    <xsl:when test="$_stereotype='CORBAshort'">
	       <xsl:value-of select="'short'"/>
            </xsl:when>
	    <xsl:when test="$_stereotype='CORBAlong'">
	       <xsl:value-of select="'int'"/>
	    </xsl:when>
            <xsl:when test="$_stereotype='CORBAlonglong'">
            <xsl:value-of select="'long'"/>
	       </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedshort'">
               <xsl:value-of select="'short'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedlong'">
               <xsl:value-of select="'int'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAunsignedlonglong'">
               <xsl:value-of select="'long'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAfloat'">
               <xsl:value-of select="'float'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAdouble'">
               <xsl:value-of select="'double'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAlongdouble'">
               <xsl:message>ERROR : CORBAlongdouble is not supported at this time.</xsl:message>
               <xsl:message>(Ref : JAVA Language Mapping Specification (99-07-53), paragraph 1.4.1.1)</xsl:message>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAchar'">
               <xsl:value-of select="'char'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAwchar'">
               <xsl:value-of select="'char'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAboolean'">
               <xsl:value-of select="'boolean'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAoctet'">
               <xsl:value-of select="'byte'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAstring'">
               <xsl:value-of select="'String'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAwstring'">
               <xsl:value-of select="'String'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAfixed'">
               <xsl:value-of select="'java.math.BigDecimal'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBASequence'">
               <xsl:call-template name="getJavaScopedTypeName">
                  <xsl:with-param name="_xmiId" select="$_xmiId"/>
               </xsl:call-template>         
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAArray'">
               <xsl:call-template name="getJavaScopedTypeName">
                  <xsl:with-param name="_xmiId" select="$_xmiId"/>
               </xsl:call-template>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAany'">
               <xsl:value-of select="'org.omg.CORBA.Any'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAEnum'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAUnion'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAStruct'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAValue'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CORBAInterface'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='void'">
               <xsl:value-of select="'void'"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CCMComponent'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:when test="$_stereotype='CCMEvent'">
               <xsl:value-of select="$_scopedName"/>
            </xsl:when>
            <xsl:otherwise>
               <xsl:message>WARNING : stereotype <xsl:value-of select="$_stereotype"/> not supported.</xsl:message>
               <xsl:value-of select="$_scopedName"/>
            </xsl:otherwise>
         </xsl:choose>
      </xsl:when>
   </xsl:choose>

</xsl:template> <!-- end of template mapToJAVA -->



<!--
   Get the Java scoped type name recursively.

   @param _xmiId   Reference id.
-->

<xsl:template name="getJavaScopedTypeName">
   <xsl:param name="_xmiId"/>

   <xsl:variable name="assocNode" select="key('associationBySourceId', $_xmiId)"/>
   <xsl:variable name="indexCount" select="count($assocNode/UML:Association.connection/UML:AssociationEnd[1]/UML:AssociationEnd.qualifier/UML:Attribute)"/>
   <xsl:variable name="assocTypeId" select="$assocNode/UML:Association.connection/UML:AssociationEnd[2]/@type"/>

   <xsl:variable name="stereotypeName">
      <xsl:choose>
         <xsl:when test="boolean(key('classById', $assocTypeId))">
            <xsl:value-of select="key('stereotypeById', key('classById', $assocTypeId)/@stereotype)/@name"/>
         </xsl:when>
         <xsl:otherwise>                           
            <xsl:value-of select="key('stereotypeById', $assocTypeId)/@name"/>
         </xsl:otherwise>
      </xsl:choose>
   </xsl:variable>

   <xsl:variable name="scopedTypeName">
      <xsl:call-template name="mapToJAVA">
         <xsl:with-param name="_stereotype" select="$stereotypeName"/>
         <xsl:with-param name="_xmiId" select="$assocTypeId"/>
      </xsl:call-template>
   </xsl:variable>
   
   <xsl:call-template name="appendBrackets">
      <xsl:with-param name="_string" select="$scopedTypeName"/>
      <xsl:with-param name="_count" select="$indexCount"/>
   </xsl:call-template>

</xsl:template> <!-- end of template getScopedTypeName -->



<!--
   Add a pair of brackets at the end of a string.

   @param _string   The string.
   @param _count    The number of brackets pair to add.
-->

<xsl:template name="appendBrackets">
   <xsl:param name="_string"/>
   <xsl:param name="_count"/>
   
   <xsl:choose>
      <xsl:when test="$_count>1">
         <xsl:call-template name="appendBrackets">
            <xsl:with-param name="_string" select="concat($_string, '[]')"/>
            <xsl:with-param name="_count" select="$_count - 1"/>
         </xsl:call-template>
      </xsl:when>
      <xsl:when test="$_count=1">
         <xsl:value-of select="concat($_string, '[]')"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="$_string"/>
      </xsl:otherwise>
   </xsl:choose>

</xsl:template> <!-- end of template appendBrackets -->



<!--
   This template maps operation name that matches Java reserved words.

   @param _opName   The operation name.
-->

<xsl:template name="mapJavaReservedWords">
   <xsl:param name="_operationName"/>

   <xsl:choose>
      <xsl:when test="starts-with($_operationName, '_')">
         <xsl:value-of select="concat('J', $_operationName)"/>
      </xsl:when>
      <xsl:when test="$_operationName = 'and' or $_operationName = 'boolean' or $_operationName = 'break' or $_operationName = 'case' or $_operationName = 'catch' or $_operationName = 'char' or $_operationName = 'class' or $_operationName = 'continue' or $_operationName = 'do' or $_operationName = 'double' or $_operationName = 'else' or $_operationName = 'false' or $_operationName = 'float' or $_operationName = 'for' or $_operationName = 'if' or $_operationName = 'int' or $_operationName = 'long' or $_operationName = 'new' or $_operationName = 'not' or $_operationName = 'not_eq' or $_operationName = 'operator' or $_operationName = 'or' or $_operationName = 'private' or $_operationName = 'protected' or $_operationName = 'public' or $_operationName = 'return' or $_operationName = 'short' or $_operationName = 'static' or $_operationName = 'switch' or $_operationName = 'this' or $_operationName = 'throw' or $_operationName = 'true' or $_operationName = 'try' or $_operationName = 'void' or $_operationName = 'volatile' or $_operationName = 'while'">
         <xsl:value-of select="concat('_', $_operationName)"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="$_operationName"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template mapJavaReservedWords -->



</xsl:stylesheet>


