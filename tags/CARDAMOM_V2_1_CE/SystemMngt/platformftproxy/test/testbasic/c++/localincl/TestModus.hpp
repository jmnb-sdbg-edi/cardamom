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


#ifndef INCL_PLATFORMMNGT_PROXY_TEST_MODUS_HPP
#define INCL_PLATFORMMNGT_PROXY_TEST_MODUS_HPP

#include <list>

#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/common/Exception.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplicationBase.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtProcessObserver.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtBase.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtProcess.skel.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEntity.skel.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtApplicationAgent.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEntity.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtHost.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp>

#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtApplication.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtProcess.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtEntity.stub.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtElementRepository.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtObservable.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp>

namespace Cdmw {
namespace PlatformMngt {
namespace Proxy {

    // Usage: This unit test implements client and dummy-server to
    // test operations of proxy. Every operation should be tested in
    // context of successfull operation and raisinig exception. The
    // client sets the exception modus for dummy-server before calling
    // any operation on proxy interfaces, the exception returned must
    // match the pattern known by client, otherwise the test failed.
    // Adding a new exception, also the catch block of macro
    // TEST_FOREACH_ACCESSOR_EXCEPTION, and the operations
    // "raiseExceptionIfExceptionModus()" and "getTestModusName()"
    // must be extended.

    // By convention an exception "CdmwPlatformMngt::DuplicateEntity"
    // will be named CdmwPlatformMngt_DuplicateEntity. 

    // Most mappings are ident-mapping, but 
    
    // 1) exception "CdmwPlatformMngt::ManagedElementNotFound" must be
    // mapped onto exception CORBA::TRANSIENT exception by
    // proxy. Otherwise the test failed.

    // 2) if pseudo exception "NoException" set, the operation called
    // on dummy-server will verify arguments and return successfully,
    // otherwise the test failed. If the values returned by operation
    // are not valid the test failed.

    // 3) if pseudo excpetion "NotImplemented" marks operations on
    // dummy-server or proxy that are obsolete now, or for future use;
    // the client must catch CORBA::NO_IMPLEMENT exception, otherwise
    // the operation

    typedef enum { NoException, 
		   NotImplemented,
		   CORBA_INTERNAL,
		   CORBA_BAD_PARAM,
		   CORBA_NO_MEMORY,
		   CORBA_MARSHAL, 
		   CORBA_BAD_INV_ORDER,
		   CORBA_TRANSIENT,
		   CORBA_OBJECT_NOT_EXIST,
		   CdmwPlatformMngt_DuplicateEntity,
		   CdmwPlatformMngt_DuplicateService,
		   CdmwPlatformMngt_DuplicateProcess,
		   CdmwPlatformMngt_DuplicateProcessEntity,
		   CdmwPlatformMngt_DuplicateProcessService,
		   CdmwPlatformMngt_DuplicateProcessStep,
		   CdmwPlatformMngt_DuplicateStep,
		   CdmwPlatformMngt_DuplicateHost,
		   CdmwPlatformMngt_DuplicateRunSetElement,

		   CdmwPlatformMngt_AlreadyDone,
		   CdmwPlatformMngt_IncompatibleStatus,
		   CdmwPlatformMngt_ManagedElementNotFound,

		   CdmwPlatformMngt_ProcessAlreadyExists,
		   CdmwPlatformMngt_ProcessNotFound,
		   CdmwPlatformMngt_ProcessHostNotFound,
		   CdmwPlatformMngt_ObserverNotFound,
		   
		   CdmwPlatformMngt_HostNotFound,
		   CdmwPlatformMngt_ReferenceNotFound,
		   CdmwPlatformMngt_CircularReference,
		   CdmwPlatformMngt_StepOutOfBound,
		   CdmwPlatformMngt_StepJump,

		   CosPropertyService_InvalidPropertyName,
		   CosPropertyService_ConflictingProperty,
		   CosPropertyService_UnsupportedTypeCode,
		   CosPropertyService_UnsupportedProperty,
		   CosPropertyService_UnsupportedMode,
		   CosPropertyService_ReadOnlyProperty,
		   CosPropertyService_MultipleExceptions,
		   CosPropertyService_FixedProperty,
		   CosPropertyService_PropertyNotFound,

		   CdmwPlatformMngt_PropertyInListNotFound,
		   CdmwPlatformMngt_PropertyObserverNotFound,

		   CdmwPlatformMngt_EntityStatusNotAllowed,

		   CdmwPlatformMngt_CommandAlreadyExists,
		   CdmwPlatformMngt_CommandArgumentAlreadyExists,
		   
		   CdmwPlatformMngt_RunSetNotFound,
		   CdmwPlatformMngt_HostAlreadyExists,
		   CdmwPlatformMngt_ApplicationAlreadyExists,

		   CdmwPlatformMngt_ApplicationNotFound,
		   CdmwPlatformMngt_ApplicationStillActive,
		   CdmwPlatformMngt_EntityNotFound, 
		   CdmwPlatformMngt_EntityAlreadyExists,
		   CdmwPlatformMngtService_ServiceAlreadyExists,
		   CdmwPlatformMngtService_ServiceNotFound,
		   CdmwPlatformMngt_ProcessPropertyExceptions,
		   CdmwPlatformMngt_DuplicateProcessCommand,
		   CdmwPlatformMngt_DuplicateProcessCommandArg,
		   CdmwPlatformMngt_DuplicateEntityCommand,
		   CdmwPlatformMngt_DuplicateEntityCommandArg,
		   CdmwPlatformMngt_DuplicateCommand,
		   CdmwPlatformMngt_DuplicateCommandArg,

		   CdmwPlatformMngt_ProcessStillActive,
		   CdmwPlatformMngt_IncompatibleType,
		   CdmwPlatformMngt_NotReadyToRun,
		   CdmwPlatformMngt_InvalidStep
		  
    } 
    TestModus;

