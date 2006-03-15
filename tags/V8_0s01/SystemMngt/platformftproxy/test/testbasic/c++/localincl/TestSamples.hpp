/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMMNGT_PROXY_TEST_SAMPLES_HPP
#define INCL_PLATFORMMNGT_PROXY_TEST_SAMPLES_HPP

#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
#include <Foundation/idllib/CosPropertyService.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCommonBase.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtSupervisionObserver.stub.hpp>

#include <SystemMngt/platformvaluetypes/GraphElement_impl.hpp>
#include <SystemMngt/platformvaluetypes/ProcessDef_impl.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCmdResponseCallback.stub.hpp>
#include <list>

namespace Cdmw {
namespace PlatformMngt {
namespace Proxy {

    /** Purpose: <p> */ 
    class TestSamples {
    public:
	static 
	char* 
	getPlatformApplicationExe() throw();

	static 
	char* getObserverName() throw();

	static 
	CdmwPlatformMngt::SupervisionObserver*
	getSupervisionObserver()
	    throw()
	{
	    CdmwPlatformMngt::SupervisionObserver *observer =
		CdmwPlatformMngt::SupervisionObserver::_nil();

	    return observer;
	}

	static 
	CdmwPlatformMngt::ProcessStatus
	getProcessStatus() throw() {
	    return  CdmwPlatformMngt::PROCESS_RUNNING;
	}

	static 
	CdmwPlatformMngt::ProcessCommandResult
	getProcessCommandResult() throw() {
	    return  CdmwPlatformMngt::PROCESS_CMD_TIMEOUT;
	}

	static 
	CdmwPlatformMngt::ProcessDef*
	getProcessDef() throw() {
	    const char * id = "proxy-process-def";

	    CdmwPlatformMngt::ProcessDef_var processDef;

#if CDMW_ORB_VDR == tao
            PlatformMngt::ProcessDefFactory processDefFactory;
	    
            processDef = processDefFactory.create
		( id );
 #else
            CORBA::ValueFactoryBase_var processDefFactory
                = new PlatformMngt::ProcessDefFactory();
	    
            processDef = dynamic_cast< PlatformMngt::ProcessDefFactory* >(
									  processDefFactory.in() )->create( id );
#endif
	     return processDef._retn();
	}

	static 
	CdmwPlatformMngt::GraphElements*
	getGraphElements() throw() {
	    CdmwPlatformMngt::GraphElements *result = 
		new CdmwPlatformMngt::GraphElements();

	    return result;
	}

	static 
	CdmwPlatformMngt::ApplicationStatus
	getApplicationStatus() throw() {
	    return  CdmwPlatformMngt::APPLICATION_RUNNING;
	}
	
	static 
	CdmwPlatformMngt::FunctioningMode
	getFunctioningMode() throw() {
	    return  CdmwPlatformMngt::NORMAL_MODE;
	}

	static 
	CdmwPlatformMngt::RestartType
	getRestartType() throw() {
	    return  CdmwPlatformMngt::COLD_RESTART;
	}

	static 
	CORBA::Long
	getRestartAttempt() throw() {
	    return  42;
	}

	static 
	CdmwPlatformMngt::SystemSnapshot*
	getSystemSnapshot() throw() {
	    CdmwPlatformMngt::SystemSnapshot *result = 
		new CdmwPlatformMngt::SystemSnapshot();
	    return result;
	}

// 	static 
// 	CdmwPlatformMngt::SystemSnapshot*
// 	getSystemSnapshot() throw() {
// 	    CdmwPlatformMngt::SystemSnapshot_var result;// tbd
// 	    return  result._retn();
// 	}

	static 
	CdmwPlatformMngtBase::StartupKind*
	getStartupKind() throw() {
	    CdmwPlatformMngtBase::StartupKind *result = 
		new CdmwPlatformMngtBase::StartupKind(); // tbd
	    return  result;
	}

		
	static 
	CdmwPlatformMngt::HostSnapshot*
	getHostSnapshot() throw() {
	    CdmwPlatformMngt::HostSnapshot *result =
		new CdmwPlatformMngt::HostSnapshot(); // tbd
	    return  result;
	}

	static 
	CdmwPlatformMngt::EntitiesSnapshot*
	getEntitiesSnapshot() throw() {
	    CdmwPlatformMngt::EntitiesSnapshot *result =
		new CdmwPlatformMngt::EntitiesSnapshot(); // tbd
	    return  result;
	}
	
	static 
	CdmwPlatformMngt::ProcessesSnapshot*
	getProcessesSnapshot() throw() {
	    CdmwPlatformMngt::ProcessesSnapshot *result =
		new CdmwPlatformMngt::ProcessesSnapshot();// tbd
	    return  result;
	}
	
