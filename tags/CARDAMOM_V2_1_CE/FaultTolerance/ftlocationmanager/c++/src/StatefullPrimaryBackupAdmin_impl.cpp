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


#include <iostream>
#include <algorithm>

// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftlocationmanager/StatefullPrimaryBackupAdmin_impl.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

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

        //
        // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin:1.0
        //
        StatefullPrimaryBackupAdmin_impl::StatefullPrimaryBackupAdmin_impl
        (CORBA::ORB_ptr orb,
         PortableServer::POA_ptr poa,
         const ::FT::Location& the_location,
         PrimaryBackupGroupRepository_impl*  group_repository,
         bool cold_restart,
         bool is_ftmanager)
            : PrimaryBackupAdmin_impl(orb, poa, the_location, group_repository, is_ftmanager),
              m_poa(PortableServer::POA::_duplicate(poa)),
              m_primary_backup_group_repository(group_repository),
              m_is_ftmanager(is_ftmanager),
              m_the_location(the_location)
        {
            CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"cold_restart="<<cold_restart<<",is_ftmanager="<<is_ftmanager);
            m_init_type = cold_restart;
        }

        StatefullPrimaryBackupAdmin_impl::~StatefullPrimaryBackupAdmin_impl()
        {
            CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        }

    
        //
        // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/date_statefull_group_view:1.0
        //
        void
        StatefullPrimaryBackupAdmin_impl::update_statefull_group_view
        (const CdmwFT::Location::StatefullGroupInfos& statefull_group_infos)
            throw(::FT::ObjectGroupNotFound, 
                  CdmwFT::Location::UnknownDataStoreID,
                  CdmwFT::StateTransfer::InvalidDataStoreState,
                  CORBA::SystemException)
        {
            CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
            try
            {
                update_from_group_info(statefull_group_infos);

                // for each statefullGroupInfos
                CORBA::ULong statefull_group_infos_length = statefull_group_infos.length();
                for (CORBA::ULong i = 0; i<statefull_group_infos_length; i++)
                {
                    // call update_group_view on the PrimaryBackupAdmin
                    update_group_view(statefull_group_infos[i].group_info);
                }
            }
            catch( const CORBA::Exception & ex )
            {
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            }
        }
        
        
        //
        // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/update_statefull_primary:1.0
        //
        void
        StatefullPrimaryBackupAdmin_impl::update_statefull_primary
        (const CdmwFT::Location::StatefullGroupInfos&  statefull_group_infos,
         const ::FT::Location& primary_location)
            throw(::FT::ObjectGroupNotFound,
                  CdmwFT::Location::UnknownDataStoreID,
                  CdmwFT::StateTransfer::InvalidDataStoreState,
                  CORBA::SystemException)
        {
            CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
            try
            {
                
                // check if the datastore must be activate or deactivate
                std::string new_primary = Cdmw::CommonSvcs::Naming::NamingInterface::to_string(primary_location);

                for (LocationMapForDSID::iterator iter = m_nominal_location.begin();
                     iter != m_nominal_location.end();
                     iter ++)
                {
                    CDMW_DEBUG(FTLogger::GetLogger(),
                               "Set coordinator location "<<(*iter).first);
                    // get the reference of the datastore for the dsid
                    Cdmw::FT::DataStoreBase* dataStore = Cdmw::FT::DataStoreBase::Get_dataStore((*iter).first);
                    CDMW_ASSERT(dataStore != 0);

                    dataStore->get_context()
                        .set_coordinator_location(new_primary);
                }

                if ((strcmp(new_primary.c_str(),m_location.c_str()) != 0) && 
                    (strcmp(m_primary_location.c_str(),m_location.c_str()) == 0))
                {
                    // realise the deactivate
                    for (LocationMapForDSID::iterator iter = m_nominal_location.begin();
                         iter != m_nominal_location.end();
                         iter ++)
                    {
                        CDMW_DEBUG(FTLogger::GetLogger(),
                                   "Deactivate datastore "<<(*iter).first);
                        // get the reference of the datastore for the dsid
                        Cdmw::FT::DataStoreBase* dataStore = Cdmw::FT::DataStoreBase::Get_dataStore((*iter).first);
                        CDMW_ASSERT(dataStore != 0);
                        dataStore->deactivate();
                    }
                }
                else
                {
                    if ((strcmp(m_primary_location.c_str(),m_location.c_str()) != 0) && 
                        (strcmp(new_primary.c_str(),m_location.c_str()) == 0))
                    {
                        // realise the activate
                        for (LocationMapForDSID::iterator iter = m_nominal_location.begin();
                             iter != m_nominal_location.end();
                             iter ++)
                        {
                            CDMW_DEBUG(FTLogger::GetLogger(),
                                       "Activate datastore "<<(*iter).first);
                            // get the reference of the datastore for the dsid
                            Cdmw::FT::DataStoreBase* dataStore = Cdmw::FT::DataStoreBase::Get_dataStore((*iter).first);
                            CDMW_ASSERT(dataStore != 0);
                            dataStore->activate();
                        }
                    }
                    else
                    {
                        // do nothing : the new primary is not for this location
                    }
                
                }   

                update_from_group_info(statefull_group_infos, true);
                
                // for each statefullGroupInfos
                CORBA::ULong statefull_group_infos_length = statefull_group_infos.length();
                for (CORBA::ULong i = 0; i<statefull_group_infos_length; i++)
                {
                    // call update_primary on the PrimaryBackupAdmin
                    update_primary(statefull_group_infos[i].group_info, primary_location);
                }
            }
            catch(const CORBA::Exception& ex)
            {
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            }
        }
        
            
        //
        // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/init_backup_data_stores:1.0
        //
        void
        StatefullPrimaryBackupAdmin_impl::init_backup_data_stores
        (const CdmwFT::StateTransfer::DataStoreInfo&
         primary_data_store_infos)
            throw(CdmwFT::Location::InitFailed,
                  CdmwFT::Location::AlreadyDone,
                  CORBA::SystemException)
        {
            CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
            try
            {
                CdmwFT::StateTransfer::LocalDataStoreInfos 
                   primary_local_data_store_infos(primary_data_store_infos.local_data_store_infos);
                
                // For each datastore
                CORBA::ULong primary_local_data_store_infos_length =
                    primary_local_data_store_infos.length();
                CORBA::ULong total_datastores = 0;
                CORBA::ULong index = 0;

                for (CORBA::ULong i = 0; i<primary_local_data_store_infos_length; i++) 
                {
                    total_datastores += primary_local_data_store_infos[i].dsids.length();

                    for (CORBA::ULong j = 0;
                         j < primary_local_data_store_infos[i].dsids.length();
                         j++)
                    {
                        // check if all datastore are not initialised
                        DataStoreIDSet::iterator id_iter =
                            std::find(m_init_backup_datastore_id.begin(),
                                      m_init_backup_datastore_id.end(),
                                      primary_local_data_store_infos[i].dsids[j]); 

                        // if the datastore is not initialise
                        if (id_iter == m_init_backup_datastore_id.end())
                        {
                            // increase the index
                            index++;
                        }
                        // get the reference of the datastore for the dsid
                        Cdmw::FT::DataStoreBase* dataStore =
                            Cdmw::FT::DataStoreBase::Get_dataStore
                                (primary_local_data_store_infos[i].dsids[j]);
                        if (dataStore == NULL)
                        {
                            std::ostringstream stream;
                            stream << MembersAdmin_impl::get_the_location()
                                   <<" datastore reference for dsid "
                                   << primary_local_data_store_infos[i].dsids[j]
                                   <<" is NULL";
                            CDMW_ERROR(FTLogger::GetLogger(),
                                       "Throw CdmwFT::Location::InitFailed : "
                                       << stream.str().c_str() );
                            CdmwFT::Location::InitFailed ex(stream.str().c_str());
                            CDMW_LOG_FUNCTION_EXCEPTION(ex);
                            throw ex;
                        }
                    }
                }

                //if all datastore must be initialise
                if (index == total_datastores)
                {
                    // For each datastore, 
                    for (CORBA::ULong i = 0;
                         i < primary_local_data_store_infos_length;
                         i++) 
                    {   
                        // get the reference of the datastore for the dsid
                        Cdmw::FT::DataStoreBase* dataStore =
                            Cdmw::FT::DataStoreBase::Get_dataStore
                            (primary_local_data_store_infos[i].dsids[0]);

                        CDMW_ASSERT(dataStore != 0);

                        // call the method on the data store to update
                        // the state with the primary
                        dataStore->get_context().update_with_coordinator
                            (primary_local_data_store_infos[i].local_data_store.in(),
                             MembersAdmin_impl::get_the_location(),
                             primary_local_data_store_infos[i].dsids);

                        const std::string primary_location = 
                           Cdmw::CommonSvcs::Naming::NamingInterface::to_string(
                                 primary_data_store_infos.the_location);

                        dataStore->get_context().set_coordinator_location(primary_location);

                        for (CORBA::ULong j = 0;
                             j < primary_local_data_store_infos[i].dsids.length();
                             j++)
                        {
                            CDMW_DEBUG(FTLogger::GetLogger(),
                                       " intialised the datastore :"
                                       << primary_local_data_store_infos[i].dsids[j]);

                            // save the id of the datastore into the set
                            // of datastore initialised
                            m_init_backup_datastore_id.insert
                                (m_init_backup_datastore_id.end(),
                                 primary_local_data_store_infos[i].dsids[j]);

                            LocationSet location_set;
                            location_set.insert
                                (MembersAdmin_impl::get_the_location());
                            m_nominal_location.insert
                                (make_pair
                                 (primary_local_data_store_infos[i].dsids[j],
                                  location_set));
                        }
                    }           
                } 
                else
                {
                    if (index == 0)
                    {
                        // all the datastore are already initialised
                        CDMW_ERROR(FTLogger::GetLogger(), "throw CdmwFT::Location::AlreadyDone()");
                        CdmwFT::Location::AlreadyDone ex;
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    } 
                    else
                    {    
                        // some datastore are initialised and some are not => insubtantial state
                        CDMW_ERROR(FTLogger::GetLogger(), "throw CdmwFT::Location::InitFailed : some datastore are initialised and some are not => insubstantial state");
                        CdmwFT::Location::InitFailed ex("some datastore are initialised and some are not => insubstantial state");
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    }
                }
            }            
            catch(const CdmwFT::Location::InitFailed& ex)
            {
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;                
            }
            catch(const Cdmw::InitErrorException&)
            {
                CdmwFT::Location::InitFailed ex("InitErrorException");
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            catch(const CORBA::SystemException &)
            {
                CdmwFT::Location::InitFailed ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            

        }
        
          
        //
        // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/init_primary_data_stores:1.0
        //
        void 
        StatefullPrimaryBackupAdmin_impl::init_primary_data_stores
        (const CdmwFT::StateTransfer::LocalDataStoreInfos& local_datastore_infos)
            throw(CdmwFT::Location::InitFailed,
                  CdmwFT::Location::AlreadyDone,
                  CORBA::SystemException)
        {
            CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
            try
            {
                // For each datastore
                CORBA::ULong datastore_info_length = local_datastore_infos.length();
                CORBA::ULong total_datastores = 0;
                CORBA::ULong index = 0;

                for (CORBA::ULong i = 0; i < datastore_info_length; i++) 
                {
                    total_datastores += local_datastore_infos[i].dsids.length();

                    for (CORBA::ULong j = 0;
                         j < local_datastore_infos[i].dsids.length();
                         j++)
                    {
                        // check if all datastore are not initialised
                        DataStoreIDSet::iterator id_iter =
                            std::find(m_init_primary_datastore_id.begin(),
                                      m_init_primary_datastore_id.end(),
                                      local_datastore_infos[i].dsids[j]); 

                        // if the datastore is not initialise
                        if (id_iter == m_init_primary_datastore_id.end())
                        {
                            // increase the index
                            index++;
                        }
                        // get the reference of the datastore for the dsid
                        Cdmw::FT::DataStoreBase* dataStore =
                            Cdmw::FT::DataStoreBase::Get_dataStore
                                (local_datastore_infos[i].dsids[j]);

                        if (dataStore == NULL)
                        {
                            std::ostringstream stream;
                            stream << MembersAdmin_impl::get_the_location()
                                   <<" datastore reference for dsid "
                                   << local_datastore_infos[i].dsids[j]
                                   <<" is NULL";
                            CDMW_ERROR(FTLogger::GetLogger(),
                                       "Throw CdmwFT::Location::InitFailed : "
                                       << stream.str().c_str());
                            CdmwFT::Location::InitFailed ex(stream.str().c_str());
                            CDMW_LOG_FUNCTION_EXCEPTION(ex);
                            throw ex;
                        }
                    }
                }

                if (index == total_datastores)
                {
                    // For each datastore, 
                    for (CORBA::ULong i = 0; i<datastore_info_length; i++) 
                    {   
                        for (CORBA::ULong j = 0;
                             j < local_datastore_infos[i].dsids.length();
                             j++)
                        {
                            CDMW_DEBUG(FTLogger::GetLogger(),
                                       "intialise the datastore :"
                                       << local_datastore_infos[i].dsids[j]);

                            // get the reference of the datastore for the dsid
                            Cdmw::FT::DataStoreBase* dataStore =
                                Cdmw::FT::DataStoreBase::Get_dataStore
                                (local_datastore_infos[i].dsids[j]);

                            CDMW_ASSERT(dataStore != 0);

                            // call init on the datastore
                            dataStore->init(m_init_type);

                            // call activate on the datastore
                            dataStore->activate();
                        
                            // save the id of the datastore into the
                            // set of datastore initialised
                            m_init_primary_datastore_id.insert
                                (m_init_primary_datastore_id.end(),
                                 local_datastore_infos[i].dsids[j]);;

                            LocationSet location_set;
                            location_set.insert
                                (MembersAdmin_impl::get_the_location());
                            m_nominal_location.insert
                                (make_pair
                                 (local_datastore_infos[i].dsids[j],
                                  location_set));
                            m_primary_location =
                                MembersAdmin_impl::get_the_location();
                        }
                    }
                } 
                else
                {
                    if (index == 0)
                    {
                        // all the datastore are already initialised
                        CDMW_ERROR(FTLogger::GetLogger(), "throw CdmwFT::Location::AlreadyDone()");
                        CdmwFT::Location::AlreadyDone ex;
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    } 
                    else
                    {    
                        // some datastore are initialised and some are not => insubtantial state
                        CDMW_ERROR(FTLogger::GetLogger(), "throw CdmwFT::Location::InitFailed : some datastore are initialised and some are not => insubtantial state");
                        CdmwFT::Location::InitFailed ex("some datastore are initialised and some are not => insubtantial state");
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    }
                }
            }            
            catch(const CdmwFT::Location::InitFailed& ex)
            {
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            }
            catch(const CdmwFT::Location::AlreadyDone& ex)
            {
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;                
            }
            catch(const Cdmw::InitErrorException&)
            {
                CdmwFT::Location::InitFailed ex("InitErrorException");
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            catch(const CORBA::SystemException &)
            {
                CdmwFT::Location::InitFailed ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
        }
        
         
        //
        // IDL:thalesgroup.com/CdmwFT/Location/StatefullPrimaryBackupAdmin/handle_backup_insertion_failure:1.0
        //
        void StatefullPrimaryBackupAdmin_impl::handle_backup_insertion_failure
        (const ::FT::Location& failed_location)
            throw(CORBA::SystemException)
        {
            CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
            Cdmw::FT::DataStoreBase::On_Failure(Cdmw::CommonSvcs::Naming::NamingInterface::to_string(failed_location));
        }
        

        void StatefullPrimaryBackupAdmin_impl::update_from_group_info(const CdmwFT::Location::StatefullGroupInfos&  statefull_group_infos,
                                                                      bool is_update_primary)
            throw(::FT::ObjectGroupNotFound,
                  CdmwFT::Location::UnknownDataStoreID,
                  CdmwFT::StateTransfer::InvalidDataStoreState,
                  CORBA::SystemException)
        {       
            CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"is_update_primary="<<is_update_primary);
            // clear all the map used to realise the on failure and the activate on the new data store or on the remove data store
            m_add_location.clear();
            m_remove_location.clear();
            m_infoMap.clear();
            m_temp_location.clear();

            // for each statefullGroupInfos
            CORBA::ULong statefull_group_infos_length = statefull_group_infos.length();
            for (CORBA::ULong i = 0; i<statefull_group_infos_length; i++)
            {
                // for each statefull_group_infos[i].data_store_infos
                CORBA::ULong data_store_infos_length = statefull_group_infos[i].data_store_infos.length();
                for (CORBA::ULong j = 0; j<data_store_infos_length; j++)
                {
                    //for each statefull_group_infos[i].data_store_infos[j].local_data_store_infos
                    CORBA::ULong local_data_store_infos_length = statefull_group_infos[i].data_store_infos[j].local_data_store_infos.length();
                    for (CORBA::ULong k = 0; k<local_data_store_infos_length; k++)
                    {
                        for (CORBA::ULong m = 0; m < statefull_group_infos[i].data_store_infos[j].local_data_store_infos[k].dsids.length(); m++)
                        {
                            CDMW_DEBUG(FTLogger::GetLogger(),
                                       "get_datastore reference for the dsid :"
                                       <<statefull_group_infos[i].data_store_infos[j].local_data_store_infos[k].dsids[m]);
                            // check if the data store is known
                            Cdmw::FT::DataStoreBase* dataStore 
                                = Cdmw::FT::DataStoreBase::Get_dataStore(statefull_group_infos[i].data_store_infos[j].local_data_store_infos[k].dsids[m]);
                            if (dataStore == NULL)
                            {
                                CDMW_ERROR(FTLogger::GetLogger(),"throw CdmwFT::Location::UnknownDataStoreID");
                                CdmwFT::Location::UnknownDataStoreID ex;
                                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                                throw ex;
                            }
                        
                            // check into the m_nominal_location if the dsid is known
                            // if the dsid is known, the data store was initialised else the data store is not initialise and the state 
                            // is in insubtantial state
                        
                            LocationMapForDSID::iterator locationMap_iter = 
                                m_nominal_location.find(statefull_group_infos[i].data_store_infos[j].local_data_store_infos[k].dsids[m]);
                            if (locationMap_iter == m_nominal_location.end())
                            {
                                CDMW_ERROR(FTLogger::GetLogger(),
                                           " throw InvalidDataStoreState exception");
                                CdmwFT::StateTransfer::InvalidDataStoreState ex;
                                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                                throw ex;
                            } 
                            else 
                            {
                                std::string location = 
                                    Cdmw::CommonSvcs::Naming::NamingInterface::to_string(statefull_group_infos[i].data_store_infos[j].the_location);
                                CDMW_DEBUG(FTLogger::GetLogger(),
                                           " save the location "<<location<<" into a temporally map");

                                LocationMapForDSID::iterator temp_iter 
                                    = m_temp_location.find(statefull_group_infos[i].data_store_infos[j].local_data_store_infos[k].dsids[m]);
                            
                                if (temp_iter == m_temp_location.end())
                                {
                                    LocationSet location_set;
                                    location_set.insert(location);
                                    m_temp_location.insert(make_pair(statefull_group_infos[i].data_store_infos[j].local_data_store_infos[k].dsids[m], location_set));
                                }
                                else
                                {
                                    (*temp_iter).second.insert(location);
                                }

                                IDLocation idlocation;
                                idlocation.id = statefull_group_infos[i].data_store_infos[j].local_data_store_infos[k].dsids[m];
                                idlocation.location = location;

                                m_infoMap[idlocation] = statefull_group_infos[i].data_store_infos[j].local_data_store_infos[k];
                            }
                        }
                    }
                }
            }
            
            // Build remove and add location map
            for ( LocationMapForDSID::iterator temp_iter = m_temp_location.begin();
                  temp_iter != m_temp_location.end();
                  temp_iter++)
            {
                // for a specific id create two map, one for the added member and one for the remove member
                ::CdmwFT::Transaction::DataStoreID id = (*temp_iter).first;
                LocationMapForDSID::iterator nom_locationMap_iter = m_nominal_location.find(id);
                LocationMapForDSID::iterator temp_locationMap_iter = m_temp_location.find(id);
                
                LocationSet addlocationset, removelocationset;
                CDMW_DEBUG(FTLogger::GetLogger(),"Build add and remove map for the dsid :"<<id);
                
                // create two insert_iterator
                std::insert_iterator<LocationSet > add_ins(addlocationset, addlocationset.begin());
                std::insert_iterator<LocationSet > remove_ins(removelocationset, removelocationset.begin());
                
                // to realise the set of the removed member, take the set of the nominal member and realise the
                // difference with the set of the updated member
                
                set_difference((*nom_locationMap_iter).second.begin(), (*nom_locationMap_iter).second.end(),
                               (*temp_locationMap_iter).second.begin(), (*temp_locationMap_iter).second.end(),
                               remove_ins);

                // to realise the set of the added member, take the set of the upated member and realise the
                // difference with the set of the nominal member
                
                set_difference((*temp_locationMap_iter).second.begin(), (*temp_locationMap_iter).second.end(),
                               (*nom_locationMap_iter).second.begin(), (*nom_locationMap_iter).second.end(),
                               add_ins);

                
                m_add_location.insert(make_pair(id,addlocationset));
                for (LocationSet::iterator iter = removelocationset.begin();
                     iter != removelocationset.end();
                     iter++)
                {
                    m_remove_location.insert(*iter);
                }
                

                // realise the add_location on all the data store
                // For the new backup, realise add_location for all the member, except himself
                // For the old backup, realise an add_location with the new backup location
                try 
                {
                    for (LocationSet::iterator set_iter = addlocationset.begin();
                         set_iter != addlocationset.end();
                         set_iter++)
                    {
                        // realise the add_location 
                        Cdmw::FT::DataStoreBase* dataStore = Cdmw::FT::DataStoreBase::Get_dataStore(id);
                        if (dataStore == NULL)
                        {
                            CDMW_ERROR(FTLogger::GetLogger(),
                                       " throw CdmwFT::Location::UnknownDataStoreID");
                            CdmwFT::Location::UnknownDataStoreID ex;
                            CDMW_LOG_FUNCTION_EXCEPTION(ex);
                            throw ex;
                        }
                        else
                            // if the location to add is the primary location or the local location, doesn't add it
                            if ((strcmp(m_location.c_str(),m_primary_location.c_str()) != 0) && 
                                (strcmp((*set_iter).c_str(),m_location.c_str()) != 0))
                            {
                                IDLocation idlocation;
                                idlocation.id = id;
                                idlocation.location = (*set_iter);
                            
                                InfoMap::iterator info_iter = m_infoMap.find(idlocation);

                                if (info_iter == m_infoMap.end())
                                {
                                    CDMW_ERROR(FTLogger::GetLogger(),
                                               "throw CdmwFT::Location::UnknownDataStoreID");
                                    CdmwFT::Location::UnknownDataStoreID ex;
                                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                                    throw ex;
                                }
                                else
                                {
                                    CDMW_DEBUG(FTLogger::GetLogger(),
                                               " add location "<<(*set_iter)<<" for the datastore "<<id);
                                    dataStore->get_context().add_location(id, (*set_iter));

                                    if (!CORBA::is_nil((*info_iter).second.coordinator.in()))
                                    {
                                        CDMW_DEBUG(FTLogger::GetLogger(),
                                                   " add IIOP location "<<(*set_iter)<<" for the datastore "<<id);
                                        dataStore->get_context().add_ref_for_location
                                            ((*set_iter),
                                             CdmwFT::Transaction::TPCCoordinator::_duplicate((*info_iter).second.coordinator.in()),
                                             CdmwFT::Transaction::TPCCohort::_duplicate((*info_iter).second.cohort.in()));
                                    }
                                }
                            }
                        
                        // save the new location into the map
                        m_nominal_location[id].insert((*set_iter));
                    }
                    
                }
                catch(const Cdmw::FT::AlreadyExistException& e)
                {
                    CDMW_ERROR(FTLogger::GetLogger(),
                              " update_from_group_info raises exception AlreadyExistException "
                              << e <<" for the datastore "<<id);
                    CdmwFT::StateTransfer::InvalidDataStoreState ex;
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
                catch(const Cdmw::FT::NotFoundException& e)
                {
                    CDMW_ERROR(FTLogger::GetLogger(),
                              " update_from_group_info raises exception NotFoundException "
                              << e <<" for the datastore "<<id);

                    CdmwFT::StateTransfer::InvalidDataStoreState ex;
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
            }
            
            // realise a on failure for all the failed location on all the data store
            for (LocationSet::iterator iter = m_remove_location.begin();
                 iter != m_remove_location.end();
                 iter++)
            {
                CDMW_INFO(FTLogger::GetLogger(),
                          "Remove location => On_Failure ("<<(*iter)<<")");
                Cdmw::FT::DataStoreBase::On_Failure(*iter);
            }
        }
    } // End of namespace Location

} // End of namespace FT

} // End of namespace Cdmw

