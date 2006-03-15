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

#ifndef _CDMW_LB_GROUP_REF_CODEC_HPP_
#define _CDMW_LB_GROUP_REF_CODEC_HPP_

#include "Foundation/commonsvcs/federation/TopicEncoder.hpp"
#include "LoadBalancing/lbcommon/GroupRefCacheMessageCodec.hpp"


namespace Cdmw {
    namespace LB {
        class GroupRefEncoder;
    }
}

class Cdmw::LB::GroupRefEncoder :
    public virtual Cdmw::CommonSvcs::TopicEncoder
{
public:
    GroupRefEncoder(::Cdmw::LB::GroupRefUpdateMessageCodec_t& codec);
    
    ~GroupRefEncoder()
        throw(){};

public:
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

    ::CdmwLB::GroupRefUpdateMessage& data();
    
private:
    ::CdmwLB::GroupRefUpdateMessage m_data;
    ::Cdmw::LB::GroupRefUpdateMessageCodec_t& m_codec;
};

#endif /* _CDMW_LB_GROUP_REF_CODEC_HPP_ */
