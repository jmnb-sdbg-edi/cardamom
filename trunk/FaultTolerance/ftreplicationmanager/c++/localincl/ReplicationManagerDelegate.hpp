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


#ifndef _CDMW_FT_REPLICATION_MANAGER_DELEGATE_H_
#define _CDMW_FT_REPLICATION_MANAGER_DELEGATE_H_

#include "ftreplicationmanager/CdmwFTObjectGroupData.stub.hpp"
#include "CdmwFTLocationInfo.stub.hpp"

namespace Cdmw {
namespace FT {
namespace ReplicationManager {

    /**
     * ReplicationManagerDelegate: a pure abstract class that gives a limited
     * access to some ReplicationManager_impl features or data members.
     */
    class ReplicationManagerDelegate
    {

    public:
    
        /*
         * Returns a pointer to a copy of the ObjectGroupData instance
         * associated with the object group with id 'object_group_id'
         */
        virtual ::CdmwFT::ObjectGroupData*
        get_object_group_data (
                ::FT::ObjectGroupId  object_group_id)
            throw (::CORBA::INTERNAL) = 0;

        /*
         * Saves the ObjectGroupData in Rep.Mgr. data store
         */
        virtual void
        set_object_group_data (
                ::CdmwFT::ObjectGroupData*  ogd)
            throw (::CORBA::INTERNAL) = 0;

        /*
         * Returns a pointer to a fresh copy of the LocationInfo
         * associated with the location with name 'loc_name'
         */
        virtual ::CdmwFT::LocationInfo*
        get_location_info (const ::FT::Location&  loc_name)
            throw ( ::CORBA::INTERNAL
                  , ::CORBA::BAD_PARAM) = 0;
        
        /*
         * Returns a pointer to a fresh copy of the LocationInfo
         * associated with the location with name 'loc_name'
         */
        virtual ::CdmwFT::LocationInfo*
        get_location_info (const std::string&  loc_name)
            throw (::CORBA::INTERNAL) = 0;
        
        /*
         * Saves the LocationInfo in Rep.Mgr. data store
         */
        virtual void
        set_location_info (
                ::CdmwFT::LocationInfo* locinfo
              , const std::string&      locname)
            throw (::CORBA::INTERNAL) = 0;
        
        /*
         * Search the ObjectGroupData with id 'ogid' in the current data. If the
         * item is not found, a fresh copy is extracted from the DataStore and
         * inserted in the current data. If the item is found, returns a pointer
         * on the current item. The current data keeps ownership of the pointed
         * item.
         */
        virtual ::CdmwFT::ObjectGroupData*
        get_current_ogd (::FT::ObjectGroupId ogid) = 0;

        /*
         * Search the LocationInfo with name 'name' in the current data. If the
         * item is not found, a fresh copy is extracted from the DataStore and
         * inserted in the current data. If the item is found, returns a pointer
         * on the current item. The current data keeps ownership of the pointed
         * item.
         */
        virtual ::CdmwFT::LocationInfo*
        get_current_loc (const std::string& name) = 0;
        
        /*
         * Tells the Rep.Mgr. the the ObjectGroupData 'ogid' has been 
         * modified in the current data
         *
         * Returns false if the item was not found in the current data.
         */
        virtual bool
        modified_ogd (::FT::ObjectGroupId ogid) = 0;
        
        /*
         * Tells the Rep.Mgr. the the LocationInfo 'name' has been 
         * modified in the current data
         *
         * Returns false if the item was not found in the current data.
         */
        virtual bool
        modified_loc (const std::string& name) = 0;
        
        /*
         * Builds a ::FT::Location from a std::string
         */
        virtual ::FT::Location*
        string_to_ftloc(const std::string& loc_name)
            throw (::CORBA::INTERNAL) = 0;

        /*
         * Builds a std::string from a ::FT::Location 
         */
        virtual std::string
        ftloc_to_string(const ::FT::Location& loc_name)
            throw (::CORBA::INTERNAL) = 0;

        /*
         * Returns a reference on the stream used to display trace data
         */
        virtual std::ostream& get_trace_stream() = 0;
        
        /*
         * Builds an object group reference and returns it in 'group_data'
         */
        virtual void
        build_iogr ( const ::FT::Location&         primary_loc
                   , ::CdmwFT::ObjectGroupData&    group_data)
            throw( ::FT::PrimaryNotSet
                 , ::FT::ObjectNotCreated
                 , ::CORBA::SystemException) = 0;
        
        virtual ::FT::Location *
        get_group_primary_location (
                   const ::CdmwFT::ObjectGroupData & ogd) const
            throw() = 0;

        virtual bool
        erase_group_primary_location (
                   ::CdmwFT::ObjectGroupData &       ogd) const
            throw() = 0;

        virtual void
        notify_group_state_observers(::FT::ObjectGroupId         object_group_id,
                                     ::CdmwFT::ObjectGroupState  new_state,
                                     CORBA::Long                 nb_of_replicas,
                                     CORBA::Long                 minimum_nb_of_replicas)
            throw() = 0;

        virtual bool
        remove_locctxt_from_container(const std::string&) = 0;
        
        virtual bool
        remove_grpctxt_from_container(::FT::ObjectGroupId) = 0;
        
        virtual bool
        remove_objctxt_from_container(::CORBA::ULong) = 0;
        
    };

}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

std::ostream& operator<< (std::ostream &, const ::FT::Location &);

#endif // _CDMW_FT_REPLICATION_MANAGER_DELEGATE_H_

