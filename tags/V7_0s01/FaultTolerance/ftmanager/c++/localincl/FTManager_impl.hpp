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


#ifndef __CDMW_FT_CDMW_FT_MANAGER_IMPL_HPP__
#define __CDMW_FT_CDMW_FT_MANAGER_IMPL_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/idllib/CdmwServiceLocator.skel.hpp"
#include "idllib/FT.stub.hpp"
#include "idllib/CdmwFTManager.skel.hpp"
#include "idllib/CdmwFTMembersAdmin.stub.hpp"
#include "idllib/CdmwFTSimpleMonitoring.skel.hpp"
#include "ftmanager/CdmwLocalFTManager.skel.hpp"
#include <map>
#include <vector>
//#include "FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp"
#include <FaultTolerance/ftcommon/IOGRFactory.hpp>
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>
#include "FaultTolerance/idllib/CdmwFTFaultDetector.stub.hpp"

#include "ftmanager/FaultDetector_impl.hpp"
#include "ftmanager/FaultNotifier_impl.hpp"
#include "Foundation/commonsvcs/datastore/StorageHome.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"


// define type_traits for ::CdmwFT::IOGRInfo (required by DataStore)
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::CdmwFT::IOGRInfo);
// define type_traits for ::CdmwFT::IOGRInfo (required by DataStore)
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::CdmwFT::FTManagerObjects);


namespace Cdmw {
namespace FT {

    class  FTManager_impl;

    class DataStoreObserver_impl:
            public ::Cdmw::CommonSvcs::DataStore::DataStoreObserver < std::string, CdmwFT::FTManagerObjects > 
    {
        
    public :
        DataStoreObserver_impl(FTManager_impl* ft_manager,
                               Cdmw::FT::DataStoreContext& context);
        
        void on_insert(_Oid_in_type oid, _Data_in_type data) throw ();
        
        void on_update(_Oid_in_type oid, _Data_in_type data) throw ();
        
        void on_remove(_Oid_in_type oid) throw ();

        void ft_manager_is_activated();
        
    private:
        
        bool m_ft_manager_is_activated;
        FTManager_impl* m_ft_manager;
        Cdmw::FT::DataStoreContext& m_context;
    };
    
/**
 *Purpose: 
 *<p> 
 *
 *Features:
 *<p> Thread safe
 *
 *@see  
 *
 */

class  FTManager_impl :
        public virtual POA_CdmwFT::LocalFTManager,
        public virtual PortableServer::RefCountServantBase
{

public:

    /**
     *Purpose:
     *<p> 
     */
    FTManager_impl(CORBA::ORB_ptr orb,
                   PortableServer::POA_ptr poa,
                   Cdmw::FT::FaultNotifier_impl* fault_notifier_impl,
                   Cdmw::FT::FaultManagement::FaultDetector_impl* fault_detector_impl,
                   ::CdmwFT::ReplicationManager_ptr replication_manager,
                   const char* ft_domain_id,
                   const char* localisation_port,
                   std::string xml_group_conf);

    /**
     *Purpose:
     *<p> 
     */
    ~FTManager_impl();
    



    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_fault_notifier:1.0
     * operation
     */
    ::FT::FaultNotifier_ptr get_fault_notifier()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_fault_detector:1.0
     * operation
     */
    CdmwFT::FaultManagement::FaultDetector_ptr get_fault_detector()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException );
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_replication_manager:1.0
     * operation
     */    
    CdmwFT::ReplicationManager_ptr get_replication_manager()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException );



    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/FTManager/get_FTManager_objects:1.0
     * operation
     */
    CdmwFT::FTManagerObjects* get_FTManager_objects
    (const char * master_location,
     CdmwFT::StateTransfer::DataStoreGroup_ptr datastore_group,
     const CdmwFT::LocationList & locations)
        throw(::FT::InterfaceNotFound,
              CORBA::SystemException);

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/LocalFTManager/activate:1.0
     * operation
     */
    void activate()
        throw( ::CdmwFT::FTManager::AlreadyActivated,
               CORBA::SystemException );
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/LocalFTManager/deactivate:1.0
     * operation
     */
    void deactivate()
        throw( CORBA::SystemException );

    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/LocalFTManager/add_location:1.0
     * operation
     */
    void add_location(const char * the_location)
        throw(::FT::MemberAlreadyPresent,
              ::FT::ObjectNotAdded,
              CORBA::SystemException);
    
