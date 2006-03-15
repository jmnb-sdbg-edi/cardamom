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


#include <ftlocationmanager/ActivationManager_impl.hpp>

#include <iostream>

// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <idllib/FT.stub.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

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
//ActivationManager_impl::ActivationManager_impl(CdmwFT::Location::GroupRepository_ptr group_repository)
//: m_group_repository(CdmwFT::Location::GroupRepository::_duplicate(group_repository))
ActivationManager_impl::ActivationManager_impl(CORBA::ORB_ptr orb)
   : m_orb(CORBA::ORB::_duplicate(orb))
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CORBA::Object_var obj = CORBA::Object::_nil();
    try {
        obj = m_orb->resolve_initial_references("ReplicationManager");
        m_replication_manager = ::FT::ReplicationManager::_narrow(obj.in());
    } catch (const CORBA::Exception& e) {
        CDMW_FATAL(FTLogger::GetLogger(),
                   "ActivationManager_impl Cannot retrieve ReplicationManager: " << e );
        CDMW_ASSERT(false);
    }
        
    obj = CORBA::Object::_nil();
    try {
        obj = m_orb->resolve_initial_references("FTGroupRepository");
        m_pbk_group_repository 
            = CdmwFT::Location::PrimaryBackupGroupRepository::_narrow(obj.in());
    } catch (const CORBA::Exception& e) {
        CDMW_FATAL(FTLogger::GetLogger(),
                   "ActivationManager_impl Cannot retrieve FTGroupRepository: " << e );
        CDMW_ASSERT(false);
    }

    CDMW_ASSERT(!CORBA::is_nil(m_replication_manager.in()));
    CDMW_ASSERT(!CORBA::is_nil(m_pbk_group_repository.in()));
}

ActivationManager_impl::~ActivationManager_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

