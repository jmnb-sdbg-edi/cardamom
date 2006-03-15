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


#ifndef INCL_CDMW_QUALIF_PROCESS_STARTER_HPP 
#define INCL_CDMW_QUALIF_PROCESS_STARTER_HPP


#include <Foundation/common/Exception.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/ossupport/ProcessCallback.hpp>



namespace Cdmw {
    

class ProcessError : public Cdmw::Exception
{
    public:
        ProcessError(const std::string& msg)
            : Cdmw::Exception(Cdmw::Exception::FATAL,
                              msg)
        {};
        
}; 
    
    

    
    
    
class ProcessStarter : Cdmw::OsSupport::ProcessCallback
{    

    public:
    
        ProcessStarter(const std::string& path,
                       const std::string& args,
                       const std::string& home = "",
                       bool autoend = false);
    
        virtual ~ProcessStarter();
  
        virtual void start()
            throw (ProcessError);
            
        virtual void stop();
        
        virtual bool check_if_started() 
            throw (ProcessError) = 0;
        
        void set_as_started();
        
        void loop_until_started(unsigned int sleep_time)
            throw (ProcessError);

        float get_start_delay()
            throw (ProcessError);
        
        
        // process creation and ending callback
        void execute(Cdmw::OsSupport::OS::ProcessId processId)
            throw ();
        
        
     protected:        
        bool m_is_created;
        bool m_is_started;
        bool m_is_ended;

        std::string m_path;
        std::string m_args;
        std::string m_home;
        
        Cdmw::OsSupport::OS::ProcessId m_pid;
        
        Cdmw::OsSupport::OS::Timeval m_start_time;
        Cdmw::OsSupport::OS::Timeval m_started_time;

        bool m_autoend;
    
};

};


#endif
