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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_TXDATASTOREBASE_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_TXDATASTOREBASE_HPP

#include <FaultTolerance/idllib/CdmwFTTPCCommon.stub.hpp>
#include <FaultTolerance/ftstatemanager/TXManager.hpp>
#include <Foundation/commonsvcs/datastore/DataStoreBase.hpp>
#include <map>

namespace Cdmw {
namespace FT {

class TXDataStoreBase : public Cdmw::CommonSvcs::DataStore::DataStoreBase
{
    typedef std::map<DsIdentifier, TXDataStoreBase*> TXDataStoreMap;

public:
    /**
     * Return the TXManager associated with this TXDataStore.
     *
     * @return the TXManager
     */
    virtual TXManager& get_manager() = 0;

    /**
     * Obtain the set of changes associated with this transaction.
     *
     * @param change the changes
     */
    virtual void get_change(::CdmwFT::Transaction::StateChange& change) = 0;

    /**
     * Clears the cache of uncommited changes.
     */
    virtual void clear_cache() = 0;

    /**
     * Applys cached changes locally.
     */
    virtual void flush() = 0;

    static TXDataStoreBase* Get_dataStore(const DsIdentifier& dsid);

protected:

    /**
    * Purpose:
    * <p> Destructor.
    */
    virtual ~TXDataStoreBase();

    /**
    * Purpose:
    * <p> Default constructor.
    */
    TXDataStoreBase(DsIdentifier dsid)
        : Cdmw::CommonSvcs::DataStore::DataStoreBase(dsid)
    {
    }
    

    /**
    * Assignement and copy operators are not allowed.
    */
    TXDataStoreBase& operator=(const TXDataStoreBase& rhs);
    TXDataStoreBase(const TXDataStoreBase& rhs);

    static void Register_datastore(TXDataStoreBase* ds);

    static void Unregister_datastore(TXDataStoreBase* ds);

private:
    static TXDataStoreMap M_dsMap;
    static Cdmw::OsSupport::ReaderWriterLock M_dsMapLock;

    TXDataStoreBase();
};

}  // End namespace FT
}  // End namespace Cdmw

#endif
