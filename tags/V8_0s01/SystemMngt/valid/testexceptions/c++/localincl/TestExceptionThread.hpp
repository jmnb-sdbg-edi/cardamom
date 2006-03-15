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

#ifndef INCL_PERFDAEMON_THREAD_HPP
#define INCL_PERFDAEMON_HREAD_HPP

#include <fstream>
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "Foundation/testutils/CORBATestManager.hpp"
#include "testexceptions/TestException.hpp"



CORBA::ORB_var Cdmw::TestException::m_orb;
std::string Cdmw::TestException::m_daemon;
std::string Cdmw::TestException::m_system;

class TestExceptionThread : public Cdmw::OsSupport::Thread
{

public:
        
    TestExceptionThread(CORBA::ORB_ptr orb,
                        std::string daemon,
                        std::string system)
           
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_daemon = daemon;
        m_system = system;
    }
    
    virtual ~TestExceptionThread()
        throw()
    {
    }

protected:

    void run() throw()
    {
        //Cdmw::TestException test();
        Cdmw::TestException::m_orb = CORBA::ORB::_duplicate(m_orb.in());
        Cdmw::TestException::m_daemon = m_daemon;
        Cdmw::TestException::m_system = m_system;

      Cdmw::TestUtils::CORBATestManager::instance()->run_tests();
      Cdmw::TestUtils::CORBATestManager::instance()->summary();
    }
        
private:

    CORBA::ORB_var m_orb;

    std::string m_daemon;

    std::string m_system;
};

#endif //INCL_PERFDAEMON_THREAD_HPP

