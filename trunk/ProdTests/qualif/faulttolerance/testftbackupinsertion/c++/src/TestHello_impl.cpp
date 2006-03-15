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


#include "testftbackupinsertion/TestHello_impl.hpp"

#include <iostream>

using namespace std;

namespace Cdmw
{

HelloInterface_impl::HelloInterface_impl(
                              CORBA::ORB_ptr          orb
                            , PortableServer::POA_ptr poa
                            , const char*             process)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
    , m_poa(PortableServer::POA::_duplicate(poa))
    , m_process(process)
{
    Cdmw::FT::DataStoreBase* dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(1);
    dataStoreObj1 = dynamic_cast<Cdmw::FT::DataStore<CORBA::Long, std::string, std::less<CORBA::Long> >*>(dsBase); 
}

HelloInterface_impl::~HelloInterface_impl()
    throw()
{
}

void
HelloInterface_impl::insert(CORBA::Long dsid, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
    // OK, execute the insert.
    try {
        TestStorageHome myStorageHome(dsid);
        myStorageHome.create(oid, data);
    }
    catch (const Cdmw::FT::AlreadyExistException&) {
        TestStorageHome myStorageHome(dsid);
        TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(oid);
        myStorageObject2Cpy.set(data);
    }
}

void
HelloInterface_impl::update(CORBA::Long dsid, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
//#warning "!!!! TODO: HelloInterface_impl::update"
}

void
HelloInterface_impl::remove(CORBA::Long dsid, CORBA::Long oid)
    throw(CORBA::SystemException)
{
//#warning "!!!! TODO: HelloInterface_impl::remove"
}


char * 
HelloInterface_impl::get_data(CORBA::Long dsid, CORBA::Long oid)
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
        cout << "[---- " << m_process << " HelloInterface_impl::get_data] DSID "<<dsid<<", OID "<<oid<<" throw BAD_PARAM"<<std::endl;
        throw CORBA::BAD_PARAM(0, CORBA::COMPLETED_YES);
    }
    return NULL;
}

}; // End namespace Cdmw

