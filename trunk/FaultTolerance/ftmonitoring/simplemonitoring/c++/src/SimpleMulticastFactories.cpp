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

#if defined(EXT_RELIABLE_MULTICAST) || defined(EXT_UNRELIABLE_MULTICAST)

// include files
#include "simplemonitoring/SimpleMulticastFactories.hpp"

#ifdef EXT_RELIABLE_MULTICAST  
#include "MulticastPushMonitorable_RMEOTemporalMulticastServerFactoryHelper.hpp"
#include "MulticastPullMonitorable_RMEOTemporalMulticastServerFactoryHelper.hpp"
#include "MulticastPushMonitorable_RMEOTemporalMulticastClientFactoryHelper.hpp"
#include "MulticastPullMonitorable_RMEOTemporalMulticastClientFactoryHelper.hpp"
#include "common/ReliableMulticastParameters.hpp"
#endif
#ifdef EXT_UNRELIABLE_MULTICAST
#include "MulticastPushMonitorable_UnreliableMulticastServerFactoryHelper.hpp"
#include "MulticastPullMonitorable_UnreliableMulticastServerFactoryHelper.hpp"
#include "MulticastPushMonitorable_UnreliableMulticastClientFactoryHelper.hpp"
#include "MulticastPullMonitorable_UnreliableMulticastClientFactoryHelper.hpp"
#endif

#include "ftmonitoring/common/MacroDefs.hpp"

namespace CdmwFT {
namespace MonitoringLibrary {

#ifdef EXT_RELIABLE_MULTICAST  

/***  Implementation of SimpleMulticastPushServerFactory ***/

MulticastPushMonitorable_RMEOTemporalMulticastServerFactory* SimpleMulticastPushServerFactory::M_pPushServerFactory = NULL;
MulticastPushMonitorable_var SimpleMulticastPushServerFactory::M_PushSrvMntr;
PortableServer::ServantBase* SimpleMulticastPushServerFactory::M_Servant;

// constructor
SimpleMulticastPushServerFactory::SimpleMulticastPushServerFactory()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

// destructor
SimpleMulticastPushServerFactory::~SimpleMulticastPushServerFactory()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

void SimpleMulticastPushServerFactory::setup(CORBA::ORB_ptr orb,
                                             std::string pushCorbaloc,
                                             SimpleMulticastPushMonitorable_impl* monitorable)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pushCorbaloc="<<pushCorbaloc);
    if (M_pPushServerFactory == NULL)
    {
        M_pPushServerFactory = new MulticastPushMonitorable_RMEOTemporalMulticastServerFactory();
    }

    // Get poa
    CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(object.in());
    
    M_pPushServerFactory->initServerSide(orb, poa.in(), monitorable);
    M_pPushServerFactory->setQueueSize(QUEUE_SIZE);
    M_pPushServerFactory->setCorbaloc(pushCorbaloc);
    //M_pPushServerFactory->setOrdering(Multicast::ORDER_TEMPORAL);
    //M_pPushServerFactory->setRetransmission(Multicast::MULTICAST_RETRANSMISSION);
    M_pPushServerFactory->setReliability(Multicast::RELIABLE_WITH_MULTIPLE_EMISSION);
    M_pPushServerFactory->setNumberOfSenders(NUMBER_OF_SENDERS);
    M_pPushServerFactory->setThreadPoolSize(THREAD_POOL_SIZE);
    //M_pPushServerFactory->setRetries(RETRIES);
    M_pPushServerFactory->setTimeout(TIMEOUT);
    M_pPushServerFactory->setFirstRequestMode(Multicast::FIRST_REQUEST_IGNORED);
    
}

/*
  MulticastPushMonitorable_RMEOTemporalMulticastServerFactory* SimpleMulticastPushServerFactory::get_factory()
  {
    return M_pPushServerFactory;
  }
*/

  MulticastPushMonitorable_ptr SimpleMulticastPushServerFactory::create_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MulticastPushMonitorable_var mPushSrvMntr;
      
      if (M_pPushServerFactory != NULL)
          {
              mPushSrvMntr = M_pPushServerFactory->create_object();
              M_Servant = M_pPushServerFactory->getServant();
          }
      