    void setTestModus(TestModus modus) throw (); 
    
    TestModus getTestModus() throw ();

    void raiseExceptionIfExceptionModus(); // throws anything

    void raiseSystemExceptionIf(bool raise_exception);
    
//     CdmwPlatformMngt::SystemSnapshot* 
//     getSystemSnapshot(const CdmwPlatformMngt::SystemDef* system_def,
// 		      const Cdmw::PlatformMngt::Parser::ApplicationDataPairList& appl_def_list)
// 	throw (CORBA::SystemException);

//     char* /* CORBA::String */ 
//     getApplicationUIDFromName (const Cdmw::PlatformMngt::Parser::ApplicationDataPairList& appl_def_list,
// 			      const char* appl_name)
// 	throw (CORBA::SystemException);
    
//      char* /* CORBA::String */ 
//     getApplicationUIDFromDef(const Cdmw::PlatformMngt::Parser::ApplicationDataPairList& appl_def_list,
// 			     const CdmwPlatformMngt::PlatformApplicationDef& platform_application_def,
// 			     const CdmwPlatformMngt::ApplicationDef& application_def) 
// 	throw (CORBA::SystemException);
    
//     std::list<CdmwPlatformMngt::ElementPath_var>
//     getAllApplicationUIDs(const Cdmw::PlatformMngt::Parser::ApplicationDataPairList& appl_def_list)
// 	throw (CORBA::SystemException);
 
//      char* /* CORBA::String */ 
//     getHostUID(const CdmwPlatformMngt::SystemDef* system_def,
// 	       const char* application_name) 
// 	throw (CORBA::SystemException);

//     bool
//     isHostDefined(const CdmwPlatformMngt::SystemDef* system_def,
// 		  const char* host_name) throw();
    
//     bool
//     isApplicationDefined(const Cdmw::PlatformMngt::Parser::ApplicationDataPairList& appl_def_list,
// 			 const char* application_name) throw();
    

//     CORBA::ULong getNumberOfApplications(const Cdmw::PlatformMngt::Parser::ApplicationDataPairList& appl_def_list) 
// 	throw();

//     char* 
//     getApplicationNameAt(const Cdmw::PlatformMngt::Parser::ApplicationDataPairList& appl_def_list,
// 			 unsigned long idx) 
// 	throw();

//     CdmwPlatformMngt::SupervisionObserver_ptr 
//     getSupervisionObserver(const char*  observer_name)  
// 	throw (CORBA::SystemException);	

