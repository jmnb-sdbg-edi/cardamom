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

#include "Foundation/osthreads/ReaderLockGuard.hpp"
#include "Foundation/osthreads/WriterLockGuard.hpp"
#include "Foundation/ossupport/OS.hpp"
#include <Foundation/orbsupport/Sequence.hpp>
#include <Foundation/common/Assert.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include "ftstatemanager/DataStoreBase.hpp"

/**
*Purpose:
* DataStoreBase .
*<p>
*
*Features:
*<p> Thread safe. Exception safe.
*/
namespace Cdmw {
namespace FT {

    std::string DataStoreBase::M_location_name = OsSupport::OS::get_hostname();
    DataStoreBase::DsidMap DataStoreBase::M_dsidMap;
    DataStoreBase::TimeoutHandlerList DataStoreBase::M_timeoutHandlerList;
    DataStoreBase::DataStoreMap DataStoreBase::M_ft_dsMap;

    DataStoreBase* DataStoreBase::Get_dataStore(const DsIdentifier& dsid)
    {

        DataStoreBase* result = 0;

        DataStoreBase::DataStoreMap::iterator iter = M_ft_dsMap.find(dsid);
            
        if (iter  != M_ft_dsMap.end())
        {
            result = (*iter).second;
        }
        return result;
    }

    bool DataStoreBase::Register_Timeout_Handler(
        DataStoreTimeoutHandler* timeout_handler) 
        throw()
    {
        TimeoutHandlerList::iterator pos =
            std::find(M_timeoutHandlerList.begin(), M_timeoutHandlerList.end(),
                      timeout_handler);

        if (pos == M_timeoutHandlerList.end())
        {
            M_timeoutHandlerList.push_back(timeout_handler);
            return true;
        }
        else
        {
            return false;
        }
    }

    bool DataStoreBase::Unregister_Timeout_Handler(
        DataStoreTimeoutHandler* timeout_handler)
        throw()
    {
        TimeoutHandlerList::iterator pos =
            std::find(M_timeoutHandlerList.begin(), M_timeoutHandlerList.end(),
                      timeout_handler);

        if (pos != M_timeoutHandlerList.end())
        {
            M_timeoutHandlerList.erase(pos);
            return true;
        }
        else
        {
            return false;
        }
    }

    void DataStoreBase::Timeout(const std::string& location)
    {
        TimeoutHandlerList::iterator pos;

        for (pos = M_timeoutHandlerList.begin();
             pos != M_timeoutHandlerList.end();
             ++pos)
        {
            (*pos)->on_freeze_timeout(location);
        }
    }
    
    void DataStoreBase::Activate_All()
    {
        for (DataStoreBase::DataStoreMap::iterator iter = M_ft_dsMap.begin(); iter != M_ft_dsMap.end(); iter++)
        {
            CDMW_ASSERT(iter->second != 0);
            iter->second->activate();
        }
    }

    void DataStoreBase::Deactivate_All()
    {
        for (DataStoreBase::DataStoreMap::iterator iter = M_ft_dsMap.begin(); iter != M_ft_dsMap.end(); iter++)
        {
            CDMW_ASSERT(iter->second != 0);
            iter->second->deactivate();
        }
    }

    void DataStoreBase::On_Failure(const std::string& location) throw (NotFoundException)
    {

        for (DataStoreBase::DataStoreMap::iterator iter = M_ft_dsMap.begin(); iter != M_ft_dsMap.end(); iter++)
        {
            CDMW_ASSERT(iter->second != 0);
            iter->second->on_failure(location);
        }
        M_dsidMap.erase(location);        
    }

}  // End namespace FT
}  // End namespace Cdmw
