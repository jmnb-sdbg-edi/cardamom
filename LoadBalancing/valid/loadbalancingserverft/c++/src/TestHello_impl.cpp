/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
 * @file
 * @brief IDL Hello Interface implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

// Cdmw Files
#include <loadbalancingserverft/TestHello_impl.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>


namespace Cdmw
{

    HelloInterface_impl::HelloInterface_impl(std::string location)
        throw(CORBA::SystemException)
        :m_location(location)
    {
        m_process_info = new Cdmw::Test::process_info();
        m_process_info->name = location.c_str();
        m_process_info->processID = Cdmw::OsSupport::OS::get_processId();
    }
    

    HelloInterface_impl::~HelloInterface_impl()
        throw()
    {
    }

    void HelloInterface_impl::hello ()
        throw(CORBA::SystemException)
    {
        std::cout << m_process_info->name << " : "  << m_process_info->processID  << " : "  <<"<####### HELLO ######>" << std::endl;
    }
    
    Cdmw::Test::process_info* HelloInterface_impl::who_is_called()
        throw(CORBA::SystemException)
    {
        std::cout<< "*****************************************************************"<<std::endl;
        std::cout<<m_process_info->name <<" is called"<<std::endl;
        std::cout<< "*****************************************************************"<<std::endl;

        Cdmw::Test::process_info_var process_info;
        Cdmw::Test::process_info info;
        info.name = m_process_info->name;
        info.processID = m_process_info->processID;
        process_info = new Cdmw::Test::process_info(info);

        return process_info._retn();


    }
    
};