//
// IDL:thalesgroup.com/CdmwFT/Location/ActivationManager/register_activation_handler:1.0
//
CdmwFT::Location::HandlerId
ActivationManager_impl::register_activation_handler(::FT::ObjectGroup_ptr object_group_ref,
                                                    CdmwFT::Location::ActivationHandler_ptr handler)
    throw(::FT::ObjectGroupNotFound,
          CdmwFT::Location::HandlerAlreadyExist,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try
    {
        // get group_id
        ::FT::ObjectGroupId group_id = m_replication_manager->get_object_group_id(object_group_ref);
  
        {
            CDMW_MUTEX_GUARD(m_mutex);
            
            // find if a handler is not already defined for that group
            HandlerMap::const_iterator iter = m_activation_handlers.find(group_id);
            
            if (iter != m_activation_handlers.end())
            {
                // handler already defined
                CdmwFT::Location::HandlerAlreadyExist ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            
            CdmwFT::Location::ActivationHandler_var new_handler(CdmwFT::Location::ActivationHandler::_duplicate(handler));
            m_activation_handlers.insert(HandlerMap::value_type(group_id, new_handler));
        }

        // Check if the local member is not the primary
        try
        {
            bool is_already_elected_primary 
                = m_pbk_group_repository->is_primary_from_gid(group_id);
            
            if (is_already_elected_primary)
            {
                CDMW_DEBUG(FTLogger::GetLogger(),
                           "[ActivationManager] register_activation_handler: activate primary '" << group_id << "'");
                this->activate_member(group_id);
            }
        }
        catch (const ::FT::ObjectGroupNotFound& ex)
        {
            // nothing to do! LocationManager has not already been updated!
            CDMW_WARN(FTLogger::GetLogger(),
                      "register_activation_handler: LocationManager has not already been updated! '" << group_id << "'");
        }
        catch (const CORBA::SystemException& ex)
        {
            // TODO: check that there is really nothing to do!
            CDMW_DEBUG(FTLogger::GetLogger(),
                       "[ActivationManager] register_activation_handler: Caught exception '" << ex << "'" << group_id);
        }

        CDMW_LOG_FUNCTION_RETURN(group_id);
        // handlerID is the group id
        return group_id;
    }
    catch (const ::FT::ObjectGroupNotFound& ex)
    {
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
    catch (const CORBA::SystemException& ex )
    {
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
}

//
// IDL:thalesgroup.com/CdmwFT/Location/ActivationManager/unregister_activation_handler:1.0
//
void
ActivationManager_impl::unregister_activation_handler(CdmwFT::Location::HandlerId handler_id)
    throw(CdmwFT::Location::InvalidHandlerId,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"handler_id="<<handler_id);
    CDMW_MUTEX_GUARD(m_mutex);

    // try to find handler in the handler map
    HandlerMap::iterator h_iter = m_activation_handlers.find(handler_id);
    
    if (h_iter != m_activation_handlers.end())
    {
        m_activation_handlers.erase(h_iter);
    }
    else
    {
        CdmwFT::Location::InvalidHandlerId ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
}

// activate order towards a group member identified by its group id
void 
ActivationManager_impl::activate_member(::FT::ObjectGroupId group_id)
    throw(::FT::ObjectGroupNotFound)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"group_id="<<group_id);
    CDMW_MUTEX_GUARD(m_mutex);

    // check this group member has not already activated
    CDMW_DEBUG(FTLogger::GetLogger(),
               "[ActivationManager] activate_member: check this group member has not already activated '" 
               << group_id << "'");
    GroupIdSet::iterator g_iter = m_activated_group_id.find(group_id);

    if (g_iter == m_activated_group_id.end())
    {
        // this member has not already been activated

        // try to find handler int the handler map
        CDMW_DEBUG(FTLogger::GetLogger(),
                   "[ActivationManager] activate_member: try to find handler int the handler map '" << group_id << "'");
        HandlerMap::iterator h_iter = m_activation_handlers.find(group_id);
        
        if (h_iter != m_activation_handlers.end())
        {
            CdmwFT::Location::ActivationHandler_var l_handler = (*h_iter).second;
            
            try
            {
                CDMW_DEBUG(FTLogger::GetLogger(),
                           "[ActivationManager] activate_member: call activate on handler '" << group_id << "'");
                l_handler->activate();
                m_activated_group_id.insert(group_id);
            }
            catch (const CORBA::SystemException& e)
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "[ActivationManager] WARNING: Impossible to activate member of group '" 
                           << group_id << "' caught exception " 
                           << OrbSupport::OrbSupport::exception_to_string(e));
            }
        }
        else
        {
            // no handler registered for that group
            CDMW_DEBUG(FTLogger::GetLogger(),
                      "[ActivationManager] activate_member: throw ::FT::ObjectGroupNotFound() '" << group_id << "'");
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
            
        }
    }
    else
    {
        // member has already been activated, DO NOTHING!
    }
}


// deactivate order towards a group member identified by its group id
void 
ActivationManager_impl::deactivate_member(::FT::ObjectGroupId group_id)
    throw(::FT::ObjectGroupNotFound)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"group_id="<<group_id);
    CDMW_MUTEX_GUARD(m_mutex);

    // check this group member is activated
    GroupIdSet::iterator g_iter = m_activated_group_id.find(group_id);

    if (g_iter != m_activated_group_id.end())
    {
        // object has been activated

        // try to find handler int the handler map
        HandlerMap::iterator it = m_activation_handlers.find(group_id);
        
        if (it != m_activation_handlers.end())
        {
            CdmwFT::Location::ActivationHandler_var l_handler = (*it).second;
            
            try
            {
                l_handler->deactivate();
                m_activated_group_id.erase(group_id);
            }
            catch (const CORBA::SystemException& e)
            {
                CDMW_WARN(FTLogger::GetLogger(),
                           "[ActivationManager] WARNING: Impossible to deactivate member of group '" 
                          << group_id << "' caught exception " 
                          << OrbSupport::OrbSupport::exception_to_string(e));
            }
        }
        else
        {
            // no handler registered for that group
            CDMW_DEBUG(FTLogger::GetLogger(),
                      "[ActivationManager] activate_member: throw ::FT::ObjectGroupNotFound() '" << group_id << "'");
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
    else
    {
        // object has not been activated, DO NOTHING!
    }
}

} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

