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


#include <iostream>

// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/PrimaryBackupAdmin_impl.hpp>
#include <FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <ftlocationmanager/ActivationManager_impl.hpp>
#include <ftlocationmanager/LocationGroupInfo.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

namespace 
{
    const CORBA::ORB::ObjectId ACTIVATION_MANAGER_ID = "FTActivationManager";

} // end of anonymous namespace

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
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupAdmin:1.0
//
PrimaryBackupAdmin_impl::PrimaryBackupAdmin_impl
        (CORBA::ORB_ptr orb,
         PortableServer::POA_ptr poa,
         const ::FT::Location& the_location,
         PrimaryBackupGroupRepository_impl*  group_repository,
         bool is_ftmanager)
    : MembersAdmin_impl(orb, poa, the_location, group_repository, is_ftmanager),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_primary_backup_group_repository(group_repository),
      m_is_ftmanager(is_ftmanager)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"is_ftmanager="<<is_ftmanager);
    if (is_ftmanager != true)
    {
        
        m_activation_manager = new ActivationManager_impl(orb);
        CdmwFT::Location::ActivationManager_ptr activation_manager
            = m_activation_manager;

        try
        {
            orb->register_initial_reference(ACTIVATION_MANAGER_ID, activation_manager);
        }
        catch (const CORBA::ORB::InvalidName& )
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "[PrimaryBackupAdmin] ERROR: Impossible to register ACTIVATION_MANAGER! Name is invalid!");
            CDMW_ASSERT(false);
        }
        catch (const CORBA::SystemException& e)
        {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "[PrimaryBackupAdmin] ERROR: Impossible to register ACTIVATION_MANAGER! \n"
                       << e );
            CDMW_ASSERT(false);
        }
    }    
}

PrimaryBackupAdmin_impl::~PrimaryBackupAdmin_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (m_is_ftmanager != true)
    {
        delete m_activation_manager;
    }    
}

PortableServer::POA_ptr
PrimaryBackupAdmin_impl::_default_POA()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    return PortableServer::POA::_duplicate(m_poa.in());
}

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupAdmin/update_primary:1.0
//
void
PrimaryBackupAdmin_impl::update_primary(const CdmwFT::GroupInfo& group_info,
                                        const ::FT::Location& primary_location)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    // group version has changed, update group in the group repository
    try
    {
        m_primary_backup_group_repository->update_group_view(group_info);
    }
    catch (const ::FT::ObjectGroupNotFound& ex)
    {
        // This group has already been removed or has never existed in this location
        CDMW_WARN(FTLogger::GetLogger(),"This group has already been removed or has never existed in this location");
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }

    // is the member in the current location the old primary
    CORBA::Boolean is_member_the_old_primary 
        = m_primary_backup_group_repository->is_primary_from_gid(group_info.object_group_id);

    ::FT::Location* location = this->the_location();
    CDMW_ASSERT(location);
    
    LocationGroupInfo* loc_group_info = NULL;
    bool object_group_not_found = false;
    try
    {
        loc_group_info = new LocationGroupInfo(group_info, *location);
    }
    catch (const ::FT::ObjectGroupNotFound& )
    {
        object_group_not_found = true;
    }

    if (MembersAdmin_impl::get_the_location() 
             == Cdmw::CommonSvcs::Naming::NamingInterface::to_string(primary_location))
    {
        // the new primary is in this location
        if (is_member_the_old_primary == false)
        {
            if (m_is_ftmanager != true)
            {                
                try
                {
                    // The local group member is not the old primary, 
                    // activate it!
                    m_activation_manager->activate_member(group_info.object_group_id);
                }
                catch (const ::FT::ObjectGroupNotFound&)
                {
                    CDMW_DEBUG(FTLogger::GetLogger(),
                               "[PrimaryBackupAdmin] WARNING: No handler registered to activate the new primary!" );
                    // TO DO : For the moment, it is  possible to set a new primary without activation handle
                    // in the furtur, this can be configurable
                    //throw;
                }
                catch (const CORBA::SystemException& ex)
                {
                    delete location;
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw;
                }
            }
            
            // advise group_repository to store this information
            CORBA::Object_var new_primary = loc_group_info->get_local_object_ref();
            
            m_primary_backup_group_repository->add_primary(group_info.object_group_id,
                                                           new_primary.in());
        }
        else
        {
            // the local group member is the old primary, do nothing!
            CDMW_WARN(FTLogger::GetLogger(),
                      "[PrimaryBackupAdmin] WARNING: The local member is already the primary!");
        }
    }
    else
    {   
       if (is_member_the_old_primary == true)
        {
            if (m_is_ftmanager != true)
            {
                // the old primary is the local member, deactivate it!
                try
                {
                    m_activation_manager->deactivate_member(group_info.object_group_id);
                }
                catch (const ::FT::ObjectGroupNotFound& ex)
                {
                    if (loc_group_info != NULL)
                        delete loc_group_info;
                    delete location;
                    CDMW_DEBUG(FTLogger::GetLogger(),
                               "[PrimaryBackupAdmin] WARNING: No handler registered to deactivate the old primary!");
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw;
                }
                catch (const CORBA::SystemException& ex)
                {
                    if (loc_group_info != NULL)
                        delete loc_group_info;                   
                    delete location;
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw;
                }   
            }            

            // advise group_repository to store this information
           m_primary_backup_group_repository->remove_primary(group_info.object_group_id);
        }
        else
        {
            // neither the new primary, nor the old primary is a local member
            // this operation is equivalent to an update_group_view which has been
            // done at the beginning
            // DO NOTHING ELSE!
        }       
    }
    if (loc_group_info != NULL)
        delete loc_group_info;
    delete location;
}


} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

