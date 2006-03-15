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


#include "testftlocationmanager/TestSync_impl.hpp"

namespace Cdmw
{

SynchInterface_impl::SynchInterface_impl()
    throw(CORBA::SystemException)
    : m_flags(false)
{ 
}
SynchInterface_impl::~SynchInterface_impl()
    throw()
{
}

CORBA::Boolean SynchInterface_impl::get_OK ()
    throw(CORBA::SystemException)
{
    return m_flags;
}


void SynchInterface_impl::set_OK (CORBA::Boolean ok)
    throw(CORBA::SystemException)
{
    m_flags = ok;
}

::FT::ObjectGroup_ptr SynchInterface_impl::get_result_get_object_group()
    throw(CORBA::SystemException)
{
    return ::FT::ObjectGroup::_duplicate(m_result_get_object_group.in());
}

::CdmwFT::MemberInfos * SynchInterface_impl::get_result_get_other_group_members()
    throw(CORBA::SystemException)
{
    return m_result_get_other_group_members._retn();
}

::FT::ObjectGroupRefVersion SynchInterface_impl::get_result_get_object_group_version()
    throw(CORBA::SystemException)
{
    return m_result_get_object_group_version;
}


::FT::ObjectGroup_ptr SynchInterface_impl::get_result_get_object_group_from_gid()
    throw(CORBA::SystemException)
{
    return ::FT::ObjectGroup::_duplicate(m_result_get_object_group_from_gid.in());
}

::CdmwFT::MemberInfos * SynchInterface_impl::get_result_get_other_group_members_from_gid()
    throw(CORBA::SystemException)
{
    if (m_result_get_other_group_members_from_gid == NULL)
    {
        return NULL;
    }
    return m_result_get_other_group_members_from_gid._retn();
}

::FT::ObjectGroupRefVersion SynchInterface_impl::get_result_get_object_group_version_from_gid()
    throw(CORBA::SystemException)
{
    return m_result_get_object_group_version_from_gid;
}

CORBA::Boolean SynchInterface_impl::get_result_is_primary()
    throw(CORBA::SystemException)
{
    return m_result_is_primary;
}

CORBA::Boolean SynchInterface_impl::get_result_is_primary_from_gid()
    throw(CORBA::SystemException)
{
        return m_result_is_primary_from_gid;
}

CORBA::Boolean SynchInterface_impl::get_activation()
    throw(CORBA::SystemException)
{
        return m_activation;
}




}; // End namespace Cdmw

