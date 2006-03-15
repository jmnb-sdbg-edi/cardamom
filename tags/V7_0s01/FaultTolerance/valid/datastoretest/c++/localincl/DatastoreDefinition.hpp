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


#ifndef INCL_DATABASE_DEFINITION_HPP
#define INCL_DATABASE_DEFINITION_HPP

#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <Foundation/commonsvcs/datastore/StorageHome.hpp>
#include <Foundation/commonsvcs/datastore/StorageObject.hpp>


// Test idl file
#include "datastoretest/Test.skel.hpp"  

DEFINE_VARIABLE_LENGTH_DATA_TRAITS(Test::DataPart1);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(Test::DataPart2);


namespace Cdmw 
{
namespace FT
{


// Definition of Storage objects for DataPart1 type 
typedef Cdmw::CommonSvcs::DataStore::StorageHome< Test::Ident, Test::DataPart1 > DataPart1StorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject< Test::Ident, Test::DataPart1 > DataPart1StorageObject;
typedef Cdmw::FT::DataStore< Test::Ident, Test::DataPart1 > DataPart1Store;

// Definition of Storage objects for DataPart2 type 
typedef Cdmw::CommonSvcs::DataStore::StorageHome< Test::Ident, Test::DataPart2 > DataPart2StorageHome;
typedef Cdmw::CommonSvcs::DataStore::StorageObject< Test::Ident, Test::DataPart2 > DataPart2StorageObject;
typedef Cdmw::FT::DataStore< Test::Ident, Test::DataPart2 > DataPart2Store;

/**
 * Identifier of the Data Store for DataPart1 type.
 */
static const int DATA_STORE_1_IDENTIFIER = 123;

/**
 * Identifier of the Data Store for DataPart2 type.
 */
static const int DATA_STORE_2_IDENTIFIER = 456;

/**
 * Maximum number of state transfer transactions in progress. 
 */
static const int MAX_TRANSACTION_IN_PROGRESS = 10;

/**
 * Maximum number of state transfer transactions done. 
 */
static const int MAX_TRANSACTION_DONE = 20;


/**
 * Cohort timeout (timeout between a prepare_ operation and the commit)
 */
static const int COHORT_TIMEOUT = 8;

/**
 * Coordinator timeout (timeout between a prepare_ operation 
 * and a yes/no, and a commit/roolback operation and a end)
 */
static const int COORDINATOR_TIMEOUT = 6;


/**
 * Multicast IP address for MIOP State Transfer
 */
static const std::string MULTICAST_IP = "224.2.4.6";

/**
 * Multicast IP port for MIOP State Transfer
 */
static const std::string MULTICAST_PORT = "3366";

/**
 * Multicast domain for MIOP State Transfer
 */
static const std::string MULTICAST_DOMAIN = "TestDomain";

// Test specific definition to modify number of oids in the test
static const int SIZESCALE = 200;

}; // End namespace FT

}; // End namespace Cdmw 


#endif //INCL_DATABASE_DEFINITION_HPP
