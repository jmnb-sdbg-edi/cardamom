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

#ifndef INCL_CDMW_FT_REQUESTLOGGING_REPLY_RECORDING_HPP
#define INCL_CDMW_FT_REQUESTLOGGING_REPLY_RECORDING_HPP

// Cdmw Files
#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <ftrequestlogging/CdmwFTRequestInfo.stub.hpp>

#include <FaultTolerance/ftstatemanager/DataStore.hpp>
#include <FaultTolerance/ftstatemanager/Timer.hpp>
#include <FaultTolerance/ftcommon/ReplyRecording.hpp>

DEFINE_VARIABLE_LENGTH_DATA_TRAITS(CdmwFT::ReplyData)

namespace Cdmw
{
    
namespace FT
{

namespace RequestLogging
{

class ReplyRecording: public ::Cdmw::FT::ReplyRecording,
                      public Timer_Event_Handler<std::string>
{
public:
    typedef ::Cdmw::CommonSvcs::DataStore::DataStoreInterface
        < std::string, CdmwFT::ReplyData > ReplyStore;

    /**
     * Constructor of ReplyRecording.
     *
     * @param ds ReplyStore used to hold reply info
     */
    ReplyRecording(ReplyStore& ds);

    /**
     * Destructor.
     */
    ~ReplyRecording() throw() ;

    /**
     * Record a reply.
     *
     * @param client_id callers FT id
     * @param retention_id the FT id of this invocation
     * @param expiration_time the FT expiration time
     * @param reply CDR encoded reply data
     */
    void record(const char* client_id, CORBA::ULong retention_id,
                CORBA::ULong expiration_time, const CORBA::OctetSeq& reply);

    /**
     * Retrieve a reply
     *
     * @param client_id callers FT id
     * @param retention_id the FT id of this invocation
     * @param reply CDR encoded reply data
     */
    void get_reply(const char* client_id, CORBA::ULong retention_id,
                   CORBA::OctetSeq_out reply)
        throw (Cdmw::Common::NotFoundException);

    /**
     * Time out reply data.
     *
     * @param key encoding of client id and retention id
     */
    int handle_timeout(const std::string* key);

private:
    std::string to_string(const char* client_id, CORBA::ULong retention_id);

    ReplyStore& m_reply_datastore;

    Timer<std::string> m_timer;

};

} // End of namespace ReqestLogging

} // End of namespace FT

} // End of namespace Cdmw

#endif  // INCL_CDMW_FT_REQUESTLOGGING_REPLY_RECORDING_HPP

