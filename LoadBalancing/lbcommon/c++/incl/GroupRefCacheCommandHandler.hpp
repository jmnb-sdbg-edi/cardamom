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

#ifndef _CDMW_PROTOS_GROUP_REF_CACHE_COMMAND_HANDLER_HPP_
#define _CDMW_PROTOS_GROUP_REF_CACHE_COMMAND_HANDLER_HPP_

#include "Foundation/orbsupport/CORBA.hpp"
#include <LoadBalancing/lbcommon/GroupRefCache.stub.hpp>

namespace Cdmw {
    namespace LB{
        class GroupRefCacheCommandHandler;
    }
}


class Cdmw::LB::GroupRefCacheCommandHandler
{
public:
    virtual void handle_set(CORBA::Short id, PortableGroup::ObjectGroupRefVersion object_group_ref_version, CORBA::Object_ptr iogr) = 0;
    
    virtual void handle_remove(CORBA::Short id) = 0;
};

#endif /* _CDMW_PROTOS_GROUP_REF_CACHE_COMMAND_HANDLER_HPP_ */
