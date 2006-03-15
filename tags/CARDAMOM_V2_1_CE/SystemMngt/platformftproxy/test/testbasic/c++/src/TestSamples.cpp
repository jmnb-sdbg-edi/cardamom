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
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <algorithm>
#include <list>
 
#include "testbasic/TestSamples.hpp"

namespace {
//     static const char           *APPLICATION_NAMES[] = {"app0",
// 						       "app1",
// 						       "app2",
// 						       "app3",
// 						       "app4",
// 						       "app5",
// 						       "app6",
// 						       NULL};


    
//     static const char           *HOST_NAMES[]      = {"host0",
// 						     "host1",
// 						     "host2",
// 						     "host3",
// 						     "host4",
// 						     NULL};

//     static const char           *PROCESS_NAMES[]  = {"proc0",
// 						     "proc1",
// 						     "proc2",
// 						     "proc3",
// 						     "proc4",
// 						     NULL};

    static const char           *PROPERTY_NAMES[]   = {"prop0",
						      "prop1",
						      "prop2",
						      "prop3",
						      "prop4",
						      "prop5",
						      NULL};

 

    static const char           *ELEMENT_NAMES[]   = {"elem0",
						     "elem1",
						     "elem2",
						     "elem3",
						     "elem4",
						     "elem5",
						     "elem6",
						     "elem7",
						     "elem8",
						     "elem9",
						     NULL};

//     static const char           *TestSamples::PARENT_NAME;
//     static const char           *OBSERVER_NAMES[ ]  = {"observer0", 
// 						      NULL};
    
//     static const char           *SERVICE_NAMES[]    = {"service0", 
// 						      NULL};

//     static const char           *RUN_SET_NAMES[]    = {"runset0", 
// 						      NULL};
    CORBA::ULong vectorLength(const char* argv[]) {
	CORBA::ULong cnt = 0;

	// increment cnt until NULL string found in list
	for (int i = 0; argv[i]!=NULL; ++i)
	    ++cnt;

	return cnt;
    }
    
    CdmwPlatformMngt::EntityStatusValues*
    toEntityStatusValues(const char *argv[]) {
	 CORBA::ULong len = vectorLength(argv);

	 CdmwPlatformMngt::EntityStatusValues* result =
	     new CdmwPlatformMngt::EntityStatusValues();
	 
	 result->length(len);
	
	 for (unsigned int i=0; i<len; ++i) {
	     (*result)[i]  = CORBA::string_dup(argv[i]);
	 }

	 return result;
     }

    CdmwPlatformMngt::ElementNames*
    toElementNames(const char *argv[]) {
	 CORBA::ULong len = vectorLength(argv);

	 CdmwPlatformMngt::ElementNames* result =
	     new CdmwPlatformMngt::ElementNames();
	
	 result->length(len);

	 for (unsigned int i=0; i<len; ++i) {
	     (*result)[i]  = CORBA::string_dup(argv[i]);
	 }

	 return result;
     }
    
    CdmwPlatformMngt::ElementPaths*
     toElementPaths(const char *argv[]) {
	 CORBA::ULong  len = vectorLength(argv);
	
	 CdmwPlatformMngt::ElementPaths* result = 
	     new CdmwPlatformMngt::ElementPaths();
	
	 result->length(len);
	 for (CORBA::ULong i=0; i<len; ++i) {
	  
	     (*result)[i]  = CORBA::string_dup(argv[i]);
	 }

	 return result;
     }   

     CosPropertyService::PropertyNames*
     toPropertyNames(const char *argv[]) {
	 CORBA::ULong len = vectorLength(argv);
	 
	 CosPropertyService::PropertyNames* result =
	     new CosPropertyService::PropertyNames();
	 
	 result->length(len);
	
	 for (unsigned int i=0; i<len; ++i) {
	     (*result)[i]  = CORBA::string_dup(argv[i]);
	 }

	 return result;
     }
    
