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


<xsl:template name="isSimpleDataTypeForDataTraits">
   <xsl:param name="_dataType"/>
   <xsl:choose>
      <xsl:when test="$_dataType = 'boolean' or
                      $_dataType = 'short' or
                      $_dataType = 'unsigned short' or
                      $_dataType = 'long' or
                      $_dataType = 'unsigned long' or
                      $_dataType = 'long long' or
                      $_dataType = 'unsigned long long' or
                      $_dataType = 'long double' or
                      $_dataType = 'float' or
                      $_dataType = 'double' or
                      $_dataType = 'octet' or
                      $_dataType = 'string' or
                      $_dataType = 'char'">
         <xsl:value-of select="true()"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="false()"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template>

<xsl:template name="getSimpleDataTypeForDataTraits">
   <xsl:param name="_dataType"/>
   <xsl:choose>
      <xsl:when test="$_dataType='boolean'">
         <xsl:value-of select="'CORBA::Boolean'"/>
      </xsl:when>
      <xsl:when test="$_dataType='short'">
         <xsl:value-of select="'CORBA::Short'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'unsigned short'">
         <xsl:value-of select="'CORBA::UShort'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'long'">
         <xsl:value-of select="'CORBA::Long'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'unsigned long'">
         <xsl:value-of select="'CORBA::ULong'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'long long'">
         <xsl:value-of select="'CORBA::LongLong'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'unsigned long long'">
         <xsl:value-of select="'CORBA::ULongLong'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'float'">
         <xsl:value-of select="'CORBA::Float'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'double'">
         <xsl:value-of select="'CORBA::Double'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'long double'">
         <xsl:value-of select="'CORBA::LongDouble'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'octet'">
         <xsl:value-of select="'CORBA::Octet'"/>
      </xsl:when>
      <xsl:when test="$_dataType = 'string'">
         <xsl:value-of select="'std::string'"/>
      </xsl:when>
       <xsl:when test="$_dataType = 'char'">
         <xsl:value-of select="'CORBA::Char'"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="''"/>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template>

<xsl:template name="global_datastore_type_traits">
  <xsl:variable name="filename">
     <xsl:call-template name="buildOutputFilename">
        <xsl:with-param name="_userFilename" select="'GlobalDataStores'"/>
	<xsl:with-param name="_fileExtension" select="'.hpp'"/>
     </xsl:call-template>
  </xsl:variable>

   <!--
      Display a message to the user.
   -->
  <xsl:message>
     <xsl:value-of select="concat('Generating file ',$filename)"/>
  </xsl:message>

  <xsl:document method="text" href="{$filename}">
     <xsl:call-template name="docHeader">
        <xsl:with-param name="_lang" select="'cpp'"/>
     </xsl:call-template>

     <xsl:call-template name="global_datastore_type_traits.content"/>
  </xsl:document>
</xsl:template>

