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

#ifndef INCL_COMMONSVCS_DATASTORE_STDMAPDATASTOREFACTORY_HPP
#define INCL_COMMONSVCS_DATASTORE_STDMAPDATASTOREFACTORY_HPP

#include "Foundation/commonsvcs/datastore/DataStoreRegistry.hpp"
#include "Foundation/commonsvcs/datastore/StdMapDataStore.hpp"

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {

/**
*This factory class creates a DataStore backed by a standard map.
*
*This is a template class where the Oid and Data should be IDL types.
*
*Features:
*<p> Thread safe. Exception safe.
*/
template <class Oid, class Data, class OidCompare=std::less<typename Cdmw::OrbSupport::type_traits<Oid>::_var_type> > 
class StdMapDataStoreFactory 
{

public:   

    typedef StdMapDataStore<Oid, Data, OidCompare> StdMapDataStore;

    /**
     * Static factory method to create a StdMapDataStore and register it with
     * the DataStoreRegistry.
     *
     *@param ds_id The identifier for the data store that is to be created.
     *@throw AlreadyExistException If a data store already exists for that data store id.
     */
    static void Create_data_store(DsIdentifier ds_id) throw (AlreadyExistException);

private:
     
    /**
     * Constructor is private - no instantiation.
     */ 
    StdMapDataStoreFactory() {};
    
    /**
     * Destructor is private - no instantiation.
     */
    virtual ~StdMapDataStoreFactory() {};

}; // End class StdMapDataStoreFactory

//
// IMPLEMENTATION PART OF THE TEMPLATE
//

template <class Oid, class Data, class OidCompare>
void
StdMapDataStoreFactory<Oid, Data, OidCompare>::Create_data_store(DsIdentifier ds_id) throw (AlreadyExistException)
{
    StdMapDataStore* result = new StdMapDataStore(ds_id);

    try
    {
        DataStoreRegistry::Register_data_store(result);
    }
    catch (const AlreadyExistException&)
    {
        delete result;
        throw;
    }
}

}  // End namespace DataStore
}  // End namespace CommonSvcs
}  // End namespace Cdmw

#endif /* INCL_COMMONSVCS_DATASTORE_STDMAPDATASTOREFACTORY_HPP */

