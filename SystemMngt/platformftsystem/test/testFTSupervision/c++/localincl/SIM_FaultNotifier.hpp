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

#ifndef INCL_SIM_FAULT_NOTIFIER_HPP
#define INCL_SIM_FAULT_NOTIFIER_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/testutils/TestManager.hpp>
#include "FaultTolerance/idllib/FT.skel.hpp"
#include <Foundation/orbsupport/RefCountServantBase.hpp>

namespace Cdmw {
namespace FT {

class SIM_FaultNotifier : virtual public POA_FT::FaultNotifier,
                          public virtual Cdmw::OrbSupport::RefCountServantBase
{
public:
    // constructor
    SIM_FaultNotifier();

    // destructor
    virtual ~SIM_FaultNotifier();
    
    // push_strunctured_fault (push the fault report to each consumer)
    virtual void push_structured_fault (const CosNotification::StructuredEvent & event)
        throw(CORBA::SystemException);
    
    // push_sequence_fault (not implemented)
    virtual void push_sequence_fault (const CosNotification::EventBatch & events)
        throw(CORBA::SystemException);
    
    // connect_structured_fault_consumer (register a new consumer)
    virtual ::FT::FaultNotifier::ConsumerId connect_structured_fault_consumer (CosNotifyComm::StructuredPushConsumer_ptr push_consumer)
        throw(CORBA::SystemException);    
    
    // connect_sequence_fault_consumer (not implemented)
    virtual ::FT::FaultNotifier::ConsumerId connect_sequence_fault_consumer (CosNotifyComm::SequencePushConsumer_ptr push_consumer)
        throw(CORBA::SystemException);
    
    // disconnect_consumer (not implemented)
    virtual void disconnect_consumer (::FT::FaultNotifier::ConsumerId connection)
        throw(CORBA::SystemException, CosEventComm::Disconnected);

    void replace_constraint(
        ::FT::FaultNotifier::ConsumerId connection,
        const CosNotification::EventTypeSeq& event_types,
        const char* constr_expr )
            throw( CORBA::SystemException );
    
protected:
    // consummer of the fault report (only one for the test)
    CosNotifyComm::StructuredPushConsumer_var m_push_consumer;
    
private:
    CORBA::ORB_var m_orb;
    PortableServer::POA_var m_rootPOA;
    
};
}; // namespace FT
}; // namespace Cdmw

#endif //INCL_TEST_FAULT_NOTIFIER_HPP

