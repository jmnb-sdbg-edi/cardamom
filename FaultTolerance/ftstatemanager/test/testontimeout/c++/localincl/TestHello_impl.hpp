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

#include "idllib/FT.skel.hpp"
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>
#include "FaultTolerance/ftlocationmanager/LocationDataStoreTimeoutHandler_impl.hpp"

#include "testontimeout/TestHello.skel.hpp"

#include <string>

namespace Cdmw
{
    typedef Cdmw::FT::DataStore<int, std::string> TestDataStore;
    typedef Cdmw::CommonSvcs::DataStore::StorageHome<int, std::string> TestStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject<int, std::string> TestStorageObject;
    typedef Cdmw::FT::DataStoreInitialiser<int, std::string> TestOnInit;
    typedef TestDataStore::DataStoreObserver TestDataStoreObserver;
    typedef TestDataStoreObserver::_Oid_in_type _Oid_in_type;
    typedef TestDataStoreObserver::_Data_in_type _Data_in_type;

    class HelloInterface1_impl :public virtual POA_CdmwReplicationManager::HelloInterface1,
                                virtual public PortableServer::RefCountServantBase,
                                virtual public TestDataStoreObserver
    {

    public:
        
        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */ 
        HelloInterface1_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
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
        virtual ::CdmwReplicationManager::LocationList * 
        get_location_list (CORBA::Long dsid)
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */
        virtual ::CdmwFT::StateTransfer::LocalDataStoreInfo * get_local_datastore_info ()
            throw(CORBA::SystemException);


        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void set_info_into_datastore(CORBA::Long dsid, CORBA::Long oid, const char * data)
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
        virtual char * get_coordinator_location()
            throw(CORBA::SystemException);

         /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 

        virtual void on_insert(_Oid_in_type oid, _Data_in_type data) throw ();

        virtual void on_update(_Oid_in_type oid, _Data_in_type data) throw ();

        virtual void on_remove(_Oid_in_type oid) throw ();

        void force_delay();

    private:

        // Hide copy constructor/assignment operator
        HelloInterface1_impl(const HelloInterface1_impl& rhs)
            throw();
        
        HelloInterface1_impl&
        operator=(const HelloInterface1_impl& rhs)
            throw();

        Cdmw::FT::DataStoreContext* m_context;
        
        TestDataStore* dataStoreObj1;
        
        TestDataStore* dataStoreObj2;
        
        CdmwFT::StateTransfer::DataStoreGroup_var datastore_group;
        
        CORBA::ORB_var m_orb;
        
        PortableServer::POA_var m_poa;

        bool m_delay;


    }; // End class HelloInterface1_impl 

    class HelloInterface2_impl :
        public virtual POA_CdmwReplicationManager::HelloInterface2,
                                                   virtual public PortableServer::RefCountServantBase
    {

    public:

        /**
         * Purpose:
         * <p> [Constructor description if necessary]
         * 
         *@param
         *@exception
         */ 
        HelloInterface2_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        virtual 
        ~HelloInterface2_impl()
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
        virtual ::CdmwReplicationManager::LocationList * 
        get_location_list (CORBA::Long dsid)
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */
        virtual ::CdmwFT::StateTransfer::LocalDataStoreInfo * get_local_datastore_info ()
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p>
         * 
         *@param
         *@return
         *@exception
         */ 
        virtual void set_info_into_datastore(CORBA::Long dsid, CORBA::Long oid, const char * data)
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

    private:

        // Hide copy constructor/assignment operator
        HelloInterface2_impl(const HelloInterface2_impl& rhs)
            throw();
        
        HelloInterface2_impl&
        operator=(const HelloInterface2_impl& rhs)
            throw();

        Cdmw::FT::DataStoreContext* m_context;

        TestDataStore* dataStoreObj3;
    
        TestDataStore* dataStoreObj4;
    
        CdmwFT::StateTransfer::DataStoreGroup_var datastore_group;

        CORBA::ORB_var m_orb;
        
        PortableServer::POA_var m_poa;

    }; // End class HelloInterface2_impl



}; // End namespace Cdmw

#endif // INCL_HELLO_INTERFACE_IMPL_HPP 

