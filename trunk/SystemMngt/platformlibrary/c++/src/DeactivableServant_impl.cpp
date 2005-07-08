/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include <sstream>

namespace Cdmw
{
namespace PlatformMngt
{


DeactivableServant_impl::DeactivableServant_impl(PortableServer::POA_ptr poa)
{

    if (CORBA::is_nil(poa))
        CDMW_THROW2(BadParameterException, "poa", "nil");

    m_poa = PortableServer::POA::_duplicate(poa);

}


PortableServer::POA_ptr DeactivableServant_impl::_default_POA()
{

    return PortableServer::POA::_duplicate(m_poa.in());

}


DeactivableServant_impl::~DeactivableServant_impl()
{

}


void DeactivableServant_impl::activate()
    throw (AlreadyDoneException, InternalErrorException)
{

    try
    {
        m_oid = m_poa->activate_object(this);
    }
    catch(const PortableServer::POA::ServantAlreadyActive&)
    {
        CDMW_THROW(AlreadyDoneException);
    }
    catch(...)
    {
        CDMW_THROW(InternalErrorException);
    }


}


void DeactivableServant_impl::activateWithId(const std::string& objectId)
    throw (AlreadyDoneException, InternalErrorException)
{

    try
    {

        m_oid = PortableServer::string_to_ObjectId(objectId.c_str());

        m_poa->activate_object_with_id(m_oid.in(), this);

    }
    catch(const PortableServer::POA::ObjectAlreadyActive&)
    {
        CDMW_THROW(AlreadyDoneException);
    }
    catch(const PortableServer::POA::ServantAlreadyActive&)
    {
        CDMW_THROW(AlreadyDoneException);
    }
    catch(...)
    {
        CDMW_THROW(InternalErrorException);
    }


}


void DeactivableServant_impl::deactivate()
    throw (BadOrderException, InternalErrorException)
{

    try
    {

        m_poa->deactivate_object(m_oid.in());

    }
    catch(const PortableServer::POA::ObjectNotActive&)
    {
        CDMW_THROW(BadOrderException);
    }
    catch(...)
    {
        CDMW_THROW(InternalErrorException);
    }

}


} // End namespace PlatformMngt
} // End namespace Cdmw


