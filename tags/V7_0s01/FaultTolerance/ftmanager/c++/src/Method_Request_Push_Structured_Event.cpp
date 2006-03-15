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

#include "ftmanager/Method_Request_Push_Structured_Event.hpp"

#include <iostream>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

namespace Cdmw {
namespace FT {
namespace Scheduler {


    Method_Request_Push_Structured_Event::Method_Request_Push_Structured_Event (const CosNotification::StructuredEvent& event,
                                                                                CosNotifyComm::StructuredPushConsumer_ptr consumer)
        : m_event(event),
          m_consumer(CosNotifyComm::StructuredPushConsumer::_duplicate(consumer))
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    }
    

    Method_Request_Push_Structured_Event::~Method_Request_Push_Structured_Event (void)
        throw()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    }
    

    void Method_Request_Push_Structured_Event::run (void)
        throw()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            m_consumer->push_structured_event(m_event);
        } catch (const CORBA::Exception & ex) {
            CDMW_WARN(FTLogger::GetLogger(),"[FaultNotifier/Method_Request_Push_Structured_Event]=> push_structured_events()"
                      << " raised an exception : " << ex );
        }
    }
    


}; // namespace Scheduler
}; // namespace FT
}; // namespace Cdmw