      return mPushSrvMntr._retn();
  }

  MulticastPushMonitorable_ptr SimpleMulticastPushServerFactory::get_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (CORBA::is_nil(M_PushSrvMntr.in()))
          {
              M_PushSrvMntr = create_object();
          }
      
      return M_PushSrvMntr._retn();
  }

  void SimpleMulticastPushServerFactory::activate_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (M_pPushServerFactory != NULL)
          {
              if (!(M_pPushServerFactory->is_activated(M_Servant)))
                  {
                      M_pPushServerFactory->activate_object_with_id(M_Servant);
                  }
          }
  }

  void SimpleMulticastPushServerFactory::deactivate_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (M_pPushServerFactory != NULL)
          {
              if (M_pPushServerFactory->is_activated(M_Servant))
                  {
                      M_pPushServerFactory->deactivate_object(M_Servant);
                  }
          }
  }


/***  Implementation of SimpleMulticastPullServerFactory ***/

MulticastPullMonitorable_RMEOTemporalMulticastServerFactory* SimpleMulticastPullServerFactory::M_pPullServerFactory = NULL;
MulticastPullMonitorable_var SimpleMulticastPullServerFactory::M_PullSrvMntr;
PortableServer::ServantBase* SimpleMulticastPullServerFactory::M_Servant;

// constructor
SimpleMulticastPullServerFactory::SimpleMulticastPullServerFactory()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

// destructor
SimpleMulticastPullServerFactory::~SimpleMulticastPullServerFactory()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

void SimpleMulticastPullServerFactory::setup(CORBA::ORB_ptr orb,
                                       std::string pullCorbaloc,
		                       SimpleMulticastPullMonitorable_impl* monitorable)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc);
    if (M_pPullServerFactory == NULL)
    {
        M_pPullServerFactory = new MulticastPullMonitorable_RMEOTemporalMulticastServerFactory();
    }

    // Get poa
    CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(object.in());
    
    M_pPullServerFactory->initServerSide(orb, poa.in(), monitorable);
    M_pPullServerFactory->setQueueSize(QUEUE_SIZE);
    M_pPullServerFactory->setCorbaloc(pullCorbaloc);
    //M_pPullServerFactory->setOrdering(Multicast::ORDER_TEMPORAL);
    //M_pPullServerFactory->setRetransmission(Multicast::MULTICAST_RETRANSMISSION);
    M_pPullServerFactory->setReliability(Multicast::RELIABLE_WITH_MULTIPLE_EMISSION);
    M_pPullServerFactory->setNumberOfSenders(NUMBER_OF_SENDERS);
    M_pPullServerFactory->setThreadPoolSize(THREAD_POOL_SIZE);
    //M_pPullServerFactory->setRetries(RETRIES);
    M_pPullServerFactory->setTimeout(TIMEOUT);
    M_pPullServerFactory->setFirstRequestMode(Multicast::FIRST_REQUEST_IGNORED);
    
}

/*
  MulticastPullMonitorable_RMEOTemporalMulticastServerFactory* SimpleMulticastPullServerFactory::get_factory()
  {
    return M_pPullServerFactory;
  }
*/

  MulticastPullMonitorable_ptr SimpleMulticastPullServerFactory::create_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MulticastPullMonitorable_var mPullSrvMntr;

      if (M_pPullServerFactory != NULL)
          {
              mPullSrvMntr = M_pPullServerFactory->create_object();
              M_Servant = M_pPullServerFactory->getServant();
          }
      
      return mPullSrvMntr._retn();
  }

  MulticastPullMonitorable_ptr SimpleMulticastPullServerFactory::get_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (CORBA::is_nil(M_PullSrvMntr.in()))
          {
              M_PullSrvMntr = create_object();
          }
      
      return M_PullSrvMntr._retn();
  }

  void SimpleMulticastPullServerFactory::activate_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (M_pPullServerFactory != NULL)
          {
              if (!(M_pPullServerFactory->is_activated(M_Servant)))
                  {
                      M_pPullServerFactory->activate_object_with_id(M_Servant);
                  }
          }
  }

  void SimpleMulticastPullServerFactory::deactivate_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (M_pPullServerFactory != NULL)
          {
              if (M_pPullServerFactory->is_activated(M_Servant))
                  {
                      M_pPullServerFactory->deactivate_object(M_Servant);
                  }
          }
  }


