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

#include "FaultTolerance/ftstatemanager/TXManager.hpp"
#include <FaultTolerance/ftstatemanager/TXDataStoreBase.hpp>
#include <Foundation/osthreads/WriterLockGuard.hpp>
#include <Foundation/osthreads/ReaderLockGuard.hpp>
#include <Foundation/common/Assert.hpp>

namespace Cdmw {
namespace FT {

TXDataStoreBase::TXDataStoreMap TXDataStoreBase::M_dsMap;
Cdmw::OsSupport::ReaderWriterLock TXDataStoreBase::M_dsMapLock;

TXDataStoreBase::~TXDataStoreBase()
{
}

void TXDataStoreBase::Register_datastore(TXDataStoreBase* ds)
{
    CDMW_WRITER_LOCK_GUARD(M_dsMapLock);
    M_dsMap[ds->get_dsid()] = ds;
}

void TXDataStoreBase::Unregister_datastore(TXDataStoreBase* ds)
{
    CDMW_WRITER_LOCK_GUARD(M_dsMapLock);
    M_dsMap.erase(ds->get_dsid());
}

TXDataStoreBase* TXDataStoreBase::Get_dataStore(const DsIdentifier& dsid)
{
    CDMW_READER_LOCK_GUARD(M_dsMapLock);
    CDMW_ASSERT(M_dsMap.find(dsid) != M_dsMap.end());
    return M_dsMap[dsid];
}

}  // End namespace FT
}  // End namespace Cdmw
