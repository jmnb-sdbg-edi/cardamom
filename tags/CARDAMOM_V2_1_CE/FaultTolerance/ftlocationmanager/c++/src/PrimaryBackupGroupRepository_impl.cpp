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


// Cdmw Files
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp>

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

PrimaryBackupGroupRepository_impl* PrimaryBackupGroupRepository_impl::mp_primary_backup_instance = 0;

PrimaryBackupGroupRepository_impl* 
PrimaryBackupGroupRepository_impl::Get_instance()
    throw()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if(mp_primary_backup_instance == 0)
        {
            // acquire a mutex
            CDMW_MUTEX_GUARD(M_mutex);
            
            // double check
            // some thread can to have already initialized it
            if (mp_primary_backup_instance == 0)
                {
                    // create instance
                    mp_primary_backup_instance = new PrimaryBackupGroupRepository_impl();
                    mp_instance = mp_primary_backup_instance;
                }
        }

    return mp_primary_backup_instance;
}

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository:1.0
//
PrimaryBackupGroupRepository_impl::PrimaryBackupGroupRepository_impl()
: GroupRepository_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

PrimaryBackupGroupRepository_impl::~PrimaryBackupGroupRepository_impl()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository/is_primary:1.0
//
CORBA::Boolean
PrimaryBackupGroupRepository_impl::is_primary(CORBA::Object_ptr member)
    throw(::FT::MemberNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CORBA::Boolean result = false;

    {
        CDMW_MUTEX_GUARD(m_mutex);

        
        for (PrimaryMap::const_iterator iter = m_primary_members.begin(); 
        iter != m_primary_members.end() ; iter++)
        {
            if (member->_is_equivalent((*iter).second.in()))
            {
                // primary is found
                result = true;
                
                // get out from the loop
                break;
            }
        }
    }

    if (result == false)
    {
        // Check this member is a local member
        try
        {
            ::FT::ObjectGroup_var object_group 
                = GroupRepository_impl::get_object_group(member);
        }
        catch (const ::FT::MemberNotFound& ex)
        {
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
        catch (const CORBA::SystemException& ex)
        {
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
    }

    CDMW_LOG_FUNCTION_RETURN(result);
    return result;
}

//
// IDL:thalesgroup.com/CdmwFT/Location/PrimaryBackupGroupRepository/is_primary_from_gid:1.0
//
CORBA::Boolean
PrimaryBackupGroupRepository_impl::is_primary_from_gid(::FT::ObjectGroupId object_group_id)
    throw(::FT::ObjectGroupNotFound,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    CDMW_MUTEX_GUARD(m_mutex);

    CORBA::Boolean result = false;

    PrimaryMap::const_iterator iter = m_primary_members.find(object_group_id);


    if (iter == m_primary_members.end())
    {
        // primary not found, does the group exist in this location?
        try
        {
            ::FT::ObjectGroup_var object_group 
                = this->get_object_group_from_gid(object_group_id);
        }
        catch (const ::FT::ObjectGroupNotFound& ex)
        {
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
        catch (const CORBA::SystemException& ex)
        {
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
    }
    else
    {
        // object found in the primary map
        result = true;
    }

    CDMW_LOG_FUNCTION_RETURN(result);
    return result;
}

// add the new primary member reference with its group id in the map
void 
PrimaryBackupGroupRepository_impl::add_primary(::FT::ObjectGroupId object_group_id, 
                                               CORBA::Object_ptr new_primary_member)
    throw()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    CDMW_MUTEX_GUARD(m_mutex);

    PrimaryMap::const_iterator iter = m_primary_members.find(object_group_id);

    if (iter == m_primary_members.end())
    {
        // this is not already a primary, insert it in the map
        m_primary_members.insert(PrimaryMap::value_type(object_group_id, 
                                                        CORBA::Object::_duplicate(new_primary_member) ));
    }
    else
    {
        // nothing to do ! The members has already been added in the map.
    }
}

// remove the old primary member reference from the map
void 
PrimaryBackupGroupRepository_impl::remove_primary(::FT::ObjectGroupId object_group_id)
    throw(::FT::MemberNotFound)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id);
    CDMW_MUTEX_GUARD(m_mutex);
    PrimaryMap::const_iterator iter = m_primary_members.find(object_group_id);

    if (iter != m_primary_members.end())
    {
        // primary id found, delete it from the map
        m_primary_members.erase(object_group_id);
    }
    else
    {
        ::FT::MemberNotFound ex;
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
}


} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

