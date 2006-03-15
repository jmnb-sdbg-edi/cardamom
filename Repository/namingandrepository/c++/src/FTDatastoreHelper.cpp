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

#include <memory> // for std::auto_ptr
#include <string>
#include <vector>
#include <map>
#include <functional>

#define CDMW_TIME_SERVICE_ENABLED

#include "Foundation/commonsvcs/datastore/DataStoreBase.hpp" 
#include "namingandrepository/FTDatastoreHelper.hpp"

namespace Cdmw {
namespace NamingAndRepository {

    
    // see code of cdmw/FaultTolerance/ftstatemanager/c++/incl/StorageHome.hpp
    FTDataStore*
    FTDataStoreHelper::getStorage(Cdmw::CommonSvcs::DataStore::DsIdentifier dsId) 
    {
	CDMW_ASSERT (dsId==NAME_DOMAIN_DATASTORE_ID ||
		     dsId==NAMING_CONTEXT_DATASTORE_ID);

	Cdmw::CommonSvcs::DataStore::DataStoreBase * dsBase = 
	 Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(dsId);
    
	FTDataStore *dataStore = dynamic_cast<FTDataStore*>(dsBase);
	CDMW_ASSERT (dataStore!=NULL);

	return dataStore;
    }

}
}

