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

#ifndef INCL_COMMONSVCS_DATASTORE_STORAGEHOME_HPP
#define INCL_COMMONSVCS_DATASTORE_STORAGEHOME_HPP

#include <string>

#include "Foundation/common/Assert.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"
#include "Foundation/commonsvcs/datastore/DataStoreInterface.hpp"
#include "Foundation/commonsvcs/datastore/DataStoreRegistry.hpp"
#include "Foundation/orbsupport/TypeTraits.hpp"
#include "Foundation/ossupport/OS.hpp"

#include <functional>
#include <list>

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {

/**
*StorageHome template.
*A StorageHome manage StorageObjects with the same Data type.
*
*Features:
*<p> Thread safe. Exception safe.
*/
template <class Oid, class Data, class OidCompare = std::less<typename Cdmw::OrbSupport::type_traits<Oid>::_var_type> > 
class StorageHome
{
  public:
    
    typedef StorageObject<Oid, Data, OidCompare> StorageObjectType;
    typedef DataStoreInterface<Oid, Data, OidCompare> DataStoreType;

    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_var_type      _Oid_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_in_type      _Oid_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<Oid>::_retn_type    _Oid_retn_type;

    typedef typename Cdmw::OrbSupport::type_traits<Data>::_var_type     _Data_var_type;
    typedef typename Cdmw::OrbSupport::type_traits<Data>::_in_type      _Data_in_type;
    typedef typename Cdmw::OrbSupport::type_traits<Data>::_retn_type    _Data_retn_type;

    /**
    * Constructor that will get the DataStore identified
    * by the given DataStore Identifier, and initialize member with it
    * 
    *@param dsId the given string
    */
    StorageHome(DsIdentifier dsId);

    /**
    * Destructor.
    */
    virtual ~StorageHome();

    /**
    * create a storage object (which is a shell to access DataStore)
    * 
    *@param oid is the Object Id
    *@param d is the data corresponding to the oid
    */
    StorageObjectType create(_Oid_in_type oid, _Data_in_type d) throw (AlreadyExistException,StorageErrorException);

    bool contains(_Oid_in_type oid) throw ();
    
    /**
    * return the data with Object Id key
    *
    *@param oid is the Object Id key
    *@exception NotFound if no object found with this oid
    */
    _Data_retn_type find_data_by_oid(_Oid_in_type oid) throw (NotFoundException);
    
    /**
    * return the data with Object Id key
    *
    *@param oid is the Object Id key
    *@exception NotFound if no object found with this oid
    */
    StorageObjectType find_by_oid(_Oid_in_type oid) throw (NotFoundException);
    
    /**
    * output in iterator each element of DataStore
    * wich satisfy the predicate
    * NOTE: The implementation does not autorise the given predicate to modify the data
    *       Only Predicate with const parameter will compile
    *       (typical example: const std::pair<Oid, Data>& is OK)
    *
    *@param Predicate p the predicate to verify
    *@param OutputIterator on result 
    */
    template <typename Predicate, typename OutputIterator>
    void find_if(Predicate p, OutputIterator result);

    /**
    * execute function for each element of DataStore
    * NOTE: The implementation does not autorise the given function to modify the data
    *       Only Function with const parameter will compile
    *       (typical example: const std::pair<Oid, Data>& is OK)
    *
    *@param Function f to execute
    */
    template <typename Function>
    void for_each(Function f)
    {
        m_dataStore->for_each(f);
    }
    
    /**
    * Remove from the DataStore the entry with the given oid key
    *
    *@param oid is the Object Id key
    */
    void remove(_Oid_in_type oid) throw (NotFoundException,StorageErrorException)
    {
        m_dataStore->remove(oid);
    }
    
    /**
    * Give the size of the DataStore
    *
    *@return int the size
    */
    int size()
    {
      return m_dataStore->size();
    }

  protected:

    /**
    * Purpose:
    * <p> Default constructor.
    */
    StorageHome()
    {
    }


  private:

    /**
    * Assignement operator is not allowed.
    */
    StorageHome& operator=(const StorageHome& rhs);

    // The DataStore associated to the storage home
    DataStoreType * m_dataStore;

}; // End class StorageHome

//
// IMPLEMENTATION PART OF THE TEMPLATE
//

template <class Oid, class Data, class OidCompare>
StorageHome<Oid, Data, OidCompare>::StorageHome(DsIdentifier dsId)
{
    DataStoreBase* dsBase = DataStoreRegistry::Get_data_store(dsId);
    m_dataStore = dynamic_cast<DataStoreInterface<Oid, Data, OidCompare>*>(dsBase);
    // DataStore must have been created previously
    CDMW_ASSERT(m_dataStore != 0);
}

template <class Oid, class Data, class OidCompare>
StorageHome<Oid, Data, OidCompare>::~StorageHome()
{
    // m_dataStore comes from static get_dataStore
}

template <class Oid, class Data, class OidCompare>
typename StorageHome<Oid, Data, OidCompare>::StorageObjectType
StorageHome<Oid, Data, OidCompare>::create(_Oid_in_type oid, _Data_in_type d) throw (AlreadyExistException,StorageErrorException)
{
    m_dataStore->insert(oid, d);
    return StorageObjectType(oid, m_dataStore);
}

template <class Oid, class Data, class OidCompare>
bool
StorageHome<Oid, Data, OidCompare>::contains(_Oid_in_type oid) throw ()
{
    return m_dataStore->contains(oid);
}

template <class Oid, class Data, class OidCompare>
typename StorageHome<Oid, Data, OidCompare>::_Data_retn_type
StorageHome<Oid, Data, OidCompare>::find_data_by_oid(_Oid_in_type oid) throw (NotFoundException)
{
    return m_dataStore->select(oid);
}

template <class Oid, class Data, class OidCompare>
typename StorageHome<Oid, Data, OidCompare>::StorageObjectType
StorageHome<Oid, Data, OidCompare>::find_by_oid(_Oid_in_type oid) throw (NotFoundException)
{
    if (!m_dataStore->contains(oid))
    {
        throw NotFoundException("The Oid does not exist");
    }

    return StorageObjectType(oid, m_dataStore);
}

  
template <class Oid, class Data, class OidCompare>
template <typename Predicate, typename OutputIterator>
void
StorageHome<Oid, Data, OidCompare>::find_if(Predicate p, OutputIterator result)
{
  // call DataStore find_if then transform result (Oids) into StorageObject Seq
  std::list< _Oid_var_type > resultOids;
  typename std::list<_Oid_var_type>::iterator itResultOids = resultOids.begin();
      
  m_dataStore->find_if(p, std::back_inserter(resultOids));
  for ( itResultOids = resultOids.begin(); itResultOids != resultOids.end();
        itResultOids++) {
      *result = StorageObjectType(*itResultOids, m_dataStore);
      result++;
  }
}

}  // End namespace DataStore
}  // End namespace CommonSvcs
}  // End namespace Cdmw

#endif /* INCL_COMMONSVCS_DATASTORE_STORAGEHOME_HPP */
