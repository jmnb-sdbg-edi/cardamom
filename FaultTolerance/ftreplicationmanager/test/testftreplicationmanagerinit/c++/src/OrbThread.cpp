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

#include <iostream>

#include <testftreplicationmanagerinit/OrbThread.hpp>


namespace Cdmw
{

namespace FT
{

namespace FTInit
{

        
OrbThread::OrbThread(CORBA::ORB_ptr orb, std::string host_name)
   : m_orb(CORBA::ORB::_duplicate(orb)),
     m_host_name(host_name)
{    
}


OrbThread::~OrbThread() 
throw() 
{
} 


void
OrbThread::shutdown() 
{
    m_orb->shutdown(true);
    m_orb->destroy();
}

void
OrbThread::run() 
    throw() 
{
    std::cout << m_host_name << " test_server: running..."<<std::endl;
    m_orb->run();   
}

 	
} // end of namespace FT

} // end of namespace FTInit

} // end of namespace Cdmw


