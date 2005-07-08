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


<xsl:stylesheet version="1.1" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">



<!--
   Main - This template fetches the necessary data from inputs
   to generate a file.

   @param none
-->

<xsl:template name="cdmw_interface_cpp">
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index" select="1"/>

   <xsl:variable name="programNodes" select="$cdmwNode/programs/program"/>

   <xsl:if test="boolean($programNodes)">
      <xsl:if test="$_index &lt;= count($programNodes)">
         <xsl:variable name="programNode" select="$programNodes[$_index]"/>

         <!--
            Get the output filename.
         -->
         <xsl:variable name="filename">
            <xsl:call-template name="buildOutputFilename">
               <xsl:with-param name="_userDirectory" select="$programNode/@directory"/>
               <xsl:with-param name="_userFilename" select="$programNode/@filename"/>
               <xsl:with-param name="_defaultFilename" select="'CdmwInterface'"/>
               <xsl:with-param name="_fileExtension" select="'.cpp'"/>
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
            <!--
               Insert the content.
            -->
            <xsl:call-template name="cdmw_interface_cpp.content">
               <xsl:with-param name="_programNode" select="$programNode"/>
            </xsl:call-template>
         </xsl:document>

         <!--
            Proceed to the next node.
         -->
         <xsl:call-template name="cdmw_interface_cpp">
            <xsl:with-param name="_index" select="$_index + 1"/>
         </xsl:call-template>
      </xsl:if>
   </xsl:if>
</xsl:template> <!-- end of template cdmw_interface_cpp -->




<xsl:template name="count.datastore.occurrence">
   <xsl:param name="_occurrences"/>
   <xsl:param name="_datastoreNodes"/>
   <xsl:param name="_datastoreOccurrences"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index_datastore"/>
   <xsl:choose>
      <xsl:when test="$_index_datastore &lt;= $_occurrences">
         <xsl:variable name="datastoreOccurrences" select="$_datastoreNodes[$_index_datastore]/@occurrences"/>
         <xsl:call-template name="count.datastore.occurrence">
            <xsl:with-param name="_datastoreNodes" select="$_datastoreNodes"/>
            <xsl:with-param name="_occurrences" select="$_occurrences"/>
            <xsl:with-param name="_datastoreOccurrences" select="$_datastoreOccurrences + $datastoreOccurrences"/>
            <xsl:with-param name="_index_datastore" select="$_index_datastore + 1"/>
         </xsl:call-template>
	 </xsl:when>
       <xsl:otherwise>
          <xsl:value-of select="$_datastoreOccurrences"/>
       </xsl:otherwise>	
   </xsl:choose>
</xsl:template>

<xsl:template name="datastore.occurrence">
   <xsl:param name="_occurrences"/>
   <xsl:param name="_occurrencesInProgress"/>
   <xsl:param name="_occurrencesInProgressForThisDataStore"/>
   <xsl:param name="_dataStoresKind"/>
   <xsl:param name="_datastoreNodes"/>
   <!--
      Parameters below are used for recursiveness.
   -->
   <xsl:param name="_index_datastore"/>

   <xsl:if test="$_occurrencesInProgress &lt;= $_occurrences">


            <xsl:variable name="datastoreIdref" select="$_datastoreNodes[$_index_datastore]/@idref"/>
	    <xsl:variable name="datastoreOccurrences" select="$_datastoreNodes[$_index_datastore]/@occurrences"/>
	  
	    <!--
               Get the matching node /cdmw/datastore-frameworks/datastore-framework.
            -->
            <xsl:variable name="datastoreTypeNode" select="$DatastoreTypeNodes[@typename = $datastoreIdref]"/>
	    <xsl:variable name="_datastoreTypename" select="$datastoreTypeNode/@typename"/>
	    <xsl:variable name="datastoreOidType" select="$datastoreTypeNode/@oid-type"/>
	    <xsl:variable name="datastoreDataType" select="$datastoreTypeNode/@data-type"/>
	    <xsl:variable name="datastoreIdlfilename" select="$datastoreTypeNode/@idl-filename"/>
	  
	    <!--
	       define the type of the storage home
	    -->

	    <xsl:variable name="datastoreoidtmp">
               <xsl:call-template name="replaceCharsInString">
                  <xsl:with-param name="_stringIn" select="$datastoreOidType"/>
                  <xsl:with-param name="_charsIn" select="'.'"/>
                  <xsl:with-param name="_charsOut" select="$cppSep"/>
               </xsl:call-template>
            </xsl:variable>
            <xsl:variable name="datastoredatatmp">
               <xsl:call-template name="replaceCharsInString">
                  <xsl:with-param name="_stringIn" select="$datastoreDataType"/>
		  <xsl:with-param name="_charsIn" select="'.'"/>
		  <xsl:with-param name="_charsOut" select="$cppSep"/>
               </xsl:call-template>
            </xsl:variable>

	    <xsl:variable name="datastoreoidissimpletype">
	       <xsl:call-template name="isSimpleDataTypeForDataTraits">
	          <xsl:with-param name="_dataType" select="$datastoreoidtmp"/>
	       </xsl:call-template>
	    </xsl:variable>
	    <xsl:variable name="datastoredataissimpletype">
	       <xsl:call-template name="isSimpleDataTypeForDataTraits">
	          <xsl:with-param name="_dataType" select="$datastoredatatmp"/>
	       </xsl:call-template>
	    </xsl:variable>


	    <xsl:variable name="datastoreoid">
	       <xsl:choose>
	          <xsl:when test="$datastoreoidissimpletype='true'">
		     <xsl:call-template name="getSimpleDataTypeForDataTraits">
		        <xsl:with-param name="_dataType" select="$datastoreoidtmp"/>
		     </xsl:call-template>
		  </xsl:when>
		  <xsl:otherwise>
		     <xsl:value-of select="$datastoreoidtmp"/>
		  </xsl:otherwise>
	       </xsl:choose>
            </xsl:variable>   
	    <xsl:variable name="datastoredata">
	       <xsl:choose>
	          <xsl:when test="$datastoredataissimpletype='true'">
		     <xsl:call-template name="getSimpleDataTypeForDataTraits">
		        <xsl:with-param name="_dataType" select="$datastoredatatmp"/>
		     </xsl:call-template>
		  </xsl:when>
		  <xsl:otherwise>
		     <xsl:value-of select="$datastoredatatmp"/>
		  </xsl:otherwise>
	       </xsl:choose>
            </xsl:variable>   
   <!--
       export the specification for the storage home
   -->
   <![CDATA[ typedef Cdmw::FT::DataStore<]]><xsl:value-of select="$datastoreoid"/><![CDATA[,]]>
	                           <xsl:value-of select="$datastoredata"/><![CDATA[>]]>
				   <xsl:value-of select="$_datastoreTypename"/><![CDATA[;]]>



<![CDATA[ // get information for  ]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",n°]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[
     try {
             std::string res;
	     unsigned long res_long;	   
             res = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							  "max-transaction-in-progress"); 
             max_transaction_in_progress = atoi(res.c_str());

             res = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "max-transaction-done"); 
             max_transaction_done = atoi(res.c_str());

             res = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "cohort-timeout");
             res_long = atoi(res.c_str());				 
             cohort_timeout.seconds = res_long/1000;
             cohort_timeout.microseconds = (res_long%1000)*1000;

             res = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "coordinator-timeout");				 
             res_long = atoi(res.c_str());				 
             coordinator_timeout.seconds = res_long/1000;
             coordinator_timeout.microseconds = (res_long%1000)*1000;

             res = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "datastore-id");
             datastore_id = atoi(res.c_str());

             res = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "number-of-scheduler-thread");
             number_of_scheduler_thread = atoi(res.c_str());

             res = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "chunk-size");
             chunk_size = atoi(res.c_str());

             res = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "freeze-size");
             freeze_size = atoi(res.c_str());

             res = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "freeze-timeout");
             res_long = atoi(res.c_str());				 
             freeze_timeout.seconds = res_long/1000;
             freeze_timeout.microseconds = (res_long%1000)*1000;
]]>
<xsl:if test="$_dataStoresKind = 'MIOP'">
<![CDATA[    multicast_port = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "multicast_port");

             multicast_domain = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "multicast_domain");

             multicast_IP_address = xml_data->getDataStoreAttributeValue("]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[.datastore",
	                                                 ]]><xsl:value-of select="$_occurrencesInProgressForThisDataStore"/><![CDATA[,
							 "multicast_IP_address");]]>
</xsl:if>

<![CDATA[    } catch (const Cdmw::BadParameterException& ex) {
                CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                             CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                              CORBA::COMPLETED_NO),ex.what() );
             }
]]>

      <xsl:variable name="datastoreVariable" select="translate($_datastoreTypename, $ucase, $lcase)"/>
      <xsl:value-of select="$_datastoreTypename"/><![CDATA[ * ]]>
      <xsl:value-of select="$datastoreVariable"/><![CDATA[_]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[ = new ]]><xsl:value-of select="$_datastoreTypename"/><![CDATA[ ( ]]>
      <xsl:choose>
	 <xsl:when test="$_dataStoresKind = 'MIOP'">
	    <![CDATA[datastore_id,
	    orb,
	    CDMW_rootPOA.in(),
	    multicast_port,
	    multicast_domain,
	    multicast_IP_address,
	    max_transaction_in_progress,
	    max_transaction_done,
	    cohort_timeout,
	    coordinator_timeout,
	    number_of_scheduler_thread,
	    freeze_size,
	    chunk_size,
	    freeze_timeout]]>
	 </xsl:when>
	 <xsl:when test="$_dataStoresKind = 'IIOP'">
	    <![CDATA[datastore_id,
	    orb,
	    CDMW_rootPOA.in(),
	    max_transaction_in_progress,
	    max_transaction_done,
	    cohort_timeout,
	    coordinator_timeout,
	    number_of_scheduler_thread,
	    freeze_size,
	    chunk_size,
	    freeze_timeout]]>
	 </xsl:when>
      </xsl:choose>
      <![CDATA[ ); ]]>

