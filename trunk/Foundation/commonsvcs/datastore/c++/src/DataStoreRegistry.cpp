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

#include <Foundation/commonsvcs/datastore/DataStoreRegistry.hpp>

namespace Cdmw {
namespace CommonSvcs {
namespace DataStore {

Cdmw::OsSupport::Mutex DataStoreRegistry::M_mutex;

void
DataStoreRegistry::Register_data_store(DataStoreBase* data_store) throw (AlreadyExistException)
{
    CDMW_MUTEX_GUARD(M_mutex);
    DataStoreDictionary & dico = Get_dictionary();
    DsIdentifier ds_id = data_store->get_dsid();

    DataStoreDictionary::iterator info_pos = dico.find(ds_id);

    if (info_pos != dico.end())
    {
        throw AlreadyExistException("DataStoreRegistry: A DataStore with that Id is already registered.");
    }
    else
    {
        dico[ds_id] = data_store;
    }
}

DataStoreBase*
DataStoreRegistry::Unregister_data_store(const DsIdentifier& dsid) throw (NotExistException)
{
    CDMW_MUTEX_GUARD(M_mutex);
    DataStoreBase* result = 0;
    DataStoreDictionary & dico = Get_dictionary();
 
    DataStoreDictionary::iterator info_pos = dico.find(dsid);

    if (info_pos != dico.end())
    {
        result = info_pos->second;
        dico.erase(dsid);
    }
    else
    {
        throw NotExistException();
    }

    return result;
}

DataStoreBase*
DataStoreRegistry::Get_data_store(const DsIdentifier& dsid) throw (NotExistException)
{
    CDMW_MUTEX_GUARD(M_mutex);
    DataStoreBase* result = 0;
    DataStoreDictionary & dico = Get_dictionary();

    DataStoreDictionary::iterator info_pos = dico.find(dsid);


    if (info_pos != dico.end())
    {
        result = info_pos->second; // dico[dsid];
    }
    else
    {
        throw NotExistException();
    }

    return result;
}


}  // End namespace DataStore    
}  // End namespace CommonSvcs
}  // End namespace Cdmw
