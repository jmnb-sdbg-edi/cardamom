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


#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "ftmanager/FaultNotifier_impl.hpp"
#include "ftmanager/Method_Request_Push_Structured_Event.hpp"
#include "ftmanager/Method_Request_Push_Sequence_Event.hpp"

#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>


namespace 
{


    const int NB_SCHEDULER_THREADS = 3;

} // end anonymous namespace




namespace Cdmw {
namespace FT {

class SequencePushConsumerPrinter
{
public:
    SequencePushConsumerPrinter(CORBA::ORB_ptr orb, std::ostream & os) 
        : m_orb(CORBA::ORB::_duplicate(orb)), m_os(os) { }     
    void operator()(const std::pair< Cdmw::OrbSupport::type_traits< ::FT::FaultNotifier::ConsumerId >::_var_type, 
                                     Cdmw::OrbSupport::type_traits< CosNotifyComm::SequencePushConsumer >::_var_type > & x)
    {
        m_os << "ConsumerID : "<<  x.first << "\n"
             <<"\t : "<<m_orb->object_to_string(x.second.in()) << "\n";
    }
private:
    CORBA::ORB_var m_orb;
    std::ostream & m_os;
};

class StructuredPushConsumerPrinter
{
public:
    StructuredPushConsumerPrinter(CORBA::ORB_ptr orb, std::ostream & os)
        : m_orb(CORBA::ORB::_duplicate(orb)), m_os(os) { }     
    void operator()(const std::pair< Cdmw::OrbSupport::type_traits< ::FT::FaultNotifier::ConsumerId >::_var_type, 
                                     Cdmw::OrbSupport::type_traits< CosNotifyComm::StructuredPushConsumer >::_var_type > & x)
    {
        m_os << "ConsumerID : "<<  x.first << "\n"
             <<"\t : "<<m_orb->object_to_string(x.second.in()) << "\n";
    }
private:
    CORBA::ORB_var m_orb;
    std::ostream & m_os;
};



// unary function to realise push_structured_event on all the consumer
class PushStructuredEvent :  public std::unary_function<std::pair< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::StructuredPushConsumer_var > , void>
{
public:
    PushStructuredEvent(const CosNotification::StructuredEvent& event, Cdmw::OsSupport::SchedulerBase& scheduler )
        : m_event(event),
          m_scheduler(scheduler)
    {}
    void operator()(const std::pair< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::StructuredPushConsumer_var >&elt)
    {
        try{
            CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"push_structured_event to the consumer "<<elt.first);
            m_scheduler.enqueue(new Scheduler::Method_Request_Push_Structured_Event(m_event, elt.second.in()));
        }
        catch(...)
        {
        }
        
    }
    

private:
    const CosNotification::StructuredEvent& m_event;
    Cdmw::OsSupport::SchedulerBase& m_scheduler;
};
    

// unary function to realise push_structured_events on all the consumer
class PushSequenceEvent :  public std::unary_function<std::pair< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::SequencePushConsumer_var > , void>
{
public:
    PushSequenceEvent(const CosNotification::EventBatch& events, Cdmw::OsSupport::SchedulerBase& scheduler )
        : m_events(events),
          m_scheduler(scheduler)
    {}
    void operator()(const std::pair< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::SequencePushConsumer_var >&elt)
    {
        try{
            CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"push_structured_events to the consumer "<<elt.first);        
            m_scheduler.enqueue(new Scheduler::Method_Request_Push_Sequence_Event(m_events, elt.second.in()));
        }
        catch(...)
        {
        }
        
    }
    

private:
    const CosNotification::EventBatch& m_events; 
    Cdmw::OsSupport::SchedulerBase& m_scheduler;
};

//function for the find_if to recover all the consumer
bool SequencePushConsumerTrueFunction(const std::pair< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::SequencePushConsumer_var >&elt)
{
    return true;    
}
//function for the find_if to recover all the consumer
bool StructuredPushConsumerTrueFunction(const std::pair< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::StructuredPushConsumer_var >&elt)
{
    return true;    
}


FaultNotifier_impl::FaultNotifier_impl(CORBA::ORB_ptr orb,
                                       PortableServer::POA_ptr poa)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_scheduler(NB_SCHEDULER_THREADS)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_DEBUG(FTLogger::GetLogger(),"create data stores and storage homes"); 


    m_structuredPushConsumerStorageHome
    = new StructuredPushConsumerStorageHome(STRUCTURED_PUSH_CONSUMER_DATA_STORE);
    
    m_sequencePushConsumerStorageHome
    = new SequencePushConsumerStorageHome(SEQUENCE_PUSH_CONSUMER_DATA_STORE);

    m_consumerIdStorageHome
	= new ConsumerIdStorageHome(CONSUMER_ID_DATA_STORE);

    m_scheduler.start();

    m_local_structure_push_consumer = CosNotifyComm::StructuredPushConsumer::_nil();
    m_local_sequence_push_consumer = CosNotifyComm::SequencePushConsumer::_nil();

}

