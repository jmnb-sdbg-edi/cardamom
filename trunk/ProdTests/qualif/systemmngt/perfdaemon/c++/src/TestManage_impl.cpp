/* =========================================================================== *
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
 * =========================================================================== */

#include "perfdaemon/TestManage_impl.hpp"
#include "SystemMngt/platformlibrary/EventHeaderFactory.hpp"
#include <iostream>
#include <fstream>

namespace Cdmw
{

namespace Test
{



    /**
     * Purpose:
     * <p> [Constructor description if necessary]
     * 
     *@param
     *@exception
     */ 
    observer_impl::observer_impl(CORBA::ORB_ptr orb)
        throw(CORBA::SystemException)
        : m_inc(0),
          m_host_is_available(true)
    {
    }
    
    
    /**
     * Purpose:
     * <p> [ Destructor description if necessary]
     * 
     */ 
    
    observer_impl::~observer_impl()
        throw()
    {
    }
    
    
    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    void  observer_impl::kill_daemon ()
        throw(CORBA::SystemException)
    {
        std::cout<<"kill_daemon"<<std::endl;
    }
    
    
    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    void  observer_impl::kill_daemon_time ( CORBA::Long second, CORBA::Long micro)
        throw(CORBA::SystemException)
    {
        std::cout<<"kill_daemon_time"<<std::endl;
    }
    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    void observer_impl::notify( CdmwPlatformMngt::Event* an_event )
        throw( CORBA::SystemException )
    {
        CdmwPlatformMngtBase::TimeStamp time = Cdmw::PlatformMngt::EventHeaderFactory::createTimeStamp();

        switch(an_event->event_kind())
        {
            case CdmwPlatformMngt::HOST_STATUS_CHANGE:
                {
                    CdmwPlatformMngt::HostStatusChange* event = dynamic_cast< CdmwPlatformMngt::HostStatusChange * > (an_event);
                    if (event->host_status() == CdmwPlatformMngt::HOST_RESPONDING)
                    {
                        m_host_is_available = true;
                        std::cout<<"The Daemon is alived"<<std::endl;
                    } else {
                        m_host_is_available = false;
                        //take the notification time
                        m_localtimebefore[m_inc] = event->header().time_stamp;
                        m_localtimeafter[m_inc] = time;
                        std::cout<<"The Daemon is failed"<<std::endl;
                    }
                    break;
                }
                
            default:
                {
 //                    std::cout<<an_event->to_string()<<std::endl;
               }
        }
        


//         CORBA::String_var message = an_event->to_string();
//         std::cout<<message.in()<<std::endl;
        
    }

    
    void observer_impl::incrementation()
    {
        m_inc++;
    }
    
    bool observer_impl::host_is_available()
    {
        return m_host_is_available;
    }
    

    /**
     * Purpose:
     * <p> [Constructor description if necessary]
     * 
     *@param
     *@exception
     */ 
    killer_impl::killer_impl(CORBA::ORB_ptr orb)
        throw(CORBA::SystemException)
    {
        std::string file("/tmp/CdmwDaemon_21869/__CDMW_DAEMON__.pid");
        std::ifstream ifs(file.c_str());
        if (ifs.good())
        {
            std::string pid_daemon;
            ifs >> pid_daemon;
            ifs.close();
            m_pid_daemon = ::atoi(pid_daemon.c_str());
        }
   
    }
    
    
    /**
     * Purpose:
     * <p> [ Destructor description if necessary]
     * 
     */ 
    
    killer_impl::~killer_impl()
        throw()
    {
    }
    
    
    void killer_impl::init(observer_ptr obs)
        throw(CORBA::SystemException)
    {
        m_observer = observer::_duplicate(obs);
    }
    

    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    void  killer_impl::kill_daemon()
        throw(CORBA::SystemException)
    {
        std::cout<<"kill_daemon()"<<std::endl;
        Cdmw::OsSupport::OS::Timeval time_before_kill;
        //m_observer->kill_daemon();
        time_before_kill = Cdmw::OsSupport::OS::get_time();
        Cdmw::OsSupport::OS::kill_process(m_pid_daemon);
        //m_observer->kill_daemon_time ( time_before_kill.seconds, time_before_kill.microseconds);
    }

    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    void  killer_impl::start_daemon()
        throw(CORBA::SystemException)
    {   
        std::cout<<"start_daemon()"<<std::endl;
        m_pid_daemon = Cdmw::OsSupport::OS::create_process( "cdmw_platform_daemon.sh" , "--CdmwXMLFile=CdmwPlatformMngtDaemon_conf.xml"); 
    }
    
    
    /**
     * Purpose:
     * <p>
     * 
     *@param
     *@return
     *@exception
     */ 
    void  killer_impl::auto_kill()
        throw(CORBA::SystemException)
    {
        std::cout<<"end of the killer"<<std::endl;
        Cdmw::OsSupport::OS::kill_process(m_pid_daemon);
        ::exit(1);
    }
}; // End namespace Test

}; // End namespace Cdmw

