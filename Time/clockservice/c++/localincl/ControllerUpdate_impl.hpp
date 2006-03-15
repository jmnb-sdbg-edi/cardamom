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

#ifndef CARDAMOM_CLOCK_POLICY_CONTROLLER_UPDATE_IMPL_HPP
#define CARDAMOM_CLOCK_POLICY_CONTROLLER_UPDATE_IMPL_HPP

#include "Foundation/orbsupport/RefCountServantBase.hpp"
#include <Time/clockservice/CdmwTime.skel.hpp>
 
/**
 * @author Francesca Annunziata <fannunziata@selex-si.com>
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */

namespace Cdmw { namespace clock {
    class Controller_impl;
} /* clock */ } /* Cdmw */
 
namespace Cdmw { namespace clock { namespace policy {
	
/**
 *  @brief This interface is used when an user needs to write
 *          its Controller policy.
 */
class  ControllerUpdate_impl 
	:  public virtual POA_Cdmw::clock::policy::ControllerUpdate
	,  public virtual Cdmw::OrbSupport::RefCountServantBase
{
public:
    ControllerUpdate_impl (void);
  
    virtual ~ControllerUpdate_impl (void);
  
    /**
     *  @brief  Called when a set method is called in ControlledClock.
     *  @param  controller The controller that needs to be updated.
     */
    virtual void 
    on_set (Cdmw::clock::ControllerEx_ptr controller)
	   throw (CORBA::SystemException) = 0;
		  
    /**
     *  @brief  Called when a set_rate method is called in ControlledClock.
     *  @param  controller The controller that needs to be updated.
     */
    virtual void 
    on_set_rate (Cdmw::clock::ControllerEx_ptr controller)
	   throw (CORBA::SystemException) = 0;
		  
    /**
     *  @brief  Called when a pause method is called in ControlledClock.
     *  @param  controller The controller that needs to be updated.
     */
    virtual void 
    on_pause (Cdmw::clock::ControllerEx_ptr controller)
	   throw (CORBA::SystemException) = 0;
		  
    /**
     *  @brief  Called when a terminate method is called in ControlledClock.
     *  @param  controller The controller that needs to be updated.
     */
    virtual void 
    on_terminate (Cdmw::clock::ControllerEx_ptr controller)
	   throw (CORBA::SystemException) = 0;
		  
    /**
     *  @brief  Called when a resume method is called in ControlledClock.
     *  @param  controller The controller that needs to be updated.
     */
    virtual void 
    on_resume (Cdmw::clock::ControllerEx_ptr controller)
        throw (CORBA::SystemException) = 0;
		    
private:
    ControllerUpdate_impl(ControllerUpdate_impl&);
    ControllerUpdate_impl& operator=(const ControllerUpdate_impl&);

protected:
    void 
    update_controller_period(Controller_impl* ctrl,
                             TimeBase::TimeT  period);
    
    void 
    update_controller_offset(Controller_impl* ctrl,
                             TimeBase::TimeT  offset);

    void 
    freeze_controller(Controller_impl* ctrl);

    bool
    isValid(Cdmw::clock::ControllerEx_ptr controller);
};
} /* policy */ } /* clock */ } /* Cdmw */

#endif /*CARDAMOM_CLOCK_POLICY_CONTROLLER_UPDATE_IMPL_HPP*/