FaultNotifier_impl::~FaultNotifier_impl()
throw( CORBA::SystemException )
 {
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
	// create a list to get all the consumer 
    std::list<SequencePushConsumerStorageObject> resultSequencePushConsumer;

	// realise a find if to get all the consumer
    m_sequencePushConsumerStorageHome->find_if(SequencePushConsumerTrueFunction,
                                               std::back_inserter(resultSequencePushConsumer));
      
    // for each consumer, remove the data
    std::list<SequencePushConsumerStorageObject>::iterator iterator1;
    for (iterator1 = resultSequencePushConsumer.begin();
         iterator1 !=resultSequencePushConsumer.end();
         iterator1++)
        {
            (*iterator1).remove();          
        }
    
 	// create a list to get all the consumer 
    std::list<StructuredPushConsumerStorageObject> resultStructuredPushConsumer;
	// realise a find if to get all the consumer
    m_structuredPushConsumerStorageHome->find_if(StructuredPushConsumerTrueFunction,
                                                 std::back_inserter(resultStructuredPushConsumer));
    
    // for each consumer, remove the data
    std::list<StructuredPushConsumerStorageObject>::iterator iterator2;
    for (iterator2 = resultStructuredPushConsumer.begin();
         iterator2 !=resultStructuredPushConsumer.end();
         iterator2++)
        {
            (*iterator2).remove();          
        }
    
 }

//
// IDL:thalesgroup.com/FT/FaultNotifier/push_structured_fault:1.0
//
void FaultNotifier_impl::push_structured_fault(
    const CosNotification::StructuredEvent& event )
throw( CORBA::SystemException )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (!CORBA::is_nil(m_local_structure_push_consumer.in()))
        m_scheduler.enqueue(new Scheduler::Method_Request_Push_Structured_Event(event, m_local_structure_push_consumer.in()));
    m_structuredPushConsumerStorageHome->for_each(PushStructuredEvent(event, m_scheduler));
}

//
// IDL:thalesgroup.com/FT/FaultNotifier/push_sequence_fault:1.0
//
void FaultNotifier_impl::push_sequence_fault(
    const CosNotification::EventBatch& events )
throw( CORBA::SystemException )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if (!CORBA::is_nil(m_local_sequence_push_consumer.in()))
        m_scheduler.enqueue(new Scheduler::Method_Request_Push_Sequence_Event(events, m_local_sequence_push_consumer.in()));
    m_sequencePushConsumerStorageHome->for_each(PushSequenceEvent(events, m_scheduler));
}

//
// IDL:thalesgroup.com/FT/FaultNotifier/connect_structured_fault_consumer:1.0
//
::FT::FaultNotifier::ConsumerId FaultNotifier_impl::connect_structured_fault_consumer(
    CosNotifyComm::StructuredPushConsumer_ptr push_consumer)
throw( CORBA::SystemException )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    if( CORBA::is_nil( push_consumer ) ) {
        CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMNilObjectReference,
                            CORBA::COMPLETED_NO );
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
	
	::FT::FaultNotifier::ConsumerId consumerId = 0;
	bool consumerId_found = false;

    CDMW_DEBUG(FTLogger::GetLogger(),"connect_structured_fault_consumer");  
	try
    {
        ConsumerIdStorageObject obj = m_consumerIdStorageHome->find_by_oid("ConsumerId");
		consumerId = obj.get();
		consumerId_found = true;
    }
    catch (const Cdmw::FT::NotFoundException&)
    {
        // if no consumerId found, start the count
		
    }

    m_structuredPushConsumerStorageHome->create(++consumerId, CosNotifyComm::StructuredPushConsumer::_duplicate( push_consumer ));

	// if the consumerId found , update it , else create it
	if (consumerId_found)
	{
		ConsumerIdStorageObject obj = m_consumerIdStorageHome->find_by_oid("ConsumerId");
		obj.set(consumerId);
	}
    else
	{
		m_consumerIdStorageHome->create("ConsumerId", consumerId);
	}

    return consumerId;
}

//
// IDL:thalesgroup.com/FT/FaultNotifier/connect_sequence_fault_consumer:1.0
//
::FT::FaultNotifier::ConsumerId FaultNotifier_impl::connect_sequence_fault_consumer(
    CosNotifyComm::SequencePushConsumer_ptr push_consumer)
throw( CORBA::SystemException )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");

    if( CORBA::is_nil( push_consumer ) ) {
        CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMNilObjectReference,
                            CORBA::COMPLETED_NO );
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    ::FT::FaultNotifier::ConsumerId consumerId = 0;
    bool consumerId_found = false;
     
    CDMW_DEBUG(FTLogger::GetLogger(),"connect_sequence_fault_consumer");  
	try
    {
        ConsumerIdStorageObject obj = m_consumerIdStorageHome->find_by_oid("ConsumerId");
		consumerId = obj.get();
		consumerId_found = true;
    }
    catch (const Cdmw::FT::NotFoundException&)
    {
        // if no consumerId found, start the count
		
    }

    m_sequencePushConsumerStorageHome->create(++consumerId,CosNotifyComm::SequencePushConsumer::_duplicate( push_consumer ) );

 	// if the consumerId found , update it , else create it
	if (consumerId_found)
	{
		ConsumerIdStorageObject obj = m_consumerIdStorageHome->find_by_oid("ConsumerId");
		obj.set(consumerId);
	}
    else
	{
		m_consumerIdStorageHome->create("ConsumerId", consumerId);
	}

    CDMW_LOG_FUNCTION_RETURN(consumerId);
    return consumerId;   
}

