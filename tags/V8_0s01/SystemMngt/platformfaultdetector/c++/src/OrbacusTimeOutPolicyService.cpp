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

#if CDMW_ORB_VDR == orbacus

#include "platformfaultdetector/OrbacusTimeOutPolicyService.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

    /**
     *Purpose:
     *<p> Constructor
     *
     *@return A Orbacus TimeOutPolicyService.
     */
    OrbacusTimeOutPolicyService::OrbacusTimeOutPolicyService()
    {
    }

    /**
     *Purpose:
     *<p> Default destructor.
     */
    OrbacusTimeOutPolicyService::~OrbacusTimeOutPolicyService()
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
    CORBA::Object_ptr OrbacusTimeOutPolicyService::SetTimeoutPolicy(
      CORBA::ORB_ptr orb,
      CORBA::Object_ptr obj,
      unsigned long timeDuration )
        throw(CORBA::SystemException)
    {
      // Create the timeout policy
      CORBA::Any timeout;
      CORBA::PolicyList policies(1);

      timeout <<= (CORBA::ULong) timeDuration;
      policies.length( 1 );
      policies[0] = orb->create_policy( OB::TIMEOUT_POLICY_ID, timeout );

      // Set the policy
      CORBA::Object_var capsule = obj->_set_policy_overrides( policies, CORBA::ADD_OVERRIDE );
      if( CORBA::is_nil( capsule ) )
          throw CORBA::INTERNAL( OrbSupport::INTERNAL, CORBA::COMPLETED_NO );

      return capsule._retn();
    }

} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //CDMW_ORB_VDR == orbacus 
