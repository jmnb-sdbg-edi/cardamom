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


#ifndef INCL_TEST_FAULT_DETECTOR_HPP
#define INCL_TEST_FAULT_DETECTOR_HPP

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/testutils/Testable.hpp>
#include "idllib/FT.skel.hpp"
#include "idllib/CdmwFTFaultDetector.skel.hpp"
#include <Foundation/orbsupport/RefCountServantBase.hpp>

namespace Cdmw {
namespace FT {

class TestFaultDetector : virtual public POA_CdmwFT::FaultManagement::FaultDetector,
                          public virtual Cdmw::OrbSupport::RefCountServantBase
{
public:
    // constructor
    TestFaultDetector();
    
    // destructor
    virtual ~TestFaultDetector();
    
    // start_monitoring (called by the Replication Manager)
    virtual CdmwFT::FaultManagement::MonitoringId
    start_monitoring (::FT::PullMonitorable_ptr monitorable_object,
                      const ::FT::Location & location,
                      const ::FT::FaultMonitoringIntervalAndTimeoutValue & timeout)
        throw(CORBA::SystemException,
              ::FT::ObjectNotFound);
      
    // stop_monitoring (called by the Replication Manager)
    virtual void stop_monitoring (CdmwFT::FaultManagement::MonitoringId monitoring_id)
        throw(CORBA::SystemException,
              CdmwFT::FaultManagement::InvalidMonitoringId);

    virtual void set_fault_notifier(::FT::FaultNotifier_ptr fault_notifier)
        throw (CORBA::SystemException);
    
    // is_alive 
    virtual CORBA::Boolean is_alive()
        throw(CORBA::SystemException)
    {
        return true;
        
    };
    
    
    
protected:
    
private:
    CORBA::ORB_var m_orb;
    PortableServer::POA_var m_rootPOA;
    
};
}; // namespace FT
}; // namespace Cdmw

#endif //INCL_TEST_FAULT_NOTIFIER_HPP

