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

#if CDMW_ORB_VDR == tao

#include "platformfaultdetector/TAOTimeOutPolicyService.hpp"
//#include "idllib/FT.stub.hpp"
#include <tao/corba.h>
#include <tao/TimeBaseC.h>
#include <tao/Messaging/Messaging.h>
 
namespace Cdmw
{
namespace PlatformMngt
{

    /**
     *Purpose:
     *<p> Constructor
     *
     *@return A TAO TimeOutPolicyService.
     */
    TAOTimeOutPolicyService::TAOTimeOutPolicyService()
    {
    }

    /**
     *Purpose:
     *<p> Default destructor.
     */
    TAOTimeOutPolicyService::~TAOTimeOutPolicyService()
    {
    }


    /**
     *Purpose:
     *<p> Concrete method for setting a timeout policy to a corba object.
     *
     *@param orb The orb.
     *@param obj The corba object that will support a timeout policy.
     *@param timeDuration The time expiration.
     *
     *@return a corba object endowed with timeout policy.
     */
    CORBA::Object_ptr TAOTimeOutPolicyService::SetTimeoutPolicy(
      CORBA::ORB_ptr orb,
      CORBA::Object_ptr obj,
      unsigned long timeDuration )
        throw(CORBA::SystemException)
    {
      // Timeout success
      TimeBase::TimeT _timeout = ((unsigned long) (timeDuration * 1.0e4));
      CORBA::Any timeout;
      CORBA::PolicyList policy_list;

      timeout <<= _timeout;

      policy_list.length( 1 );
      policy_list[0] = orb->create_policy(
          Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE,
          timeout );

      // Timed call to server
      CORBA::Object_var capsule
          = obj->_set_policy_overrides(
            policy_list,
            CORBA::SET_OVERRIDE );

      return capsule._retn();
    }

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //CDMW_ORB_VDR == tao
