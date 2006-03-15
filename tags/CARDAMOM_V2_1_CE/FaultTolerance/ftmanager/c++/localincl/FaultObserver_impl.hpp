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


#ifndef __CDMW_FT_CDMW_FAULT_DETECTOR_HPP__
#define __CDMW_FT_CDMW_FAULT_DETECTOR_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "ftmanager/CdmwFTFaultObserver.skel.hpp"

namespace Cdmw {
namespace FT {
namespace FaultManagement {


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
class  FaultObserver_impl
    : public virtual POA_CdmwFT::FaultManagement::FaultObserver,
      public virtual PortableServer::RefCountServantBase
{

public:

    /**
     *Purpose:
     *<p> 
     */
    FaultObserver_impl(
        CORBA::ORB_ptr orb )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p> 
     */
    virtual ~FaultObserver_impl()
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/FT/PullMonitorable/is_alive:1.0
     * operation
     */
    CORBA::Boolean is_alive()
            throw( CORBA::SystemException );
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector/start_monitoring:1.0
     * operation
     */
    CdmwFT::FaultManagement::MonitoringId start_monitoring(
        ::FT::PullMonitorable_ptr monitorable_object,
        const ::FT::Location& location,
        const ::FT::FaultMonitoringIntervalAndTimeoutValue& timeout )
            throw( CORBA::SystemException,
                   ::FT::ObjectNotFound );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector/stop_monitoring:1.0
     * operation
     */
    void stop_monitoring(
        CdmwFT::FaultManagement::MonitoringId monitoring_id )
            throw( CORBA::SystemException,
                   CdmwFT::FaultManagement::InvalidMonitoringId );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PlatformObserver/notify:1.0
     * operation
     */
    void notify(
        CdmwPlatformMngt::Event * an_event )
            throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwPlatformMngt/PlatformObserver/set_fault_notifier:1.0
     * operation
     */
    void set_fault_notifier(
        ::FT::FaultNotifier_ptr fault_notifier )
            throw( CORBA::SystemException );

private:

    CORBA::ORB_var m_orb;
    ::FT::FaultNotifier_var m_fault_notifier;
};
    


}  // End namespace FaultManagement
}  // End namespace FT
}  // End namespace Cdmw

#endif // __CDMW_FT_CDMW_FAULT_DETECTOR_HPP__

