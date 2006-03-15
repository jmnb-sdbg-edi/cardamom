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

/**
 * @brief An Activation Handler for LBGroupManager fault tolerance
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */


#include "LoadBalancing/lbgroupmanagerft/LBGroupManagerActivationHandler.hpp"
#include "LoadBalancing/lbgroupmanagerft/FTGroupManager_impl.hpp"



Cdmw::LB::LBGroupManagerActivationHandler::LBGroupManagerActivationHandler(const std::string&        name,
									   Cdmw::LB::FTGroupManager_impl* group_manager_impl)
    : m_name(name),
      m_is_active(false),
      m_group_manager_impl(group_manager_impl)
      
      
{
    m_group_manager_impl->_add_ref();
   
}

Cdmw::LB::LBGroupManagerActivationHandler::~LBGroupManagerActivationHandler()
{
    m_group_manager_impl->_remove_ref();
}

void
Cdmw::LB::LBGroupManagerActivationHandler::activate()
    throw (CORBA::SystemException)
{
    m_is_active = true;
    m_group_manager_impl->activate();
}

void
Cdmw::LB::LBGroupManagerActivationHandler::deactivate()
    throw (CORBA::SystemException)
{
    m_is_active = false;
}

bool
Cdmw::LB::LBGroupManagerActivationHandler::is_active()
{
    return m_is_active;
}

Cdmw::LB::FTGroupManager_impl*
Cdmw::LB::LBGroupManagerActivationHandler::getServant()
{
    return m_group_manager_impl;
}