<![CDATA[
      CdmwFT::StateTransfer::LocalDataStoreInfo localDataStoreInfo]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[ ;
      localDataStoreInfo]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[.dsid = datastore_id;
      CdmwFT::StateTransfer::DataStore_var ]]><xsl:value-of select="$datastoreVariable"/><![CDATA[_]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[_ref = ]]>
<![CDATA[
      (new Cdmw::FT::DataStore_impl(CDMW_rootPOA.in(),*]]><xsl:value-of select="$datastoreVariable"/><![CDATA[_]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[))->_this();


      localDataStoreInfo]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[.local_data_store = CdmwFT::StateTransfer::DataStore::_duplicate(]]><xsl:value-of select="$datastoreVariable"/><![CDATA[_]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[_ref.in());]]>

      <xsl:choose>
	 <xsl:when test="$_dataStoresKind = 'MIOP'">
	 <![CDATA[
	 localDataStoreInfo]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[.coordinator = NULL;
	 localDataStoreInfo]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[.cohort = NULL; ]]>
	 </xsl:when>
	 <xsl:when test="$_dataStoresKind = 'IIOP'">
	 <![CDATA[      localDataStoreInfo]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[.coordinator = ]]>
	    <xsl:value-of select="$datastoreVariable"/><![CDATA[_]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[->get_tpc_coordinator();]]>
	 <![CDATA[      localDataStoreInfo]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[.cohort = ]]>
	    <xsl:value-of select="$datastoreVariable"/><![CDATA[_]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[->get_tpc_cohort();]]>
	 </xsl:when>
      </xsl:choose>
<![CDATA[
      localDataStoreInfos[dataStore_inc] = localDataStoreInfo]]><xsl:value-of select="$_occurrencesInProgress"/><![CDATA[;
      dataStore_inc++;
]]>
      <!--
          Proceed to the next node.
       -->

       <xsl:choose>
          <xsl:when test="$_occurrencesInProgressForThisDataStore &lt; $datastoreOccurrences">
	     <xsl:call-template name="datastore.occurrence">
                <xsl:with-param name="_datastoreNodes" select="$_datastoreNodes"/>
                <xsl:with-param name="_occurrences" select="$_occurrences"/>
		<xsl:with-param name="_occurrencesInProgress" select="$_occurrencesInProgress + 1"/>
		<xsl:with-param name="_occurrencesInProgressForThisDataStore" select="$_occurrencesInProgressForThisDataStore + 1"/>
		<xsl:with-param name="_dataStoresKind" select="$_dataStoresKind"/>
		<xsl:with-param name="_index_datastore" select="$_index_datastore"/>
	     </xsl:call-template>
	  </xsl:when>
	  <xsl:otherwise>
	     <xsl:call-template name="datastore.occurrence">
                <xsl:with-param name="_datastoreNodes" select="$_datastoreNodes"/>
                <xsl:with-param name="_occurrences" select="$_occurrences"/>
		<xsl:with-param name="_occurrencesInProgress" select="$_occurrencesInProgress + 1"/>
		<xsl:with-param name="_occurrencesInProgressForThisDataStore" select="1"/>
		<xsl:with-param name="_dataStoresKind" select="$_dataStoresKind"/>
		<xsl:with-param name="_index_datastore" select="$_index_datastore + 1"/>
	     </xsl:call-template>	  
	  </xsl:otherwise>
       </xsl:choose>	  
   </xsl:if>
</xsl:template>






<!--
   This template generates the content of the file.

   @param _programNode   A reference to a node /cdmw/programs/program.
-->

