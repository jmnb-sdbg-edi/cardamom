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


#ifndef _CDMW_FT_REPLICATION_MANAGER_OBJECT_GROUP_CTXT_H_
#define _CDMW_FT_REPLICATION_MANAGER_OBJECT_GROUP_CTXT_H_

#include <list>
#include <map>

#include "idllib/FT.stub.hpp"
#include "idllib/CdmwFTStateFullPrimaryBackupAdmin.stub.hpp"
#include "ftreplicationmanager/CdmwFTObjectGroupData.stub.hpp"
#include "ftreplicationmanager/ObjectGroup_sm.h"
#include "ftreplicationmanager/Object.hpp"

namespace Cdmw {
namespace FT {
namespace ReplicationManager {

    // Forward
    class ReplicationManagerDelegate;

namespace Location {
    // Forward
    class Location;
}
    
namespace ObjectGroup {

    /**
     * ObjectGroup: the context class tied with the generated ObjectGroup_sm
     * state machine. This class is actually part of the ReplicationManager,
     * hence its declaration within the ReplicationManager namespace. The 
     * additional namespace "ObjectGroup" is needed to avoid name clashes in
     * include files provided by the state machine generation tool (SMC) when
     * more than one machine is nested in the same namespace.
     */
    class ObjectGroup {
    
    private:

        ObjectGroupContext _fsm;

    public:

        /**
         * Creates an ObjectGroup context.
         *
         */
        ObjectGroup ( ::FT::ObjectGroupId           ogid
                    , ReplicationManagerDelegate*   rep_mgr);
        
        /**
         * Destroys an ObjectGroup context.
         */
        ~ObjectGroup();

        // -------- Methods called by the replication manager --------
          
        ::FT::ObjectGroupId ogid()                       { return m_ogid; }
        ::CdmwFT::ObjectGroupState fsm_state()           { return m_fsm_state;  }
        int fsm_internal_state()                         { return _fsm.getState().getId();}

        // #### WARNING - WARNING - WARNING - WARNING - WARNING - WARNING ####
        // This modifier is **RESERVED** it **MUST NOT** be used in a regular
        // CARDAMOM code or it will lead to an inconsistent machine state
        void fsm_state(::CdmwFT::ObjectGroupState state) { m_fsm_state = state; }
        void fsm_internal_state(int state)               { _fsm.setState(_fsm.valueOf(state)); }
        
        // #### WARNING - WARNING - WARNING - WARNING - WARNING - WARNING ####

        /*
         * Association/Disassociation ObjectGroup/Location
         */
        void associate_location  (
                const std::string&        locname
              , Location::Location*       location);
        bool disassociate_location (
                const std::string&        locname);
        
        /*
         * Association/Disassociation ObjectGroup/Objects
         *
         * throw BAD_INV_ORDER if attempting to associate an object
         * while the Location does not exist (NULL loc pointer).
         */
        void associate_object (
                const std::string&        locname
              , Object::Object*           object)
            throw(::CORBA::BAD_INV_ORDER);
        bool disassociate_object (
                const std::string&        locname);

        bool remove_location (
                const std::string&        locname);

        /*
         * Disassociate the current ObjectGroup from its Location and Objects
         */
        void disassociate ();
        
        Location::Location* get_locctxt(const std::string& name);
        Object::Object*     get_objctxt(const std::string& name);

        /*
         * Returns wether the group is replicated using datastore or not.
         * May only be called after add_member has been called on this
         * group.
         */
        ::CdmwFT::GroupStateness stateness () { return m_stateness; }
        void stateness(CdmwFT::GroupStateness state)  { m_stateness = state; }


        /*
         * Returns true if the location and all the groups within that 
         * have the same stateness, false else.
         */
        bool
        match_stateness (
                Location::Location&       location);
    
        
        /*
         * Removes the pair (this->get_key, this) from the ContextContainer.
         */
        bool
        remove_from_container ();
        
        // -------- Methods related to the location state machine ----
          
        /**
         * Transitions of the associated state machine.
         */
        void ev_primary_ready  (Object::Object& objctxt)
            throw (CORBA::INTERNAL);

        void ev_backup_ready   (Object::Object& objctxt)
            throw (CORBA::INTERNAL);
      
        void ev_member_removed (Object::Object& objctxt)
            throw (CORBA::INTERNAL);

        /**
         * Actions called out when a transition is issued.
         */
        void primary_ready  (Object::Object& objctxt)
            throw (CORBA::INTERNAL);

        void backup_ready   (Object::Object& objctxt)
            throw (CORBA::INTERNAL);
      
        void member_removed (Object::Object& objctxt)
            throw (CORBA::INTERNAL);

