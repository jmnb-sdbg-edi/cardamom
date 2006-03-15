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

#ifndef CARDAMOM_CLOCK_POLICY_CONTROLLER_UPDATE_ADMIN_IMPL_HPP
#define CARDAMOM_CLOCK_POLICY_CONTROLLER_UPDATE_ADMIN_IMPL_HPP

#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/orbsupport/RefCountServantBase.hpp"
#include <Time/clockservice/CdmwTime.skel.hpp>
#include <map>
 
/**
 * @author Francesca Annunziata <fannunziata@selex-si.com>
 * @author Fabrizio Morciano <fmorciano@selex-si.com>
 */
 
namespace Cdmw { namespace clock { namespace policy {
	
    /**
     * @brief This class is the implementation of Policy Administrator class.
     */
	class  ControllerUpdateAdmin_impl 
	:    public virtual POA_Cdmw::clock::policy::ControllerUpdateAdmin
    ,    public virtual Cdmw::OrbSupport::RefCountServantBase
	{
            /**
             * @brief for a singleton class, constructors and destructor are private.
             */
            ControllerUpdateAdmin_impl();
            ~ControllerUpdateAdmin_impl();

	public:
    	 	  
      /**
       * @brief Register the policy with an identifyer.
       * @param id The identifyer.
       * @param policy The policy to register.
       * @exception AlreadyExist is raised if the policy is already present.
       */     
	  void _cxx_register (
	      ::Cdmw::clock::policy::ControllerUpdateID id,
	      ::Cdmw::clock::policy::ControllerUpdate_ptr policy
	    )
	    throw (CORBA::SystemException,      
	           ::Cdmw::clock::policy::ControllerUpdateAdmin::AlreadyExist
	    );
	  
      /**
       * @brief Remove the policy with an identifyer.
       * @param id The identifyer.
       * @exception Unavailable the policy is not in list of available policy.
       * @exception OperationNotAllow you are trying to remove a system policy.
       */     
	  void unregister (
	      ::Cdmw::clock::policy::ControllerUpdateID id
	    )
	    throw (CORBA::SystemException,
	           ::Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable,
               ::Cdmw::clock::policy::ControllerUpdateAdmin::OperationNotAllow);
	  
      /**
       * @brief Retrieve the policy from an identifyer.
       * @param id The identifyer.
       * @exception Unavailable the policy is not in list of available policy.
       */           
	  ::Cdmw::clock::policy::ControllerUpdate_ptr get_policy (
	      ::Cdmw::clock::policy::ControllerUpdateID id
	    )
	    throw (CORBA::SystemException,
               ::Cdmw::clock::policy::ControllerUpdateAdmin::Unavailable);
                              
      static  ControllerUpdateAdmin_impl*
      instance();    

        private:
   	       
    	typedef std::map<Cdmw::clock::policy::ControllerUpdateID,
	                    Cdmw::clock::policy::ControllerUpdate_ptr> dbType;
            typedef dbType::const_iterator const_iterator;
    	typedef dbType::iterator iterator;
	
            static ::Cdmw::clock::policy::ControllerUpdateAdmin_impl* m_instance;
            static ::Cdmw::OsSupport::Mutex                           m_static_mutex;

            ::Cdmw::OsSupport::Mutex                                  m_mutex;
    	dbType m_dbPolicy;
	};	
}
}
}

#endif /*CARDAMOM_CLOCK_POLICY_CONTROLLER_UPDATE_ADMIN_IMPL_HPP*/
