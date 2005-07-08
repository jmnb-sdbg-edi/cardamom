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

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/ossupport/OS.hpp>


void ClientThread::run() throw()
{
 
    // call hello() operation 20 times with an interval of 3 seconds
    for (int i=0; i<20 && !m_stopped; i++) {
        
        std::cout << "   =======> send hello number " << i+1 << std::endl;
        
        // If the server stop during the hello() operation,
        // the client will receive a COMM_FAILURE SystemException.
        // In this case, he has to retry the call.
        bool send_ok = false;
        int nb_try = 3;   // 3 tries
        while (!send_ok && nb_try-- > 0) {
            try {
                m_hello->hello();
                send_ok = true;
            } catch (const CORBA::SystemException& e) {
                std::cout << "       ===> SystemException received: " 
                          << Cdmw::OrbSupport::OrbSupport::exception_to_string(e)
                          << std::endl;
                if (nb_try > 0) {
                    std::cout << "       ===> Try again !!" << std::endl;
                }
            }
        }
        
        if (send_ok) {            
            std::cout << "   <======= send hello " << i+1 << " OK" << std::endl;
        } else {
            std::cout << "   <======= send hello " << i+1 << " aborted !!!" << std::endl;
        }

        // wait 3 seconds    
        Cdmw::OsSupport::OS::sleep(3000);
        
    }
    
}




