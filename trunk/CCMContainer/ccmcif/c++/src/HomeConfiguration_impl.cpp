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


// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <CCMContainer/ccmcif/HomeConfiguration_impl.hpp>


namespace Cdmw {

namespace CCM {

namespace CIF {


//
// IDL:omg.org/Components/HomeConfiguration:1.0
//
HomeConfiguration_impl::HomeConfiguration_impl()
  : m_configurator(Components::Configurator::_nil()),
    m_config_values(),
    m_enable_comp_config(false),
    m_enable_home_config(true)
{
}

HomeConfiguration_impl::~HomeConfiguration_impl()
{
}


//
// IDL:omg.org/Components/HomeConfiguration/set_configurator:1.0
//
void
HomeConfiguration_impl::set_configurator(Components::Configurator_ptr cfg)
    throw(CORBA::SystemException)
{
    if (!m_enable_home_config)
    {
        throw CORBA::BAD_INV_ORDER(OrbSupport::BAD_INV_ORDER, CORBA::COMPLETED_NO);
    }

    if (CORBA::is_nil(cfg))
        throw CORBA::BAD_PARAM(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
    m_configurator = Components::Configurator::_duplicate(cfg);
}

//
// IDL:omg.org/Components/HomeConfiguration/set_configuration_values:1.0
//
void
HomeConfiguration_impl::set_configuration_values(const Components::ConfigValues& config)
    throw(CORBA::SystemException)
{
    if (!m_enable_home_config)
    {
        throw CORBA::BAD_INV_ORDER(OrbSupport::BAD_INV_ORDER, CORBA::COMPLETED_NO);
    }

    m_config_values = new Components::ConfigValues(config);
}

//
// IDL:omg.org/Components/HomeConfiguration/complete_component_configuration:1.0
//
void
HomeConfiguration_impl::complete_component_configuration(CORBA::Boolean b)
    throw(CORBA::SystemException)
{
    if (!m_enable_home_config)
    {
        throw CORBA::BAD_INV_ORDER(OrbSupport::BAD_INV_ORDER, CORBA::COMPLETED_NO);
    }

    m_enable_comp_config = b;
}

//
// IDL:omg.org/Components/HomeConfiguration/disable_home_configuration:1.0
//
void
HomeConfiguration_impl::disable_home_configuration()
    throw(CORBA::SystemException)
{
    m_enable_home_config = false;
}



} // End of namespace CIF

} // End of namespace CCM

} // End of namespace Cdmw


