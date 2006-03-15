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
#include "clockservice/ControllerUpdate_impl.hpp"
#include "clockservice/Controller_impl.hpp"

#include <Foundation/common/Assert.hpp>

namespace Cdmw { namespace clock { namespace policy {

    ////////////////////////////////////////////////////////////////////////////

    ControllerUpdate_impl::ControllerUpdate_impl()
    {
    }
    
    ////////////////////////////////////////////////////////////////////////////
    
    ControllerUpdate_impl::~ControllerUpdate_impl()
    {
    }
    
    ////////////////////////////////////////////////////////////////////////////
    
    void
    ControllerUpdate_impl::update_controller_period (Controller_impl* ctrl,
                                                     TimeBase::TimeT  period)
    {
        if(!ctrl)
            return;
        ctrl->update_period(period);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    
    void
    ControllerUpdate_impl::update_controller_offset (Controller_impl* ctrl,
                                                     TimeBase::TimeT  offset)
    {
        if(!ctrl)
            return;
        ctrl->update_offset(offset);
    }
    
    ////////////////////////////////////////////////////////////////////////////
    
    void
    ControllerUpdate_impl::freeze_controller(Controller_impl* ctrl)
    {
        if(!ctrl)
            return;
        ctrl->freezeTimerLockFree();
    }
    
    ////////////////////////////////////////////////////////////////////////////
    
    bool
    ControllerUpdate_impl::isValid(Cdmw::clock::ControllerEx_ptr controller)
    {
        CDMW_ASSERT(!CORBA::is_nil(controller));
        Controller_impl* controller_ptr =
            dynamic_cast<Controller_impl*>(controller);
        CDMW_ASSERT(controller_ptr);
        
        if(CORBA::is_nil(controller) || 
           controller_ptr == 0  ||
           (controller_ptr->executions() ==
            controller_ptr->execution_limit()))
           return false;
        //    throw CORBA::OBJECT_NOT_EXIST(Cdmw::OrbSupport::OBJECT_NOT_EXIST, 
        //                                  CORBA::COMPLETED_NO);            
        return true;
    }
    
} /* policy */ } /* clock */ } /* Cdmw */
                                                           
////////////////////////////////////////////////////////////////////////////////

    
