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

#include "idllib/FT.skel.hpp"
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <map>

class SIM_consumer : virtual public POA_CosNotifyComm::StructuredPushConsumer,
                     public virtual Cdmw::OrbSupport::RefCountServantBase
{
public:
    SIM_consumer();
    
    ~SIM_consumer();        
    
    virtual void push_structured_event(const CosNotification::StructuredEvent & notification)
        throw (CosEventComm::Disconnected,
               CORBA::SystemException);
    
    void offer_change (const CosNotification::EventTypeSeq & added,
                       const CosNotification::EventTypeSeq & removed)
        throw (CosNotifyComm::InvalidEventType,
               CORBA::SystemException)
    {}
    
    void disconnect_structured_push_consumer()
        throw (CORBA::SystemException)
    {}
    
    bool new_event_not_responding();
    
    bool new_event_responding();
private:
    typedef std::map<std::string, bool> HostStatus;
    HostStatus m_hostStatus;
    bool m_a_new_event_not_responding_is_arrived;        
    bool m_a_new_event_responding_is_arrived;
    
};
