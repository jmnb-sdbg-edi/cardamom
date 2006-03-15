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

#ifndef INCL_TESTNOTIFICATIONRECEPTION_HREAD_HPP
#define INCL_TESTNOTIFICATIONRECEPTION_HREAD_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/osthreads/Thread.hpp"
#include <testnotificationreception/TestNotificationReception.hpp>

class TestNotificationReceptionThread : public Cdmw::OsSupport::Thread
{

public:
        
    TestNotificationReceptionThread(CORBA::ORB_ptr orb,
                     std::string name,
                     std::string function)
        :  m_orb( CORBA::ORB::_duplicate( orb ) ),
           m_name(name),
           m_function(function)
           
    {
    }
    
    virtual ~TestNotificationReceptionThread()
        throw()
    {
    }

protected:

    void run() throw()
    {
        // Do test
        TestSupervision::TestNotificationReception test (m_orb.in(),
                                                         m_name,
                                                         m_function);


        test.start();
    }
        
private:

    CORBA::ORB_var m_orb;
    std::string m_name;
    std::string m_function;

};

#endif //INCL_TESTNOTIFICATIONRECEPTION_HREAD_HPP

