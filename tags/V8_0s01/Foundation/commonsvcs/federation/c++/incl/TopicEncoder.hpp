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
#ifndef _CDMW_CommonSvcs_TOPIC_ENCODER_HPP_
#define _CDMW_CommonSvcs_TOPIC_ENCODER_HPP_

#include "tao/OctetSeqC.h"

namespace Cdmw {
        namespace CommonSvcs {
            class TopicEncoder; 
    }
}

/**
 * This class creates an encapsulation (i.e. a sequence of octet)
 * starting from a given topic.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
class Cdmw::CommonSvcs::TopicEncoder
{
public:
    /**
     * Destroy a <code>TopicEncoder</code> instance.
     */ 
    virtual ~TopicEncoder() throw();
    
public:
    /**
     * Encode a topic into an octet sequence.
     *
     * @param encodedTopic the topic encapsulation.
     */
    virtual void encode(CORBA::OctetSeq& encodedTopic) const = 0;

    /**
     * Encode a topic into an octet sequence.
     *
     * @return the topic encapsulation.
     */
    virtual CORBA::OctetSeq* encode() const = 0;
};

#endif /* _CDMW_CommonSvcs_TOPIC_ENCODER_HPP_ */
