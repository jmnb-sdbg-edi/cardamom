/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * This file and the information it contains are confidential and proprietary. 
 * They shall not be reproduced nor disclosed to any person except to those 
 * having a need to know them without prior written consent of the owner.
*/
/* ===================================================================== */


#ifndef INCL_HELLO_INTERFACE_IMPL_HPP 
#define INCL_HELLO_INTERFACE_IMPL_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>
#include "testftinitsequencing/TestOnInit.hpp"
#include "testftinitsequencing/TestHello.skel.hpp"

#include <string>

namespace Cdmw
{
    typedef Cdmw::FT::DataStore<CORBA::Long, std::string, std::less<CORBA::Long> > TestDataStore;
    typedef Cdmw::CommonSvcs::DataStore::StorageHome<CORBA::Long, std::string, std::less<CORBA::Long> > TestStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject<CORBA::Long, std::string, std::less<CORBA::Long> > TestStorageObject;
    typedef Cdmw::OnInit_impl<CORBA::Long, std::string, std::less<CORBA::Long> > TestOnInitImpl;



    class time_collector_impl : public virtual POA_CdmwReplicationManager::time_collector,
                               virtual public PortableServer::RefCountServantBase
    {
    public :
        time_collector_impl()
        {
        }
        
        
        virtual ~time_collector_impl()
            throw()
        {
        }
        /**
         * set_init_primary_data_stores_time
         */
        
        void set_init_primary_data_stores_time(const CdmwReplicationManager::timestruct& time)
            throw(CORBA::SystemException)
        {        
            m_init_primary_data_stores_time = time;          
        }
        
        
        /**
         * set_get_record_iterator_time()
         */
        void set_get_record_iterator_time(const CdmwReplicationManager::timestruct& time)
            throw(CORBA::SystemException)
        {
            m_get_record_iterator_time = time;
        }
        

        /**
         * get_init_primary_data_stores_time
         */
        
        CdmwReplicationManager::timestruct get_init_primary_data_stores_time()
            throw(CORBA::SystemException)
        {
            return m_init_primary_data_stores_time;
        }
        
        
        /**
         * get_get_record_iterator_time()
         */
        CdmwReplicationManager::timestruct get_get_record_iterator_time()
            throw(CORBA::SystemException)
        {
            return m_get_record_iterator_time;           
        }
        

    private:
        CdmwReplicationManager::timestruct m_init_primary_data_stores_time;
        CdmwReplicationManager::timestruct m_get_record_iterator_time;
       
    };
    


    class HelloInterface_impl :public virtual POA_CdmwReplicationManager::HelloInterface,
                               virtual public PortableServer::RefCountServantBase
    {

    public:
        
        HelloInterface_impl ( CORBA::ORB_ptr          orb
                            , PortableServer::POA_ptr poa
                            , const char*             process)
            throw(CORBA::SystemException);
        
        virtual 
        ~HelloInterface_impl()
            throw();
        
        virtual void
        insert(CORBA::Long dsid, CORBA::Long oid, const char * data)
            throw(CORBA::SystemException);
        
        virtual void
        update(CORBA::Long dsid, CORBA::Long oid, const char * data)
            throw(CORBA::SystemException);
        
        virtual void
        remove(CORBA::Long dsid, CORBA::Long oid)
            throw(CORBA::SystemException);

        virtual char * 
        get_data(CORBA::Long dsid, CORBA::Long oid)
            throw(CORBA::SystemException);

        void ping() throw(CORBA::SystemException) { }

        CORBA::ULong get_time()
            throw(CORBA::SystemException);

        CORBA::ULong get_init_time()
            throw(CORBA::SystemException);
    private:

        // Hide copy constructor/assignment operator
        HelloInterface_impl(const HelloInterface_impl& rhs)
            throw();
        
        HelloInterface_impl&
        operator=(const HelloInterface_impl& rhs)
            throw();
        
        TestDataStore* dataStoreObj1;
        CORBA::ORB_var m_orb;
        PortableServer::POA_var m_poa;
    
        bool          m_test_done;
        std::string   m_process;
        CORBA::Long   m_target_step;
        TestOnInitImpl* m_on_init;

    }; // End class HelloInterface_impl 

}; // End namespace Cdmw

#endif // INCL_HELLO_INTERFACE_IMPL_HPP 

