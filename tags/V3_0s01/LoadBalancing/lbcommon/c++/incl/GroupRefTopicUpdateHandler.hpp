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

#ifndef _CDMW_LB_GROUP_REF_TOPIC_UPDATE_HANDLER_HPP_
#define _CDMW_LB_GROUP_REF_TOPIC_UPDATE_HANDLER_HPP_

#include <Foundation/commonsvcs/federation/CommandBasedTopicUpdateHandler.hpp>
#include "LoadBalancing/lbcommon/GroupRefCache.stub.hpp"
#include "LoadBalancing/lbcommon/UpdateMessage2CommandID.hpp"


namespace Cdmw {
    namespace LB {
        typedef Cdmw::CommonSvcs::CommandBasedTopicUpdateHandler<CdmwLB::GroupRefUpdateMessage,
                                                                 Cdmw::LB::UpdateMessage2CommandID>
        GroupRefTopicUpdateHandler_t; 
    }
}

#endif /* _CDMW_LB_GROUP_REF_TOPIC_UPDATE_HANDLER_HPP_ */
