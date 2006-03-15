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

#ifndef INCL_CDMW_COMMONSVCS_STORAGEOBJECT_HPP
#define INCL_CDMW_COMMONSVCS_STORAGEOBJECT_HPP

#include "Foundation/commonsvcs/datastore/DataStoreInterface.hpp"
#include "Foundation/orbsupport/TypeTraits.hpp"
#include <functional>

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {

template <class Oid, class Data, class OidCompare> class StorageHome;

/**
*StorageObject template.
*A StorageObject object stores a Data associated to an Oid.
*
*Features:
*<p> Thread safe. Exception safe.
*/
template <class Oid, class Data, class OidCompare = std::less<typename Cdmw::OrbSupport::type_traits<Oid>::_var_type> > 
class StorageObject
{
    friend class StorageHome<Oid, Data, OidCompare>;

  public:

    typedef DataStoreInterface<Oid, Data, OidCompare> DataStoreBaseType;

    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_var_type     _Oid_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_in_type      _Oid_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_retn_type    _Oid_retn_type;

    typedef typename Cdmw::OrbSupport::type_traits<Data>::_var_type    _Data_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<Data>::_in_type     _Data_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<Data>::_retn_type   _Data_retn_type;

    /**
    * Set Data in DataStore for the Oid of this StorageObject.
    *
    *@param Data is new Data for the Oid of this StorageObject.
    */
    void set(_Data_in_type d) throw (NotFoundException,StorageErrorException)
    {
        m_dataStore->update(m_oid, d);
    }

    /**
    * Get Data in DataStore for the Oid of this StorageObject.
    *
    *@return Data is the Data in DataStore
    */
    _Data_retn_type get() throw (NotFoundException)
    {
        return m_dataStore->select(m_oid);
    }

    /**
    * Conversion operator to data corresponding to the StorageObject
    */
    operator _Data_retn_type() // TODO: Check this!
    {
        return get();
    }

    /**
    * Destructor.
    */
    virtual ~StorageObject()
    {
    }

    /**
    * Get Oid of this StorageObject.
    *
    *@return Oid of this StorageObject.
    */
    inline _Oid_retn_type get_oid() const { 
       _Oid_var_type oid(m_oid);
       return Cdmw::OrbSupport::type_traits<Oid>::_retn(oid); 
    }

    /**
    * Assignment operator between two StorageObjects
    */
    StorageObject& operator=(const StorageObject& so)
    {
      // Does nothing in DataStore : just duplicate oid ref
      m_oid = so.get_oid();
      return *this;
    }

    /**
    * Assignment operator to allow aStorageObject = aData
    */
    StorageObject& operator=(_Data_in_type d)
    {
      set(d);
      return *this;
    }

    /**
    * Remove this StorageObject from the DataStore
    *
    */
    void remove() throw (NotFoundException,StorageErrorException)
    {
        m_dataStore->remove(m_oid);
    }
    

  protected:

    /**
    * Purpose:
    * <p> Constructor
    *
    *@param oid is the Object Id
    *@param d is the data corresponding to the oid
    */
    StorageObject(_Oid_in_type oid, DataStoreBaseType* dataStore)
      : m_oid(oid),
        m_dataStore(dataStore)
    {
    }

  private:

    /**
    * Purpose:
    * <p> Default constructor.
    */
    StorageObject()
    {
    }

    // Identifies Storage Object. Data written to DataStore  
    _Oid_var_type m_oid; 

    DataStoreBaseType* m_dataStore; 

}; // End class StorageObject

}  // End namespace DataStore
}  // End namespace FT
}  // End namespace Cdmw

#endif /* INCL_CDMW_COMMONSVCS_STORAGEOBJECT_HPP */

