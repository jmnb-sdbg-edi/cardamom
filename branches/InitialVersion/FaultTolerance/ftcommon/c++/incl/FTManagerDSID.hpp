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


#ifndef CDMW_FTMANAGER_DSID_H
#define CDMW_FTMANAGER_DSID_H


#include <iostream>
#include "ftreplicationmanager/CdmwFTLocationInfo.stub.hpp"
#include "ftreplicationmanager/CdmwFTObjectGroupData.stub.hpp"
#include <FaultTolerance/idllib/FT.stub.hpp>
#include "FaultTolerance/ftstatemanager/TXDataStore.hpp"
#include "Foundation/commonsvcs/datastore/StorageHome.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"
#include "FaultTolerance/idllib/CdmwFTManager.stub.hpp"



namespace {

    // replication manager
    const int PROPERTIES_DATA_STORE = 1;
    const int OGD_DATA_STORE = 2;
    const int LOCATIONS_DATA_STORE = 3;
    const int OGID_DATA_STORE = 12;

    // ftmanager
    const int IOGR_INFOS_DATA_STORE = 4;
    const int FT_MANAGER_DATA_STORE = 5;

    // simple monitoring observer
    const int HOST_DATA_STORE2 = 6;

    // fault detector
    const int MONITORING_ID_DATA_STORE = 7;
    const int HOST_DATA_STORE = 8;

    // fault notifier
    const int STRUCTURED_PUSH_CONSUMER_DATA_STORE = 9;
    const int SEQUENCE_PUSH_CONSUMER_DATA_STORE = 10;
    const int CONSUMER_ID_DATA_STORE = 11;


    typedef Cdmw::FT::TXDataStore< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::StructuredPushConsumer> StructuredPushConsumerDataStore;
    typedef Cdmw::FT::TXDataStore< ::FT::FaultNotifier::ConsumerId, CosNotifyComm::SequencePushConsumer> SequencePushConsumerDataStore;
    typedef Cdmw::FT::TXDataStore< std::string , ::FT::FaultNotifier::ConsumerId > ConsumerIdDataStore;
    typedef Cdmw::FT::TXDataStore< std::string, CORBA::Boolean > HostStatusDataStore;
    typedef Cdmw::FT::TXDataStore< CdmwFT::FaultManagement::MonitoringId, std::string  > HostDataStore;
    typedef Cdmw::FT::TXDataStore< std::string , CdmwFT::FaultManagement::MonitoringId > MonitoringIdDataStore;
    typedef Cdmw::FT::TXDataStore< std::string, CdmwFT::FTManagerObjects > FTManagerDataStore;


	typedef Cdmw::CommonSvcs::DataStore::StorageHome< int, ::CdmwFT::IOGRInfo > IOGRInfoStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< int, ::CdmwFT::IOGRInfo > IOGRInfoStorageObject;
    typedef Cdmw::FT::TXDataStore< int, ::CdmwFT::IOGRInfo > IOGRInfoDataStore;

	typedef Cdmw::CommonSvcs::DataStore::StorageHome< std::string, CdmwFT::FTManagerObjects > FTManagerStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< std::string, CdmwFT::FTManagerObjects > FTManagerStorageObject;
    typedef Cdmw::FT::TXDataStore< std::string, CdmwFT::FTManagerObjects > FTManagerDataStore;

    typedef Cdmw::CommonSvcs::DataStore::StorageHome  < std::string, ::FT::Properties > PropertiesStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< std::string, ::FT::Properties > PropertiesStorageObject;
    typedef Cdmw::FT::TXDataStore    < std::string, ::FT::Properties > PropertiesDataStore;
    
    typedef Cdmw::CommonSvcs::DataStore::StorageHome  < ::FT::ObjectGroupId, ::CdmwFT::ObjectGroupData > OGDStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< ::FT::ObjectGroupId, ::CdmwFT::ObjectGroupData > OGDStorageObject;
    typedef Cdmw::FT::TXDataStore    < ::FT::ObjectGroupId, ::CdmwFT::ObjectGroupData > OGDDataStore;
    
        
    typedef Cdmw::CommonSvcs::DataStore::StorageHome  < std::string, ::CdmwFT::LocationInfo > LocInfoStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< std::string, ::CdmwFT::LocationInfo > LocInfoStorageObject;
    typedef Cdmw::FT::TXDataStore    < std::string, ::CdmwFT::LocationInfo > LocInfoDataStore;
    
    typedef Cdmw::CommonSvcs::DataStore::StorageHome  < std::string, ::FT::ObjectGroupId > OGIDStorageHome;
    typedef Cdmw::CommonSvcs::DataStore::StorageObject< std::string, ::FT::ObjectGroupId > OGIDStorageObject;
    typedef Cdmw::FT::TXDataStore    < std::string, ::FT::ObjectGroupId > OGIDDataStore;
    

};


#endif // CDMW_FTMANAGER_DSID_H


