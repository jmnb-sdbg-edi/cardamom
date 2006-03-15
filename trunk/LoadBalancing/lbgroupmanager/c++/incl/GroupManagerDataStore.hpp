/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

#ifndef CARDAMOM_GROUP_MANAGER_DATA_STORE_HPP
#define CARDAMOM_GROUP_MANAGER_DATA_STORE_HPP

/**
 * @file
 *
 * @author Raffaella Panella <rpanella@amsjv.it>
 */

//#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>
#include <LoadBalancing/idllib/CdmwLBCommon.stub.hpp>

DEFINE_VARIABLE_LENGTH_DATA_TRAITS( ::CdmwLB::ObjectGroupData );

namespace Cdmw { 
namespace LB {

    
/**
 * \var typedef Cdmw::CommonSvcs::DataStore::StorageHome<PortableGroup::ObjectGroupId,CdmwLB::ObjectGroupData> ObjectGroupDataStorageHome 
 * Definition of Storage objects for DataPart1 type 
 */
typedef Cdmw::CommonSvcs::DataStore::StorageHome<PortableGroup::ObjectGroupId,
                              CdmwLB::ObjectGroupData>
ObjectGroupDataStorageHome;

/**
 * \var typedef Cdmw::CommonSvcs::DataStore::StorageObject<PortableGroup::ObjectGroupId,CdmwLB::ObjectGroupData> ObjectGroupDataStorageObject 
 * Definition
 */
  typedef Cdmw::CommonSvcs::DataStore::StorageObject<PortableGroup::ObjectGroupId,
                              CdmwLB::ObjectGroupData>
ObjectGroupDataStorageObject;
  
/**
 * \var typedef Cdmw::FT::DataStore<PortableGroup::ObjectGroupId,
 *  CdmwLB::ObjectGroupData> ObjectGroupDataStore
 *
 */
/*typedef Cdmw::FT::DataStore<PortableGroup::ObjectGroupId,
                              CdmwLB::ObjectGroupData>
			      ObjectGroupDataStore;*/

typedef Cdmw::CommonSvcs::DataStore::DataStoreInterface<PortableGroup::ObjectGroupId,
					     CdmwLB::ObjectGroupData>
ObjectGroupDataStore;

} /* Cdmw */ } /* LB */

#endif /* CARDAMOM_GROUP_MANAGER_DATA_STORE_HPP */
