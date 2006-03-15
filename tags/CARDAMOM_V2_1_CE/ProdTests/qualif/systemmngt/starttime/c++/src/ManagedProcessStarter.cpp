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


#include "starttime/ManagedProcessStarter.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>



namespace Cdmw
{
    
const std::string ManagedProcessStarter::SUPERVISION_CORBALOC = 
    "corbaloc::localhost:21871/CdmwPlatformMngtSupervision";
const std::string ManagedProcessStarter::APPLICATION_NAME =
    "APPL";
const std::string ManagedProcessStarter::PROCESS_NAME = 
    "P11";
const std::string ManagedProcessStarter::RUN_DATE_FILENAME = 
    "GetTimeManagedProcess.log";
    
    
    ManagedProcessStarter::ManagedProcessStarter(CORBA::ORB_ptr orb, std::string hostname)
    : ProcessStarter("", ""),
      m_orb(CORBA::ORB::_duplicate(orb)),
      m_process(CdmwPlatformMngt::Process::_nil()),
      m_hostname(hostname)
{
}



void ManagedProcessStarter::start()
    throw (ProcessError)    
{
    try {
        // remove old run date log file if exists
        remove(RUN_DATE_FILENAME.c_str());
        // retrieve System object from Supervision
        CORBA::Object_var obj = m_orb->string_to_object(SUPERVISION_CORBALOC.c_str());
        CdmwPlatformMngt::System_var system = 
            CdmwPlatformMngt::System::_narrow(obj.in());
        
        // check if System is started
        CORBA::String_var status_info; 
        CdmwPlatformMngt::SystemStatus status =
            system->get_status(status_info.out());
        if (status != CdmwPlatformMngt::SYSTEM_STARTED) {
            throw ProcessError("System is not started");
        }
        
        // get Application
        CdmwPlatformMngt::Application_var appl = 
            system->get_application(APPLICATION_NAME.c_str());
            
        // get Process proxy
        m_process = appl->get_process(PROCESS_NAME.c_str(), m_hostname.c_str());
        
        CdmwPlatformMngtBase::StartupKind_var startupKind
            = new CdmwPlatformMngtBase::StartupKind();
        startupKind->state_retrieval = CdmwPlatformMngtBase::LAST_STATE;
        startupKind->startup_mode = 0;
        startupKind->specified_state_id = CORBA::string_dup("");

        // start time
        m_start_time = Cdmw::OsSupport::OS::get_time();

        // create and initialise process
        m_process->initialise(startupKind.in());
        m_is_created = true;

        // run process
        m_process->run();
  
    } catch (const CdmwPlatformMngt::ApplicationNotFound& e) {
        std::string msg = "Supervision cannot find application ";
        msg += APPLICATION_NAME;
        throw ProcessError(msg);
    } catch (const CdmwPlatformMngt::ProcessNotFound& e) {
        std::string msg = "Supervision cannot find process ";
        msg += PROCESS_NAME;
        throw ProcessError(msg);
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


void ManagedProcessStarter::stop()
{
    if (!CORBA::is_nil(m_process.in())) {
        try {
            m_process->stop(false);
        } catch (const CORBA::SystemException& e) {
            std::cout << "Error while trying to stop managed process !" << std::endl;
        }
    }    
}



bool ManagedProcessStarter::check_if_started() 
    throw (ProcessError)
{
    // try to open run date log file
    FILE* file = fopen(RUN_DATE_FILENAME.c_str(), "r");
    if (file == NULL) {
        // not yet started
        return false;
    }
    
    // read run date log file
    if (fscanf(file, "run date: %ld.%ld", 
               &m_started_time.seconds, &m_started_time.microseconds) 
        != 2)
    {
        std::string msg = "Error while reading file ";
        msg += RUN_DATE_FILENAME;
        throw ProcessError(msg);
    }
    
    fclose(file);

    m_is_started = true;
    return m_is_started;
}




}


