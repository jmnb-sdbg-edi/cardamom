/* =========================================================================== *
 * Copyright (c) 2003-2005 THALES All rights reserved.
 * Software commonly developed by THALES and AMS.
 *
 * This file is part of CARDAMOM.
 *
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Library General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with CARDAMOM; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */


#include "starttime/SystemStarter.hpp"
#include "starttime/TimeCollector_impl.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include <iostream>

namespace
{
    int NB_PROCESSES = 20;
    const std::string SUPERVISION_CORBALOC = 
    "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";
};


namespace Cdmw
{

    SystemStarter::SystemStarter(CORBA::ORB_ptr orb)
    : ProcessStarter("", ""),
      m_orb(CORBA::ORB::_duplicate(orb))

{         
    Cdmw::TimeCollector_impl* obj = new Cdmw::TimeCollector_impl(m_orb.in(), NB_PROCESSES);
    m_collector = obj->_this();
    std::cout<<"bind _object"<<std::endl;
    //bind the collector to the corbaloc
    Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc(m_orb.in(), "TIME_COLLECTOR", m_collector.in());
}



void SystemStarter::start()
    throw (ProcessError)    
{

    try {
        // retrieve System object from Supervision
        CORBA::Object_var obj = m_orb->string_to_object(SUPERVISION_CORBALOC.c_str());
        m_system = CdmwPlatformMngt::System::_narrow(obj.in());
        
        CdmwPlatformMngtBase::StartupKind_var startupKind
            = new CdmwPlatformMngtBase::StartupKind();
        startupKind->state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
        startupKind->startup_mode = 0;
        startupKind->specified_state_id = CORBA::string_dup("");

        // start time
        m_start_time = Cdmw::OsSupport::OS::get_time();
        m_system->start(startupKind.in());
        std::cout<<"start_time"<<m_start_time.seconds<<"."<<m_start_time.microseconds<<std::endl;
        m_is_created = true;


    } catch (const CdmwPlatformMngt::IncompatibleStatus& e) {
        std::string msg = "Cannot initialise or run process (IncompatibleStatus !)";
        throw ProcessError(msg);
    } catch (const CdmwPlatformMngt::NotReadyToRun& e) {
        std::string msg = "Cannot run process (NotReadyToRun !)";
        throw ProcessError(msg);
    } catch (const CdmwPlatformMngt::IncompatibleType& e) {
        std::string msg = "Cannot run process (IncompatibleType !)";
        throw ProcessError(msg);
    } catch (const CORBA::SystemException& e) {
        std::string msg = "Error while contacting Supervision on ";
        msg += SUPERVISION_CORBALOC;
        throw ProcessError(msg);
    }

}


void SystemStarter::stop()
{
    if (!CORBA::is_nil(m_system.in())) {
        try {
            m_system->stop(false);
        } catch (const CORBA::SystemException& e) {
            std::cout << "Error while trying to stop managed process !" << std::endl;
        }
    }   
}


bool SystemStarter::check_if_started() 
    throw (ProcessError)
{
    if (m_collector->all_process_are_started())
    {
        Cdmw::Test::time timestruct= m_collector->get_max_time();
        
        m_started_time.seconds = timestruct.seconds ;
        m_started_time.microseconds = timestruct.microseconds;
        
        std::cout<<m_started_time.seconds<<"."<<m_started_time.microseconds<<std::endl;
        m_is_started = true;
        
    }
    
     return m_collector->all_process_are_started();

    
}




}

