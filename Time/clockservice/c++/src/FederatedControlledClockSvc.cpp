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
 
#include "Foundation/commonsvcs/federation/LocalTopicUpdateManager.hpp"
#include "Time/clockservice/FederatedControlledClockSvc.hpp"

Cdmw::clock::FederatedControlledClockSvc* 
Cdmw::clock::FederatedControlledClockSvc::m_instance=0;

Cdmw::clock::FederatedControlledClockSvc::FederatedControlledClockSvc
(CORBA::ORB_ptr           orb,
 PortableServer::POA_ptr  poa,
 const std::string&       miop_corbaloc,
 int                      send_repetition)
    : m_protocol_handler(orb,
                         poa,
                         miop_corbaloc,
                         send_repetition)
{
    Cdmw::CommonSvcs::LocalTopicUpdateManager::init(&m_protocol_handler);
    Cdmw::OrbSupport::CodecBase::init(orb);
    m_codec = std::auto_ptr<Cdmw::clock::FederatedControlledClockDataCodec_t >\
                           ( new Cdmw::clock::FederatedControlledClockDataCodec_t );
}

//////////////////////////////////////////////////////////////////////////////

Cdmw::clock::FederatedControlledClockSvc::~FederatedControlledClockSvc()
    throw()
{
    // No-Op
}

//////////////////////////////////////////////////////////////////////////////

void
Cdmw::clock::FederatedControlledClockSvc::init(CORBA::ORB_ptr           orb,
                                               PortableServer::POA_ptr  poa,
                                               const std::string&       miop_corbaloc,
                                               int                      send_repetition)
{
    CDMW_ASSERT(m_instance == 0);
    m_instance =
        new FederatedControlledClockSvc(orb,
                                        poa,
                                        miop_corbaloc,
                                        send_repetition); 
}

//////////////////////////////////////////////////////////////////////////////

Cdmw::clock::FederatedControlledClockSvc&
Cdmw::clock::FederatedControlledClockSvc::instance()
{
    return *m_instance;
}

//////////////////////////////////////////////////////////////////////////////

::Cdmw::clock::FederatedControlledClockDataCodec_t&
Cdmw::clock::FederatedControlledClockSvc::get_codec()
{
    return *m_codec;
}

//////////////////////////////////////////////////////////////////////////////

::Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler&
Cdmw::clock::FederatedControlledClockSvc::get_protocol_handler()
{
    return m_protocol_handler;
}

//////////////////////////////////////////////////////////////////////////////
