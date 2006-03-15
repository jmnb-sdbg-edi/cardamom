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

#ifndef _CDMW_TIME_FEDERATED_CONTROLLED_CLOCK_HPP_
#define _CDMW_TIME_FEDERATED_CONTROLLED_CLOCK_HPP_

#include <clockservice/ControlledExecutor_impl.hpp>
#include <clockservice/FederatedControlledClock_impl.hpp>
#include <clockservice/FederatedControlledExecutor_impl.hpp>

namespace Cdmw {
    namespace clock {
        /**
         *  @brief  This typedef insert a FederatedControlledClock_t in EVoT.
         */
        typedef ::Cdmw::clock::FederatedControlledClock_impl< ::Cdmw::clock::ControlledClock_impl,
                                                              ::Cdmw::OsSupport::Mutex >
        FederatedControlledClock_t;
        
        /**
         *  @brief  This typedef insert a FederatedControlledExecutor_t in EVoT.
         */        
        typedef ::Cdmw::clock::FederatedControlledExecutor_impl< ACE_Recursive_Thread_Mutex,
                                                                 ACE_Guard >
        FederatedControlledExecutor_t;

    }
}

#endif /* _CDMW_TIME_FEDERATED_CONTROLLED_CLOCK_HPP_ */