//
// IDL:thalesgroup.com/FT/FaultNotifier/disconnect_consumer:1.0
//
void FaultNotifier_impl::disconnect_consumer( 
    ::FT::FaultNotifier::ConsumerId connection )
throw( CORBA::SystemException, CosEventComm::Disconnected )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    bool SequencePushConsumerFind = true;
    bool StructuredPushConsumerFind = true;
    
    CDMW_DEBUG(FTLogger::GetLogger(),"disconnect_consumer :" << connection);    
    if (!m_sequencePushConsumerStorageHome->contains(connection))
    {
        SequencePushConsumerFind=false;        
    }

    if (!m_structuredPushConsumerStorageHome->contains(connection))
    {
        StructuredPushConsumerFind=false;        
    }


    if ((!SequencePushConsumerFind) && (!StructuredPushConsumerFind))
        throw CosEventComm::Disconnected();

    if (SequencePushConsumerFind)
        m_sequencePushConsumerStorageHome->remove(connection);    
    
    if (StructuredPushConsumerFind)
        m_structuredPushConsumerStorageHome->remove(connection);    
}

//
// IDL:thalesgroup.com/FT/FaultNotifier/replace_constraint:1.0
//
void FaultNotifier_impl::replace_constraint(
    ::FT::FaultNotifier::ConsumerId connection,
    const CosNotification::EventTypeSeq& event_types,
    const char* constr_expr )
throw( CORBA::SystemException )
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CORBA::NO_IMPLEMENT ex(Cdmw::OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
    CDMW_LOG_FUNCTION_EXCEPTION(ex);
    throw ex;
}

void FaultNotifier_impl::update_consumer_id(CosNotifyComm::SequencePushConsumer_ptr push_consumer,
                                            ::FT::FaultNotifier::ConsumerId connection )
    throw(Cdmw::FT::NotFoundException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_DEBUG(FTLogger::GetLogger(),"update consumer id"<<connection);
    SequencePushConsumerStorageObject obj = m_sequencePushConsumerStorageHome->find_by_oid(connection);
    CosNotifyComm::SequencePushConsumer_var consumer = CosNotifyComm::SequencePushConsumer::_duplicate( push_consumer );
    obj.set(consumer.in());
    CDMW_DEBUG(FTLogger::GetLogger(),"end update consumer id");
}

    
void FaultNotifier_impl::update_consumer_id( CosNotifyComm::StructuredPushConsumer_ptr push_consumer,
                                             ::FT::FaultNotifier::ConsumerId connection )
    throw(Cdmw::FT::NotFoundException)
{        
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_DEBUG(FTLogger::GetLogger(),"update consumer id"<<connection);
    StructuredPushConsumerStorageObject obj = m_structuredPushConsumerStorageHome->find_by_oid(connection);
    CosNotifyComm::StructuredPushConsumer_var consumer = CosNotifyComm::StructuredPushConsumer::_duplicate( push_consumer );
    obj.set(consumer.in());
    CDMW_DEBUG(FTLogger::GetLogger(),"end update consumer id");
}


void FaultNotifier_impl::register_local_consumer(CosNotifyComm::StructuredPushConsumer_ptr push_consumer)
    throw( CORBA::SystemException )
{
    m_local_structure_push_consumer = CosNotifyComm::StructuredPushConsumer::_duplicate(push_consumer);
}


void FaultNotifier_impl::register_local_consumer(CosNotifyComm::SequencePushConsumer_ptr push_consumer)
    throw( CORBA::SystemException )
{
    m_local_sequence_push_consumer = CosNotifyComm::SequencePushConsumer::_duplicate(push_consumer);
}




/** 
 * dump the data store 
 */ 
void  FaultNotifier_impl::dumpConsumersDataStore()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    std::ostringstream oss;
    oss << "Dump StructuredPushConsumer datastore\n";
    m_structuredPushConsumerStorageHome->for_each(Cdmw::FT::StructuredPushConsumerPrinter(m_orb.in(), oss));
    oss << "\nDump SequencePushConsumer datastore\n";
    m_sequencePushConsumerStorageHome->for_each(Cdmw::FT::SequencePushConsumerPrinter(m_orb.in(), oss));
    oss << std::endl;
    CDMW_INFO(FTLogger::GetLogger(), oss.str());
}


}  // End namespace FT
}  // End namespace Cdmw

