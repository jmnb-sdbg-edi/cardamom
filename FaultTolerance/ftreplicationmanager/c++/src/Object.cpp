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

#include "ftreplicationmanager/Object.hpp"
#include "ftreplicationmanager/Location.hpp"
#include "ftreplicationmanager/ObjectGroup.hpp"

namespace Cdmw {
namespace  FT {
namespace ReplicationManager {
namespace Object {

    Object::Object ( Location::Location*         parent_location
                   , ObjectGroup::ObjectGroup*   parent_object_group
                   , ReplicationManagerDelegate* rep_mgr)
    : _fsm(*this)
    , m_locctxt(parent_location)
    , m_grpctxt(parent_object_group)
    , m_rep_mgr(rep_mgr)
    , m_fsm_state(::CdmwFT::MemberState_INITIAL)
    {
        // Uncomment to see debug output.
        //_fsm.setDebugFlag(true);
        m_mutex.lock();
        m_local_id_count++;
        m_local_id = m_local_id_count;
        m_mutex.unlock();
    }
    
    ::Cdmw::OsSupport::Mutex Object::Object::m_mutex;
    ::CORBA::Long Object::m_local_id_count = 0;

    Object::~Object ()
    {
        remove_from_container();
    }


    /*
     * Removes the pair (this->get_key, this) from the ContextContainer.
     */
    bool
    Object::remove_from_container ()
    {
        return m_rep_mgr->remove_objctxt_from_container(m_local_id);
    }

    /*
     * Returns the Object location name as a std::string &
     */
    const std::string&
    Object::locname()
    {
        return m_locctxt->name();
    }
    
    /*
     * Returns the Object ObjectGroupId
     */
    ::FT::ObjectGroupId
    Object::ogid()
    {
        return m_grpctxt->ogid();
    }

}; // namespace Object
}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

