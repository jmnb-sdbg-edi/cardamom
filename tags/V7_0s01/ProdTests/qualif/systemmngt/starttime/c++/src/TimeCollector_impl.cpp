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

#include "starttime/TimeCollector_impl.hpp"
#include <iostream>

namespace Cdmw {
    

    
    
    TimeCollector_impl::TimeCollector_impl(CORBA::ORB_ptr orb, int nb_processes)
        : m_orb(CORBA::ORB::_duplicate(orb)),
          m_nb_processes(nb_processes)
    {
        m_max_time.seconds = 0;
        m_max_time.microseconds = 0;
        m_current_nb_processes = 0;
    }
    
    TimeCollector_impl::~TimeCollector_impl()
    {
        std::cout<<"~TimeCollector_impl"<<std::endl;
        
    }
    
    void TimeCollector_impl::set_time_creation (::CORBA::Long second,
                                                ::CORBA::Long micro)
        throw (CORBA::SystemException)
    {
        Cdmw::OsSupport::OS::Timeval run_timeval = Cdmw::OsSupport::OS::get_time();
        std::cout<<"set_time_creation"<<std::endl;
        Cdmw::OsSupport::OS::Timeval time;
        time.seconds = second;
        time.microseconds = micro;
        
        if (run_timeval > m_max_time)
            m_max_time = run_timeval;

        std::cout<<"max_time    "<<m_max_time.seconds<<"."<<m_max_time.microseconds<<std::endl;
        std::cout<<"run_timeval " <<run_timeval.seconds<<"."<<run_timeval.microseconds<<std::endl;
        m_current_nb_processes++;
    }
    
    CORBA::Boolean TimeCollector_impl::all_process_are_started ()
        throw (CORBA::SystemException)
    {
        std::cout<<"all_process_are_started "<<m_current_nb_processes <<" "<<m_nb_processes<<std::endl;
        if (m_current_nb_processes < m_nb_processes)
            return false;
        else
            return true;
    }
    
    
    ::Cdmw::Test::time TimeCollector_impl::get_max_time ()
         throw(CORBA::SystemException)
    {
        std::cout<<"get_max_time"<<std::endl;
        Cdmw::Test::time timestruct;
        timestruct.seconds = m_max_time.seconds;
        timestruct.microseconds = m_max_time.microseconds;
        
        return (timestruct);
    

    }
    
};
