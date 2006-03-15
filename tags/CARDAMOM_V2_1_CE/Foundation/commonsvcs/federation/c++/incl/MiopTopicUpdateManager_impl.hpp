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
#ifndef _CDMW_TIME_CommonSvcs_MIOP_TOPIC_UPDATE_MANAGER_HPP_
#define _CDMW_TIME_CommonSvcs_MIOP_TOPIC_UPDATE_MANAGER_HPP_

#include "Foundation/commonsvcs/federation/TopicUpdateManager.skel.hpp"
#include "Foundation/commonsvcs/federation/UpdateProtocolHandler.hpp"

namespace Cdmw {
    namespace CommonSvcs {
        class MiopTopicUpdateManager_impl;
    }
}

/**
 * This class implements a MIOP based distribution service. Since the
 * CORBA MIOP relies on UDP multicast the distribution mechanism provided by
 * this class is not reliable. Notice that reliability can be added
 * at higher level.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 * @see ::Cdmw::CommonSvcs::UpdateProtocolHandler
 * @see ::Cdmw::CommonSvcs::SimpleMiopUpdateProtocolHandler
 */
class Cdmw::CommonSvcs::MiopTopicUpdateManager_impl :
    public virtual POA_Cdmw::CommonSvcs::MiopTopicUpdateManager
{
public:
    /**
     * Create a <code>MiopTopicUpdateManager_impl</code> instance.
     *
     * @param orb a pointer to the ORB.
     * @param protocol_handler the protocol handler that implements
     *        the update update protocol.
     */
    MiopTopicUpdateManager_impl(CORBA::ORB_ptr orb,
                                ::Cdmw::CommonSvcs::UpdateProtocolHandler&
                                protocol_handler);
                              
    /**
     * Finalize the <code>MiopTopicUpdateManager_impl</code>
     */                                
    virtual ~MiopTopicUpdateManager_impl() throw();
    
public:
    /**
     * Distribute a <code>TopicMessage</code> to the current member of
     * the group.
     *
     * @param msg the message to be distributed.
     */
    virtual void distribute(const ::Cdmw::CommonSvcs::TopicMessage& msg)
        throw (CORBA::SystemException);
    
private:
    MiopTopicUpdateManager_impl(const MiopTopicUpdateManager_impl&);
    MiopTopicUpdateManager_impl& operator=(const MiopTopicUpdateManager_impl&);
protected:
    CORBA::ORB_var                            m_orb;
    Cdmw::CommonSvcs::UpdateProtocolHandler&  m_protocol_handler;    
};

#endif /* _CDMW_TIME_CommonSvcs_MIOP_TOPIC_UPDATE_MANAGER_HPP_ */
