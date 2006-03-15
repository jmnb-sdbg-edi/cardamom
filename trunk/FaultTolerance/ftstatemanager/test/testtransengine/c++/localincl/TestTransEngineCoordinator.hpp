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

#ifndef INCL_CDMW_TEST_TRANS_ENGINE_COORDINATOR_HPP
#define INCL_CDMW_TEST_TRANS_ENGINE_COORDINATOR_HPP

// include files
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/commonsvcs/datastore/StorageHome.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <FaultTolerance/ftstatemanager/TXDataStore.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
// class definition


class CohortTest : public virtual POA_CdmwFT::Transaction::TPCCohort,
   public virtual PortableServer::RefCountServantBase
{
public:   

    /**
     * Purpose:
     * <p> Constructor
     */
    CohortTest();
    

    virtual ~CohortTest();
    

    void send_prepare (const CdmwFT::Transaction::TransactionIdentification & id,
                       const CdmwFT::Transaction::StateChanges& change)
        throw(CORBA::SystemException);

    void send_commit (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);
    
    void send_rollback (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);

    void send_get_transaction_status (const CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);

    void send_set_transaction_status (const char * sender,
                                 const CdmwFT::Transaction::TransactionIdentification & id,
                                 CdmwFT::Transaction::TransactionStatus status)
      throw(CORBA::SystemException);

    CORBA::Boolean prepare (const CdmwFT::Transaction::StateChanges& change,
                           CdmwFT::Transaction::TransactionIdentification & id,
                           CORBA::String_out sender)
        throw(CORBA::SystemException);

    void commit (CdmwFT::Transaction::TransactionIdentification & id,
                 CORBA::String_out sender)
        throw(CORBA::SystemException);
    

    void rollback (CdmwFT::Transaction::TransactionIdentification & id,
                   CORBA::String_out sender)
        throw(CORBA::SystemException);
    

    CdmwFT::Transaction::TransactionStatus get_transaction_status (CdmwFT::Transaction::TransactionIdentification & id,
                                                      CORBA::String_out sender)
        throw(CORBA::SystemException);
    
    
    void set_transaction_status (CdmwFT::Transaction::TransactionStatus status,
                                 char *& sender,
                                 CdmwFT::Transaction::TransactionIdentification & id)
        throw(CORBA::SystemException);

    std::string get_answer();

    void set_answer(std::string answer);

    void do_nothing();
private:
    
    std::string m_answer;
    std::string m_prev_answer;
    bool m_do_nothing;
};





class TestTransEngineCoordinator : public Cdmw::TestUtils::Testable
{

public:
  TestTransEngineCoordinator(CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA);
  
  virtual ~TestTransEngineCoordinator();

protected:
  virtual void do_tests();

private:
    CORBA::ORB_var m_orb;
    PortableServer::POA_var m_rootPOA;
};
#endif //INCL_CDMW_TEST_TRANS_ENGINE_COORDINATOR_HPP

