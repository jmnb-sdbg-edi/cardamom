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

#ifndef CARDAMOM_CLOCK_CLOCK_STATE_DATA_STORE_HPP
#define CARDAMOM_CLOCK_CLOCK_STATE_DATA_STORE_HPP

/**
 * @author Angelo Corsaro <acorsaro@selex-si.com>
 */

#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>
#include <Time/clockservice/CdmwClockState.stub.hpp>

DEFINE_VARIABLE_LENGTH_DATA_TRAITS( ::Cdmw::clock::TimerInfo );
DEFINE_VARIABLE_LENGTH_DATA_TRAITS( ::Cdmw::clock::ControllerData );

namespace Cdmw { namespace clock {
    
/**
 * @var typedef Cdmw::CommonSvcs::DataStore::StorageHome<Cdmw::clock::Ident,Cdmw::clock::ControllerData> ControllerDataStorageHome 
 * Definition of Storage objects for DataPart1 type 
 */
typedef Cdmw::CommonSvcs::DataStore::StorageHome<Cdmw::clock::Ident,
                              Cdmw::clock::ControllerData>
ControllerDataStorageHome;

/**
 * @var typedef Cdmw::CommonSvcs::DataStore::StorageObject<Cdmw::clock::Ident,Cdmw::clock::ControllerData> ControllerDataStorageObject 
 * Definition of Storage objects.
 */
typedef Cdmw::CommonSvcs::DataStore::StorageObject<Cdmw::clock::Ident,
                                Cdmw::clock::ControllerData>
ControllerDataStorageObject;

/**
 * @var typedef Cdmw::FT::DataStore<Cdmw::clock::Ident,
 *  Cdmw::clock::ControllerData> ControllerDataStore
 *  Definition of FT::Datastore.
 */
typedef Cdmw::FT::DataStore<Cdmw::clock::Ident,
                            Cdmw::clock::ControllerData>
ControllerDataStore;

} /* Cdmw */ } /* clock */

#endif /* CARDAMOM_CLOCK_CLOCK_STATE_DATA_STORE_HPP */
