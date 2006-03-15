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


#include "platformfaultdetector/TimeOutPolicyServiceFactory.hpp"
#include "platformfaultdetector/TAOTimeOutPolicyService.hpp"
#include "platformfaultdetector/OrbacusTimeOutPolicyService.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

#if CDMW_ORB_VDR == orbacus
    TimeOutPolicyService* TimeOutPolicyServiceFactory::M_pService = new OrbacusTimeOutPolicyService();
#elif CDMW_ORB_VDR == tao
    TimeOutPolicyService* TimeOutPolicyServiceFactory::M_pService = new TAOTimeOutPolicyService();
#endif

    /**
     *Purpose:
     *<p> Constructor
     *
     *@return A TimeOutPolicyServiceFactory.
     */
    TimeOutPolicyServiceFactory::TimeOutPolicyServiceFactory()
    {
    }

    /**
     *Purpose:
     *<p> Default destructor.
     */
    TimeOutPolicyServiceFactory::~TimeOutPolicyServiceFactory()
    {
       if (M_pService != NULL) delete M_pService;
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
     TimeOutPolicyService* TimeOutPolicyServiceFactory::getService()
    {
       return M_pService;
    }

} // End namespace PlatformMngt
} // End namespace Cdmw
