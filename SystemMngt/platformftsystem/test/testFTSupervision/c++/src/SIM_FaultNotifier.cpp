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
#include "testFTSupervision/SIM_FaultNotifier.hpp"
#include "idllib/FT.stub.hpp"
#include <ossupport/OS.hpp>
#include <exception>
#include <iostream>

namespace Cdmw {
namespace FT {

// constructor
SIM_FaultNotifier::SIM_FaultNotifier( )
{
}

// destructor
SIM_FaultNotifier::~SIM_FaultNotifier()
{
}

// push the fault report to each consumer (For the test program, the consumer is the Replication Manager)
void SIM_FaultNotifier::push_structured_fault (const CosNotification::StructuredEvent & event)
    throw(CORBA::SystemException)
{
    std::cout<<"SIM_FaultNotifier::push_structured_fault"<<std::endl;
    try 
    {
        m_push_consumer->push_structured_event( event );
    }
    catch (const CORBA::SystemException& e)
    {
       std::cerr << "SIM_FaultNotifier::push_structured_fault : CORBA::SystemException raised => " << e << std::endl;
    }
    catch (const CORBA::Exception& e)
    {
       std::cerr << "SIM_FaultNotifier::push_structured_fault : CORBA::Exception raised => " << e << std::endl;
    }
    catch (...)
    {
       std::cerr << "SIM_FaultNotifier::push_structured_fault : unexpected exception raised => " << std::endl;
    }

}

// not implemented
void SIM_FaultNotifier::push_sequence_fault (const CosNotification::EventBatch & events)
    throw(CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT();
}


// store the consumer (this method is called by the Replication Manager) 
::FT::FaultNotifier::ConsumerId SIM_FaultNotifier::connect_structured_fault_consumer (CosNotifyComm::StructuredPushConsumer_ptr push_consumer)
    throw(CORBA::SystemException)
{
    std::cout<<"SIM_FaultNotifier::connect_structured_fault_consumer begin"<<std::endl;
    m_push_consumer = CosNotifyComm::StructuredPushConsumer::_duplicate(push_consumer);    
    return (0);
}

// not implemented
::FT::FaultNotifier::ConsumerId SIM_FaultNotifier::connect_sequence_fault_consumer (CosNotifyComm::SequencePushConsumer_ptr push_consumer)
    throw(CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT();

    return (0);
}


void SIM_FaultNotifier::replace_constraint(
                                           ::FT::FaultNotifier::ConsumerId connection,
                                           const CosNotification::EventTypeSeq& event_types,
                                           const char* constr_expr )
    throw( CORBA::SystemException )
{
    throw CORBA::NO_IMPLEMENT();
}


// not implemented
void SIM_FaultNotifier::disconnect_consumer (::FT::FaultNotifier::ConsumerId connection)
    throw(CORBA::SystemException, CosEventComm::Disconnected)
{
    throw CORBA::NO_IMPLEMENT();
}

}; // namespace FT
}; // namespace Cdmw

