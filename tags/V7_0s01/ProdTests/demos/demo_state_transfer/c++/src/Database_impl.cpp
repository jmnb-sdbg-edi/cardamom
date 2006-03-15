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


// Demo Files
#include <Database_impl.hpp>
#include <StateTransferConfiguration.hpp>


Database_impl::Database_impl(CORBA::ORB_ptr orb,
                             PortableServer::POA_ptr poa,
                             bool pointToPoint)
:  m_orb(CORBA::ORB::_duplicate(orb)),
   m_poa(PortableServer::POA::_duplicate(poa))
{
    
    // Timeout for cohort
    Cdmw::OsSupport::OS::Timeval cohort_timeout;
    cohort_timeout.seconds = COHORT_TIMEOUT;
    cohort_timeout.microseconds = 0;

    // Timeout for coordinator
    Cdmw::OsSupport::OS::Timeval coordinator_timeout;
    coordinator_timeout.seconds = COORDINATOR_TIMEOUT;
    coordinator_timeout.microseconds = 0;

    try {
        // get RootPOA
        CORBA::Object_var obj = m_orb->resolve_initial_references( "RootPOA" );
        PortableServer::POA_var rootPOA
            = PortableServer::POA::_narrow( obj.in() );

        if (pointToPoint) 
        {            
            // Create DataStores with point-to-point StateTransfer
            std::cout << "         -- Database_impl: use point-to-point StateTransfer" << std::endl;

            std::cout << "         -- Database_impl: create DataStorageHome for DataPart1 "<<DATA_STORE_1_IDENTIFIER << std::endl;
            m_dataPart1StorageHome = new DataPart1StorageHome(DATA_STORE_1_IDENTIFIER);

            std::cout << "         -- Database_impl: create DataStorageHome for DataPart2 " <<DATA_STORE_2_IDENTIFIER<< std::endl;
            m_dataPart2StorageHome = new DataPart2StorageHome(DATA_STORE_2_IDENTIFIER);

            std::cout << "         -- Database_impl: create DataStorageHome for DataPart1 " <<DATA_STORE_3_IDENTIFIER<< std::endl;
            m_dataPart3StorageHome = new DataPart3StorageHome(DATA_STORE_3_IDENTIFIER);
        }
        else 
        {
            // Create DataStores with multicast StateTransfer
            std::cout << "         -- Database_impl: use multicast StateTransfer" << std::endl;

            std::cout << "         -- Database_impl: create DataStorageHome for DataPart1" << std::endl;
            m_dataPart1StorageHome = new DataPart1StorageHome(DATA_STORE_1_IDENTIFIER);

            std::cout << "         -- Database_impl: create DataStorageHome for DataPart2" << std::endl;
            m_dataPart2StorageHome = new DataPart2StorageHome(DATA_STORE_2_IDENTIFIER);

            std::cout << "         -- Database_impl: create DataStorageHome for DataPart1 " <<DATA_STORE_3_IDENTIFIER<< std::endl;
            m_dataPart3StorageHome = new DataPart3StorageHome(DATA_STORE_3_IDENTIFIER);
        }        
        
    } catch (CORBA::SystemException & e) {
         std::cout << "         -- Database_impl: SystemException: " << e << std::endl;
         // This is to avoid Assertion failure in debug mode as internal ref count
         // equals 1 (CORBA C++ mapping oblige!)
         _set_ref_count(0);
         throw;
    }
    
    std::cout << "         -- Database_impl: constructor end " << std::endl;
}


Database_impl::~Database_impl()
throw()
{
    delete m_dataPart1StorageHome;
    delete m_dataPart2StorageHome;
    delete m_dataPart3StorageHome;
}


PortableServer::POA_ptr Database_impl::_default_POA()
{
    return PortableServer::POA::_duplicate(m_poa.in());
}



Demo::Data Database_impl::get_data (Demo::Ident id)
    throw ( Demo::DataNotFound, CORBA::SystemException )
{
  std::cout << "------   Database_impl::get_data   -----" << std::endl;
    try {
        // get part1 of data
        Demo::DataPart1 dataPart1 = m_dataPart1StorageHome->find_data_by_oid(id);
        // get part2 of data
        Demo::DataPart2 dataPart2 = m_dataPart2StorageHome->find_data_by_oid(id);
        // get part3 of data
        Demo::DataPart1 dataPart3 = m_dataPart3StorageHome->find_data_by_oid(id);

        // reconstruct data
        Demo::Data data;
        data.id = id;
        data.part1 = dataPart1;
        data.part2 = dataPart2;
        data.part3 = dataPart3;
        
        return data;

    } catch (Cdmw::FT::NotFoundException& e) {
        throw Demo::DataNotFound(id);
    }
}


void Database_impl::set_data (const Demo::Data& d)
    throw ( CORBA::SystemException )
{
  std::cout << "------   Database_impl::set_data   -----" << std::endl;
    Demo::Ident id = d.id;
    
    // Store part1 of data
    try {
        // find if it is already in data store
        DataPart1StorageObject obj = m_dataPart1StorageHome->find_by_oid(id);
        obj.set(d.part1);

    } catch (Cdmw::FT::NotFoundException& e) {
        // part1 not found in DataStore. Create a new StorageObject.
        DataPart1StorageObject obj = m_dataPart1StorageHome->create(id, d.part1);
    }

    // Store part2 of data
    try {
        // find if it is already in data store
        DataPart2StorageObject obj = m_dataPart2StorageHome->find_by_oid(id);
        obj.set(d.part2);

    } catch (Cdmw::FT::NotFoundException& e) {
        // part2 not found in DataStore. Create a new StorageObject.
        DataPart2StorageObject obj = m_dataPart2StorageHome->create(id, d.part2);
    }

    // Store part3 of data
    try {
        // find if it is already in data store
        DataPart3StorageObject obj = m_dataPart3StorageHome->find_by_oid(id);
        obj.set(d.part3);

    } catch (Cdmw::FT::NotFoundException& e) {
        // part3 not found in DataStore. Create a new StorageObject.
        DataPart3StorageObject obj = m_dataPart3StorageHome->create(id, d.part3);
    }


}



