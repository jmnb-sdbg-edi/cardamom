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

#include "lifecycleclient/MyProcessControl.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"


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
MyProcessControl::MyProcessControl(CORBA::ORB_ptr      orb,
                                   std::istream &      is,
                                   std::ostream &      os)
    throw(CORBA::SystemException)
    : m_orb (CORBA::ORB::_duplicate(orb)),
      m_is (is),
      m_os (os),
      m_client_thread (0)
{
}

MyProcessControl::~MyProcessControl() 
  throw()
{
    if (m_client_thread != 0) 
	this->on_stop();
}
    
void 
MyProcessControl::on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
    throw(CORBA::SystemException)
{
    // This call is legal since Cdmw services are initialised before calling
    // this operation.
    CdmwNamingAndRepository::Repository_var repository
        = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
    m_client_thread 
        = new Cdmw::LifeCycle::ClientThread(m_orb.in(),repository.in(),
                                             m_is,m_os);
}
    
void 
MyProcessControl::on_run()
    throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun, 
          CORBA::SystemException)
{
    if (m_client_thread) 
	    m_client_thread->start();
}

void 
MyProcessControl::on_stop()
    throw(CORBA::SystemException)
{
    if (m_client_thread != 0) {
	// First signal termination to client thread if it was at least requested to run
        // enum ThreadStatus {THREAD_CREATED,THREAD_RUN_REQUEST,THREAD_RUNNING,THREAD_ENDED};
        using namespace Cdmw::OsSupport;
        ThreadStatus status = m_client_thread -> get_status();
        if ((status == THREAD_RUN_REQUEST) ||
            (status == THREAD_RUNNING)) {
            m_client_thread -> stop();
            m_client_thread -> join();
        }
	delete m_client_thread;
	m_client_thread = 0;
    }
}


}; // End namespace LifeCycle
}; // End namespace Cdmw