    /**
     *Purpose:
     *<p>
     * Implements the
     * IDL:thalesgroup.com/CdmwFT/LocalFTManager/remove_location:1.0
     * operation
     */
    void remove_location(const char * the_location)
        throw(::FT::MemberNotFound,
              CORBA::SystemException );

    /**
     * register the mutual monitoring object 
     */
    void register_mutual_monitoring(CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoring_ptr mutual_monitoring)
        throw(CORBA::SystemException );

    
    /**
     * update mutual monitoring location from the datastore
     */
    void update_mutual_monitoring_locations()
        throw(CORBA::SystemException );
    
    
    /** 
     * return true if the manager is primary
     */
    CORBA::Boolean is_activated()
        throw(CORBA::SystemException);
    
    /**
     * return the reference of the object group for the id
     */
    CORBA::Object_ptr get_object_group(::FT::ObjectGroupId id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);
    /**
     * return the object group version for objects groups (all the objects have the same version)
     */
    ::FT::ObjectGroupRefVersion get_object_group_version(::FT::ObjectGroupId id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException);


    /**
     * dump data store for the specific object
     */
    void dump_datastore( ::CdmwFT::FaultManagerObject object )
        throw(CORBA::SystemException);


    /**
     * get_IOGR_dsid
     */
    int get_IOGR_dsid();
protected:

    
    CORBA::Object_ptr create_object_group(::CdmwFT::MemberInfos& members, 
										  ::FT::ObjectGroupId id,
										  const char* repid,
										  ::FT::ObjectGroupRefVersion object_group_ref_version,
                                          bool with_primary_location = false)
        throw(::FT::ObjectGroupNotFound,
              CORBA::NO_MEMORY,
              CORBA::SystemException );

    void set_primary()
        throw(CORBA::SystemException );
    
    void create_group(std::string xml_conf_file)
        throw(CORBA::SystemException );
    

private:
    
    // the fault notifier of the ft manager
    ::FT::FaultNotifier_var m_fault_notifier;

    Cdmw::FT::FaultNotifier_impl* m_fault_notifier_impl;

    // the fault notifier object group of the ft manager 
    ::FT::ObjectGroup_var m_fault_notifier_obj_group;    

    // the fault detecor of the ft manager
    CdmwFT::FaultManagement::FaultDetector_var m_fault_detector;    
    
    Cdmw::FT::FaultManagement::FaultDetector_impl* m_fault_detector_impl;
    
    // the fault detector object group of the ft manager
    ::FT::ObjectGroup_var m_fault_detector_obj_group;

    // the replication manageer of the ft manager 
    ::CdmwFT::ReplicationManager_var m_replication_manager;

    // the replication manager object group of the replication manager 
    ::FT::ObjectGroup_var m_replication_manager_obj_group;

    // the InterrogatingSimpleMonitoring 
    CdmwFT::MonitoringLibrary::SimpleMonitoring::InterrogatingSimpleMonitoring_var m_interrogatingSimpleMonitoring;

    // the mutual monitoring object
    CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoring_var m_mutual_monitoring;

	FTManagerStorageHome* m_ftManagerStorageHome;

	IOGRInfoStorageHome* m_iogrInfoStorageHome;

    // the orb 
    CORBA::ORB_var m_orb;
    
    // the poa 
    PortableServer::POA_var m_poa;

    // IOGR Factory
    Cdmw::FT::IOGRFactory* m_iogrFactory;

    // bool to know if the ft manager is activated or not
    bool m_is_activated;
    
    // the ft_domain_id
    CORBA::String_var m_ft_domain_id;

    // the localisation port
    std::string m_localisation_port;    

    // the name of the local host
    std::string m_hostname;
    
    // consumer id
    ::FT::FaultNotifier::ConsumerId m_consumerID;

    // sequence of all datastore reference for this process
    ::CdmwFT::StateTransfer::DataStoreGroup_var m_datastore_group;

    // data store observer for the FTManagerDataStore
    Cdmw::FT::DataStoreObserver_impl* m_dataStoreObserver;

    /**
    * The mutex protecting concurrent access to m_processes.
    */
    OsSupport::Mutex m_mutex;

    // ftmanager datastore
    FTManagerDataStore* m_FTManagerDataStoreObj;

    // xml group configuration file
    std::string m_xml_group_conf;
    
    
};
    

}  // End namespace FT
}  // End namespace Cdmw

#endif // __CDMW_FT_CDMW_FT_MANAGER_IMPL_HPP__

