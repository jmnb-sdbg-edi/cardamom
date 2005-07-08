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

#ifndef INCL_LIFECYCLE_COMMAND_THREAD_HPP
#define INCL_LIFECYCLE_COMMAND_THREAD_HPP

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/osthreads/ThreadHandle.hpp"
#include <string>

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
class CommandThread : public Cdmw::OsSupport::Thread 
{
public:
    CommandThread(CORBA::ORB_ptr     orb,
                 const std::string & proc_url,
                 unsigned int        wait_duration,
                 unsigned int        processing_duration)
        throw (CORBA::SystemException);

    ~CommandThread() throw();
protected:
    void
    run() throw();
private:
    CORBA::ORB_var m_orb;
    std::string    m_process_url;
    unsigned int   m_wait_duration;
    unsigned int   m_processing_duration;
}; // End of class CommandThread


}; // End namespace LifeCycle
}; // End namespace Cdmw

#endif // INCL_LIFECYCLE_COMMAND_THREAD_HPP

