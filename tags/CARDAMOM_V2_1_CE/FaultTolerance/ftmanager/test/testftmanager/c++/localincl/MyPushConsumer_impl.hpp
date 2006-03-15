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


#ifndef __MY_PUSH_CONSUMER_HPP__
#define __MY_PUSH_CONSUMER_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "testftmanager/MyPushConsumer.skel.hpp"

#include <string>


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
class  MyStructuredPushConsumer_impl
    : public virtual POA_MyStructuredPushConsumer,
      public virtual PortableServer::RefCountServantBase
{

public:

    /**
     *Purpose:
     *<p> 
     */
    MyStructuredPushConsumer_impl( const char* name )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p> 
     */
    virtual ~MyStructuredPushConsumer_impl()
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CosNotifyComm/StructuredPushConsumer/push_structured_event:1.0
     * operation
     */
    void push_structured_event(
        const CosNotification::StructuredEvent& notification )
            throw( CORBA::SystemException, CosEventComm::Disconnected );
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CosNotifyComm/StructuredPushConsumer/disconnect_structured_push_consumer:1.0
     * operation
     */
    void disconnect_structured_push_consumer()
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CosNotifyComm/NotifyPublish/offer_change:1.0
     * operation
     */
    void offer_change (
        const CosNotification::EventTypeSeq & added,
        const CosNotification::EventTypeSeq & removed )
            throw( CORBA::SystemException, CosNotifyComm::InvalidEventType );

private:

    std::string m_name;
};
    


#endif // __MY_PUSH_CONSUMER_HPP__

