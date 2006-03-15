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
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreGroup_impl.hpp>
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>

#include "testftlocationmanager2/TestHello.skel.hpp"
#include "testftlocationmanager2/TestOnInit.hpp"


#include <string>

namespace Cdmw
{
    typedef Cdmw::FT::DataStore<int, std::string> TestDataStore;
    typedef Cdmw::CommonSvcs::DataStore::StorageHome<int, std::string> TestStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject<int, std::string> TestStorageObject;
    typedef Cdmw::FT::DataStoreInitialiser<int, std::string> TestOnInit;
    typedef Cdmw::OnInit_impl<int, std::string> TestOnInitImpl;

    class HelloInterface1_impl :public virtual POA_CdmwReplicationManager::HelloInterface1,
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
        
        virtual CORBA::Boolean 
        datastore_is_acticated (CORBA::Long dsid)
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
        HelloInterface1_impl(const HelloInterface1_impl& rhs)
            throw();
        
        HelloInterface1_impl&
        operator=(const HelloInterface1_impl& rhs)
            throw();

        Cdmw::FT::DataStoreContext* context;
        
        TestDataStore* dataStoreObj1;
        
        TestDataStore* dataStoreObj2;
        
        TestOnInitImpl* onInitObj1;

        TestOnInitImpl* onInitObj2;

        CdmwFT::StateTransfer::DataStoreGroup_var datastore_group;
        
        CORBA::ORB_var m_orb;
        
        PortableServer::POA_var m_poa;
    

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
        
        virtual CORBA::Boolean 
        datastore_is_acticated (CORBA::Long dsid)
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

        Cdmw::FT::DataStoreContext* context;

        TestDataStore* dataStoreObj3;
    
        TestDataStore* dataStoreObj4;
    
        TestOnInitImpl* onInitObj3;

        TestOnInitImpl* onInitObj4;

        CdmwFT::StateTransfer::DataStoreGroup_var datastore_group;

        CORBA::ORB_var m_orb;
        
        PortableServer::POA_var m_poa;

    }; // End class HelloInterface2_impl




    class HelloInterface3_impl :
        public virtual POA_CdmwReplicationManager::HelloInterface3,
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
        HelloInterface3_impl(CORBA::ORB_ptr orb, PortableServer::POA_ptr poa)
            throw(CORBA::SystemException);

        /**
         * Purpose:
         * <p> [ Destructor description if necessary]
         * 
         */ 
        virtual 
        ~HelloInterface3_impl()
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
        virtual ::CdmwFT::StateTransfer::LocalDataStoreInfo * get_local_datastore_info ()
            throw(CORBA::SystemException);


    private:

        // Hide copy constructor/assignment operator
        HelloInterface3_impl(const HelloInterface3_impl& rhs)
            throw();
        
        HelloInterface3_impl&
        operator=(const HelloInterface3_impl& rhs)
            throw();

        Cdmw::FT::DataStoreContext* context;

        TestDataStore* dataStoreObj0;

        TestDataStore* dataStoreObj1;
    
        TestDataStore* dataStoreObj2;
    
        TestDataStore* dataStoreObj3;

        TestDataStore* dataStoreObj4;
    
        TestDataStore* dataStoreObj5;
    
        TestDataStore* dataStoreObj6;

        CdmwFT::StateTransfer::DataStoreGroup_var datastore_group;

        CORBA::ORB_var m_orb;
        
        PortableServer::POA_var m_poa;

    }; // End class HelloInterface3_impl

}; // End namespace Cdmw

#endif // INCL_HELLO_INTERFACE_IMPL_HPP 

