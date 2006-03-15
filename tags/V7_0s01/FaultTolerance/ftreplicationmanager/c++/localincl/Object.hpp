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


#ifndef _CDMW_FT_REPLICATION_MANAGER_OBJECT_H_
#define _CDMW_FT_REPLICATION_MANAGER_OBJECT_H_

#include "Foundation/osthreads/Mutex.hpp"
#include "ftreplicationmanager/Object_sm.h"
#include "ftreplicationmanager/ReplicationManagerDelegate.hpp"

namespace Cdmw {
namespace FT {
namespace ReplicationManager {

namespace Location {
    // Forward
    class Location;
}

namespace ObjectGroup {
    // Forward
    class ObjectGroup;
}

namespace Object {

    /**
     * Object: the context class tied with the generated ObjectGroupFSM
     * state machine. This class is actually part of the ReplicationManager,
     * hence its declaration within the ReplicationManager namespace.
     */
    class Object {
    
    private:

        ObjectContext _fsm;

    public:

        /**
         * Creates an ObjectGroup context.
         *
         */
        Object ( Location::Location*         parent_location
               , ObjectGroup::ObjectGroup*   parent_group
               , ReplicationManagerDelegate* rep_mgr);
        
        /**
         * Destroys an ObjectGroup context.
         */
        ~Object();

        void clear_parent_location()  { m_locctxt = NULL; }
        void clear_parent_group()     { m_grpctxt = NULL; }

        /**
         * Transitions of this machine.
         */
        void ev_init_primary_done() { _fsm.ev_init_primary_done();  }
        void ev_init_backup_done()  { _fsm.ev_init_backup_done();   }
        void ev_set_primary()       { _fsm.ev_set_primary(); }
        void ev_failure()           { _fsm.ev_failure(); }

        // #### WARNING - WARNING - WARNING - WARNING - WARNING - WARNING ####
        // This modifier is **RESERVED** it **MUST NOT** be used in a regular
        // CARDAMOM code or it will lead to an inconsistent machine state
        void fsm_state(::CdmwFT::MemberState state)      { m_fsm_state = state; }
        void fsm_internal_state(int state)               { _fsm.setState(_fsm.valueOf(state)); }
        // #### WARNING - WARNING - WARNING - WARNING - WARNING - WARNING ####

        ::CdmwFT::MemberState fsm_state()                { return m_fsm_state; }
        int fsm_internal_state()                         { return _fsm.getState().getId();}

        /*
         * The local_id is a unique identifier assigned to the Object
         * at creation time. This is a read-only attribute.
         */
        ::CORBA::Long local_id()                    { return m_local_id; }
        void local_id(::CORBA::Long id)             { m_local_id = id; }
            
        /*
         * Accessors to parent location name and parent object group ID
         */
        const std::string&   locname();
        ::FT::ObjectGroupId  ogid();
 
        /*
         * Removes the pair (this->get_key, this) from the ContextContainer.
         */
        bool
        remove_from_container ();
        
    private:

        Location::Location*             m_locctxt;
        ObjectGroup::ObjectGroup*       m_grpctxt;
        ReplicationManagerDelegate*     m_rep_mgr;

        /*
         * NOTE: SMC doesn't generate a header file containing the
         *       declaration of symbolic constants for its internal
         *       machine state. IDL files contain declaration of
         *       state values to save in DataStore. That's why we
         *       need to save both kinds of state.
         */
        ::CdmwFT::MemberState           m_fsm_state;
        //int                             m_fsm_internal_state;
        ::CORBA::Long                   m_local_id;

    public:
        static ::CORBA::Long            m_local_id_count;

    private:
        static ::Cdmw::OsSupport::Mutex m_mutex;

        // A reference to an output stream for debug and trace information
        //std::ostream &   m_ostream;


    private:

        // Hide the default constructor.
        Object(void);
        
        // Hide the copy constructor.
        Object(const Object & obj_grp);
        
    };

}; // namespace Object
}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_REPLICATION_MANAGER_OBJECT_H_

