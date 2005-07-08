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


#include "test_perf/ClientThread.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/naminginterface/NamingUtil.hpp"
#include "Foundation/testutils/Testable.hpp"
#include <string>
#include <cctype>
#include <algorithm>

#include "Foundation/common/System.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"

#include "test_perf/TestEventChannel.hpp"
#include "test_perf/TestEventChannelContinue.hpp"
#include "test_perf/TestTypedEventChannel.hpp"

namespace
{


}; // End anonymous namespace

namespace Cdmw
{
namespace Event
{

//----------------------------------------------------------------------------
ClientThread::ClientThread(CORBA::ORB_ptr orb, const char* actualAgentIOR) 
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_actualAgentIOR(actualAgentIOR)
{
}

//----------------------------------------------------------------------------
ClientThread::~ClientThread() throw()
{
}

//----------------------------------------------------------------------------
void ClientThread::run() throw()
{
    using namespace Cdmw::Common;
    using namespace Cdmw::NamingAndRepository;
    using namespace Cdmw::NamingAndRepository;
    using namespace Cdmw::OsSupport;
    
    
    // Testing TestEventChannel
    TestEventChannel testEventChannel(m_orb.in());
    testEventChannel.start();

    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OS::sleep(timescale*5000);

    TestEventChannelContinue testEventChannelContinue(m_orb.in());
    testEventChannelContinue.start();

    OS::sleep(timescale*5000);

#if CDMW_ORB_VDR == orbacus        
    // Testing TestTypedEventChannel
    TestTypedEventChannel testTypedEventChannel(m_orb.in());
    testTypedEventChannel.start();
#endif

     m_orb->shutdown(false);
    
}

}; // namespace Event
}; // namespace Cdmw


