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

#include <ClientThread.hpp>

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/ossupport/OS.hpp"

namespace Hello
{

void ClientThread::run() throw()
{
 
    // call hello() operation 20 times with an interval of 2 seconds
    for (int i=0; i<20 && !m_stopped; i++) {
        
        std::cout << "   =======> sending hello number " << i+1 << "..." << std::flush;
        try {
            m_display->display_hello();
            std::cout << " done." << std::endl;
        } catch (const CORBA::SystemException& e) {
            std::cout << "\nFailed to send " << i+1 << "\n"
                      << "===> SystemException received: " 
                      << Cdmw::OrbSupport::OrbSupport::exception_to_string(e)
                      << std::endl;
        }
        // wait 2 seconds    
        std::cout << "   =======> waiting 2 seconds for next message." << std::endl;
        Cdmw::OsSupport::OS::sleep(2000);
    }
}

}; // End of namespace Hello
