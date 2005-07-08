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

#include "Foundation/orbsupport/CORBA.hpp"
#include "lifecycle/MacroDefs.hpp"
#include "lifecycle/CommandThread.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtProcessCallback.stub.hpp"
#include "Foundation/ossupport/OS.hpp"
#include "SystemMngt/tools/ProcessAdmin.hpp"
#include "Foundation/testutils/Testable.hpp"
#include <iostream>
#include <sstream>

namespace Cdmw
{
namespace LifeCycle
{

/**
*Purpose:
*<p>    [class description]
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/
// Thread for sending platform management commands
// during unit tests
CommandThread::CommandThread(CORBA::ORB_ptr     orb,
                             const std::string & proc_url,
                             unsigned int        wait_duration,
                             unsigned int        processing_duration)
    throw (CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_process_url(proc_url),
      m_wait_duration(wait_duration),
      m_processing_duration(processing_duration)
{
}

CommandThread::~CommandThread() throw()
{
}

void
CommandThread::run() throw()
{
    using namespace Cdmw::OsSupport;
    // Wait for m_wait_duration before calling initialise
    // ECHO_DEF << "Waiting for " << m_wait_duration << " ms." << std::endl;
	int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OS::sleep(timescale*m_wait_duration);
    try {
        CdmwPlatformMngt::Process_var proc = CdmwPlatformMngt::Process::_nil();
        CORBA::Object_var obj = m_orb->string_to_object(m_process_url.c_str());
        if (!CORBA::is_nil(obj.in())) {
            proc = CdmwPlatformMngt::Process::_narrow(obj.in());
            Cdmw::Tools::ProcessAdmin admin(m_orb.in(),proc.in());
            // send initialise order
            {
                std::string commands("verbose \n init LAST 2\n exit \n");
                std::istringstream commands_is(commands);
                // ECHO_DEF << "Calling 'init LAST 2'." << std::endl;
                admin.run(commands_is,std::cout);
            }
            // send run order
            {
                std::string commands("verbose \n run\n exit \n");
                std::istringstream commands_is(commands);
                // ECHO_DEF << "Calling 'run'." << std::endl;
                admin.run(commands_is,std::cout);
            }
            // Wait for m_processing_duration before calling initialise
            // ECHO_DEF << "Waiting for " << m_processing_duration << " ms." << std::endl;
            OS::sleep(timescale*m_processing_duration);
            // send stop order
            {
                std::string commands("verbose \n stop\n exit \n");
                std::istringstream commands_is(commands);
                // ECHO_DEF << "Calling 'stop'." << std::endl;
                admin.run(commands_is,std::cout);
            }
        }
    } catch (...) {
        std::cerr << "Exception raised within CommandThread!" << std::endl;
    }
};


}; // End namespace LifeCycle
}; // End namespace Cdmw