	static 
	CdmwPlatformMngt::ApplicationDef*
	getApplicationDef() throw() {
	    CdmwPlatformMngt::ApplicationDef *result =
		new CdmwPlatformMngt::ApplicationDef();// tbd
	    return  result;
	}
	
	static 
	CdmwPlatformMngt::ApplicationAutoRestartInfo
	getApplicationAutoRestartInfo() throw() {
	    CdmwPlatformMngt::ApplicationAutoRestartInfo result; // tbd
	    return  result;
	}
	static 
	CdmwPlatformMngtService::ServiceProviderID* 
	getServiceProviderID() 
	    throw ()
	{
	    CdmwPlatformMngtService::ServiceProviderID *result = 
		new CdmwPlatformMngtService::ServiceProviderID();
	    return  result;
	}

	static 
	CdmwPlatformMngt::LongRequestStatus
	getLongRequestStatus() throw() {
	    return CdmwPlatformMngt::LONG_REQUEST_ABORTED;
	}

	static 
	char* getRunSetName() throw();
	
		
	static 
	CdmwPlatformMngt::EntityStatusValues*
	getEntityStatusValues() 
	    throw (); 
	
	static 
	char*
	getEntityStatus() 
	    throw (); 

	static 
	CdmwPlatformMngt::ElementNames*
	getElementNames() 
	    throw (); 
	
	static 
	char*
	getElementName()  
	    throw () 
	{
	    CdmwPlatformMngt::ElementNames_var names = 
		getElementNames();

	    return CORBA::string_dup((*names)[0]);
	}
	
	static 
	CdmwPlatformMngt::ElementPaths*
	getElementPaths() 
	    throw (); 

	static 
	char*
	getElementPath() 
	    throw () 
	{
	    CdmwPlatformMngt::ElementPaths_var paths =
		getElementPaths();

	    return CORBA::string_dup((*paths)[0]);
	}

	

// 	CdmwPlatformMngt::ElementPaths*
// 	TestSamples::getApplicationIds() 
// 	    throw ();
 
	static 
	CosPropertyService::PropertyNames*
	getPropertyNames() 
	    throw (); 

	static 
	CosPropertyService::Properties*
	getProperties() 
	    throw (); 

	static 
	CORBA::Any* 
	getPropertyValue() 
	    throw ();

	static 
	CosPropertyService::PropertyTypes*
	getPropertyTypes()
	    throw ();

	static 
	CosPropertyService::PropertyDefs*
	getPropertyDefs()
	    throw () 
	{
	    CosPropertyService::PropertyDefs *result = 
		new CosPropertyService::PropertyDefs();

	    return result;
	}

	static 
	CosPropertyService::PropertyModes*
	getPropertyModes()
	    throw () 
	{
	    CosPropertyService::PropertyModes *result = 
		new CosPropertyService::PropertyModes();

	    return result;
	}

	static 
	CosPropertyService::PropertyModeType
	getPropertyModeType()
	    throw ();

	static 
	bool hasElementName(const char *elem) 
	    throw();

	static 
	bool hasElementPath(const char *elem) 
	    throw();

	static 
	bool hasPropertyValue(const CORBA::Any &value) 
	    throw();

	static
	bool 
	TestSamples::hasPropertyName(const char *elem) 
	    throw();

	
	static 
	CdmwPlatformMngt::ProcessType
	getProcessType() throw() {
	    return  CdmwPlatformMngt::MANAGED_PROCESS;
	}
	
	static 
	CdmwPlatformMngt::ProcessPID
	getProcessPID() throw() {
	    return  (CORBA::Long) 4711;
	}
	
	static 
	CdmwPlatformMngt::RequestStatus
	getRequestStatus() throw() {
	    return  CdmwPlatformMngt::REQUEST_TIMEOUT;
	}

	static 
	CdmwPlatformMngt::ProcessInfo*
	getProcessInfo()
	    throw () 
	{
	    CdmwPlatformMngt::ProcessInfo *result = 
		new CdmwPlatformMngt::ProcessInfo();// tbd
	    return result;
	}
	
	static 
	CdmwPlatformMngt::ProcessSnapshot*
	getProcessSnapshot()
	    throw () 
	{
	    CdmwPlatformMngt::ProcessSnapshot *result =
		new CdmwPlatformMngt::ProcessSnapshot();// tbd
	    return result;
	}

	static 
	CdmwPlatformMngt::ApplicationSnapshot*
	getApplicationSnapshot()
	    throw () 
	{
	    CdmwPlatformMngt::ApplicationSnapshot *result =
		new CdmwPlatformMngt::ApplicationSnapshot();// tbd
	    return result;
	}

	static 
	CdmwPlatformMngt::EntitySnapshot*
	getEntitySnapshot()
	    throw () 
	{
	    CdmwPlatformMngt::EntitySnapshot *result =
		new CdmwPlatformMngt::EntitySnapshot();// tbd
	    return result;
	}


