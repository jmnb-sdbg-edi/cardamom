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


#include <iostream>
#include <stdio.h>

#include <sstream>
#include "testbasic/PILOT_System.hpp"
#include "testbasic/TestModus.hpp"
#include "testbasic/TestSamples.hpp"


#include <list>
#include <fstream>
#include <memory> // auto_ptr 

#include <sax/SAXParseException.hpp>
#include <util/PlatformUtils.hpp>


#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/common/Exception.hpp>


#include <Foundation/common/Options.hpp>
#include <Foundation/testutils/CORBATestManager.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>

#include "testbasic/SIM_OrbThread.hpp"
#include "testbasic/SIM_SystemAccessor_impl.hpp"

#include "testbasic/SIM_ApplicationAccessor_impl.hpp"
#include "testbasic/SIM_HostAccessor_impl.hpp"
#include "testbasic/SIM_ProcessAccessor_impl.hpp"
#include "testbasic/SIM_EntityAccessor_impl.hpp"
#include "testbasic/SIM_ServiceDefContainerAccessor_impl.hpp"
#include "testbasic/SIM_RWServiceDefContainerAccessor_impl.hpp"
#include "testbasic/SIM_RWEntityContainerAccessor_impl.hpp"

#include <FaultTolerance/ftreplicationmanager/InitUtils.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>

#include <SystemMngt/idllib/CdmwPlatformMngtProcessObserver.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtSystem.stub.hpp>
#include <SystemMngt/idllib/CdmwFTSystMngtSystem.stub.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtCmdResponseCallback.stub.hpp>
#include <SystemMngt/platformftproxy/InitUtils.hpp>
#include <SystemMngt/platformftproxy/AccessorLookupService.hpp>
#include <SystemMngt/platformftproxy/DefaultAccessorLookupService.hpp>
#include <SystemMngt/platformlibrary/Iterator.hpp>
#include <SystemMngt/idllib/CdmwPlatformMngtProcess.stub.hpp>

#include <SystemMngt/platformvaluetypes/GraphElement_impl.hpp>
#include <SystemMngt/platformvaluetypes/ProcessDef_impl.hpp>

using namespace Cdmw;

using namespace Common;

using namespace PlatformMngt;

using namespace CdmwPlatformMngt;

using namespace Cdmw::PlatformMngt::Proxy;

CPPUNIT_TEST_SUITE_REGISTRATION( PILOT_System );

namespace Cdmw
{
namespace PlatformMngt
{
namespace Proxy
{


/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/

PILOT_System::PILOT_System()
    throw()
{

}


PILOT_System::~PILOT_System()
    throw()
{

}

CdmwFTSystMngt::SystemAccessor_ptr 
PILOT_System::createSystemAccessor() 
    throw ()
{
    /* create more SIM_ servants here and use as parameter to
       construct new SIM_SystemAccessor_impl */ 
     Cdmw::PlatformMngt::Proxy::SIM_SystemAccessor_impl* 
	 theSystemAccessorImpl = // CHECKME lifetime
	 new Cdmw::PlatformMngt::Proxy::SIM_SystemAccessor_impl();
    
    CdmwFTSystMngt::SystemAccessor_ptr theSystemAccessor =
	theSystemAccessorImpl->_this();
    
    return theSystemAccessor;
}



void
PILOT_System::testManagedElement
(CdmwPlatformMngt::ManagedElement_ptr obj) 
    throw()
{
    testPropertyManager(obj);

//         readonly attribute ElementName name;
    {
	CORBA::String_var name;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (name = obj->name(),
	     
	     CPPUNIT_ASSERT(TestSamples::hasElementName(name.in())),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound);
    }  


//         readonly attribute ElementPath element_path;
    {
	CORBA::String_var element_path;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (element_path = obj->element_path(),
	     	     
	     CPPUNIT_ASSERT(TestSamples::hasElementName(element_path.in())),

	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound);
    }    


//         ManagedElement get_parent ();
    {
	CORBA::String_var name;
	CdmwPlatformMngt::ManagedElement_var parent;
	
	
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (parent = obj->get_parent(),
	     
	     CPPUNIT_ASSERT( ! CORBA::is_nil(parent.in()) ),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound);
    }    


}

void
PILOT_System::testPropertySet
(CosPropertyService::PropertySet_ptr obj) 
    throw()
{

    
    CosPropertyService::PropertyNames_var property_names = 
	TestSamples::getPropertyNames();

    CosPropertyService::Properties_var properties = 
	TestSamples::getProperties();

    
//         void define_property(in PropertyName property_name,
//                      in any property_value)
//             raises(InvalidPropertyName,
//                ConflictingProperty,
//                UnsupportedTypeCode,
//                UnsupportedProperty,
//                ReadOnlyProperty);
    {

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->define_property((*properties)[0].property_name.in(), 
				  (*properties)[0].property_value),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CosPropertyService_InvalidPropertyName +
	     CosPropertyService_ConflictingProperty +
	     CosPropertyService_UnsupportedTypeCode +
	     CosPropertyService_UnsupportedProperty +
	     CosPropertyService_ReadOnlyProperty);
    }    


//         void define_properties(in Properties nproperties)
//             raises(MultipleExceptions);
    {

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->define_properties(properties.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CosPropertyService_MultipleExceptions);
    }    


//         unsigned long get_number_of_properties();

    {
	CORBA::ULong num = 0; 

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (num = obj->get_number_of_properties(),
	     
	     CPPUNIT_ASSERT(properties->length() == num),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound);
    }    
    

//         void get_all_property_names(in unsigned long how_many,
//                         out PropertyNames property_names,
//                         out PropertyNamesIterator rest);
    {
	CORBA::ULong how_many = 2;
	CosPropertyService::PropertyNames_var tokens;
	CosPropertyService::PropertyNamesIterator_var rest;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->get_all_property_names(how_many,
					 tokens.out(),
					 rest.out()),
	     
	     CPPUNIT_ASSERT((tokens->length() <= how_many) &&
			! CORBA::is_nil(rest.in()) &&
			(countIteratorElementsAndDestroy<CosPropertyService::PropertyNamesIterator_ptr, CosPropertyService::PropertyName_var>(rest.in()) + 
			 tokens->length() == 
			 properties->length())),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented );
	     // 	     Cdmw::PlatformMngt::Proxy::NoException + 
	     // 	     TEST_MINIMUM_EXCEPTION_SET + 
	     // 	     CdmwPlatformMngt_ManagedElementNotFound );
    }    
    


//         any get_property_value(in PropertyName property_name)
//             raises(PropertyNotFound, InvalidPropertyName);

    {
	CORBA::Any_var property_value;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (property_value = obj->get_property_value((*property_names)[0].in()),
	     
	     CPPUNIT_ASSERT(TestSamples::hasPropertyValue(property_value.in())),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound + 
	     CosPropertyService_PropertyNotFound +
	     CosPropertyService_InvalidPropertyName );
    } 
    

//         boolean get_properties(in PropertyNames property_names,
//                        out Properties nproperties);

    {
	CosPropertyService::Properties_var nproperties;

	bool result = false;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result = obj->get_properties(property_names.in(), nproperties),
	      
	      CPPUNIT_ASSERT( nproperties->length() == properties->length() ), // FIXME check contents of nproperties
	      
	      Cdmw::PlatformMngt::Proxy::NoException + 
	      TEST_MINIMUM_EXCEPTION_SET + 
	      CdmwPlatformMngt_ManagedElementNotFound);
    }    
    

//         void get_all_properties(in unsigned long how_many,
//                     out Properties nproperties,
//                     out PropertiesIterator rest);

    
    {
	CORBA::ULong how_many = 2;
	CosPropertyService::Properties_var tokens;
	CosPropertyService::PropertiesIterator_var rest;
	
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->get_all_properties(how_many,
				     tokens.out(),
				     rest.out()),
	     
	     
	     CPPUNIT_ASSERT((tokens->length() <= how_many) &&
			! CORBA::is_nil(rest.in()) &&
			(countIteratorElementsAndDestroy<CosPropertyService::PropertiesIterator_ptr, CosPropertyService::Property_var>(rest.in()) + 
			 tokens->length() == properties->length())),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented);
	     // Cdmw::PlatformMngt::Proxy::NoException + 
	     // TEST_MINIMUM_EXCEPTION_SET + 
	     // CdmwPlatformMngt_ManagedElementNotFound);
    }   
    

//         void delete_property(in PropertyName property_name)
//             raises(PropertyNotFound, InvalidPropertyName, FixedProperty);
    {
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->delete_property((*property_names)[0].in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CosPropertyService_PropertyNotFound +
	     CosPropertyService_InvalidPropertyName + 
	     CosPropertyService_FixedProperty);
    } 