    CosPropertyService::Properties*
    toProperties(const char **names, const char **values) {
	CORBA::ULong len = vectorLength(names);
	
	CosPropertyService::Properties*  result =
	    new CosPropertyService::Properties();
	
	result->length(len);
	
	for (unsigned int i=0; i<len; ++i) {
	    CosPropertyService::Property *elem = &(*result)[i];
	    
	    elem->property_name  = CORBA::string_dup(names[i]); // deep copy
	    elem->property_value <<= static_cast<const char*> (values[i]); // deep copy
	    
// 	    (*result)[i]  = elem; // deep copy
	}
	
	 return result;
     }
    
    bool 
    hasElement(const char *lst[], const char *elem) 
	throw()
    {
	if (elem==NULL || lst == NULL) 
	    return false;

	for (unsigned long i=0; lst[i]!=NULL; ++i) {
	    if (strcmp(lst[i],elem)==0) {
		return true;
	    }
	} 
	return false;
    }

};


namespace Cdmw {
namespace PlatformMngt {
namespace Proxy {

    char* 
    TestSamples::getPlatformApplicationExe() throw()
    {
	return CORBA::string_dup("foobar.exe");
    }

    char*
    TestSamples::getEntityStatus() 
	throw () 
    {
	return CORBA::string_dup(ELEMENT_NAMES[0]);
    }
    
    CdmwPlatformMngt::EntityStatusValues*
    TestSamples::getEntityStatusValues() 
	throw () 
    {
	return toEntityStatusValues(ELEMENT_NAMES);
    }


    CdmwPlatformMngt::ElementNames*
    TestSamples::getElementNames() 
	throw () 
    {
	return toElementNames(ELEMENT_NAMES);
    }

    CdmwPlatformMngt::ElementPaths*
    TestSamples::getElementPaths() 
	throw () 
    {
	return toElementPaths(ELEMENT_NAMES);
    }

//     CdmwPlatformMngt::ElementPaths*
//     TestSamples::getApplicationIds() 
// 	throw () 
//     {
// 	return toElementPaths(APPLICATION_NAMES);
//     }

    char* TestSamples::getObserverName() throw()
    {
	return CORBA::string_dup("observer");
    }

    char* TestSamples::getRunSetName() throw() 
    {
	return CORBA::string_dup("runsetname"); 
    }

    CosPropertyService::PropertyNames*
    TestSamples::getPropertyNames() 
	throw () 
    {
	return toPropertyNames(PROPERTY_NAMES);
    }

    CosPropertyService::Properties*
    TestSamples::getProperties() 
	throw () 
    {
	return toProperties(PROPERTY_NAMES, PROPERTY_NAMES);
    }

    CORBA::Any* 
     TestSamples::getPropertyValue() 
	throw () 
    {
	CORBA::Any *value = new CORBA::Any();
	*value <<= static_cast<const char*>(PROPERTY_NAMES[0]);
	return value;
    }


    CosPropertyService::PropertyTypes*
    TestSamples::getPropertyTypes()
	throw ()
    {
	CosPropertyService::PropertyTypes *result = 
	    new CosPropertyService::PropertyTypes();

	result->length(1);

	CORBA::Any a; 
	a <<= "foo";
	(*result)[0] = a.type();

	return result;
    }

    
    CosPropertyService::PropertyModeType
    TestSamples::getPropertyModeType()
	throw () 
    {
	return CosPropertyService::fixed_normal;
    }

    //  FIXME tbd
 //    CosPropertyService::PropertyModes*
//     TestSamples::getPropertyModes()
// 	throw () 
//     {
// 	CosPropertyService::PropertyModeType  mode =
// 	    CosPropertyService::fixed_normal;

	
//     }
       
    bool 
    TestSamples::hasElementName(const char *elem) 
	throw()
    {
	return hasElement(ELEMENT_NAMES, elem);
    }

