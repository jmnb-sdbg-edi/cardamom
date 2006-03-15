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


#include "Foundation/ossupport/OS.hpp"
#include "Foundation/osthreads/MutexGuard.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "ftmanager/FTManager_impl.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "FaultTolerance/idllib/CdmwFTCommon.stub.hpp"
#include "FaultTolerance/ftstatemanager/DataStoreGroup_impl.hpp"
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>
#include <sstream>
#include <ftreplicationmanager/CdmwFTReplicationManagerPushConsumer.stub.hpp>
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>
#include "FaultTolerance/ftstatemanager/TXDataStore.hpp"
#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>
#include <ftgroupcreator/GroupConfigurationMngr.hpp>





#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>
#else
#error "Unsupported ORB"
#endif

namespace 
{
    const char* NOTIFIER_NAME = "fault_notifier";
    const char* REPLICATION_MANAGER_NAME = "replication_mgr";
    const char* DETECTOR_NAME = "fault_detector";

    const char* REP_ID_REPL_MNGT = "IDL:thalesgroup.com/CdmwFT/ReplicationManagerPushConsumer:1.0";
    const char* REP_ID_FAULT_NOTIFIER = "IDL:omg.org/FT/FaultNotifier:1.0";    
    const char* REP_ID_FAULT_DETECTOR = "IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector:1.0";

    // Ids run from 1 to MAX_BUILTIN_DATASTORE_ID
    const CORBA::ULong MAX_BUILTIN_DATASTORE_ID = 12;


	const ::FT::ObjectGroupId ID_REPL_MNGT = 0xffffff01L;
	const ::FT::ObjectGroupId ID_FAULT_NOTIFIER = 0xffffff02L;
	const ::FT::ObjectGroupId ID_FAULT_DETECTOR = 0xffffff03L;



bool FTMObjectsTrueFunction(const std::pair< std::string, CdmwFT::FTManagerObjects >&elt)
{
    return true;    
}

// unary function to realise the construction of membersinfos
class BuildMembersInfos :  
    public std::unary_function<std::pair< const Cdmw::OrbSupport::type_traits<std::string>::_var_type , 
                                          Cdmw::OrbSupport::type_traits<CdmwFT::FTManagerObjects>::_var_type > , 
                               void>
{
public:
    BuildMembersInfos( ::CdmwFT::MemberInfos& replication_manager_members,
                       ::CdmwFT::MemberInfos& fault_notifier_members,
                       ::CdmwFT::MemberInfos& fault_detector_members)
        : m_replication_manager_members(replication_manager_members),
          m_fault_notifier_members(fault_notifier_members),
          m_fault_detector_members(fault_detector_members),
          iter(0)
    {}

    void operator()(const std::pair< const Cdmw::OrbSupport::type_traits<std::string>::_var_type, 
                                     Cdmw::OrbSupport::type_traits<CdmwFT::FTManagerObjects>::_var_type> & elt)
    {
        CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"Create memberInfo for the location: "<<elt.first);
        ::FT::Location loc;
        loc.length(1);
        loc[0].id=elt.first.c_str();
        m_replication_manager_members[iter].the_reference = ::CdmwFT::ReplicationManager::_duplicate(elt.second->replication_manager.in());
        m_replication_manager_members[iter].the_location = loc;
        m_fault_notifier_members[iter].the_reference = ::FT::FaultNotifier::_duplicate(elt.second->fault_notifier.in());
        m_fault_notifier_members[iter].the_location = loc;
        m_fault_detector_members[iter].the_reference = CdmwFT::FaultManagement::FaultDetector::_duplicate(elt.second->fault_detector.in());
        m_fault_detector_members[iter].the_location = loc;
        iter++;
    }
    

private:
    ::CdmwFT::MemberInfos& m_replication_manager_members;
    ::CdmwFT::MemberInfos& m_fault_notifier_members;
    ::CdmwFT::MemberInfos& m_fault_detector_members;
    int iter;
};



// unary function to realise on failure on the failed location 
class OnFailureOnDataStore :  
    public std::unary_function<std::pair< const Cdmw::OrbSupport::type_traits<std::string>::_var_type , 
                                          Cdmw::OrbSupport::type_traits<CdmwFT::FTManagerObjects>::_var_type > , 
                               void>
{
public:
    typedef std::set<std::string> locationList;
    OnFailureOnDataStore( locationList& locations, locationList *removeList)
        : m_locations(locations),
          m_removeList(removeList)
    {}

    void operator()(const std::pair< const Cdmw::OrbSupport::type_traits<std::string>::_var_type, 
                                     Cdmw::OrbSupport::type_traits<CdmwFT::FTManagerObjects>::_var_type> & elt)
    {
        // realise on_failure on the location already in the datastore but unknow by the mutual monitoring
        locationList::iterator iter = m_locations.find(elt.first.c_str());
        if ((iter == m_locations.end()) && (strcmp(elt.first.c_str(),Cdmw::OsSupport::OS::get_hostname().c_str())!=0 ))
        {
            try
            {
                Cdmw::FT::DataStoreBase::On_Failure(elt.first.c_str());
                m_removeList->insert(elt.first);
            }
            catch(...)
            {}
        }
    }
    

private:
    locationList m_locations;
    locationList* m_removeList;
};

// unary function to realise on failure on the failed location 
class StartedLocations :  
    public std::unary_function<std::pair< const Cdmw::OrbSupport::type_traits<std::string>::_var_type , 
                                          Cdmw::OrbSupport::type_traits<CdmwFT::FTManagerObjects>::_var_type > , 
                               void>
{
public:
    StartedLocations(CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoring_ptr mutual_monitoring)
        : m_mutual_monitoring(CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoring::_duplicate(mutual_monitoring))
    {}

    void operator()(const std::pair< const Cdmw::OrbSupport::type_traits<std::string>::_var_type, 
                                     Cdmw::OrbSupport::type_traits<CdmwFT::FTManagerObjects>::_var_type> & elt)
    {
        m_mutual_monitoring->add_location(elt.first.c_str());
    }
    

private:

    // the mutual monitoring object
    CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoring_var m_mutual_monitoring;
};





} // end anonymous namespace


