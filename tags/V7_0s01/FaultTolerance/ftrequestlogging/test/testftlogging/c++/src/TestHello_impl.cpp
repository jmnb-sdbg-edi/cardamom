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

#include "testftlogging/TestHello_impl.hpp"
#include <iostream>
#include <sstream>

using namespace std;

namespace Cdmw
{


HelloInterface_impl::HelloInterface_impl(CORBA::ORB_ptr orb,
                                         PortableServer::POA_ptr poa)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa))
{
    const Cdmw::CommonSvcs::DataStore::DsIdentifier DS_ID = 1;

    Cdmw::CommonSvcs::DataStore::DataStoreBase* ds
        = Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(DS_ID);
    m_store = 
        dynamic_cast<Cdmw::FT::RequestLogging::ReplyRecording::ReplyStore*>(ds);
}

HelloInterface_impl::~HelloInterface_impl()
    throw()
{
}

void HelloInterface_impl::hello ()
     throw(CORBA::SystemException)                  
{
    std::cout << "<####### HELLO 1 ######>" << std::endl;
}

CORBA::Long HelloInterface_impl::do_something (CORBA::Long val)
    throw(CORBA::SystemException)
{
    return val * val;
}

char * HelloInterface_impl::do_something_more (char *& val, 
                                               ::CdmwRequestLogging::MyStruct_out val_struct)
    throw(CORBA::SystemException)
{
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
    val_struct = sval._retn();

    return result;
}

CORBA::Long HelloInterface_impl::get_log_size ()
    throw(CORBA::SystemException)
{
    return m_store->size();
}


}; // End namespace Cdmw

