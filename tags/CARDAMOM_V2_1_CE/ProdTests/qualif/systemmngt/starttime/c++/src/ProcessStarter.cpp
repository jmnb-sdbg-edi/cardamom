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


#include "starttime/ProcessStarter.hpp"

#include <iostream>



namespace Cdmw
{

ProcessStarter::ProcessStarter(const std::string& path,
                               const std::string& args,
                               const std::string& home,
                               bool autoend)
    : m_is_created(false),
      m_is_started(false),
      m_is_ended(false),
      m_path(path),
      m_args(args),
      m_home(home),
      m_pid(0),
      m_autoend(autoend)
{
    m_start_time.seconds = 0;
    m_start_time.microseconds = 0;
    m_started_time.seconds = 0;
    m_started_time.microseconds = 0;
}



ProcessStarter::~ProcessStarter()
{
    stop();
}



void ProcessStarter::start()
    throw (ProcessError)    
{
    try {
        m_start_time = Cdmw::OsSupport::OS::get_time();
        std::cout<<"Start "<<m_path<< " with "<<m_args<<std::endl;
        
        m_pid = Cdmw::OsSupport::OS::create_process(m_path, 
                                                    m_args, 
                                                    m_home, 
                                                    this, 
                                                    this);

    } catch (const Cdmw::BadParameterException& e) {
        std::string msg = "BadParameter while creating process: ";
        msg += e.what();
        throw ProcessError(msg);
    } catch (const Cdmw::OutOfMemoryException& e) {
        throw ProcessError("OutOfMemory while creating process !");
    } catch (const Cdmw::InternalErrorException& e) {
        std::string msg = "InternalError while creating process: ";
        msg += e.what();
        throw ProcessError(msg);
    }

}


void ProcessStarter::stop()
{
    try {
        if ((m_pid != 0) && (m_autoend == false))
        {
            Cdmw::OsSupport::OS::kill_process(m_pid);
            // wait 1 second
            Cdmw::OsSupport::OS::sleep(1000);
            m_pid = 0;
        }
    } catch (const Cdmw::BadParameterException& e) {
        std::cerr << "Cannot kill pid " << m_pid << ". Process crashed ?" << std::endl;
    } catch (const Cdmw::PermissionDeniedException& e) {
        std::cerr << "Cannot kill pid " << m_pid << ". Permission denied !" << std::endl;
    } catch (const Cdmw::InternalErrorException& e) {
        std::cerr << "Cannot kill pid " << m_pid << ". Internal error !!" << std::endl;
    }
}


void ProcessStarter::loop_until_started(unsigned int sleep_time)
    throw (ProcessError)    
{
    while (!m_is_started && !m_is_ended) {
        check_if_started();
        
        try {
            Cdmw::OsSupport::OS::sleep(sleep_time);
        } catch (const Cdmw::OsSupport::InterruptedException& e) {
            // ignore
        } catch (const Cdmw::InternalErrorException& e) {
            // ignore
        }            
    }
    
}


void ProcessStarter::set_as_started()
{
    m_started_time = Cdmw::OsSupport::OS::get_time();
    m_is_started = true;
}


float ProcessStarter::get_start_delay()
    throw (ProcessError)    
{
    if (!m_is_started) {
        throw ProcessError("Process was not started.");
    }
    
    float delay;
    delay = (float)m_started_time.seconds - (float)m_start_time.seconds;
    delay += ((float)m_started_time.microseconds - (float)m_start_time.microseconds) / 1000000;
    
    return delay;
}



void ProcessStarter::execute(Cdmw::OsSupport::OS::ProcessId processId)
    throw()
{
//     if (!m_is_created) {
//         // first call => process created
//         m_is_created = true;
//     } else {
//         // second call => process ended
//         m_is_ended = true;
//         m_pid = 0;
//         if (!m_is_started) {
//             std::cout << "Process stopped but was never set as started ! (process crashed ?)" << std::endl;
//         }
//     }
}


}