<xsl:template name="global_datastore_type_traits.content">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   <xsl:if test="boolean($DatastoreTypeNodes)">
      <xsl:if test="$_index &lt;= count($DatastoreTypeNodes)">
         <xsl:variable name="datastoreTypeNode" select="$DatastoreTypeNodes[$_index]"/>

	 <xsl:variable name="oidIsSimpleDataType">
	    <xsl:call-template name="isSimpleDataTypeForDataTraits">
	       <xsl:with-param name="_dataType" select="$datastoreTypeNode/@oid-type"/>
	    </xsl:call-template>
	 </xsl:variable>

	 <xsl:variable name="macroMustBeGeneForOID">
	    <xsl:choose>
	       <xsl:when test="$oidIsSimpleDataType='false'">
	          <xsl:variable name="oidXMIID">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@oid-type"/>
		        <xsl:with-param name="_attribute" select="'xmi.id'"/>
	             </xsl:call-template>
	          </xsl:variable>
	          <xsl:variable name="oidStereotype">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@oid-type"/>
		        <xsl:with-param name="_attribute" select="'stereotype'"/>
	             </xsl:call-template>
	          </xsl:variable>
	          <xsl:call-template name="mustGeneratedMacro">
	             <xsl:with-param name="_dataTypeID" select="$oidStereotype"/>
	          </xsl:call-template>
	       </xsl:when>
	       <xsl:otherwise>
	          <xsl:value-of select="false()"/>
	       </xsl:otherwise>
	    </xsl:choose>
	 </xsl:variable>

	 <xsl:variable name="dataIsSimpleDataType">
	    <xsl:call-template name="isSimpleDataTypeForDataTraits">
	       <xsl:with-param name="_dataType" select="$datastoreTypeNode/@data-type"/>
	    </xsl:call-template>
	 </xsl:variable>
	 <xsl:variable name="macroMustBeGeneForDATA">
	    <xsl:choose>
	       <xsl:when test="$dataIsSimpleDataType='false'">
 	          <xsl:variable name="dataXMIID">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@data-type"/>
		        <xsl:with-param name="_attribute" select="'xmi.id'"/>
	             </xsl:call-template>
	          </xsl:variable>
	          <xsl:variable name="dataStereotype">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@data-type"/>
		        <xsl:with-param name="_attribute" select="'stereotype'"/>
	             </xsl:call-template>
	          </xsl:variable>
	          <xsl:call-template name="mustGeneratedMacro">
	             <xsl:with-param name="_dataTypeID" select="$dataStereotype"/>
	          </xsl:call-template>
	       </xsl:when>
	       <xsl:otherwise>
	          <xsl:value-of select="false()"/>
	       </xsl:otherwise>
	    </xsl:choose>
	 </xsl:variable>


	 <xsl:if test="($macroMustBeGeneForOID='true') or ($macroMustBeGeneForDATA='true')">
            <!--
               Get the output filename.
            -->
            <xsl:variable name="pathPrefix">
	       <xsl:choose>
		  <xsl:when test="string-length($CSC_NAME) > 0">
		     <xsl:value-of select="$CSC_NAME"/>
		  </xsl:when>
		  <xsl:otherwise>
		     <xsl:text></xsl:text>
		  </xsl:otherwise>
	       </xsl:choose>
            </xsl:variable>

	    <xsl:variable name="filename">
	       <xsl:call-template name="buildOutputFilename">
	          <xsl:with-param name="_userFilename" select="$datastoreTypeNode/@filename"/>
		  <xsl:with-param name="_defaultFilename" select="concat($datastoreTypeNode/@typename,'TypeTraits')"/>
		  <xsl:with-param name="_fileExtension" select="'.hpp'"/>
		  <xsl:with-param name="_userDirectory">
		     <xsl:choose>
		        <xsl:when test="string-length($CSC_NAME) > 0">
		           <xsl:value-of select="$CSC_NAME"/>
		        </xsl:when>
		        <xsl:otherwise>
		           <xsl:text></xsl:text>
		        </xsl:otherwise>
	              </xsl:choose>
		  </xsl:with-param>
               </xsl:call-template>
            </xsl:variable>

            <xsl:text>#include "</xsl:text><xsl:value-of select="$filename"/>"<xsl:value-of select="$lf"/>
	 </xsl:if>
         <!--
            Proceed to the next item in idlNodes if any.
         -->
         <xsl:call-template name="global_datastore_type_traits.content">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template>





<!--
   Main - This template fetches the necessary data from inputs
   to generate a file.

   @param none
