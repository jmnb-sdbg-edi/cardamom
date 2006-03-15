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

#ifndef INCL_CDMWFT_MONITORINGLIBRARY_MUTUALMONITORING_MUTUALMULTICASTFACTORIES_HPP
#define INCL_CDMWFT_MONITORINGLIBRARY_MUTUALMONITORING_MUTUALMULTICASTFACTORIES_HPP

#include "MutualMulticastPushMonitorable_impl.hpp"
#include "MutualMulticastPullMonitorable_impl.hpp"

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

namespace CdmwFT {
namespace MonitoringLibrary {

// These factories are used only with the external reliable multicast layer
// whose name is defined by EXT_REL_MCAST_LIB_NAME : see ReliableMulticastParameters
// This factory will be used to create only one object in the process
// It is not supposed to be used by several threads
// So no thread safe mecanism are present (for example in create_object, setup..)

#ifdef EXT_RELIABLE_MULTICAST

/********************************************************************/

class MutualMulticastPushServerFactory 
{

public:

  virtual ~MutualMulticastPushServerFactory();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pushCorbaloc,
		    MutualMulticastPushMonitorable_impl* monitorable);

  // static MulticastPushMonitorable_RMEOTemporalMulticastServerFactory* get_factory();

  static MulticastPushMonitorable_ptr get_unique_object();
  static void activate_unique_object();
  static void deactivate_unique_object();
  
protected:
  static MulticastPushMonitorable_ptr create_object();

private:
  MutualMulticastPushServerFactory();

  static MulticastPushMonitorable_RMEOTemporalMulticastServerFactory* M_pPushServerFactory;
  static MulticastPushMonitorable_var M_PushSrvMntr;
  //static PortableServer::ObjectId M_oid;
  static PortableServer::ServantBase* M_Servant; 
};

/********************************************************************/

class MutualMulticastPullServerFactory 
{

public:

  virtual ~MutualMulticastPullServerFactory();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pullCorbaloc,
		    MutualMulticastPullMonitorable_impl* monitorable);

  // static MulticastPullMonitorable_RMEOTemporalMulticastServerFactory* get_factory();

  static MulticastPullMonitorable_ptr get_unique_object();
  static void activate_unique_object();
  static void deactivate_unique_object();
  
protected:
  static MulticastPullMonitorable_ptr create_object();

private:
  MutualMulticastPullServerFactory();

  static MulticastPullMonitorable_RMEOTemporalMulticastServerFactory* M_pPullServerFactory;
  static MulticastPullMonitorable_var M_PullSrvMntr;
  static PortableServer::ServantBase* M_Servant;
};

/********************************************************************/

class MutualMulticastPushClientFactory 
{

public:

  virtual ~MutualMulticastPushClientFactory();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pushCorbaloc);

  // static MulticastPushMonitorable_RMEOTemporalMulticastClientFactory* get_factory();

  static MulticastPushMonitorable_ptr get_unique_object();
  
protected:
  static MulticastPushMonitorable_ptr create_object();

private:
  MutualMulticastPushClientFactory();

  static MulticastPushMonitorable_RMEOTemporalMulticastClientFactory* M_pPushClientFactory;
  static MulticastPushMonitorable_var M_PushCltMntr;
};

/********************************************************************/

class MutualMulticastPullClientFactory 
{

public:

  virtual ~MutualMulticastPullClientFactory();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pullCorbaloc);

  // static MulticastPullMonitorable_RMEOTemporalMulticastClientFactory* get_factory();

  static MulticastPullMonitorable_ptr get_unique_object();
  
protected:
  static MulticastPullMonitorable_ptr create_object();

private:
  MutualMulticastPullClientFactory();

  static MulticastPullMonitorable_RMEOTemporalMulticastClientFactory* M_pPullClientFactory;
  static MulticastPullMonitorable_var M_PullCltMntr;
};

#endif

#ifdef EXT_UNRELIABLE_MULTICAST 

/********************************************************************/

class MutualMulticastPushServerFactoryUnreliable 
{

public:

  virtual ~MutualMulticastPushServerFactoryUnreliable();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pushCorbaloc,
		    MutualMulticastPushMonitorable_impl* monitorable);

  // static MulticastPushMonitorable_UnreliableMulticastServerFactory* get_factory();

  static MulticastPushMonitorable_ptr get_unique_object();
  static void activate_unique_object();
  static void deactivate_unique_object();

protected:
  static MulticastPushMonitorable_ptr create_object();

private:
  MutualMulticastPushServerFactoryUnreliable();

  static MulticastPushMonitorable_UnreliableMulticastServerFactory* M_pPushServerFactoryUnreliable;
  static MulticastPushMonitorable_var M_PushSrvMntr;
  static PortableServer::ServantBase* M_Servant;
};

/********************************************************************/

class MutualMulticastPullServerFactoryUnreliable 
{

public:

  virtual ~MutualMulticastPullServerFactoryUnreliable();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pullCorbaloc,
		    MutualMulticastPullMonitorable_impl* monitorable);

  // static MulticastPullMonitorable_UnreliableMulticastServerFactory* get_factory();

  static MulticastPullMonitorable_ptr get_unique_object();
  static void activate_unique_object();
  static void deactivate_unique_object();
  
protected:
  static MulticastPullMonitorable_ptr create_object();

private:
  MutualMulticastPullServerFactoryUnreliable();

  static MulticastPullMonitorable_UnreliableMulticastServerFactory* M_pPullServerFactoryUnreliable;
  static MulticastPullMonitorable_var M_PullSrvMntr;
  static PortableServer::ServantBase* M_Servant;
};

/********************************************************************/

class MutualMulticastPushClientFactoryUnreliable 
{

public:

  virtual ~MutualMulticastPushClientFactoryUnreliable();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pushCorbaloc);

  // static MulticastPushMonitorable_UnreliableMulticastClientFactory* get_factory();

  static MulticastPushMonitorable_ptr get_unique_object();
  
protected:
  static MulticastPushMonitorable_ptr create_object();

private:
  MutualMulticastPushClientFactoryUnreliable();

  static MulticastPushMonitorable_UnreliableMulticastClientFactory* M_pPushClientFactoryUnreliable;
  static MulticastPushMonitorable_var M_PushCltMntr;
};

/********************************************************************/

class MutualMulticastPullClientFactoryUnreliable 
{

public:

  virtual ~MutualMulticastPullClientFactoryUnreliable();

  static void setup(CORBA::ORB_ptr orb,
                    std::string pullCorbaloc);

  // static MulticastPullMonitorable_UnreliableMulticastClientFactory* get_factory();

  static MulticastPullMonitorable_ptr get_unique_object();
  
protected:
  static MulticastPullMonitorable_ptr create_object();

private:
  MutualMulticastPullClientFactoryUnreliable();

  static MulticastPullMonitorable_UnreliableMulticastClientFactory* M_pPullClientFactoryUnreliable;
  static MulticastPullMonitorable_var M_PullCltMntr;
};
#endif // of multicast Unreliable case
};
};
#endif

