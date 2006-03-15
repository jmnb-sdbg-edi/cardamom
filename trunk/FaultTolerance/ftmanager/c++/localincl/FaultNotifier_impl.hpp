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


#ifndef __CDMW_FT_CDMW_FAULT_NOTIFIER_HPP__
#define __CDMW_FT_CDMW_FAULT_NOTIFIER_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "idllib/FT.skel.hpp"

#include <map>
#include "Foundation/commonsvcs/datastore/StorageHome.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"
#include "FaultTolerance/ftstatemanager/DataStore.hpp"
#include "FaultTolerance/ftstatemanager/DataStoreContext.hpp"

namespace Cdmw {
namespace FT {

/**
 *Purpose: 
 *<p> 
 *
 *Features:
 *<p> Thread safe
 *
 *@see  
 *
 */
class  FaultNotifier_impl
    : public virtual POA_FT::FaultNotifier,
      public virtual PortableServer::RefCountServantBase
{

public:

    /**
     *Purpose:
     *<p> 
     */
    FaultNotifier_impl(CORBA::ORB_ptr orb,
                       PortableServer::POA_ptr poa);

    /**
     *Purpose:
     *<p> 
     */
    virtual ~FaultNotifier_impl()
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/FT/FaultNotifier/push_structured_fault:1.0
     * operation
     */
	void push_structured_fault(
	    const CosNotification::StructuredEvent& event )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/FT/FaultNotifier/push_sequence_fault:1.0
     * operation
     */
	void push_sequence_fault(
	    const CosNotification::EventBatch& events )
            throw( CORBA::SystemException );


    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/FT/FaultNotifier/connect_structured_fault_consumer:1.0
     * operation
     */
	::FT::FaultNotifier::ConsumerId connect_structured_fault_consumer(
	    CosNotifyComm::StructuredPushConsumer_ptr push_consumer)
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/FT/FaultNotifier/connect_sequence_fault_consumer:1.0
     * operation
     */
    ::FT::FaultNotifier::ConsumerId connect_sequence_fault_consumer(
	    CosNotifyComm::SequencePushConsumer_ptr push_consumer)
            throw( CORBA::SystemException );
  
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/FT/FaultNotifier/disconnect_consumer:1.0
     * operation
     */
	void disconnect_consumer( 
	    ::FT::FaultNotifier::ConsumerId connection )
            throw( CORBA::SystemException,
                   CosEventComm::Disconnected );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/FT/FaultNotifier/replace_constraint:1.0
     * operation
     */
    void replace_constraint(
        ::FT::FaultNotifier::ConsumerId connection,
        const CosNotification::EventTypeSeq& event_types,
        const char* constr_expr )
            throw( CORBA::SystemException );


    // 
    // update the reference of the consumer
    // (it's used to update the reference of object group)
    //
    void update_consumer_id(CosNotifyComm::SequencePushConsumer_ptr push_consumer,
                                          ::FT::FaultNotifier::ConsumerId connection )
        throw(Cdmw::FT::NotFoundException);
    
    void update_consumer_id( CosNotifyComm::StructuredPushConsumer_ptr push_consumer,
                                          ::FT::FaultNotifier::ConsumerId connection )
        throw(Cdmw::FT::NotFoundException);

    // 
    // register local consumer
    //

    void register_local_consumer(CosNotifyComm::StructuredPushConsumer_ptr push_consumer)
            throw( CORBA::SystemException );

    void register_local_consumer(CosNotifyComm::SequencePushConsumer_ptr push_consumer)
            throw( CORBA::SystemException );

    /** 
     * dump the data store 
     */ 
    void dumpConsumersDataStore();


private:
    typedef Cdmw::CommonSvcs::DataStore::StorageHome< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::StructuredPushConsumer> StructuredPushConsumerStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< ::FT::FaultNotifier::ConsumerId , CosNotifyComm::StructuredPushConsumer> StructuredPushConsumerStorageObject;
    typedef Cdmw::FT::DataStore< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::StructuredPushConsumer> StructuredPushConsumerDataStore;
    
    typedef Cdmw::CommonSvcs::DataStore::StorageHome< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::SequencePushConsumer> SequencePushConsumerStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< ::FT::FaultNotifier::ConsumerId , CosNotifyComm::SequencePushConsumer> SequencePushConsumerStorageObject;
    typedef Cdmw::FT::DataStore< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::SequencePushConsumer> SequencePushConsumerDataStore;

    typedef Cdmw::CommonSvcs::DataStore::StorageHome< std::string , ::FT::FaultNotifier::ConsumerId > ConsumerIdStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< std::string , ::FT::FaultNotifier::ConsumerId > ConsumerIdStorageObject;
    typedef Cdmw::FT::DataStore< std::string , ::FT::FaultNotifier::ConsumerId > ConsumerIdDataStore;

    StructuredPushConsumerStorageHome* m_structuredPushConsumerStorageHome;

    SequencePushConsumerStorageHome* m_sequencePushConsumerStorageHome;

    ConsumerIdStorageHome* m_consumerIdStorageHome;


    CORBA::ORB_var m_orb;
    PortableServer::POA_var m_poa;

    Cdmw::OsSupport::SchedulerBase m_scheduler;

    CosNotifyComm::StructuredPushConsumer_var m_local_structure_push_consumer;
    CosNotifyComm::SequencePushConsumer_var m_local_sequence_push_consumer;
    
    
};
    

}  // End namespace FT
}  // End namespace Cdmw

#endif // __CDMW_FT_CDMW_FAULT_NOTIFIER_HPP__