//         void delete_properties(in PropertyNames property_names)
//             raises(MultipleExceptions);
    {		
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->delete_properties(property_names.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CosPropertyService_MultipleExceptions);
    } 

//         boolean delete_all_properties();
    {
	CORBA::Boolean result = false;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->delete_all_properties(), // FIXME tbd check return value
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound);
    } 

//         boolean is_property_defined(in PropertyName property_name)
//             raises(InvalidPropertyName);

    {
	CORBA::Boolean result = false;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->is_property_defined((*property_names)[0].in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CosPropertyService_InvalidPropertyName );
    } 

}

void
PILOT_System::testPropertySetDef
(CosPropertyService::PropertySetDef_ptr obj) 
    throw()
{
    testPropertySet(obj);

    
    CosPropertyService::PropertyNames_var property_names = 
	TestSamples::getPropertyNames();

    CosPropertyService::Properties_var properties = 
	TestSamples::getProperties();

    CosPropertyService::PropertyTypes_var property_types_check =
	    TestSamples::getPropertyTypes();

//         void get_allowed_property_types(out PropertyTypes property_types);
    {
	CosPropertyService::PropertyTypes_var property_types =
	    TestSamples::getPropertyTypes();

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->get_allowed_property_types(property_types.out()),
	     
	     CPPUNIT_ASSERT(property_types->length() ==
			property_types_check->length()),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound );
    }  

//         void get_allowed_properties(out PropertyDefs property_defs);
    {
	CosPropertyService::PropertyDefs_var property_defs;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->get_allowed_properties(property_defs.out()), // FIXME tbd
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound);
    }

//         void define_property_with_mode(in PropertyName property_name,
//                            in any property_value,
//                            in PropertyModeType property_mode)
//             raises(InvalidPropertyName,
//                ConflictingProperty,
//                UnsupportedTypeCode,
//                UnsupportedProperty,
//                UnsupportedMode,
//                ReadOnlyProperty);
    {
	CosPropertyService::PropertyModeType property_mode = 
	    TestSamples::getPropertyModeType();
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->define_property_with_mode((*properties)[0].property_name.in(),
					    (*properties)[0].property_value,
					    property_mode),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CosPropertyService_InvalidPropertyName +
	     CosPropertyService_ConflictingProperty +
	     CosPropertyService_UnsupportedTypeCode +
	     CosPropertyService_UnsupportedProperty +
	     CosPropertyService_UnsupportedMode +
	     CosPropertyService_ReadOnlyProperty);
    } 

//         void define_properties_with_modes(in PropertyDefs property_defs)
//             raises(MultipleExceptions);
    {	
	CosPropertyService::PropertyDefs_var property_defs = 
	    TestSamples::getPropertyDefs(); 
	
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->define_properties_with_modes(property_defs.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CosPropertyService_MultipleExceptions);
    } 
        


//         PropertyModeType get_property_mode(in PropertyName property_name)
//             raises(PropertyNotFound, InvalidPropertyName);
    {	
	CosPropertyService::PropertyModeType result; 
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->get_property_mode((*property_names)[0].in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CosPropertyService_PropertyNotFound +
	     CosPropertyService_InvalidPropertyName);
    } 

//         boolean get_property_modes(in PropertyNames property_names,
//                        out PropertyModes property_modes);
    {	
	CosPropertyService::PropertyNames_var property_names =
	    TestSamples::getPropertyNames(); 

	CosPropertyService::PropertyModes_var modes =
	    TestSamples::getPropertyModes();

	CORBA::Boolean result = false;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->get_property_modes(property_names.in(), 
					      modes.out()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound);
    } 


//         void set_property_mode(in PropertyName property_name,
//                        in PropertyModeType property_mode)
//             raises(InvalidPropertyName, PropertyNotFound, UnsupportedMode);

    {	
	CosPropertyService::PropertyModeType property_mode =
	    TestSamples::getPropertyModeType();
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_property_mode((*property_names)[0].in(),
				    property_mode),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound + 
	     CosPropertyService_InvalidPropertyName +
	     CosPropertyService_PropertyNotFound +
	     CosPropertyService_UnsupportedMode);
    } 
    

//         void set_property_modes(in PropertyModes property_modes)
//             raises(MultipleExceptions);
    {	
	CosPropertyService::PropertyModes_var modes = 
	    TestSamples::getPropertyModes();
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_property_modes(modes.in()),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound + 
	     CosPropertyService_MultipleExceptions);
    } 

}

void
PILOT_System::testPropertyManager
(CdmwPlatformMngt::PropertyManager_ptr obj) 
    throw()
{
    testPropertySetDef(obj);

//         SupervisionObserver register_all_properties_observer (
//             in string observer_name,
//             in SupervisionObserver observer,
//             in boolean deeply);
    {	
	CORBA::String_var observer_name = 
	    TestSamples::getObserverName();

	CdmwPlatformMngt::SupervisionObserver_var observer = 
	    TestSamples::getSupervisionObserver();

	CdmwPlatformMngt::SupervisionObserver_var result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->register_all_properties_observer
	     (observer_name.in(), 
	      observer.in(),
	      true),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound);
    } 

//         SupervisionObserver unregister_all_properties_observer (in string observer_name)
//             raises(ObserverNotFound);
                {	
	CORBA::String_var observer_name = 
	    TestSamples::getObserverName();
	    
	CdmwPlatformMngt::SupervisionObserver_var observer = 
	    TestSamples::getSupervisionObserver();

	CdmwPlatformMngt::SupervisionObserver_var result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->unregister_all_properties_observer
	     (observer_name.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CdmwPlatformMngt_ObserverNotFound);
    } 

//         void register_property_observer (
//             in string observer_name,
//             in SupervisionObserver observer,
//             in CosPropertyService::PropertyNames property_names)
//             raises(PropertyInListNotFound);


    {	
	CORBA::String_var observer_name = 
	    TestSamples::getObserverName();
	
	CdmwPlatformMngt::SupervisionObserver_var observer = 
	    TestSamples::getSupervisionObserver();

	CosPropertyService::PropertyNames_var property_names =
	    TestSamples::getPropertyNames();

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->register_property_observer
	     (observer_name.in(),
	      observer.in(),
	      property_names.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CdmwPlatformMngt_PropertyInListNotFound);
    } 

//         void unregister_property_observer (
//             in string observer_name,
//             in CosPropertyService::PropertyNames property_names)
//             raises(PropertyInListNotFound,
//                    PropertyObserverNotFound);
    
    {	
	CORBA::String_var observer_name = 
	    TestSamples::getObserverName();
	
	CosPropertyService::PropertyNames_var property_names; // FIXME full list
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->unregister_property_observer
	     (observer_name.in(),
	      property_names.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_ManagedElementNotFound +
	     CdmwPlatformMngt_PropertyInListNotFound +
	     CdmwPlatformMngt_PropertyObserverNotFound);
    } 
}


