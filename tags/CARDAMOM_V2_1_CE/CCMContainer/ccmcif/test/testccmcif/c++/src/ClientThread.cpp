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


#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/testutils/Testable.hpp"
#include "testccmcif/ClientThread.hpp"
#include "testccmcif/TestBasicDemo.hpp"
#include "testccmcif/TestTypedBasicDemo.hpp"

namespace
{

}; // End anonymous namespace

namespace Cdmw
{
namespace CCM
{
namespace CIF
{

//----------------------------------------------------------------------------
ClientThread::ClientThread(CORBA::ORB_ptr orb, const char* containerIOR) 
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_containerIOR(containerIOR)
{
}

//----------------------------------------------------------------------------
ClientThread::~ClientThread() throw()
{
}

//----------------------------------------------------------------------------
void ClientThread::run() throw()
{
    using namespace Cdmw::OsSupport;
    
    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OS::sleep(timescale*3000);
    // Testing TestEventChannel
    TestBasicDemo test_basic_demo(m_orb.in(), m_containerIOR.c_str());
    test_basic_demo.start();

    OS::sleep(timescale*3000);
    
    TestTypedBasicDemo test_typed_basic_demo(m_orb.in(), m_containerIOR.c_str());
    test_typed_basic_demo.start();
    m_orb->shutdown(false);

}

}; // namespace CIF
}; // namespace CCM
}; // namespace Cdmw

