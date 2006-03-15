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

#ifndef _CDMW_LB_UPDATE_COMMAND_HANDLER_HPP_
#define _CDMW_LB_UPDATE_COMMAND_HANDLER_HPP

#include "LoadBalancing/lbcommon/GroupRefCache.stub.hpp"
#include "Foundation/commonsvcs/federation/ParametricCommand.hpp"
#include "Foundation/orbsupport/TypeTraits.hpp"

#include "LoadBalancing/lbcommon/GroupRefUpdateMessage_traits.hpp"
#include "LoadBalancing/lbcommon/GroupRefCacheCommandHandler.hpp"

namespace Cdmw {
    namespace LB {
        class SetCommand;
        class RemoveCommand;
    }
}



class Cdmw::LB::SetCommand :
    public virtual Cdmw::CommonSvcs::ParametricCommand< ::CdmwLB::GroupRefUpdateMessage > 
{ 
public: 
    SetCommand(::Cdmw::LB::GroupRefCacheCommandHandler& handler); 


public: 
    virtual void execute(Cdmw::OrbSupport::type_traits< ::CdmwLB::GroupRefUpdateMessage >::_in_type data);
    
private: 
    ::Cdmw::LB::GroupRefCacheCommandHandler& m_handler; 
};


class Cdmw::LB::RemoveCommand :
    public virtual Cdmw::CommonSvcs::ParametricCommand< ::CdmwLB::GroupRefUpdateMessage > 
{ 
public: 
    RemoveCommand(::Cdmw::LB::GroupRefCacheCommandHandler& handler); 


public: 
    virtual void execute(const ::CdmwLB::GroupRefUpdateMessage& data); 
private: 
    ::Cdmw::LB::GroupRefCacheCommandHandler& m_handler; 
};

#endif /* _CDMW_LB_UPDATE_COMMAND_HANDLER_HPP */
