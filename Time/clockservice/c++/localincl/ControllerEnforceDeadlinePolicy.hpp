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

#ifndef CARDAMOM_CLOCK_CONTROLLER_ENFORCE_DEADLINE_POLICY_HPP
#define CARDAMOM_CLOCK_CONTROLLER_ENFORCE_DEADLINE_POLICY_HPP

#include "clockservice/ControllerUpdate_impl.hpp"
 
/**
 * @author Francesca Annunziata <fannunziata@selex-si.com>
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */
 
namespace Cdmw { namespace clock { 
	
    /**
     * @brief This policy assures that the temporal deadline
     *        of timer on virtual time, will be respected.
     */
	class  ControllerEnforceDeadlinePolicy 
    :  public virtual Cdmw::clock::policy::ControllerUpdate_impl
    {
		public:
		  ControllerEnforceDeadlinePolicy ();
  
		  virtual ~ControllerEnforceDeadlinePolicy ();
  
		  virtual
		  void on_set (::Cdmw::clock::ControllerEx_ptr controller)
		    throw (CORBA::SystemException);
		  
		  virtual
		  void on_set_rate (::Cdmw::clock::ControllerEx_ptr controller)
		    throw (CORBA::SystemException);
		  
		  virtual
		  void on_pause (::Cdmw::clock::ControllerEx_ptr controller)
		    throw (CORBA::SystemException);
		  
		  virtual
		  void on_terminate (::Cdmw::clock::ControllerEx_ptr controller)
		    throw (CORBA::SystemException);
		  
		  virtual
		  void on_resume (::Cdmw::clock::ControllerEx_ptr controller)
		    throw (CORBA::SystemException);
	};
} 
}
#endif /*CARDAMOM_CLOCK_CONTROLLER_ENFORCE_DEADLINE_POLICY_HPP*/
