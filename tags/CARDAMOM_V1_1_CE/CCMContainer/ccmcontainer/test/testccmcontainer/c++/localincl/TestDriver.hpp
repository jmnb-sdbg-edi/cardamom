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


#ifndef CDMW_CCM_CONTAINER_TEST_DRIVER_H
#define CDMW_CCM_CONTAINER_TEST_DRIVER_H

#include <Foundation/osthreads/ThreadHandle.hpp>
#include <Foundation/orbsupport/CORBA.hpp>

namespace TestCdmwCCM
{
    class TestDriver : public Cdmw::OsSupport::Thread
    {
    public:
        TestDriver(CORBA::ORB_ptr          orb,
                   PortableServer::POA_ptr poa,
                   std::string process_name)
            throw (CORBA::SystemException)
            : m_orb(CORBA::ORB::_duplicate(orb)),
              m_poa(PortableServer::POA::_duplicate(poa)),
              m_process_name(process_name)
        {
        }
      
        ~TestDriver() throw() {};

    protected:
        virtual void run() throw();
    private:
        CORBA::ORB_var          m_orb;
        PortableServer::POA_var m_poa;
        std::string             m_process_name;
    };
}; // End namespace TestCdmwCCM



#endif // CDMW_CCM_CONTAINER_TEST_DRIVER_H

