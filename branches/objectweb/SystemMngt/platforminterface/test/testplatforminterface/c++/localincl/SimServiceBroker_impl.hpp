/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#ifndef INCL_PLATFORMSIMU_SIM_SERVICE_BROKER_IMPL_HPP
#define INCL_PLATFORMSIMU__SIM_SERVICE_BROKER_IMPL_HPP

#include "Foundation/common/System.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
//#include "naminginterface/NamingInterface.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtServiceBroker.skel.hpp"


class Simulator;

/**
*Purpose:
*<p> 
*
*Features:
*<p> Thread safe
*
*@see  
*
*/
class SimServiceBroker_impl : 
        public POA_CdmwPlatformMngtService::ServiceBroker,
        public virtual PortableServer::RefCountServantBase
{

public:
  
  /**
   * Purpose:
   * <p> Constructor
   * 
   */ 
  SimServiceBroker_impl()
    throw(CORBA::SystemException)
  {
  };
  

  /**
   * Purpose:
   * <p> Destructor
   * 
   */ 
  virtual 
  ~SimServiceBroker_impl()
    throw()
  {
    std::cout<<"delete SimServiceBroker_impl"<<std::endl;
  };
  
  
  //
  // IDL:thalesgroup.com/CdmwPlatformMngtService/ServiceBroker/get_service:1.0
  //
  virtual
  CORBA::Object_ptr
  get_service(const char* service_name)
    throw(CdmwPlatformMngtService::ServiceNotFound,
	  CdmwPlatformMngtService::ServiceUnavailable,
	  CORBA::SystemException)
    
  {
    std::string header("ServiceBroker::get_service(");
    header += service_name;
    header += ")";
    
    CORBA::Object_var obj = CORBA::Object::_nil();
    

    return obj._retn();
  };

    //
    // IDL:thalesgroup.com/CdmwPlatformMngtService/ServiceBroker/set_global_service:1.0
    //
#ifdef CDMW_USE_FAULTTOLERANCE
    virtual
    void 
    set_global_service(const char* service_name, CORBA::Object_ptr service_object)
        throw( CdmwPlatformMngtService::ServiceAlreadyExists,
               CORBA::SystemException )
    {
        std::cout<<"set_global_service"<<std::endl;        
    }
#endif

protected:
  
private:
  
  /**
   * Purpose:
   * <p>  Copy constructor
   * 
   */ 
  SimServiceBroker_impl (const SimServiceBroker_impl& rhs)
    throw();
  
  
  /**
   * Purpose:
   * <p> Assignment operator
   * 
   */ 
  SimServiceBroker_impl&
  operator=(const SimServiceBroker_impl& rhs)
    throw();
  
  /**
   * Holds a reference to ...
   */
  
  Simulator* m_simulator_descr;
  
}; // End class SimServiceBroker_impl 



#endif // INCL_PLATFORMSIMU__SIM_SERVICE_BROKER_IMPL_HPP
