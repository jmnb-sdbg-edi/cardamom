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


#include "SystemMngt/platformelementlibrary/ManagedElement_impl.hpp"

#include "SystemMngt/platformlibrary/ManagedElementKey.hpp"
#include "SystemMngt/platformlibrary/Configuration.hpp"

#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Assert.hpp"



namespace Cdmw
{
  namespace PlatformMngt
  {



// ----------------------------------------------------------------------
// ManagedElement_impl class.
// ----------------------------------------------------------------------

    ManagedElement_impl::ManagedElement_impl (CdmwPlatformMngt::
					      RWElementRepository_ptr
					      element_repository) throw ()
    {
      m_element_repository =
	CdmwPlatformMngt::RWElementRepository::
	_duplicate (element_repository);
      try
      {
	Cdmw::CommonSvcs::DataStore::DataStoreBase * ds =
	  Cdmw::CommonSvcs::DataStore::DataStoreRegistry::
	  Get_data_store (Configuration::Get_managed_element_datastore_id ());
	m_managed_element_ds =
	  dynamic_cast < Cdmw::PlatformMngt::ManagedElementDataStore * >(ds);

	std::auto_ptr < CosPropertyService::PropertyDefs >
	  userDef (new CosPropertyService::PropertyDefs);

	m_user_defined_attributes = userDef.get ();

	userDef.release ();


	m_property_set = new TAO_PropertySetDef ();

	PortableServer::ServantBase_var property_set = m_property_set;

	m_property_set_var = m_property_set->_this ();

      }
      catch (const NotExistException & e)
      {
	CDMW_ASSERT (false);
      }

      m_element_path = CORBA::string_dup ("");

      m_element_name = CORBA::string_dup ("");
    }


    ManagedElement_impl::~ManagedElement_impl ()
    {
      remove_element_in_datastore ();
    }


    char *ManagedElement_impl::name () throw (CORBA::SystemException)
    {
      CORBA::String_var name = m_element_name;

      return name._retn ();
    }

    char *ManagedElement_impl::element_path () throw (CORBA::SystemException)
    {
      CORBA::String_var path = m_element_path;

      return path._retn ();
    }


    CdmwPlatformMngt::ManagedElement_ptr ManagedElement_impl::
      get_parent ()throw (CORBA::SystemException)
    {
      CdmwPlatformMngt::ManagedElement_var parent_element;

      try
      {
	// get parent path of the managed element
	std::string parent_path;
	std::string element_name;
	std::string element_path = m_element_path.in ();

	ManagedElementKey::Get_parent_path (element_path, element_name,
					    parent_path);

	// get the parent element object from repository
	parent_element =
	  m_element_repository->get_element (parent_path.c_str ());
      }
      catch (CdmwPlatformMngt::ElementPathInvalid)
      {
	CDMW_THROW2 (BadParameterException, "element_path", "invalid");
      }


      return parent_element._retn ();
    }


    void ManagedElement_impl::set_element_path (const char *element_path,
						const char *element_name)
    {
      m_element_path = CORBA::string_dup (element_path);

      m_element_name = CORBA::string_dup (element_name);
    }


