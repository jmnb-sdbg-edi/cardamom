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


#include "testftwritesequencing/TestSequenceRecorder_impl.hpp"

namespace Cdmw
{

SequenceRecorder_impl::SequenceRecorder_impl(
    CORBA::ORB_ptr          orb,
    PortableServer::POA_ptr poa,
    const char*             hostname)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
    , m_poa(PortableServer::POA::_duplicate(poa))
    , m_host(hostname)
{
}

SequenceRecorder_impl::~SequenceRecorder_impl()
    throw()
{
}

void
SequenceRecorder_impl::insert(const char * host, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
    m_insert_hosts.push_front(host);
}

void
SequenceRecorder_impl::update(const char * host, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
    m_update_hosts.push_front(host);
}

void
SequenceRecorder_impl::remove(const char * host, CORBA::Long oid)
    throw(CORBA::SystemException)
{
    m_remove_hosts.push_front(host);
}


CORBA::Boolean
SequenceRecorder_impl::validate(const char * primary)
    throw(CORBA::SystemException)
{
    if ((m_insert_hosts.size() != 3) || (*m_insert_hosts.begin() != primary))
    {
        return false;
    }

#if 0
    if ((m_update_hosts.size() != 3) || (*m_update_hosts.begin() != primary))
    {
        return false;
    }

    if ((m_remove_hosts.size() != 3) || (*m_remove_hosts.begin() != primary))
    {
        return false;
    }
#endif

    return true;
}

}; // End namespace Cdmw

