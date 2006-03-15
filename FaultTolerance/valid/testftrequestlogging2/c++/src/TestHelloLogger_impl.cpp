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
#include <iostream>
#include <sstream>

#include "testftrequestlogging2/TestHelloLogger_impl.hpp"

#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"



using namespace std;

namespace Cdmw
{


HelloLogger_impl::HelloLogger_impl(CORBA::ORB_ptr orb,
                                   PortableServer::POA_ptr poa)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa))
{
    m_log_traces = new CdmwRequestLogging::LogSeq();
}

HelloLogger_impl::~HelloLogger_impl()
    throw()
{
}


CdmwRequestLogging::LogSeq*
HelloLogger_impl::get_log_traces ()
   throw(CORBA::SystemException)
{
   OsSupport::MutexGuard guard(m_mutex);
   return m_log_traces._retn();
}



void
HelloLogger_impl::set_log_trace (const char* trace)
   throw(CORBA::SystemException)
{
   OsSupport::MutexGuard guard(m_mutex);

   CORBA::ULong index = m_log_traces->length();
   m_log_traces->length(index+1);
   m_log_traces[index] = CORBA::string_dup(trace);
}



}; // End namespace Cdmw