    void ManagedElement_impl::set_element_in_datastore ()
    {
      if (!Configuration::Is_fault_tolerant ()
	  || Configuration::Is_primary ())
	{
      // set in DataStore
      CdmwPlatformMngt::ManagedElementRecord record;
      record.name = m_element_name;
      record.default_user_defined_attributes = *m_user_defined_attributes;

      // adding the new UDAs in datastore
      CosPropertyService::Properties_var props;
      CORBA::ULong propCount = get_number_of_properties ();
      CosPropertyService::PropertyNames_var prop_names;
      CosPropertyService::PropertyNamesIterator_var prop_names_It;
      get_all_property_names (propCount, prop_names.out (),
			      prop_names_It.out ());
      get_properties (prop_names, props.out ());
      CosPropertyService::PropertyDefs_var propdefs =
	new CosPropertyService::PropertyDefs;
      propdefs->length (propCount);
      CosPropertyService::PropertyModes_var prop_modes;
      get_property_modes (prop_names, prop_modes.out ());

      CosPropertyService::PropertyMode property_mode_out;
      for (CORBA::ULong i = 0; i < propCount; i++)
	{
	  propdefs[i].property_name = prop_names[i];
	  propdefs[i].property_mode = property_mode_out.property_mode;
	  propdefs[i].property_value =
	    *get_property_value (props[i].property_name);
	}
      // set the default user defined attributes value
      record.user_defined_attributes = propdefs;

      // added the new UDAs in datastore 

      if (m_managed_element_ds->contains (m_element_path.in ()))
	{
	  m_managed_element_ds->update (m_element_path.in (), record);
	}
      else
	{
	  m_managed_element_ds->insert (m_element_path.in (), record);
	}
    }
      
    }

    void ManagedElement_impl::remove_element_in_datastore ()
    {
      if (!Configuration::Is_fault_tolerant ()
	  || Configuration::Is_primary ())
	{
	  try
	  {
	    m_managed_element_ds->remove (m_element_path.in ());
	  }
	  catch (Common::NotFoundException &)
	  {
	  }
	}
    }

    const char *ManagedElement_impl::get_element_name ()
    {
      return m_element_name.in ();
    }

    const char *ManagedElement_impl::get_element_path ()
    {
      return m_element_path.in ();
    }




    void ManagedElement_impl::define_property (const char *property_name,
					       const CORBA::
					       Any & property_value)
      throw (CosPropertyService::InvalidPropertyName,
	     CosPropertyService::ConflictingProperty,
	     CosPropertyService::UnsupportedTypeCode,
	     CosPropertyService::UnsupportedProperty,
	     CosPropertyService::ReadOnlyProperty, CORBA::SystemException)
    {
      m_property_set_var->define_property (property_name, property_value);
      set_element_in_datastore ();
    }



    void ManagedElement_impl::define_properties (const CosPropertyService::
						 Properties & nproperties)
      throw (CosPropertyService::MultipleExceptions, CORBA::SystemException)
    {
      m_property_set_var->define_properties (nproperties);
      set_element_in_datastore ();
    }



    CORBA::ULong ManagedElement_impl::
      get_number_of_properties ()throw (CORBA::SystemException)
    {
      return m_property_set_var->get_number_of_properties ();

    }

    void ManagedElement_impl::get_all_property_names (CORBA::ULong how_many,
						      CosPropertyService::
						      PropertyNames_out
						      property_names,
						      CosPropertyService::
						      PropertyNamesIterator_out
						      rest) throw (CORBA::
								   SystemException)
    {
      m_property_set_var->get_all_property_names (how_many, property_names,
						  rest);
    }

    CORBA::Any *
      ManagedElement_impl::
      get_property_value (const char *property_name)
      throw (CosPropertyService::PropertyNotFound,
	     CosPropertyService::InvalidPropertyName, CORBA::SystemException)
    {
      return m_property_set_var->get_property_value (property_name);

    }

    CORBA::Boolean ManagedElement_impl::
      get_properties (const CosPropertyService::
		      PropertyNames & property_names,
		      CosPropertyService::
		      Properties_out nproperties) throw (CORBA::
							 SystemException)
    {
      return m_property_set_var->get_properties (property_names, nproperties);
    }

    void ManagedElement_impl::get_all_properties (CORBA::ULong how_many,
						  CosPropertyService::
						  Properties_out nproperties,
						  CosPropertyService::
						  PropertiesIterator_out rest)
      throw (CORBA::SystemException)
    {
      m_property_set_var->get_all_properties (how_many, nproperties, rest);


    }



    void ManagedElement_impl::delete_property (const char *property_name)
      throw (CosPropertyService::PropertyNotFound,
	     CosPropertyService::InvalidPropertyName,
	     CosPropertyService::FixedProperty, CORBA::SystemException)
    {
      m_property_set_var->delete_property (property_name);

    }


