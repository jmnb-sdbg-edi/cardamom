/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#include "Foundation/osthreads/Guard.hpp"
#include "clockservice/ControllerUpdateAdmin_impl.hpp"
#include <iostream>

Cdmw::clock::policy::ControllerUpdateAdmin_impl*
Cdmw::clock::policy::ControllerUpdateAdmin_impl::m_instance = 0;

Cdmw::OsSupport::Mutex
Cdmw::clock::policy::ControllerUpdateAdmin_impl::m_static_mutex;

///////////////////////////////////////////////////////////////////////////////

Cdmw::clock::policy::ControllerUpdateAdmin_impl::ControllerUpdateAdmin_impl()
{
}
	  
///////////////////////////////////////////////////////////////////////////////

Cdmw::clock::policy::ControllerUpdateAdmin_impl::~ControllerUpdateAdmin_impl()
{
    Cdmw::OsSupport::Guard<Cdmw::OsSupport::Mutex> guard(m_mutex);

    iterator it = m_dbPolicy.begin();
    iterator end = m_dbPolicy.end();    
    while (it != end)
    {
        CORBA::release(it->second);        
        ++it;
    }

    // this object is a singleton and it is not
    // really controlled from RefCountServantBase
    this->_set_ref_count(0);
}

///////////////////////////////////////////////////////////////////////////////
	  
void 
Cdmw::clock::policy::ControllerUpdateAdmin_impl::
_cxx_register (::Cdmw::clock::policy::ControllerUpdateID id,
	            ::Cdmw::clock::policy::ControllerUpdate_ptr policy) 
    throw (CORBA::SystemException, 
           Cdmw::clock::policy::ControllerUpdateAdmin::AlreadyExist) 
{
    Cdmw::OsSupport::Guard<Cdmw::OsSupport::Mutex> guard(m_mutex);

    iterator it = m_dbPolicy.find(id);
    iterator end = m_dbPolicy.end();    
    if (it != end) throw Cdmw::clock::policy::ControllerUpdateAdmin::AlreadyExist();

    m_dbPolicy[id] = Cdmw::clock::policy::ControllerUpdate::_duplicate(policy);
}
	  
///////////////////////////////////////////////////////////////////////////////
      
void
Cdmw::clock::policy::ControllerUpdateAdmin_impl::
unregister(::Cdmw::clock::policy::ControllerUpdateID id)
    throw (CORBA::SystemException,
   	           ::Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable,
               ::Cdmw::clock::policy::ControllerUpdateAdmin::OperationNotAllow) 
{
    Cdmw::OsSupport::Guard<Cdmw::OsSupport::Mutex> guard(m_mutex);

    iterator it = m_dbPolicy.find(id);
    iterator end = m_dbPolicy.end();    
    if (it == end) throw Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable();
    
    iterator begin = m_dbPolicy.begin();    
    if (distance(begin,it) <= Cdmw::clock::policy::ENFORCE_DEADLINE)
        throw Cdmw::clock::policy::ControllerUpdateAdmin::OperationNotAllow();
    
    CORBA::release(it->second);
    m_dbPolicy.erase(it);
}

///////////////////////////////////////////////////////////////////////////////
	  
::Cdmw::clock::policy::ControllerUpdate_ptr 
Cdmw::clock::policy::ControllerUpdateAdmin_impl::
get_policy(::Cdmw::clock::policy::ControllerUpdateID id)
    throw (CORBA::SystemException,
           ::Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable) 
{
    Cdmw::OsSupport::Guard<Cdmw::OsSupport::Mutex> guard(m_mutex);

    iterator it = m_dbPolicy.find(id);
    iterator end = m_dbPolicy.end();    
    if (it == end) throw Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable();
        
    Cdmw::clock::policy::ControllerUpdate_var controller_update =
        Cdmw::clock::policy::ControllerUpdate::_duplicate(it->second);
    return controller_update._retn();
}

///////////////////////////////////////////////////////////////////////////////

Cdmw::clock::policy::ControllerUpdateAdmin_impl*
Cdmw::clock::policy::ControllerUpdateAdmin_impl::instance()
{
    static  Cdmw::clock::policy::ControllerUpdateAdmin_impl  singleton;
    return &singleton;
}  

///////////////////////////////////////////////////////////////////////////////
// End Of File
///////////////////////////////////////////////////////////////////////////////
