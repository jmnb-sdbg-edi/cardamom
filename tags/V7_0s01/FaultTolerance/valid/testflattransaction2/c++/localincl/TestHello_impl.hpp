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


#ifndef INCL_HELLO_INTERFACE_IMPL_HPP 
#define INCL_HELLO_INTERFACE_IMPL_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <FaultTolerance/ftstatemanager/TXDataStore.hpp>
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreGroup_impl.hpp>

#include "testflattransaction2/TestHello.skel.hpp"
#include "testflattransaction2/TestOnInit.hpp"


#include <string>

namespace Cdmw
{
    typedef Cdmw::FT::TXDataStore<int, std::string> TestDataStore;
    typedef Cdmw::CommonSvcs::DataStore::StorageHome<int, std::string> TestStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject<int, std::string> TestStorageObject;
    typedef Cdmw::FT::DataStoreInitialiser<int, std::string> TestOnInit;
    typedef Cdmw::OnInit_impl<int, std::string> TestOnInitImpl;

    class MyDataStoreObserver1 : public ::Cdmw::CommonSvcs::DataStore::DataStoreObserver < int, std::string > {
        
    public :
        MyDataStoreObserver1() 
            : on_insert_called(0)
            , on_update_called(0)
            , on_remove_called(0) {
                
            }
        
        void on_insert(_Oid_in_type oid, _Data_in_type data) throw () {
            std::cout<<"------ MyDataStoreObserver1 -> on_insert ------"<<std::endl;
            on_insert_called++;
        }
        
        void on_update(_Oid_in_type oid, _Data_in_type data) throw () {            
            std::cout<<"------ MyDataStoreObserver1 -> on_update ------"<<std::endl;
            on_update_called++;
        }
        
        void on_remove(_Oid_in_type oid) throw () {
            std::cout<<"------ MyDataStoreObserver1 -> on_remove ------"<<std::endl; 
            on_remove_called++;
        }
        
        int return_on_insert_called()
        {
            int res = on_insert_called;
            on_insert_called = 0;

            return res;
        }
        
        int return_on_update_called()
        {
            int res = on_update_called;
            on_update_called = 0;

            return res;
        }

        int return_on_remove_called()
        {
            int res = on_remove_called;
            on_remove_called = 0;

            return res;
        }

        int on_insert_called;
        int on_update_called;
        int on_remove_called;
    };

    class HelloInterface1_impl :public virtual POA_CdmwFlatTransaction::HelloInterface1,
                                                                           virtual public PortableServer::RefCountServantBase
    {

    public:
        
    public:
        
        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */ 
        HelloInterface1_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa, std::string name)
            throw(CORBA::SystemException);
        
        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        virtual 
        ~HelloInterface1_impl()
            throw();
        
        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void hello ()
            throw(CORBA::SystemException);
     
         /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual char * get_info_from_datastore(CORBA::Long dsid, CORBA::Long oid)
            throw(CORBA::SystemException);

        
        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual char * trigger_on_insert(CORBA::Long dsid, CORBA::Long oid, const char * data)
            throw(CORBA::SystemException);

        /**
         * who_is_called
         */
        virtual char * who_is_called()
            throw(CORBA::SystemException);

        /**
         * begin
         */ 
        virtual void begin()
            throw(CORBA::SystemException);
        
        /**
         * begin_without_commit
         */ 
        virtual CORBA::Boolean begin_without_commit (const ::CdmwFlatTransaction::datastructs & d)
            throw(CORBA::SystemException);

        
        /**
         * begin_with_commit
         */ 
        virtual CORBA::Boolean begin_with_commit (const ::CdmwFlatTransaction::datastructs & d)
            throw(CORBA::SystemException);

            
        virtual CORBA::Boolean begin_with_rollback (const ::CdmwFlatTransaction::datastructs & d)
            throw(CORBA::SystemException);
        
        virtual CORBA::Boolean begin_with_kill (const ::CdmwFlatTransaction::datastructs & d,
                                                ::CdmwFlatTransaction::HelloInterface_ptr p)
            throw(CORBA::SystemException);


        /**
         * kill_process
         */
        virtual void kill_process()
            throw(CORBA::SystemException);
    private:

        // Hide copy constructor/assignment operator
        HelloInterface1_impl(const HelloInterface1_impl& rhs)
            throw();
        
        HelloInterface1_impl&
        operator=(const HelloInterface1_impl& rhs)
            throw();
        
        TestDataStore* m_dataStoreObj1;
        
        TestDataStore* m_dataStoreObj2;
        
        TestDataStore* m_dataStoreObj3;
        
        TestOnInitImpl* m_onInitObj1;

        TestOnInitImpl* m_onInitObj2;

        TestOnInitImpl* m_onInitObj3;

        MyDataStoreObserver1* m_observer1;
        
        MyDataStoreObserver1* m_observer2;
        
        MyDataStoreObserver1* m_observer3;

        CdmwFT::StateTransfer::DataStoreGroup_var m_datastore1;
        
        CdmwFT::StateTransfer::DataStoreGroup_var m_datastore2;
        
        CdmwFT::StateTransfer::DataStoreGroup_var m_datastore3;

        CORBA::ORB_var m_orb;
        
        PortableServer::POA_var m_poa;
    
        std::string m_name;

    }; // End class HelloInterface1_impl 

}; // End namespace Cdmw

#endif // INCL_HELLO_INTERFACE_IMPL_HPP 