-->
<xsl:template name="datastore_type_traits">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>
   
   <xsl:if test="boolean($DatastoreTypeNodes)">
      <xsl:if test="$_index &lt;= count($DatastoreTypeNodes)">
         <xsl:variable name="datastoreTypeNode" select="$DatastoreTypeNodes[$_index]"/>

	 <xsl:variable name="oidIsSimpleDataType">
	    <xsl:call-template name="isSimpleDataTypeForDataTraits">
	       <xsl:with-param name="_dataType" select="$datastoreTypeNode/@oid-type"/>
	    </xsl:call-template>
	 </xsl:variable>

	 <xsl:variable name="macroMustBeGeneForOID">
	    <xsl:choose>
	       <xsl:when test="$oidIsSimpleDataType='false'">
	          <xsl:variable name="oidXMIID">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@oid-type"/>
		        <xsl:with-param name="_attribute" select="'xmi.id'"/>
	             </xsl:call-template>
	          </xsl:variable>
	          <xsl:variable name="oidStereotype">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@oid-type"/>
		        <xsl:with-param name="_attribute" select="'stereotype'"/>
	             </xsl:call-template>
	          </xsl:variable>
	          <xsl:call-template name="mustGeneratedMacro">
	             <xsl:with-param name="_dataTypeID" select="$oidStereotype"/>
	          </xsl:call-template>
	       </xsl:when>
	       <xsl:otherwise>
	          <xsl:value-of select="false()"/>
	       </xsl:otherwise>
	    </xsl:choose>
	 </xsl:variable>

	 <xsl:variable name="dataIsSimpleDataType">
	    <xsl:call-template name="isSimpleDataTypeForDataTraits">
	       <xsl:with-param name="_dataType" select="$datastoreTypeNode/@data-type"/>
	    </xsl:call-template>
	 </xsl:variable>
	 <xsl:variable name="macroMustBeGeneForDATA">
	    <xsl:choose>
	       <xsl:when test="$dataIsSimpleDataType='false'">
 	          <xsl:variable name="dataXMIID">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@data-type"/>
		        <xsl:with-param name="_attribute" select="'xmi.id'"/>
	             </xsl:call-template>
	          </xsl:variable>
	          <xsl:variable name="dataStereotype">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@data-type"/>
		        <xsl:with-param name="_attribute" select="'stereotype'"/>
	             </xsl:call-template>
	          </xsl:variable>
	          <xsl:call-template name="mustGeneratedMacro">
	             <xsl:with-param name="_dataTypeID" select="$dataStereotype"/>
	          </xsl:call-template>
	       </xsl:when>
	       <xsl:otherwise>
	          <xsl:value-of select="false()"/>
	       </xsl:otherwise>
	    </xsl:choose>
	 </xsl:variable>

	 <xsl:if test="($macroMustBeGeneForOID='true') or ($macroMustBeGeneForDATA='true')">
            <!--
               Get the output filename.
            -->
	    <xsl:variable name="filename">
	       <xsl:call-template name="buildOutputFilename">
	          <xsl:with-param name="_userFilename" select="$datastoreTypeNode/@filename"/>
		  <xsl:with-param name="_defaultFilename" select="concat($datastoreTypeNode/@typename,'TypeTraits')"/>
		  <xsl:with-param name="_fileExtension" select="'.hpp'"/>
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


               <xsl:variable name="_idlfilename" select="$datastoreTypeNode/@idl-filename"/>
	       <xsl:variable name="_string" select="substring-before($_idlfilename,'.idl')"/>

               <xsl:variable name="pathPrefix">
	          <xsl:choose>
		     <xsl:when test="string-length($CSC_NAME) > 0">
		        <xsl:value-of select="concat($CSC_NAME, $fileSep)"/>
			</xsl:when>
			<xsl:otherwise>
			   <xsl:text></xsl:text>
			</xsl:otherwise>
	          </xsl:choose>
               </xsl:variable>

	       <xsl:text>#include "</xsl:text><xsl:value-of select="concat($pathPrefix, $_string)"/>.stub.hpp"<xsl:value-of select="$lf"/>

               <!--
                  Insert the #ifndef #define instructions.
               -->
	       <xsl:if test="$macroMustBeGeneForOID='true'">
	          <xsl:variable name="oidXMIID">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@oid-type"/>
		        <xsl:with-param name="_attribute" select="'xmi.id'"/>
	             </xsl:call-template>
	          </xsl:variable>
		  <xsl:variable name="oidIsVariableSize">
		     <xsl:call-template name="isVariableSizeTypeCPP">
                        <xsl:with-param name="_typeId" select="$oidXMIID"/>
                     </xsl:call-template>
		  </xsl:variable>

	          <xsl:variable name="oidtodefine">
                     <xsl:call-template name="replaceCharsInString">
                        <xsl:with-param name="_stringIn" select="$datastoreTypeNode/@oid-type"/>
			<xsl:with-param name="_charsIn" select="'.'"/>
			<xsl:with-param name="_charsOut" select="'_'"/>
                     </xsl:call-template>
                  </xsl:variable>
	          <xsl:variable name="cppoidtodefine">
                     <xsl:call-template name="replaceCharsInString">
                        <xsl:with-param name="_stringIn" select="$datastoreTypeNode/@oid-type"/>
			<xsl:with-param name="_charsIn" select="'.'"/>
			<xsl:with-param name="_charsOut" select="'::'"/>
                     </xsl:call-template>
                  </xsl:variable>
	          <xsl:variable name="hppoidtodefine">
                     <xsl:call-template name="replaceCharsInString">
                        <xsl:with-param name="_stringIn" select="$datastoreTypeNode/@oid-type"/>
			<xsl:with-param name="_charsIn" select="'.'"/>
			<xsl:with-param name="_charsOut" select="''"/>
                     </xsl:call-template>
                  </xsl:variable>		 

	          <xsl:variable name="defineOidString" select="concat('__DECLARE_', translate($oidtodefine, $lcase, $ucase), '_TYPE_TRAITS__')"/>
      
                  <xsl:value-of select="concat($lf, '#ifndef ', $defineOidString, $lf)"/>
		  <xsl:value-of select="concat('#define ', $defineOidString, $lf)"/>
		  <xsl:text>#include "</xsl:text><xsl:value-of select="concat($pathPrefix,'StdLess', $hppoidtodefine)"/>.hpp"<xsl:value-of select="$lf"/>
	          <xsl:variable name="oidStereotype">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@oid-type"/>
		        <xsl:with-param name="_attribute" select="'stereotype'"/>
	             </xsl:call-template>
	          </xsl:variable>
		  <xsl:variable name="oidStereotypeForDataType" select="key('stereotypeById', $oidStereotype)"/>
		  <xsl:choose>
		     <xsl:when test="$oidStereotypeForDataType/@name = 'CORBAEnum'">
		        <xsl:value-of select="concat('DEFINE_SIMPLE_DATA_TRAITS(', $oidtodefine,');', $lf)"/>		        
		     </xsl:when>
		     <xsl:when test="$oidStereotypeForDataType/@name = 'CORBASequence'">
		        <xsl:value-of select="concat('DEFINE_VARIABLE_LENGTH_DATA_TRAITS(', $cppoidtodefine,');', $lf)"/>		        
		     </xsl:when>
		     <xsl:when test="$oidStereotypeForDataType/@name = 'CORBAUnion' or
                                     $oidStereotypeForDataType/@name = 'CORBAStruct'">
		        <xsl:choose>
			   <xsl:when test="$oidIsVariableSize='true'">
		              <xsl:value-of select="concat('DEFINE_VARIABLE_LENGTH_DATA_TRAITS(', $cppoidtodefine,');', $lf)"/>
			   </xsl:when>
			   <xsl:otherwise>
		              <xsl:value-of select="concat('DEFINE_FIXED_LENGTH_DATA_TRAITS(', $cppoidtodefine,');', $lf)"/>
			   </xsl:otherwise>
			</xsl:choose>
	             </xsl:when>
		     <xsl:when test="$oidStereotypeForDataType/@name = 'CORBAArray'">
		        <xsl:choose>
			   <xsl:when test="$oidIsVariableSize='true'">
		              <xsl:value-of select="concat('DEFINE_VARIABLE_LENGTH_ARRAY_TRAITS(', $cppoidtodefine,');', $lf)"/>
			   </xsl:when>
			   <xsl:otherwise>
		              <xsl:value-of select="concat('DEFINE_FIXED_LENGTH_ARRAY_TRAITS(', $cppoidtodefine,');', $lf)"/>
			   </xsl:otherwise>
			</xsl:choose>
		     </xsl:when>
		     <xsl:otherwise>
		        <xsl:message terminate="yes">
		           <xsl:value-of select="'FAILURE : Never Here.'"/>
		        </xsl:message>
		     </xsl:otherwise>
		  </xsl:choose>
	          <xsl:value-of select="concat('#endif // ', $defineOidString, $lf)"/>
	       </xsl:if>



	       <xsl:if test="$macroMustBeGeneForDATA='true'">
 	          <xsl:variable name="dataXMIID">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@data-type"/>
		        <xsl:with-param name="_attribute" select="'xmi.id'"/>
	             </xsl:call-template>
	          </xsl:variable>
		  <xsl:variable name="dataIsVariableSize">
		     <xsl:call-template name="isVariableSizeTypeCPP">
                        <xsl:with-param name="_typeId" select="$dataXMIID"/>
                     </xsl:call-template>
		  </xsl:variable>

	          <xsl:variable name="datatodefine">
                     <xsl:call-template name="replaceCharsInString">
                        <xsl:with-param name="_stringIn" select="$datastoreTypeNode/@data-type"/>
                        <xsl:with-param name="_charsIn" select="'.'"/>
                        <xsl:with-param name="_charsOut" select="'_'"/>
                     </xsl:call-template>
                  </xsl:variable>
	          <xsl:variable name="cppdatatodefine">
                     <xsl:call-template name="replaceCharsInString">
                        <xsl:with-param name="_stringIn" select="$datastoreTypeNode/@data-type"/>
                        <xsl:with-param name="_charsIn" select="'.'"/>
                        <xsl:with-param name="_charsOut" select="'::'"/>
                     </xsl:call-template>
                  </xsl:variable>

	          <xsl:variable name="defineDataString" select="concat('__DECLARE_', translate($datatodefine, $lcase, $ucase), '_TYPE_TRAITS__')"/>

	          <xsl:variable name="dataStereotype">
	             <xsl:call-template name="findAttributeNodeForTheModule">
	                <xsl:with-param name="_module" select="$datastoreTypeNode/@data-type"/>
		        <xsl:with-param name="_attribute" select="'stereotype'"/>
	             </xsl:call-template>
	          </xsl:variable>
      		  <xsl:variable name="dataStereotypeForDataType" select="key('stereotypeById', $dataStereotype)"/>
                  <xsl:value-of select="concat($lf, '#ifndef ', $defineDataString, $lf)"/>
	          <xsl:value-of select="concat('#define ', $defineDataString, $lf)"/>


		  <xsl:choose>
		     <xsl:when test="$dataStereotypeForDataType/@name = 'CORBAEnum'">
		        <xsl:value-of select="concat('DEFINE_SIMPLE_DATA_TRAITS(', $cppdatatodefine,');', $lf)"/>		        
		     </xsl:when>
		     <xsl:when test="$dataStereotypeForDataType/@name = 'CORBASequence'">
		        <xsl:value-of select="concat('DEFINE_VARIABLE_LENGTH_DATA_TRAITS(', $cppdatatodefine,');', $lf)"/>		        
		     </xsl:when>
		     <xsl:when test="$dataStereotypeForDataType/@name = 'CORBAUnion' or
                                     $dataStereotypeForDataType/@name = 'CORBAStruct'">
		        <xsl:choose>
			   <xsl:when test="$dataIsVariableSize='true'">
		              <xsl:value-of select="concat('DEFINE_VARIABLE_LENGTH_DATA_TRAITS(', $cppdatatodefine,');', $lf)"/>
			   </xsl:when>
			   <xsl:otherwise>
		              <xsl:value-of select="concat('DEFINE_FIXED_LENGTH_DATA_TRAITS(', $cppdatatodefine,');', $lf)"/>
			   </xsl:otherwise>
			</xsl:choose>
	             </xsl:when>
		     <xsl:when test="$dataStereotypeForDataType/@name = 'CORBAArray'">
		        <xsl:choose>
			   <xsl:when test="$dataIsVariableSize='true'">
		              <xsl:value-of select="concat('DEFINE_VARIABLE_LENGTH_ARRAY_TRAITS(', $cppdatatodefine,');', $lf)"/>
			   </xsl:when>
			   <xsl:otherwise>
		              <xsl:value-of select="concat('DEFINE_FIXED_LENGTH_ARRAY_TRAITS(', $cppdatatodefine,');', $lf)"/>
			   </xsl:otherwise>
			</xsl:choose>
		     </xsl:when>
		     <xsl:otherwise>
		        <xsl:message terminate="yes">
		           <xsl:value-of select="'FAILURE : Never Here.'"/>
		        </xsl:message>
		     </xsl:otherwise>
		  </xsl:choose>
	          <xsl:value-of select="concat('#endif // ', $defineDataString, $lf)"/>
	       </xsl:if>
            </xsl:document>
	 </xsl:if>	 

         <!--
            Proceed to the next item in idlNodes if any.
         -->
         <xsl:call-template name="datastore_type_traits">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template data_type_traits -->

<!--
   Check if the macro  must be generated for the specific type
-->
<xsl:template name="mustGeneratedMacro">
   <xsl:param name="_dataTypeID"/>
   <xsl:variable name="stereotypeForDataType" select="key('stereotypeById', $_dataTypeID)"/>

   <xsl:choose>
      <xsl:when test="$stereotypeForDataType/@name = 'CORBAEnum' or
                      $stereotypeForDataType/@name = 'CORBAUnion' or
                      $stereotypeForDataType/@name = 'CORBAStruct' or
                      $stereotypeForDataType/@name = 'CORBASequence' or
		      $stereotypeForDataType/@name = 'CORBAArray'">
         <xsl:value-of select="true()"/>
      </xsl:when>
      <xsl:otherwise>
         <xsl:value-of select="false()"/>
      </xsl:otherwise>
   </xsl:choose>

</xsl:template>

</xsl:stylesheet>


