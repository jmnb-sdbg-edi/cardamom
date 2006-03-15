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


#ifndef INCL_DATABASE_IMPL_HPP
#define INCL_DATABASE_IMPL_HPP


// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>

// Demo Files
#include <Database.skel.hpp>


// Define data_traits for fixed struct Demo::DataPart1 and Demo::DataPart2
// This is required to use a DataStore with those struct
DEFINE_FIXED_LENGTH_DATA_TRAITS(Demo::DataPart1);
DEFINE_FIXED_LENGTH_DATA_TRAITS(Demo::DataPart2);


class Database_impl : public virtual POA_Demo::Database,
                      public virtual Cdmw::OrbSupport::RefCountServantBase
{

public:

    // Constructor
    Database_impl(CORBA::ORB_ptr orb,
                  PortableServer::POA_ptr parentPOA,
                  bool pointToPoint);

    // Destructor
    virtual ~Database_impl()
        throw();

    // returns the POA of this servant
    virtual PortableServer::POA_ptr _default_POA();
        
        
    // IDL:thalesgroup.com/Demo/Database/get_data()
    virtual Demo::Data get_data (Demo::Ident id)
        throw( Demo::DataNotFound, CORBA::SystemException );

    // IDL:thalesgroup.com/Demo/Database/set_data()
    virtual void set_data (const Demo::Data& d)
        throw ( CORBA::SystemException );

        
private:

    // Hide copy constructor/assignment operator
    Database_impl(
        const Database_impl& rhs )
            throw();

    Database_impl& operator =(
        const Database_impl& rhs )
            throw();

    // Definition of Storage objects for DataPart1 type 
    typedef Cdmw::CommonSvcs::DataStore::StorageHome< Demo::Ident, Demo::DataPart1 > DataPart1StorageHome;    
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< Demo::Ident, Demo::DataPart1 > DataPart1StorageObject;
    typedef Cdmw::FT::DataStore< Demo::Ident, Demo::DataPart1 > DataPart1Store;
            
    // Definition of Storage objects for DataPart2 type 
    typedef Cdmw::CommonSvcs::DataStore::StorageHome< Demo::Ident, Demo::DataPart2 > DataPart2StorageHome;    
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< Demo::Ident, Demo::DataPart2 > DataPart2StorageObject;
    typedef Cdmw::FT::DataStore< Demo::Ident, Demo::DataPart2 > DataPart2Store;

    // Definition of Storage objects for DataPart1 type 
    typedef Cdmw::CommonSvcs::DataStore::StorageHome< Demo::Ident, Demo::DataPart1 > DataPart3StorageHome;    
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< Demo::Ident, Demo::DataPart1 > DataPart3StorageObject;
    typedef Cdmw::FT::DataStore< Demo::Ident, Demo::DataPart1 > DataPart3Store;

    // StorageHome for DataPart1
    DataPart1StorageHome* m_dataPart1StorageHome;
    
    // StorageHome for DataPart2
    DataPart2StorageHome* m_dataPart2StorageHome;
    
    // StorageHome for DataPart1
    DataPart3StorageHome* m_dataPart3StorageHome;

    // orb
    CORBA::ORB_var m_orb;

    // servant's POA
    PortableServer::POA_var m_poa;

    
}; // End class Database_impl 



#endif // INCL_DATABASE_IMPL_HPP