namespace Cdmw {
namespace FT {
        
    DataStoreObserver_impl::DataStoreObserver_impl
    (FTManager_impl* ft_manager,
     Cdmw::FT::DataStoreContext& context)
        : m_ft_manager(ft_manager),
          m_context(context)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_ft_manager_is_activated = false;
    }
    
    
    void DataStoreObserver_impl::ft_manager_is_activated()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_ft_manager_is_activated = true;
    }
        
    void DataStoreObserver_impl::on_insert(_Oid_in_type oid, _Data_in_type data) throw ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        if ((strcmp(OsSupport::OS::get_hostname().c_str(), oid.c_str()) != 0) && !m_ft_manager_is_activated)
        {
            for (CORBA::ULong i = 1; i < (MAX_BUILTIN_DATASTORE_ID + 1); i ++)
            {
                try 
                {
                    m_context.add_location(i,oid);
                }
                catch(...)
                {
                    CDMW_DEBUG(FTLogger::GetLogger(),"Received exception in on_insert()");
                }
            }
            m_context.add_ref_for_location(oid,
                                           data.local_data_store_info.coordinator,
                                           data.local_data_store_info.cohort);

        }
        m_ft_manager->update_mutual_monitoring_locations();
    }
    
        
    void DataStoreObserver_impl::on_update(_Oid_in_type oid, _Data_in_type data) throw ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    }
    
        
    void DataStoreObserver_impl::on_remove(_Oid_in_type oid) throw ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        if ((strcmp(OsSupport::OS::get_hostname().c_str(), oid.c_str()) != 0) && !m_ft_manager_is_activated)
        {
            Cdmw::FT::DataStoreBase::On_Failure(oid);
        }
        m_ft_manager->update_mutual_monitoring_locations();
    }
    


