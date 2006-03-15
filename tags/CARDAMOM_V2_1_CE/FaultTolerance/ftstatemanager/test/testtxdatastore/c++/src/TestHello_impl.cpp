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


#include "testtxdatastore/TestHello_impl.hpp"
#include "ftstatemanager/DataStoreGroup_impl.hpp"

namespace Cdmw
{

HelloInterface_impl::HelloInterface_impl(CORBA::ORB_ptr orb,PortableServer::POA_ptr poa)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa))
{ 
    typedef Cdmw::FT::DataStore<int, std::string> TestDataStore;
    Cdmw::OsSupport::OS::Timeval cohort_timeout;
    cohort_timeout.seconds=10;
    cohort_timeout.microseconds=0;
    Cdmw::OsSupport::OS::Timeval coordinator_timeout;
    coordinator_timeout.seconds=10;
    coordinator_timeout.microseconds=0;

    m_context = new Cdmw::FT::DataStoreContext(m_orb.in(),
                                               m_poa.in(),
                                               100, 100,
                                               cohort_timeout,
                                               coordinator_timeout,
                                               1);

    datastore_group =
        (new Cdmw::FT::DataStoreGroup_impl(m_poa.in(), *m_context))->_this();

    m_manager = new Cdmw::FT::TXManager(*m_context);

    dataStoreObj1 = new MyTestDataStore(*m_manager,
                                        1,
                                        m_orb.in(),
                                        m_poa.in());

    dataStoreObj2 = new MyTestDataStore(*m_manager,
                                        2,
                                        m_orb.in(),
                                        m_poa.in());
    
}

HelloInterface_impl::~HelloInterface_impl()
    throw()
{
}

void HelloInterface_impl::hello ()
     throw(CORBA::SystemException)                  
{
    std::cout << "<####### HELLO ######>" << std::endl;
}

void HelloInterface_impl::set_info_into_datastore(CORBA::Long dsid, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
    try
    {
        TestStorageHome myStorageHome(dsid);
        myStorageHome.create(oid, data);
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        TestStorageHome myStorageHome(dsid);
        TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(oid);
        myStorageObject2Cpy.set(data);
    }
}

char * HelloInterface_impl::get_info_from_datastore(CORBA::Long dsid, CORBA::Long oid)
    throw(CORBA::SystemException)
{
    try
    {
        TestStorageHome myStorageHome(dsid);
        TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(oid);
        
        CORBA::String_var data = CORBA::string_dup(myStorageObject2Cpy.get().c_str());
        return data._retn();
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        throw;
    }
    return NULL;
}


}; // End namespace Cdmw