    /** Purpose: <p> Iterate the sequence and return the number of
     * elements within list; finally call destroy operation of
     * iterator
     */
    template<class Iterator, class ElemVarType>
    unsigned long
    countIteratorElementsAndDestroy(Iterator it) {

	unsigned long result = 0;
	ElemVarType obj;

	// FIXME use nextn operation

	// fore each element contained in reference iterator
	while (it->next_one (obj.out())) { 
	    // FIXME trigger some operations on application
	    ++result;
	}

	it->destroy();

	return result;
    }


    typedef std::list<Cdmw::PlatformMngt::Proxy::TestModus>  TestModusList;

    /** Puspose: <p> 
     *  Add exception mode to list and return a new list.
     */
    inline
    TestModusList operator+ (const TestModusList &vec, 
			     const Cdmw::PlatformMngt::Proxy::TestModus &state) 
    {
	TestModusList result(vec);

	result.push_back(state); 
	return result; // return by copy
    }

    /** Puspose: <p> 
     *  Transform exception mode into const string
     */
    const char* getTestModusName() throw ();

    // Usage: The macro expects 3 arguments, the command to be
    // executed, the check in case the operation returned
    // successfully, and last a set of exception modes to be
    // checked. The exception modes must be seperated by plus-symbol
    // "+". If modus NoException is set, the operation should return
    // with valid return values, which can be used within
    // check-excpression.
 
    // {
    //    /* declare local variables within  block */
    // 	  CORBA::String_var arg = CORBA::string_dup("foo");
    // 	  CORBA::String_var result; 
    
    // 	  TEST_MINIMUM_EXCEPTION_SET
    // 	    (result = obj->doSomething(arg.in()),
    // 	     CPPUNIT_ASSERT(strlen(result.in())>0),
    // 	     NoException + CORBA_INTERNAL + CORBA_BAD_PARAM
    // 	     );
    
