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

#ifndef INCL_PLATFORMMNGT_FT_CONVERTER_IMPL_HPP
#define INCL_PLATFORMMNGT_FT_CONVERTER_IMPL_HPP

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtMonitoringObserver.stub.hpp"
#include "SystemMngt/platformapplicationlibrary/CdmwPlatformMngtFTConverter.skel.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtApplication.stub.hpp"
#include "platformsystem/HostContainer_impl.hpp"
#include <map>
namespace Cdmw
{
namespace PlatformMngt
{
        
class System_impl;
        
        
/**
*Purpose:
*<p> Provides the event notifications to the processes of an application.
*/
class FTConverter_impl : virtual public POA_CdmwPlatformMngt::FTConverter,
                         virtual public PortableServer::RefCountServantBase
{
    
public:
    
    /**
     *Purpose:
     *<p> Constructor.
     */
    FTConverter_impl (HostContainer_impl* hostContainer);

    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/FTConverter/register_application_agent_observer:1.0
     * operation
     */
    void register_application_agent_observer(const char* observer_name,
                           CdmwPlatformMngt::MonitorableProcessObserver_ptr observer )
        throw( CORBA::SystemException );


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/FTConverter/unregister_application_agent_observer:1.0
     * operation
     */
  
    void unregister_application_agent_observer(const char* observer_name)
        throw( CORBA::SystemException );
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/FTConverter/register_process_observer:1.0
     * operation
     */
    void register_process_observer(const char* observer_name,
                                   CdmwPlatformMngt::ProcessObserver_ptr observer )
        throw( CORBA::SystemException );


    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/FTConverter/unregister_process_observer:1.0
     * operation
     */
  
    void unregister_process_observer(const char* observer_name)
        throw( CORBA::SystemException );
    

    /// Report a change in the list of publications.
    /**
     * IDL:omg.org/CosNotifyComm/NotifyPublish/offer_change:1.0
     *
     * Not Yet Implemented!
     *
     * @param added The list of new event types that the consumer can
     *   expect.
     * @param removed The list of event types that the consumer should
     *   no longer expect.
     * @exception InvalidEventType if the one or more event types
     *   provided is invalid
     * @exception CORBA::NO_IMPLEMENT
     */
    virtual void offer_change (const CosNotification::EventTypeSeq & added,
                               const CosNotification::EventTypeSeq & removed)
        throw (CosNotifyComm::InvalidEventType,
               CORBA::SystemException);
    
    /// Receive one structured event
    /**
     * IDL:omg.org/CosNotifyComm/StructuredPushConsumer/push_structered_event:1.0
     *
     *  This operation is invoked to provide one event to the
     * consumer.
     * @exception CosEventComm::Disconnected if the object considers
     *   itself no longer connected to its peer.
     */
    virtual void push_structured_event(const CosNotification::StructuredEvent & notification)
        throw (CosEventComm::Disconnected,
               CORBA::SystemException);
    
    /// The peer has disconnected
    /**
     * IDL:omg.org/CosNotifyComm/StructuredPushConsumer/disconnect_structured_push_consumer:1.0
     *
     * Does nothing ...
     * This operation is invoked by the consumer peer when it wishes
     * to disconnect.  The consumer can safely assume that no more
     * events will follow this request.
     */
    virtual void disconnect_structured_push_consumer()
        throw (CORBA::SystemException);
    
private:
    
    /**
     *Purpose:
     *<p> Copy constructor is not allowed.
     */
    FTConverter_impl (FTConverter_impl& rhs)
    {};

    /**
     * Purpose:
     * <p> Assignment operator is not allowed.
     */ 
    FTConverter_impl& operator= (const FTConverter_impl& rhs)
    {
        return *this;
    };


    typedef std::map< std::string, CdmwPlatformMngt::MonitorableProcessObserver_var > ApplicationAgentObservers;
    
    ApplicationAgentObservers m_applicationAgentObservers;

    typedef std::map< std::string, CdmwPlatformMngt::ProcessObserver_var > ProcessObservers;
    
    ProcessObservers m_processObservers;

    /**
     * The host container of the system
     */
    HostContainer_impl* m_hostContainer;
    
    
};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif //INCL_PLATFORMMNGT_APPLICATION_OBSERVER_IMPL_HPP