/***  Implementation of SimpleMulticastPushClientFactory ***/

MulticastPushMonitorable_RMEOTemporalMulticastClientFactory* SimpleMulticastPushClientFactory::M_pPushClientFactory = NULL;
MulticastPushMonitorable_var SimpleMulticastPushClientFactory::M_PushCltMntr;

// constructor
SimpleMulticastPushClientFactory::SimpleMulticastPushClientFactory()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

// destructor
SimpleMulticastPushClientFactory::~SimpleMulticastPushClientFactory()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

void SimpleMulticastPushClientFactory::setup(CORBA::ORB_ptr orb,
                                       std::string pushCorbaloc)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pushCorbaloc="<<pushCorbaloc);
    if (M_pPushClientFactory == NULL)
    {
        M_pPushClientFactory = new MulticastPushMonitorable_RMEOTemporalMulticastClientFactory();
    }

    M_pPushClientFactory->initClientSide(orb);
    M_pPushClientFactory->setQueueSize(QUEUE_SIZE);
    M_pPushClientFactory->setCorbaloc(pushCorbaloc);
    //M_pPushClientFactory->setOrdering(Multicast::ORDER_TEMPORAL);
    //M_pPushClientFactory->setRetransmission(Multicast::MULTICAST_RETRANSMISSION);
    M_pPushClientFactory->setReliability(Multicast::RELIABLE_WITH_MULTIPLE_EMISSION);
    M_pPushClientFactory->setRepetitionNumber(REPETITION_NUMBER);
    M_pPushClientFactory->setRepetitionDelay(REPETITION_DELAY);
    //M_pPushClientFactory->setRetries(RETRIES);
    //M_pPushClientFactory->setTimeout(TIMEOUT);

}

/*
  MulticastPushMonitorable_RMEOTemporalMulticastClientFactory* SimpleMulticastPushClientFactory::get_factory()
  {
    return M_pPushClientFactory;
  }
*/

  MulticastPushMonitorable_ptr SimpleMulticastPushClientFactory::create_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MulticastPushMonitorable_var mPushCltMntr;

      if (M_pPushClientFactory != NULL)
          {
              mPushCltMntr = M_pPushClientFactory->create_object();
          }
      
      return mPushCltMntr._retn();
  }

  MulticastPushMonitorable_ptr SimpleMulticastPushClientFactory::get_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (CORBA::is_nil(M_PushCltMntr.in()))
          {
              M_PushCltMntr = create_object();
          }
      
      return M_PushCltMntr._retn();
  }

/***  Implementation of SimpleMulticastPullClientFactory ***/

MulticastPullMonitorable_RMEOTemporalMulticastClientFactory* SimpleMulticastPullClientFactory::M_pPullClientFactory = NULL;
MulticastPullMonitorable_var SimpleMulticastPullClientFactory::M_PullCltMntr;

// constructor
SimpleMulticastPullClientFactory::SimpleMulticastPullClientFactory()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

// destructor
SimpleMulticastPullClientFactory::~SimpleMulticastPullClientFactory()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

void SimpleMulticastPullClientFactory::setup(CORBA::ORB_ptr orb,
                                       std::string pullCorbaloc)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc);
    if (M_pPullClientFactory == NULL)
    {
        M_pPullClientFactory = new MulticastPullMonitorable_RMEOTemporalMulticastClientFactory();
    }

    M_pPullClientFactory->initClientSide(orb);
    M_pPullClientFactory->setQueueSize(QUEUE_SIZE);
    M_pPullClientFactory->setCorbaloc(pullCorbaloc);
    //M_pPullClientFactory->setOrdering(Multicast::ORDER_TEMPORAL);
    //M_pPullClientFactory->setRetransmission(Multicast::MULTICAST_RETRANSMISSION);
    M_pPullClientFactory->setReliability(Multicast::RELIABLE_WITH_MULTIPLE_EMISSION);
    M_pPullClientFactory->setRepetitionNumber(REPETITION_NUMBER);
    M_pPullClientFactory->setRepetitionDelay(REPETITION_DELAY);
    //M_pPullClientFactory->setRetries(RETRIES);
    //M_pPullClientFactory->setTimeout(TIMEOUT);

}

