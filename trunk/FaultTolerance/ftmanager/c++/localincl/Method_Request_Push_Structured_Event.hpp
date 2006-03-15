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

#ifndef _CDMW_FT_METHOD_REQUEST_PUSH_STRUCTURED_EVENT_HPP_
#define _CDMW_FT_METHOD_REQUEST_PUSH_STRUCTURED_EVENT_HPP_

#include <Foundation/osthreads/Runnable.hpp>
#include <FaultTolerance/idllib/FT.stub.hpp>

namespace Cdmw {
namespace FT {
namespace Scheduler {



class Method_Request_Push_Structured_Event : public Cdmw::OsSupport::Runnable
{
public:
    Method_Request_Push_Structured_Event (const CosNotification::StructuredEvent& event,
                                          CosNotifyComm::StructuredPushConsumer_ptr consumer);

    ~Method_Request_Push_Structured_Event (void)
        throw();

    void run (void)
        throw();

private:
    const CosNotification::StructuredEvent m_event;
    CosNotifyComm::StructuredPushConsumer_var m_consumer;
};




}; // namespace Scheduler
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_METHOD_REQUEST_PUSH_STRUCTURED_EVENT_HPP_