        void notify_observers () throw();

        /*
         * 
         */
        void add_primary (
                Location::Location*       locctxt
              , Object::Object*           objctxt
              , ::CORBA::Object_ptr       member)
            throw ( ::FT::ObjectNotAdded
                  , ::CORBA::SystemException);

        /*
         * 
         */
        void add_backup (
                Location::Location*       locctxt
              , Object::Object*           objctxt
              , ::CORBA::Object_ptr       member)
            throw ( ::FT::ObjectNotAdded
                  , ::CORBA::SystemException);

        /*
         * 
         */
        void remove_member (
                Object::Object*           member
              , bool                      is_primary)
            throw ( ::FT::PrimaryNotSet
                  , ::FT::ObjectNotCreated
                  , ::FT::MemberNotFound
                  , ::CORBA::SystemException);


        /**
         *
         */
        void set_primary(const ::FT::Location&           primary_loc)
            throw(::FT::PrimaryNotSet
                  , ::CORBA::SystemException);

        /**
         * Appends a StatefullGroupInfo build from the data that  are
         * specific to the current object group to the sequence passed
         * as an inout parameter.
         *
         * @param object_group_id
         *
         * @return A pointer to a group_info structure 
         *
         * @exception CORBA::NO_MEMORY Failure to allocate memory.
         */
        void
        update_statefull_infos (
                ::CdmwFT::Location::StatefullGroupInfos& group_infos)
            throw (::CORBA::SystemException);

        /**
         * Extracts a GroupInfo structure from the current ObjectGroup
         *
         * @return A pointer to a group_info structure 
         *
         * @exception CORBA::NO_MEMORY Failure to allocate memory.
         */
        ::CdmwFT::GroupInfo*
        get_object_group_info ()
        throw ( ::CORBA::NO_MEMORY
              , ::CORBA::INTERNAL);
        
        /*
         * Static method: Extracts a GroupInfo structure from the ObjectGroupData
         * instance passed as an in parameter.
         *
         * @return A pointer to a group_info structure 
         *
         * @exception CORBA::NO_MEMORY Failure to allocate memory.
         */
        static ::CdmwFT::GroupInfo*
        get_object_group_info (const ::CdmwFT::ObjectGroupData& ogd)
            throw(::CORBA::NO_MEMORY);
        
        /**
         * Builds a LocalDataStoreInfo sequence containing only the items with a
         * DataStoreID that matches one of the IDs passed at create_object time.
         *
         * @param ogd The object group data object holding all known
         * data concering an object group.
         *
         * @param local_dsinfos The LocalDataStoreInfo sequence associated to the
         * location (passed at register_statefull_location time).
         *
         * @return A pointer to a LocalDataStoreInfos sequence 
         *
         * @exception CORBA::NO_MEMORY Failure to allocate memory.
         */
        ::CdmwFT::StateTransfer::LocalDataStoreInfos *
        get_local_data_store_infos (
                const ::CdmwFT::StateTransfer::LocalDataStoreInfos & local_dsinfos
              ) const
            throw (CORBA::NO_MEMORY);

        /*
         * Return the list of the replicated Locations containing this group.
         *
         * @param running indicates wether a member is present on a location
         */
        void
        get_peer_locations (std::list < Location::Location* > & peers_list, bool running = false);
          
    public:
        /*
         * Actions called while a transition is occuring: public because
         * called by the classes generated by the SMC tool
         */
        void members_count(int count)      { m_members_count = count; }
        int  members_count()               { return m_members_count; }
        void inc_members_count()           { ++m_members_count; }
        void dec_members_count()           { --m_members_count; }
        void min_members_count(int count)  { m_min_members_count = count; }
        int  min_members_count()           { return m_min_members_count; }

        struct Infos {
            Infos() : loc(NULL), obj(NULL) { }
            Location::Location* loc;
            Object::Object*     obj;
        };
        typedef std::map < std::string, Infos > InfosMap;

        InfosMap& get_infos() { return m_infos; }

    private:

        int                          m_members_count;
        int                          m_min_members_count;

        ReplicationManagerDelegate*  m_rep_mgr;
        ::CdmwFT::GroupStateness     m_stateness;
        ::CdmwFT::ObjectGroupState   m_fsm_state;
        ::FT::ObjectGroupId          m_ogid;

        // Associated Location and Objects
        InfosMap                     m_infos;

    private:

        // Hide the default constructor.
        ObjectGroup(void);
        
        // Hide the copy constructor.
        ObjectGroup(const ObjectGroup & obj_grp);
        
    };

}; // namespace ObjectGroup
}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_REPLICATION_MANAGER_OBJECT_GROUP_CTXT_H_

