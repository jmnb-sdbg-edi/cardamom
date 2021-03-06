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


#include <Foundation/orbsupport/CORBA.hpp>
#include "testftreplicationmanager/TestFaultNotifier.hpp"
#include "idllib/FT.stub.hpp"
#include <ossupport/OS.hpp>
#include <exception>
#include <iostream>

namespace Cdmw {
namespace FT {

// constructor
TestFaultNotifier::TestFaultNotifier( )
{
}

// destructor
TestFaultNotifier::~TestFaultNotifier()
{
}

// push the fault report to each consumer (For the test program, the consumer is the Replication Manager)
void TestFaultNotifier::push_structured_fault (const CosNotification::StructuredEvent & event)
    throw(CORBA::SystemException)
{
    std::cout<<"TestFaultNotifier::push_structured_fault"<<std::endl;
    
    m_push_consumer->push_structured_event ( event );    
}

// not implemented
void TestFaultNotifier::push_sequence_fault (const CosNotification::EventBatch & events)
    throw(CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT();
}


// store the consumer (this method is called by the Replication Manager) 
::FT::FaultNotifier::ConsumerId TestFaultNotifier::connect_structured_fault_consumer (CosNotifyComm::StructuredPushConsumer_ptr push_consumer)
    throw(CORBA::SystemException)
{
    std::cout<<"TestFaultNotifier::connect_structured_fault_consumer begin"<<std::endl;
    m_push_consumer =CosNotifyComm::StructuredPushConsumer::_duplicate (push_consumer);
    std::cout<<"TestFaultNotifier::connect_structured_fault_consumer end"<<std::endl;
    return (0);
}

// not implemented
::FT::FaultNotifier::ConsumerId TestFaultNotifier::connect_sequence_fault_consumer (CosNotifyComm::SequencePushConsumer_ptr push_consumer)
    throw(CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT();

    return (0);
}


void TestFaultNotifier::replace_constraint(
                                           ::FT::FaultNotifier::ConsumerId connection,
                                           const CosNotification::EventTypeSeq& event_types,
                                           const char* constr_expr )
    throw( CORBA::SystemException )
{
    throw CORBA::NO_IMPLEMENT();
}


// not implemented
void TestFaultNotifier::disconnect_consumer (::FT::FaultNotifier::ConsumerId connection)
    throw(CORBA::SystemException, CosEventComm::Disconnected)
{
    throw CORBA::NO_IMPLEMENT();
}

}; // namespace FT
}; // namespace Cdmw


