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
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/common/Assert.hpp>

namespace Cdmw {
namespace FT {

NoTransactionException::NoTransactionException(const std::string& exceptionInfo)
    : Exception( Exception::SAFE, exceptionInfo )
{}

InTransactionException::InTransactionException(const std::string& exceptionInfo)
    : Exception( Exception::SAFE, exceptionInfo )
{}

TXManager::TXManager(DataStoreContext& context)
    : m_context(context)
{
    m_tss_info = new ACE_TSS<TXDataStoreInfo>;
}

TXManager::~TXManager()
{
}

void TXManager::begin() throw (InTransactionException)
{
    if (in_transaction())
    {
        throw InTransactionException();
    }

    (*m_tss_info)->m_level++;
}

bool TXManager::commit() throw (NoTransactionException)
{
    bool doLocal = true;

    CdmwFT::Transaction::StateChanges changes;
    int i = 0;

    if (!in_transaction())
    {
        throw NoTransactionException();
    }

    CDMW_ASSERT(m_context.is_activated());
    if (!m_context.is_activated())
        return false; // Do nothing if we are in backup !

    if (!(*m_tss_info)->m_set.empty())
    {
        DSSet ds_for_remote_update;

       for(DSSet::iterator iter = (*m_tss_info)->m_set.begin();
            iter != (*m_tss_info)->m_set.end();
            ++iter)
        {
            if (!m_context.get_locations((*iter)->get_dsid()).empty())
            {
                ds_for_remote_update.insert(*iter);
            }
        }

        if (!ds_for_remote_update.empty())
        {
            changes.length(ds_for_remote_update.size());

            for(DSSet::iterator iter = ds_for_remote_update.begin();
                iter != ds_for_remote_update.end();
                ++iter)
            {
                if (!m_context.get_locations((*iter)->get_dsid()).empty())
                {
                   (*iter)->get_change(changes[i++]);
                }
            }

            // apply changes
            CDMW_MUTEX_GUARD(m_mutex);

            try
            {
                doLocal = m_context.request(changes);
            }
            catch(ValidationFailureException)
            {
                std::cout<<"ValidationFailureException"<<std::endl;
                throw NoTransactionException();
            }
            
        }

        if (doLocal)
        {
            for(DSSet::iterator iter = (*m_tss_info)->m_set.begin();
                iter != (*m_tss_info)->m_set.end();
                ++iter)
            {
                (*iter)->flush();
            }
        }
    }

    (*m_tss_info)->m_level--;

    return doLocal;
}

void TXManager::rollback() throw (NoTransactionException)
{
    if (!in_transaction())
    {
        throw NoTransactionException();
    }

    for(DSSet::iterator iter = (*m_tss_info)->m_set.begin();
        iter != (*m_tss_info)->m_set.end();
        ++iter)
    {
        (*iter)->clear_cache();
    }

    (*m_tss_info)->m_set.clear();
    (*m_tss_info)->m_level--;
}

bool TXManager::in_transaction() const
{
    return (*m_tss_info)->m_level > 0;
}

void TXManager::register_datastore(TXDataStoreBase* ds)
{
    (*m_tss_info)->m_set.insert(ds);
}

DataStoreContext& TXManager::get_context() const
{
    return m_context;
}

}  // End namespace FT
}  // End namespace Cdmw
