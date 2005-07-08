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


#ifndef INCL_PLATFORMMNGT_CALLBACK_IMPL_HPP 
#define INCL_PLATFORMMNGT_CALLBACK_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "SystemMngt/platformlibrary/DeactivableServant_impl.hpp"
#include "SystemMngt/platformlibrary/AcknowledgementCallback_impl.hpp"

#include "idllib/CdmwPlatformMngtDaemon.skel.hpp"
#include "idllib/CdmwPlatformMngtApplicationAgent.stub.hpp"
#include "idllib/CdmwPlatformMngtCommon.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{


// /**
// * Purpose:
// * <p>
// * implements the
// * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationCallback:1.0
// * interface
// */
// class ApplicationCallback_impl : virtual public POA_CdmwPlatformMngt::ApplicationCallback,
//     virtual public PortableServer::RefCountServantBase,
//     virtual public AcknowledgementCallback_impl
// {
// public:
// 
//     /**
//     * Purpose:
//     * <p>
//     * implements the
//     * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationCallback/set_ready:1.0
//     * operation
//     */
//     virtual CdmwPlatformMngt::ApplicationAck* set_ready(
//         CdmwPlatformMngt::Application_ptr the_application,
//         CdmwPlatformMngt::ApplicationControl_ptr the_application_control)
//         throw(CORBA::SystemException);
// };


/**
* Purpose:
* <p>
* implements the
* IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentCallback:1.0
* interface
*/
class ApplicationAgentCallback_impl : virtual public DeactivableServant_impl,
                                      virtual public POA_CdmwPlatformMngt::ApplicationAgentCallback,
                                      virtual public PortableServer::RefCountServantBase,
                                      virtual public AcknowledgementCallback_impl
{
public:

    /**
    * Purpose:
    * <p> Constructor
    */ 
    ApplicationAgentCallback_impl(
            PortableServer::POA_ptr poa,
            const CdmwPlatformMngt::ApplicationAgentAck& acknowledgementData);

    /**
    * Purpose:
    * <p> Destructor
    */ 
    virtual ~ApplicationAgentCallback_impl() throw();

    /**
    * Purpose:
    * <p> Returns the acknowledged application agent.
    */ 
    CdmwPlatformMngt::ApplicationAgent_ptr getApplicationAgent();


    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentCallback/set_ready:1.0
    * operation
    */
    virtual CdmwPlatformMngt::ApplicationAgentAck* set_ready(
            CdmwPlatformMngt::ApplicationAgent_ptr the_agent)
        throw(CORBA::SystemException);

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentCallback/notify_error:1.0
    * operation
    */
    virtual void notify_error(const char* error_info)
        throw(CORBA::SystemException);


protected:

    /**
    * The data returned to the application agent upon its acknowledgemnt.
    */
    CdmwPlatformMngt::ApplicationAgentAck_var m_acknowledgementData;

    /**
    * The reference to the application agent.
    */
    CdmwPlatformMngt::ApplicationAgent_var m_applicationAgent;

};


// /**
// * Purpose:
// * <p>
// * implements the
// * IDL:thalesgroup.com/CdmwPlatformMngt/HostProbeCallback:1.0
// * interface
// */
// class HostProbeCallback_impl : virtual public POA_CdmwPlatformMngt::HostProbeCallback,
//     virtual public PortableServer::RefCountServantBase,
//     virtual public AcknowledgementCallback_impl
// {
// public:
// 
//     /**
//     * Purpose:
//     * <p>
//     * implements the
//     * IDL:thalesgroup.com/CdmwPlatformMngt/HostProbeCallback/set_ready:1.0
//     * operation
//     */
//     virtual void set_ready(CdmwPlatformMngt::HostProbe_ptr the_probe)
//         throw(CORBA::SystemException);
// };


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_CALLBACK_IMPL_HPP

