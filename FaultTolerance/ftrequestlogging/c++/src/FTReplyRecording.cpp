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


// Cdmw Files
#include <Foundation/common/Assert.hpp> 
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <idllib/FT.stub.hpp>

#include <ftrequestlogging/FTReplyRecording.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <FaultTolerance/ftstatemanager/TXDataStore.hpp>
#include <Foundation/logging/FunctionLogger.hpp>


namespace Cdmw
{
    
namespace FT
{

namespace RequestLogging
{

ReplyRecording::ReplyRecording(ReplyStore& ds)
    : m_reply_datastore(ds)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    m_timer.start();
}

ReplyRecording::~ReplyRecording() throw() 
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try {
        m_timer.stop();
    } catch (...) {
        // There is nothing we can do here!
    }
}

void ReplyRecording::record(const char* client_id, CORBA::ULong retention_id,
                            CORBA::ULong expiration_time,
                            const CORBA::OctetSeq& reply)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                      "client_id="<<client_id<<",retention_id="<<retention_id<<",expiration_time"<<expiration_time);
    CdmwFT::ReplyData_var reply_data = new CdmwFT::ReplyData();
    std::string key = to_string(client_id, retention_id);
    reply_data->expiration_time = expiration_time;
    reply_data->data.length(reply.length());
    for(CORBA::ULong i = 0; i < reply.length(); i++)
    {
        reply_data->data[i] = reply[i];
    }


    // expiration_time units of 100 ns
    Cdmw::OsSupport::OS::Timeval now = Cdmw::OsSupport::OS::get_time();
    CORBA::ULong now_utc = now.seconds * 10000000L + now.microseconds * 10;

    if (expiration_time >= now_utc)
    {
        CORBA::ULong delta = expiration_time - now_utc;

        try
        {
            m_reply_datastore.insert(key, reply_data.in());


            Cdmw::OsSupport::OS::Timeval retain_time;
            retain_time.seconds = delta / 10000000L;
            retain_time.microseconds = (delta % 10000000L) / 10;

            m_timer.start_timer(this, new std::string(key), retain_time);
        }
        catch (...)
        {
            // Ignore error as operation being logged has worked
            // Is there a better approach?
            CDMW_WARN(FTLogger::GetLogger(),"Failed to log request's reply!");
        }
    }
}

void ReplyRecording::get_reply(const char* client_id,
                               CORBA::ULong retention_id,
                               CORBA::OctetSeq_out reply)
    throw (Cdmw::Common::NotFoundException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                      "client_id="<<client_id<<",retention_id="<<retention_id);
    CdmwFT::ReplyData_var reply_data
        = m_reply_datastore.select(to_string(client_id, retention_id));
    reply = new CORBA::OctetSeq(reply_data->data.length());
    reply->length(reply_data->data.length());

    for(CORBA::ULong i = 0; i < reply->length(); i++)
    {
        (*reply)[i] = reply_data->data[i];
    }
}

int ReplyRecording::handle_timeout(const std::string* key)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"key="<<key);

    // We should check the datastore is still activated, otherwise an
    // assertion is raised in remove operation of the datastore.
    try
    {
        typedef DataStore<std::string, CdmwFT::ReplyData> DSType;
        DSType& ds = dynamic_cast<DSType&>(m_reply_datastore);
        if (ds.is_activated())
        {
            m_reply_datastore.remove(*key);
        }
    }
    catch(const std::bad_cast&)
    {
        try
        {
            // the datastore is not a DataStore. Test if it is a TXDataStore
            typedef TXDataStore<std::string, CdmwFT::ReplyData> TXDSType;
            TXDSType& tds = dynamic_cast<TXDSType&>(m_reply_datastore);
            if (tds.get_datastore().is_activated())
            {
                m_reply_datastore.remove(*key);
            }
        }
        catch(const std::bad_cast&)
        {
            // the datastore is neither a TXDataStore nor a Datastore.
            m_reply_datastore.remove(*key);
        }
        catch(const Cdmw::CommonSvcs::DataStore::NotFoundException&)
        {
            // do nothing
        }
    }
    catch(const Cdmw::CommonSvcs::DataStore::NotFoundException&)
    {
        // do nothing
    }

    delete key;
    return 0;
}

std::string ReplyRecording::to_string(const char* client_id,
                                      CORBA::ULong retention_id)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                      "client_id="<<client_id<<",retention_id="<<retention_id);
    std::ostringstream id_stream;
    id_stream << client_id << "/" << retention_id;
    CDMW_LOG_FUNCTION_RETURN(id_stream.str());
    return id_stream.str();
}

} // End of namespace RequestLogging

} // End of namespace FT

} // End of namespace Cdmw
