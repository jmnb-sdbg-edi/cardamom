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


#include "testftstatetransfer/TestHello_impl.hpp"

namespace Cdmw
{


HelloInterface1_impl::HelloInterface1_impl(CORBA::ORB_ptr orb,PortableServer::POA_ptr poa, std::string name)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_name(name)
{ 


    Cdmw::FT::DataStoreBase* dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(1);
    dataStoreObj1 = dynamic_cast<Cdmw::FT::DataStore<CORBA::Long, std::string, std::less<CORBA::Long> >*>(dsBase); 


    // create the observer for the dsid 1
    m_observer1 = new MyDataStoreObserver1();

    //  register the previous observer
    bool registered = dataStoreObj1->register_observer(m_observer1);
    if ( ! registered) {
      // ###### ?????
    }

    // create the CORBA object for the datastore
    datastore_group = (new Cdmw::FT::DataStoreGroup_impl(m_poa.in(), dataStoreObj1->get_context()))->_this();
    onInitObj1 = new TestOnInitImpl();
    
    dataStoreObj1->register_initialiser(onInitObj1);

    dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(2);
    dataStoreObj2 = dynamic_cast<Cdmw::FT::DataStore<CORBA::Long, std::string, std::less<CORBA::Long> >*>(dsBase);
    
    // create the observer for the dsid 2
    m_observer2 = new MyDataStoreObserver1();

    //  register the previous observer
    registered = dataStoreObj2->register_observer(m_observer2);
    if ( ! registered) {
      // ###### ?????
    }

    // create the initialiser for the datastore (on_init)
    onInitObj2 = new TestOnInitImpl();

    // register the initialiser
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
        CORBA::String_var ret = CORBA::string_dup("NotFoundException");
        return ret._retn();
    }
    return NULL;
}
    

char * HelloInterface1_impl::trigger_on_insert(CORBA::Long dsid, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
    try
    {
        TestStorageHome myStorageHome(dsid);
        // Should trigger call to on_insert callback
        myStorageHome.create(oid, data);
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("AlreadyExistException");
        return ret._retn();
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("NotFoundException");
        return ret._retn();
    }

    return NULL;
}


char * HelloInterface1_impl::trigger_on_update(CORBA::Long dsid, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
    try
    {
        // Should trigger call to on_update callback
        TestStorageHome myStorageHome(dsid);
        TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(oid);
        myStorageObject2Cpy.set(data);
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("AlreadyExistException");
        return ret._retn();
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("NotFoundException");
        return ret._retn();
    }

    return NULL;
}


char * HelloInterface1_impl::trigger_on_remove(CORBA::Long dsid, CORBA::Long oid)
    throw(CORBA::SystemException)
{
    try
    {
        // Should trigger call to on_remove callback
        TestStorageHome myStorageHome(dsid);
        myStorageHome.remove(oid);
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("AlreadyExistException");
        return ret._retn();
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("NotFoundException");
        return ret._retn();
    }

    return NULL;
}

CORBA::Long HelloInterface1_impl::callback_called(CORBA::Long dsid, const char * callback_name)
    throw(CORBA::SystemException)
{
    if (dsid == 1)
    {
        if (!strcmp(callback_name,"remove"))
            return m_observer1->return_on_remove_called();
        if (!strcmp(callback_name,"update"))
            return m_observer1->return_on_update_called();
        if (!strcmp(callback_name,"insert"))
            return m_observer1->return_on_insert_called();
    }
    else
    {
        if (!strcmp(callback_name,"remove"))
            return m_observer2->return_on_remove_called();
        if (!strcmp(callback_name,"update"))
            return m_observer2->return_on_update_called();
        if (!strcmp(callback_name,"insert"))
            return m_observer2->return_on_insert_called();
    }
    return 0;
}

char * HelloInterface1_impl::who_is_called()
    throw(CORBA::SystemException)
{
    CORBA::String_var ret = CORBA::string_dup(m_name.c_str());
    return ret._retn();
}


