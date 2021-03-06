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

#ifndef INCL_CDMW_TEST_STATE_TRANSFER_HPP
#define INCL_CDMW_TEST_STATE_TRANSFER_HPP

// include files
#include "Foundation/testutils/Testable.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "ftstatemanager/DataStore.hpp"

// class definition

namespace Cdmw {
namespace FT {

    typedef Cdmw::FT::DataStore<int, std::string> TestDataStore;

    class TestStateTransfer : public Cdmw::TestUtils::Testable
    {

        public:
            TestStateTransfer(CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA);
  
            virtual ~TestStateTransfer();

        protected:
            virtual void do_tests();

            bool equivalent(TestDataStore& ds1, TestDataStore& ds2);

        private:
            CORBA::ORB_var m_orb;
            PortableServer::POA_var m_rootPOA;
    };

    class Inserter: public Cdmw::OsSupport::Thread
    {
        public:
            Inserter(TestDataStore& data_store)
                throw (OutOfMemoryException,
                       InternalErrorException,
                       BadParameterException);

            virtual ~Inserter() throw() {};

            virtual void run() throw();

        private:
            TestDataStore& m_data_store;
    };


    class Modifier: public Cdmw::OsSupport::Thread
    {
        public:
            Modifier(TestDataStore& data_store)
                throw (OutOfMemoryException,
                       InternalErrorException,
                       BadParameterException);

            virtual ~Modifier() throw() {};

            virtual void run() throw();

        private:
            TestDataStore& m_data_store;
    };

    class Observer: public TestDataStore::DataStoreObserver
    {
        public:
            typedef Cdmw::OrbSupport::type_traits<int>::_in_type _Oid_in_type;
            typedef Cdmw::OrbSupport::type_traits<std::string>::_in_type _Data_in_type;

            Observer();

            ~Observer() throw() {}

            virtual void on_insert(_Oid_in_type oid, _Data_in_type data)
                throw ();

            virtual void on_update(_Oid_in_type oid, _Data_in_type data)
                throw ();

            virtual void on_remove(_Oid_in_type oid)
                throw ();

            bool validate();

        private:
            int m_inserts;
            int m_updates;
            int m_removes;
            std::list<_Oid_in_type> m_update_oids;
            bool m_failure;
    };

}; // namespace FT
}; // namespace Cdmw

#endif

