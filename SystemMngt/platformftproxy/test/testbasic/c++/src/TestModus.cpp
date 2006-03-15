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

#include "testbasic/TestModus.hpp"


#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>


namespace Cdmw {
namespace PlatformMngt {
namespace Proxy {

    static TestModus _currentTestModus = NoException;

    void setTestModus(TestModus modus) throw ()
    {
	_currentTestModus = modus;
    }
    
    TestModus getTestModus() throw ()
    {
	return _currentTestModus;
    }


    const char* getTestModusName() throw ()
    {
	switch (getTestModus()) {
	    // this list has been created from enumeration using sed-command:
	    // sed -r -e 's/^(.*),/case \1: return \"\1\";/g' states.txt
	case NoException: return "NoException"; 
	case NotImplemented: return "NotImplemented";
	case CORBA_INTERNAL: return "CORBA_INTERNAL";
	case CORBA_BAD_PARAM: return "CORBA_BAD_PARAM";
	case CORBA_NO_MEMORY: return "CORBA_NO_MEMORY";
	case CORBA_MARSHAL: return "CORBA_MARSHAL"; 
	case CORBA_BAD_INV_ORDER: return "CORBA_BAD_INV_ORDER";
	case CORBA_TRANSIENT: return "CORBA_TRANSIENT";
	case CORBA_OBJECT_NOT_EXIST: return "CORBA_OBJECT_NOT_EXIST_";
	case CdmwPlatformMngt_DuplicateEntity: return "CdmwPlatformMngt_DuplicateEntity";
	case CdmwPlatformMngt_DuplicateService: return "CdmwPlatformMngt_DuplicateService";
	case CdmwPlatformMngt_DuplicateProcess: return "CdmwPlatformMngt_DuplicateProcess";
	case CdmwPlatformMngt_DuplicateProcessEntity: return "CdmwPlatformMngt_DuplicateProcessEntity";
	case CdmwPlatformMngt_DuplicateProcessService: return "CdmwPlatformMngt_DuplicateProcessService";
	case CdmwPlatformMngt_DuplicateProcessStep: return "CdmwPlatformMngt_DuplicateProcessStep";
	case CdmwPlatformMngt_DuplicateStep: return "CdmwPlatformMngt_DuplicateStep";
	case CdmwPlatformMngt_DuplicateHost: return "CdmwPlatformMngt_DuplicateHost";
	case CdmwPlatformMngt_DuplicateRunSetElement: return "CdmwPlatformMngt_DuplicateRunSetElement";
	case CdmwPlatformMngt_AlreadyDone: return "CdmwPlatformMngt_AlreadyDone";
	case CdmwPlatformMngt_IncompatibleStatus: return "CdmwPlatformMngt_IncompatibleStatus";
	case CdmwPlatformMngt_ManagedElementNotFound: return "CdmwPlatformMngt_ManagedElementNotFound";
	case CdmwPlatformMngt_ProcessAlreadyExists: return "CdmwPlatformMngt_ProcessAlreadyExists";
	case CdmwPlatformMngt_ProcessNotFound: return "CdmwPlatformMngt_ProcessNotFound";
	case CdmwPlatformMngt_ProcessHostNotFound: return "CdmwPlatformMngt_ProcessHostNotFound";
	case CdmwPlatformMngt_ObserverNotFound: return "CdmwPlatformMngt_ObserverNotFound";
	case CdmwPlatformMngt_HostNotFound: return "CdmwPlatformMngt_HostNotFound";
	case CdmwPlatformMngt_ReferenceNotFound: return "CdmwPlatformMngt_ReferenceNotFound";
	case CdmwPlatformMngt_CircularReference: return "CdmwPlatformMngt_CircularReference";
	case CdmwPlatformMngt_StepOutOfBound: return "CdmwPlatformMngt_StepOutOfBound";
	case CdmwPlatformMngt_StepJump: return "CdmwPlatformMngt_StepJump";
	case CosPropertyService_InvalidPropertyName: return "CosPropertyService_InvalidPropertyName";
	case CosPropertyService_ConflictingProperty: return "CosPropertyService_ConflictingProperty";
	case CosPropertyService_UnsupportedTypeCode: return "CosPropertyService_UnsupportedTypeCode";
	case CosPropertyService_UnsupportedProperty: return "CosPropertyService_UnsupportedProperty";
	case CosPropertyService_UnsupportedMode: return "CosPropertyService_UnsupportedMode";
	case CosPropertyService_ReadOnlyProperty: return "CosPropertyService_ReadOnlyProperty";
	case CosPropertyService_MultipleExceptions: return "CosPropertyService_MultipleExceptions";
	case CosPropertyService_FixedProperty: return "CosPropertyService_FixedProperty";
	case CosPropertyService_PropertyNotFound: return "CosPropertyService_PropertyNotFound";
	case CdmwPlatformMngt_PropertyInListNotFound: return "CdmwPlatformMngt_PropertyInListNotFound";
	case CdmwPlatformMngt_PropertyObserverNotFound: return "CdmwPlatformMngt_PropertyObserverNotFound";
	case CdmwPlatformMngt_EntityStatusNotAllowed: return "CdmwPlatformMngt_EntityStatusNotAllowed";
	case CdmwPlatformMngt_CommandAlreadyExists: return "CdmwPlatformMngt_CommandAlreadyExists";
	case CdmwPlatformMngt_CommandArgumentAlreadyExists: return "CdmwPlatformMngt_CommandArgumentAlreadyExists";

	case  CdmwPlatformMngt_RunSetNotFound: return "CdmwPlatformMngt_RunSetNotFound";
	case CdmwPlatformMngt_HostAlreadyExists:return "CdmwPlatformMngt_HostAlreadyExists";
	case CdmwPlatformMngt_ApplicationAlreadyExists:return "CdmwPlatformMngt_ApplicationAlreadyExists";

	case CdmwPlatformMngt_ApplicationNotFound: return "CdmwPlatformMngt_ApplicationNotFound";
	case CdmwPlatformMngt_ApplicationStillActive: return "CdmwPlatformMngt_ApplicationStillActive"; 
	case CdmwPlatformMngt_EntityNotFound: return "CdmwPlatformMngt_EntityNotFound";
	case CdmwPlatformMngt_EntityAlreadyExists: return "CdmwPlatformMngt_EntityAlreadyExists";
	case CdmwPlatformMngtService_ServiceAlreadyExists: return "CdmwPlatformMngtService_ServiceAlreadyExists";
	case CdmwPlatformMngtService_ServiceNotFound: return "CdmwPlatformMngtService_ServiceNotFound";
	case CdmwPlatformMngt_ProcessPropertyExceptions: return "CdmwPlatformMngt_ProcessPropertyExceptions";
	case CdmwPlatformMngt_DuplicateProcessCommand: return "CdmwPlatformMngt_DuplicateProcessCommand";
	case CdmwPlatformMngt_DuplicateProcessCommandArg: return "CdmwPlatformMngt_DuplicateProcessCommandArg";
	case CdmwPlatformMngt_DuplicateEntityCommand: return "CdmwPlatformMngt_DuplicateEntityCommand";
	case CdmwPlatformMngt_DuplicateEntityCommandArg: return "CdmwPlatformMngt_DuplicateEntityCommandArg";
	case CdmwPlatformMngt_DuplicateCommand: return "CdmwPlatformMngt_DuplicateCommand";
	case CdmwPlatformMngt_DuplicateCommandArg: return "CdmwPlatformMngt_DuplicateCommandArg";

	case CdmwPlatformMngt_ProcessStillActive: return "CdmwPlatformMngt_ProcessStillActive";
	case CdmwPlatformMngt_IncompatibleType: return "CdmwPlatformMngt_IncompatibleType";
	case CdmwPlatformMngt_NotReadyToRun: return "CdmwPlatformMngt_NotReadyToRun";
	case CdmwPlatformMngt_InvalidStep: return "CdmwPlatformMngt_InvalidStep";
	default:
	    // Unknown test modus!  Add to list abough!!
	    CDMW_NEVER_HERE();
 	}
    }



void raiseExceptionIfExceptionModus() 
{
    switch (getTestModus()) {
    case Cdmw::PlatformMngt::Proxy::NoException:
	break;
    case Cdmw::PlatformMngt::Proxy::NotImplemented:
       throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT, 
				  CORBA::COMPLETED_NO);

    case Cdmw::PlatformMngt::Proxy::CORBA_INTERNAL:
	throw CORBA::INTERNAL();

     case Cdmw::PlatformMngt::Proxy::CORBA_BAD_PARAM:
	throw CORBA::BAD_PARAM();
	
    case Cdmw::PlatformMngt::Proxy::CORBA_NO_MEMORY:
	throw CORBA::NO_MEMORY();
	
    case Cdmw::PlatformMngt::Proxy::CORBA_MARSHAL:
	throw CORBA::MARSHAL();
	
    case Cdmw::PlatformMngt::Proxy::CORBA_BAD_INV_ORDER:
	throw CORBA::BAD_INV_ORDER();

    case Cdmw::PlatformMngt::Proxy::CORBA_TRANSIENT:
	throw CORBA::TRANSIENT();

    case Cdmw::PlatformMngt::Proxy::CORBA_OBJECT_NOT_EXIST:
	throw CORBA::OBJECT_NOT_EXIST();

    case CdmwPlatformMngt_DuplicateEntity:
	throw CdmwPlatformMngt::DuplicateEntity();

    case CdmwPlatformMngt_DuplicateService:
	throw CdmwPlatformMngt::DuplicateService();
	
    case CdmwPlatformMngt_DuplicateProcess:
	throw CdmwPlatformMngt::DuplicateProcess();

    case  CdmwPlatformMngt_DuplicateProcessEntity:
	throw CdmwPlatformMngt::DuplicateProcessEntity();
	
    case CdmwPlatformMngt_DuplicateProcessService:
	throw CdmwPlatformMngt::DuplicateProcessService();
	
    case CdmwPlatformMngt_DuplicateProcessStep:
	throw CdmwPlatformMngt::DuplicateProcessStep();
	
    case CdmwPlatformMngt_DuplicateStep:
	throw CdmwPlatformMngt::DuplicateStep();

    case CdmwPlatformMngt_DuplicateHost:
	throw CdmwPlatformMngt::DuplicateHost();

    case CdmwPlatformMngt_DuplicateRunSetElement:
	throw CdmwPlatformMngt::DuplicateRunSetElement();

    case   CdmwPlatformMngt_AlreadyDone:
	throw CdmwPlatformMngt::AlreadyDone();

    case  CdmwPlatformMngt_IncompatibleStatus:
	throw CdmwPlatformMngt::IncompatibleStatus();

    case  CdmwPlatformMngt_ManagedElementNotFound:
	throw CdmwPlatformMngt::ManagedElementNotFound();
	
    case  CdmwPlatformMngt_ProcessAlreadyExists:
	throw CdmwPlatformMngt::ProcessAlreadyExists();

    case  CdmwPlatformMngt_ProcessNotFound:
	throw CdmwPlatformMngt::ProcessNotFound();

    case  CdmwPlatformMngt_ProcessHostNotFound:
	throw CdmwPlatformMngt::ProcessHostNotFound();

    case  CdmwPlatformMngt_ObserverNotFound:
	throw CdmwPlatformMngt::ObserverNotFound();
	
    case  CdmwPlatformMngt_HostNotFound:
	throw CdmwPlatformMngt::HostNotFound();

    case  CdmwPlatformMngt_ReferenceNotFound:
	throw CdmwPlatformMngt::ReferenceNotFound();

    case  CdmwPlatformMngt_CircularReference:
	throw CdmwPlatformMngt::CircularReference();

    case  CdmwPlatformMngt_StepOutOfBound:
	throw CdmwPlatformMngt::StepOutOfBound();

    case  CdmwPlatformMngt_StepJump:
	throw CdmwPlatformMngt::StepJump();
	
    case  CosPropertyService_InvalidPropertyName:
	throw CosPropertyService::InvalidPropertyName();

    case  CosPropertyService_ConflictingProperty:
	throw CosPropertyService::ConflictingProperty();

    case  CosPropertyService_UnsupportedTypeCode:
	throw CosPropertyService::UnsupportedTypeCode();

    case  CosPropertyService_UnsupportedProperty:
	throw CosPropertyService::UnsupportedProperty();

    case  CosPropertyService_UnsupportedMode:
	throw CosPropertyService::UnsupportedMode();

    case  CosPropertyService_ReadOnlyProperty:
	throw CosPropertyService::ReadOnlyProperty();

    case  CosPropertyService_MultipleExceptions:
	throw CosPropertyService::MultipleExceptions();

    case  CosPropertyService_FixedProperty:
	throw CosPropertyService::FixedProperty();

    case  CosPropertyService_PropertyNotFound:
	throw CosPropertyService::PropertyNotFound();
	
    case CdmwPlatformMngt_PropertyInListNotFound:
	throw CdmwPlatformMngt::PropertyInListNotFound();

    case CdmwPlatformMngt_PropertyObserverNotFound:
	throw CdmwPlatformMngt::PropertyObserverNotFound();
	
    case CdmwPlatformMngt_EntityStatusNotAllowed:
	throw CdmwPlatformMngt::EntityStatusNotAllowed();
	
    case CdmwPlatformMngt_CommandAlreadyExists:
	throw CdmwPlatformMngt::CommandAlreadyExists();

    case CdmwPlatformMngt_CommandArgumentAlreadyExists:
	throw CdmwPlatformMngt::CommandArgumentAlreadyExists();
	
    case CdmwPlatformMngt_RunSetNotFound:
	throw CdmwPlatformMngt::RunSetNotFound();

    case CdmwPlatformMngt_HostAlreadyExists:
	throw CdmwPlatformMngt::HostAlreadyExists();

    case CdmwPlatformMngt_ApplicationAlreadyExists:
	throw CdmwPlatformMngt::ApplicationAlreadyExists();

    case CdmwPlatformMngt_ApplicationNotFound:
	throw CdmwPlatformMngt::ApplicationNotFound();
		
    case CdmwPlatformMngt_ApplicationStillActive:
	throw CdmwPlatformMngt::ApplicationStillActive(); 
		
    case CdmwPlatformMngt_EntityNotFound:
	throw CdmwPlatformMngt::EntityNotFound(); 
		
    case CdmwPlatformMngt_EntityAlreadyExists:
	throw CdmwPlatformMngt::EntityAlreadyExists();
	
    case CdmwPlatformMngtService_ServiceAlreadyExists:
	throw CdmwPlatformMngtService::ServiceAlreadyExists();
		
    case CdmwPlatformMngtService_ServiceNotFound:
	throw CdmwPlatformMngtService::ServiceNotFound();
	
    case CdmwPlatformMngt_ProcessPropertyExceptions:
	throw CdmwPlatformMngt::ProcessPropertyExceptions();
	
    case CdmwPlatformMngt_DuplicateProcessCommand:
	throw CdmwPlatformMngt::DuplicateProcessCommand();
	
    case CdmwPlatformMngt_DuplicateProcessCommandArg:
	throw CdmwPlatformMngt::DuplicateProcessCommandArg();
		 
    case CdmwPlatformMngt_DuplicateEntityCommand:
	throw CdmwPlatformMngt::DuplicateEntityCommand();
		
    case CdmwPlatformMngt_DuplicateEntityCommandArg:
	throw CdmwPlatformMngt::DuplicateEntityCommandArg();
		
    case CdmwPlatformMngt_DuplicateCommand:
	throw CdmwPlatformMngt::DuplicateCommand();
	
    case CdmwPlatformMngt_DuplicateCommandArg:
	throw CdmwPlatformMngt::DuplicateCommandArg();

    case CdmwPlatformMngt_ProcessStillActive:
	throw CdmwPlatformMngt::ProcessStillActive();
    case CdmwPlatformMngt_IncompatibleType:
	throw CdmwPlatformMngt::IncompatibleType();
    case CdmwPlatformMngt_NotReadyToRun:
	throw CdmwPlatformMngt::NotReadyToRun();
    case CdmwPlatformMngt_InvalidStep:
	throw CdmwPlatformMngt::InvalidStep();
	
    default:
	 // Unknown test modus!  Add to list abough!!
	CDMW_NEVER_HERE();
    }
}




    void raiseSystemExceptionIf(bool raise_exception) 
    {
	if (raise_exception) {
	    throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL,
				  CORBA::COMPLETED_NO);
	}
    }

   unsigned long
   countApplicationIteratorElementsAndDestroy
   (CdmwPlatformMngt::ApplicationsIterator_ptr it) 
    {
	unsigned long result = 0;
	CdmwPlatformMngt::Application_var application;

	// FIXME use nextn operation

	// fore each element contained in reference iterator
	while (it->next_one (application.out())) { 
	    // FIXME trigger some operations on application
	    ++result;
	}

	it->destroy();

	return result;
    }

    

}; // namespace Proxy    
}; // namespace PlatformMngt
}; // namespace Cdmw
