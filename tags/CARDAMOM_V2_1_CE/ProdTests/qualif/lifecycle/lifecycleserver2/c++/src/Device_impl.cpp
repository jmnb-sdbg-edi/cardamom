/* =========================================================================== *
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
 * =========================================================================== */

#include <iostream>
#include "lifecycleserver2/Device_impl.hpp"

using namespace std;

namespace Cdmw
{
namespace Resources
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/
// Device_impl::Device_impl()
//     throw(CORBA::SystemException)
//     : m_poa(PortableServer::POA::_nil()),
//       m_state(CF::Device::IDLE),
//       m_location("nowhere")
// {
// }

Device_impl::Device_impl(PortableServer::POA_ptr                     poa,
                         const Cdmw::LifeCycle::ObjectDeactivator & deactivator,
                         CF::Device::UsageType                       state,
                         const std::string &                         location)
    throw(CORBA::SystemException)
    : m_poa(PortableServer::POA::_duplicate(poa)),
      m_object_deactivator(deactivator),
      m_state(state),
      m_location(location)
{
}

Device_impl::~Device_impl()
    throw()
{
  std::cerr << "Device_impl::~Device_impl()" << std::endl;
}


//
// IDL:thalesgroup.com/CF/Device/start:1.0
//
void Device_impl::start()
  throw(CF::Device::StartError,
        CORBA::SystemException)
{
  if (m_state == CF::Device::IDLE)
    m_state = CF::Device::ACTIVE;
  else if (m_state == CF::Device::ACTIVE)
    throw CF::Device::StartError("Device active!");
  else
    throw CF::Device::StartError("Device busy!");
}

//
// IDL:thalesgroup.com/CF/Device/stop:1.0
//
void Device_impl::stop()
  throw(CF::Device::StopError,
        CORBA::SystemException)
{
  if (m_state == CF::Device::IDLE)
    throw  CF::Device::StopError("Device not active!");
  else if (m_state == CF::Device::ACTIVE) {
    m_state = CF::Device::IDLE;
    try {
        m_object_deactivator.deactivate_object();
    } catch (...) {
    }
  }
  else
    throw CF::Device::StopError("Device busy!");
}

//
// IDL:thalesgroup.com/CF/Device/state:1.0
//
CF::Device::UsageType Device_impl::state()
  throw(CORBA::SystemException)
{
  return m_state;
}

//
// IDL:thalesgroup.com/CF/Device/label:1.0
//
CF::Device::Location Device_impl::label()
  throw(CORBA::SystemException)
{
  return CORBA::string_dup(m_location.c_str());
}
        

}; // End namespace Resources
}; // End namespace Cdmw

