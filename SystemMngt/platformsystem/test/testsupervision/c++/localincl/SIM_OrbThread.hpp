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


#ifndef INCL_TRACE_ORBTHREAD_HPP 
#define INCL_TRACE_ORBTHREAD_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/StrategyList.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include "SystemMngt/platformvaluetypes/SystemModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/SystemStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/HostStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationAgentDeath_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationModeChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ApplicationStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/ProcessStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/EntityStatusChange_impl.hpp"
#include "SystemMngt/platformvaluetypes/UserEvent_impl.hpp"

namespace Cdmw 
{
namespace PlatformMngt
{


class OrbThread : public OsSupport::Thread {

    public:
        
        OrbThread(){
             
            
            OrbSupport::StrategyList strategyList;
            strategyList.add_OrbThreaded();
            strategyList.add_PoaThreadPool(10);
            strategyList.add_multicast();

            // set orb arguments
            std::string str_opt("");
            str_opt += Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
            str_opt += "=5041";
            char** argv__ = Cdmw::Common::String::to_char_array
            (Cdmw::Common::String::to_strings(str_opt));
            int argc__(1);

            m_orb = OrbSupport::OrbSupport::ORB_init(argc__,argv__, strategyList ); 
            
            // And activate the root POA
            CORBA::Object_var obj = m_orb->resolve_initial_references("RootPOA");
            m_poa = PortableServer::POA::_narrow(obj.in());
            PortableServer::POAManager_var poaMgr = m_poa->the_POAManager();
            poaMgr->activate();

            // Register event factories
            CORBA::ValueFactoryBase_var factory, old_factory;

            factory = new SystemModeChangeFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/SystemModeChange:1.0",
                                                        factory.in() );

            factory = new SystemStatusChangeFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/SystemStatusChange:1.0",
                                                      factory.in() );

            factory = new HostStatusChangeFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/HostStatusChange:1.0",
                                                      factory.in() );

            factory = new ApplicationAgentDeathFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationAgentDeath:1.0",
                                                      factory.in() );

            factory = new ApplicationModeChangeFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationModeChange:1.0",
                                                      factory.in() );

            factory = new ApplicationStatusChangeFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationStatusChange:1.0",
                                                      factory.in() );

            factory = new ProcessMessageFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/ProcessMessage:1.0",
                                                      factory.in() );

            factory = new ProcessStatusChangeFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/ProcessStatusChange:1.0",
                                                      factory.in() );

            factory = new SystemEntityStatusChangeFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/SystemEntityStatusChange:1.0",
                                                      factory.in() );

            factory = new ApplicationEntityStatusChangeFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/ApplicationEntityStatusChange:1.0",
                                                      factory.in() );

            factory = new ProcessEntityStatusChangeFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/ProcessEntityStatusChange:1.0",
                                                      factory.in() );
            
            factory = new UserEventFactory;
            old_factory = m_orb->register_value_factory("IDL:thalesgroup.com/CdmwPlatformMngt/UserEvent:1.0",
                                                      factory.in() );
            

        }

        ~OrbThread() throw(){};


        void
        shutdown() {
            m_orb->shutdown(false);
            OrbSupport::OrbSupport::ORB_cleanup(m_orb.in());
            m_orb->destroy();
        }


        CORBA::ORB_ptr orb()
        {
            return CORBA::ORB::_duplicate(m_orb.in());
        }

        
        PortableServer::POA_ptr poa()
        {
            return PortableServer::POA::_duplicate(m_poa.in());
        }

    protected:

        void
        run() throw() {

            m_orb->run();
            
        }

    private:

        CORBA::ORB_var  m_orb;

        PortableServer::POA_var m_poa;

};

} // End namespace PlatformMngt

} // End namespace Cdmw

#endif // INCL_TRACE_ORBTHREAD_HPP 

