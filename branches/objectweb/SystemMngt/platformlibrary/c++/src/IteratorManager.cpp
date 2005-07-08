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


#include "SystemMngt/platformlibrary/Iterator.hpp"


namespace Cdmw
{
namespace PlatformMngt
{


/**
*Implementation notes : none
*<p>
*Portability issues :   none
*/


PortableServer::POA_ptr IteratorManager::M_POA;


void IteratorManager::initialize(PortableServer::POA_ptr poa)
    throw (BadParameterException)
{

    // Initialisation can be done only once.
    CDMW_ASSERT(CORBA::is_nil(M_POA));

    if (CORBA::is_nil(poa))
    {
        CDMW_THROW2(BadParameterException, "poa", "nil");
    }

    M_POA = PortableServer::POA::_duplicate(poa);

}


CORBA::Object_ptr IteratorManager::createIterator(PortableServer::ServantBase* servant,
    BaseIterator* iterator)
    throw (BadParameterException, CORBA::SystemException)
{

    // Initialisation should have been done.
    CDMW_ASSERT(!CORBA::is_nil(M_POA));

    if (servant == NULL )
    {
        CDMW_THROW2(BadParameterException, "servant", "NULL");
    }

    if (iterator == NULL)
    {
        CDMW_THROW2(BadParameterException, "iterator", "NULL");
    }

    iterator->m_POA = PortableServer::POA::_duplicate(M_POA);
    iterator->m_oid = M_POA->activate_object(servant);
    return M_POA->id_to_reference(iterator->m_oid.in());

}


} // End of namespace PlatformMngt
} // End of namespace Cdmw