<xsl:template name="cdmw_interface_cpp.content">
   <xsl:param name="_programNode"/>

   <!--
      References to some nodes.
   -->
   <xsl:variable name="servicesNode" select="$_programNode/services"/>
   <xsl:variable name="frameworkNodes" select="$servicesNode/lifecycle/framework"/>
   <xsl:variable name="datastoreNodes" select="$servicesNode/fault-tolerance/datastores/datastore"/>

   <!--
      Miscellaneous variables.
   -->
   <xsl:variable name="hasRepositoryInterface" select="boolean($servicesNode/repository-interface)"/>
   <xsl:variable name="hasPerformance" select="boolean($servicesNode/performance)"/>
   <xsl:variable name="hasTrace" select="boolean($servicesNode/trace)"/>
   <xsl:variable name="hasTime" select="boolean($servicesNode/time)"/>
   <xsl:variable name="hasFaultTolerance" select="boolean($servicesNode/fault-tolerance)"/>
   <xsl:variable name="hasDataStore" select="boolean($servicesNode/fault-tolerance/datastores)"/>
   
   <xsl:variable name="hasLifecycle" select="boolean($servicesNode/lifecycle)"/>
   <xsl:variable name="hasHomeImpl">
      <xsl:call-template name="containsHomeImpl">
         <xsl:with-param name="_programNode" select="$_programNode"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="hasEventPorts">
      <xsl:call-template name="containsEventPorts">
         <xsl:with-param name="_programNode" select="$_programNode"/>
      </xsl:call-template>
   </xsl:variable>
   <xsl:variable name="cdmwinitNamespace" select="'Cdmw.CdmwInit'"/>
   <xsl:variable name="datastoreNamespace" select="'DataStore'"/>

   <!--
      The code generation starts here.
   -->

   <![CDATA[
   #include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
   #include "ConfAndPlug/cdmwinit/CdmwInitConfiguration.hpp"
   #include "ConfAndPlug/cdmwinit/InitUtils.hpp"
   #include "Foundation/common/Assert.hpp"

   #include "Foundation/orbsupport/StrategyList.hpp"

   #include "Foundation/osthreads/Mutex.hpp"
   #include "Foundation/osthreads/Condition.hpp"

   #include "Foundation/common/Exception.hpp"
   #include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

   #include "Foundation/orbsupport/OrbSupport.hpp"
   #include "Repository/repositoryinterface/RepositoryInterface.hpp"
   ]]>

   <xsl:if test="$hasLifecycle">
      <![CDATA[
      #include "LifeCycle/lifecycleinit/InitUtils.hpp"
      ]]>
   </xsl:if>

   <xsl:if test="$hasFaultTolerance">
      <![CDATA[
      #include "Foundation/ossupport/OS.hpp"
      #include "FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp"
      #include "FaultTolerance/ftlocationmanager/PrimaryBackupAdmin_impl.hpp"
      #include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"
      #include "FaultTolerance/ftcommon/FTConfiguration.hpp"
      #include "FaultTolerance/ftstatemanager/StorageHome.hpp"
	  
      ]]>
   </xsl:if>

   <xsl:if test="$hasDataStore">

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

      <![CDATA[
      #include "FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp"
      #include <FaultTolerance/ftlocationmanager/LocationDataStoreTimeoutHandler_impl.hpp>
      #include "]]><xsl:value-of select="$pathPrefix"/><![CDATA[GlobalDataStores.hpp"
      ]]>
   </xsl:if>


   <xsl:if test="$hasHomeImpl='true'">
      <![CDATA[
      #include "CCMContainer/ccmcif/InitUtils.hpp"
      #include "CCMContainer/ccmcontainer/InitUtils.hpp"
      #include "CCMContainer/ccmcomponentserver/InitUtils.hpp"
      #include "CCMContainer/ccmcontainer/HomeAllocatorRegistry.hpp"
      #include "Foundation/common/Locations.hpp"
      #include "Repository/naminginterface/NamingUtil.hpp"
      #include "Event/idllib/CdmwEvent.stub.hpp"
      ]]>
   </xsl:if>

   <xsl:for-each select="$frameworkNodes">
      <xsl:call-template name="includeFramework">
         <xsl:with-param name="_framework" select="."/>
         <xsl:with-param name="_hasHomeImplRef" select="$hasHomeImpl"/>
      </xsl:call-template>
   </xsl:for-each>


   <xsl:call-template name="includeOidDataForDataStore">
      <xsl:with-param name="_datastorenodes" select="$datastoreNodes"/>
   </xsl:call-template>



   <![CDATA[
      #include <memory> // for std::auto_ptr
      #include <string>
      #include <vector>
   ]]>

   <xsl:if test="$hasTrace">
      <![CDATA[
      #include "TraceAndPerf/tracelibrary/Trace.hpp"
      #include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"
      #include "TraceAndPerf/tracelibrary/InitUtils.hpp"
      
      #define CDMW_TRACES_ENABLED
      
      ]]>
   </xsl:if>
   
   <xsl:if test="$hasPerformance">
      <![CDATA[
      #include "TraceAndPerf/performancelibrary/InitUtils.hpp"
      
      #define CDMW_PERFORMANCE_ENABLED
      
      ]]>
   </xsl:if>
   
   
   #define CDMW_TIME_SERVICE_ENABLED

   <![CDATA[
      namespace
      {
         using namespace Cdmw::CdmwInit;

         static void ADVISE_EXCEPTION_RAISED(CdmwProcess_impl *process_impl)
         {
             if (process_impl != 0) {
                 process_impl->set_cdmw_init_exception_raised();
                 process_impl->get_condition().broadcast();
             }
         }
      } // end anonymous namespace
   ]]>

   //--------------------------------------------------------------------------

   <xsl:call-template name="openNamespace">
      <xsl:with-param name="_scope" select="$cdmwinitNamespace"/>
      <xsl:with-param name="_lastTokenIsNamespace" select="true()"/>
   </xsl:call-template>



   <![CDATA[
    void 
    CDMW_init(CORBA::ORB_ptr orb, int & argc, char** argv, ProcessControl* ctrl) 
   	throw (CORBA::SystemException)
    {

            CDMW_ASSERT(!CORBA::is_nil(orb));
            CdmwProcess_impl * process_impl = 0;

      try {
         // Get the XML init filename. This may raise CORBA::BAD_PARAM, CORBA::NO_MEMORY,
         // or CORBA::INTERNAL    
         std::string xml_file = InitUtils::get_xml_initialisation_file(argc, argv);
        
         // Get the XML documents validation option. This may raise CORBA::NO_MEMORY,
         // or CORBA::INTERNAL
         bool validate_xml = InitUtils::get_xml_validation_option(argc, argv);
        
         // Parsing xml
         std::auto_ptr<Cdmw::CdmwInit::CdmwInitConfiguration> xml_data(0);
         try {
            std::auto_ptr<Cdmw::CdmwInit::CdmwInitConfiguration> init
               (Cdmw::CdmwInit::CdmwInitConfiguration::ReadConfiguration(xml_file,
                                                                            validate_xml));
            
            xml_data = init;
         } catch (const Cdmw::OutOfMemoryException &) {
            CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                          CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY, 
                                           CORBA::COMPLETED_NO) );
         } catch (const Cdmw::CdmwInit::ConfigurationException & ex) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                           CORBA::COMPLETED_NO),
                          ex.what());
         } catch(const Cdmw::InternalErrorException & ex){
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALXMLLibraryError,
                                          CORBA::COMPLETED_NO),
                          ex.what() );
         }

         // *****************************************************************
         // *****************  CDMW root POA and servant manager ***********
         // *****************************************************************
         //
         // Create the CDMW root POA. It has its own POA Manager
         // May raise a CORBA::SystemException.
         //
         PortableServer::POA_var CDMW_rootPOA = InitUtils::create_cdmw_root_POA(orb);
        
         PortableServer::POAManager_var poaManager = CDMW_rootPOA -> the_POAManager();
         //
         // Initialize the server so that incoming requests are
         // handled from this point on
         //
         // May raise PortableServer::AdapterInactive or CORBA::SystemException
         try {
            poaManager -> activate(); // TODO: maybe move it?
         } catch (const PortableServer::POAManager::AdapterInactive & ) {
            CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALORBAdapterInactive,
                                          CORBA::COMPLETED_NO) );
         } catch (const CORBA::SystemException & ex ) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                          ex, "Could not activate the CDMW POA Manager!" );
         }
   ]]>

    <xsl:if test="$hasFaultTolerance">
      <![CDATA[
         // *****************************************************************
         // *************  setup FaultTolerance Policies   ******************
         // *****************************************************************

         Cdmw::OrbSupport::OrbSupport::set_endpoint_selector_factory(::Cdmw::FT::FTConfiguration::get_FT_endpoint_selector_factory());

         size_t duration_time;
         std::string client_id_str;
            
         try {
               std::string res;
               try {
                  res = xml_data->getServiceAttributeValue("fault-tolerance", "request-duration-policy"); 
                  duration_time = atoi(res.c_str());
               } catch (const Cdmw::BadParameterException& ex) {
                  CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                 CORBA::COMPLETED_NO),
                                ex.what() );
               }

               try  {
                  client_id_str = xml_data->getServiceAttributeValue("fault-tolerance", "client-id");
				  ::Cdmw::FT:: FTConfiguration::set_client_id(orb, client_id_str.c_str());
               } catch (const Cdmw::BadParameterException& ) {
                  // do nothing : parameter is optional
               }
          } catch (const Cdmw::CdmwInit::XMLErrorException & ex) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                             CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                              CORBA::COMPLETED_NO),
                             ex.what() );
          } catch (const OutOfMemoryException & ex) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                             CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                              CORBA::COMPLETED_NO),
                             ex.what() );
          } 

         CORBA::Object_var obj;
         obj = orb->resolve_initial_references("ORBPolicyManager");
        
         CORBA::PolicyManager_var policy_manager 
            = CORBA::PolicyManager::_narrow(obj.in());
        
         // Create and apply an ORB-wide Routed policy
         CORBA::Any any;
         TimeBase::TimeT duration = duration_time;

         any <<= duration;
         CORBA::PolicyList policies(1);
         policies.length(1);
         policies[0] =
            orb->create_policy(::FT::REQUEST_DURATION_POLICY, any);
        
         policy_manager->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
      ]]>
   </xsl:if>



      <![CDATA[
         // *****************************************************************
         // *****************  setup platforminterface object  **************
         // *****************************************************************

         // Did we receive a ProcessControl callback object?
         {
            ProcessControl_var process_ctrl_var;
            if (ctrl != 0)
                process_ctrl_var = ctrl;
            else
                process_ctrl_var = new DefaultProcessControl();
            
            // CdmwProcess_impl() will acquire a reference on ProcessControl object and
            // will automatically release it at object's destruction.
            PortableServer::POA_var rootPOA = InitUtils::get_root_POA(orb);
            process_impl = new CdmwProcess_impl(orb,
                                                 rootPOA.in(),
                                                 CDMW_rootPOA.in(),
                                                 process_ctrl_var._retn());
            PortableServer::ServantBase_var process_impl_servant = process_impl;
            
            CdmwPlatformMngt::Process_var process = process_impl->_this();
            
            InitUtils::init_platform_interface(orb, argc, argv, process.in());
            //  From now on, a platform supervisor may initiate a call to 
            //  process->init() method. 
            //  process->init() call is blocked with a condition until CDMW
            //  init has completed?
         }

         //
         // Get the process name. This may raise CORBA::BAD_INV_ORDER
         std::string process_name = InitUtils::get_cdmw_process_name();
         //
         // Get the application name. This may raise CORBA::BAD_INV_ORDER
         std::string application_name = InitUtils::get_cdmw_application_name();  
        
         // Retreive Repository object reference. This may raise CORBA::BAD_PARAM
         // or CORBA::BAD_INV_ORDER
         CdmwNamingAndRepository::Repository_var repository 
            = InitUtils::get_cdmw_repository();
   ]]>
   	 
   <xsl:if test="$hasFaultTolerance">
   <![CDATA[
         // *****************************************************************
         // ***********  setup FaultTolerance ReplicationManager  ***********
         // *****************************************************************

         // Get the reference of the Replication Manager and register it in the ORB
         CORBA::Object_var objref;
         const char* replication_manager_name = "ReplicationManager";
			objref =  InitUtils::get_service(replication_manager_name);  

         CdmwFT::ReplicationManager_var replication_manager = CdmwFT::ReplicationManager::_narrow(objref.in());

	      try 
         {
             orb->register_initial_reference("ReplicationManager", replication_manager.in());      
         }
         catch (const CORBA::SystemException& ex)
         {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                          ex,
                          "Cannot register ReplicationManager." );
          }
          catch (const CORBA::ORB::InvalidName& )
          {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO),
								  "InvalidName => Cannot register ReplicationManager.");
          }
	 ]]>
	</xsl:if>

     <xsl:variable name="hasFaultToleranceServer">
         <xsl:choose>
             <xsl:when test="boolean($servicesNode/fault-tolerance/@server)">
                 <xsl:value-of select="$servicesNode/fault-tolerance/@server"/>
             </xsl:when>
             <xsl:otherwise>
                 <xsl:value-of select="'no'"/>
             </xsl:otherwise>
         </xsl:choose>
     </xsl:variable>

      <xsl:choose>
         <xsl:when test="$hasFaultToleranceServer='yes'">

      <![CDATA[
         // *****************************************************************
         // *************  setup FaultTolerance LocationManager  ************
         // *****************************************************************
         
         // Location information shall be obtain from supervision
         ::FT::Location loc;
         loc.length(3);

         loc[0].id = CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
         loc[0].kind = "hostname";
         loc[1].id = application_name.c_str();
         loc[1].kind = "applicationname";
         loc[2].id = process_name.c_str();
         loc[2].kind = "processname";
         
	std::string location_str 
	   = Cdmw::NamingAndRepository::NamingInterface::to_string(loc);
	std::cout << "setup FaultTolerance LocationManager for location: " << location_str << std::endl;
			
         // create a reference for the PrimaryBackupGroupRepository
         ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl * primary_backup_repository
             = ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::get_instance();
	     

      ]]>
      <xsl:choose>
      <xsl:when test="$hasDataStore='true'">

      <![CDATA[

      // get the fault notifier reference
      ::FT::FaultNotifier_var fault_notifier = replication_manager->get_fault_notifier();

      // create the DataStoreTimeoutHandler
      Cdmw::FT::LocationDataStoreTimeoutHandler* locationDataStoreTimeoutHandler
          = new Cdmw::FT::LocationDataStoreTimeoutHandler(fault_notifier.in());

      // Register the timeoutHandler into the DataStoreBase
      Cdmw::FT::DataStoreBase::Register_Timeout_Handler(locationDataStoreTimeoutHandler);


      // FaultTolerance use datastore
      CORBA::ULong datastore_id;
      std::string multicast_port;
      std::string multicast_domain;
      std::string multicast_IP_address;
      int max_transaction_in_progress;
      int max_transaction_done;
      OsSupport::OS::Timeval cohort_timeout;
      OsSupport::OS::Timeval coordinator_timeout;
      OsSupport::OS::Timeval freeze_timeout;
      long number_of_scheduler_thread;
      long chunk_size;
      long freeze_size;

      long dataStore_inc=0;
      ]]>

      <xsl:variable name="dataStoresKind" select="$servicesNode/fault-tolerance/datastores/@kind"/>
      <xsl:variable name="countDatastoreNodes" select="count($datastoreNodes)"/>

      <xsl:variable name="dataStoreOccurrence">
         <xsl:call-template name="count.datastore.occurrence">
            <xsl:with-param name="_datastoreNodes" select="$datastoreNodes"/>
            <xsl:with-param name="_occurrences" select="$countDatastoreNodes"/>
            <xsl:with-param name="_datastoreOccurrences" select="0"/>
            <xsl:with-param name="_index_datastore" select="1"/>
         </xsl:call-template>
      </xsl:variable>
      
      <![CDATA[long localDataStoreInfosSize = ]]><xsl:value-of select="$dataStoreOccurrence"/><![CDATA[;
      CdmwFT::StateTransfer::LocalDataStoreInfos_var localDataStoreInfos = new CdmwFT::StateTransfer::LocalDataStoreInfos();
      localDataStoreInfos->length(localDataStoreInfosSize);
      ]]>


      <!-- 
          get the kind of datastore (IIOP or MIOP)
      --> 
      <xsl:call-template name="datastore.occurrence">
         <xsl:with-param name="_datastoreNodes" select="$datastoreNodes"/>
	 <xsl:with-param name="_occurrencesInProgress" select="1"/>
	 <xsl:with-param name="_occurrencesInProgressForThisDataStore" select="1"/>
         <xsl:with-param name="_occurrences" select="$dataStoreOccurrence"/>
         <xsl:with-param name="_dataStoresKind" select="$dataStoresKind"/>
         <xsl:with-param name="_index_datastore" select="1"/>
      </xsl:call-template>


      <![CDATA[
         std::string string_arg = ::Cdmw::OsSupport::OS::get_option_value( argc, argv, "--HotRestart");
	 bool cold_restart= true;
	 if (strcmp(string_arg.c_str(), "yes")==0)
	     cold_restart = false;

         ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl * statefull_primary_backup_admin
             = new ::Cdmw::FT::Location::StatefullPrimaryBackupAdmin_impl(orb, 
                                                                          CDMW_rootPOA.in(), 
                                                                          loc, 
                                                                          primary_backup_repository, 
									  cold_restart);
         
         ::CdmwFT::Location::StatefullPrimaryBackupAdmin_var statefull_primary_backup_admin_ref 
             = statefull_primary_backup_admin->_this();

         replication_manager->register_statefull_location(loc, 
	                                                  statefull_primary_backup_admin_ref.in(),
	                                                  localDataStoreInfos.in());
      ]]>

      </xsl:when>
      <xsl:otherwise>
      
      <![CDATA[
         ::Cdmw::FT::Location::PrimaryBackupAdmin_impl * primary_backup_admin
             = new ::Cdmw::FT::Location::PrimaryBackupAdmin_impl(orb, 
                                                                  CDMW_rootPOA.in(), 
                                                                  loc, 
                                                                  primary_backup_repository);
         
         ::CdmwFT::Location::PrimaryBackupAdmin_var primary_backup_admin_ref 
             = primary_backup_admin->_this();

         replication_manager->register_location(loc, 
	                                        primary_backup_admin_ref.in());
      ]]>
      
      </xsl:otherwise>
      </xsl:choose>
      </xsl:when>
   </xsl:choose>

   <xsl:if test="$hasRepositoryInterface">
      <![CDATA[
         // *****************************************************************
         // *****************  INIT of RepositoryInterface ******************
         // *****************************************************************
         std::string default_domaine_name("");
         try {
            default_domaine_name  
               = xml_data->getServiceAttributeValue("repository-interface", "default-domain-name");
	      } catch (const Cdmw::BadParameterException& ex) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                           CORBA::COMPLETED_NO),
                          ex.what() );       
         } catch (const Cdmw::CdmwInit::XMLErrorException & ex) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                           CORBA::COMPLETED_NO),
                          ex.what() );
         } catch (const OutOfMemoryException & ex) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                          CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                           CORBA::COMPLETED_NO),
                          ex.what() );
         } 

         try {
            Cdmw::NamingAndRepository::RepositoryInterface::init
               (default_domaine_name, repository.in());
         } catch (const CdmwNamingAndRepository::NoNameDomain & ) {
            // Default NameDomain is not a NameDomain! 
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMNameDomainNotFound,
                                           CORBA::COMPLETED_NO),
                          default_domaine_name + " is not a NameDomain!" );
         } catch (const CdmwNamingAndRepository::InvalidName &  ex) {
            // Default NameDomain name is ill-formed! 
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidDefaultDomainName,
                                           CORBA::COMPLETED_NO),
                          default_domaine_name + " is ill-formed :" + ex.reason.in() );
         } catch (const CORBA::SystemException & ex) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                          ex,
                          "Cannot initialise RepositoryInterface." );
         }
      ]]>
   </xsl:if>

   <xsl:if test="$hasTrace">
      <![CDATA[
         #ifdef CDMW_TRACES_ENABLED
         
         // *****************************************************************
         // *********         Init The CDMW Trace Library          *********
         // ***************************************************************** 
         {
            std::string res;
            size_t flushing_time;
            size_t nb_FlushArea;
            size_t size_FlushArea;
            std::string collectorName;
            std::vector<std::string> collectorNameList;
            
            try {
               try {
                  res = xml_data->getServiceAttributeValue("trace", "flushing-time"); //FlushAreaMngr::DEFAULT_FLUSHING_TIME;
                  flushing_time = atoi(res.c_str());
                  res = xml_data->getServiceAttributeValue("trace", "flush-area-nb"); //FlushAreaMngr::DEFAULT_AREA_NB;
                  nb_FlushArea = atoi(res.c_str());
                  res = xml_data->getServiceAttributeValue("trace", "flush-area-sz"); //FlushAreaMngr::DEFAULT_AREA_SIZE;
                  size_FlushArea = atoi(res.c_str());
               } catch (const Cdmw::BadParameterException& ex) {
                  CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                 CORBA::COMPLETED_NO),
                                ex.what() );
               }

               try  {
                  collectorName = xml_data->getServiceAttributeValue("trace", "collector-name");
                  collectorNameList.push_back(collectorName);
               } catch (const Cdmw::BadParameterException& ) {
                  // do nothing : parameter is optional
               }
            } catch (const Cdmw::CdmwInit::XMLErrorException & ex) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                             CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                              CORBA::COMPLETED_NO),
                             ex.what() );
            } catch (const OutOfMemoryException & ex) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                             CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                              CORBA::COMPLETED_NO),
                             ex.what() );
            } 

            // May raise CORBA::SystemException
            Trace::InitUtils::init_trace_library(CDMW_rootPOA.in(),
                                                 application_name,
                                                 process_name,
                                                 flushing_time,nb_FlushArea,size_FlushArea,
                                                 collectorNameList);

            CDMW_TRACE_ACTIVE_FLUSHING();           
         }

         #endif // CDMW_TRACES_ENABLED
      ]]>
   </xsl:if>

   <xsl:if test="$hasPerformance">
      <![CDATA[
         #ifdef CDMW_PERFORMANCE_ENABLED
         
         // *****************************************************************
         // *********         Init The CDMW Performance Library    *********
         // ***************************************************************** 
         {
            std::string performanceFileName;
            try {
               performanceFileName = xml_data->getServiceAttributeValue("performance", "result-file-name");
            } catch (const Cdmw::BadParameterException& ex ) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                             CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                              CORBA::COMPLETED_NO),
                             ex.what() );
            } catch (const Cdmw::CdmwInit::XMLErrorException & ex ) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                             CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                              CORBA::COMPLETED_NO),
                             ex.what() );
            } catch (const OutOfMemoryException & ex ) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                             CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                              CORBA::COMPLETED_NO),
                             ex.what() );
            }
	
            // May raise CORBA::SystemException
            Performance::InitUtils::init_performance_library(CDMW_rootPOA.in(),
                                                             application_name,
                                                             process_name,
                                                             performanceFileName);
         }
         #endif // CDMW_PERFORMANCE_ENABLED
      ]]>
   </xsl:if>

   <xsl:if test="$hasTime">
      <![CDATA[
         #ifdef CDMW_TIME_SERVICE_ENABLED
         
         // *****************************************************************
         // *********         Init The CDMW Time Service           *********
         // ***************************************************************** 
         {
            // definition of Corbaname URL :
            //    Note : Time Service port is set to 2001
            std::string timeServiceCorbaname =
               "corbaname::localhost:2001#COS/TimeService/TimeService";
            std::string timerEventServiceCorbaname =
               "corbaname::localhost:2001#COS/TimeService/TimerEventHandler";
               
            // definitions of services
            std::string timeServiceName = "TimeService";
            std::string timerEventServiceName = "TimerEventService";
      
            CORBA::Object_var timeServiceObj = CORBA::Object::_nil();
               
            try {
               timeServiceObj = orb->string_to_object(timeServiceCorbaname.c_str());
            } catch(...) {
               // Do nothing (ignore : time service is not started)
               // TODO: Add trace logs here
            }
                      
            if (CORBA::is_nil(timeServiceObj.in())) {
               // Do nothing (ignore : time service is not started)
               // TODO: Add trace logs here
            } else {
               try {
                  // register TimeService as service for ORB
                  // to be reached by resolve_initial_reference orb api
                     
                  Cdmw::OrbSupport::OrbSupport::register_initial_reference (orb,
                                                                             timeServiceName, 
                                                                             timeServiceObj.in());
               } catch(...) {
                  // Do nothing (ignore : time service is not started)
                  // TODO: Add trace logs here
               }
            }

            CORBA::Object_var timerEventServiceObj = CORBA::Object::_nil();
      
            try {
               timerEventServiceObj = orb->string_to_object(timerEventServiceCorbaname.c_str());
            } catch(...) {
               // Do nothing (ignore : time service is not started)
               // TODO: Add trace logs here
            }
                      
            if (CORBA::is_nil(timerEventServiceObj.in())) {
               // Do nothing (ignore : time service is not started)
               // TODO: Add trace logs here
            } else {
               try {
                  // register TimeService as service for ORB
                  // to be reached by resolve_initial_reference orb api
                       
                  Cdmw::OrbSupport::OrbSupport::register_initial_reference (orb, 
                                                                             timerEventServiceName, 
                                                                             timerEventServiceObj.in());
               } catch(...) {
                  // Do nothing (ignore : time service is not started)
                  // TODO: Add trace logs here
               }
            }
         }
         
         #endif // CDMW_TIME_SERVICE_ENABLED
      ]]>
   </xsl:if>

   <xsl:if test="$hasLifecycle">
      <![CDATA[
         // *****************************************************************
         // *********  Create CDMW Factories POA (persistent POA)  *********
         // ***************************************************************** 
         // Raises CORBA::SystemException
         PortableServer::POA_var factories_poa 
            = Cdmw::LifeCycle::InitUtils::create_cdmw_LifeCycle_POA(CDMW_rootPOA.in());

         // Create CDMW factories
        
         using namespace Cdmw::LifeCycle;
         CosNaming::NamingContext_var nc_root_context = CosNaming::NamingContext::_nil();
         try {
            nc_root_context
               = repository->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
         } catch (const CdmwNamingAndRepository::Repository::NoRootContext& ) {
            std::string s (CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
            s += " is not a Root Context!";
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALCdmwRepositoryError,
                                          CORBA::COMPLETED_NO),
                          s );
         } catch (const CORBA::SystemException & ex) {
            CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                          ex );
         }
         // May raise a CORBA::SystemException 
         Cdmw::NamingAndRepository::NamingInterface root_context (nc_root_context.in());

      ]]>

      <xsl:if test="$hasHomeImpl='true'">
         using namespace Cdmw::CCM::Container;
         // Register Valuetype  factories
         Cdmw::CCM::CIF::InitUtils::register_valuetype_factories(orb);
         Cdmw::CCM::Container::InitUtils::register_valuetype_factories(orb);
      </xsl:if>

      <xsl:for-each select="$frameworkNodes">
         <xsl:variable name="frameworkIdref" select="@idref"/>

         <!--
            Get the matching node /cdmw/lifecycle-frameworks/lifecycle-framework.
         -->
         <xsl:variable name="lifecycleFrameworkNode" select="$lifecycleFrameworkNodes[@id = $frameworkIdref]"/>
         <xsl:variable name="isHomeImpl" select="boolean($lifecycleFrameworkNode/home-impl)"/>

         <!--
            Get the factory classname.
         -->
         <!--
         <xsl:variable name="factoryClassname" select="$lifecycleFrameworkNode/*/@classname"/>
         -->
         <xsl:variable name="factoryClassname">
            <xsl:choose>
               <xsl:when test="$isHomeImpl">
                  <xsl:call-template name="buildHomeImplClassname">
                     <xsl:with-param name="_homeImplNode" select="$lifecycleFrameworkNode/home-impl"/>
                  </xsl:call-template>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="$lifecycleFrameworkNode/*/@classname"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:variable>

         <!--
            Format the class name for output.
         -->
         <xsl:variable name="cppFactoryClassname">
            <xsl:call-template name="replaceCharsInString">
               <xsl:with-param name="_stringIn" select="$factoryClassname"/>
               <xsl:with-param name="_charsIn" select="$xmlSep"/>
               <xsl:with-param name="_charsOut" select="$cppSep"/>
            </xsl:call-template>
         </xsl:variable>

         <!--
            Get the strategy.
         -->
         <xsl:variable name="strategy">
            <xsl:variable name="category" select="$lifecycleFrameworkNode/*/@category"/>
            <xsl:choose>
               <xsl:when test="boolean($category)">
                  <xsl:value-of select="$category"/>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="'USER_DEFINED'"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:variable>

         <!--
            Register the home allocator.
         -->
         <xsl:if test="$isHomeImpl">
            // Register <xsl:value-of select="$cppFactoryClassname"/> HomeAllocator
            {
               typedef <![CDATA[HomeAllocator<]]><xsl:value-of select="$cppFactoryClassname"/><![CDATA[> CdmwHome_Allocator;]]>
               HomeAllocatorRegistry::Register("<xsl:value-of select="$factoryClassname"/>",
                                               <![CDATA[&CdmwHome_Allocator::TheAllocator);]]>
            }
         </xsl:if>

         <!--
            Strategy selection.
         -->
         <xsl:choose>
            <!--
               ENTITY.
            -->
            <xsl:when test="$strategy = 'ENTITY'">
               <xsl:choose>
                  <xsl:when test="$isHomeImpl">
                     <!--
                        TODO: Add component server code here!
                     -->
                  </xsl:when>
                  <xsl:otherwise>
                     // *****************************************************************
                     // ****** <xsl:value-of select="$cppFactoryClassname"/>  ******
                     // ***************************************************************** 
                     {
                        std::string full_name_domain_name;
                        std::string full_factory_name;
                        <![CDATA[
                           typedef std::auto_ptr<Cdmw::OrbSupport::POAThreadingPolicy> threading_auto_ptr;
                           threading_auto_ptr threading_policy(0);
                           // Cdmw::LifeCycle::POAThreadingPolicy * threading_policy = 0;
                           
                           int i = 0;
                        ]]>

                        <xsl:if test="@occurrences > 0">
                           try {
                              <![CDATA[full_name_domain_name =  xml_data->getFrameworkAttributeValue]]>
                                 ("<xsl:value-of select="$frameworkIdref"/>", i, "domain-name");
                              // TODO: If full_name_domain_name and factory_name_domain_name are empty use
                              // default name domain
                              <![CDATA[full_factory_name = xml_data->getFrameworkAttributeValue]]>
                                 ("<xsl:value-of select="$frameworkIdref"/>", i, "full-factory-name");
                      
                           <xsl:choose>
                              <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                                 threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                                 threading_policy = thrdptr;
                                 // threading_policy = threading_auto_ptr(new SingleThreadPolicy());
                              </xsl:when>
                              <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                                 std::string xmlThreadingPolicy("");
                                 std::string xmlThreadingAttribute("");
                                 <![CDATA[std::string xmlThreadingAttributeValue = xml_data->getFrameworkMultithreadAttribute]]>
                                    ("<xsl:value-of select="$frameworkIdref"/>", i, xmlThreadingPolicy, xmlThreadingAttribute);
                      
                                 if (!xmlThreadingPolicy.compare("serialize")) {
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                                    threading_policy = thrdptr; 
                                    // threading_policy = threading_auto_ptr(new SingleThreadPolicy());
                                 } else if (!xmlThreadingPolicy.compare("thread-per-request")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-per-request policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerRequestPolicy());
                                    threading_policy = thrdptr;
                                    // threading_policy = threading_auto_ptr(new ThreadPerRequestPolicy());
#endif
                                 } else if (!xmlThreadingPolicy.compare("thread-pool")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-pool policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                                    size_t thread_pool_size = atoi(xmlThreadingAttributeValue.c_str());
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPoolPolicy(thread_pool_size));
                                    threading_policy = thrdptr;
                                    // threading_policy = threading_auto_ptr(new ThreadPoolPolicy(thread_pool_size));
#endif
                                 }
                              </xsl:when>
                           </xsl:choose>
                           
                           <![CDATA[
                              } catch (const std::bad_alloc & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                               CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const OutOfMemoryException & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                               CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const Cdmw::BadParameterException & ex ) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const Cdmw::CdmwInit::XMLErrorException & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                                 CORBA::COMPLETED_NO),
                                               ex.what() );
                              }
                           ]]>

                           typedef NamedObjectsFactory_initUtil Util;
                           typedef NamedObjectsFactoryAllocatorBase AllocatorType;

                           // Get the factory Allocator
                           // TODO: Use a user-provided name rather than a naming convention!
                           <![CDATA[AllocatorType & allocator =]]>
                              <xsl:value-of select="$cppFactoryClassname"/>_Allocator::TheAllocator;

                           try {
                              Util::setup_named_object_factory(allocator,
                                                               orb,           // ORB
                                                               factories_poa.in(),    // PARENT POA
                                                               repository.in(),
                                                               root_context,
                                                               full_name_domain_name,
                                                               full_factory_name,
                                                               "", // TODO:
                                                               *threading_policy,
                                                               argc, argv
                                 );
                           <![CDATA[
                              } catch (const Cdmw::OutOfMemoryException & ex) {
                                 std::string s ("Could not allocate memory for ");
                                 s += full_factory_name;
                                 s += "\n";
                                 s += ex.what();
                                 
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                               CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                                CORBA::COMPLETED_NO),
                                               s );
                              } catch (const Cdmw::BadParameterException & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMLifeCycleFrameworkInit,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const Cdmw::InternalErrorException & ex ) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                                               CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                                               CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const CORBA::SystemException & ex) {
                                 CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                                               ex );
                              }
                           ]]>
                        </xsl:if>
                     }
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <!--
               UNMANAGED_ENTITY.
            -->
            <xsl:when test="$strategy = 'UNMANAGED_ENTITY'">
               // *****************************************************************
               // ****** <xsl:value-of select="$cppFactoryClassname"/>  ******
               // *****************************************************************
               {
                  std::string full_name_domain_name;
                  std::string full_factory_name;
                  <![CDATA[
                     typedef std::auto_ptr<Cdmw::OrbSupport::POAThreadingPolicy> threading_auto_ptr;
                     threading_auto_ptr threading_policy(0);
                     // Cdmw::LifeCycle::POAThreadingPolicy * threading_policy = 0;
            
                     int i = 0;
                  ]]>

                  <xsl:if test="@occurrences > 0">
                     try {
                        <![CDATA[full_name_domain_name = xml_data->getFrameworkAttributeValue]]>
                           ("<xsl:value-of select="$frameworkIdref"/>", i, "domain-name");
                        // TODO: If full_name_domain_name and factory_name_domain_name are empty use
                        // default name domain
                        <![CDATA[full_factory_name = xml_data->getFrameworkAttributeValue]]>
                           ("<xsl:value-of select="$frameworkIdref"/>", i, "full-factory-name");
                
                     <xsl:choose>
                        <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                           threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                           threading_policy = thrdptr;
                           // threading_policy = threading_auto_ptr(new SingleThreadPolicy());
                        </xsl:when>
                        <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                           std::string xmlThreadingPolicy("");
                           std::string xmlThreadingAttribute("");
                           <![CDATA[std::string xmlThreadingAttributeValue = xml_data->getFrameworkMultithreadAttribute]]>
                              ("<xsl:value-of select="$frameworkIdref"/>", i, xmlThreadingPolicy, xmlThreadingAttribute);
                
                           if (!xmlThreadingPolicy.compare("serialize")) {
                              threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                              threading_policy = thrdptr;
                              // threading_policy = threading_auto_ptr(new SingleThreadPolicy());
                           } else if (!xmlThreadingPolicy.compare("thread-per-request")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-per-request policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                              threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerRequestPolicy());
                              threading_policy = thrdptr;
                              // threading_policy = threading_auto_ptr(new ThreadPerRequestPolicy());
#endif
                           } else if (!xmlThreadingPolicy.compare("thread-pool")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-pool policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                              size_t thread_pool_size = atoi(xmlThreadingAttributeValue.c_str());
                              threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPoolPolicy(thread_pool_size));
                              threading_policy = thrdptr;
                              // threading_policy = threading_auto_ptr(new ThreadPoolPolicy(thread_pool_size));
#endif
                           }
                        </xsl:when>
                     </xsl:choose>

                     <![CDATA[
                        } catch (const std::bad_alloc & ex) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                         CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const OutOfMemoryException & ex) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                         CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const Cdmw::BadParameterException & ex ) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                         CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const Cdmw::CdmwInit::XMLErrorException & ex) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                         CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        }
                     ]]>

                     typedef NamedObjectsFactory_initUtil Util;
                     typedef NamedObjectsFactoryAllocatorBase AllocatorType;

                     // Get the factory Allocator
                     // TODO: Use a user-provided name rather than a naming convention!
                     <![CDATA[AllocatorType & allocator =]]>
                        <xsl:value-of select="$cppFactoryClassname"/>_Allocator::TheAllocator;

                     try {
                        Util::setup_named_object_factory(allocator,
                                                         orb,           // ORB
                                                         factories_poa.in(),    // PARENT POA
                                                         repository.in(),
                                                         root_context,
                                                         full_name_domain_name,
                                                         full_factory_name,
                                                         "", // TODO:
                                                         *threading_policy,
                                                         argc, argv
                           );
                     <![CDATA[
                        } catch (const Cdmw::OutOfMemoryException & ex ) {
                           std::string s ("Could not allocate memory for ");
                           s += full_factory_name;
                           s += "\n";
                           s += ex.what();

                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                         CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                          CORBA::COMPLETED_NO),
                                         s );
                        } catch (const Cdmw::BadParameterException & ex) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                         CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMLifeCycleFrameworkInit,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const Cdmw::InternalErrorException & ex ){
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                                         CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                                         CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const CORBA::SystemException & ex ) {
                           CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                                         ex );
                        }
                     ]]>
                  </xsl:if>
               }
            </xsl:when>
            <!--
               SESSION.
            -->
            <xsl:when test="$strategy = 'SESSION'">
               <xsl:choose>
                  <xsl:when test="$isHomeImpl">
                     <!--
                        TODO: Add component server code here!
                     -->
                  </xsl:when>
                  <xsl:otherwise>
                     // *****************************************************************
                     // ****** <xsl:value-of select="$cppFactoryClassname"/>  ******
                     // *****************************************************************
                     {
                        std::string full_factory_name;
                        <![CDATA[
                           typedef std::auto_ptr<Cdmw::OrbSupport::POAThreadingPolicy> threading_auto_ptr;
                           threading_auto_ptr threading_policy(0);
                           // Cdmw::LifeCycle::POAThreadingPolicy * threading_policy = 0;
                          
                           int i = 0;
                        ]]>

                        <xsl:if test="@occurrences > 0">
                           try {
                              <![CDATA[full_factory_name = xml_data->getFrameworkAttributeValue]]>
                                 ("<xsl:value-of select="$frameworkIdref"/>", i, "full-factory-name");
                      
                           <xsl:choose>
                              <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                                 threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                                 threading_policy = thrdptr;
                                 // threading_policy = threading_auto_ptr(new SingleThreadPolicy());
                              </xsl:when>
                              <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                                 std::string xmlThreadingPolicy("");
                                 std::string xmlThreadingAttribute("");	
                                 <![CDATA[std::string xmlTreadingAttributeValue = xml_data->getFrameworkMultithreadAttribute]]>
                                    ("<xsl:value-of select="$frameworkIdref"/>", i, xmlThreadingPolicy, xmlThreadingAttribute);
                      
                                 if (!xmlThreadingPolicy.compare("serialize")) {
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                                    threading_policy = thrdptr; 
                                    // threading_policy = threading_auto_ptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                                 } else if (!xmlThreadingPolicy.compare("thread-per-request")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-per-request policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerRequestPolicy());
                                    threading_policy = thrdptr; 
                                    // threading_policy = threading_auto_ptr(new ThreadPerRequestPolicy());
#endif
                                 } else if (!xmlThreadingPolicy.compare("thread-pool")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-pool policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                                    size_t thread_pool_size = atoi(xmlTreadingAttributeValue.c_str());
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPoolPolicy(thread_pool_size));
                                    threading_policy = thrdptr; 
                                    // threading_policy = threading_auto_ptr(new ThreadPoolPolicy(thread_pool_size));
#endif
                                 }
                              </xsl:when>
                           </xsl:choose>

                           <![CDATA[
                              } catch (const std::bad_alloc & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                               CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const OutOfMemoryException & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const Cdmw::BadParameterException & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );       
                              } catch (const Cdmw::CdmwInit::XMLErrorException & ex ) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              }
                           ]]>

                           typedef AnonymousObjectsFactory_initUtil Util;
                           typedef AnonymousObjectsFactoryAllocatorBase AllocatorType;

                           // Get the factory Allocator
                           // TODO: Use a user-provided name rather than a naming convention!
                           <![CDATA[AllocatorType & allocator =]]>
                              <xsl:value-of select="$cppFactoryClassname"/>_Allocator::TheAllocator;

                           try {
                              Util::setup_anonymous_object_factory(allocator,
                                                                   orb,           // ORB
                                                                   factories_poa.in(),    // PARENT POA
                                                                   repository.in(),
                                                                   root_context,
                                                                   full_factory_name,
                                                                   "", // TODO:
                                                                   *threading_policy,
                                                                   argc, argv
                                 );
                            <![CDATA[
                              } catch (const Cdmw::OutOfMemoryException & ex ) {
                                 std::string s ("Could not allocate memory for ");
                                 s += full_factory_name;
                                 s += "\n";
                                 s += ex.what();

                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                               CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                                CORBA::COMPLETED_NO),
                                               s );
                              } catch (const Cdmw::BadParameterException & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMLifeCycleFrameworkInit,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const Cdmw::InternalErrorException & ex ) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                                               CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                                               CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const CORBA::SystemException & ex) {
                                 CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                                               ex );
                              }
                            ]]>
                        </xsl:if>
                     }
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <!--
               SERVICE.
            -->
            <xsl:when test="$strategy = 'SERVICE'">
               <xsl:choose>
                  <xsl:when test="$isHomeImpl">
                     <!--
                        TODO: Add component server code here!
                     -->
                  </xsl:when>
                  <xsl:otherwise>
                     // *****************************************************************
                     // ****** <xsl:value-of select="$cppFactoryClassname"/>  ******
                     // *****************************************************************
                     {
                        std::string full_factory_name;
                        <![CDATA[
                           typedef std::auto_ptr<Cdmw::OrbSupport::POAThreadingPolicy> threading_auto_ptr;
                           threading_auto_ptr threading_policy(0);
                           // Cdmw::LifeCycle::POAThreadingPolicy * threading_policy = 0;
                              
                           int i = 0;
                        ]]>

                        <xsl:if test="@occurrences > 0">
                           try {
                              <![CDATA[full_factory_name = xml_data->getFrameworkAttributeValue]]>
                                 ("<xsl:value-of select="$frameworkIdref"/>", i, "full-factory-name");

                           <xsl:choose>
                              <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                                 threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                                 threading_policy = thrdptr;
                                 // threading_policy = threading_auto_ptr(new SingleThreadPolicy());
                              </xsl:when>
                              <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                                 std::string xmlThreadingPolicy("");
                                 std::string xmlThreadingAttribute("");
                                 <![CDATA[std::string xmlThreadingAttributeValue = xml_data->getFrameworkMultithreadAttribute]]>
                                    ("<xsl:value-of select="$frameworkIdref"/>", i, xmlThreadingPolicy, xmlThreadingAttribute);
                      
                                 if (!xmlThreadingPolicy.compare("serialize")) {
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                                    threading_policy = thrdptr; 
                                    // threading_policy = threading_auto_ptr(new SingleThreadPolicy());
                                 } else if (!xmlThreadingPolicy.compare("thread-per-request")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-per-request policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerRequestPolicy());
                                    threading_policy = thrdptr; 
                                    // threading_policy = threading_auto_ptr(new ThreadPerRequestPolicy());
#endif
                                 } else if (!xmlThreadingPolicy.compare("thread-pool")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-pool policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                                    size_t thread_pool_size = atoi(xmlThreadingAttributeValue.c_str());
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPoolPolicy(thread_pool_size));
                                    threading_policy = thrdptr; 
                                    // threading_policy = threading_auto_ptr(new Cdmw::OrbSupport::ThreadPoolPolicy(thread_pool_size));
#endif
                                 }
                              </xsl:when>
                           </xsl:choose>

                           <![CDATA[
                              } catch (const std::bad_alloc & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                               CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const OutOfMemoryException & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                               CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const Cdmw::BadParameterException & ex ) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const Cdmw::CdmwInit::XMLErrorException & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              }
                           ]]>

                           typedef AnonymousObjectsFactory_initUtil Util;
                           typedef AnonymousObjectsFactoryAllocatorBase AllocatorType;

                           // Get the factory Allocator
                           // TODO: Use a user-provided name rather than a naming convention!
                           <![CDATA[AllocatorType & allocator =]]>
                              <xsl:value-of select="$cppFactoryClassname"/>_Allocator::TheAllocator;

                           try {
                              Util::setup_anonymous_object_factory(allocator,
                                                                   orb,           // ORB
                                                                   factories_poa.in(),    // PARENT POA
                                                                   repository.in(),
                                                                   root_context,
                                                                   full_factory_name,
                                                                   "", // TODO:
                                                                   * threading_policy,
                                                                   argc, argv
                                 );
                           <![CDATA[
                              } catch (const Cdmw::OutOfMemoryException & ex ) {
                                 std::string s ("Could not allocate memory for ");
                                 s += full_factory_name;
                                 s += "\n";
                                 s += ex.what();

                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                               CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                                CORBA::COMPLETED_NO),
                                               s );
                              } catch (const Cdmw::BadParameterException & ex) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                               CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMLifeCycleFrameworkInit,
                                                                CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const Cdmw::InternalErrorException & ex ) {
                                 CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                                               CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                                               CORBA::COMPLETED_NO),
                                               ex.what() );
                              } catch (const CORBA::SystemException & ex ) {
                                 CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                                               ex );
                              }
                           ]]>
                        </xsl:if>
                     }
                  </xsl:otherwise>
               </xsl:choose>
            </xsl:when>
            <xsl:when test="$strategy = 'PROCESS'">
               <!--
                  TODO: Add component server code here!
               -->
            </xsl:when>
            <!--
               USER_DEFINED.
            -->
            <xsl:when test="$strategy = 'USER_DEFINED'">
               <xsl:variable name="isNamed" select="$factoryNodes[@idl-interface = $lifecycleFrameworkNode/@factory-idref]/@kind = 'NAMED'"/>
               <xsl:variable name="namedString">
                  <xsl:choose>
                     <xsl:when test="$isNamed">
                        <xsl:text>named</xsl:text>
                     </xsl:when>
                     <xsl:otherwise>
                        <xsl:text>anonymous</xsl:text>
                     </xsl:otherwise>
                  </xsl:choose>
               </xsl:variable>
               <xsl:variable name="cppFactoryAllocatorClassname">
                  <xsl:call-template name="replaceCharsInString">
                     <xsl:with-param name="_stringIn" select="$lifecycleFrameworkNode/user-factory-impl/factory-allocator/@classname"/>
                     <xsl:with-param name="_charsIn" select="$xmlSep"/>
                     <xsl:with-param name="_charsOut" select="$cppSep"/>
                  </xsl:call-template>
               </xsl:variable>

               // *****************************************************************
               // ******                User defined factory                 ******
               // *****************************************************************
               {
                  std::string full_factory_name;
                  <xsl:if test="$isNamed">std::string full_name_domain_name;</xsl:if>
                  <![CDATA[
                     typedef std::auto_ptr<Cdmw::OrbSupport::POAThreadingPolicy> threading_auto_ptr;
                     threading_auto_ptr threading_policy(0);
                     // Cdmw::LifeCycle::POAThreadingPolicy * threading_policy = 0;
                   
                     int i = 0;
                  ]]>

                  <xsl:if test="@occurrences > 0">
                     try {
                     <xsl:choose>
                        <xsl:when test="$isNamed">
                           <![CDATA[full_name_domain_name =  xml_data->getFrameworkAttributeValue]]>
                              ("<xsl:value-of select="$frameworkIdref"/>", i, "domain-name");
                           // TODO: If full_name_domain_name and factory_name_domain_name are empty use
                           // default name domain
                           <![CDATA[full_factory_name = xml_data->getFrameworkAttributeValue]]>
                              ("<xsl:value-of select="$frameworkIdref"/>", i, "full-factory-name");
                        </xsl:when>
                        <xsl:otherwise>
                           <![CDATA[full_factory_name = xml_data->getFrameworkAttributeValue]]>
                              ("<xsl:value-of select="$frameworkIdref"/>", i, "full-factory-name");
                        </xsl:otherwise>
                     </xsl:choose>

                     <xsl:choose>
                        <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'SERIALIZE'">
                           threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                           threading_policy = thrdptr;
                           // threading_policy = threading_auto_ptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                        </xsl:when>
                        <xsl:when test="$lifecycleFrameworkNode/*/@threading = 'MULTITHREAD'">
                           std::string xmlThreadingPolicy("");
                           std::string xmlThreadingAttribute("");
                           <![CDATA[std::string xmlThreadingAttributeValue = xml_data->getFrameworkMultithreadAttribute]]>
                              ("<xsl:value-of select="$frameworkIdref"/>", i, xmlThreadingPolicy, xmlThreadingAttribute);
                
                           if (!xmlThreadingPolicy.compare("serialize")) {
                              threading_auto_ptr thrdptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                              threading_policy = thrdptr; 
                              // threading_policy = threading_auto_ptr(new Cdmw::OrbSupport::SingleThreadPolicy());
                           } else if (!xmlThreadingPolicy.compare("thread-per-request")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-per-request policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                              threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerRequestPolicy());
                              threading_policy = thrdptr; 
                              // threading_policy = threading_auto_ptr(new ThreadPerRequestPolicy());
#endif
                           } else if (!xmlThreadingPolicy.compare("thread-pool")) {
#if CDMW_ORB_VDR == tao
                                    // TAO doesn't support thread-pool policy with a persistent POA
                                    threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPerConnectionPolicy());
                                    threading_policy = thrdptr;
#else
                              size_t thread_pool_size = atoi(xmlThreadingAttributeValue.c_str());
                              threading_auto_ptr thrdptr(new Cdmw::OrbSupport::ThreadPoolPolicy(thread_pool_size));
                              threading_policy = thrdptr; 
                              // threading_policy = threading_auto_ptr(new ThreadPoolPolicy(thread_pool_size));
#endif
                           }
                        </xsl:when>
                     </xsl:choose>

                     <![CDATA[
                        } catch (const std::bad_alloc & ex) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                         CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const OutOfMemoryException & ex) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                         CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const Cdmw::BadParameterException & ex ) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                         CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const Cdmw::CdmwInit::XMLErrorException & ex) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                         CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMInvalidXMLInitData,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        }
                     ]]>

                     <xsl:choose>
                        <xsl:when test="$isNamed">
                           typedef NamedObjectsFactory_initUtil Util;
                           typedef NamedObjectsFactoryAllocatorBase AllocatorType;
                        </xsl:when>
                        <xsl:otherwise>
                           typedef AnonymousObjectsFactory_initUtil Util;
                           typedef AnonymousObjectsFactoryAllocatorBase AllocatorType;
                        </xsl:otherwise>
                     </xsl:choose>

                     // Get the factory Allocator
                     // TODO: Use a user-provided name rather than a naming convention!
                     <![CDATA[AllocatorType & allocator =]]>
                        <xsl:value-of select="$cppFactoryAllocatorClassname"/>::TheAllocator;

                     try {
                        Util::setup_<xsl:value-of select="$namedString"/>_object_factory(allocator,
                                                             orb,           // ORB
                                                             factories_poa.in(),    // PARENT POA
                                                             repository.in(),
                                                             root_context,
                                                             <xsl:if test="$isNamed">full_name_domain_name,</xsl:if>
                                                             full_factory_name,
                                                             "", // TODO:
                                                             *threading_policy,
                                                             argc, argv
                           );
                     <![CDATA[
                        } catch (const Cdmw::OutOfMemoryException & ex ) {
                           std::string s ("Could not allocate memory for ");
                           s += full_factory_name;
                           s += "\n";
                           s += ex.what();

                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                                         CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                                                          CORBA::COMPLETED_NO),
                                         s );
                        } catch (const Cdmw::BadParameterException & ex) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                                         CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMLifeCycleFrameworkInit,
                                                          CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const Cdmw::InternalErrorException & ex ) {
                           CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                                         CORBA::INTERNAL(Cdmw::OrbSupport::INTERNALLifeCycleFrameworkError,
                                                         CORBA::COMPLETED_NO),
                                         ex.what() );
                        } catch (const CORBA::SystemException & ex ) {
                           CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,
                                         ex );
                        }
                     ]]>
                  </xsl:if>
               }
            </xsl:when>
         </xsl:choose>
      </xsl:for-each>

      <xsl:if test="$hasHomeImpl='true'">
         <![CDATA[  
         // Get a Default Event Channel Factory
         CdmwEvent::EventChannelFactory_var event_channel_factory 
             = CdmwEvent::EventChannelFactory::_nil();
         ]]>

         <xsl:if test="$hasEventPorts = 'true'">
            <![CDATA[
            try {
                // Retreive reference to the Default Event Channel factory 
                using namespace Cdmw::NamingAndRepository;             
                using namespace Cdmw::Common;

                std::string evt_factories_nd(Locations::CDMW_EVENT_SERVICE_NAME_DOMAIN);
                evt_factories_nd += "/FACTORIES";

                NamingInterface evt_ni = 
                    RepositoryInterface::get_domain_naming_interface(evt_factories_nd);
           
                const std::string EVENT_CHANNEL_FACTORY_NAME("DefaultEventChannelFactory");

                typedef NamingUtil<CdmwEvent::EventChannelFactory> Util;
               
                event_channel_factory = Util::resolve_name(evt_ni,EVENT_CHANNEL_FACTORY_NAME);
            } catch (const CdmwNamingAndRepository::NoNameDomain &) {
               CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                             CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMNameDomainNotFound,
                                              CORBA::COMPLETED_NO),
                             std::string(Cdmw::Common::Locations::CDMW_EVENT_SERVICE_NAME_DOMAIN) +
                             "/FACTORIES is not a NameDomain!" );
            } catch (const CdmwNamingAndRepository::InvalidName &) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                             CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                             CORBA::COMPLETED_NO) );
            } catch (const CosNaming::NamingContext::NotFound &) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                             CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                             CORBA::COMPLETED_NO) );
            } catch (const CosNaming::NamingContext::CannotProceed &) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                             CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                             CORBA::COMPLETED_NO) );
            } catch (const CosNaming::NamingContext::InvalidName &) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                             CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                             CORBA::COMPLETED_NO) );
            } catch (const Cdmw::Common::TypeMismatchException &) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                             CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                             CORBA::COMPLETED_NO) );
            } catch (const CORBA::SystemException & ex) {
                CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
            }
            ]]>
         </xsl:if>

         <![CDATA[
         // Create the ComponentServer
         try {

             CdmwCcmComponentServer::ComponentServer_var the_component_server =
                 Cdmw::CCM::ComponentServer::InitUtils::create_component_server(orb,
                                                                                 CDMW_rootPOA.in(),
                                                                                 event_channel_factory.in(),
                                                                                 process_name,
                                                                                 application_name,
                                                                                 argc,argv);
             // Register it as Cdmw::Common::Locations::ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN/
             //      process_name@application_name.ComponentServer
             std::string id = process_name + std::string("@") + application_name;
             std::string kind("ComponentServer");
             CosNaming::Name name(1);
             name.length(1);
             name[0].id   = id.c_str();
             name[0].kind = kind.c_str();

             using namespace Cdmw::NamingAndRepository;             
             using namespace Cdmw::Common;

             std::string key = NamingInterface::to_string(name);

             CdmwNamingAndRepository::NameDomain_var domain =
                 RepositoryInterface::get_domain(Locations::ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN);

             try {
                 domain->register_new_factory(key.c_str(),the_component_server.in());
             } catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists &) {
                 // Re-register it again.
                 domain->release_name(key.c_str());
                 domain->register_new_factory(key.c_str(),the_component_server.in());
             } 
         } catch (const CdmwNamingAndRepository::NoNameDomain &) {
            CDMW_THROW2( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                          CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMNameDomainNotFound,
                                           CORBA::COMPLETED_NO),
                          std::string(Cdmw::Common::Locations::ASSEMBLY_AND_DEPLOYMENT_NAME_DOMAIN) +
                          " is not a NameDomain!" );
         } catch (const CdmwNamingAndRepository::InvalidName &) {
             CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO) );
         } catch (const CdmwNamingAndRepository::NameDomain::NotRegistered &) {
             CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                          CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
                                          CORBA::COMPLETED_NO) );
         } catch (const CORBA::SystemException & ex) {
             CDMW_THROW1( Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex );
         }
         ]]>
      </xsl:if>
   </xsl:if>

   <![CDATA[
         // Discard requests until run order comes from the platform management
         poaManager->discard_requests(false);

         // advise process_impl that cdmw_init is done
         process_impl->set_cdmw_init_done();
         process_impl->get_condition().broadcast();
    ]]>
     
   <xsl:choose>
      <xsl:when test="$hasTrace">
      <![CDATA[
     
      } catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex) {
         #ifdef CDMW_TRACES_ENABLED
     
         if (Cdmw::Trace::FlushAreaMngr::is_to_be_traced(Cdmw::CDMW_INIT, Cdmw::FTL)) {
            Cdmw::Trace::Stream stream;
            stream.set_line(__LINE__);
            stream.set_file(__FILE__);
            stream.set_userDomain(Cdmw::CDMW_INIT);
            stream.set_userLevel(Cdmw::FTL);
            stream << ex << std::ends;
            stream.do_flush();
         } else // In case there is a problem in the trace library itself!
            std::cerr << Cdmw::CDMW_INIT << " : Fatal Error : " << ex << std::endl;        
         #else
         std::cerr << "CDMW_init: Fatal Error: \n"
                   << "File : " << __FILE__ << "\n"
                   << "Line : " << __LINE__ << "\n"
                   << "CORBA Exception : " << ex << std::endl;
         
         #endif // CDMW_TRACES_ENABLED
           
            ADVISE_EXCEPTION_RAISED(process_impl);   
            ex.raise_SystemException();
      } catch (const CORBA::SystemException & ex) {
         #ifdef CDMW_TRACES_ENABLED
     
         if (Cdmw::Trace::FlushAreaMngr::is_to_be_traced(Cdmw::CDMW_INIT, Cdmw::FTL)) {
            Cdmw::Trace::Stream stream;
            stream.set_line(__LINE__);
            stream.set_file(__FILE__);
            stream.set_userDomain(Cdmw::CDMW_INIT);
            stream.set_userLevel(Cdmw::FTL);
            stream << ex << std::ends;
            stream.do_flush();
         } else // In case there is a problem in the trace library itself!
            std::cerr << Cdmw::CDMW_INIT << " : Fatal Error : " << ex << std::endl;        
         #else
         std::cerr << "CDMW_init: Fatal Error: \n"
                   << "File : " << __FILE__ << "\n"
                   << "Line : " << __LINE__ << "\n"
                   << "CORBA Exception : " << ex << std::endl;
         #endif // CDMW_TRACES_ENABLED
           
            ADVISE_EXCEPTION_RAISED(process_impl);
            throw;
      }
      ]]>
      </xsl:when>
      <xsl:otherwise>
      <![CDATA[
     
      } catch (const Cdmw::OrbSupport::CORBASystemExceptionWrapper & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;
            ADVISE_EXCEPTION_RAISED(process_impl);   
            ex.raise_SystemException();
      } catch (const CORBA::SystemException & ex) {
            std::cerr << "CDMW_init: Fatal Error: \n"
                      << "File : " << __FILE__ << "\n"
                      << "Line : " << __LINE__ << "\n"
                      << "CORBA Exception : " << ex << std::endl;           
            ADVISE_EXCEPTION_RAISED(process_impl);
            throw;
      }
      ]]>
      </xsl:otherwise>
   </xsl:choose>
   <![CDATA[

   }

      // CDMW Factory objects will be deactivated at ORB shutdown.
      void
      CDMW_cleanup(CORBA::ORB_ptr orb)
         throw ()
      {
         CDMW_ASSERT(!CORBA::is_nil(orb));
         
         // Release all static object references held by RepositoryInterface
         Cdmw::NamingAndRepository::RepositoryInterface::finish();
   ]]>
   
      <xsl:if test="$hasHomeImpl='true'">
         using namespace Cdmw::CCM::Container;

         // Unregister HomeAllocators
         {
            <xsl:for-each select="$frameworkNodes">
               <xsl:variable name="frameworkIdref" select="@idref"/>

               <!--
                  Get the matching node /cdmw/lifecycle-frameworks/lifecycle-framework.
               -->
               <xsl:variable name="lifecycleFrameworkNode" select="$lifecycleFrameworkNodes[@id = $frameworkIdref]"/>
               <xsl:variable name="isHomeImpl" select="boolean($lifecycleFrameworkNode/home-impl)"/>

               <xsl:if test="$isHomeImpl">
                  <xsl:variable name="factoryClassname">
                     <xsl:call-template name="buildHomeImplClassname">
                        <xsl:with-param name="_homeImplNode" select="$lifecycleFrameworkNode/home-impl"/>
                     </xsl:call-template>
                  </xsl:variable>

                  HomeAllocatorRegistry::Unregister("<xsl:value-of select="$factoryClassname"/>");
               </xsl:if>
            </xsl:for-each>
         }
      </xsl:if>
   
     <xsl:if test="$hasTrace">
     <![CDATA[
         #ifdef CDMW_TRACES_ENABLED

         // Free all allocated memory and stop the trace service
         try {
            Cdmw::Trace::FlushAreaMngr::cleanup();
         } catch (const Cdmw::InternalErrorException & ) {
            // Do nothing (ignore)
            // TODO: Check if exception has been removed from cleanup() specification.
         }
         
         #endif // CDMW_TRACES_ENABLED

     ]]>
     </xsl:if>

     <xsl:if test="$hasPerformance">
     <![CDATA[
         #ifdef CDMW_PERFORMANCE_ENABLED
         
         // Free all allocated memory and stop the performance service
         try {
            Cdmw::Performance::PerformanceMngr::cleanup();
         } catch (const Cdmw::BadParameterException & ) {
            // Do nothing (ignore)
		      // TODO: Add trace logs here
         }

         #endif // CDMW_PERFORMANCE_ENABLED

      
     ]]>
     </xsl:if>
     
      }

   <xsl:call-template name="closeNamespace">
      <xsl:with-param name="_scope" select="$cdmwinitNamespace"/>
      <xsl:with-param name="_lastTokenIsNamespace" select="true()"/>
   </xsl:call-template>

   <!--
      The code generation ends here.
   -->

</xsl:template> <!-- end of template cdmw_interface_cpp.content -->



</xsl:stylesheet>