void PILOT_System::testObservable
(CdmwPlatformMngt::Observable_ptr obj) throw() 
{
    CdmwPlatformMngt::ElementPaths_var element_paths =
	TestSamples::getElementPaths();

//         SupervisionObserver register_observer(
//             in string observer_name,
//             in SupervisionObserver observer);
    
    {  
	CORBA::String_var observer_name = 
	    TestSamples::getObserverName();

	CdmwPlatformMngt::SupervisionObserver_var observer = 
	    TestSamples::getSupervisionObserver();

	CdmwPlatformMngt::SupervisionObserver_var result;

	// System::register_observer
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (result = obj->register_observer 
	     (observer_name.in(), observer.in()),
	     
	     CPPUNIT_ASSERT(true),

	     Cdmw::PlatformMngt::Proxy::NotImplemented);
// 	     Cdmw::PlatformMngt::Proxy::NoException + 
// 	     TEST_MINIMUM_EXCEPTION_SET);
    }


//         SupervisionObserver register_proxy_observer(
//             in string observer_name,
//             in ElementName host_name,
//             in SupervisionObserver observer);
    {  
	CORBA::String_var observer_name = 
	    TestSamples::getObserverName();

	CORBA::String_var host_name     = 
	    CORBA::string_dup((*element_paths)[0]);

	CdmwPlatformMngt::SupervisionObserver_var observer = 
	    TestSamples::getSupervisionObserver();

	CdmwPlatformMngt::SupervisionObserver_var result;

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (result = obj->register_proxy_observer 
	     (observer_name.in(),
	      host_name.in(),
	      observer.in()),
	     
	     CPPUNIT_ASSERT(true),

	     Cdmw::PlatformMngt::Proxy::NotImplemented);
// 	     Cdmw::PlatformMngt::Proxy::NoException + 
// 	     TEST_MINIMUM_EXCEPTION_SET);
    }
  
//         SupervisionObserver unregister_observer(in string observer_name)
//             raises(ObserverNotFound);
    {
	CORBA::String_var observer_name = 
	    TestSamples::getObserverName();

	CdmwPlatformMngt::SupervisionObserver_var result;

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (result = obj->unregister_observer(observer_name.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented);
// 	     Cdmw::PlatformMngt::Proxy::NoException + 
// 	     TEST_MINIMUM_EXCEPTION_SET);
    }

}

void PILOT_System::testSystem
(CdmwPlatformMngt::ManagedElement_ptr managedElem) throw() 
{
    SIM_ManagedElementAccessor_impl::setParentType
	(CdmwPlatformMngt::SYSTEM_TYPE);

    TEST_INFO("Get System Object invoking parent() operation");

    CdmwPlatformMngt::ManagedElement_var parent = 
	managedElem->get_parent();

    TEST_INFO("Get System Object invoking parent() operation done");
      
    CPPUNIT_ASSERT ( ! CORBA::is_nil(parent.in()));
    if ( CORBA::is_nil(parent.in()) ) 
	return;

    TEST_INFO("Narrow System Object");

    CdmwPlatformMngt::System_var obj = 
	CdmwPlatformMngt::System::_narrow (parent.in());
    
    CPPUNIT_ASSERT ( ! CORBA::is_nil(obj.in()));
    if ( CORBA::is_nil(obj.in()) ) 
	return;
    
    testManagedElement(obj.in());
    testObservable(obj.in());
    testHostContainer(obj.in());
    testRWEntityContainer(obj.in());
    testRWServiceDefContainer(obj.in());

    CdmwPlatformMngt::ElementPaths_var element_paths =
	TestSamples::getElementPaths();

//         SystemStatus get_status(out string status_info);
    {
	CORBA::String_var out_result;
	CdmwPlatformMngt::SystemStatus status;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (status = 
	     obj->get_status(out_result.out()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
    
//         FunctioningMode get_mode(out string mode_info);
    {  
	CORBA::String_var out_result = CORBA::string_dup("");
	CdmwPlatformMngt::FunctioningMode mode;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (mode = 
	     obj->get_mode(out_result.out()),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         SystemSnapshot get_snapshot();
    {  
	CdmwPlatformMngt::SystemSnapshot_var snapshot;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (snapshot = obj->get_snapshot(),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         Applications get_applications(in unsigned long how_many,
//             out ApplicationsIterator applications_iterator);
    {  
	CORBA::ULong how_many = 2;
	CdmwPlatformMngt::ApplicationsIterator_var it;
	CdmwPlatformMngt::Applications_var apps;

	CORBA::ULong itLength = 0;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( apps = 
	     obj->get_applications(how_many, it),
	     
	     CPPUNIT_ASSERT((apps->length() <= how_many) &&
			! CORBA::is_nil(it.in()) &&
			((itLength = countIteratorElementsAndDestroy<CdmwPlatformMngt::ApplicationsIterator_ptr, CdmwPlatformMngt::Application_var>(it.in())) + 
			 apps->length() == 
			 element_paths->length())),
	     
	     TEST_MINIMUM_EXCEPTION_SET +
	     Cdmw::PlatformMngt::Proxy::NoException);
	
	if (! CORBA::is_nil(it.in())) {
	    std::ostringstream temp;
	    temp << "Expected length: "  << element_paths->length();
	    temp << " Vector length: "   << apps->length();
	    temp << " Iterator length: " << itLength;
	    TEST_INFO(temp.str());
	}
    }
    

//         Application get_application(in ElementName application_name)
//             raises(ApplicationNotFound);
    {  
	CORBA::String_var application_name = 
	    CORBA::string_dup((*element_paths)[0]);

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (CdmwPlatformMngt::Application_var app = 
	     obj->get_application (application_name.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
    

//         void define(in SystemDef system_def)
//             raises(DuplicateEntity, CosPropertyService::MultipleExceptions,
//                    DuplicateService, DuplicateHost,
//                    AlreadyDone, IncompatibleStatus);

    {  
	CdmwPlatformMngt::SystemDef_var systemDef; //FIXME tbd

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->define (systemDef.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_DuplicateEntity +
	     CdmwPlatformMngt_DuplicateService +
	     CdmwPlatformMngt_DuplicateHost +
	     CdmwPlatformMngt_AlreadyDone +
	     CdmwPlatformMngt_IncompatibleStatus );
    }
    


//         void set_init_graph(in GraphElements application_graph)
//             raises(ReferenceNotFound, CircularReference, IncompatibleStatus);
    {  
	CdmwPlatformMngt::GraphElements_var applicationGraph =
	    TestSamples::getGraphElements();
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_init_graph(applicationGraph.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_ReferenceNotFound +
	     CdmwPlatformMngt_CircularReference +
	     CdmwPlatformMngt_IncompatibleStatus);
    
    }


//         void set_stop_graph(in GraphElements application_graph)
//             raises(ReferenceNotFound, CircularReference, IncompatibleStatus);
    { 
	CdmwPlatformMngt::GraphElements_var applicationGraph; // FIXME read  from file
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->set_stop_graph(applicationGraph.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_ReferenceNotFound +
	     CdmwPlatformMngt_CircularReference +
	     CdmwPlatformMngt_IncompatibleStatus);
    }


//         void set_run_set(in RunSetDef run_set_def)
//             raises(DuplicateRunSetElement, IncompatibleStatus);
 
    {
	CdmwPlatformMngt::RunSetDef_var runSetDef; // FIXME tbd
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->set_run_set(runSetDef.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_DuplicateRunSetElement +
	     CdmwPlatformMngt_IncompatibleStatus);
    }
    
//         void remove_run_set(in RunSetName run_set_name)
//             raises(RunSetNotFound, IncompatibleStatus);
    {
	CORBA::String_var run_set_name = 
	    TestSamples::getRunSetName();

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->remove_run_set(run_set_name.in()),

	     CPPUNIT_ASSERT(true),

	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_RunSetNotFound +
	     CdmwPlatformMngt_IncompatibleStatus);
    }


//         void remove_all_run_sets() raises(IncompatibleStatus);
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->remove_all_run_sets(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_IncompatibleStatus);
    }
    
//         Host add_host(in HostDef host_def)
//             raises(HostAlreadyExists, DuplicateEntity, 
//                    CosPropertyService::MultipleExceptions, IncompatibleStatus);
    {
	CdmwPlatformMngt::HostDef_var hostDef;  // FIXME tbd

	CdmwPlatformMngt::Host_var result;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->add_host(hostDef.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	      Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_HostAlreadyExists +
	     CdmwPlatformMngt_IncompatibleStatus);
    }
    
//         void remove_host(in ElementName host_name)
//             raises(HostNotFound, IncompatibleStatus);
    {
	CORBA::String_var host_name =
	    CORBA::string_dup((*element_paths)[0]);

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->remove_host(host_name.in()),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_HostNotFound +
	     CdmwPlatformMngt_IncompatibleStatus);
    }


//         Application add_application(
//                 in PlatformApplicationDef platform_application_def,
//                 in ApplicationDef application_def)
//             raises(ApplicationAlreadyExists, HostNotFound,
//             DuplicateEntity, CosPropertyService::MultipleExceptions, 
//             DuplicateService, DuplicateProcess, ProcessHostNotFound,
//             DuplicateProcessEntity, ProcessPropertyExceptions,
//             DuplicateProcessService, DuplicateProcessStep,
//             DuplicateProcessCommand, DuplicateProcessCommandArg,
//             DuplicateEntityCommand, DuplicateEntityCommandArg,
//             IncompatibleStatus);

    // System::add_application
    {
	CdmwPlatformMngt::PlatformApplicationDef_var platformAppDef; // FIXME valid

	CdmwPlatformMngt::ApplicationDef_var appDef;         // FIXME valid
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->add_application(platformAppDef.in(), appDef.in()),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_ApplicationAlreadyExists+
	     CdmwPlatformMngt_HostNotFound+
	     CdmwPlatformMngt_DuplicateEntity+
// 	     CdmwPlatformMngt_DuplicateService+
	     CdmwPlatformMngt_DuplicateProcess+
	     CdmwPlatformMngt_ProcessHostNotFound+
	     CdmwPlatformMngt_DuplicateProcessEntity+
	     CdmwPlatformMngt_DuplicateProcessService+
	     CdmwPlatformMngt_DuplicateProcessStep+
	     CdmwPlatformMngt_IncompatibleStatus);
    }



//         void remove_application(in ElementName application_name)
//             raises(ApplicationNotFound, ApplicationStillActive, IncompatibleStatus);
    {
	CORBA::String_var application_name =
	    CORBA::string_dup((*element_paths)[0]);
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->remove_application(application_name.in()),

	     CPPUNIT_ASSERT(true),

	      Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_ApplicationNotFound +
	     CdmwPlatformMngt_ApplicationStillActive +
	     CdmwPlatformMngt_IncompatibleStatus);	
    }


//         void start(in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
    {
	CdmwPlatformMngtBase::StartupKind_var startup_kind;
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->start(startup_kind.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }



//         void resume()
//             raises(IncompatibleStatus);
    {

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->resume(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }


//         void acknowledge()
//             raises(IncompatibleStatus);
    {

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->acknowledge(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }


//         void stop(in boolean emergency);
    {
	CORBA::Boolean emergency = false;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->stop(emergency),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }



        
//         void cold_restart(in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
            
    {
	CdmwPlatformMngtBase::StartupKind_var startup_kind;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->cold_restart(startup_kind.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }


        
//         void hot_restart(in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
    {
	CdmwPlatformMngtBase::StartupKind_var startup_kind;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->hot_restart(startup_kind.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }
}

void PILOT_System::testPullMonitorable
(CdmwPlatformMngt::PullMonitorable_ptr obj) throw() 
{
//     void is_alive();
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->is_alive(),
	     
	     CPPUNIT_ASSERT(true),

	     Cdmw::PlatformMngt::Proxy::NotImplemented);

// 	     Cdmw::PlatformMngt::Proxy::NoException + 
// 	     TEST_MINIMUM_EXCEPTION_SET );
    }
}


void PILOT_System::testApplication
(CdmwPlatformMngt::ManagedElement_ptr managedElem) throw() 
{
    SIM_ManagedElementAccessor_impl::setParentType
	(CdmwPlatformMngt::APPLICATION_TYPE);
    
    CdmwPlatformMngt::ManagedElement_var parent = 
	managedElem->get_parent();

    CPPUNIT_ASSERT ( ! CORBA::is_nil(parent.in()));
    if ( CORBA::is_nil(parent.in()) ) 
	return;

    CdmwPlatformMngt::Application_var obj = 
	CdmwPlatformMngt::Application::_narrow (parent.in());
    
    CPPUNIT_ASSERT ( ! CORBA::is_nil(obj.in()));
    if ( CORBA::is_nil(obj.in()) ) 
	return;
    
    testManagedElement(obj.in());
    testObservable(obj.in());
    testPullMonitorable(obj.in());
    testRWEntityContainer(obj.in());
    testRWServiceDefContainer(obj.in());
    
    CdmwPlatformMngt::ElementPaths_var element_paths =
	TestSamples::getElementPaths();

//     readonly attribute RestartType restart_type;
    {
	CdmwPlatformMngt::RestartType result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = 
	     obj->restart_type(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//     readonly attribute long restart_attempt;
    {
	CORBA::Long result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = 
	     obj->restart_attempt(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

// boolean is_manual_starting();
    {
	CORBA::Boolean  result = false;
	

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->is_manual_starting(),
	     
	     CPPUNIT_ASSERT(result == TestSamples::getManualStarting()),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

// set_manual_starting(in boolean manual_starting)
// 	raises(IncompatibleStatus);
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_manual_starting(TestSamples::getManualStarting()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }
    
            
//     ApplicationStatus get_status(out string status_info);
    {
	CORBA::String_var out_result;
	CdmwPlatformMngt::ApplicationStatus status;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (status = 
	     obj->get_status(out_result.out()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
    
//     FunctioningMode get_mode(out string mode_info);
    {  
	CORBA::String_var out_result = CORBA::string_dup("");
	CdmwPlatformMngt::FunctioningMode mode;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (mode = 
	     obj->get_mode(out_result.out()),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//     ApplicationSnapshot get_snapshot();
    {  
	CdmwPlatformMngt::ApplicationSnapshot_var snapshot;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (snapshot = obj->get_snapshot(),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

    
//     Processes get_processes(in unsigned long how_many,
//             out ProcessesIterator processes_iterator );
    {  
	CORBA::ULong how_many = 2;
	CdmwPlatformMngt::ProcessesIterator_var it = 
	    CdmwPlatformMngt::ProcessesIterator::_nil();
	CdmwPlatformMngt::Processes_var elems =
	    new CdmwPlatformMngt::Processes();

	CORBA::ULong itLength = 0;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (elems = 
	     obj->get_processes(how_many, it),

	     CPPUNIT_ASSERT((elems->length() <= how_many) && 
			! CORBA::is_nil(it.in()) &&
			((itLength = countIteratorElementsAndDestroy<CdmwPlatformMngt::ProcessesIterator_ptr, CdmwPlatformMngt::Process_var>(it.in())) + 
			 elems->length() == 
			 element_paths->length())),
	     
	     TEST_MINIMUM_EXCEPTION_SET +
	     Cdmw::PlatformMngt::Proxy::NoException);

	if (!CORBA::is_nil(it.in())) {
	    std::ostringstream temp;
	    temp << "Expected length: " << element_paths->length();
	    temp << " Vector length: "  << elems->length();
	    temp << " Iterator length: " << itLength;
	    TEST_INFO(temp.str());
	}
    }
	

//     Process get_process(in ElementName process_name,
// 			in ElementName host_name)
// 	raises(ProcessNotFound);
    {  
	CdmwPlatformMngt::ElementName_var process_name =
	    TestSamples::getElementName();

	CdmwPlatformMngt::ElementName_var host_name =
	    TestSamples::getElementName();
	
	CdmwPlatformMngt::Process_var result; 

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result = 
	     obj->get_process(process_name.in(), host_name.in()),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_ProcessNotFound );
    }
	
//         void define(in ApplicationDef application_def)
//             raises(DuplicateEntity, CosPropertyService::MultipleExceptions, 
//             DuplicateService, DuplicateProcess, ProcessHostNotFound,
//             DuplicateProcessEntity, ProcessPropertyExceptions, 
//             DuplicateProcessService, DuplicateProcessStep,
//             DuplicateProcessCommand, DuplicateProcessCommandArg,
//             DuplicateEntityCommand, DuplicateEntityCommandArg,
//             AlreadyDone, IncompatibleStatus);
    {  
	CdmwPlatformMngt::ApplicationDef_var appl_def =
	    TestSamples::getApplicationDef();

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->define(appl_def.in()),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_DuplicateEntity +
               CdmwPlatformMngt_DuplicateService+
  //              CdmwPlatformMngt_DuplicateHost+
               CdmwPlatformMngt_AlreadyDone+
               CdmwPlatformMngt_IncompatibleStatus);
    }

//         Process add_process(in ProcessDef process_def)
//             raises(ProcessAlreadyExists, HostNotFound,
//             DuplicateEntity, CosPropertyService::MultipleExceptions,
//             DuplicateService, DuplicateStep,
//             DuplicateCommand, DuplicateCommandArg,
//             DuplicateEntityCommand, DuplicateEntityCommandArg,
//             IncompatibleStatus);
    {  
	CdmwPlatformMngt::ProcessDef_var process_def =
	    TestSamples::getProcessDef();

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (CdmwPlatformMngt::Process_var result = 
	     obj->add_process(process_def.in()) ,
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented);

// 	     Cdmw::PlatformMngt::Proxy::NoException + 
// 	     TEST_MINIMUM_EXCEPTION_SET +
// 	     CdmwPlatformMngt_ProcessAlreadyExists + 
// 	     CdmwPlatformMngt_HostNotFound + 
// 	     CdmwPlatformMngt_DuplicateEntity + 
// 	     CosPropertyService_MultipleExceptions + 
	     
// 	     CdmwPlatformMngt_DuplicateService + 
// 	     CdmwPlatformMngt_DuplicateStep + 
// 	     CdmwPlatformMngt_DuplicateCommand + 
// 	     CdmwPlatformMngt_DuplicateCommandArg + 
// 	     CdmwPlatformMngt_DuplicateEntityCommand  + 
// 	     CdmwPlatformMngt_DuplicateEntityCommandArg + 
// 	     CdmwPlatformMngt_IncompatibleStatus    );
    }
    //         void remove_process(in ElementName process_name,
//                             in ElementName host_name)
//             raises(ProcessNotFound, ProcessStillActive, IncompatibleStatus);
    {  
	CdmwPlatformMngt::ElementName_var process_name =
	    TestSamples::getElementName();

	CdmwPlatformMngt::ElementName_var host_name =
	    TestSamples::getElementName();
	
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->remove_process(process_name.in(), host_name.in()),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented);

// 	     Cdmw::PlatformMngt::Proxy::NoException + 
// 	     TEST_MINIMUM_EXCEPTION_SET +
// 	     CdmwPlatformMngt_ProcessNotFound +
// 	     CdmwPlatformMngt_ProcessStillActive +
// 	     CdmwPlatformMngt_IncompatibleStatus);
    }
	
//         void set_init_graph(in GraphElements process_graph)
//             raises(ReferenceNotFound, CircularReference,
//             StepOutOfBound, StepJump, IncompatibleStatus);

    {  
	CdmwPlatformMngt::GraphElements_var processGraph;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_init_graph(processGraph.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_ReferenceNotFound +
	     CdmwPlatformMngt_CircularReference +
	     CdmwPlatformMngt_StepOutOfBound +
	     CdmwPlatformMngt_StepJump +
	     CdmwPlatformMngt_IncompatibleStatus);
    
    }

//         void set_stop_graph(in GraphElements process_graph)
//             raises(ReferenceNotFound, CircularReference, IncompatibleStatus);
    { 
	CdmwPlatformMngt::GraphElements_var processGraph; // FIXME read  from file
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->set_stop_graph(processGraph.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_ReferenceNotFound +
	     CdmwPlatformMngt_CircularReference +
	     CdmwPlatformMngt_IncompatibleStatus);
    }


    
//         ApplicationAutoRestartInfo get_autorestart_info();
    {  
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (CdmwPlatformMngt::ApplicationAutoRestartInfo_var result =
	     obj->get_autorestart_info(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET );
	
    }
//         void set_autorestart_info(in ApplicationAutoRestartInfo autorestart_info)
//             raises(IncompatibleStatus);
    {  
	CdmwPlatformMngt::ApplicationAutoRestartInfo_var info = 
	    TestSamples::getApplicationAutoRestartInfo();
	     
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_autorestart_info(info.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    
    }  

//         void initialise(in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
    {  
	CdmwPlatformMngtBase::StartupKind_var kind = 
	    TestSamples::getStartupKind();
	     
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->initialise(kind.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    
    }  
    
//         LongRequestStatus initialise_and_wait(
//             in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
    {  
	CdmwPlatformMngtBase::StartupKind_var kind = 
	    TestSamples::getStartupKind();
	
	CdmwPlatformMngt::LongRequestStatus result;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result =
	     obj->initialise_and_wait(kind.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    
    }  
//         void resume()
//             raises(IncompatibleStatus);
    {

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->resume(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }
    
    //         LongRequestStatus resume_and_wait()
    //             raises(IncompatibleStatus);
    
    {  	
	CdmwPlatformMngt::LongRequestStatus result;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result =
	     obj->resume_and_wait(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }  

//         void acknowledge()
//             raises(IncompatibleStatus);
    {

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->acknowledge(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }


//         void run()
//             raises(IncompatibleStatus);

//         void stop(in boolean emergency);
    {
	CORBA::Boolean emergency = false;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->stop(emergency),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }


//         LongRequestStatus stop_and_wait(in boolean emergency);
    {  
	CdmwPlatformMngt::LongRequestStatus result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result =
	     obj->stop_and_wait(true),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET );
    }  
        
//         void cold_restart(in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
    {
	CdmwPlatformMngtBase::StartupKind_var startup_kind;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->cold_restart(startup_kind.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }

//         void hot_restart(in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
    {
	CdmwPlatformMngtBase::StartupKind_var startup_kind;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->hot_restart(startup_kind.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }



}

void PILOT_System::testCommandedElement
(CdmwPlatformMngt::CommandedElement_ptr obj) throw() 
{

//         CommandStatus get_command_status(out string status_info);
    {
	CORBA::String_var info;
	CdmwPlatformMngt::CommandStatus status;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    ( status = 
	     obj->get_command_status(info.out()),
	     
	     CPPUNIT_ASSERT(true),
	     
	    Cdmw::PlatformMngt::Proxy::NotImplemented );
    }

//         void call_command (
//             in string command_name,
//             in CmdParameterValues command_args,
//             in CommandResponseCallback response_callback)
//             raises(CommandNotFound,
//                    CommandArgumentInvalid,
//                    CommandRejected);
    {
	const char * command_name = "command exe";
	CdmwPlatformMngt::CmdParameterValues_var command_args = 
	    TestSamples::getCmdParameterValues();
	
	CdmwPlatformMngt::CommandResponseCallback_var callback =
	    TestSamples::getCommandResponseCallback();
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->call_command
	     (command_name, 
	      command_args.in(),
	      CdmwPlatformMngt::CommandResponseCallback::_nil()),
	     
	     CPPUNIT_ASSERT(true),
	     Cdmw::PlatformMngt::Proxy::NotImplemented );
    }

}

void PILOT_System::testCommandContainer
(CdmwPlatformMngt::CommandContainer_ptr obj) throw() 
{
    testCommandedElement(obj);

//         void add_command (
//             in string command_name,
//             in Timeout time_out,
//             in CmdParameterDescriptors command_args)
//             raises(CommandAlreadyExists,
//                    CommandArgumentAlreadyExists);
    {
	const char * command_name = "command exe";
	CdmwPlatformMngt::Timeout time_out = 0;
	CdmwPlatformMngt::CmdParameterDescriptors_var descr;

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->add_command
	     (command_name, 
	      time_out,
	      descr.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented );
    }

//         void remove_command (
//             in string command_name)
//             raises(CommandNotFound);
    {
	const char * command_name = "command exe";
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->remove_command(command_name),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented );
    }
}

void PILOT_System::testEntityContainer
(CdmwPlatformMngt::EntityContainer_ptr obj) throw() 
{

    CdmwPlatformMngt::ElementPaths_var element_paths =
	TestSamples::getElementPaths();
    
    CdmwPlatformMngt::ElementNames_var element_names =
	TestSamples::getElementNames();
    
// unsigned long get_number_of_entities();
    {
	CORBA::ULong result;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (result = obj->get_number_of_entities(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         void get_all_entity_names(in unsigned long how_many,
//             out ElementNames entity_names,
//             out ElementNamesIterator rest);
    

    {
	CORBA::ULong how_many = 2;
	CdmwPlatformMngt::ElementNames_var         elems;
	CdmwPlatformMngt::ElementNamesIterator_var it;

	CORBA::ULong itLength = 0;

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->get_all_entity_names(how_many, elems, it),
	     
	     CPPUNIT_ASSERT((elems->length() <= how_many) &&
			! CORBA::is_nil(it.in()) &&
			((itLength = countIteratorElementsAndDestroy<CdmwPlatformMngt::ElementNamesIterator_ptr, CdmwPlatformMngt::ElementName_var>(it.in())) + 
			 elems->length() == element_paths->length())),
	     
	     TEST_MINIMUM_EXCEPTION_SET +
	     Cdmw::PlatformMngt::Proxy::NoException);

	if (! CORBA::is_nil(it.in())) {
	    std::ostringstream temp;
	    temp << "Expected length: " << element_paths->length();
	    temp << " Vector length: " << elems->length();
	    temp << " Iterator length: " << itLength;
	    TEST_INFO(temp.str());
	}
     
    }
//         Entity get_entity(in ElementName entity_name)
//             raises(EntityNotFound);
    {
	CORBA::String_var name; // empty
	
	CdmwPlatformMngt::Entity_var entity;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (entity = obj->get_entity(name.in()),
	     
	     CPPUNIT_ASSERT(true),		       
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_EntityNotFound); 
    }


//         boolean get_entities(in ElementNames entity_names,
//             out Entities nentities);
   {
	CdmwPlatformMngt::ElementNames_var names; // empty
	
	CdmwPlatformMngt::Entities_var elems;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->get_entities(names.in(),
			       elems.out()),
	     
	     CPPUNIT_ASSERT(true), // FIXME iterate the sequence		       
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);	     
    }
//         void get_all_entities(in unsigned long how_many,
//             out Entities nentities,
//             out EntitiesIterator rest);
    {
	CORBA::ULong how_many = 2;
	CdmwPlatformMngt::Entities_var         entities;
	CdmwPlatformMngt::EntitiesIterator_var it;

	CORBA::ULong itLength = 0;

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->get_all_entities(how_many, entities.out(), it.out()),
	     
	     CPPUNIT_ASSERT((entities->length() <= how_many) &&
			! CORBA::is_nil(it.in()) &&
			((countIteratorElementsAndDestroy<CdmwPlatformMngt::EntitiesIterator_ptr, CdmwPlatformMngt::Entity_var>(it.in())) + 
			 entities->length() == element_paths->length())),
	     
	     TEST_MINIMUM_EXCEPTION_SET +
	     Cdmw::PlatformMngt::Proxy::NoException);

	if (! CORBA::is_nil(it.in())) {
	    std::ostringstream temp;
	    temp << "Expected length: " << element_paths->length();
	    temp << " Vector length: " <<  entities->length();
	    temp << " Iterator length: " << itLength;
	    TEST_INFO(temp.str());
	}
    }
}

void PILOT_System::testRWEntityContainer
(CdmwPlatformMngt::RWEntityContainer_ptr obj) throw() 
{
    testEntityContainer(obj);

//         Entity add_entity(in EntityDef entity_def)
//             raises(EntityAlreadyExists, CosPropertyService::MultipleExceptions,
//                    DuplicateCommand, DuplicateCommandArg);
    {
	CdmwPlatformMngt::EntityDef_var entityDef;
	
	CdmwPlatformMngt::Entity_var entity;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (entity = obj->add_entity(entityDef.in()),

	     CPPUNIT_ASSERT(true),		       
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_EntityAlreadyExists  );	   
    }


//         void remove_entity(in ElementName entity_name)
//             raises(EntityNotFound);
    {
	CORBA::String_var name = 
	    TestSamples::getElementName();

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->remove_entity(name.in()),

	     CPPUNIT_ASSERT(true), 		       
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_EntityNotFound);	   
    }
  

    
}

void PILOT_System::testServiceDefContainer
(CdmwPlatformMngtService::ServiceDefContainer_ptr obj) throw() 
{

//         ServiceProviderID get_service_def (in ServiceName service_name)
//             raises(ServiceNotFound);
    {
	CORBA::String_var name = 
	    TestSamples::getElementName();
	
	CdmwPlatformMngtService::ServiceProviderID_var id;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->get_service_def(name.in()),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
}

void PILOT_System::testRWServiceDefContainer
(CdmwPlatformMngtService::RWServiceDefContainer_ptr obj) throw() 
{
    testServiceDefContainer(obj);
    

//         void add_service_def (in ServiceDef service_def)
//             raises(ServiceAlreadyExists);
    {
	CdmwPlatformMngtService::ServiceDef_var service_def ;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->add_service_def(service_def.in()),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngtService_ServiceAlreadyExists);
    }
    


//         void remove_service_def (in ServiceName service_name)
//             raises(ServiceNotFound);
   {
	CORBA::String_var name = 
	    TestSamples::getElementName();

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->remove_service_def(name.in()),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngtService_ServiceNotFound);
    }

//         void remove_all_service_defs ();
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->remove_all_service_defs(),

	     CPPUNIT_ASSERT(true), 		       
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);	   
    }

}

void PILOT_System::testProcess
(CdmwPlatformMngt::ManagedElement_ptr managedElem) throw() 
{
    SIM_ManagedElementAccessor_impl::setParentType
	(CdmwPlatformMngt::PROCESS_TYPE);
    
    CdmwPlatformMngt::ManagedElement_var parent = 
	managedElem->get_parent();

    CPPUNIT_ASSERT ( ! CORBA::is_nil(parent.in()));
    if ( CORBA::is_nil(parent.in()) ) 
	return;

    CdmwPlatformMngt::Process_var obj = 
	CdmwPlatformMngt::Process::_narrow (parent.in());
    
    CPPUNIT_ASSERT ( ! CORBA::is_nil(obj.in()));
    if ( CORBA::is_nil(obj.in()) ) 
	return;
    
    testManagedElement(obj.in());  
    testCommandContainer(obj.in());
    testRWEntityContainer(obj.in());
    testRWServiceDefContainer(obj.in());
    
    CdmwPlatformMngt::ElementPaths_var element_paths =
	TestSamples::getElementPaths();
    
//         readonly attribute ProcessType type;
    {
	CdmwPlatformMngt::ProcessType result;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result = 
	     obj->type(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
//         readonly attribute Object embedded_service;
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (CORBA::Object_var result = 
	     obj->embedded_service(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
  
//         readonly attribute ProcessPID pid;
    {
	CdmwPlatformMngt::ProcessPID result;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result = 
	     obj->pid(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
    
//         readonly attribute boolean created_dynamically;
    {
	CORBA::Boolean result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result = 
	     obj->created_dynamically(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         readonly attribute RestartType restart_type;
    {
	CdmwPlatformMngt::RestartType result;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result = 
	     obj->restart_type(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         readonly attribute long restart_attempt;
    {
	CORBA::Long result;
    
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result = 
	     obj->restart_attempt(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         readonly attribute CdmwPlatformMngtBase::TimeStamp start_time;
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (CdmwPlatformMngtBase::TimeStamp_var result = 
	     obj->start_time(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
    
//         readonly attribute CdmwPlatformMngtBase::TimeStamp stop_time;
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (CdmwPlatformMngtBase::TimeStamp_var result = 
	     obj->stop_time(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
    
// 	boolean is_manual_starting();
    {
	CORBA::Boolean result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result = 
	     obj->is_manual_starting(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         void set_manual_starting(in boolean manual_starting)
//             raises(IncompatibleStatus);
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_manual_starting(TestSamples::getManualStarting()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_IncompatibleStatus);
    }
    
//         ProcessStatus get_status(out string status_info);
    {
	CORBA::String_var out_result;
	CdmwPlatformMngt::ProcessStatus status;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (status = 
	     obj->get_status(out_result.out()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
    //         boolean is_auto_ending();
        
//         void set_auto_ending(in boolean auto_ending)
//             raises(IncompatibleStatus);
    
//         Host get_host();
    {
	CORBA::String_var name = 
	    CORBA::string_dup((*element_paths)[0]);
	
	CdmwPlatformMngt::Host_var result;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (result = obj->get_host(),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
    
	  

//         ProcessInfo get_process_info();
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (CdmwPlatformMngt::ProcessInfo_var result =
	     obj->get_process_info(),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
    
//         void set_process_info(in ProcessInfo process_info)
//             raises(IncompatibleStatus);
    {
	CdmwPlatformMngt::ProcessInfo_var info;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->set_process_info(info.in()),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_IncompatibleStatus);		   
    }
  
//         LifeCycleDef get_life_cycle()
//             raises(IncompatibleType);
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (CdmwPlatformMngt::LifeCycleDef_var  result =
	     obj->get_life_cycle(),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_IncompatibleType);
    }
    //         void set_life_cycle(in LifeCycleDef life_cycle)
    //             raises(DuplicateStep, IncompatibleType, IncompatibleStatus);
    {
	CdmwPlatformMngt::LifeCycleDef_var def;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->set_life_cycle(def.in()),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_DuplicateStep + 
	     CdmwPlatformMngt_IncompatibleStatus +
	     CdmwPlatformMngt_IncompatibleType);		   
    }
    
//         CompleteMonitoringInfo get_monitoring_info()
//             raises(IncompatibleType);
    {
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (CdmwPlatformMngt::CompleteMonitoringInfo_var result =
	     obj->get_monitoring_info(),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);		   
    }
    
//         void set_monitoring_info(in CompleteMonitoringInfo monitoring_info)
//             raises(IncompatibleType, IncompatibleStatus);
    
    {
	CdmwPlatformMngt::CompleteMonitoringInfo_var info; // tbd
		    
	    TEST_FOREACH_ACCESSOR_EXCEPTION 
		(obj->set_monitoring_info(info.in()),
	     
		 CPPUNIT_ASSERT(true), 
		 
		 Cdmw::PlatformMngt::Proxy::NoException + 
		 TEST_MINIMUM_EXCEPTION_SET +
		 CdmwPlatformMngt_IncompatibleStatus +
		 CdmwPlatformMngt_IncompatibleType);		   
    }


//         ExitAutoRestartInfo get_exit_autorestart_info();
    {
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (CdmwPlatformMngt::ExitAutoRestartInfo_var result =
	     obj->get_exit_autorestart_info(),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);		   
    }
    
//         void set_exit_autorestart_info(in ExitAutoRestartInfo exit_autorestart_info)
//             raises(IncompatibleStatus);

    {
	CdmwPlatformMngt::ExitAutoRestartInfo_var info; // tbd
	     
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->set_exit_autorestart_info(info.in()),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_IncompatibleStatus);		   
    }
    
//         ProcessSnapshot get_snapshot();
    {  
	CdmwPlatformMngt::ProcessSnapshot_var snapshot;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (snapshot = obj->get_snapshot(),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         RequestStatus initialise(in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
    {  
	CdmwPlatformMngtBase::StartupKind_var kind;
	CdmwPlatformMngt::RequestStatus result;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result  = 
	     obj->initialise(kind.in()),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_IncompatibleStatus);
    }

//         RequestStatus next_step()
//             raises(InvalidStep, IncompatibleType, IncompatibleStatus);
    {  
	CdmwPlatformMngt::RequestStatus result;
    
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result  = 
	     obj->next_step(),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_IncompatibleStatus +
	     CdmwPlatformMngt_IncompatibleType + 
	     CdmwPlatformMngt_InvalidStep );
    }
//         RequestStatus run()
//             raises(NotReadyToRun, IncompatibleType, IncompatibleStatus);
    {  
	CdmwPlatformMngt::RequestStatus result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result  = 
	     obj->run(),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_NotReadyToRun +
	     CdmwPlatformMngt_IncompatibleType + 
	     CdmwPlatformMngt_IncompatibleStatus );
    }
//         RequestStatus stop(in boolean emergency);
    {  
	CdmwPlatformMngt::RequestStatus result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    ( result  = 
	     obj->stop(true),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET );
    }
        
//         RequestStatus cold_restart(in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
    {
	CdmwPlatformMngtBase::StartupKind_var startup_kind;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->cold_restart(startup_kind.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }

//         RequestStatus hot_restart(in CdmwPlatformMngtBase::StartupKind startup_kind)
//             raises(IncompatibleStatus);
    {
	CdmwPlatformMngtBase::StartupKind_var startup_kind;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->hot_restart(startup_kind.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET + 
	     CdmwPlatformMngt_IncompatibleStatus);
    }


    //
    // Following operations are not implemented
    //

//         void set_create_process_result (in ProcessCommandResult result, 
//                                         in string error_info,
//                                         in string error_issuer,
//                                         in ProcessDelegateWrapper process_delegate_wrap);
//     {
// 	CdmwPlatformMngt::ProcessCommandResult result =
// 	    TestSamples::getProcessCommandResult();
// 	const char* error_info = "error_info";
// 	const char* error_issuer = "error_issuer";
// 	CdmwPlatformMngt::ProcessDelegateWrapper_var process_delegate_wrap = 
// 	    CdmwPlatformMngt::ProcessDelegateWrapper::_nil();

// 	TEST_FOREACH_ACCESSOR_EXCEPTION
// 	    (obj->set_create_process_result (result, 
// 					     error_info,
// 					     error_issuer,
// 					     process_delegate_wrap.in()),
	     
// 	     CPPUNIT_ASSERT(true),
	     
// 	     Cdmw::PlatformMngt::Proxy::NotImplemented);
//     }    
 
//         void set_initialisation_result (in ProcessCommandResult result);
    {
	CdmwPlatformMngt::ProcessCommandResult result =
	    TestSamples::getProcessCommandResult();

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj-> set_initialisation_result(result),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented);
    }                         
    
//         void set_next_step_result (in ProcessCommandResult result,
//                                    in unsigned long step_nbr);
    {
	CdmwPlatformMngt::ProcessCommandResult result =
	    TestSamples::getProcessCommandResult();

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_next_step_result (result, 5),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented);
    }                     
    
//         void set_run_result (in ProcessCommandResult result)
//              raises(ProcessNotFound);
    {
	CdmwPlatformMngt::ProcessCommandResult result =
	    TestSamples::getProcessCommandResult();

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_run_result (result),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented);
    }                        
                                
//         void set_stop_result (in ProcessCommandResult result)
//              raises(ProcessNotFound);
    {
	CdmwPlatformMngt::ProcessCommandResult result =
	    TestSamples::getProcessCommandResult();

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_stop_result (result),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NotImplemented);
    }    
}

void PILOT_System::testEntity
(CdmwPlatformMngt::ManagedElement_ptr managedElem) throw() 
{
    SIM_ManagedElementAccessor_impl::setParentType
	(CdmwPlatformMngt::ENTITY_TYPE);
    
    CdmwPlatformMngt::ManagedElement_var parent = 
	managedElem->get_parent();

    CPPUNIT_ASSERT ( ! CORBA::is_nil(parent.in()));
    if ( CORBA::is_nil(parent.in()) ) 
	return;

    CdmwPlatformMngt::Entity_var obj = 
	CdmwPlatformMngt::Entity::_narrow (parent.in());
    
    CPPUNIT_ASSERT ( ! CORBA::is_nil(obj.in()));
    if ( CORBA::is_nil(obj.in()) ) 
	return;
    
    testManagedElement(obj.in());
    testCommandContainer(obj.in());

//         EntityStatus get_status(out string entity_info);
    {
	CORBA::String_var out_result;
	CdmwPlatformMngt::EntityStatus status;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (status = 
	     obj->get_status(out_result.out()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
//         void set_status(in EntityStatus entity_status, in string entity_info)
//             raises(EntityStatusNotAllowed);
    {
	CORBA::String_var entity_info;
	CdmwPlatformMngt::EntityStatus status;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->set_status(status, entity_info.in()),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_EntityStatusNotAllowed);
    }
        
//         EntityStatusValues get_status_values ();
    {
	
	CdmwPlatformMngt::EntityStatusValues_var result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->get_status_values(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET );
    }
    
//         EntitySnapshot get_snapshot();
    {  
	CdmwPlatformMngt::EntitySnapshot_var snapshot;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (snapshot = obj->get_snapshot(),

	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }


}

void PILOT_System::testHost
(CdmwPlatformMngt::ManagedElement_ptr managedElem) throw() 
{
    SIM_ManagedElementAccessor_impl::setParentType
	(CdmwPlatformMngt::HOST_TYPE);
    
    CdmwPlatformMngt::ManagedElement_var parent = 
	managedElem->get_parent();

    CPPUNIT_ASSERT ( ! CORBA::is_nil(parent.in()));
    if ( CORBA::is_nil(parent.in()) ) 
	return;

    CdmwPlatformMngt::Host_var obj = 
	CdmwPlatformMngt::Host::_narrow (parent.in());
    
    CPPUNIT_ASSERT ( ! CORBA::is_nil(obj.in()));
    if ( CORBA::is_nil(obj.in()) ) 
	return;
    
    testManagedElement(obj.in());  

    CdmwPlatformMngt::ElementPaths_var element_paths =
	TestSamples::getElementPaths();

//         readonly attribute HostStatus status;
    {
	CORBA::String_var out_result;
	CdmwPlatformMngt::HostStatus result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->status(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         readonly attribute string platform_application_exe;
    {
	CORBA::String_var result;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (result = obj->platform_application_exe(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         Processes get_processes(in unsigned long how_many,
//             out ProcessesIterator processes_iterator);
    {
	CdmwPlatformMngt::ProcessesIterator_var rest;
	CdmwPlatformMngt::Processes_var tokens;
	CORBA::ULong how_many = 2;

	CORBA::ULong itLength = 0;

	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (tokens = obj->get_processes(how_many, rest.out()),
	     
	     CPPUNIT_ASSERT((tokens->length() <= how_many) &&
			! CORBA::is_nil(rest.in()) &&
			((itLength = countIteratorElementsAndDestroy<CdmwPlatformMngt::ProcessesIterator_ptr, CdmwPlatformMngt::Process_var>(rest.in())) + 
			 tokens->length() == 
			 element_paths->length())),
	     	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);

	if (! CORBA::is_nil(rest.in())) {
	    std::ostringstream temp;
	    temp << "Expected length: "  << element_paths->length();
	    temp << " Vector length: "   << tokens->length();
	    temp << " Iterator length: " << itLength;
	    TEST_INFO(temp.str());
	}
    }

    // 
    // NOT IMPLEMENTED, deprecated or future use
    // 


    
//         void shutdown ();
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->shutdown(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         void cold_reboot ();
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->cold_reboot(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         void hot_reboot ();
    {
	TEST_FOREACH_ACCESSOR_EXCEPTION
	    (obj->hot_reboot(),
	     
	     CPPUNIT_ASSERT(true),
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }
}


void PILOT_System::testHostContainer
(CdmwPlatformMngt::HostContainer_ptr obj) throw() 
{
    CdmwPlatformMngt::ElementPaths_var element_paths =
	TestSamples::getElementPaths();

    CdmwPlatformMngt::ElementNames_var element_names =
	TestSamples::getElementNames();

//     unsigned long get_number_of_hosts();
    {
	CORBA::ULong  num;
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (num = obj->get_number_of_hosts(),
	         
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);
    }

//         void get_all_host_names(in unsigned long how_many,
//             out ElementNames host_names,
//             out ElementNamesIterator rest);
    {
	CORBA::ULong how_many = 2;
	CdmwPlatformMngt::ElementNames_var         elems;
 	CdmwPlatformMngt::ElementNamesIterator_var it;

	CORBA::ULong itLength = 0;

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->get_all_host_names(how_many, elems.out(), it.out()),
	     
	     CPPUNIT_ASSERT((elems->length() <= how_many) &&
			! CORBA::is_nil(it.in()) &&
			((itLength = countIteratorElementsAndDestroy<CdmwPlatformMngt::ElementNamesIterator_ptr, CdmwPlatformMngt::ElementName_var>(it.in())) + 
			 elems->length() == element_names->length())),
	     
	     TEST_MINIMUM_EXCEPTION_SET +
	     Cdmw::PlatformMngt::Proxy::NoException);

	if (! CORBA::is_nil(it.in())) {
	    std::ostringstream temp;
	    temp << "Expected length: "  << element_names->length();
	    temp << " Vector length: "   << elems->length();
	    temp << " Iterator length: " << itLength;
	    TEST_INFO(temp.str());
	}
    }
    
//         Host get_host(in ElementName host_name)
//             raises(HostNotFound);
     {
	CORBA::String_var name = 
	    CORBA::string_dup((*element_paths)[0]);
	
	CdmwPlatformMngt::Host_var result;

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (result = obj->get_host(name),
	     
	     CPPUNIT_ASSERT(true), 
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET +
	     CdmwPlatformMngt_HostNotFound);
    }
  

//         boolean get_hosts(
//             in ElementNames host_names,
//             out Hosts hosts);
    {
	CdmwPlatformMngt::ElementNames_var names; // empty
	
	CdmwPlatformMngt::Hosts_var hosts;
	
	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->get_hosts(names.in(),
			    hosts.out()),
	     
	     CPPUNIT_ASSERT(true), // FIXME iterate the sequence		       
	     
	     Cdmw::PlatformMngt::Proxy::NoException + 
	     TEST_MINIMUM_EXCEPTION_SET);	     
    }

//         void get_all_hosts(in unsigned long how_many,
//             out Hosts hosts,
//             out HostsIterator rest);

    {
	CORBA::ULong how_many = 2;
	CdmwPlatformMngt::Hosts_var         elems;
 	CdmwPlatformMngt::HostsIterator_var it;

	CORBA::ULong itLength = 0;

	TEST_FOREACH_ACCESSOR_EXCEPTION 
	    (obj->get_all_hosts(how_many, elems.out(), it.out()),
	     
	     CPPUNIT_ASSERT((elems->length() <= how_many) &&
			! CORBA::is_nil(it.in()) &&
			((itLength = countIteratorElementsAndDestroy<CdmwPlatformMngt::HostsIterator_ptr, CdmwPlatformMngt::Host_var>(it.in())) + 
			 elems->length() == element_paths->length())),
	     
	     TEST_MINIMUM_EXCEPTION_SET + 
	     Cdmw::PlatformMngt::Proxy::NoException );
	   
	if (! CORBA::is_nil(it.in())) {
	    std::ostringstream temp;
	    temp << "Expected length: " << element_paths->length();
	    temp << " Vector length: " << elems->length();
	    temp << " Iterator length: " << itLength;
	    TEST_INFO(temp.str());
	}
    }
    

}



void
PILOT_System::do_tests()                
{
    // set number of requested successfull tests
// //     set_nbOfRequestedTestOK (2319);
    
    TEST_INFO("Creating the ORB");
    OrbThread orbThread;
    orbThread.start();
    
    // int timescale = Cdmw::TestUtils::get_timescale();
    CORBA::ORB_var  orb = orbThread.orb();
   
    // define value-type factories
    CORBA::ValueFactoryBase_var factory, old_factory;
    
    factory = new Cdmw::PlatformMngt::GraphElementFactory();
    old_factory = orb->register_value_factory (
	    "IDL:thalesgroup.com/CdmwPlatformMngt/GraphElement:1.0",
	    factory.in() );
    
    factory = new Cdmw::PlatformMngt::ProcessDefFactory;
    old_factory = orb->register_value_factory (
            "IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDef:1.0",
            factory.in() );
                
    factory = new Cdmw::PlatformMngt::ManagedProcessDefFactory;
    old_factory = orb->register_value_factory (
            "IDL:thalesgroup.com/CdmwPlatformMngt/ManagedProcessDef:1.0",
            factory.in() );
    
    // get the ROOT-POA
    PortableServer::POA_var poa = orbThread.poa();
    
    // use this poa to manage iterators
    Cdmw::PlatformMngt::IteratorManager::initialize(poa.in());
    
//     std::string configfile("system.xml");
//     auto_ptr<TestSamples> samples( readTestSamples(configfile) );

//     CDMW_ASSERT( samples.get() != NULL );

    TEST_INFO("Create the SystemAccessor");
    CdmwFTSystMngt::SystemAccessor_var theSystemAccessor = 
	createSystemAccessor();

    CDMW_ASSERT( ! CORBA::is_nil(theSystemAccessor.in()) );

 
    Cdmw::PlatformMngt::Proxy::SystemProxy *theSystemProxyHandle = NULL;

    CdmwPlatformMngt::System_var theSystemProxy =
	CdmwPlatformMngt::System::_nil();

    const char * proxyName = "SystemMngtProxy";

    TEST_INFO("Create the Platform FT Proxy");
    try {   
	// create lookup service
	auto_ptr<Cdmw::PlatformMngt::Proxy::AccessorLookupService> lookupService 
	    (new Cdmw::PlatformMngt::Proxy::DefaultAccessorLookupService(theSystemAccessor.in())); 

	theSystemProxyHandle = 
	    Cdmw::PlatformMngt::Proxy::InitUtils::create_platformmngt_proxy
	    (orb.in(),
	     poa.in(),
	     proxyName,
	     lookupService.get()); /* proxy takes over responsibility
				      for lookup service */

	lookupService.release();

	theSystemProxy = theSystemProxyHandle->getReference();

	CPPUNIT_ASSERT(! CORBA::is_nil(theSystemProxy.in()));
    } 
    catch (const Cdmw::BadParameterException &ex) {
	CPPUNIT_ASSERT(false);
    }
    catch (const Cdmw::OutOfMemoryException &ex) {
	CPPUNIT_ASSERT(false);
    }

    TEST_INFO("Test Destruction and Recreation of Platform FT Proxy");
    // while (usleep(100*1000)>=0) {

    try {
	

	/** NIL the object reference */
	theSystemProxy =
	    CdmwPlatformMngt::System::_nil();
	
	/** control the moment, the proxy is deactivated (sync or
	 *   async); otherwise the following delete statement would
	 *   block indefinetly until all pending request have been
	 *   processed */
	theSystemProxyHandle->deactivate(true /* wait_for_completion */);

	/* if not deactivated, the proxy is going to be deactivated
	 * now with "wait_for_completion=true", blocking mode until
	 * all pending requests have been processed. If the proxy has
	 * been deactivated before, only the remaining handler will be
	 * free-ed, without any blocking. */
	delete theSystemProxyHandle;
	
	/* create lookup service */
	auto_ptr<Cdmw::PlatformMngt::Proxy::AccessorLookupService> lookupService 
	    (new Cdmw::PlatformMngt::Proxy::DefaultAccessorLookupService(theSystemAccessor.in())); 

	// re-create proxy
	theSystemProxyHandle = 
	    Cdmw::PlatformMngt::Proxy::InitUtils::create_platformmngt_proxy
	    (orb.in(),
	     poa.in(),
	     proxyName,
	     lookupService.get()); /* proxy takes over responsibility
				      for lookup service */
	
	lookupService.release();

	theSystemProxy = theSystemProxyHandle->getReference();
	
	CPPUNIT_ASSERT(! CORBA::is_nil(theSystemProxy.in()));
    } 
    catch (const Cdmw::BadParameterException &ex) {
	CPPUNIT_ASSERT(false);
    }
    catch (const Cdmw::OutOfMemoryException &ex) {
	CPPUNIT_ASSERT(false);
    }

    
    TEST_INFO("Testing Interface System ");
    testSystem(theSystemProxy.in());
    TEST_INFO("Testing Interface Application");
    testApplication(theSystemProxy.in());
    TEST_INFO("Testing Interface Process");
    testProcess(theSystemProxy.in());
    TEST_INFO("Testing Interface Entity");
    testEntity(theSystemProxy.in());
    TEST_INFO("Testing Interface Host");
    testHost(theSystemProxy.in());
    

    TEST_INFO("Stopping the orb");
    orbThread.shutdown();
    orbThread.join();
}    



} // End namespace Proxy
} // End namespace PlatformMngt
} // End namespace Cdmw


