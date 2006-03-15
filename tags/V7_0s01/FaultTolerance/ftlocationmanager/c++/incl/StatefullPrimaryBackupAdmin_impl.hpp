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


#ifndef INCL_CDMW_FT_LOCATION_STATEFULL_PRIMARY_BACKUP_ADMIN_IMPL_HPP
#define INCL_CDMW_FT_LOCATION_STATEFULL_PRIMARY_BACKUP_ADMIN_IMPL_HPP

// Cdmw Files
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <FaultTolerance/ftlocationmanager/PrimaryBackupAdmin_impl.hpp>


#include <idllib/CdmwFTMembersAdmin.skel.hpp>
#include <idllib/CdmwFTStateFullPrimaryBackupAdmin.skel.hpp>
#include <idllib/CdmwFTStateTransfer.stub.hpp>
#include <functional>
#include <list>
#include <set>
#include <map>

//
// IDL:thalesgroup.com/CdmwFT:1.0
//
namespace Cdmw
{

namespace FT
{

//
// IDL:thalesgroup.com/CdmwFT/Location:1.0
//
namespace Location
{

struct IDLocation
{
    ::CdmwFT::Transaction::DataStoreID id;
    std::string location;
};
    
struct  IDLocation_less
{
    bool operator() 
        (const IDLocation& x, 
         const IDLocation& y) const
    {
        if (x.id < y.id)
        {
            return true;
        }
        else if (x.id == y.id)
        {
            return(strcmp(x.location.c_str(), y.location.c_str()) < 0);
        }
        else 
            return false;
    };
};
    
    
//
// IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin:1.0
//
class StatefullPrimaryBackupAdmin_impl : virtual public POA_CdmwFT::Location::StatefullPrimaryBackupAdmin,
                                         public PrimaryBackupAdmin_impl
{

public:

    StatefullPrimaryBackupAdmin_impl(CORBA::ORB_ptr orb,
                            PortableServer::POA_ptr poa, 
                            const ::FT::Location& the_location,
                            PrimaryBackupGroupRepository_impl* group_repository,
                            bool cold_restart,
                            bool is_ftmanager = false);

    ~StatefullPrimaryBackupAdmin_impl();



    //
    // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/date_statefull_group_view:1.0
    //
    virtual void update_statefull_group_view(const CdmwFT::Location::StatefullGroupInfos& statefull_group_infos)
        throw(::FT::ObjectGroupNotFound,
              CdmwFT::Location::UnknownDataStoreID,
              CdmwFT::StateTransfer::InvalidDataStoreState,
              CORBA::SystemException);

    //
    // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/update_statefull_primary:1.0
    //
    virtual void update_statefull_primary(const CdmwFT::Location::StatefullGroupInfos&  statefull_group_infos,
                                          const ::FT::Location& primary_location)
        throw(::FT::ObjectGroupNotFound,
              CdmwFT::Location::UnknownDataStoreID,
              CdmwFT::StateTransfer::InvalidDataStoreState,
              CORBA::SystemException);
            
    //
    // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/init_backup_data_stores:1.0
    //
    virtual void init_backup_data_stores(const CdmwFT::StateTransfer::DataStoreInfo& primary_data_store_infos)
        throw(CdmwFT::Location::InitFailed,
              CdmwFT::Location::AlreadyDone,
              CORBA::SystemException);
          
    //
    // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/init_primary_data_stores:1.0
    //
    virtual void init_primary_data_stores(const CdmwFT::StateTransfer::LocalDataStoreInfos& local_datastore_infos)
        throw(CdmwFT::Location::InitFailed,
              CdmwFT::Location::AlreadyDone,
              CORBA::SystemException);
         
    //
    // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/handle_backup_insertion_failure:1.0
    //
    virtual void handle_backup_insertion_failure(const ::FT::Location& failed_location)
        throw(CORBA::SystemException);



private:
    StatefullPrimaryBackupAdmin_impl();
    StatefullPrimaryBackupAdmin_impl(const StatefullPrimaryBackupAdmin_impl&);
    void operator=(const StatefullPrimaryBackupAdmin_impl&);

    void update_from_group_info(const CdmwFT::Location::StatefullGroupInfos&  statefull_group_infos,
                                bool is_update_primary = false)
        throw(::FT::ObjectGroupNotFound,
              CdmwFT::Location::UnknownDataStoreID,
              CdmwFT::StateTransfer::InvalidDataStoreState,
              CORBA::SystemException);
    

    // list of datastore id already initialised
    typedef std::set < ::CdmwFT::Transaction::DataStoreID >  DataStoreIDSet;
    DataStoreIDSet m_init_primary_datastore_id;
    DataStoreIDSet m_init_backup_datastore_id;

    typedef std::set < std::string > LocationSet;
    typedef std::map < ::CdmwFT::Transaction::DataStoreID, LocationSet> LocationMapForDSID;
    LocationMapForDSID m_nominal_location;
    LocationSet m_remove_location;
    LocationMapForDSID m_add_location;
    LocationMapForDSID m_temp_location;
    
    typedef std::map <IDLocation, 
                      ::CdmwFT::StateTransfer::LocalDataStoreInfo,
                      IDLocation_less > InfoMap;
    InfoMap m_infoMap;

    PortableServer::POA_var m_poa;

    // repository containing group information and the list of primary members
    PrimaryBackupGroupRepository_impl* m_primary_backup_group_repository;

    bool          m_is_ftmanager; 

    // is cold or hot initialisation
    // 0 is for the cold 
    // 1 is for the hot
    bool m_init_type;

    std::string m_primary_location;

    ::FT::Location m_the_location;
};

} // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

#endif  // INCL_CDMW_FT_LOCATION_STATEFULL_PRIMARY_BACKUP_ADMIN_IMPL_HPP

