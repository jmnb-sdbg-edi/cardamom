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

#ifndef _CDMW_FEDERATED_CONTROLLED_CLOCK_DATA_ENCODER_HPP_
#define _CDMW_FEDERATED_CONTROLLED_CLOCK_DATA_ENCODER_HPP_

#include <Foundation/commonsvcs/federation/TopicEncoder.hpp>
#include "Time/clockservice/FederatedControlledClockDataCodec.hpp"

namespace Cdmw {
    namespace clock {
        class FederatedControlledClockDataEncoder;
    }
}

/**
 * This class provides a way of encoding the state of a
 * <code>ControlledClock</code>.
 *
 * @author <a href="mailto:corsaro@cse.wustl.edu">Angelo Corsaro</a>
 * @version 1.0
 */
class Cdmw::clock::FederatedControlledClockDataEncoder :
    public virtual Cdmw::CommonSvcs::TopicEncoder
{
public:
    /**
     * Create a <code>FederatedControlledClockDataEncoder</code> instance.
     *
     * @param codec the codec that will be used to encode the clock state.
     */
    FederatedControlledClockDataEncoder
    (Cdmw::clock::FederatedControlledClockDataCodec_t& codec)
        throw();

    /**
     * Destroy a <code> FederatedControlledClockDataEncoder</code>
     * instance.
     */
    virtual ~FederatedControlledClockDataEncoder() throw();
    
public:
    /**
     * Accessor for the clock data.
     *
     * @return the clock data associated with this encoder.
     */
    Cdmw::clock::ControlledClockData& data();

public:
    // -- Methods inherited from Cdmw::CommonSvcs::TopicEncoder

    /**
     * Encode a topic into an octet sequence.
     *
     * @param encodedTopic the topic encapsulation.
     */
    virtual void encode(CORBA::OctetSeq& encodedTopic) const;

    /**
     * Encode a topic into an octet sequence.
     *
     * @return the topic encapsulation.
     */    
    virtual CORBA::OctetSeq* encode() const;

private:
    Cdmw::clock::ControlledClockData                   m_data; 
    Cdmw::clock::FederatedControlledClockDataCodec_t&  m_codec;
};
#endif /* _CDMW_FEDERATED_CONTROLLED_CLOCK_DATA_ENCODER_HPP_ */
