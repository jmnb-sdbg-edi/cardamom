/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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
#ifndef _CARDAMOM_EVOT_CLOCKSVC_CLOCK_SERVICE_WORKER_H
#define _CARDAMOM_EVOT_CLOCKSVC_CLOCK_SERVICE_WORKER_H

#include "Foundation/orbsupport/CORBA.hpp"
#include "ace/Thread.h"
#include "ace/Thread_Manager.h"
#include "ace/TP_Reactor.h"
#include "ace/Select_Reactor.h"
#include "ace/Get_Opt.h"
#include "ace/Task.h"

namespace Cdmw {
    namespace clock {
        namespace svcs {
            class ORBWorker;
            class ReactorWorker;
        }
    }
}

class Cdmw::clock::svcs::ORBWorker : public ACE_Task_Base
{
public:
    ORBWorker (CORBA::ORB_ptr orb);
    virtual ~ORBWorker();
    
public:
    virtual int svc (void);

private:
    CORBA::ORB_var orb_;
};


class Cdmw::clock::svcs::ReactorWorker : public ACE_Task_Base
{
public:
    virtual ~ReactorWorker();
    
public:
    virtual int svc (void);
};


#endif /* _CARDAMOM_EVOT_CLOCKSVC_CLOCK_SERVICE_WORKER_H */
