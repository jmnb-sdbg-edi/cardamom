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

#ifndef INCL_DATA_STORE_DEFINITION_HPP
#define INCL_DATA_STORE_DEFINITION_HPP

#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/DataStoreInterface.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>

#include "SystemMngt/idllib/CdmwPlatformMngtDataStore.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"


DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngt::ManagedElementRecord);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngt::EntityRecord);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngt::HostRecord);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngt::SystemRecord);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngt::ApplicationRecord);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngt::ProcessRecord);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngt::SystemObserverRegistrationRecord);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngt::RunSet);



namespace Cdmw {

namespace PlatformMngt {


// define the datastore responsible to store ManagedElement
// key is CdmwPlatformMngt::ElementPath
typedef Cdmw::CommonSvcs::DataStore::StorageHome<std::string,
                              CdmwPlatformMngt::ManagedElementRecord> 
        ManagedElementStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<std::string,
                            CdmwPlatformMngt::ManagedElementRecord> 
        ManagedElementStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<std::string,
                            CdmwPlatformMngt::ManagedElementRecord> 
        ManagedElementDataStore;

        
// define the datastore responsible to store Entity
// key is CdmwPlatformMngt::ElementPath
typedef Cdmw::CommonSvcs::DataStore::StorageHome<std::string,
                              CdmwPlatformMngt::EntityRecord> 
        EntityStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<std::string,
                            CdmwPlatformMngt::EntityRecord> 
        EntityStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<std::string,
                            CdmwPlatformMngt::EntityRecord> 
        EntityDataStore;

        
// define the datastore responsible to store Host
// key is CdmwPlatformMngt::ElementPath
typedef Cdmw::CommonSvcs::DataStore::StorageHome<std::string,
                              CdmwPlatformMngt::HostRecord> 
        HostStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<std::string,
                            CdmwPlatformMngt::HostRecord> 
        HostStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<std::string,
                            CdmwPlatformMngt::HostRecord> 
        HostDataStore;

        
// define the datastore responsible to store System
// key is CdmwPlatformMngt::ElementPath
typedef Cdmw::CommonSvcs::DataStore::StorageHome<std::string,
                              CdmwPlatformMngt::SystemRecord> 
        SystemStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<std::string,
                            CdmwPlatformMngt::SystemRecord> 
        SystemStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<std::string,
                            CdmwPlatformMngt::SystemRecord> 
        SystemDataStore;

// define the datastore responsible to store Application
// key is CdmwPlatformMngt::ElementPath
typedef Cdmw::CommonSvcs::DataStore::StorageHome<std::string,
                              CdmwPlatformMngt::ApplicationRecord> 
        ApplicationStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<std::string,
                            CdmwPlatformMngt::ApplicationRecord> 
        ApplicationStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<std::string,
                            CdmwPlatformMngt::ApplicationRecord> 
        ApplicationDataStore;

// define the datastore responsible to store Process
// key is CdmwPlatformMngt::ElementPath
typedef Cdmw::CommonSvcs::DataStore::StorageHome<std::string,
                              CdmwPlatformMngt::ProcessRecord> 
        ProcessStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<std::string,
                            CdmwPlatformMngt::ProcessRecord> 
        ProcessStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<std::string,
                            CdmwPlatformMngt::ProcessRecord> 
        ProcessDataStore;

        
// define the datastore responsible to store SystemObserverRegistration
// key is observer name
typedef Cdmw::CommonSvcs::DataStore::StorageHome<std::string,
                              CdmwPlatformMngt::SystemObserverRegistrationRecord> 
        SystemObserverRegistrationStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<std::string,
                            CdmwPlatformMngt::SystemObserverRegistrationRecord> 
        SystemObserverRegistrationStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<std::string,
                            CdmwPlatformMngt::SystemObserverRegistrationRecord> 
        SystemObserverRegistrationDataStore;

        

        
// define the datastore responsible to store RunSet
// key is CdmwPlatformMngt::ElementPath
typedef Cdmw::CommonSvcs::DataStore::StorageHome<std::string,
                              CdmwPlatformMngt::RunSet> 
        RunSetStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<std::string,
                            CdmwPlatformMngt::RunSet> 
        RunSetStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<std::string,
                            CdmwPlatformMngt::RunSet> 
        RunSetDataStore;

        
} // end of namespace PlatformMngt

} // end of namespace Cdmw

        
#endif //INCL_DATA_STORE_DEFINITION_HPP