    bool 
    TestSamples::hasElementPath(const char *elem) 
	throw()
    {
	return hasElement(ELEMENT_NAMES, elem);
    }

    bool 
    TestSamples::hasPropertyName(const char *elem) 
	throw()
    {
	return hasElement(PROPERTY_NAMES, elem);
    }   

    bool 
    TestSamples::hasPropertyValue(const CORBA::Any& value) 
	throw()
    {
	const char *str;
	value >>= str;
	return hasElement(PROPERTY_NAMES, str);
    }
    
  
    void split (const CdmwPlatformMngt::ElementPaths *lst,
		unsigned long how_many,
		CdmwPlatformMngt::ElementPaths_out head,
		CdmwPlatformMngt::ElementPaths_out tail)
	throw()
    {
	unsigned int nhead = std::min((CORBA::ULong) how_many, lst->length());
	
	head = new CdmwPlatformMngt::ElementPaths(nhead);
	tail = new CdmwPlatformMngt::ElementPaths(lst->length()-nhead);

	head->length(nhead);
	tail->length(lst->length()-nhead);

	for (unsigned int i=0; i<lst->length(); ++i) {
	    if (i<nhead) {
		(*head)[i]=CORBA::string_dup((*lst)[i]);
	    }
	    else {
		(*tail)[i-nhead]=CORBA::string_dup((*lst)[i]);
	    }
	}

    }      
  
    void split (const CdmwPlatformMngt::ElementNames *lst,
		unsigned long how_many,
		CdmwPlatformMngt::ElementNames_out head,
		CdmwPlatformMngt::ElementNames_out tail)
	throw()
    {
	unsigned int nhead = std::min((CORBA::ULong) how_many, lst->length());
	
	head = new CdmwPlatformMngt::ElementNames(nhead);
	tail = new CdmwPlatformMngt::ElementNames(lst->length()-nhead);

	head->length(nhead);
	tail->length(lst->length()-nhead);

	for (unsigned int i=0; i<lst->length(); ++i) {
	    if (i<nhead) {
		(*head)[i]=CORBA::string_dup((*lst)[i]);
	    }
	    else {
		(*tail)[i-nhead]=CORBA::string_dup((*lst)[i]);
	    }
	}

    }      

    void split (const CosPropertyService::PropertyNames *lst,
		unsigned long how_many,
		CosPropertyService::PropertyNames_out head,
		CosPropertyService::PropertyNames_out tail)
	throw()
    {
	unsigned int nhead = std::min((CORBA::ULong) how_many, lst->length());

	head->length(nhead);
	tail->length(lst->length()-nhead);
	
	head = new CosPropertyService::PropertyNames(nhead);
	tail = new CosPropertyService::PropertyNames(lst->length()-nhead);

	for (unsigned int i=0; i<lst->length(); ++i) {
	    if (i<nhead) {
		(*head)[i]=CORBA::string_dup((*lst)[i]);
	    }
	    else {
		(*tail)[i-nhead]=CORBA::string_dup((*lst)[i]);
	    }
	}

    }     

    void split (const CosPropertyService::Properties *lst,
		unsigned long how_many,
		CosPropertyService::Properties_out head,
		CosPropertyService::Properties_out tail)
	throw()
    {
	unsigned int nhead = std::min((CORBA::ULong) how_many, lst->length());
	
	head = new CosPropertyService::Properties(nhead);
	tail = new CosPropertyService::Properties(lst->length()-nhead);

	head->length(nhead);
	tail->length(lst->length()-nhead);

	for (unsigned int i=0; i<lst->length(); ++i) {
	    if (i<nhead) {
		(*head)[i]=  CosPropertyService::Property((*lst)[i]);
	    }
	    else {
		(*tail)[i-nhead]=  CosPropertyService::Property((*lst)[i]);
	    }
	}

    }   
}; // namespace Proxy    
}; // namespace PlatformMngt
}; // namespace Cdmw
