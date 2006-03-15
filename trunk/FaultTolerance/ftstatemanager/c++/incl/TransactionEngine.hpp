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

#ifndef INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_ENGINE_HPP
#define INCL_CDMW_FT_FTSTATEMANAGER_TRANSACTION_ENGINE_HPP

#include <list>

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <FaultTolerance/idllib/CdmwFTTPCCommon.stub.hpp>
#include <FaultTolerance/ftstatemanager/Transaction.hpp>
#include <Foundation/orbsupport/TypeTraits.hpp>
#include <Foundation/orbsupport/Codec.hpp>
#include <functional>

/**
*Purpose:
* TransactionEngine template.
*<p>
*
*Features:
*<p> Thread safe. Exception safe.
*/
namespace Cdmw {
namespace FT {

template<>
bool std::less< ::CdmwFT::Transaction::TransactionIdentification>::operator() 
    (const ::CdmwFT::Transaction::TransactionIdentification& x, 
     const ::CdmwFT::Transaction::TransactionIdentification& y) const
{
    if (x.id < y.id)
    {
        return true;
    }
    else if (x.id == y.id)
    {
        return(strcmp(x.initiator, y.initiator) < 0);
    }
    else 
        return false;
};

struct DoneTransactionStruct
{
    ::CdmwFT::Transaction::TransactionIdentification id;
    CdmwFT::Transaction::TransactionStatus status;
};

typedef std::list<DoneTransactionStruct> DoneTransactionList;


class TransactionEngine
{
public:   

    /**
    * Purpose:
    * <p> Constructor with given data store
    */
    TransactionEngine(CORBA::ORB_ptr orb,
                      PortableServer::POA_ptr poa,
                      int max_transaction_in_progress,
                      int max_transaction_done )
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p> Destructor.
    */
    virtual ~TransactionEngine();

    /**
    * Purpose:
    * <p> process warning of a failure
    *
    *@param location is the failed location
    */
    virtual void on_failure(const std::string& location) = 0;

    /**
     * Purpose:
     * <p> get the transaction map (to be used when cohort becomes coordinator)
     *
     */
    DoneTransactionList get_cohort_transactions_done();

    DoneTransactionList m_doneTransactionList;

protected:

    /**
    * Purpose:
    * <p> Default constructor.
    */
    TransactionEngine() {}

    void save_aborted_trans(const CdmwFT::Transaction::TransactionIdentification & id);
    
    void save_committed_trans(const CdmwFT::Transaction::TransactionIdentification & id);

    typedef std::map<CdmwFT::Transaction::TransactionIdentification, 
                     Transaction*, 
                     std::less< ::CdmwFT::Transaction::TransactionIdentification> > TransactionMap;

    CORBA::ORB_var m_orb;

    PortableServer::POA_var m_poa;

    std::string m_sender_name;

    unsigned int m_max_transaction_in_progress;
        
    unsigned int m_max_transaction_done;
    
private:

    // 2 phase protocol time out
    int m_timeout;

    /**
    * Assignement operator is not allowed.
    */
    TransactionEngine& operator=(const TransactionEngine& rhs);

}; // End class TransactionEngine



}  // End namespace FT
}  // End namespace Cdmw

#endif
