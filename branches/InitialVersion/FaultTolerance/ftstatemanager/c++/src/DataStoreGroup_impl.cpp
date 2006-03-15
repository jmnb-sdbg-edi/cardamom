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

#include "ftstatemanager/DataStoreGroup_impl.hpp"

namespace Cdmw {
namespace FT {

DataStoreGroup_impl::DataStoreGroup_impl(PortableServer::POA_ptr poa,
                                         DataStoreContext& context)
    : m_poa(PortableServer::POA::_duplicate(poa)),
      m_context(context)
{
}

PortableServer::POA_ptr DataStoreGroup_impl::_default_POA()
{
    return PortableServer::POA::_duplicate(m_poa.in());
}

::CdmwFT::StateTransfer::RecordIterator_ptr
DataStoreGroup_impl::get_record_iterator
(const char * from_location,
 const CdmwFT::StateTransfer::DataStoreIDs& datastore_ids,
 CdmwFT::Transaction::TPCCoordinator_ptr coordinator,
 CdmwFT::Transaction::TPCCohort_ptr cohort)
    throw (CORBA::SystemException,
           CdmwFT::StateTransfer::InvalidDataStoreState)
{
    return m_context.get_record_iterator(from_location, datastore_ids,
                                         coordinator, cohort);
}

}
}