    // 	  /* free all variables at end of block */
    // }
    

#define TEST_MINIMUM_EXCEPTION_SET \
	     Cdmw::PlatformMngt::Proxy::CORBA_INTERNAL+ \
	     Cdmw::PlatformMngt::Proxy::CORBA_BAD_PARAM+ \
	     Cdmw::PlatformMngt::Proxy::CORBA_NO_MEMORY+ \
	     Cdmw::PlatformMngt::Proxy::CORBA_MARSHAL+ \
	     Cdmw::PlatformMngt::Proxy::CORBA_BAD_INV_ORDER+ \
	     Cdmw::PlatformMngt::Proxy::CORBA_OBJECT_NOT_EXIST+ \
	     Cdmw::PlatformMngt::Proxy::CORBA_TRANSIENT

 
#define TEST_FOREACH_ACCESSOR_EXCEPTION(cmdexpr,check,modelist)\
   for (TestModusList ml(TestModusList()+modelist); \
        ! ml.empty();                               \
        ml.pop_front())                             \
   {                                                \
        try {                                       \
            setTestModus(*(ml.begin()));            \
            TEST_INFO((std::string("*TestMode=") + getTestModusName()).c_str());   \
            cmdexpr;                                \
            check;                                  \
            setTestModus(NoException); /**reset*/   \
	}                                           \
 	catch (const CORBA::INTERNAL &) {    \
	    CPPUNIT_ASSERT(getTestModus() == CORBA_INTERNAL);\
	}\
	catch (const CORBA::BAD_PARAM &) {    \
	    CPPUNIT_ASSERT(getTestModus() == CORBA_BAD_PARAM);\
	}\
	catch (const CORBA::NO_MEMORY &) {    \
	    CPPUNIT_ASSERT(getTestModus() == CORBA_NO_MEMORY);\
	}\
	catch (const CORBA::MARSHAL &) {    \
	    CPPUNIT_ASSERT(getTestModus() == CORBA_MARSHAL);\
	}\
	catch (const CORBA::BAD_INV_ORDER &) {    \
	    CPPUNIT_ASSERT(getTestModus() == CORBA_BAD_INV_ORDER);\
	}\
	catch (const  CORBA::OBJECT_NOT_EXIST &) {    \
           CPPUNIT_ASSERT( getTestModus() == CORBA_OBJECT_NOT_EXIST  || \
                       getTestModus() == CdmwPlatformMngt_ManagedElementNotFound ); \
	}\
        catch (const CORBA::TRANSIENT &) {    \
	    CPPUNIT_ASSERT(getTestModus() == CORBA_TRANSIENT || \
                       getTestModus() == CdmwPlatformMngt_ManagedElementNotFound);\
	}\
        catch (const CORBA::NO_IMPLEMENT &) {    \
	    CPPUNIT_ASSERT(getTestModus() == NotImplemented);\
	}\
	catch (const  CdmwPlatformMngt::DuplicateEntity &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateEntity);\
	}\
	catch (const CdmwPlatformMngt::DuplicateService  &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateService);\
	}\
	catch (const  CdmwPlatformMngt::DuplicateProcess &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateProcess);\
	}\
	catch (const CdmwPlatformMngt::DuplicateProcessEntity &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateProcessEntity);\
	}\
	catch (const CdmwPlatformMngt::DuplicateProcessService &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateProcessService);\
	}\
	catch (const CdmwPlatformMngt::DuplicateProcessStep &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateProcessStep);\
	}\
	catch (const CdmwPlatformMngt::DuplicateHost &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateHost);\
	}\
	catch (const CdmwPlatformMngt::DuplicateRunSetElement &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateRunSetElement);\
	}\
	catch (const CdmwPlatformMngt::AlreadyDone &) /* new */  {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_AlreadyDone);\
	}\
	catch (const   CdmwPlatformMngt::IncompatibleStatus&) {\
	    CPPUNIT_ASSERT(getTestModus() ==  CdmwPlatformMngt_IncompatibleStatus);\
	}\
	catch (const CdmwPlatformMngt::ManagedElementNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ManagedElementNotFound);\
	}\
	catch (const CdmwPlatformMngt::ProcessAlreadyExists &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ProcessAlreadyExists);\
	}\
	catch (const CdmwPlatformMngt::ProcessNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ProcessNotFound);\
	}\
	catch (const CdmwPlatformMngt::ProcessHostNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ProcessHostNotFound);\
	}\
	catch (const CdmwPlatformMngt::ObserverNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ObserverNotFound);\
	}\
	catch (const CdmwPlatformMngt::HostNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_HostNotFound);\
	}\
	catch (const CdmwPlatformMngt::ReferenceNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ReferenceNotFound);\
	}\
	catch (const CdmwPlatformMngt::CircularReference &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_CircularReference);\
	}\
	catch (const CdmwPlatformMngt::StepOutOfBound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_StepOutOfBound);\
	}\
	catch (const CdmwPlatformMngt::StepJump &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_StepJump);\
	}\
	catch (const  CosPropertyService::InvalidPropertyName &) {\
	    CPPUNIT_ASSERT(getTestModus() ==  CosPropertyService_InvalidPropertyName);\
	}\
	catch (const CosPropertyService::ConflictingProperty &) {\
	    CPPUNIT_ASSERT(getTestModus() == CosPropertyService_ConflictingProperty);\
	}\
	catch (const CosPropertyService::UnsupportedTypeCode &) {\
	    CPPUNIT_ASSERT(getTestModus() == CosPropertyService_UnsupportedTypeCode);\
	}\
	catch (const CosPropertyService::UnsupportedProperty &) {\
	    CPPUNIT_ASSERT(getTestModus() == CosPropertyService_UnsupportedProperty);\
	}\
	catch (const CosPropertyService::UnsupportedMode &) {\
	    CPPUNIT_ASSERT(getTestModus() == CosPropertyService_UnsupportedMode);\
	}\
	catch (const CosPropertyService::ReadOnlyProperty &) {\
	    CPPUNIT_ASSERT(getTestModus() == CosPropertyService_ReadOnlyProperty);\
	}\
	catch (const CosPropertyService::MultipleExceptions &) {\
	    CPPUNIT_ASSERT(getTestModus() == CosPropertyService_MultipleExceptions);\
	}\
	catch (const CosPropertyService::FixedProperty &) {\
	    CPPUNIT_ASSERT(getTestModus() == CosPropertyService_FixedProperty);\
	}\
	catch (const  CosPropertyService::PropertyNotFound&) {\
	    CPPUNIT_ASSERT(getTestModus() == CosPropertyService_PropertyNotFound);\
	}\
	catch (const CdmwPlatformMngt::PropertyInListNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_PropertyInListNotFound);\
	}\
	catch (const CdmwPlatformMngt::PropertyObserverNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_PropertyObserverNotFound);\
	}\
	catch (const CdmwPlatformMngt::EntityStatusNotAllowed &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_EntityStatusNotAllowed);\
	}\
	catch (const CdmwPlatformMngt::CommandAlreadyExists &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_CommandAlreadyExists);\
	}\
	catch (const CdmwPlatformMngt::CommandArgumentAlreadyExists  &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_CommandArgumentAlreadyExists);\
	} \
	catch (const CdmwPlatformMngt::RunSetNotFound  &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_RunSetNotFound);\
	} \
	catch (const CdmwPlatformMngt::HostAlreadyExists  &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_HostAlreadyExists);\
	} \
	catch (const CdmwPlatformMngt::ApplicationAlreadyExists  &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ApplicationAlreadyExists);\
	} \
	catch (const CdmwPlatformMngt::ApplicationNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ApplicationNotFound);\
	} \
	catch (const CdmwPlatformMngt::ApplicationStillActive &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ApplicationStillActive);\
	} \
	catch (const CdmwPlatformMngt::EntityNotFound  &) {\
	    CPPUNIT_ASSERT(getTestModus() ==  CdmwPlatformMngt_EntityNotFound);\
	} \
	catch (const CdmwPlatformMngt::EntityAlreadyExists &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_EntityAlreadyExists);\
	} \
	catch (const CdmwPlatformMngtService::ServiceAlreadyExists &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngtService_ServiceAlreadyExists);\
	} \
	catch (const CdmwPlatformMngtService::ServiceNotFound &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngtService_ServiceNotFound );\
	} \
	catch (const CdmwPlatformMngt::ProcessPropertyExceptions &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ProcessPropertyExceptions);\
	} \
	catch (const CdmwPlatformMngt::DuplicateProcessCommand &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateProcessCommand);\
	} \
	catch (const CdmwPlatformMngt::DuplicateProcessCommandArg &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateProcessCommandArg);\
	} \
	catch (const CdmwPlatformMngt::DuplicateEntityCommand &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateEntityCommand);\
	} \
	catch (const CdmwPlatformMngt::DuplicateEntityCommandArg &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateEntityCommandArg);\
	} \
	catch (const CdmwPlatformMngt::DuplicateCommand &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateCommand);\
	} \
	catch (const CdmwPlatformMngt::DuplicateCommandArg &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateCommandArg);\
	} \
	catch (const CdmwPlatformMngt::DuplicateStep &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_DuplicateStep);\
	} \
	catch (const CdmwPlatformMngt::ProcessStillActive &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_ProcessStillActive );\
	} \
	catch (const  CdmwPlatformMngt::IncompatibleType &) {\
	    CPPUNIT_ASSERT(getTestModus() ==  CdmwPlatformMngt_IncompatibleType);\
	} \
	catch (const CdmwPlatformMngt::NotReadyToRun &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_NotReadyToRun);\
	} \
	catch (const CdmwPlatformMngt::InvalidStep &) {\
	    CPPUNIT_ASSERT(getTestModus() == CdmwPlatformMngt_InvalidStep);\
	} \
        catch (const CORBA::SystemException &ex) {\
            std::cerr << "Unexpected CORBA::SystemException caught " << __FILE__ << " " << ex << std::endl;\
	    CPPUNIT_ASSERT(false);\
	} \
        catch (const CORBA::UserException &ex) {\
            std::cerr << "Unexpected CORBA::UserException caught " << __FILE__ << " " << ex << std::endl;\
	    CPPUNIT_ASSERT(false);\
	} \
	catch (...) {\
            std::cerr << "Unexpected exception caught " << __FILE__ << std::endl;\
	    CPPUNIT_ASSERT(false);\
	}\
    } 


}; // namespace Proxy    
}; // namespace PlatformMngt
}; // namespace Cdmw

#endif // INCL_PLATFORMMNGT_PROXY_TEST_MODUS_HPP
