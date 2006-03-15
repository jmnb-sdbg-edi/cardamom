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

#ifndef INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_SIMPLEMULTICASTFACTORIES_HPP
#define INCL_CDMWFT_MONITORINGLIBRARY_SIMPLEMONITORING_SIMPLEMULTICASTFACTORIES_HPP

#include "SimpleMulticastPushMonitorable_impl.hpp"
#include "SimpleMulticastPullMonitorable_impl.hpp"

// class definition

#ifdef EXT_RELIABLE_MULTICAST       
class MulticastPushMonitorable_RMEOTemporalMulticastServerFactory;
class MulticastPullMonitorable_RMEOTemporalMulticastServerFactory;
class MulticastPushMonitorable_RMEOTemporalMulticastClientFactory;
class MulticastPullMonitorable_RMEOTemporalMulticastClientFactory;
#endif
#ifdef EXT_UNRELIABLE_MULTICAST
class MulticastPushMonitorable_UnreliableMulticastServerFactory;
class MulticastPullMonitorable_UnreliableMulticastServerFactory;
class MulticastPushMonitorable_UnreliableMulticastClientFactory;
class MulticastPullMonitorable_UnreliableMulticastClientFactory;
#endif

// These factories are used only with the external reliable multicast layer
// whose name is defined by EXT_REL_MCAST_LIB_NAME : see ReliableMulticastParameters
// These factories will be used to create only one object in the process
// It is not supposed to be used by several threads
// So no thread safe mecanism are present (for example in create_object, setup..)

namespace CdmwFT {
namespace MonitoringLibrary {
 
#ifdef EXT_RELIABLE_MULTICAST 

/********************************************************************/

class SimpleMulticastPushServerFactory 
{

public:

  virtual ~SimpleMulticastPushServerFactory();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pushCorbaloc,
		    SimpleMulticastPushMonitorable_impl* monitorable);

  // static MulticastPushMonitorable_RMEOTemporalMulticastServerFactory* get_factory();

  static MulticastPushMonitorable_ptr get_unique_object();
  static void activate_unique_object();
  static void deactivate_unique_object();

protected:
  static MulticastPushMonitorable_ptr create_object();

private:
  SimpleMulticastPushServerFactory();

  static MulticastPushMonitorable_RMEOTemporalMulticastServerFactory* M_pPushServerFactory;
  static MulticastPushMonitorable_var M_PushSrvMntr;
  //static PortableServer::ObjectId M_oid;
  static PortableServer::ServantBase* M_Servant; 
};

/********************************************************************/

class SimpleMulticastPullServerFactory 
{

public:

  virtual ~SimpleMulticastPullServerFactory();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pullCorbaloc,
		    SimpleMulticastPullMonitorable_impl* monitorable);

  // static MulticastPullMonitorable_RMEOTemporalMulticastServerFactory* get_factory();

  static MulticastPullMonitorable_ptr get_unique_object();
  static void activate_unique_object();
  static void deactivate_unique_object();
  
protected:
  static MulticastPullMonitorable_ptr create_object();

private:
  SimpleMulticastPullServerFactory();

  static MulticastPullMonitorable_RMEOTemporalMulticastServerFactory* M_pPullServerFactory;
  static MulticastPullMonitorable_var M_PullSrvMntr;
  static PortableServer::ServantBase* M_Servant;
};

/********************************************************************/

class SimpleMulticastPushClientFactory 
{

public:

  virtual ~SimpleMulticastPushClientFactory();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pushCorbaloc);

  // static MulticastPushMonitorable_RMEOTemporalMulticastClientFactory* get_factory();

  static MulticastPushMonitorable_ptr get_unique_object();
  
protected:
  static MulticastPushMonitorable_ptr create_object();

private:
  SimpleMulticastPushClientFactory();

  static MulticastPushMonitorable_RMEOTemporalMulticastClientFactory* M_pPushClientFactory;
  static MulticastPushMonitorable_var M_PushCltMntr;
};

