/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include "testplatformdaemon/SIM_PlatformObserver.hpp"
#include "Foundation/ossupport/OS.hpp"

namespace Cdmw
{
namespace PlatformMngt
{

/**
*Implementation notes: none 
*<p>
*Portability issues: none 
*<p>
*/


//
// Ctor
//
SIM_PlatformObserver::SIM_PlatformObserver(const char* logFile)
    : m_fs(logFile, std::ios::out)
{

}


//
//
//
void SIM_PlatformObserver::stop()
{
    m_fs.close();
}


//
// Dtor
//
SIM_PlatformObserver::~SIM_PlatformObserver()
{

}



//
//
//
void SIM_PlatformObserver::notify(const CdmwPlatformMngt::PlatformInfo& platform_info)
    throw(CORBA::SystemException)
{

    m_mutex.lock();

    m_fs << "Process \"" << platform_info.process_name.in() << "\" ";

    if (platform_info.process_status == CdmwPlatformMngt::STARTED)
        m_fs << "started" << std::endl;
    else
        m_fs << "ended" << std::endl;


    m_mutex.unlock();

}


} // End namespace PlatformMngt
} // End namespace Cdmw


