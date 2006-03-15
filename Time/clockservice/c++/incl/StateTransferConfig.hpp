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

#ifndef CARDAMOM_STATE_TRANSFER_DATA_HPP
#define CARDAMOM_STATE_TRANSFER_DATA_HPP


#include <string>
#include <FaultTolerance/idllib/FT.stub.hpp>


/**
 * Name of the executor object group.
 */
static const std::string CLOCK_GROUP_NAME       = "ClockGroup";
static const std::string CONTROLLER_GROUP_NAME  = "ControllerGroup";

/**
 * Repository registration name of the executor object group.
 */
static const std::string REPOSITORY_CLOCK_GROUP_NAME       = "clock/object_groups";
static const std::string REPOSITORY_CONTROLLER_GROUP_NAME  = "controller/object_groups";


/**
 * Interface Repository identifier of Executor
 */
static const std::string CLOCK_REPOSITORY_ID =
   "IDL:omg.org/CosClockService/Clock:1.0";
static const std::string EXECUTOR_REPOSITORY_ID =
   "IDL:omg.org/CosClockService/PeriodicExecution/Executor:1.0";
static const std::string UTC_TIME_SERVICE_REPOSITORY_ID =
   "IDL:omg.org/CosClockService/UtcTimeService:1.0";
static const std::string CONTROLLER_REPOSITORY_ID =
   "IDL:omg.org/CosClockService/PeriodicExecution/Controller:1.0";
static const std::string CONTROLLED_CLOCK_REPOSITORY_ID =
   "IDL:omg.org/CosClockService/ControlledClock:1.0";

/**
 * Identifier of the Data Store for ControllerData type.
 */
static const int CLOCK_DATA_IDENTIFIER             = 100;
static const int CONTROLLER_DATA_IDENTIFIER        = 101;
static const int CONTROLLED_CLOCK_DATA_IDENTIFIER  = 102;


/**
 * Maximum number of state transfer transactions in progress. 
 */
static const int MAX_TRANSACTION_IN_PROGRESS = 10;

/**
 * Maximum number of state transfer transactions done. 
 */
static const int MAX_TRANSACTION_DONE = 20;

/**
 * Minimum number of replicas
 */
static const int MINIMUM_NUMBER_REPLICAS = 2;

/**
 * Replication style (only warm passive is supported)
 */
static const int REPLICATION_STYLE = FT::WARM_PASSIVE;

/**
 * Membership style (application control)
 */
static const int MEMBERSHIP_STYLE = FT::MEMB_APP_CTRL;

/**
 * Fault Monitoring style
 */
static const int FAULT_MONITORING_STYLE = FT::PULL;    
    



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
static const std::string MULTICAST_DOMAIN = "CdmwTimeDomain";


/**
 * command line option to set a Executor as primary
 */
static const std::string SET_PRIMARY_OPTION = "--SetPrimary";

/**
 * command line option to use point-to-point State Transfer
 * instead of multicast state transfer.
 * (group_creator and all executors should use or not this
 *  option at the same time)
 */
static const std::string POINT_TO_POINT_OPTION = "--PointToPoint";


#endif // CARDAMOM_STATE_TRANSFER_DATA_HPP

