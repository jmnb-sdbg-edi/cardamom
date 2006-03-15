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

#ifndef _CDMW_CLOCK_FEDERATED_CONTROLLED_CLOCK_SVC_HPP_
#define _CDMW_CLOCK_FEDERATED_CONTROLLED_CLOCK_SVC_HPP_

#include "Foundation/commonsvcs/federation/SimpleMiopUpdateProtocolHandler.hpp"
#include "Time/clockservice/FederatedControlledClockDataCodec.hpp"
#include <memory>

namespace Cdmw {
    namespace clock {
        class FederatedControlledClockSvc;
    }
}

/**
 * This utility class provides access to the clock federation service.
 * It provides methods for initializing the service, as well as for
 * getting the codec and protocol handler.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
class Cdmw::clock::FederatedControlledClockSvc
{
public:
    /**
     * Destoy a <code>FederatedControlledClockSvc</code> instance.
     */
    ~FederatedControlledClockSvc() throw();
    
public:
    /**
     * Initialize the service.
     *
     * @param orb  the ORB instance.
     * @param poa  the POA on which the needed CORBA objects
                   will be activated.
     * @param miop_corbaloc  the corbaloc under wich the service will register. 
     * @param send_repetition the number of times that a message has to resent.
     */
    static void init(CORBA::ORB_ptr           orb,
                     PortableServer::POA_ptr  poa,
                     const std::string&       miop_corbaloc,
                     int                      send_repetition);

    /**
     * Provide access the the unique instance of the service.
     *
     * @return the unique instance of the service.
     */ 
    static FederatedControlledClockSvc& instance();

    /**
     * Return the service codec.
     *
     * @return the codec.
     */
    ::Cdmw::clock::FederatedControlledClockDataCodec_t& get_codec();

    /**
     * Return the service protocol handler.
     *
     * @return the protocol handler.
     */ 
    ::Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler& get_protocol_handler();

private:
    // -- Forbid Copy Ctor and Assignment Operator.
    FederatedControlledClockSvc(const FederatedControlledClockSvc&);
    FederatedControlledClockSvc& operator=(const FederatedControlledClockSvc&);

protected:
    FederatedControlledClockSvc(CORBA::ORB_ptr           orb,
                                PortableServer::POA_ptr  poa,
                                const std::string&       miop_corbaloc,
                                int                      send_repetition);
    
private:
    ::Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler  m_protocol_handler;
    std::auto_ptr<Cdmw::clock::FederatedControlledClockDataCodec_t>   m_codec;
    static FederatedControlledClockSvc*                  m_instance;
};

#endif /* _CDMW_CLOCK_FEDERATED_CONTROLLED_CLOCK_SVC_HPP_ */
