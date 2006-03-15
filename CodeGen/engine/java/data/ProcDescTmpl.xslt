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
   Main - This template fetches the necessary data to generate a DTD.

   @param _programNode   A reference to a node /cdmw/programs/program.
-->
<xsl:template name="proc_desc">
   <xsl:param name="_programNode"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="servicesNode" select="$_programNode/services"/>
   <xsl:variable name="lifecycleNode" select="$servicesNode/lifecycle"/>
   <xsl:variable name="frameworkNodes" select="$lifecycleNode/framework"/>
   <xsl:variable name="datastoresNodes" select="$servicesNode/fault-tolerance/datastores/datastore"/>
   <xsl:variable name="hasDatastoreNodes" select="boolean($servicesNode/fault-tolerance/datastores/datastore)"/>


   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="indent">
      <xsl:value-of select="concat(' ', ' ', ' ')"/>
   </xsl:variable>
   <xsl:variable name="hasHomeImpl">
      <xsl:call-template name="containsHomeImpl">
         <xsl:with-param name="_programNode" select="$_programNode"/>
         <xsl:with-param name="_only" select="true()"/>
      </xsl:call-template>
   </xsl:variable>


   <!--
      Get the output filename.
   -->
   <xsl:variable name="filename">
      <xsl:call-template name="buildOutputFilename">
         <xsl:with-param name="_userDirectory" select="$_programNode/@directory"/>
         <xsl:with-param name="_userFilename" select="$_programNode/@init-dtd"/>
         <xsl:with-param name="_defaultFilename" select="'Proc_Desc'"/>
         <xsl:with-param name="_fileExtension" select="'.dtd'"/>
      </xsl:call-template>
   </xsl:variable>
   
   <!--
      Checks the presence of some elements in the input files.
   -->
   <xsl:variable name="hasRepositoryInterface" select="boolean($servicesNode/repository-interface)"/>
   <xsl:variable name="hasPerformance" select="boolean($servicesNode/performance)"/>
   <xsl:variable name="hasTrace" select="boolean($servicesNode/trace)"/>
   <xsl:variable name="hasTime" select="boolean($servicesNode/time)"/>
   <xsl:variable name="hasFaultTolerance" select="boolean($servicesNode/fault-tolerance)"/>
   <xsl:variable name="hasFaultToleranceWithDatastore" select="boolean($servicesNode/fault-tolerance/datastores)"/>
   <xsl:variable name="hasLifeCycle" select="boolean($lifecycleNode)"/>
   
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
         <xsl:with-param name="_lang" select="'xml'"/>
      </xsl:call-template>
      
      <xsl:value-of select="concat($lf, '&lt;!ELEMENT process (services+)&gt;', $lf)"/>
      <xsl:value-of select="concat($lf, '&lt;!ELEMENT services (')"/>

      <!--
         Include repository-interface.
      -->
      <xsl:if test="$hasRepositoryInterface">
         <xsl:text>repository-interface</xsl:text>
      </xsl:if>
      
      <!--
         Include performance (only if found in input files).
      -->
      <xsl:if test="$hasPerformance">
         <!--
            If repository-interface has been added, then add a coma.
         -->
         <xsl:if test="$hasRepositoryInterface">
            <xsl:text>, </xsl:text>
         </xsl:if>
         <xsl:text>performance</xsl:text>
      </xsl:if>
      
      <!--
         Include trace (only if found in input files).
      -->
      <xsl:if test="$hasTrace">
         <!--
            If repository-interface or performance has been added, then add a comma.
         -->
         <xsl:if test="$hasRepositoryInterface or $hasPerformance">
            <xsl:text>, </xsl:text>
         </xsl:if>
         <xsl:text>trace</xsl:text>
      </xsl:if>

      <!--
         Include time (only if found in input files).
      -->
      <xsl:if test="$hasTime">
         <!--
            If repository-interface, performance or trace
            have been added, then add a comma.
         -->
         <xsl:if test="$hasRepositoryInterface or $hasPerformance or $hasTrace">
            <xsl:text>, </xsl:text>
         </xsl:if>
         <xsl:text>time</xsl:text>
      </xsl:if>

      <!--
         Include fault-tolerance (only if found in input files).
      -->
      <xsl:if test="$hasFaultTolerance">
         <!--
            If repository-interface, performance, trace or time
            have been added, then add a comma.
         -->
         <xsl:if test="$hasRepositoryInterface or $hasPerformance or $hasTrace or $hasTime">
            <xsl:text>, </xsl:text>
         </xsl:if>
         <xsl:text>fault-tolerance</xsl:text>
      </xsl:if>
      
      <!--
         Include lifecycle (only if found in input files).
      -->
      <xsl:if test="$hasLifeCycle and $hasHomeImpl = 'false'">
         <!--
            If repository-interface, performance, trace or time
            have been added then add a comma.
         -->
         <xsl:if test="$hasRepositoryInterface or $hasPerformance or $hasTrace or $hasTime">
            <xsl:text>, </xsl:text>
         </xsl:if>
         <xsl:text>lifecycle</xsl:text>
      </xsl:if>
      
      <!--
         Close tag : services.
      -->
      <xsl:value-of select="concat(')&gt;', $lf)"/>

      <!--
         Describe repository-interface.
      -->
      <xsl:if test="$hasRepositoryInterface">
         <xsl:value-of select="concat($lf, '&lt;!ELEMENT repository-interface EMPTY&gt;', $lf)"/>
         <xsl:value-of select="concat('&lt;!ATTLIST repository-interface', $lf)"/>
         <xsl:value-of select="concat($indent, 'default-domain-name CDATA #REQUIRED', $lf)"/>
         <xsl:value-of select="concat('&gt;', $lf)"/>
      </xsl:if>
      
      <!--
         Describe performance.
      -->
      <xsl:if test="$hasPerformance">
         <xsl:value-of select="concat($lf, '&lt;!ELEMENT performance EMPTY&gt;', $lf)"/>
         <xsl:value-of select="concat('&lt;!ATTLIST performance', $lf)"/>
         <xsl:value-of select="concat($indent, 'result-file-name CDATA #REQUIRED', $lf)"/>
         <xsl:value-of select="concat('&gt;', $lf)"/>
      </xsl:if>
      
      <!--
         Describe trace.
      -->
      <xsl:if test="$hasTrace">
         <xsl:value-of select="concat($lf, '&lt;!ELEMENT trace EMPTY&gt;', $lf)"/>
         <xsl:value-of select="concat('&lt;!ATTLIST trace ', $lf)"/>
         <xsl:value-of select="concat($indent, 'flush-area-sz CDATA #REQUIRED', $lf)"/>
         <xsl:value-of select="concat($indent, 'flush-area-nb CDATA #REQUIRED', $lf)"/>
         <xsl:value-of select="concat($indent, 'flushing-time CDATA #REQUIRED', $lf)"/>
         <xsl:value-of select="concat($indent, 'msg-threshold CDATA #IMPLIED', $lf)"/> 
         <xsl:value-of select="concat($indent, 'collector-name CDATA #IMPLIED', $lf)"/>
         <xsl:value-of select="concat('&gt;', $lf)"/>
      </xsl:if>
      
      <!--
         TODO : Describe time.
      -->
      <xsl:if test="$hasTime">
         <xsl:value-of select="concat($lf, '&lt;!ELEMENT time EMPTY&gt;', $lf)"/>
      </xsl:if>
      
      <!--
         Describe fault-tolerance.
      -->
      


      <!--
         Now that elements have been added for each /cdmw/programs/program/services/fault-tolerance/datastores/datastore found,
         insert their descriptions.
      -->
      <xsl:for-each select="$datastoresNodes">
         <xsl:variable name="datastoreIdref" select="@idref"/>
	 <xsl:variable name="DataStoreKind" select="$servicesNode/fault-tolerance/datastores/@kind"/>

	 <xsl:value-of select="concat($lf, '&lt;!ELEMENT ', @idref, '.datastore', ' EMPTY&gt;', $lf)"/>
	 <xsl:value-of select="concat('&lt;!ATTLIST ', @idref, '.datastore', $lf)"/>
	 <xsl:value-of select="concat($indent, 'datastore-id CDATA #REQUIRED ', $lf)"/>
	 <xsl:value-of select="concat($indent, 'chunk-size CDATA #IMPLIED', $lf)"/>
	 <xsl:value-of select="concat($indent, 'freeze-size CDATA #IMPLIED', $lf)"/>
	 <xsl:value-of select="concat($indent, 'freeze-timeout CDATA #IMPLIED', $lf)"/>		
	 <xsl:value-of select="concat('&gt;', $lf)"/>

      </xsl:for-each>


      <xsl:if test="$hasFaultTolerance">
         <xsl:choose>
	    <xsl:when test="$hasFaultToleranceWithDatastore">

	        <xsl:value-of select="concat($lf, '&lt;!ELEMENT fault-tolerance (datastores)&gt;', $lf)"/>


		<xsl:choose>
                  <xsl:when test="$hasDatastoreNodes='true'">

		  
	              <xsl:value-of select="concat($lf, '&lt;!ELEMENT datastores (')"/>

	              <xsl:for-each select="$datastoresNodes">
		      <xsl:variable name="multiplicity">
                      <xsl:if test="@occurrences > 1">
                          <xsl:text>+</xsl:text>
                      </xsl:if>
                      </xsl:variable>
		      <xsl:variable name="datastoreIdref" select="@idref"/>
                      <xsl:choose>
                      <!--
                          If first position 
                       -->
                          <xsl:when test="position() = 1">
                               <xsl:value-of select="concat($datastoreIdref, '.datastore',$multiplicity)"/>
                          </xsl:when>
                      <!--
                          Else insert a comma before 
                      -->
                          <xsl:otherwise>
                              <xsl:value-of select="concat(', ',$datastoreIdref, '.datastore',$multiplicity)"/>
                          </xsl:otherwise>
                      </xsl:choose>
                      </xsl:for-each>
		<xsl:value-of select="concat(')&gt;', $lf)"/>
		</xsl:when>
		<xsl:otherwise>	
		<xsl:value-of select="concat($lf, '&lt;!ELEMENT datastores EMPTY&gt;', $lf)"/>
		</xsl:otherwise>
		</xsl:choose>


		<xsl:value-of select="concat('&lt;!ATTLIST datastores', $lf)"/>
		<xsl:variable name="DataStoreKind" select="$servicesNode/fault-tolerance/datastores/@kind"/>

		<xsl:choose>
                    <xsl:when test="$DataStoreKind = 'MIOP'">

		        <xsl:value-of select="concat($indent, 'mutlicast-port CDATA #REQUIRED ', $lf)"/>
		        <xsl:value-of select="concat($indent, 'multicast-domain CDATA #REQUIRED ', $lf)"/>
		        <xsl:value-of select="concat($indent, 'ip-address CDATA #REQUIRED ', $lf)"/>
		    </xsl:when>
                    <xsl:when test="$DataStoreKind = 'IIOP'">

		    </xsl:when>
		</xsl:choose>


	        <xsl:value-of select="concat($indent, 'max-transaction-in-progress CDATA #REQUIRED', $lf)"/>
		<xsl:value-of select="concat($indent, 'max-transaction-done CDATA #REQUIRED', $lf)"/>
		<xsl:value-of select="concat($indent, 'cohort-timeout CDATA #REQUIRED', $lf)"/>
		<xsl:value-of select="concat($indent, 'coordinator-timeout CDATA #REQUIRED', $lf)"/>
		<xsl:value-of select="concat($indent, 'number-of-scheduler-thread CDATA #REQUIRED', $lf)"/>
		<xsl:value-of select="concat($indent, 'chunk-size CDATA #REQUIRED', $lf)"/>
		<xsl:value-of select="concat($indent, 'freeze-size CDATA #REQUIRED', $lf)"/>
		<xsl:value-of select="concat($indent, 'freeze-timeout CDATA #REQUIRED', $lf)"/>		
		<xsl:value-of select="concat('&gt;', $lf)"/>
		
	    </xsl:when>
	    <xsl:otherwise>
	        <xsl:value-of select="concat($lf, '&lt;!ELEMENT fault-tolerance EMPTY&gt;', $lf)"/>
	    </xsl:otherwise>
         </xsl:choose>	    
	 
         <xsl:value-of select="concat('&lt;!ATTLIST fault-tolerance', $lf)"/>
         <xsl:value-of select="concat($indent, 'request-duration-policy CDATA #REQUIRED', $lf)"/>
         <xsl:value-of select="concat($indent, 'client-id CDATA #REQUIRED', $lf)"/>
         <xsl:value-of select="concat('&gt;', $lf)"/>	
      </xsl:if>





      <!--
         Describe lifecycle.
      -->
      <xsl:if test="$hasLifeCycle and $hasHomeImpl = 'false'">
         <xsl:value-of select="concat($lf, '&lt;!ELEMENT lifecycle (')"/>
            <!--
               Create an element for each /cdmw/programs/program/services/lifecycle/framework found.
            -->
            <xsl:for-each select="$frameworkNodes">
               <xsl:choose>
                  <!--
                     If first position then insert the @id.
                  -->
                  <xsl:when test="position() = 1">
                     <xsl:text></xsl:text><xsl:value-of select="@idref"/>
                  </xsl:when>
                  <!--
                     Else insert a comma before @id.
                  -->
                  <xsl:otherwise>
                     <xsl:text></xsl:text><xsl:value-of select="concat(', ', @idref)"/>
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:for-each>
         <!--
            Close tag : lifecycle.
         -->
         <xsl:text disable-output-escaping="yes">)&gt;</xsl:text><xsl:value-of select="$lf"/>
      </xsl:if>
      
      <!--
         Now that elements have been added for each /cdmw/programs/program/services/lifecycle/framework found,
         insert their descriptions.
      -->
      <xsl:for-each select="$frameworkNodes">
         <xsl:variable name="frameworkIdref" select="@idref"/>

         <!--
            Get the matching node /cdmw/lifecycle-frameworks/lifecycle-framework.
         -->
         <xsl:variable name="lifecycleFrameworkNode" select="$lifecycleFrameworkNodes[@id = $frameworkIdref]"/>
         <xsl:variable name="isHomeImpl" select="boolean($lifecycleFrameworkNode/home-impl)"/>
         
         <xsl:if test="not($isHomeImpl)">
            <!--
               Get the kind of the factory referred by the lifecycle-framework found above.
            -->
            <xsl:variable name="isNamed">
               <xsl:variable name="factoryNodeKind" select="$factoryNodes[@idl-interface = $lifecycleFrameworkNode/@factory-idref]/@kind"/>
               <xsl:choose>
                  <xsl:when test="$factoryNodeKind = 'NAMED'">
                     <xsl:value-of select="true()"/>
                  </xsl:when>
                  <xsl:when test="$factoryNodeKind = 'ANONYMOUS'">
                     <xsl:value-of select="false()"/>
                  </xsl:when>
               </xsl:choose>
            </xsl:variable>

            <!--
               Get the threading of the lifecycle-framework found above.
            -->
            <xsl:variable name="threading" select="translate($lifecycleFrameworkNode/*/@threading, $ucase, $lcase)"/>

            <!--
               Declare a factory.
               Output <!ENTITY framework-id (framework-id-instance[multiplicity])>
                      <!ATTLIST framework-id
                         occurrences CDATA #FIXED "[multiplicity]"
                      >
            -->
            <xsl:variable name="multiplicity">
               <xsl:if test="@occurrences > 1">
                  <xsl:text>+</xsl:text>
               </xsl:if>
            </xsl:variable>
            <xsl:value-of select="concat($lf, '&lt;!ELEMENT ', @idref, ' (', @idref, '-instance', $multiplicity, ')&gt;', $lf)"/>
            <xsl:value-of select="concat('&lt;!ATTLIST ', @idref, $lf)"/>
            <xsl:value-of select="concat($indent, 'occurrences CDATA #FIXED ')"/>"<xsl:value-of select="@occurrences"/>"<xsl:value-of select="$lf"/>
            <xsl:value-of select="concat('&gt;', $lf)"/>

            <!--
               Declare a factory instance.
            -->
            <xsl:choose>
               <!--
                  Named object.
                  Output <!ELEMENT framework-id-instance (threading)>
                         <!ATTLIST framework-id-instance
                            domain-name CDATA #REQUIRED
                            full-factory-name CDATA #REQUIRED
                         >
               -->
               <xsl:when test="$isNamed = 'true'">
                  <xsl:value-of select="concat($lf, '&lt;!ELEMENT ', @idref, '-instance (', $threading, ')&gt;', $lf)"/>
                  <xsl:value-of select="concat('&lt;!ATTLIST ', @idref, '-instance', $lf)"/>
                  <xsl:value-of select="concat($indent, 'domain-name CDATA #REQUIRED', $lf)"/>
                  <xsl:value-of select="concat($indent, 'full-factory-name CDATA #REQUIRED', $lf)"/>
                  <xsl:value-of select="concat('&gt;', $lf)"/>
               </xsl:when>

               <!--
                  Anonymous object.
                  Output <!ELEMENT framework-id-instance (threading)>
                         <!ATTLIST framework-id-instance
                            full-factory-name CDATA #REQUIRED
                         >
               -->
               <xsl:otherwise>
                  <xsl:value-of select="concat($lf, '&lt;!ELEMENT ', @idref, '-instance (', $threading, ')&gt;', $lf)"/>
                  <xsl:value-of select="concat('&lt;!ATTLIST ', @idref, '-instance', $lf)"/>
                  <xsl:value-of select="concat($indent, 'full-factory-name CDATA #REQUIRED', $lf)"/>
                  <xsl:value-of select="concat('&gt;', $lf)"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:if>
      </xsl:for-each>

      <!--
         Complete element descriptions with additional elements
         regarding threading, named and anonymous instances.
      -->
      <xsl:if test="$hasLifeCycle and $hasHomeImpl = 'false'">
         <!--
            Look for MULTITHREAD.
         -->
         <xsl:variable name="multithreadObjs">
            <xsl:call-template name="countObjs">
               <xsl:with-param name="_nodes" select="$frameworkNodes"/>
               <xsl:with-param name="_obj" select="'MULTITHREAD'"/>
            </xsl:call-template>
         </xsl:variable>
         
         <!--
            If found MULTITHREAD then describe MULTITHREAD.
         -->
         <xsl:if test="$multithreadObjs > 0">
            <xsl:value-of select="concat($lf, '&lt;!ELEMENT multithread (serialize | thread-per-request | thread-pool)&gt;', $lf)"/>
            <xsl:value-of select="concat($lf, '&lt;!ELEMENT serialize EMPTY&gt;', $lf)"/>
            <xsl:value-of select="concat($lf, '&lt;!ELEMENT thread-per-request EMPTY&gt;', $lf)"/>
            <xsl:value-of select="concat($lf, '&lt;!ELEMENT thread-pool EMPTY&gt;', $lf)"/>
            <xsl:value-of select="concat('&lt;!ATTLIST thread-pool', $lf)"/>
            <xsl:value-of select="concat($indent, 'thread-size CDATA #REQUIRED', $lf)"/>
            <xsl:value-of select="concat('&gt;', $lf)"/>
         </xsl:if>
         
         <!--
            Look for SERIALIZE.
         -->
         <xsl:variable name="serializeObjs">
            <xsl:call-template name="countObjs">
               <xsl:with-param name="_nodes" select="$frameworkNodes"/>
               <xsl:with-param name="_obj" select="'SERIALIZE'"/>
            </xsl:call-template>
         </xsl:variable>
         
         <!--
            If found SERIALIZE but no MULTITHREAD then describe SERIALIZE.
         -->
         <xsl:if test="($serializeObjs > 0) and ($multithreadObjs = 0)">
            <xsl:value-of select="concat($lf, '&lt;!ELEMENT serialize EMPTY&gt;', $lf)"/>
         </xsl:if>
      </xsl:if>
   </xsl:document>
