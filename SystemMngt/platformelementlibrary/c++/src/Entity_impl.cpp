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


#include "SystemMngt/platformelementlibrary/Entity_impl.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"
#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"


namespace Cdmw
{
    namespace PlatformMngt
    {


// ----------------------------------------------------------------------
// Entity_impl class.
// ----------------------------------------------------------------------


	Entity_impl::Entity_impl (PortableServer::POA_ptr poa,
				  const CdmwPlatformMngt::EntityDef& entity_def,
				  const char* entity_path,
				  CdmwPlatformMngt::RWElementRepository_ptr element_repository)
	    throw (BadParameterException,
		   OutOfMemoryException,CosPropertyService::MultipleExceptions)
	    : DeactivableServant_impl (poa),
	      ManagedElement_impl(element_repository)                             
	{
	    try
	    {  
		try
		{
		    Cdmw::CommonSvcs::DataStore::DataStoreBase* ds =
			Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
			    Configuration::Get_entity_datastore_id());
		    m_entity_ds = 
			dynamic_cast<Cdmw::PlatformMngt::EntityDataStore*>(ds);
		}
		catch (const NotExistException& e)
		{
		    CDMW_ASSERT(false);
		}
         
		// store element path in managed element
		set_element_path (entity_path,
				  entity_def.entity_name.in());
	                     
		unsigned int propCount = entity_def.properties.length();
		for (unsigned int propIndex=0 ; propIndex < propCount ; propIndex++)
		{
		    const CdmwPlatformMngt::PropertyDef& prop_def =
			entity_def.properties[propIndex];
		    try
		    {
			std::string propertyName(entity_def.properties[propIndex].property_name);
			CosPropertyService::PropertyModeType propertyMode = CosPropertyService::PropertyModeType(0);
			CdmwPlatformMngt::PropertyModeType propertyModeCdmw = entity_def.properties[propIndex].mode_type;
			if (propertyModeCdmw == 1) propertyMode = CosPropertyService::PropertyModeType(1);
			define_property_with_mode(propertyName.c_str() , prop_def.initial_value, propertyMode);
		    }
		    catch (const CosPropertyService::InvalidPropertyName&)
		    {
                        throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		    }
		    catch (const CosPropertyService::ConflictingProperty&)
		    {
                        throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		    }
		    catch (const CosPropertyService::UnsupportedTypeCode&)
		    {
                        throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		    }
		    catch (const CosPropertyService::UnsupportedProperty&)
		    {
                        throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		    }
		    catch (const CosPropertyService::UnsupportedMode&)
		    {
                        throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		    }
		    catch (const CosPropertyService::ReadOnlyProperty&)
		    {
                        throw  CosPropertyService::MultipleExceptions(); /* TO-DO */
		    }
	
		}
                                                                                                                             
		// initialise data store of the object
		create_record_in_datastore(entity_def);
        
	    }
	    catch (const std::bad_alloc&)
	    {
		CDMW_THROW (OutOfMemoryException);
	    } 

	}


	Entity_impl::~Entity_impl ()
	{
	    remove_record_in_datastore();
	}


	void 
	Entity_impl::create_record_in_datastore(const CdmwPlatformMngt::EntityDef& entity_def)
	    throw (OutOfMemoryException)
	{  
	    // test if the supervision is not fault tolerant or if it is, is it the primary?
	    if (!Configuration::Is_fault_tolerant() || Configuration::Is_primary())
	    { 
		// set managed element
		set_element_in_datastore();
	
		CdmwPlatformMngt::EntityRecord record;
        
		record.entity_status = CORBA::string_dup ("");
		record.entity_info = CORBA::string_dup ("");
		record.status_values.length(0);
        
		try
		{
		    m_entity_ds->insert(m_element_path.in(), record);
		}
		catch (Common::AlreadyExistException&)
		{
		    std::cout << "entity record already exists in datastore : " 
			      << m_element_path.in() << std::endl;
		    CDMW_ASSERT(false);
		}  
	    }
	}


	void 
	Entity_impl::remove_record_in_datastore()
	{
	    // inhibition if backup
	    if (!Configuration::Is_fault_tolerant() || Configuration::Is_primary())
	    {
		try
		{
		    m_entity_ds->remove(m_element_path.in());
		}
		catch (Common::NotFoundException&)
		{
		}
	    }
	}

	CdmwPlatformMngt::EntityRecord* 
	Entity_impl::get_record_from_datastore()
	{
	    CdmwPlatformMngt::EntityRecord* entity_record = NULL;
    
	    try
	    {
		entity_record = m_entity_ds->select(m_element_path.in());
	    }
	    catch (Common::NotFoundException&)
	    {
		std::cout << "entity record cannot be found in datastore : " 
			  << m_element_path.in() << std::endl;
		CDMW_ASSERT(false);
	    }   
    
	    return entity_record;
	}

    

	void 
	Entity_impl::update_record_in_datastore(const CdmwPlatformMngt::EntityRecord& entity_record)
	{
	    try
	    {
		m_entity_ds->update(m_element_path.in(), entity_record);
	    }
	    catch (Common::NotFoundException&)
	    {
		std::cout << "entity record cannot be found in datastore : " 
			  << m_element_path.in() << std::endl;
		CDMW_ASSERT(false);
	    } 
	}

    /**
     *Purpose:
     *<p> facade to get an attribute
     *
     */

     void Entity_impl::get_attribute(const char * attr_name, CORBA::Any_out ret_attr  , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
   throw CORBA::NO_IMPLEMENT (Cdmw::OrbSupport::NO_IMPLEMENT, 
                              CORBA::COMPLETED_NO);
}