/*
  MulticastPullMonitorable_RMEOTemporalMulticastClientFactory* SimpleMulticastPullClientFactory::get_factory()
  {
    return M_pPullClientFactory;
  }
*/

  MulticastPullMonitorable_ptr SimpleMulticastPullClientFactory::create_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MulticastPullMonitorable_var mPullCltMntr;
      
      if (M_pPullClientFactory != NULL)
          {
              mPullCltMntr = M_pPullClientFactory->create_object();
          }
      
      return mPullCltMntr._retn();
  }

  MulticastPullMonitorable_ptr SimpleMulticastPullClientFactory::get_unique_object()
  {      
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (CORBA::is_nil(M_PullCltMntr.in()))
          {
              M_PullCltMntr = create_object();
          }
      
      return M_PullCltMntr._retn();
  }

#endif

#ifdef EXT_UNRELIABLE_MULTICAST 

/***  Implementation of SimpleMulticastPushServerFactoryUnreliable ***/

MulticastPushMonitorable_UnreliableMulticastServerFactory* SimpleMulticastPushServerFactoryUnreliable::M_pPushServerFactoryUnreliable = NULL;
MulticastPushMonitorable_var SimpleMulticastPushServerFactoryUnreliable::M_PushSrvMntr;
PortableServer::ServantBase* SimpleMulticastPushServerFactoryUnreliable::M_Servant;

// constructor
SimpleMulticastPushServerFactoryUnreliable::SimpleMulticastPushServerFactoryUnreliable()
{      
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

// destructor
SimpleMulticastPushServerFactoryUnreliable::~SimpleMulticastPushServerFactoryUnreliable()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

void SimpleMulticastPushServerFactoryUnreliable::setup(CORBA::ORB_ptr orb,
                                                       std::string pushCorbaloc,
		                       SimpleMulticastPushMonitorable_impl* monitorable)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pushCorbaloc="<<pushCorbaloc);
    if (M_pPushServerFactoryUnreliable == NULL)
    {
        M_pPushServerFactoryUnreliable = new MulticastPushMonitorable_UnreliableMulticastServerFactory();
    }

    // Get poa
    CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(object.in());
 
    M_pPushServerFactoryUnreliable->initServerSide(orb, poa.in(), monitorable);
    M_pPushServerFactoryUnreliable->setQueueSize(QUEUE_SIZE);
    M_pPushServerFactoryUnreliable->setCorbaloc(pushCorbaloc);
    M_pPushServerFactoryUnreliable->setReliability(Multicast::UNRELIABLE);
    
}

/*
  MulticastPushMonitorable_UnreliableMulticastServerFactory* SimpleMulticastPushServerFactoryUnreliable::get_factory()
  {
    return M_pPushServerFactoryUnreliable;
  }
*/

  MulticastPushMonitorable_ptr SimpleMulticastPushServerFactoryUnreliable::create_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MulticastPushMonitorable_var mPushSrvMntr;
      
      if (M_pPushServerFactoryUnreliable != NULL)
          {
              mPushSrvMntr = M_pPushServerFactoryUnreliable->create_object();
              M_Servant = M_pPushServerFactoryUnreliable->getServant();
          }
      
      return mPushSrvMntr._retn();
  }

  MulticastPushMonitorable_ptr SimpleMulticastPushServerFactoryUnreliable::get_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (CORBA::is_nil(M_PushSrvMntr.in()))
          {
              M_PushSrvMntr = create_object();
          }
      
      return M_PushSrvMntr._retn();
  }

  void SimpleMulticastPushServerFactoryUnreliable::activate_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (M_pPushServerFactoryUnreliable != NULL)
          {
              if (!(M_pPushServerFactoryUnreliable->is_activated(M_Servant)))
                  {
                      M_pPushServerFactoryUnreliable->activate_object_with_id(M_Servant);
                  }
          }
  }

  void SimpleMulticastPushServerFactoryUnreliable::deactivate_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (M_pPushServerFactoryUnreliable != NULL)
          {
              if (M_pPushServerFactoryUnreliable->is_activated(M_Servant))
                  {
                      M_pPushServerFactoryUnreliable->deactivate_object(M_Servant);
                  }
          }
  }


