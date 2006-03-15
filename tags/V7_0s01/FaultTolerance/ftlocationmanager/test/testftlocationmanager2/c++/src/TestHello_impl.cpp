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


#include "testftlocationmanager2/TestHello_impl.hpp"

namespace Cdmw
{


HelloInterface1_impl::HelloInterface1_impl(CORBA::ORB_ptr orb,PortableServer::POA_ptr poa)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa))
{ 


    Cdmw::OsSupport::OS::Timeval cohort_timeout;
    cohort_timeout.seconds=10;
    cohort_timeout.microseconds=0;
    Cdmw::OsSupport::OS::Timeval coordinator_timeout;
    coordinator_timeout.seconds=10;
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

HelloInterface1_impl::~HelloInterface1_impl()
    throw()
{
}

void HelloInterface1_impl::hello ()
     throw(CORBA::SystemException)                  
{
    std::cout << "<####### HELLO 1 ######>" << std::endl;
}

CORBA::Boolean HelloInterface1_impl::datastore_is_acticated (CORBA::Long dsid)
    throw(CORBA::SystemException)
{
    if (dsid == 1)
        return dataStoreObj1->is_activated();
    else
        return dataStoreObj2->is_activated();
}

::CdmwReplicationManager::LocationList * HelloInterface1_impl::get_location_list (CORBA::Long dsid)
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
HelloInterface1_impl::get_local_datastore_info ()
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
    

void HelloInterface1_impl::set_info_into_datastore(CORBA::Long dsid, CORBA::Long oid, const char * data)
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

char * HelloInterface1_impl::get_info_from_datastore(CORBA::Long dsid, CORBA::Long oid)
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
    

HelloInterface2_impl::HelloInterface2_impl(CORBA::ORB_ptr orb,PortableServer::POA_ptr poa)
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

    context = new Cdmw::FT::DataStoreContext(m_orb.in(),
                                             m_poa.in(),
                                             100, 100,
                                             cohort_timeout,
                                             coordinator_timeout,
                                             1);

    datastore_group =
        (new Cdmw::FT::DataStoreGroup_impl(m_poa.in(),*context))->_this();

    dataStoreObj3 = new TestDataStore(3,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    
    onInitObj3 = new TestOnInitImpl();

    dataStoreObj3->register_initialiser(onInitObj3);
    
    dataStoreObj4 = new TestDataStore(4,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    
    onInitObj4 = new TestOnInitImpl();

    dataStoreObj4->register_initialiser(onInitObj4);
}

HelloInterface2_impl::~HelloInterface2_impl()
    throw()
{
}

void HelloInterface2_impl::hello ()
     throw(CORBA::SystemException)                  
{
    std::cout << "<####### HELLO 2 ######>" << std::endl;
}

CORBA::Boolean HelloInterface2_impl::datastore_is_acticated (CORBA::Long dsid)
    throw(CORBA::SystemException)
{
    if (dsid == 3)
        return dataStoreObj3->is_activated();
    else
        return dataStoreObj4->is_activated();
}

::CdmwReplicationManager::LocationList * HelloInterface2_impl::get_location_list (CORBA::Long dsid)
    throw(CORBA::SystemException)
{
    Cdmw::FT::DataStoreContext::LocationList list = context->get_locations(dsid);
    ::CdmwReplicationManager::LocationList* seqlist = new ::CdmwReplicationManager::LocationList;
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
HelloInterface2_impl::get_local_datastore_info ()
    throw(CORBA::SystemException)
{
    ::CdmwFT::StateTransfer::LocalDataStoreInfo* info = new  ::CdmwFT::StateTransfer::LocalDataStoreInfo;

    info->dsids.length(2);
    info->dsids[0] = dataStoreObj3->get_dsid();
    info->dsids[1] = dataStoreObj4->get_dsid();

    info->coordinator = dataStoreObj3->get_context().get_coordinator_ref();
    info->cohort = dataStoreObj3->get_context().get_cohort_ref();
    info->local_data_store = CdmwFT::StateTransfer::DataStoreGroup::_duplicate(datastore_group.in());

    return info;
}

void HelloInterface2_impl::set_info_into_datastore(CORBA::Long dsid, CORBA::Long oid, const char * data)
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

char * HelloInterface2_impl::get_info_from_datastore(CORBA::Long dsid, CORBA::Long oid)
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













HelloInterface3_impl::HelloInterface3_impl(CORBA::ORB_ptr orb,PortableServer::POA_ptr poa)
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

    context = new Cdmw::FT::DataStoreContext(m_orb.in(),
                                             m_poa.in(),
                                             100, 100,
                                             cohort_timeout,
                                             coordinator_timeout,
                                             1);

    datastore_group =
        (new Cdmw::FT::DataStoreGroup_impl(m_poa.in(),*context))->_this();

    dataStoreObj0 = new TestDataStore(5,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    
    dataStoreObj0->activate();

    dataStoreObj1 = new TestDataStore(11,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    
    dataStoreObj1->activate();
    dataStoreObj2 = new TestDataStore(12,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    dataStoreObj2->activate();    
    dataStoreObj3 = new TestDataStore(13,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    dataStoreObj3->activate();
    dataStoreObj4 = new TestDataStore(14,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    
    dataStoreObj4->activate();
    dataStoreObj5 = new TestDataStore(15,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    dataStoreObj5->activate();    
    dataStoreObj6 = new TestDataStore(16,
                                      m_orb.in(),
                                      m_poa.in(),
                                      *context);
    dataStoreObj6->activate();

}

HelloInterface3_impl::~HelloInterface3_impl()
    throw()
{
}

void HelloInterface3_impl::hello ()
     throw(CORBA::SystemException)                  
{
    std::cout << "<####### HELLO 3 ######>" << std::endl;
}

::CdmwFT::StateTransfer::LocalDataStoreInfo *
HelloInterface3_impl::get_local_datastore_info ()
    throw(CORBA::SystemException)
{
    ::CdmwFT::StateTransfer::LocalDataStoreInfo* info = new  ::CdmwFT::StateTransfer::LocalDataStoreInfo;

    info->dsids.length(3);
    info->dsids[0] = dataStoreObj1->get_dsid();
    info->dsids[1] = dataStoreObj2->get_dsid();
    info->dsids[2] = dataStoreObj3->get_dsid();

    info->coordinator = dataStoreObj1->get_context().get_coordinator_ref();
    info->cohort = dataStoreObj1->get_context().get_cohort_ref();
    info->local_data_store = CdmwFT::StateTransfer::DataStoreGroup::_duplicate(datastore_group.in());

    return info;
}



}; // End namespace Cdmw