    /**
     *Purpose:
     *<p> facade to set an attribute
     *
     */

     void Entity_impl::set_attribute(const char * attr_name, const CORBA::Any & attr  , CORBA::Char flag)  throw (CORBA::SystemException, CdmwPlatformMngt::AttributeNotFound)
{
   throw CORBA::NO_IMPLEMENT (Cdmw::OrbSupport::NO_IMPLEMENT, 
                              CORBA::COMPLETED_NO);
}


/**
 *Purpose:
 *<p>
 * Implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_status:1.0
 * operation
 */
	char* 
	Entity_impl::get_status(CORBA::String_out entity_info)
	    throw (CORBA::SystemException)
	{
	    // get record                
	    CdmwPlatformMngt::EntityRecord_var entity_record = get_record_from_datastore();
    
	    // get entity status
	    CORBA::String_var entity_status = 
		CORBA::string_dup ((entity_record->entity_status).in());
    
	    // get info associated to status
	    entity_info = CORBA::string_dup ((entity_record->entity_info).in());

	    return entity_status._retn();
	}

/**
 *Purpose:
 *<p>
 * Implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/set_status:1.0
 * operation
 */
	void 
	Entity_impl::set_status(const char* entity_status, const char* entity_info)
	    throw (CdmwPlatformMngt::EntityStatusNotAllowed, 
		   CORBA::SystemException)
	{    
	    CdmwPlatformMngt::EntityRecord_var entity_record;
	    CdmwPlatformMngt::EntityStatus current_status;
    
	    {
		// lock for concurent access
		CDMW_MUTEX_GUARD(m_mutex_entity_record);
    
		// get current status
		entity_record = get_record_from_datastore();
		current_status = CORBA::string_dup ((entity_record->entity_status).in());
    
		// if status change
		if (strcmp(entity_status, current_status))
		{
		    // store new status
		    entity_record->entity_status = CORBA::string_dup (entity_status);
		    entity_record->entity_info = CORBA::string_dup (entity_info);
            
		    update_record_in_datastore(entity_record.in());
		}
	    }
    
	    // if status has been changed
	    if (strcmp(entity_status, current_status))
	    {
		// TODO notify status change ?
	    }
	}

    

/**
 *Purpose:
 *<p>
 * Implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_status_values:1.0
 * operation
 */
	CdmwPlatformMngt::EntityStatusValues* 
	Entity_impl::get_status_values()
	    throw (CORBA::SystemException)
	{
	    throw CORBA::NO_IMPLEMENT (Cdmw::OrbSupport::NO_IMPLEMENT, 
				       CORBA::COMPLETED_NO);

	    return 0;
	}


/**
 *Purpose:
 *<p>
 * Implements the
 * IDL:thalesgroup.com/CdmwPlatformMngt/Entity/get_snapshot:1.0
 * operation
 */
	CdmwPlatformMngt::EntitySnapshot* 
	Entity_impl::get_snapshot()
	    throw (CORBA::SystemException)
	{
	    try
	    {
		CdmwPlatformMngt::EntitySnapshot_var snapshot =
		    new CdmwPlatformMngt::EntitySnapshot;
		snapshot->entity_name = name();
		snapshot->entity_status = 
		    get_status ((snapshot->entity_info).out());

		// managing properties 

	 	CosPropertyService::PropertiesIterator_var propertiesIt;
		CORBA::ULong nbr_of_properties;
		CdmwPlatformMngt::PropertiesSnapshot_var propertiesSnapshot = new CdmwPlatformMngt::PropertiesSnapshot;
		nbr_of_properties = get_number_of_properties();
		CosPropertyService::PropertyNames_var prop_names;
		CosPropertyService::PropertyNamesIterator_var prop_namesIt;
		get_all_property_names(nbr_of_properties, prop_names.out(), prop_namesIt.out());
		propertiesSnapshot->length(nbr_of_properties);
		for (unsigned int j = 0; j < nbr_of_properties ; j++)
		{
		    CdmwPlatformMngt::PropertySnapshot* propertySnapshot = new CdmwPlatformMngt::PropertySnapshot();
		    propertySnapshot -> property_name = CORBA::string_dup(prop_names[j]);
		    CORBA::Any prop_value;
		    prop_value = *(get_property_value(prop_names[j]));
		    CORBA::TypeCode_var tc;
		    tc = prop_value.type();
		    switch (tc->kind())	
		    {
		    default:
			break;
		    case CORBA::tk_string:
			propertySnapshot -> property_type = CORBA::string_dup("String");
			break;
		    case CORBA::tk_short:
			propertySnapshot -> property_type = CORBA::string_dup("Short");
			break;
		    case CORBA::tk_float:
			propertySnapshot -> property_type = CORBA::string_dup("Float");
			break;
		    };
		    

		    CosPropertyService::PropertyModeType mode= get_property_mode(prop_names[j]);
		    propertySnapshot->property_value = prop_value;
		    if (mode==0)  propertySnapshot->property_mode = CORBA::string_dup("Read/Write");
		    else  propertySnapshot->property_mode = CORBA::string_dup("Read/Only");
		    propertySnapshot->property_value = prop_value;
		    propertiesSnapshot[j] = *(propertySnapshot);
		}

		
		snapshot->entity_properties = propertiesSnapshot.in();
	    


	


		// return snapshot sequence
		return snapshot._retn();
	    }
	    catch (const std::bad_alloc&)
	    {
		throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
	    }
	    catch (const Cdmw::OutOfResourcesException&)
	    {
		throw CORBA::NO_MEMORY (OrbSupport::NO_MEMORY, CORBA::COMPLETED_NO);
	    }
	}

    } // End namespace PlatformMngt
} // End namespace Cdmw