//
// Constructor
//
FTManager_impl::FTManager_impl(CORBA::ORB_ptr orb,
                               PortableServer::POA_ptr poa,
                               Cdmw::FT::FaultNotifier_impl* fault_notifier_impl,
                               Cdmw::FT::FaultManagement::FaultDetector_impl* fault_detector_impl,
                               ::CdmwFT::ReplicationManager_ptr replication_manager,
                               const char* ft_domain_id,
                               const char* localisation_port,
                               std::string xml_group_conf)
    :m_fault_notifier_impl(fault_notifier_impl),
     m_fault_notifier_obj_group(::FT::ObjectGroup::_nil()),
     m_fault_detector_impl(fault_detector_impl),
     m_fault_detector_obj_group(::FT::ObjectGroup::_nil()),
     m_replication_manager(::CdmwFT::ReplicationManager::_duplicate(replication_manager)),
     m_replication_manager_obj_group(::FT::ObjectGroup::_nil()),
     m_interrogatingSimpleMonitoring(CdmwFT::MonitoringLibrary::SimpleMonitoring::InterrogatingSimpleMonitoring::_nil()),
     m_orb(CORBA::ORB::_duplicate(orb)),
     m_poa(PortableServer::POA::_duplicate(poa)),
     m_is_activated(false),
     m_ft_domain_id(CORBA::string_dup(ft_domain_id)),
     m_localisation_port(localisation_port),
     m_xml_group_conf(xml_group_conf)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                      "ft_domain_id="<<ft_domain_id<<",localisation_port="<<localisation_port);
    m_hostname = OsSupport::OS::get_hostname();
    
    CDMW_DEBUG(FTLogger::GetLogger(),m_hostname << ": FTManager creation"); 
    
    m_fault_notifier = m_fault_notifier_impl->_this();
    m_fault_detector = m_fault_detector_impl->_this();    

    // Create the IOGR factory
    m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(m_orb.in(),
                                                  std::cout);
    

	m_iogrInfoStorageHome = new IOGRInfoStorageHome(IOGR_INFOS_DATA_STORE);

	m_ftManagerStorageHome = new FTManagerStorageHome(FT_MANAGER_DATA_STORE);

    Cdmw::FT::TXDataStoreBase* dsBase = Cdmw::FT::TXDataStoreBase::Get_dataStore(FT_MANAGER_DATA_STORE);
    CDMW_ASSERT(dsBase != 0);
        

    FTManagerDataStore* dataStoreObj = dynamic_cast< FTManagerDataStore* >(dsBase);
    CDMW_ASSERT(dataStoreObj != 0);

    m_dataStoreObserver = new DataStoreObserver_impl(this, dsBase->get_manager().get_context());



    //  register the previous observer
    bool registered = dataStoreObj->register_observer(m_dataStoreObserver);
    if ( ! registered) {
        // ###### ?????
    }

    m_datastore_group =
    (new Cdmw::FT::DataStoreGroup_impl(m_poa.in(),dsBase->get_manager().get_context()))->_this();


    dsBase = Cdmw::FT::TXDataStoreBase::Get_dataStore(FT_MANAGER_DATA_STORE);
    CDMW_ASSERT(dsBase != 0);
            
    m_FTManagerDataStoreObj = dynamic_cast< FTManagerDataStore* >(dsBase);
    CDMW_ASSERT(m_FTManagerDataStoreObj != 0);
}

    //
    // destructor
    //
    FTManager_impl::~FTManager_impl()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    }


    int FTManager_impl::get_IOGR_dsid()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_LOG_FUNCTION_RETURN(IOGR_INFOS_DATA_STORE);
        return IOGR_INFOS_DATA_STORE;
    }
    

    //
    // return the fault notifier of the ftmanager
    //
    ::FT::FaultNotifier_ptr FTManager_impl::get_fault_notifier()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException )
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try
        {       
            // get a storage object for the fault notifier
            IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
            // get the data
            ::CdmwFT::IOGRInfo_var iogrInfo = objIOGRInfo.get();
        
            ::FT::FaultNotifier_var fn = ::FT::FaultNotifier::_narrow(iogrInfo->fault_notifier.in());
        
            // return the reference of the object group
            return fn._retn();
        }
        catch(const Cdmw::FT::NotFoundException &)
        {     
            CDMW_WARN(FTLogger::GetLogger(),
                      "Can not find the FaultNotifier object!");
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }

    //
    // return the fault detector of the ftmanager
    //
    CdmwFT::FaultManagement::FaultDetector_ptr FTManager_impl::get_fault_detector()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException )
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
 
        try
        {
            // get a storage object for the fault notifier
            IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
            // get the data
            ::CdmwFT::IOGRInfo_var iogrInfo = objIOGRInfo.get();
            CdmwFT::FaultManagement::FaultDetector_var fd = CdmwFT::FaultManagement::FaultDetector::_narrow(iogrInfo->fault_detector.in());
            // return the reference of the object group
            return fd._retn();
        }
        catch(const Cdmw::FT::NotFoundException &)
        {        
            CDMW_WARN(FTLogger::GetLogger(),
                      "Can not find the FaultDetector object!");
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }

    //
    // return the fault notifier of the ftmanager
    //
    CdmwFT::ReplicationManager_ptr FTManager_impl::get_replication_manager()
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException )
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
 
        try
        {
            // get a storage object for the fault notifier
            IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
            // get the data
            ::CdmwFT::IOGRInfo_var iogrInfo = objIOGRInfo.get();
            CdmwFT::ReplicationManager_var rm = CdmwFT::ReplicationManager::_narrow(iogrInfo->replication_manager.in());
            // return the reference of the object group
            return rm._retn();
        }
        catch(const Cdmw::FT::NotFoundException &)
        {        
            CDMW_WARN(FTLogger::GetLogger(),"Can not find the ReplicationManager object!");
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
 

    //
    // Return the FT manager objects
    CdmwFT::FTManagerObjects *
    FTManager_impl::get_FTManager_objects
    (const char * master_location,
     CdmwFT::StateTransfer::DataStoreGroup_ptr datastore_group,
     const CdmwFT::LocationList & locations)
        throw(::FT::InterfaceNotFound,
              CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"master_location="<<master_location);

        // for each datastore on this process, realise un update with the coordinator
        // and set location for each datastore
        std::string location = OsSupport::OS::get_hostname();
        FTManagerDataStore* dataStoreObj = NULL;
        CdmwFT::StateTransfer::DataStoreIDs ids;

        if (strcmp(master_location, location.c_str())!= 0)
        { 
            std::set<std::string> locationList;

            ids.length(MAX_BUILTIN_DATASTORE_ID);
        
            Cdmw::FT::TXDataStoreBase* dsBase = Cdmw::FT::TXDataStoreBase::Get_dataStore(FT_MANAGER_DATA_STORE);
            CDMW_ASSERT(dsBase != 0);
        
        
            dataStoreObj = dynamic_cast< FTManagerDataStore* >(dsBase);
            CDMW_ASSERT(dataStoreObj != 0);
        

            for (CORBA::ULong index = 0 ; index < locations.length(); index ++)
            {
                locationList.insert(std::string(locations[index]));
            }
        
            for (CORBA::ULong i = 1; i < (MAX_BUILTIN_DATASTORE_ID + 1); i++)
            {
                dataStoreObj->get_manager().get_context().set_locations(i, locationList);
                ids[i - 1] = i;
            }

            try
            {
                dataStoreObj->get_manager().get_context().update_with_coordinator(datastore_group, location,
                                                                                  ids);
            }
            catch(const CdmwFT::StateTransfer::InvalidDataStoreState&){
                CDMW_ERROR(FTLogger::GetLogger(),"FTManager_impl::get_FTManager_objects()<" 
                           << location << "> InvalidDataStoreState\n"
                           << "Raising CORBA::INTERNAL exception.");
                CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL
                                   , CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            catch(CORBA::SystemException& ex)
            {
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            }
            //         catch(...)
            //         {
            //             throw;
            //         }
        }

        CdmwFT::FTManagerObjects_var objects = new CdmwFT::FTManagerObjects();
    
        if (CORBA::is_nil(m_fault_notifier.in())) {
            CDMW_ERROR(FTLogger::GetLogger(), "FTManager - Nil object reference of FaultNotifier!");
            ::FT::InterfaceNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        objects->fault_notifier = ::FT::FaultNotifier::_duplicate(m_fault_notifier.in());


        if (CORBA::is_nil(m_fault_detector.in())) {
            CDMW_DEBUG(FTLogger::GetLogger(), "FTManager - Nil object reference of FaultDetector!");
            ::FT::InterfaceNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        objects->fault_detector = CdmwFT::FaultManagement::FaultDetector::_duplicate(m_fault_detector.in());

        if (CORBA::is_nil(m_replication_manager.in())) {
            CDMW_DEBUG(FTLogger::GetLogger(),"FTManager - Nil object reference of ReplicationManager!");
            ::FT::InterfaceNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        objects->replication_manager = ::CdmwFT::ReplicationManager::_duplicate(m_replication_manager.in());     

        std::string corbaloc = ("corbaloc::");
        corbaloc += OsSupport::OS::get_hostname().c_str();
        corbaloc += ":";        
        corbaloc += m_localisation_port;
        corbaloc +="/fault_manager";

        CORBA::Object_var object = m_orb->string_to_object(corbaloc.c_str());
        CdmwFT::FTManager_var ftManager;
        // TODO! FIXME!!!!
        try
        {
            ftManager= CdmwFT::FTManager::_narrow(object.in());
       
            if (CORBA::is_nil(ftManager.in())) {
                CDMW_ERROR(FTLogger::GetLogger(),"FTManager - Nil object reference of FTManager! "
                           << "Corbaloc='" << corbaloc << "'");
                throw ::FT::InterfaceNotFound();
            }
        }
        catch(const CORBA::Exception & e)
        {
            CDMW_ERROR(FTLogger::GetLogger(),"FTManager - FTManager object not found! "
                       << "Corbaloc='" << corbaloc << "' "
                       << "Exception=" << e);   
            ::FT::InterfaceNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;   
        } 

        objects->ft_manager = ::CdmwFT::FTManager::_duplicate(ftManager.in());

        CdmwFT::StateTransfer::LocalDataStoreInfo localDataStoreInfo;

        if (strcmp(master_location, location.c_str())!= 0)
        { 
            CdmwFT::StateTransfer::DataStoreGroup_var ftdatastoregroup_ref = 
            (new Cdmw::FT::DataStoreGroup_impl(m_poa.in(), dataStoreObj->get_manager().get_context()))->_this();
        
            localDataStoreInfo.local_data_store = CdmwFT::StateTransfer::DataStoreGroup::_duplicate(ftdatastoregroup_ref.in());
            localDataStoreInfo.coordinator = CdmwFT::Transaction::TPCCoordinator::_duplicate(dataStoreObj->get_manager().get_context().get_tpc_coordinator());
            localDataStoreInfo.cohort = CdmwFT::Transaction::TPCCohort::_duplicate(dataStoreObj->get_manager().get_context().get_tpc_cohort());
            localDataStoreInfo.dsids = ids;
        }
    
        objects->local_data_store_info = localDataStoreInfo;
    

        return objects._retn();
    
    }

    // 
    // this function is call by the mutual monitoring observer when the ftmanager is primary
    //
    void FTManager_impl::activate()
        throw( CdmwFT::FTManager::AlreadyActivated,
               CORBA::SystemException )
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        if (m_is_activated) {
            CdmwFT::FTManager::AlreadyActivated ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;   
        }


        try
        {
            
            m_dataStoreObserver->ft_manager_is_activated();

            bool first_primary = false;
            // activate all data store


            ::FT::Location old_primary_location;
            ::CdmwFT::IOGRInfo_var iogrInfo;

            if (!m_ftManagerStorageHome->contains(OsSupport::OS::get_hostname().c_str())) {
                // if the location is not present, realise an add_location
                first_primary = true;
            }

            if (!first_primary)
            {
                try
                {
                    IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
                    // get the data
                    iogrInfo = objIOGRInfo.get();
                    old_primary_location = iogrInfo->the_location;
                    DataStoreBase::On_Failure(std::string(old_primary_location[0].id));
                }
                catch (const Cdmw::FT::NotFoundException &)
                {
                }
            } 
            

            Cdmw::FT::DataStoreBase::Activate_All();


            m_FTManagerDataStoreObj->get_manager().begin();

            if (!first_primary)
            {
                try
                {
                   m_ftManagerStorageHome->remove(std::string(old_primary_location[0].id));
               }
                catch (const Cdmw::FT::NotFoundException &)
                {
                }
            }
 
            if (first_primary) {
                // if the location is not present, realise an add_location
                first_primary = true;
                add_location(OsSupport::OS::get_hostname().c_str());
            }





    

    
            // TODO ******* put into the schduler
            //         // get all the locations already started (by some of there locations may be not added)
    
            //         {
            //             ::CdmwFT::MonitoringLibrary::Locations_var locations = m_mutual_monitoring->get_started_locations();
            //             std::set<std::string> locationList;
        
            //             for (CORBA::ULong index = 0 ; index < locations->length(); index ++)
            //             {
            //                 locationList.insert(std::string(locations[index]));
            //                 if (!m_ftManagerStorageHome->contains(std::string(locations[index]))) {
            //                     // if the location is not present, realise an add_location
            //                     add_location(std::string(locations[index]).c_str());
            //                 }
            //             }
        
        
            //             std::set<std::string>* removeList = new std::set<std::string>(); 
            //             m_ftManagerStorageHome->for_each(OnFailureOnDataStore( locationList, removeList));
        
            //             for (std::set<std::string>::iterator iter = removeList->begin();
            //                  iter != removeList->end();
            //                  iter ++)
            //             {
            //                 remove_location((*iter).c_str());
            //             }   
            //         }
    
            set_primary();


            //commit multiple transaction  
            m_FTManagerDataStoreObj->get_manager().commit();


            // register the replication manager into the ORB
            m_orb->register_initial_reference( "ReplicationManager", m_replication_manager.in());

            // register the fault notifier into the replication manager
            m_replication_manager->register_fault_notifier( ::FT::FaultNotifier::_narrow(get_fault_notifier()));

            // register the fault detector into the replication manager
            m_replication_manager->register_fault_detector( m_fault_detector.in());

            //         // Connect the replication manager as a structured push consumer
            //         DEBUG_ECHO("connect the consumer");
            //         CosNotifyComm::StructuredPushConsumer_var rm_consumer
            //         = CosNotifyComm::StructuredPushConsumer::_narrow(m_replication_manager.in());
        
            //         m_consumerID = m_fault_notifier_impl->connect_structured_fault_consumer(rm_consumer.in());

            // activate the fault detector
            m_fault_detector_impl->activate();

            if (!first_primary)
            {
                // change fallback in the object group data
                m_replication_manager->update_fallback(old_primary_location);
            }


            // m_is_activated is equal to true here for not realise an update on all the ftmanager for each previous add_location
            // the update will be realise at the end of the function
            m_is_activated = true;

            if (first_primary) {
                //create the object group from the configuration file
                if (!m_xml_group_conf.empty())
                    create_group(m_xml_group_conf);
            }

            // ATTENTION :
            // the corbaloc is not possible with an IOGR object because, in the client side,
            // when the client realise a narrow of the object with the corbaloc, 
            // the request is send without the FT_REQUEST service context because the IOGR object is not exptected.
            // when the narrow realise a location forward of the good reference, the reference is an IOGR, so the 
            // request is intercept by the interceptor but the request haven't the FT_REQUEST because the request of
            // the context in only realise the first time
            // consequently, the FT_REQUEST is missing

            //     OrbSupport::OrbSupport::bind_object_to_corbaloc(
            //             m_orb.in(), DETECTOR_NAME, m_fault_detector_obj_group.in());
    
            //     // display corbaloc 

            //     std::cout << "corbaloc::" << OsSupport::OS::get_hostname()
            //               << ":" << m_localisation_port
            //               << "/" << DETECTOR_NAME
            //               << std::endl;


            std::cout<<"[FTManager] : The FT Manager is activated"<<std::endl;
            Cdmw::OsSupport::OS::Timeval time = Cdmw::OsSupport::OS::get_time();
            std::cout << "########## FT Manager is active. Date: " 
                      << time.seconds << "sec " << time.microseconds << "microsec" 
                      << std::endl; 
        }
    
        catch(...)
        {
            //rolback multiple transaction
            //m_FTManagerDataStoreObj->get_manager().rollback();
            throw;
        }
    
        CDMW_INFO(FTLogger::GetLogger(),"The FT Manager is activated");
    }
    
    //
    // this function is called by the mutual monitoring observer when the ftmanager is elected as  backup
    //
    void FTManager_impl::deactivate()
        throw( CORBA::SystemException )
    {
        // NEVER CALLED
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_is_activated = false;    
    }
    
    //
    // this function is call by the mutual monitoring observer when a new host is find and is monitored
    //
    void FTManager_impl::add_location(const char * the_location)
        throw(::FT::MemberAlreadyPresent,
              ::FT::ObjectNotAdded,
              CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
        bool allow_commit_rollback = false;
        try
        {
            //begin multiple transaction
            if (!m_FTManagerDataStoreObj->get_manager().in_transaction())
            {
                allow_commit_rollback = true;
                m_FTManagerDataStoreObj->get_manager().begin();
            } else {
                allow_commit_rollback = false;
            }
        

            CDMW_MUTEX_GUARD(m_mutex);
            if (m_ftManagerStorageHome->contains(the_location)) {
                // if the location is found 
                CDMW_ERROR(FTLogger::GetLogger(),"Member '" << the_location << "' already present!");
                ::FT::MemberAlreadyPresent ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }

            // construct the corbaloc of the fault_manager
            std::string corbaloc = ("corbaloc::");
            corbaloc += the_location;
            corbaloc += ":";        
            corbaloc += m_localisation_port;
            corbaloc +="/fault_manager";
            std::cout<<"Corbaloc: "<<corbaloc<<std::endl;
    
            CORBA::Object_var object = m_orb->string_to_object(corbaloc.c_str());
            CdmwFT::FTManager_var ftManager;
       
            try
            {
                ftManager= CdmwFT::FTManager::_narrow(object.in());
        
                if (CORBA::is_nil(ftManager.in()))  
                {
                    CDMW_ERROR(FTLogger::GetLogger(),"FTManager - Nil object reference of FTManager! "
                               << "Corbaloc='" << corbaloc << "'");
                    throw  ::FT::ObjectNotAdded();    
                }
                
            }
            catch(const CORBA::Exception & e)
            {
                CDMW_ERROR(FTLogger::GetLogger(),"FTManager - FTManager object not found! "
                           << "Corbaloc='" << corbaloc << "' "
                           << "Exception=" << e);
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;   
            }     

            ::FT::Location loc;
            loc.length(1);
            loc[0].id=the_location;
    
            int membersSize = 0;  
            CDMW_DEBUG(FTLogger::GetLogger(),"Get information for the new FTManager.");
            try 
            {       
                CdmwFT::FTManagerObjects objects_tmp;
                if (strcmp(the_location,OsSupport::OS::get_hostname().c_str()) == 0)
                {
                    objects_tmp.replication_manager = ::CdmwFT::ReplicationManager::_duplicate(m_replication_manager.in());
                    objects_tmp.fault_notifier = ::FT::FaultNotifier::_duplicate(m_fault_notifier.in());
                    objects_tmp.fault_detector = CdmwFT::FaultManagement::FaultDetector::_duplicate(m_fault_detector.in());   
                }
                else
                {
                    DataStoreContext::LocationList list = m_FTManagerDataStoreObj->get_manager().get_context().get_locations(1);
                    CdmwFT::LocationList locations;
                    locations.length(list.size());
                    int inc = 0;
                    for ( DataStoreContext::LocationList::iterator iter = list.begin();
                          iter != list.end();
                          iter ++)
                    {
                        locations[inc] = (*iter).c_str();
                        inc++;
                    }

                    CdmwFT::FTManagerObjects* objects = NULL;
            
                    objects = ftManager->get_FTManager_objects(OsSupport::OS::get_hostname().c_str(),
                                                               m_datastore_group.in(),
                                                               locations);
            
                    objects_tmp.replication_manager = ::CdmwFT::ReplicationManager::_duplicate(objects->replication_manager.in());
                    objects_tmp.fault_notifier = ::FT::FaultNotifier::_duplicate(objects->fault_notifier.in());
                    objects_tmp.fault_detector = CdmwFT::FaultManagement::FaultDetector::_duplicate(objects->fault_detector.in());
            
                    CdmwFT::StateTransfer::LocalDataStoreInfo localDataStoreInfo;
                    localDataStoreInfo.local_data_store 
                    = CdmwFT::StateTransfer::DataStoreGroup::_duplicate(objects->local_data_store_info.local_data_store.in());

                    localDataStoreInfo.coordinator 
                    = CdmwFT::Transaction::TPCCoordinator::_duplicate(objects->local_data_store_info.coordinator.in());
            
                    localDataStoreInfo.cohort 
                    = CdmwFT::Transaction::TPCCohort::_duplicate(objects->local_data_store_info.cohort.in());

                    localDataStoreInfo.dsids = objects->local_data_store_info.dsids;
                    objects_tmp.local_data_store_info = localDataStoreInfo;    
                }

            
                m_ftManagerStorageHome->create(the_location, objects_tmp);
                membersSize = m_ftManagerStorageHome->size();
                
            }
            catch (const ::FT::InterfaceNotFound & e)
            {
                CDMW_ERROR(FTLogger::GetLogger(),"FTManager - received CORBA::Exception ["<<e <<"] raising "
                           <<"::FT::ObjectNotAdded()");
                ::FT::ObjectNotAdded ex;       
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;            
            }
            catch(const CORBA::Exception& e)
            {
                CDMW_ERROR(FTLogger::GetLogger(),"FTManager - received CORBA::Exception ["<<e <<"] raising "
                           <<"::FT::ObjectNotAdded()");
                ::FT::ObjectNotAdded ex;       
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;        
            }
        
    
            CDMW_DEBUG(FTLogger::GetLogger(),"Update data store");
            //create members infos
            ::CdmwFT::MemberInfos replication_manager_members;
            replication_manager_members.length(membersSize);

            ::CdmwFT::MemberInfos fault_notifier_members;
            fault_notifier_members.length(membersSize);

            ::CdmwFT::MemberInfos fault_detector_members;
            fault_detector_members.length(membersSize);
 
            m_ftManagerStorageHome->for_each(BuildMembersInfos(replication_manager_members,
                                                               fault_notifier_members,
                                                               fault_detector_members));

            ::FT::ObjectGroupRefVersion iorgs_version = 0;
            ::CdmwFT::IOGRInfo_var iogrInfo = new ::CdmwFT::IOGRInfo();
            bool first_creation = false;
            try
            {
                IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
                // get the data
                iogrInfo = objIOGRInfo.get();
                iorgs_version = iogrInfo->version;
        
        
            }
            catch (const Cdmw::FT::NotFoundException &)
            {
                first_creation = true;
            }

            ::FT::ObjectGroup_var rmog;
            ::FT::ObjectGroup_var fnog;
            ::FT::ObjectGroup_var fdog;

            rmog = create_object_group(replication_manager_members,
                                       ID_REPL_MNGT,
                                       REP_ID_REPL_MNGT,
                                       iorgs_version);
    
    
            fnog = create_object_group(fault_notifier_members,
                                       ID_FAULT_NOTIFIER,
                                       REP_ID_FAULT_NOTIFIER,
                                       iorgs_version);
    
            fdog = create_object_group(fault_detector_members,
                                       ID_FAULT_DETECTOR,
                                       REP_ID_FAULT_DETECTOR,
                                       iorgs_version);
            ++iorgs_version;

    
            iogrInfo->replication_manager = ::FT::ObjectGroup::_duplicate(rmog.in());
            iogrInfo->fault_notifier = ::FT::ObjectGroup::_duplicate(fnog.in());
            iogrInfo->fault_detector = ::FT::ObjectGroup::_duplicate(fdog.in());
            iogrInfo->version = iorgs_version;

            ::FT::Location primary_location;
            primary_location.length(1);
            primary_location[0].id=OsSupport::OS::get_hostname().c_str();
            iogrInfo->the_location = primary_location;
            try
            {
                if (first_creation)
                {
                    m_iogrInfoStorageHome->create(1, iogrInfo.in());
                }
                else
                {
                    IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);            
                    objIOGRInfo.set(iogrInfo.in());
                }
            }
            catch (const Cdmw::FT::NotFoundException &)
            {
                // to do
            }
            
            
            if (m_is_activated)
            {
                set_primary();
                //commit multiple transaction
                if (allow_commit_rollback)
                    m_FTManagerDataStoreObj->get_manager().commit();
                
                // register the fault notifier into the replication manager
                m_replication_manager->register_fault_notifier( ::FT::FaultNotifier::_narrow(get_fault_notifier()));
                
                // register the fault detector into the replication manager
                m_replication_manager->register_fault_detector( m_fault_detector.in());
                
                //             CosNotifyComm::StructuredPushConsumer_var rm_consumer
                //             = CosNotifyComm::StructuredPushConsumer::_narrow(m_replication_manager.in());
                //             m_fault_notifier_impl->update_consumer_id(rm_consumer.in(), m_consumerID);
            } else {
                //commit multiple transaction
                if (allow_commit_rollback)
                    m_FTManagerDataStoreObj->get_manager().commit();
            }   
        }
        catch(...)
        {
            //rollback multiple transaction
            if (allow_commit_rollback)
                m_FTManagerDataStoreObj->get_manager().rollback();
            throw;
        }
    }
    


    //
    // call by the mutual monitoring observer when a host deasappeared
    //
    void FTManager_impl::remove_location(const char * the_location)
        throw(::FT::MemberNotFound,
              CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
        CDMW_MUTEX_GUARD(m_mutex);
        // TODO**********
        // Check that location is already in DS and then proceed with a TX
        bool allow_commit_rollback = false;
        try
        {


            try
            {
                // remove the entry for the location into the ftManagerStorageHome
                m_ftManagerStorageHome->remove(the_location);

                //begin multiple transaction
                if (!m_FTManagerDataStoreObj->get_manager().in_transaction())
                {
                    allow_commit_rollback = true;
                    m_FTManagerDataStoreObj->get_manager().begin();
                } else {
                    allow_commit_rollback = false;
                }


                CDMW_DEBUG(FTLogger::GetLogger(),"Update data store");
                //create members infos
                int membersSize = m_ftManagerStorageHome->size();
                ::CdmwFT::MemberInfos replication_manager_members;
                replication_manager_members.length(membersSize);
        
                ::CdmwFT::MemberInfos fault_notifier_members;
                fault_notifier_members.length(membersSize);
        
                ::CdmwFT::MemberInfos fault_detector_members;
                fault_detector_members.length(membersSize);
        
                m_ftManagerStorageHome->for_each(BuildMembersInfos(replication_manager_members,
                                                                   fault_notifier_members,
                                                                   fault_detector_members));


                // creer la sequence des members infos
        
                IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
                // get the data
                ::CdmwFT::IOGRInfo_var iogrInfo = objIOGRInfo.get();
                ::FT::ObjectGroupRefVersion iorgs_version = iogrInfo->version;
        
        
                ::FT::ObjectGroup_var rmog = create_object_group(replication_manager_members,
                                                                 ID_REPL_MNGT,
                                                                 REP_ID_REPL_MNGT,
                                                                 iorgs_version);
		

                ::FT::ObjectGroup_var fnog = create_object_group(fault_notifier_members,
                                                                 ID_FAULT_NOTIFIER,
                                                                 REP_ID_FAULT_NOTIFIER,
                                                                 iorgs_version);

                ::FT::ObjectGroup_var fdog = create_object_group(fault_detector_members,
                                                                 ID_FAULT_DETECTOR,
                                                                 REP_ID_FAULT_DETECTOR,
                                                                 iorgs_version);

                iogrInfo->replication_manager = ::FT::ObjectGroup::_duplicate(rmog.in());
                iogrInfo->fault_notifier = ::FT::ObjectGroup::_duplicate(fnog.in());
                iogrInfo->fault_detector = ::FT::ObjectGroup::_duplicate(fdog.in());
                iogrInfo->version = iorgs_version++;

                objIOGRInfo.set(iogrInfo.in());
	
                if (m_is_activated)
                {
                    set_primary();

                    //commit multiple transaction
                    if (allow_commit_rollback)
                        m_FTManagerDataStoreObj->get_manager().commit();
        
                    // register the fault notifier into the replication manager
                    m_replication_manager->register_fault_notifier( ::FT::FaultNotifier::_narrow(get_fault_notifier()));
            
                    // register the fault detector into the replication manager
                    m_replication_manager->register_fault_detector( m_fault_detector.in());

                    //                 CosNotifyComm::StructuredPushConsumer_var rm_consumer
                    //                 = CosNotifyComm::StructuredPushConsumer::_narrow(m_replication_manager.in());
                    //                 m_fault_notifier_impl->update_consumer_id( rm_consumer.in(), m_consumerID);
                } else {
                    //commit multiple transaction
                    if (allow_commit_rollback)
                        m_FTManagerDataStoreObj->get_manager().commit();
                }
            
            }
            catch (const Cdmw::FT::NotFoundException &)
            {
                std::cout<<" [ FTManager - "<<__LINE__<<" ] throw"
                         <<"::FT::MemberNotFound()"<<std::endl;
                throw ::FT::MemberNotFound();    
            }
 
        }
        catch(...)
        {
            //rollback multiple transaction
            if (allow_commit_rollback)
                m_FTManagerDataStoreObj->get_manager().rollback();
            throw;
            CDMW_ERROR(FTLogger::GetLogger(),"Member '" << the_location << "' not found!");
            ::FT::MemberNotFound ex;   
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex; 
        }
    
    }

    CORBA::Object_ptr FTManager_impl::create_object_group(::CdmwFT::MemberInfos& members, 
                                                          ::FT::ObjectGroupId id,
                                                          const char* repid,
                                                          ::FT::ObjectGroupRefVersion object_group_ref_version,
                                                          bool with_primary_location)
        throw(::FT::ObjectGroupNotFound,
              CORBA::NO_MEMORY,
              CORBA::SystemException )
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<id<<",repid="<<repid
                          <<",object_group_ref_version="<<object_group_ref_version
                          <<",with_primary_location="<<with_primary_location);
        CORBA::Object_var object_group = CORBA::Object::_nil();
        try
        {
            try {
                // create a null fallback object sequence
                ::CdmwFT::Objects_var no_fallBack = new ::CdmwFT::Objects;     
                no_fallBack->length(0);
                ::FT::Location primary_location;
                if (!with_primary_location)
                {
                    primary_location.length(0 );
                } else {
                    primary_location.length(1);
                    primary_location[0].id=OsSupport::OS::get_hostname().c_str();
                }
            
            
                object_group = m_iogrFactory->build_iogr(members,
                                                         primary_location,
                                                         id,
                                                         repid,
                                                         m_ft_domain_id.in(),
                                                         object_group_ref_version,
                                                         no_fallBack.in());
            
            } catch (const ::FT::PrimaryNotSet & ) {
                CDMW_ERROR(FTLogger::GetLogger(),"Cannot set primary for IOGR ["
                           << "ogid="<<id<<",repid="<<repid
                           << ",object_group_ref_version="<<object_group_ref_version
                           << ",with_primary_location="<<with_primary_location << "]. "
                           << "Raising ::FT::ObjectNotAdded()");
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            } catch (const ::FT::ObjectNotCreated & ) {
                CDMW_ERROR(FTLogger::GetLogger(),"Cannot create IOGR ["
                           << "ogid="<<id<<",repid="<<repid
                           << ",object_group_ref_version="<<object_group_ref_version
                           << ",with_primary_location="<<with_primary_location << "]. "
                           << "Raising ::FT::ObjectNotAdded()");
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }

            return object_group._retn(); 
        }
        catch (const Cdmw::FT::NotFoundException &)
        {
            CDMW_ERROR(FTLogger::GetLogger(),"Cannot find group ["
                       << "ogid="<<id<<",repid="<<repid << "]. "
                       << "Raising ::FT::ObjectGroupNotFound()");
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
    
    void FTManager_impl::set_primary()
        throw(CORBA::SystemException )
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_DEBUG(FTLogger::GetLogger(),"Update data store");
        //create members infos
        int membersSize = m_ftManagerStorageHome->size();
        ::CdmwFT::MemberInfos replication_manager_members;
        replication_manager_members.length(membersSize);

        ::CdmwFT::MemberInfos fault_notifier_members;
        fault_notifier_members.length(membersSize);

        ::CdmwFT::MemberInfos fault_detector_members;
        fault_detector_members.length(membersSize);
    
        m_ftManagerStorageHome->for_each(BuildMembersInfos(replication_manager_members,
                                                           fault_notifier_members,
                                                           fault_detector_members));
 
        ::FT::ObjectGroupRefVersion iorgs_version = 0;
        ::CdmwFT::IOGRInfo_var iogrInfo;
        try
        {
            IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
            // get the data
            iogrInfo = objIOGRInfo.get();
            iorgs_version = iogrInfo->version;
        }
        catch (const Cdmw::FT::NotFoundException &)
        {
            CDMW_WARN(FTLogger::GetLogger(),"Could not find IOGR info in datastore!");
        }

        ::FT::ObjectGroup_var rmog;
        ::FT::ObjectGroup_var fnog;
        ::FT::ObjectGroup_var fdog;
        
        rmog = create_object_group(replication_manager_members,
                                   ID_REPL_MNGT,
                                   REP_ID_REPL_MNGT,
                                   iorgs_version, true);
    
    
        fnog = create_object_group(fault_notifier_members,
                                   ID_FAULT_NOTIFIER,
                                   REP_ID_FAULT_NOTIFIER,
                                   iorgs_version, true);
    
        fdog = create_object_group(fault_detector_members,
                                   ID_FAULT_DETECTOR,
                                   REP_ID_FAULT_DETECTOR,
                                   iorgs_version, true);
        ++iorgs_version;
    
    
        iogrInfo->replication_manager = ::FT::ObjectGroup::_duplicate(rmog.in());
        iogrInfo->fault_notifier = ::FT::ObjectGroup::_duplicate(fnog.in());
        iogrInfo->fault_detector = ::FT::ObjectGroup::_duplicate(fdog.in());
        iogrInfo->version = iorgs_version;

        ::FT::Location primary_location;
        primary_location.length(1);
        primary_location[0].id=OsSupport::OS::get_hostname().c_str();
        iogrInfo->the_location = primary_location;
 
        IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);            
        objIOGRInfo.set(iogrInfo.in());
    
    }

    void FTManager_impl::register_mutual_monitoring(CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoring_ptr mutual_monitoring)
        throw(CORBA::SystemException )
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_mutual_monitoring = CdmwFT::MonitoringLibrary::PrimaryBackup::MutualMonitoring::_duplicate(mutual_monitoring);    
    }


    void FTManager_impl::update_mutual_monitoring_locations()
        throw(CORBA::SystemException )
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_mutual_monitoring->reset_locations();
        m_ftManagerStorageHome->for_each(StartedLocations(m_mutual_monitoring.in()));
    }    


    CORBA::Boolean FTManager_impl::is_activated()
        throw(CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    
        CDMW_LOG_FUNCTION_RETURN(m_is_activated);
        return m_is_activated;
    }

    
    CORBA::Object_ptr FTManager_impl::get_object_group(::FT::ObjectGroupId id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<id);
        try
        {
            IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
            // get the data
            ::CdmwFT::IOGRInfo_var iogrInfo = objIOGRInfo.get();
            ::FT::ObjectGroup_var obj;
            switch (id){
                case ID_REPL_MNGT:
                    {
                        obj = CORBA::Object::_duplicate(iogrInfo->replication_manager.in());
                        break;
                    }
                case ID_FAULT_NOTIFIER :
                    {
                        obj = CORBA::Object::_duplicate(iogrInfo->fault_notifier.in());
                        break;
                    }
                case ID_FAULT_DETECTOR :
                    {
                        obj = CORBA::Object::_duplicate(iogrInfo->fault_detector.in());
                        break;
                    }
                default:
                    throw Cdmw::FT::NotFoundException();
            }
            return obj._retn();
        }
        catch (const Cdmw::FT::NotFoundException &)
        {
            CDMW_ERROR(FTLogger::GetLogger(),"FTManager - Object group '" << id << "' not found!");
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;   
        }   
    }

    
    ::FT::ObjectGroupRefVersion FTManager_impl::get_object_group_version(::FT::ObjectGroupId id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<id);
        try
        {
            IOGRInfoStorageObject objIOGRInfo = m_iogrInfoStorageHome->find_by_oid(1);
            // get the data
            ::CdmwFT::IOGRInfo_var iogrInfo = objIOGRInfo.get();
            CDMW_LOG_FUNCTION_RETURN(iogrInfo->version);
            return iogrInfo->version;
        }
        catch (const Cdmw::FT::NotFoundException &)
        {
            CDMW_ERROR(FTLogger::GetLogger(),"Object group '" << id << "' not found!");
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;   
        }     
    }

    /**
     * dump data store for the specific object
     */
    void FTManager_impl::dump_datastore( ::CdmwFT::FaultManagerObject obj )
        throw(CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        switch (obj)
        {
            case ( ::CdmwFT::FAULT_DETECTOR ):
                m_fault_detector_impl->dumpHostDataStore();
                break;
            
            case ( ::CdmwFT::FAULT_NOTIFIER ):
                m_fault_notifier_impl->dumpConsumersDataStore();
                break;
            
            case ( ::CdmwFT::REPLICATION_MANAGER ):

                ::CdmwFT::ReplicationManagerPushConsumer_var consumer_dumper = 
                ::CdmwFT::ReplicationManagerPushConsumer::_narrow(m_replication_manager.in());

                consumer_dumper->dump_ogd_datastore();
                consumer_dumper->dump_location_datastore();
                break;
        }
    }
    void FTManager_impl::create_group(std::string xml_conf_file)
        throw(CORBA::SystemException )
    {
        CDMW_INFO(Cdmw::FT::FTLogger::GetLogger(),"Reading default Group configuration..." );
        // The default Group configuration
        CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"create GroupConfigurationMngr " );
        Cdmw::FT::GroupConfigurationMngr group_conf(m_replication_manager.in());
        CDMW_DEBUG(Cdmw::FT::FTLogger::GetLogger(),"GroupConfigurationMngr parse"<< xml_conf_file );
        group_conf.parse(xml_conf_file, false);
    }
    
    
    
}  // End namespace FT
}  // End namespace Cdmw

