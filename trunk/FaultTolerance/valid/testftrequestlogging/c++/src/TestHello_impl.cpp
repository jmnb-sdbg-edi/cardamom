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

#include "testftrequestlogging/TestHello_impl.hpp"
#include "testftrequestlogging/TestObserver.hpp"
#include "ftcommon/ReplyRecording.hpp"

#include <Foundation/osthreads/MutexGuard.hpp>


using namespace std;

namespace Cdmw
{


HelloInterface_impl::HelloInterface_impl(CORBA::ORB_ptr orb,
                                         PortableServer::POA_ptr poa)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa))
{

    Cdmw::CommonSvcs::DataStore::DataStoreBase* ds
        = Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
              Cdmw::FT::FT_REPLY_DATASTORE_ID);
    m_store = 
        dynamic_cast<Cdmw::FT::RequestLogging::ReplyRecording::ReplyStore*>(ds);

    m_observer = new Observer_impl(m_orb.in(), *this);
    m_store->register_observer(m_observer);

    m_recorded_durations = new CdmwRequestLogging::TimeSeq;
    m_hostname = OsSupport::OS::get_hostname();
}

HelloInterface_impl::~HelloInterface_impl()
    throw()
{
   delete m_observer;
}

void HelloInterface_impl::hello ()
     throw(CORBA::SystemException)                  
{
    std::cout << "<####### HELLO 1 ######>" << std::endl;
}

CORBA::Long HelloInterface_impl::do_something (CORBA::Long val)
    throw(CORBA::SystemException)
{
    std::cout << "do_something return square of " << val << std::endl;
    return val * val;
}

char * HelloInterface_impl::do_something_more (char *& val, 
                                               ::CdmwRequestLogging::MyStruct_out val_struct)
    throw(CORBA::SystemException)
{
    std::cout << "do_something_more entering..." << std::endl;
    char* result = CORBA::string_dup(val);
    char* rev = CORBA::string_dup(val);

    for (CORBA::ULong i = 0; i < strlen(rev) / 2; i++)
    {
        char tmp = rev[i];
        rev[i] = rev[strlen(rev) - i - 1];
        rev[strlen(rev) - i - 1] = tmp;
    }

    val = rev;
    ::CdmwRequestLogging::MyStruct_var sval = new ::CdmwRequestLogging::MyStruct();
     
    std::ostringstream ostr;
    ostr << rev << "|" << result;
    sval->val = CORBA::string_dup(ostr.str().c_str());
    sval->hostname = CORBA::string_dup(m_hostname.c_str());
    val_struct = sval._retn();
    std::cout << m_hostname
              << " do_something_more return " << result 
              << std::endl;

    return result;
}

CORBA::Long HelloInterface_impl::get_log_size ()
    throw(CORBA::SystemException)
{
    return m_store->size();
}

CdmwRequestLogging::TimeSeq*
HelloInterface_impl::get_recorded_durations ()
            throw(CORBA::SystemException)
{
   OsSupport::MutexGuard guard(m_mutex);

   return m_recorded_durations._retn();
}

bool 
HelloInterface_impl::send_bad_context_exception ()
            throw(CORBA::SystemException)
{
   return false;
}


void
HelloInterface_impl::set_recorded_duration (
      CORBA::ULong duration)
{
   OsSupport::MutexGuard guard(m_mutex);

   CORBA::ULong index = m_recorded_durations->length();
   m_recorded_durations->length(index+1);
   m_recorded_durations[index] = duration;
}



}; // End namespace Cdmw

