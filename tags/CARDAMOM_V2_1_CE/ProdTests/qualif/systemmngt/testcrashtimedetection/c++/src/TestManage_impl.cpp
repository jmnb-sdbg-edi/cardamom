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

#include "testcrashtimedetection/TestManage_impl.hpp"
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
        : m_inc(0)
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
     * get_time_detection
     */ 
    CdmwPlatformMngtBase::TimeStamp observer_impl::get_time_detection()
        throw(CORBA::SystemException)
    {
        return m_localtimebefore[m_inc];
    }
    
    
    /**
     * get_time_notification
     */
    CdmwPlatformMngtBase::TimeStamp observer_impl::get_time_notification()
        throw(CORBA::SystemException)
    {
        return m_localtimeafter[m_inc];
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
            case CdmwPlatformMngt::PROCESS_STATUS_CHANGE:
                {
                    CdmwPlatformMngt::ProcessStatusChange* event = dynamic_cast< CdmwPlatformMngt::ProcessStatusChange * > (an_event);
                    if ((event->process_status() == CdmwPlatformMngt::PROCESS_FAILED_INVALID) ||
                        (event->process_status() == CdmwPlatformMngt::PROCESS_FAILED_DEATH) ||
                        (event->process_status() == CdmwPlatformMngt::PROCESS_FAILED_ERROR))
                    {
                        m_inc++;
                        //take the notification time
                        m_localtimebefore[m_inc] = event->header().time_stamp;
                        m_localtimeafter[m_inc] = time;
                        std::cout<<"The process is failed"<<std::endl;
                    } else {
                        std::cout<<"The process is alived"<<std::endl;
                    }
                    break;
                }
                
            default:
                {
                }
        }
        
    }

}; // End namespace Test

}; // End namespace Cdmw

