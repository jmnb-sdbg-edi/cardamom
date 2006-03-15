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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_TXMANAGER_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_TXMANAGER_HPP

#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include "Foundation/common/Exception.hpp"
#include <set>
#include <ace/Synch_T.h>

namespace Cdmw {
namespace FT {

class NoTransactionException: public Exception
{
  public:
    NoTransactionException(const std::string& = "NoTransactionException");
};

class InTransactionException: public Exception
{
  public:
    InTransactionException(const std::string& = "InTransactionException");
};

class TXDataStoreBase;

/**
 * Local datastore transaction manager. Controls and holds state for
 * open transactions in thread specific storage.
 */
class TXManager
{
public:

    /**
     * Contructor.
     * @param context the DataStoreContext used to apply changes upon commit.
     */
    TXManager(DataStoreContext& context);

    /**
     * Destuctor.
     */
    virtual ~TXManager();

    /**
     * Begin a transaction in the current thread. Changes to any TXDataStore
     * managed by this TXManager are not visible outside the current thread.
     */
    void begin() throw (InTransactionException);

    /**
     * Commit any changes from the current thread transaction. Ends the
     * transaction. Returns true if commit succeded.
     *
     * This operation will potentially block until updating possible replicas
     */
    bool commit() throw (NoTransactionException);

    /**
     * Rollback any changes in the current thread transaction to the state
     * of the TXDataStores managed by this TXManager at the time of the begin.
     * Ends the transaction.
     */
    void rollback() throw (NoTransactionException);

    /**
     * Test for an open transaction in the current thread.
     *
     * @return true if transaction is open else false.
     */
    bool in_transaction() const;

    /**
     * Registers a TXDataStore as being managed by this TXManager.
     * Performed automatically when a TXDataStore is modified within the scope
     * of a transaction.
     */
    void register_datastore(TXDataStoreBase* ds);

    /**
     * Returns the DataStoreContext associated with this TXManager.
     *
     * @return a DataStoreContext
     */
    DataStoreContext& get_context() const;

private:
    /**
    * Assignement and copy operators are not allowed.
    */
    TXManager& operator=(const TXManager& rhs);
    TXManager(const TXManager& rhs);


    typedef std::set<TXDataStoreBase*> DSSet;

    struct TXDataStoreInfo
    {
        int m_level;
        DSSet m_set;

        TXDataStoreInfo() : m_level(0) {}
    };

    ACE_TSS<TXDataStoreInfo> *m_tss_info;

    Cdmw::OsSupport::Mutex m_mutex;

    DataStoreContext& m_context;
};

}  // End namespace FT
}  // End namespace Cdmw

#endif