HelloInterface2_impl::HelloInterface2_impl(CORBA::ORB_ptr orb,PortableServer::POA_ptr poa, std::string name)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_name(name)
{ 
    Cdmw::FT::DataStoreBase* dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(3);
    dataStoreObj3 = dynamic_cast<Cdmw::FT::DataStore<CORBA::Long, std::string, std::less<CORBA::Long> >*>(dsBase);
    
    // create the observer for the dsid 3
    m_observer3 = new MyDataStoreObserver2();

    //  register the previous observer
    bool registered = dataStoreObj3->register_observer(m_observer3);
    if ( ! registered) {
      // ###### ?????
    }

    // create the CORBA object for the datastore
    datastore_group = (new Cdmw::FT::DataStoreGroup_impl(m_poa.in(), dataStoreObj3->get_context()))->_this();

    // create the initialiser for the datastore (on_init)
    onInitObj3 = new TestOnInitImpl();

    // register the initialiser
    dataStoreObj3->register_initialiser(onInitObj3);
    
    dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(4);
    dataStoreObj4 = dynamic_cast<Cdmw::FT::DataStore<CORBA::Long, std::string, std::less<CORBA::Long> >*>(dsBase);
    
    // create the observer for the dsid 4
    m_observer4 = new MyDataStoreObserver2();

    //  register the previous observer
    registered = dataStoreObj4->register_observer(m_observer4);
    if ( ! registered) {
      // ###### ?????
    }

    // create the initialiser for the datastore (on_init)
    onInitObj4 = new TestOnInitImpl();

    // register the initialiser
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
        CORBA::String_var ret = CORBA::string_dup("NotFoundException");
        return ret._retn();
    }
    return NULL;
}


char * HelloInterface2_impl::trigger_on_insert(CORBA::Long dsid, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
    try
    {
        TestStorageHome myStorageHome(dsid);
        // Should trigger call to on_insert callback
        myStorageHome.create(oid, data);
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("AlreadyExistException");
        return ret._retn();
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("NotFoundException");
        return ret._retn();
    }

    return NULL;
}


char * HelloInterface2_impl::trigger_on_update(CORBA::Long dsid, CORBA::Long oid, const char * data)
    throw(CORBA::SystemException)
{
    try
    {
        // Should trigger call to on_update callback
        TestStorageHome myStorageHome(dsid);
        TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(oid);
        myStorageObject2Cpy.set(data);
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("AlreadyExistException");
        return ret._retn();
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("NotFoundException");
        return ret._retn();
    }

    return NULL;
}


char * HelloInterface2_impl::trigger_on_remove(CORBA::Long dsid, CORBA::Long oid)
    throw(CORBA::SystemException)
{
    try
    {
        // Should trigger call to on_remove callback
        TestStorageHome myStorageHome(dsid);
        myStorageHome.remove(oid);
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("AlreadyExistException");
        return ret._retn();
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cout<<"DISD : "<<dsid<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("NotFoundException");
        return ret._retn();
    }

    return NULL;
}


CORBA::Long HelloInterface2_impl::callback_called(CORBA::Long dsid, const char * callback_name)
    throw(CORBA::SystemException)
{
    if (dsid == 3)
    {
        if (!strcmp(callback_name,"remove"))
            return m_observer3->return_on_remove_called();
        if (!strcmp(callback_name,"update"))
            return m_observer3->return_on_update_called();
        if (!strcmp(callback_name,"insert"))
            return m_observer3->return_on_insert_called();
    }
    else
    {
        if (!strcmp(callback_name,"remove"))
            return m_observer4->return_on_remove_called();
        if (!strcmp(callback_name,"update"))
            return m_observer4->return_on_update_called();
        if (!strcmp(callback_name,"insert"))
            return m_observer4->return_on_insert_called();
    } 
    return 0; 
}
        
char * HelloInterface2_impl::who_is_called()
    throw(CORBA::SystemException)
{
    CORBA::String_var ret = CORBA::string_dup(m_name.c_str());
    return ret._retn();
}
    

}; // End namespace Cdmw