    void ManagedElement_impl::delete_properties (const CosPropertyService::
						 PropertyNames &
						 property_names)
      throw (CosPropertyService::MultipleExceptions, CORBA::SystemException)
    {
      m_property_set_var->delete_properties (property_names);
      set_element_in_datastore ();
    }


    CORBA::Boolean ManagedElement_impl::delete_all_properties ()throw (CORBA::
								       SystemException)
    {
      CORBA::Boolean result = m_property_set_var->delete_all_properties ();
      set_element_in_datastore ();
      return result;
    }



    CORBA::Boolean ManagedElement_impl::
      is_property_defined (const char *property_name)
      throw (CosPropertyService::InvalidPropertyName, CORBA::SystemException)
    {
      return m_property_set_var->is_property_defined (property_name);
    }




    void ManagedElement_impl::get_allowed_property_types (CosPropertyService::
							  PropertyTypes_out
							  property_types)
      throw (CORBA::SystemException)
    {
      return m_property_set_var->get_allowed_property_types (property_types);
    }



    void ManagedElement_impl::get_allowed_properties (CosPropertyService::
						      PropertyDefs_out
						      property_defs)
      throw (CORBA::SystemException)
    {
      return m_property_set_var->get_allowed_properties (property_defs);
    }



    void ManagedElement_impl::
      define_property_with_mode (const char *property_name,
				 const CORBA::Any & property_value,
				 CosPropertyService::
				 PropertyModeType property_mode)
      throw (CosPropertyService::InvalidPropertyName,
	     CosPropertyService::ConflictingProperty,
	     CosPropertyService::UnsupportedTypeCode,
	     CosPropertyService::UnsupportedProperty,
	     CosPropertyService::UnsupportedMode,
	     CosPropertyService::ReadOnlyProperty, CORBA::SystemException)
    {
      m_property_set_var->define_property_with_mode (property_name,
						     property_value,
						     property_mode);
      set_element_in_datastore ();
    }



    void ManagedElement_impl::
      define_properties_with_modes (const CosPropertyService::
				    PropertyDefs & property_defs)
      throw (CosPropertyService::MultipleExceptions, CORBA::SystemException)
    {
      m_property_set_var->define_properties_with_modes (property_defs);
      set_element_in_datastore ();
    }


    CosPropertyService::PropertyModeType ManagedElement_impl::
      get_property_mode (const char *property_name)
      throw (CosPropertyService::PropertyNotFound,
	     CosPropertyService::InvalidPropertyName, CORBA::SystemException)
    {
      return m_property_set_var->get_property_mode (property_name);
    }



    CORBA::Boolean ManagedElement_impl::
      get_property_modes (const CosPropertyService::
			  PropertyNames & property_names,
			  CosPropertyService::
			  PropertyModes_out property_modes) throw (CORBA::
								   SystemException)
    {
      return m_property_set_var->get_property_modes (property_names,
						     property_modes);
    }


    void ManagedElement_impl::set_property_mode (const char *property_name,
						 CosPropertyService::
						 PropertyModeType
						 property_mode)
      throw (CosPropertyService::InvalidPropertyName,
	     CosPropertyService::PropertyNotFound,
	     CosPropertyService::UnsupportedMode, CORBA::SystemException)
    {
      m_property_set_var->set_property_mode (property_name, property_mode);
      set_element_in_datastore ();
    }



    void ManagedElement_impl::set_property_modes (const CosPropertyService::
						  PropertyModes &
						  property_modes)
      throw (CosPropertyService::MultipleExceptions, CORBA::SystemException)
    {
      m_property_set_var->set_property_modes (property_modes);
      set_element_in_datastore ();
    }