/********************************************************************/

class SimpleMulticastPullClientFactory 
{

public:

  virtual ~SimpleMulticastPullClientFactory();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pullCorbaloc);

  // static MulticastPullMonitorable_RMEOTemporalMulticastClientFactory* get_factory();

  static MulticastPullMonitorable_ptr get_unique_object();
  
protected:
  static MulticastPullMonitorable_ptr create_object();

private:
  SimpleMulticastPullClientFactory();

  static MulticastPullMonitorable_RMEOTemporalMulticastClientFactory* M_pPullClientFactory;
  static MulticastPullMonitorable_var M_PullCltMntr;
};
#endif

#ifdef EXT_UNRELIABLE_MULTICAST 

/********************************************************************/

class SimpleMulticastPushServerFactoryUnreliable 
{

public:

  virtual ~SimpleMulticastPushServerFactoryUnreliable();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pushCorbaloc,
		    SimpleMulticastPushMonitorable_impl* monitorable);

  // static MulticastPushMonitorable_UnreliableMulticastServerFactory* get_factory();

  static MulticastPushMonitorable_ptr get_unique_object();
  static void activate_unique_object();
  static void deactivate_unique_object();

protected:
  static MulticastPushMonitorable_ptr create_object();

private:
  SimpleMulticastPushServerFactoryUnreliable();

  static MulticastPushMonitorable_UnreliableMulticastServerFactory* M_pPushServerFactoryUnreliable;
  static MulticastPushMonitorable_var M_PushSrvMntr;
  static PortableServer::ServantBase* M_Servant;
};

/********************************************************************/

class SimpleMulticastPullServerFactoryUnreliable 
{

public:

  virtual ~SimpleMulticastPullServerFactoryUnreliable();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pullCorbaloc,
		    SimpleMulticastPullMonitorable_impl* monitorable);

  // static MulticastPullMonitorable_UnreliableMulticastServerFactory* get_factory();

  static MulticastPullMonitorable_ptr get_unique_object();
  static void activate_unique_object();
  static void deactivate_unique_object();
  
protected:
  static MulticastPullMonitorable_ptr create_object();

private:
  SimpleMulticastPullServerFactoryUnreliable();

  static MulticastPullMonitorable_UnreliableMulticastServerFactory* M_pPullServerFactoryUnreliable;
  static MulticastPullMonitorable_var M_PullSrvMntr;
  static PortableServer::ServantBase* M_Servant;
};

/********************************************************************/

class SimpleMulticastPushClientFactoryUnreliable 
{

public:

  virtual ~SimpleMulticastPushClientFactoryUnreliable();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pushCorbaloc);

  // static MulticastPushMonitorable_UnreliableMulticastClientFactory* get_factory();

  static MulticastPushMonitorable_ptr get_unique_object();
  
protected:
  static MulticastPushMonitorable_ptr create_object();

private:
  SimpleMulticastPushClientFactoryUnreliable();

  static MulticastPushMonitorable_UnreliableMulticastClientFactory* M_pPushClientFactoryUnreliable;
  static MulticastPushMonitorable_var M_PushCltMntr;
};

/********************************************************************/

class SimpleMulticastPullClientFactoryUnreliable 
{

public:

  virtual ~SimpleMulticastPullClientFactoryUnreliable();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pullCorbaloc);

  // static MulticastPullMonitorable_UnreliableMulticastClientFactory* get_factory();

  static MulticastPullMonitorable_ptr get_unique_object();
  
protected:
  static MulticastPullMonitorable_ptr create_object();

private:
  SimpleMulticastPullClientFactoryUnreliable();

  static MulticastPullMonitorable_UnreliableMulticastClientFactory* M_pPullClientFactoryUnreliable;
  static MulticastPullMonitorable_var M_PullCltMntr;
};
#endif // of multicast Unreliable case
};
};
#endif

