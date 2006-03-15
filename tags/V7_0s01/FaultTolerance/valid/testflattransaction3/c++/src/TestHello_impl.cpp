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


#include "testflattransaction3/TestHello_impl.hpp"

#include <iostream>  

using namespace std;

namespace Cdmw
{

HelloInterface_impl::HelloInterface_impl(CORBA::ORB_ptr          orb
                                         , PortableServer::POA_ptr poa
                                         , const char*             process
                                         , Cdmw::TestORBInitializer* initializer)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb))
    , m_poa(PortableServer::POA::_duplicate(poa))
    , m_process(process)
    , m_initializer(initializer)
{
    Cdmw::FT::TXDataStoreBase* dsBase = Cdmw::FT::TXDataStoreBase::Get_dataStore(1);
    dataStoreObj1 = dynamic_cast<Cdmw::FT::TXDataStore<CORBA::Long, std::string, std::less<CORBA::Long> >*>(dsBase); 

//     onInitObj1 = new TestOnInitImpl();
    
//     dataStoreObj1->get_datastore().register_initialiser(onInitObj1);
}

HelloInterface_impl::~HelloInterface_impl()
    throw()
{
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



CORBA::Boolean
HelloInterface_impl::insert_and_commit()
    throw(CORBA::SystemException)
{
    bool result = true;
    try
    {
        std::cout<<"insert and commit"<<std::endl;
        dataStoreObj1->get_manager().begin();
        dataStoreObj1->insert(11, "one/one");
        dataStoreObj1->insert(12, "one/two");
        dataStoreObj1->insert(13, "one/three");
        dataStoreObj1->insert(14, "one/four");
//         TestStorageHome myStorageHome(1);
//         myStorageHome.create(11, "one/one");
//         myStorageHome.create(12, "one/two");
//         myStorageHome.create(13, "one/three");
//         myStorageHome.create(14, "one/four");
        

        std::cout<<"start of commit()"<<std::endl;
        dataStoreObj1->get_manager().commit();        
        std::cout<<"end of commit"<<std::endl;
    }
    catch(...)
    {
        result = false;
    }    
    return result;
}

CORBA::Boolean
HelloInterface_impl::update_and_rollback()
    throw(CORBA::SystemException)
{
    bool result = true;
    try
    {
        dataStoreObj1->get_manager().begin();
        
//         TestStorageHome myStorageHome(1);
//         TestStorageObject myStorageObject2Cpy11 = myStorageHome.find_by_oid(11);
//         myStorageObject2Cpy11.set("two/one");
//         TestStorageObject myStorageObject2Cpy12 = myStorageHome.find_by_oid(12);
//         myStorageObject2Cpy12.set("two/two");
//         TestStorageObject myStorageObject2Cpy13 = myStorageHome.find_by_oid(13);
//         myStorageObject2Cpy13.set("two/three");
//         TestStorageObject myStorageObject2Cpy14 = myStorageHome.find_by_oid(14);
//         myStorageObject2Cpy14.set("two/four");
        dataStoreObj1->update(11, "two/one");
        dataStoreObj1->update(12, "two/two");
        dataStoreObj1->update(13, "two/three");
        dataStoreObj1->update(14, "two/four");
        

        std::cout<<"start of rollback"<<std::endl;
        dataStoreObj1->get_manager().rollback();
        std::cout<<"end of rollback"<<std::endl;
    }
    catch(...)
    {
        result = false;
    }    
    return result;
}

CORBA::Boolean
HelloInterface_impl::update_forward_and_commit()
    throw(CORBA::SystemException)
{
    bool result = true;
    try
    {
        dataStoreObj1->get_manager().begin();
        
//         TestStorageHome myStorageHome(1);
//         TestStorageObject myStorageObject2Cpy11 = myStorageHome.find_by_oid(11);
//         myStorageObject2Cpy11.set("two/one");
//         TestStorageObject myStorageObject2Cpy12 = myStorageHome.find_by_oid(12);
//         myStorageObject2Cpy12.set("two/two");
//         TestStorageObject myStorageObject2Cpy13 = myStorageHome.find_by_oid(13);
//         myStorageObject2Cpy13.set("two/three");
//         TestStorageObject myStorageObject2Cpy14 = myStorageHome.find_by_oid(14);
//         myStorageObject2Cpy14.set("two/four");
        dataStoreObj1->update(11, "two/one");
        dataStoreObj1->update(12, "two/two");
        dataStoreObj1->update(13, "two/three");
        dataStoreObj1->update(14, "two/four");
        
        
        std::cout<<"start of commit"<<std::endl;
        try
        {
            dataStoreObj1->get_manager().commit();
            result = false;
        }
        catch(Cdmw::Common::NotFoundException)
        {
            result = true;
        }
        std::cout<<"end of commit"<<std::endl;
    }
    catch(...)
    {
        result = false;
    }    
    return result;
}
        
CORBA::Boolean
HelloInterface_impl::update_kill_and_commit()
    throw(CORBA::SystemException)
{
    bool result = true;
    try
    {
        dataStoreObj1->get_manager().begin();

//         TestStorageHome myStorageHome(1);
//         TestStorageObject myStorageObject2Cpy11 = myStorageHome.find_by_oid(11);
//         myStorageObject2Cpy11.set("two/one");
//         TestStorageObject myStorageObject2Cpy12 = myStorageHome.find_by_oid(12);
//         myStorageObject2Cpy12.set("two/two");
//         TestStorageObject myStorageObject2Cpy13 = myStorageHome.find_by_oid(13);
//         myStorageObject2Cpy13.set("two/three");
//         TestStorageObject myStorageObject2Cpy14 = myStorageHome.find_by_oid(14);
//         myStorageObject2Cpy14.set("two/four");

        dataStoreObj1->update(11, "two/one");
        dataStoreObj1->update(12, "two/two");
        dataStoreObj1->update(13, "two/three");
        dataStoreObj1->update(14, "two/four");
        // the process must be kill during the commit
        std::cout<<"start of commit"<<std::endl;
        dataStoreObj1->get_manager().commit();

        std::cout<<"end of commit"<<std::endl;
    }
    catch(...)
    {
        result = false;
    }    
    return result;
}

void HelloInterface_impl::setup_interception(CORBA::Long expected_event_count,
                                             const char * method,
                                             const char * where,
                                             const char * what)
    throw(CORBA::SystemException)
{
    std::cout<<"before setup_interception()"<<std::endl;
    m_initializer->setup_interception(expected_event_count, method, where, what);
    std::cout<<"after setup_interception()"<<std::endl;
};


}; // End namespace Cdmw

