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
#ifndef _CDMW_FOUNDATION_CommonSvcs_TOPIC_UPDATE_HANDLER_H_
#define _CDMW_FOUNDATION_CommonSvcs_TOPIC_UPDATE_HANDLER_H_

#include "tao/OctetSeqC.h"

namespace Cdmw {
        namespace CommonSvcs {
            class TopicUpdateHandler; 
        }
}

/**
 * The <code>TopicUpdateHandler</code> takes care of handling topic
 * updates.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
class Cdmw::CommonSvcs::TopicUpdateHandler
{
public:
    /**
     * Destroy a <code>TopicUpdateHandler</code> instance.
     */ 
    virtual ~TopicUpdateHandler() throw();
    
public:
    /**
     * Handle a topic update.
     *
     * @param data the data containing the topic update.
     */
    virtual void handle_update(const CORBA::OctetSeq& data) = 0;
};

#endif /* _CDMW_CommonSvcs_TOPIC_UPDATE_HANDLER_H_ */