</xsl:template> <!-- end of template proc_desc -->



<!--
   This template returns the total number of a given type of objects.

   @param _nodes   Some node references for the search.
   @param _obj     The type of object to count.
-->
<xsl:template name="countObjs">
   <xsl:param name="_nodes"/>
   <xsl:param name="_obj"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_position" select="0"/>
   <xsl:param name="_count" select="0"/>

   <xsl:choose>
      <!--
         If there are no node references or if we have been through all the nodes,
         then return the count value.
      -->
      <xsl:when test="(count($_nodes) = 0) or (count($_nodes) = $_position)">
         <xsl:value-of select="$_count"/>
      </xsl:when>     
      <!--
         Otherwise, continue the process.
      -->
      <xsl:otherwise>
         <xsl:variable name="frameworkIdref" select="$_nodes[$_position + 1]/@idref"/>
         <!--
            Get the matching node /cdmw/lifecycle-frameworks/lifecycle-framework.
         -->
         <xsl:variable name="lifecycleFrameworkNode" select="$lifecycleFrameworkNodes[@id = $frameworkIdref]"/>
         
         <!--
            Increase value.
         -->
         <xsl:variable name="incValue">
            <xsl:choose>
               <xsl:when test="($_obj = 'MULTITHREAD') and boolean($lifecycleFrameworkNode/*/@threading = $_obj)">
                  <xsl:value-of select="1"/>
               </xsl:when>
               <xsl:when test="($_obj = 'SERIALIZE') and boolean($lifecycleFrameworkNode/*/@threading = $_obj)">
                  <xsl:value-of select="1"/>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="0"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:variable>
         <xsl:call-template name="countObjs">
            <xsl:with-param name="_nodes" select="$_nodes"/>
            <xsl:with-param name="_obj" select="$_obj"/>
            <xsl:with-param name="_position" select="$_position + 1"/>
            <xsl:with-param name="_count" select="$_count + $incValue"/>
         </xsl:call-template>
      </xsl:otherwise>
   </xsl:choose>
</xsl:template> <!-- end of template countObjs -->



</xsl:stylesheet>


