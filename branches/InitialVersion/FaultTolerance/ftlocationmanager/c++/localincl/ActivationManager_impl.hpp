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


#ifndef INCL_CDMW_FT_LOCATION_ACTIVATION_MANAGER_IMPL_HPP 
#define INCL_CDMW_FT_LOCATION_ACTIVATION_MANAGER_IMPL_HPP 

#include <map>
#include <set>
#include <string>


// Cdmw Files
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>


#include <idllib/CdmwFTActivation.skel.hpp>
#include <idllib/CdmwFTGroupRepository.stub.hpp>


//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{

namespace FT
{

//
// IDL:thalesgroup.com/CdmwFT/Location:1.0
//
namespace Location
{

//
// IDL:thalesgroup.com/CdmwFT/Location/ActivationManager:1.0
//
class ActivationManager_impl : virtual public CdmwFT::Location::ActivationManager, 
                               virtual public Cdmw::OrbSupport::RefCountLocalObject

{

public:
    ActivationManager_impl(CORBA::ORB_ptr );
    ~ActivationManager_impl();

    //
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationManager/register_activation_handler:1.0
    //
    virtual CdmwFT::Location::HandlerId
        register_activation_handler(::FT::ObjectGroup_ptr object_group_ref,
                                    CdmwFT::Location::ActivationHandler_ptr handler)
        throw(::FT::ObjectGroupNotFound,
              CdmwFT::Location::HandlerAlreadyExist,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationManager/unregister_activation_handler:1.0
    //
    virtual void unregister_activation_handler(CdmwFT::Location::HandlerId handler_id)
       throw(CdmwFT::Location::InvalidHandlerId,
              CORBA::SystemException);

    // activate order towards a group member identified by its group id
    void activate_member(::FT::ObjectGroupId group_id)
        throw(::FT::ObjectGroupNotFound);

    
    // deactivate order towards a group member identified by its group id
    void deactivate_member(::FT::ObjectGroupId group_id)
        throw(::FT::ObjectGroupNotFound);

private:
    ActivationManager_impl();
    ActivationManager_impl(const ActivationManager_impl&);
    void operator=(const ActivationManager_impl&);

    // list of activation handler order by their membership group_id 
    typedef 
        std::map< ::FT::ObjectGroupId, 
                  CdmwFT::Location::ActivationHandler_var, 
                  std::less< ::FT::ObjectGroupId> > HandlerMap;
    HandlerMap  m_activation_handlers;  

    // set of activated member identified by their group_id 
    // (there is only one member per group in a location!)
    typedef std::set< ::FT::ObjectGroupId, std::less< ::FT::ObjectGroupId>, std::allocator< ::FT::ObjectGroupId> >  GroupIdSet;
    GroupIdSet  m_activated_group_id;

    // Mutex protecting m_activation_handlers and m_activated_group_id manipulation
    Cdmw::OsSupport::Mutex     m_mutex;

    CORBA::ORB_var m_orb;

    // Reference to the replication manager
    ::FT::ReplicationManager_var m_replication_manager;

    // Reference to the Group repository
    CdmwFT::Location::PrimaryBackupGroupRepository_var m_pbk_group_repository;


};

} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

#endif // INCL_CDMW_FT_LOCATION_ACTIVATION_MANAGER_IMPL_HPP 

