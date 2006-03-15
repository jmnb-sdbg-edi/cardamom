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


#ifndef INCL_PLATFORMMNGT_PULLMONITORABLE_REGISTRATION_IMPL_HPP 
#define INCL_PLATFORMMNGT_PULLMONITORABLE_REGISTRATION_IMPL_HPP

#include <string> 

#include "orbsupport/CORBA.hpp"

#include "idllib/CdmwPlatformMngtPullMonitorableRegistration.skel.hpp"

#include "idllib/CdmwPlatformMngtLocalFaultDetector.stub.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

class Daemon_impl;



/**
* Purpose:
* <p>
* implements the
* IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorableRegistration:1.0
* interface
*/
class PullMonitorableRegistration_impl : virtual public POA_CdmwPlatformMngt::PullMonitorableRegistration,
                                         virtual public PortableServer::RefCountServantBase
{
public:

    /**
    * Purpose:
    * <p> Constructor
    *
    * @param orb the initialised ORB.
    * @param faultDetector the local fault detector
    * @param daemon the daemon pointer
    * @param pullInterval the pull monitoring interval
    * @param pullTimeout the pull monitoring timeout
    */ 
    PullMonitorableRegistration_impl(CORBA::ORB_ptr orb,
                                     CdmwPlatformMngt::LocalFaultDetector_ptr faultDetector,
                                     Daemon_impl* daemon,
                                     unsigned long pullInterval,
                                     unsigned long pullTimeout);

    /**
    * Purpose:
    * <p> Destructor
    */ 
    virtual ~PullMonitorableRegistration_impl() throw();

    /**
    * Purpose:
    * <p>
    * implements the
    * IDL:thalesgroup.com/CdmwPlatformMngt/PullMonitorableRegistration_impl/register_pull_monitorable:1.0
    * operation
    */
    virtual void register_pull_monitorable(::FT::PullMonitorable_ptr pull_monitorable)
        throw(CORBA::SystemException);


private:

    /**
     * The local hostname
     */
    std::string m_localHostName;

    /**
    * The reference to the pull monitorable object.
    */
    ::FT::PullMonitorable_var m_pull_monitorable;
    
    /**
     * The PortableServer::Current to get the current interface.
     */
    PortableServer::Current_var m_current;
    
    /**
     * The daemon reference.
     */
    Daemon_impl* m_daemon;
    
    /**
     * The pull monitoring interval.
     */
    unsigned long m_pull_interval;
    
    /**
     * The pull monitoring timeout.
     */
    unsigned long m_pull_timeout;
    
    /**
    * The local fault detector.
    */
    CdmwPlatformMngt::LocalFaultDetector_var m_localFaultDetector;

};


} // End namespace PlatformMngt
} // End namespace Cdmw

#endif // INCL_PLATFORMMNGT_PULLMONITORABLE_REGISTRATION_IMPL_HPP