	static 
	CdmwPlatformMngt::ExitAutoRestartInfo*
	getExitAutoRestartInfo()
	    throw () 
	{
	    CdmwPlatformMngt::ExitAutoRestartInfo *result =
		new CdmwPlatformMngt::ExitAutoRestartInfo();// tbd
	    return result;
	}

	static 
	CdmwPlatformMngt::CmdParameterValues*
	getCmdParameterValues()
	    throw () 
	{
	    CdmwPlatformMngt::CmdParameterValues *result =
		new CdmwPlatformMngt::CmdParameterValues();// tbd
	    return result;
	}

	static 
	CdmwPlatformMngt::CommandResponseCallback*
	getCommandResponseCallback()
	    throw () 
	{
	    CdmwPlatformMngt::CommandResponseCallback *result =
		CdmwPlatformMngt::CommandResponseCallback::_nil(); // tbd
	    return result;
	}

	static 
	CdmwPlatformMngt::CompleteMonitoringInfo
	getCompleteMonitoringInfo()
	    throw () 
	{
	    CdmwPlatformMngt::CompleteMonitoringInfo result; // tbd
	    return result;
	}

	static 
	CdmwPlatformMngt::LifeCycleDef*
	getLifeCycleDef()
	    throw () 
	{
	    CdmwPlatformMngt::LifeCycleDef *result =
		new CdmwPlatformMngt::LifeCycleDef(); // tbd
	    return result;
	}
	
	static 
	CORBA::Object_ptr
	getEmbeddedService()
	    throw () 
	{
	    return CORBA::Object::_nil(); 
	}
	
	static 
	CORBA::Boolean
	getCreatedDynamically() {
	    return true;
	}

	static 
	CORBA::Boolean
	getAutoEnding() {
	    return true;
	}

	static 
	CdmwPlatformMngtBase::TimeStamp
	getTimeStamp() throw() {
	    CdmwPlatformMngtBase::TimeStamp result; // tbd

	    return result;
	}
	
	static 
	CORBA::Boolean
	getManualStarting() throw() {
	    return true;
	}
    };
    
  
    void split (const CdmwPlatformMngt::ElementPaths *lst,
		unsigned long how_many,
		CdmwPlatformMngt::ElementPaths_out head,
		CdmwPlatformMngt::ElementPaths_out tail)
	throw();

     void split (const CdmwPlatformMngt::ElementNames *lst,
		unsigned long how_many,
		CdmwPlatformMngt::ElementNames_out head,
		CdmwPlatformMngt::ElementNames_out tail)
	throw();
    
    void split (const CosPropertyService::Properties *lst,
		unsigned long how_many,
		CosPropertyService::Properties_out head,
		CosPropertyService::Properties_out tail)
	throw();

    void split (const CosPropertyService::PropertyNames *lst,
		unsigned long how_many,
		CosPropertyService::PropertyNames_out head,
		CosPropertyService::PropertyNames_out tail)
	throw();


    inline
    std::list<CdmwPlatformMngt::ElementPath_var> 
    seq2list(const CdmwPlatformMngt::ElementPaths* seq) {
	std::list<CdmwPlatformMngt::ElementPath_var> result;
	
	for (unsigned long i=0; i<seq->length(); ++i) {
	    result.push_back(CORBA::string_dup((*seq)[i])); // deep copy
	}
	
	return result; // deep copy
    }
    
    inline
    std::list<CdmwPlatformMngt::ElementName_var> 
    seq2list(const CdmwPlatformMngt::ElementNames* seq) {
	std::list<CdmwPlatformMngt::ElementName_var> result;
	
	for (unsigned long i=0; i<seq->length(); ++i) {
	    result.push_back(CORBA::string_dup((*seq)[i])); // deep copy
	}
	
	return result; // deep copy
    }

    inline
    std::list<CosPropertyService::Property_var> 
    seq2list(const CosPropertyService::Properties* seq) {
	std::list<CosPropertyService::Property_var> result;
	
	for (unsigned long i=0; i<seq->length(); ++i) {	 
	    CosPropertyService::Property *prop = 
		new CosPropertyService::Property((*seq)[i]);

	    result.push_back(prop); // hand over maintainership  
	}
	
	return result; // deep copy
    }

    inline
    std::list<CosPropertyService::PropertyName_var> 
    seq2list(const CosPropertyService::PropertyNames* seq) {
	std::list<CosPropertyService::PropertyName_var> result;
	
	for (unsigned long i=0; i<seq->length(); ++i) {
	    result.push_back(CORBA::string_dup((*seq)[i])); // deep copy
	}
	
	return result; // deep copy
    }

}; // Namespace Proxy    
}; // namespace PlatformMngt
}; // namespace Cdmw

#endif // INCL_PLATFORMMNGT_PROXY_TEST_MODUS_HPP
