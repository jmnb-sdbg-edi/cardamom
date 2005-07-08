/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

#include "Foundation/orbsupport/CORBA.hpp"
#include "Time/clockservice/FederatedControlledClockDataEncoder.hpp"

Cdmw::clock::FederatedControlledClockDataEncoder::FederatedControlledClockDataEncoder
(Cdmw::clock::FederatedControlledClockDataCodec_t& codec)
    throw()
    : m_codec(codec)
{
    // No-Op
}

Cdmw::clock::FederatedControlledClockDataEncoder::~FederatedControlledClockDataEncoder()
    throw()
{
    // No-Op
}

Cdmw::clock::ControlledClockData&
Cdmw::clock::FederatedControlledClockDataEncoder::data()
{
    return m_data;
}

void
Cdmw::clock::FederatedControlledClockDataEncoder::encode
(CORBA::OctetSeq& encodedTopic) const
{
    // Angelo 3 feb 2005> This copy should be removed. To do so the
    // Codec interface has to be extended. Perhaps we should plan this
    // change for the V2. Notice that all the purpose of the Encoder
    // simply to avoid coping...
    CORBA::OctetSeq* temp = this->encode();
    encodedTopic = *temp;
    delete temp;

}

CORBA::OctetSeq*
Cdmw::clock::FederatedControlledClockDataEncoder::encode() const
{
    return m_codec.encode(m_data);
}
