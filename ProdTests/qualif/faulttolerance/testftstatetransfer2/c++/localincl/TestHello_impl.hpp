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


#ifndef INCL_HELLO_INTERFACE_IMPL_HPP 
#define INCL_HELLO_INTERFACE_IMPL_HPP 

#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/StorageHome.hpp>
#include <FaultTolerance/ftstatemanager/StorageObject.hpp>

#include "testftstatetransfer2/TestHello.skel.hpp"
#include "testftstatetransfer2/TestOnInit.hpp"


#include <string>

namespace Cdmw
{
    typedef Cdmw::FT::DataStore<CORBA::Long, std::string, std::less<CORBA::Long> > TestDataStore;
    typedef Cdmw::FT::StorageHome<CORBA::Long, std::string, std::less<CORBA::Long> > TestStorageHome;
    typedef Cdmw::FT::StorageObject<CORBA::Long, std::string, std::less<CORBA::Long> > TestStorageObject;
    typedef Cdmw::FT::DataStoreInitialiser<CORBA::Long, std::string, std::less<CORBA::Long> > TestOnInit;
    typedef Cdmw::OnInit_impl<CORBA::Long, std::string, std::less<CORBA::Long> > TestOnInitImpl;

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

    private:

        // Hide copy constructor/assignment operator
        HelloInterface_impl(const HelloInterface_impl& rhs)
            throw();
        
        HelloInterface_impl&
        operator=(const HelloInterface_impl& rhs)
            throw();
        
        TestDataStore* dataStoreObj1;
        TestOnInitImpl* onInitObj1;
        CORBA::ORB_var m_orb;
        PortableServer::POA_var m_poa;
    
        bool          m_test_done;
        std::string   m_process;
        CORBA::Long   m_target_step;

    }; // End class HelloInterface_impl 

}; // End namespace Cdmw

#endif // INCL_HELLO_INTERFACE_IMPL_HPP 

