/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under 
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your 
 * option) any later version. 
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public 
 * License for more details. 
 * 
 * You should have received a copy of the GNU Library General 
 * Public License along with CARDAMOM; see the file COPYING. If not, write to 
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


#include "SystemMngt/platformlibrary/Entity.hpp"



/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW PlatformMngt
*/
namespace PlatformMngt
{




// Entity implementation

//
// default constructor
//
Entity::Entity()
{

}

//
// constructor with parameters
// param entityInfo  entity info
// param entityState entity status
//
Entity::Entity(const std::string& entityInfo, 
               CdmwPlatformMngtEntity::EntityStatus entityState)
     : m_entityInfo(entityInfo), 
       m_entityState(entityState)
{

}

//
// copy constructor
//
Entity::Entity(const Entity& rhs)
{
    m_entityInfo = rhs.m_entityInfo;
    m_entityState = rhs.m_entityState;
}

//
// destructor
//
Entity::~Entity()
{

}


//
// = operator
//
Entity& Entity::operator=(const Entity& rhs)
{
    m_entityInfo = rhs.m_entityInfo;
    m_entityState = rhs.m_entityState;

    return *this;
}


} // End of namespace PlatformMngt
} // End of namespace Cdmw


