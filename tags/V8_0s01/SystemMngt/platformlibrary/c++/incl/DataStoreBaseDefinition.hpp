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

#ifndef INCL_DATA_STORE_BASE_DEFINITION_HPP
#define INCL_DATA_STORE_BASE_DEFINITION_HPP

#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/DataStoreInterface.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>

#include "SystemMngt/idllib/CdmwPlatformMngtDataStore.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtEvent.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtService.stub.hpp"

DEFINE_FIXED_LENGTH_DATA_TRAITS(CdmwPlatformMngtBase::EventKey);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngt::EventReportRecord);
DEFINE_VALUE_TYPE_TRAITS(CdmwPlatformMngt::Event);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwPlatformMngtService::ServiceDefs);



namespace Cdmw {

namespace PlatformMngt {


        
// define the datastore responsible to store EventHandler
// key is CdmwPlatformMngtBase::EventKey
template<>
bool std::less<CdmwPlatformMngtBase::EventKey_var>::operator() 
    (const CdmwPlatformMngtBase::EventKey_var& x, 
     const CdmwPlatformMngtBase::EventKey_var& y) const
{
    bool result = false;
    
    if (x->primary_key < y->primary_key)
    {
        result = true;
    }    
    else if (x->primary_key == y->primary_key)
    {
        if (x->seconds < y->seconds)
        {
            result = true;
        }
        else if (x->seconds == y->seconds)
        {
            if (x->microseconds < y->microseconds)
            {
                result = true;
            }
            else if (x->microseconds == y->microseconds)
            {
                if (x->counter_inx < y->counter_inx)
                {
                    result = true;
                }
            }
        }
    }

    return result;
};

typedef Cdmw::CommonSvcs::DataStore::StorageHome<CdmwPlatformMngtBase::EventKey,
                              CdmwPlatformMngt::EventReportRecord> 
        EventReportStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<CdmwPlatformMngtBase::EventKey,
                            CdmwPlatformMngt::EventReportRecord>
        EventReportStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<CdmwPlatformMngtBase::EventKey,
                            CdmwPlatformMngt::EventReportRecord>
        EventReportDataStore;

        
// define the datastore responsible to store EventNotifier
// key is CdmwPlatformMngtBase::EventKey
typedef Cdmw::CommonSvcs::DataStore::StorageHome<CdmwPlatformMngtBase::EventKey,
                              CdmwPlatformMngt::Event> 
        EventNotifierStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<CdmwPlatformMngtBase::EventKey,
                            CdmwPlatformMngt::Event> 
        EventNotifierStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<CdmwPlatformMngtBase::EventKey,
                            CdmwPlatformMngt::Event>
        EventNotifierDataStore;

        
// define the datastore responsible to store ServiceDefs
// key is CdmwPlatformMngt::ElementPath
typedef Cdmw::CommonSvcs::DataStore::StorageHome<std::string,
                              CdmwPlatformMngtService::ServiceDefs> 
        ServiceDefsStorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject<std::string,
                            CdmwPlatformMngtService::ServiceDefs> 
        ServiceDefsStorageObject;
typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<std::string,
                            CdmwPlatformMngtService::ServiceDefs> 
        ServiceDefsDataStore;
        

} // end of namespace PlatformMngt

} // end of namespace Cdmw

        
#endif //INCL_DATA_STORE_BASE_DEFINITION_HPP
