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
#ifndef CARDAMOM_LB_STATE_TRANSFER_DATA_HPP
#define CARDAMOM_LB_STATE_TRANSFER_DATA_HPP


#include <string>


/**
 * Name used to register LBGroupManager in the orb as initiali reference.
 */
static const std::string GROUP_MANAGER_NAME            ("LBGroupManager"    );


/**
 * Name of the executor object group.
 */
static const std::string GROUP_MANAGER_OBJECT_GROUP_NAME       = "LBGroupManagerGroup";

/**
 * Repository registration name of the executor object group.
 */
static const std::string REPOSITORY_GROUP_MANAGER_GROUP_NAME       = "lb_group_manager/object_groups";


/**
 * Interface Repository identifier of Executor
 */
static const std::string GROUP_MANAGER_REPOSITORY_ID =
   "IDL:CdmwLB/LBGroupManager:1.0";

/**
 * Identifier of the Data Store for ControllerData type.
 */
static const int OBJECT_GROUP_DATA_IDENTIFIER = 103;

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



#endif // CARDAMOM_LB_STATE_TRANSFER_DATA_HPP

