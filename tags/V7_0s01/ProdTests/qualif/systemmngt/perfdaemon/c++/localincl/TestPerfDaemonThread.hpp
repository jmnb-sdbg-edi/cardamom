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

#ifndef INCL_PERFDAEMON_THREAD_HPP
#define INCL_PERFDAEMON_HREAD_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include "perfdaemon/TestPerfDaemon.hpp"


class TestPerfDaemonThread : public Cdmw::OsSupport::Thread
{

public:
        
    TestPerfDaemonThread(
        CORBA::ORB_ptr orb,
        PortableServer::POA_ptr rootPOA,
        const std::string& name,
        const std::string& nbtest,
        const std::string& supervision,
        const std::string& limittime,
        const std::string& percentcase )
           
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_poa = PortableServer::POA::_duplicate(rootPOA);
        m_name = name;
        m_nbtest = nbtest;
        m_supervision = supervision;
        m_limittime = limittime;
        m_percentcase = percentcase;

    }
    
    virtual ~TestPerfDaemonThread()
        throw()
    {
    }

protected:

    void run() throw()
    {


        Cdmw::TestPerfDaemon test(m_orb.in(),
                                  m_poa.in(),
                                  m_name,
                                  m_nbtest,
                                  m_supervision,
                                  m_limittime,
                                  m_percentcase);
        test.start();
    }
        
private:

    CORBA::ORB_var m_orb;

    PortableServer::POA_var m_poa;
    
    std::string m_name;

    std::string m_nbtest;

    std::string m_supervision;

    std::string m_limittime;
    
    std::string m_percentcase;


};

#endif //INCL_PERFDAEMON_THREAD_HPP