/***  Implementation of SimpleMulticastPullServerFactoryUnreliable ***/

MulticastPullMonitorable_UnreliableMulticastServerFactory* SimpleMulticastPullServerFactoryUnreliable::M_pPullServerFactoryUnreliable = NULL;
MulticastPullMonitorable_var SimpleMulticastPullServerFactoryUnreliable::M_PullSrvMntr;
PortableServer::ServantBase* SimpleMulticastPullServerFactoryUnreliable::M_Servant;

// constructor
SimpleMulticastPullServerFactoryUnreliable::SimpleMulticastPullServerFactoryUnreliable()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

// destructor
SimpleMulticastPullServerFactoryUnreliable::~SimpleMulticastPullServerFactoryUnreliable()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

void SimpleMulticastPullServerFactoryUnreliable::setup(CORBA::ORB_ptr orb,
                                                       std::string pullCorbaloc,
		                       SimpleMulticastPullMonitorable_impl* monitorable)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc);
    if (M_pPullServerFactoryUnreliable == NULL)
    {
        M_pPullServerFactoryUnreliable = new MulticastPullMonitorable_UnreliableMulticastServerFactory();
    }

    // Get poa
    CORBA::Object_var object = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(object.in());
    
    M_pPullServerFactoryUnreliable->initServerSide(orb, poa.in(), monitorable);
    M_pPullServerFactoryUnreliable->setQueueSize(QUEUE_SIZE);
    M_pPullServerFactoryUnreliable->setCorbaloc(pullCorbaloc);
    M_pPullServerFactoryUnreliable->setReliability(Multicast::UNRELIABLE);
    
}

/*
  MulticastPullMonitorable_UnreliableMulticastServerFactory* SimpleMulticastPullServerFactoryUnreliable::get_factory()
  {
    return M_pPullServerFactoryUnreliable;
  }
*/

  MulticastPullMonitorable_ptr SimpleMulticastPullServerFactoryUnreliable::create_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MulticastPullMonitorable_var mPullSrvMntr;
      
      if (M_pPullServerFactoryUnreliable != NULL)
          {
              mPullSrvMntr = M_pPullServerFactoryUnreliable->create_object();
              M_Servant = M_pPullServerFactoryUnreliable->getServant();
          }
      
      return mPullSrvMntr._retn();
  }

  MulticastPullMonitorable_ptr SimpleMulticastPullServerFactoryUnreliable::get_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (CORBA::is_nil(M_PullSrvMntr.in()))
          {
              M_PullSrvMntr = create_object();
          }
      
      return M_PullSrvMntr._retn();
  }
    
  void SimpleMulticastPullServerFactoryUnreliable::activate_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (M_pPullServerFactoryUnreliable != NULL)
          {
              if (!(M_pPullServerFactoryUnreliable->is_activated(M_Servant)))
                  {
                      M_pPullServerFactoryUnreliable->activate_object_with_id(M_Servant);
                  }
          }
  }

  void SimpleMulticastPullServerFactoryUnreliable::deactivate_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (M_pPullServerFactoryUnreliable != NULL)
          {
              if (M_pPullServerFactoryUnreliable->is_activated(M_Servant))
                  {
                      M_pPullServerFactoryUnreliable->deactivate_object(M_Servant);
                  }
          }
  }


/***  Implementation of SimpleMulticastPushClientFactoryUnreliable ***/

MulticastPushMonitorable_UnreliableMulticastClientFactory* SimpleMulticastPushClientFactoryUnreliable::M_pPushClientFactoryUnreliable = NULL;
MulticastPushMonitorable_var SimpleMulticastPushClientFactoryUnreliable::M_PushCltMntr;

// constructor
SimpleMulticastPushClientFactoryUnreliable::SimpleMulticastPushClientFactoryUnreliable()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

// destructor
SimpleMulticastPushClientFactoryUnreliable::~SimpleMulticastPushClientFactoryUnreliable()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

