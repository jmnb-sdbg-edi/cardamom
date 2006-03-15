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

#ifndef INCL_CDMW_FT_COMMON_REPLY_RECORDER_HPP
#define INCL_CDMW_FT_COMMON_REPLY_RECORDER_HPP

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/common/NotFoundException.hpp>


namespace Cdmw
{
    
namespace FT
{

static const CORBA::ULong FT_REPLY_DATASTORE_ID =98765;

class ReplyRecording
{
public:

    /**
     * Destructor.
     */
    virtual ~ReplyRecording() throw() {}

    /**
     * Record a reply.
     *
     * @param client_id callers FT id
     * @param retention_id the FT id of this invocation
     * @param expiration_time the FT expiration time
     * @param reply CDR encoded reply data
     */
    virtual void record(const char* client_id, CORBA::ULong retention_id,
                        CORBA::ULong expiration_time, const CORBA::OctetSeq& reply) = 0;

    /**
     * Retrieve a reply
     *
     * @param client_id callers FT id
     * @param retention_id the FT id of this invocation
     * @param reply CDR encoded reply data
     */
    virtual void get_reply(const char* client_id, CORBA::ULong retention_id,
                           CORBA::OctetSeq_out reply)
        throw (Cdmw::Common::NotFoundException) = 0;
    
protected:

    /**
     * Constructor of ReplyRecording.
     *
     */
    ReplyRecording() {}

};

} // End of namespace FT

} // End of namespace Cdmw

#endif  // INCL_CDMW_FT_COMMON_REPLY_RECORDER_HPP

