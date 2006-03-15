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

#ifndef INCL_COMMONSVCS_DATASTORE_DATASTOREREGISTRY_HPP
#define INCL_COMMONSVCS_DATASTORE_DATASTOREREGISTRY_HPP

#include <string>
#include <set>
#include <map>
#include <vector>

#include "Foundation/commonsvcs/datastore/DataStoreBase.hpp"
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/osthreads/Mutex.hpp>

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {

/**
* This class offers some static operations to manage instanciated DataStores.
*
*Features:
*<p> Thread safe. Exception safe.
*/
class DataStoreRegistry
{
public:
    
    typedef std::map<DsIdentifier, DataStoreBase*> DataStoreDictionary;

    /**
    * Register a DataStore instance against its DataStore Id.
    *
    *@param data_store The DataStore.
    *@throw AlreadyExistException If a DataStore is already registered against that Id.
    */
    static void Register_data_store(DataStoreBase* data_store) throw (AlreadyExistException);

    /**
    * Unregister the DataStore corresponding to this Id.
    *
    *@param dsid The identifierof the DataStore to be de-registered.
    *@return Pointer to the de-registered DataStore.
    *@throw NotExistException If no DataStore was registered against that Id.
    */
    static DataStoreBase* Unregister_data_store(const DsIdentifier& dsid) throw (NotExistException);

    /**
    * Return the DataStore with the specified identifier.
    *
    *@param dsid The identifier of the required DataStore.
    *@return The requested DataStore.
    *@throw NotExistException If no DataStore with that Id is registered.
    */
    static DataStoreBase* Get_data_store(const DsIdentifier& dsid) throw (NotExistException); 

    static DataStoreDictionary& Get_dictionary()
    {
      // will return the same single instance
      static DataStoreDictionary d;
      return d;
    }

    static Cdmw::OsSupport::Mutex M_mutex;

private:

    /**
    * Constructor is private - no instantiation.
    */
    DataStoreRegistry() {};

    /**
    * Destructor is private - no instantiation.
    */
    virtual ~DataStoreRegistry() {};

    /**
    * Assignement operator is not allowed.
    */
    DataStoreRegistry& operator=(const DataStoreRegistry& rhs);

}; // End class DataStoreRegistry

}  // End namespace DataStore
}  // End namespace CommonSvcs
}  // End namespace Cdmw

#endif /* INCL_COMMONSVCS_DATASTORE_DATASTOREREGISTRY_HPP */

