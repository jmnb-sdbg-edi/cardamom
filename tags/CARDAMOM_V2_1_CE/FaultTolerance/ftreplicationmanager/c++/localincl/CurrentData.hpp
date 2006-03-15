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

#ifndef _CDMW_FT_REPLICATION_MANAGER_CURRENT_DATA_HPP_
#define _CDMW_FT_REPLICATION_MANAGER_CURRENT_DATA_HPP_

#include <map>

#include "CdmwFTObjectGroupData.stub.hpp"
#include "CdmwFTLocationInfo.stub.hpp"
#include "ftreplicationmanager/ThreadSpecificData.hpp"
#include "ftreplicationmanager/ReplicationManagerDelegate.hpp"
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>

namespace Cdmw {
namespace FT {
namespace ReplicationManager {

    /*
     * WARNING: reader is strongly advised to *CAREFULLY* read *ALL* the
     *          following comments, especially the note concerning the
     *          ownership's rules at the end of this comment.
     * 
     * Definition of a thread specific data intended to contain pointers
     * and 'modify flags' on ObjectGroupData and LocationInfo instances
     * extracted from a DataStore.
     *
     * This class aims at two goals: 
     * - make internal ObjectGroupData and LocationInfo instances accessible
     *   by any code called from a ReplicationManager method,
     * - enable called code to not modify DataStores but instead to work on
     *   the current internal copies and possibly to modify them.
     *
     * How is it used?:
     *
     * - The ReplicationManager methods that needs to directly or indirectly
     *   call ObjectGroup and/or Location methods create an empty CurrentData 
     *   on the current thread stack.
     *
     * - The CurrentData takes care of extracting item from DataStores and
     *   providing the user with pointers on the internal extracted items.
     *
     * - The current thread CurrentData is made available to callers including
     *   the Rep.Mgr by the mean of some ReplicationManagerDelegate methods
     *   which immediately delegates to the current thread CurrentData:
     *
     *   - get_current_ogd(::FT::ObjectGroupId ogid): search the ObjectGroupData 
     *     with id 'ogid' in the internal map. If the item is not found, a fresh
     *     copy is extracted from the DataStore and inserted in the map. In both
     *     cases, the caller is returned a pointer to the *internal* item.
     *
     *   - get_current_locinfo(::std::string name): search the LocationInfo with
     *     name in the internal map. If the item is not found, a fresh copy is
     *     extracted from the DataStore and inserted in the map. In both cases,
     *     the caller is returned a pointer to the *internal* item.
     *
     * - when a CurrentData user has modified an internal item he *MUST* set the
     *   'modified' flag associated to the item by calling modified_ogd(ogid) or
     *   modified_loc(name).
     *
     * _ Finally, modified items are saved in Rep.Mgr. DataStores by calling the
     *   save_modified_items() method. It is typically called just before returning
     *   from the method where the CurrenData was created (i.e. at the end of a 
     *   Rep.Mgr. method).
     *
     * - In case of exception:
     *   - the save_modified_items() method is not called leaving the DataStores 
     *     in the state they had before,
     *   - the CurrentData destructor is automatically called which leads to the
     *     destruction of internal items, hence avoiding memory leaks.
     *
     * #### IMPORTANT NOTE / IMPORTANT NOTE / IMPORTANT NOTE / IMPORTANT NOTE ####
     * ####
     *   The CurrentData *ALWAYS* keeps ownership of item it contains. When the
     *   caller is returned a pointer to the internal item, he *MUST NOT* save
     *   this pointer in a _var local variable but instead in a plain C pointer.
     * ####
     * #### IMPORTANT NOTE / IMPORTANT NOTE / IMPORTANT NOTE / IMPORTANT NOTE ####
     *
     */
    class CurrentData {

      public:

        CurrentData ( ReplicationManagerDelegate* rep_mgr
                    , ThreadSpecificData*         thread_data)
                : m_rep_mgr(rep_mgr)
                , m_thread_data(thread_data)
        {
            m_thread_data->set_value(this);

            m_dsBase = Cdmw::FT::TXDataStoreBase::Get_dataStore(OGD_DATA_STORE);
            CDMW_ASSERT(m_dsBase != 0);

        }

        ~CurrentData () {
            m_thread_data->set_value(NULL);
        }

        // Returns NULL if the item was not found in the DataStore
        ::CdmwFT::ObjectGroupData* get_current_ogd (::FT::ObjectGroupId ogid);
        ::CdmwFT::LocationInfo*    get_current_loc (std::string name);
        
        // Sets the 'modified' flag for the item: returns false if
        // the item was not found in the DataStore.
        bool modified_ogd (::FT::ObjectGroupId ogid);
        bool modified_loc (std::string name);
        
        // Saves the modified item in the Rep.Mgr DataStores
        void save_modified_items ()
            throw (::CORBA::INTERNAL);

      private:
      
        struct FlaggedObjectGroupData {
            FlaggedObjectGroupData() : modified(false) { }
            bool                          modified;
            ::CdmwFT::ObjectGroupData_var ogd;
        };

        struct FlaggedLocationInfo {
            FlaggedLocationInfo() : modified(false) { }
            bool                          modified;
            ::CdmwFT::LocationInfo_var    loc;
        };

        typedef std::map < ::FT::ObjectGroupId, FlaggedObjectGroupData > RCFlgdGrpDataMap;
        typedef std::map < std::string, FlaggedLocationInfo >            RCFlgdLocInfoMap;
            
      
        ReplicationManagerDelegate* m_rep_mgr;
        ThreadSpecificData*         m_thread_data;
        RCFlgdGrpDataMap            m_ogd_map;
        RCFlgdLocInfoMap            m_loc_map;

        Cdmw::FT::TXDataStoreBase* m_dsBase;
        
    };

}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_REPLICATION_MANAGER_CURRENT_DATA_HPP_