void SimpleMulticastPushClientFactoryUnreliable::setup(CORBA::ORB_ptr orb,
                                                       std::string pushCorbaloc)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pushCorbaloc="<<pushCorbaloc);
    if (M_pPushClientFactoryUnreliable == NULL)
    {
        M_pPushClientFactoryUnreliable = new MulticastPushMonitorable_UnreliableMulticastClientFactory();
    }

    M_pPushClientFactoryUnreliable->initClientSide(orb);
    M_pPushClientFactoryUnreliable->setQueueSize(QUEUE_SIZE);
    M_pPushClientFactoryUnreliable->setCorbaloc(pushCorbaloc);
    M_pPushClientFactoryUnreliable->setReliability(Multicast::UNRELIABLE);
    
}

/*
  MulticastPushMonitorable_UnreliableMulticastClientFactory* SimpleMulticastPushClientFactoryUnreliable::get_factory()
  {
    return M_pPushClientFactoryUnreliable;
  }
*/

  MulticastPushMonitorable_ptr SimpleMulticastPushClientFactoryUnreliable::create_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MulticastPushMonitorable_var mPushCltMntr;

      if (M_pPushClientFactoryUnreliable != NULL)
          {
              mPushCltMntr = M_pPushClientFactoryUnreliable->create_object();
          }
      
      return mPushCltMntr._retn();
  }

  MulticastPushMonitorable_ptr SimpleMulticastPushClientFactoryUnreliable::get_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (CORBA::is_nil(M_PushCltMntr.in()))
          {
              M_PushCltMntr = create_object();
          }
      
      return M_PushCltMntr._retn();
  }

/***  Implementation of SimpleMulticastPullClientFactoryUnreliable ***/

MulticastPullMonitorable_UnreliableMulticastClientFactory* SimpleMulticastPullClientFactoryUnreliable::M_pPullClientFactoryUnreliable = NULL;
MulticastPullMonitorable_var SimpleMulticastPullClientFactoryUnreliable::M_PullCltMntr;

// constructor
SimpleMulticastPullClientFactoryUnreliable::SimpleMulticastPullClientFactoryUnreliable()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

// destructor
SimpleMulticastPullClientFactoryUnreliable::~SimpleMulticastPullClientFactoryUnreliable()
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
}

void SimpleMulticastPullClientFactoryUnreliable::setup(CORBA::ORB_ptr orb,
                                       std::string pullCorbaloc)
{
    CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"pullCorbaloc="<<pullCorbaloc);
    if (M_pPullClientFactoryUnreliable == NULL)
    {
        M_pPullClientFactoryUnreliable = new MulticastPullMonitorable_UnreliableMulticastClientFactory();
    }

    M_pPullClientFactoryUnreliable->initClientSide(orb);
    M_pPullClientFactoryUnreliable->setQueueSize(QUEUE_SIZE);
    M_pPullClientFactoryUnreliable->setCorbaloc(pullCorbaloc);
    M_pPullClientFactoryUnreliable->setReliability(Multicast::UNRELIABLE);
    
}

/*
  MulticastPullMonitorable_UnreliableMulticastClientFactory* SimpleMulticastPullClientFactoryUnreliable::get_factory()
  {
    return M_pPullClientFactoryUnreliable;
  }
*/

  MulticastPullMonitorable_ptr SimpleMulticastPullClientFactoryUnreliable::create_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      MulticastPullMonitorable_var mPullCltMntr;
      
      if (M_pPullClientFactoryUnreliable != NULL)
          {
              mPullCltMntr = M_pPullClientFactoryUnreliable->create_object();
          }
      
      return mPullCltMntr._retn();
  }

  MulticastPullMonitorable_ptr SimpleMulticastPullClientFactoryUnreliable::get_unique_object()
  {
      CDMW_LOG_FUNCTION(::Cdmw::FT::FTLogger::GetLogger(),"");
      if (CORBA::is_nil(M_PullCltMntr.in()))
          {
              M_PullCltMntr = create_object();
          }
      
      return M_PullCltMntr._retn();
  }

#endif // of multicast Unreliable case

} // End of namespace MonitoringLibrary
} // End of namespace CdmwFT

#endif // of ndef Tao multicast