    CdmwPlatformMngt::ManagedElementRecord *
      ManagedElement_impl::
      get_managed_element_record_from_datastore ()throw (Common::
							 NotFoundException)
    {
      CdmwPlatformMngt::ManagedElementRecord * managed_element_record = NULL;

      try
      {
	managed_element_record =
	  m_managed_element_ds->select (m_element_path.in ());
      }
      catch (Common::NotFoundException &)
      {
	throw;
      }
      catch (...)
      {
	CDMW_ASSERT (false);
      }

      return managed_element_record;
    }



    void ManagedElement_impl::synchro_with_datastore ()
    {
      // synchronise with system record
      try
      {
	CdmwPlatformMngt::ManagedElementRecord_var managed_element_record =
	  get_managed_element_record_from_datastore ();
      }
      catch (const Common::NotFoundException &)
      {
	// Element not found, This append when the SystemManagement is the first primary
      }
    }


    CdmwPlatformMngt::SupervisionObserver_ptr
      ManagedElement_impl::
      register_all_properties_observer (const char *observer_name,
					CdmwPlatformMngt::
					SupervisionObserver_ptr observer,
					CORBA::Boolean deeply) throw (CORBA::
								      SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);


      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }


    CdmwPlatformMngt::SupervisionObserver_ptr
      ManagedElement_impl::
      unregister_all_properties_observer (const char *observer_name)
      throw (CdmwPlatformMngt::ObserverNotFound, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);


      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }



    void ManagedElement_impl::
      register_property_observer (const char *observer_name,
				  CdmwPlatformMngt::
				  SupervisionObserver_ptr observer,
				  const CosPropertyService::
				  PropertyNames & property_names)
      throw (CdmwPlatformMngt::PropertyInListNotFound, CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);


      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }



    void ManagedElement_impl::
      unregister_property_observer (const char *observer_name,
				    const CosPropertyService::
				    PropertyNames & property_names)
      throw (CdmwPlatformMngt::PropertyInListNotFound,
	     CdmwPlatformMngt::PropertyObserverNotFound,
	     CORBA::SystemException)
    {
      // TODO
      throw CORBA::NO_IMPLEMENT (OrbSupport::NO_IMPLEMENT,
				 CORBA::COMPLETED_NO);


      // TODO: Notify the configuration change

      // TODO: Trace to the log manager
    }

    void ManagedElement_impl::recover_default_properties ()
    {
      delete_all_properties ();
      //CosPropertyService::PropertyDefs_var m_user_defined_attributes_var = new CosPropertyService::PropertyDefs(*m_user_defined_attributes);
      define_properties_with_modes (*m_user_defined_attributes);
      set_element_in_datastore ();
    }


    void ManagedElement_impl::store_default_properties ()
    {
      // Properties to get
      CosPropertyService::Properties_var props;
      // get the number of properties
      CORBA::ULong propCount = get_number_of_properties ();

      CosPropertyService::PropertyNames_var prop_names;
      CosPropertyService::PropertyNamesIterator_var prop_names_It;
      //get all property names 
      get_all_property_names (propCount, prop_names.out (),
			      prop_names_It.out ());
      get_properties (prop_names, props.out ());
      //prepare the PropertyDef variable 
      CosPropertyService::PropertyDefs_var propdefs =
	new CosPropertyService::PropertyDefs;
      // set the right length
      propdefs->length (propCount);
      // prepare the Property Modes needed to prepare the the Property
      CosPropertyService::PropertyModes_var prop_modes;
      get_property_modes (prop_names.in (), prop_modes.out ());

      CosPropertyService::PropertyMode property_mode_out;
      for (CORBA::ULong i = 0; i < propCount; i++)
	{
	  propdefs[i].property_name = prop_names[i];
	  propdefs[i].property_mode = property_mode_out.property_mode;
	  propdefs[i].property_value =
	    *get_property_value (props[i].property_name);
	}

      // store properties default values 
      m_user_defined_attributes =
	new CosPropertyService::PropertyDefs (propdefs.in ());
      m_user_defined_attributes->length (propCount);

      set_element_in_datastore ();
    }

  }				// End namespace PlatformMngt
}				// End namespace Cdmw
