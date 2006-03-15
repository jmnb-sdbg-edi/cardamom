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


#ifndef _CDMW_FT_SIMPLE_MONITORING_OBSERVER_HPP_
#define _CDMW_FT_SIMPLE_MONITORING_OBSERVER_HPP_

#include "idllib/CdmwFTSimpleMonitoring.skel.hpp"
#include "idllib/FT.stub.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBASystemExceptionWrapper.hpp"
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include "Foundation/commonsvcs/datastore/StorageHome.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"
#include "FaultTolerance/ftstatemanager/DataStore.hpp"
#include "FaultTolerance/ftstatemanager/DataStoreContext.hpp"
#include <string>
#include <ostream>
#include <map>
namespace Cdmw {
namespace FT {


    class SimpleMonitoringObserver_impl:
        public virtual CdmwFT::MonitoringLibrary::MonitoringObserver,
        public virtual Cdmw::OrbSupport::RefCountLocalObject
    {
    public:

        SimpleMonitoringObserver_impl(CORBA::ORB_ptr orb,
                                      PortableServer::POA_ptr poa,
                                      ::FT::FaultNotifier_ptr fault_notifier);
        
        ~SimpleMonitoringObserver_impl();        
        
        virtual void on_failure (const char * the_location)
            throw(CORBA::SystemException);

        virtual void on_start (const char * the_location)
            throw(CORBA::SystemException);
        
        bool status_is(const char * the_location)
            throw(::FT::MemberNotFound);
        
        void set_status(const char * the_location, bool status)
            throw(::FT::MemberNotFound);

        void add_location(const char * the_location)
            throw(::FT::MemberAlreadyPresent);
        
        void remove_location(const char * the_location)
            throw(::FT::MemberNotFound);

    private:

        CORBA::ORB_var m_orb;
        PortableServer::POA_var m_poa;

        /**
         * The fault notifier reference;
         */
        ::FT::FaultNotifier_var m_faultNotifier;

		typedef Cdmw::CommonSvcs::DataStore::StorageHome< std::string, CORBA::Boolean > HostStatusStorageHome;
		typedef Cdmw::CommonSvcs::DataStore::StorageObject< std::string, CORBA::Boolean > HostStatusStorageObject;

        HostStatusStorageHome* m_hostStatusStorageHome;
    }; // End class   MonitoringObserver_impl
    
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_SIMPLE_MONITORING_OBSERVER_HPP_

