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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_DATASTOREGROUP_IMPL_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_DATASTOREGROUP_IMPL_HPP

#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <idllib/CdmwFTStateTransfer.skel.hpp>


namespace Cdmw {
namespace FT {

class DataStoreGroup_impl:
            virtual public POA_CdmwFT::StateTransfer::DataStoreGroup,
            virtual public Cdmw::OrbSupport::RefCountServantBase 
{
public:
    DataStoreGroup_impl(PortableServer::POA_ptr poa, DataStoreContext& context);
    virtual PortableServer::POA_ptr _default_POA();

    ::CdmwFT::StateTransfer::RecordIterator_ptr
    get_record_iterator
    (const char * from_location,
     const CdmwFT::StateTransfer::DataStoreIDs& datastore_ids,
     CdmwFT::Transaction::TPCCoordinator_ptr coordinator,
     CdmwFT::Transaction::TPCCohort_ptr cohort)
    throw (CORBA::SystemException,
           CdmwFT::StateTransfer::InvalidDataStoreState);

private:
    PortableServer::POA_var m_poa;
    DataStoreContext& m_context;
};

}
}

#endif
