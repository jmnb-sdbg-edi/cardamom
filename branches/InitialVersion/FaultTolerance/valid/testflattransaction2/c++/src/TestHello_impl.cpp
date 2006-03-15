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


#include "testflattransaction2/TestHello_impl.hpp"

namespace Cdmw
{


HelloInterface1_impl::HelloInterface1_impl(CORBA::ORB_ptr orb,PortableServer::POA_ptr poa, std::string name)
    throw(CORBA::SystemException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
      m_poa(PortableServer::POA::_duplicate(poa)),
      m_name(name)
{ 

    Cdmw::FT::TXDataStoreBase* dsBase = Cdmw::FT::TXDataStoreBase::Get_dataStore(1);
    m_dataStoreObj1 = dynamic_cast<Cdmw::FT::TXDataStore<CORBA::Long, std::string, std::less<CORBA::Long> >*>(dsBase); 


    // create the observer for the dsid 1
    m_observer1 = new MyDataStoreObserver1();



    dsBase = Cdmw::FT::TXDataStoreBase::Get_dataStore(2);
    m_dataStoreObj2 = dynamic_cast<Cdmw::FT::TXDataStore<CORBA::Long, std::string, std::less<CORBA::Long> >*>(dsBase);
    
    // create the observer for the dsid 2
    m_observer2 = new MyDataStoreObserver1();

    dsBase = Cdmw::FT::TXDataStoreBase::Get_dataStore(3);
    m_dataStoreObj3 = dynamic_cast<Cdmw::FT::TXDataStore<CORBA::Long, std::string, std::less<CORBA::Long> >*>(dsBase);

    // create the observer for the dsid 3
    m_observer3 = new MyDataStoreObserver1();

}

HelloInterface1_impl::~HelloInterface1_impl()
    throw()
{
}

void HelloInterface1_impl::hello ()
     throw(CORBA::SystemException)                  
{
    std::cerr << "<####### HELLO 1 ######>" << std::endl;
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
        std::cerr<<"DISD : "<<dsid<<" throw Cdmw::FT::NotFoundException"<<std::endl;
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
        std::cerr<<"DISD : "<<dsid<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("AlreadyExistException");
        return ret._retn();
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cerr<<"DISD : "<<dsid<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        CORBA::String_var ret = CORBA::string_dup("NotFoundException");
        return ret._retn();
    }

    return NULL;
}




char * HelloInterface1_impl::who_is_called()
    throw(CORBA::SystemException)
{
    CORBA::String_var ret = CORBA::string_dup(m_name.c_str());
    return ret._retn();
}


void HelloInterface1_impl:: begin()
    throw(CORBA::SystemException)
{
    m_dataStoreObj1->get_manager().begin();
}
        
CORBA::Boolean HelloInterface1_impl::begin_without_commit (const ::CdmwFlatTransaction::datastructs & d)
    throw(CORBA::SystemException)
{
    bool result = true;
    
    Cdmw::FT::TXDataStoreBase* dsBase = Cdmw::FT::TXDataStoreBase::Get_dataStore(1);

    m_dataStoreObj1 = dynamic_cast<Cdmw::FT::TXDataStore< CORBA::Long, std::string, std::less< CORBA::Long > > * >(dsBase); 
    m_dataStoreObj1->get_manager().begin();


    try
    {
        int length = d.length();
        for (int i = 0 ; i < length; i++)
        {
            
            switch (d[i].type)
            {
                case 0:
                    {
                        std::cerr<<"insert into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<" with the data "<<d[i].data<<std::endl;
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         // Should trigger call to on_insert callback
//                         myStorageHome.create(d[i].oid, (const char*)d[i].data);

                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->insert(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->insert(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->insert(d[i].oid, (const char*)d[i].data);
                        }

                    }
                    break;
                case 1:
                    {
                        std::cerr<<"update into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<" with the data "<<d[i].data<<std::endl;
                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->update(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->update(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->update(d[i].oid, (const char*)d[i].data);
                        }
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(d[i].oid);
//                         myStorageObject2Cpy.set((const char*)d[i].data);
                    }
                    break;
                case 2:
                    {
 //                         // Should trigger call to on_remove callback
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         myStorageHome.remove(d[i].oid);
                       std::cerr<<"remove into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<std::endl;
                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->remove(d[i].oid);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->remove(d[i].oid);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->remove(d[i].oid);
                        }
                    }
                    break;
                default:
                    std::cerr<<"Not a good type"<<std::endl;
            }
        }
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cerr<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        return false;
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cerr<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        return false;
    }

    std::cerr<<"stop transaction"<<std::endl;
    m_dataStoreObj1->get_manager().rollback();
    return result;
}


        
CORBA::Boolean HelloInterface1_impl::begin_with_commit (const ::CdmwFlatTransaction::datastructs & d)
    throw(CORBA::SystemException)
{
    bool result = true;
    
    m_dataStoreObj1->get_manager().begin();


    try
    {
        int length = d.length();
        for (int i = 0 ; i < length; i++)
        {
            
            switch (d[i].type)
            {
                case 0:
                    {
                        std::cerr<<"insert into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<" with the data "<<d[i].data<<std::endl;
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         // Should trigger call to on_insert callback
//                         myStorageHome.create(d[i].oid, (const char*)d[i].data);

                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->insert(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->insert(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->insert(d[i].oid, (const char*)d[i].data);
                        }

                    }
                    break;
                case 1:
                    {
                        std::cerr<<"update into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<" with the data "<<d[i].data<<std::endl;
                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->update(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->update(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->update(d[i].oid, (const char*)d[i].data);
                        }
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(d[i].oid);
//                         myStorageObject2Cpy.set((const char*)d[i].data);
                    }
                    break;
                case 2:
                    {
 //                         // Should trigger call to on_remove callback
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         myStorageHome.remove(d[i].oid);
                       std::cerr<<"remove into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<std::endl;
                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->remove(d[i].oid);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->remove(d[i].oid);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->remove(d[i].oid);
                        }
                    }
                    break;
                default:
                    std::cerr<<"Not a good type"<<std::endl;
            }
        }
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cerr<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        return false;
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cerr<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        return false;
    }

    std::cerr<<"stop transaction"<<std::endl;
    try
    {
        m_dataStoreObj1->get_manager().commit();
    }
    catch(const CORBA::SystemException& ex)
    {
        std::cerr<<"Commit exception "<<ex<<std::endl;
        throw;
    }
    catch(...)
    {
        std::cerr<<"Commit exception "<<std::endl;
        throw;
    }
        
    
    return result;
}

CORBA::Boolean HelloInterface1_impl::begin_with_rollback (const ::CdmwFlatTransaction::datastructs & d)
    throw(CORBA::SystemException)
{

    bool result = true;
    
    std::cerr<<"------------------> start transaction"<<std::endl;
    m_dataStoreObj1->get_manager().begin();


    try
    {
        int length = d.length();
        for (int i = 0 ; i < length; i++)
        {
            
            switch (d[i].type)
            {
                case 0:
                    {
                        std::cerr<<"insert into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<" with the data "<<d[i].data<<std::endl;
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         // Should trigger call to on_insert callback
//                         myStorageHome.create(d[i].oid, (const char*)d[i].data);

                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->insert(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->insert(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->insert(d[i].oid, (const char*)d[i].data);
                        }

                    }
                    break;
                case 1:
                    {
                        std::cerr<<"update into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<" with the data "<<d[i].data<<std::endl;
                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->update(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->update(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->update(d[i].oid, (const char*)d[i].data);
                        }
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(d[i].oid);
//                         myStorageObject2Cpy.set((const char*)d[i].data);
                    }
                    break;
                case 2:
                    {
 //                         // Should trigger call to on_remove callback
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         myStorageHome.remove(d[i].oid);
                       std::cerr<<"remove into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<std::endl;
                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->remove(d[i].oid);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->remove(d[i].oid);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->remove(d[i].oid);
                        }
                    }
                    break;
                default:
                    std::cerr<<"Not a good type"<<std::endl;
            }
        }
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cerr<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        return false;
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cerr<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        return false;
    }
    std::cerr<<"stop transaction"<<std::endl;
    m_dataStoreObj1->get_manager().rollback();
    return result;
}


CORBA::Boolean HelloInterface1_impl::begin_with_kill (const ::CdmwFlatTransaction::datastructs & d,
                                                      ::CdmwFlatTransaction::HelloInterface_ptr p)
    throw(CORBA::SystemException)
{

    bool result = true;
    
    std::cerr<<"------------------> start transaction"<<std::endl;
    try
    {
        m_dataStoreObj1->get_manager().begin();
    }
    catch(const CORBA::SystemException& ex)
    {
        std::cout<<ex<<std::endl;
    }


    try
    {
        int length = d.length();
        for (int i = 0 ; i < length; i++)
        {
            
            switch (d[i].type)
            {
                case 0:
                    {
                        std::cerr<<"insert into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<" with the data "<<d[i].data<<std::endl;
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         // Should trigger call to on_insert callback
//                         myStorageHome.create(d[i].oid, (const char*)d[i].data);

                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->insert(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->insert(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->insert(d[i].oid, (const char*)d[i].data);
                        }

                    }
                    break;
                case 1:
                    {
                        std::cerr<<"update into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<" with the data "<<d[i].data<<std::endl;
                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->update(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->update(d[i].oid, (const char*)d[i].data);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->update(d[i].oid, (const char*)d[i].data);
                        }
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         TestStorageObject myStorageObject2Cpy = myStorageHome.find_by_oid(d[i].oid);
//                         myStorageObject2Cpy.set((const char*)d[i].data);
                    }
                    break;
                case 2:
                    {
 //                         // Should trigger call to on_remove callback
//                         TestStorageHome myStorageHome(d[i].dsid);
//                         myStorageHome.remove(d[i].oid);
                       std::cerr<<"remove into dsid "<<d[i].dsid<<" the oid "<<d[i].oid<<std::endl;
                        if (d[i].dsid == 1)
                        {
                            m_dataStoreObj1->remove(d[i].oid);
                        }
                        else if (d[i].dsid == 2)
                        {
                            m_dataStoreObj2->remove(d[i].oid);
                        }
                        else if (d[i].dsid == 3)
                        {
                            m_dataStoreObj3->remove(d[i].oid);
                        }
                    }
                    break;
                default:
                    std::cerr<<"Not a good type"<<std::endl;
            }
        }
    }
    catch (const Cdmw::FT::AlreadyExistException&)
    {
        std::cerr<<" throw Cdmw::FT::AlreadyExistException"<<std::endl;
        return false;
    }
    catch(const Cdmw::FT::NotFoundException&)
    {
        std::cerr<<" throw Cdmw::FT::NotFoundException"<<std::endl;
        return false;
    }
    std::cerr<<"kill process"<<std::endl;
    p->kill_process();
    return result;
}



void HelloInterface1_impl::kill_process()
    throw(CORBA::SystemException)
{
    std::cerr<<"Kill process"<<std::endl;
    ::exit(-1);
}
    
    

}; // End namespace Cdmw

