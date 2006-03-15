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


#include "testftlocationmanager3/TestHello_impl.hpp"
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreGroup_impl.hpp>

using namespace std;

namespace Cdmw
{

HelloInterface_impl::HelloInterface_impl(
                              CORBA::ORB_ptr          orb
                            , PortableServer::POA_ptr poa
                            , const char*             process
                            , CORBA::Long             target_step)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
    , m_poa(PortableServer::POA::_duplicate(poa))
    , m_test_done(false)
    , m_process(process)
    , m_target_step(target_step)
{ 


    Cdmw::OsSupport::OS::Timeval cohort_timeout;
    cohort_timeout.seconds=2;
    cohort_timeout.microseconds=0;
    Cdmw::OsSupport::OS::Timeval coordinator_timeout;
    coordinator_timeout.seconds=2;
    coordinator_timeout.microseconds=0;

    context = new Cdmw::FT::DataStoreContext(m_orb.in(),
                                             m_poa.in(),
                                             100, 100,
                                             cohort_timeout,
                                             coordinator_timeout,
                                             1);

    datastore_group =
        (new Cdmw::FT::DataStoreGroup_impl(m_poa.in(),*context))->_this();

    dataStoreObj1 = new TestDataStore(1,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    
    onInitObj1 = new TestOnInitImpl();

    dataStoreObj1->register_initialiser(onInitObj1);
    
    dataStoreObj2 = new TestDataStore(2,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    
    onInitObj2 = new TestOnInitImpl();

    dataStoreObj2->register_initialiser(onInitObj2);
    

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
        try
        {
            TestStorageHome myStorageHome(dsid);
            TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(oid);
            myStorageObject2Cpy.set(data);
        }
        catch(const Cdmw::FT::NotFoundException&)
        {
            std::cout<<"---------------------->  insert throw Cdmw::FT::NotFoundException"<<std::endl;
        }
        
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


CORBA::Boolean HelloInterface_impl::datastore_is_activated (CORBA::Long dsid)
    throw(CORBA::SystemException)
{
    if (dsid == 1)
        return dataStoreObj1->is_activated();
    else
        return dataStoreObj2->is_activated();
}

::CdmwReplicationManager::LocationList * HelloInterface_impl::get_location_list (CORBA::Long dsid)
    throw(CORBA::SystemException)
{
    Cdmw::FT::DataStoreContext::LocationList list = context->get_locations(dsid);
    ::CdmwReplicationManager::LocationList * seqlist = new ::CdmwReplicationManager::LocationList;
    seqlist->length(list.size());
    int i = 0;
    for (Cdmw::FT::DataStoreContext::LocationList::iterator iter = list.begin();
         iter != list.end();
         iter++)
    {
        (*seqlist)[i] = (*iter).c_str();
        i++;
    }
    return seqlist;
}

::CdmwFT::StateTransfer::LocalDataStoreInfo *
HelloInterface_impl::get_local_datastore_info ()
    throw(CORBA::SystemException)
{
    ::CdmwFT::StateTransfer::LocalDataStoreInfo* info = new  ::CdmwFT::StateTransfer::LocalDataStoreInfo;

    info->dsids.length(2);
    info->dsids[0] = dataStoreObj1->get_dsid();
    info->dsids[1] = dataStoreObj2->get_dsid();

    info->coordinator = dataStoreObj1->get_context().get_coordinator_ref();
    info->cohort = dataStoreObj1->get_context().get_cohort_ref();
    info->local_data_store = CdmwFT::StateTransfer::DataStoreGroup::_duplicate(datastore_group.in());

    return info;
}
    

void 
HelloInterface_impl::set_info_into_datastore(CORBA::Long dsid, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
    try
    {
        TestStorageHome myStorageHome(dsid);
        myStorageHome.create(oid, data);
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        try
        {
            TestStorageHome myStorageHome(dsid);
            TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(oid);
            myStorageObject2Cpy.set(data);
        }
        catch(const Cdmw::FT::NotFoundException&)
        {
            std::cout<<"---------------------->  insert throw Cdmw::FT::NotFoundException"<<std::endl;
        }
    }
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
        cout << "[**** " << m_process << " HelloInterface_impl::get_data] DSID "<<dsid<<", OID "<<oid<<" throw BAD_PARAM"<<std::endl;
        throw CORBA::BAD_PARAM(0, CORBA::COMPLETED_YES);
    }
    return NULL;
}

}; // End namespace Cdmw

