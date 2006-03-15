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
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

// Local Files
#include <ftinit/FTServerRequestInterceptor_impl.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <idllib/FT.stub.hpp>

#include <sstream>

#if TAO_HAS_EXTENDED_FT_INTERCEPTORS == 1
#include "tao/Any_Impl.h"
#include "tao/DynamicC.h"
#endif /*TAO_HAS_EXTENDED_FT_INTERCEPTORS*/



namespace 
{
    const PortableInterceptor::ORBInitInfo::ObjectId FT_SERVER_REQUEST_INTERCEPTOR_NAME 
        = "FTServerRequestInterceptor";
    const PortableInterceptor::ORBInitInfo::ObjectId FT_REPLICATION_MANAGER 
        = "ReplicationManager";
    const PortableInterceptor::ORBInitInfo::ObjectId FT_GROUP_REPOSITORY 
        = "FTGroupRepository";


    // TODO: Move this to Foundation CSCI
    TimeBase::TimeT 
    convert_TimeVal_to_TimeT (const ::Cdmw::OsSupport::OS::Timeval & tv)
    {
        // FIXME: TAO Does not seem use DCE time timebase for the expiration_time
        // but uses rather Posix time!
        // TimeVal is in sec/microsec from 1st Jan 1970 (This is different in Windows!!!)
        //
        // Time difference in 100 nanoseconds between DCE and POSIX
        // is 141427 days.
        //        static const TimeBase::TimeT DELTA_POSIX_DCE = 
        //            static_cast<TimeBase::TimeT>(141427) * 
        //            static_cast<TimeBase::TimeT>(86400) *
        //            static_cast<TimeBase::TimeT>(10000000);
        //        TimeBase::TimeT result = 
        //            DELTA_POSIX_DCE +
        //            static_cast<TimeBase::TimeT>(tv.seconds ) * 10000000 +
        //            static_cast<TimeBase::TimeT>(tv.microseconds) * 10 ;

        TimeBase::TimeT result = 
            static_cast<TimeBase::TimeT>(tv.seconds ) * 10000000 +
            static_cast<TimeBase::TimeT>(tv.microseconds) * 10 ;
        return result;
    }

    //
    // Return the current time in DCE std: 100 ns from 15/10/1582
    //
    TimeBase::TimeT
    get_DCE_time()
        throw( ::Cdmw::InternalErrorException )
    {
        // First get current time in sec/microsec from 1st Jan 1970
        return convert_TimeVal_to_TimeT(::Cdmw::OsSupport::OS::get_time());
    }

} // end of anonymous namespace


namespace Cdmw
{
    
namespace FT
{

namespace FTInit
{



ServerRequestInterceptor_impl::ServerRequestInterceptor_impl(
                 PortableInterceptor::ORBInitInfo_ptr info,
                 const PortableInterceptor::SlotId  ft_domain_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_ref_version_slot_id,
                 const PortableInterceptor::SlotId  client_id_slot_id,
                 const PortableInterceptor::SlotId  retention_id_slot_id,
                 const PortableInterceptor::SlotId  expiration_time_slot_id,
                 const PortableInterceptor::SlotId  is_a_ft_request_slot_id,
                 const PortableInterceptor::SlotId  is_request_valid_slot_id)
    : m_ft_domain_id_slot_id(ft_domain_id_slot_id),
      m_object_group_id_slot_id(object_group_id_slot_id),
      m_object_group_ref_version_slot_id(object_group_ref_version_slot_id),
      m_client_id_slot_id(client_id_slot_id),
      m_retention_id_slot_id(retention_id_slot_id),
      m_expiration_time_slot_id(expiration_time_slot_id),
      m_is_a_ft_request_slot_id(is_a_ft_request_slot_id),
      m_is_request_valid_slot_id(is_request_valid_slot_id),
      m_recorder(0)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    m_recorder = NULL;
    
    CDMW_DEBUG(FTLogger::GetLogger(),"m_recorder: " << m_recorder);
    //
    // Get the codec factory
    //
    IOP::CodecFactory_var factory = info->codec_factory();
    CDMW_ASSERT(!CORBA::is_nil(factory.in()));

    //
    // Create codec
    //
    IOP::Encoding how;
    how.major_version = 1;
    how.minor_version = 0;
    how.format = IOP::ENCODING_CDR_ENCAPS;

    try
    {
        m_cdrCodec = factory->create_codec(how);
    }
    catch(const IOP::CodecFactory::UnknownEncoding&)
    {
        CDMW_ASSERT(false);
    }

    CDMW_ASSERT(!CORBA::is_nil(m_cdrCodec.in()));

    try
    {
        CORBA::Object_var obj = info->resolve_initial_references(FT_GROUP_REPOSITORY);
        m_group_repository 
            = CdmwFT::Location::PrimaryBackupGroupRepository::_narrow(obj.in());
    }
    catch (const PortableInterceptor::ORBInitInfo::InvalidName& )
    {
        CDMW_FATAL(FTLogger::GetLogger(),
                   "Impossible to resolve initial reference: InvalidName raised!");
        CDMW_ASSERT(false);
    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_FATAL(FTLogger::GetLogger(),
                   "Impossible to resolve initial reference: CORBA::SystemException raised!\n" 
                   << ex );
        CDMW_ASSERT(false);
    }

    CDMW_ASSERT(!CORBA::is_nil(m_group_repository.in()));
}

char*
ServerRequestInterceptor_impl::name() 
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_LOG_FUNCTION_RETURN(FT_SERVER_REQUEST_INTERCEPTOR_NAME);
    return CORBA::string_dup(FT_SERVER_REQUEST_INTERCEPTOR_NAME);
}

void
ServerRequestInterceptor_impl::destroy() 
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

void
ServerRequestInterceptor_impl::receive_request_service_contexts(
        PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
#if TAO_HAS_EXTENDED_FT_INTERCEPTORS == 0
    
    try {     
      ::FT::FTRequestServiceContext_var          ft_request_context;
        if (check_incoming_request(ri,ft_request_context.out())) {
            // Request is valid. There is no request logging so no handling 
            // of expiration time & request logging
            if (m_recorder) {
                CDMW_DEBUG(FTLogger::GetLogger(),"reply is not recorded");
                
                CORBA::ULong expiration_time = ft_request_context->expiration_time;
                
                Cdmw::OsSupport::OS::Timeval now =
                    Cdmw::OsSupport::OS::get_time();
                CORBA::ULong current_time = 
                    (CORBA::ULong) (now.seconds * 10000000L + now.microseconds * 10);
                if (expiration_time < current_time ) {
                    CDMW_INFO(FTLogger::GetLogger(),
                              "Request has expired (expiration_time=" << expiration_time
                              << ", now=" << current_time << "). Raising BAD_CONTEXT exception.");
                    // The operation may have already been executed so
                    // in accordance with the CORBA FT specification
                    // we raise BAD_CONTEXT
                    CORBA::BAD_CONTEXT ex;
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
            }
        }
    } catch (const PortableInterceptor::ForwardRequest & ex) {
        CDMW_DEBUG(FTLogger::GetLogger(),
                   "PortableServer::ForwardRequest exception raised in receive_request_service_contexts!\n");
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    } catch (const CORBA::SystemException& ex) {
        CDMW_DEBUG(FTLogger::GetLogger(),
                   "CORBA System exception raised in receive_request_service_contexts!\n" << ex);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }

#else
    // tao_ft_interception_point has already perfomed checking ... etc.

    try {
        bool is_request_valid = false;
        CORBA::Any_var any = ri->get_slot(m_is_request_valid_slot_id);

        if (!(any >>= CORBA::Any::to_boolean(is_request_valid))) {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "ERROR: unexpected data returned by get_slot" );
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        if (is_request_valid) {
            if (m_recorder) {
                ::FT::FTRequestServiceContext* context = get_FTRequestServiceContext(ri); 
                if (context) { // FT Request. Check expiration time
                    CDMW_DEBUG(FTLogger::GetLogger(),"reply was not recorded");
                    
                    TimeBase::TimeT expiration_time = context->expiration_time;
                    
                    TimeBase::TimeT current_time = get_DCE_time();
                    if (expiration_time < current_time ) {
                        CDMW_INFO(FTLogger::GetLogger(),
                                  "Request has expired (expiration_time=" << expiration_time
                                   << ", now=" << current_time << "). Raising BAD_CONTEXT exception.");
                        // The operation may have already been executed so
                        // in accordance with the CORBA FT specification
                        // we raise BAD_CONTEXT
                        CORBA::BAD_CONTEXT ex;
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    }
                }
            }
        } else {
            // An exception was raised!
            
            // There is either a SystemException or a ForwardRequest exception!
            // The exception could have been stored in a slot; but since TAO does not
            // provide an operation for extracting an exception from an CORBA::Any,
            // we will perform the processing again! We only pay when there is an exception
            // to raise.
            
            ::FT::FTRequestServiceContext_var          ft_request_context;
            check_incoming_request(ri,ft_request_context.out());
            // Should not be here! TODO: Check this!!!!
            CDMW_ERROR(FTLogger::GetLogger(),
                       "INTERNAL ERROR: exception was previously tagged as unvalid" );
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    } catch (const PortableInterceptor::InvalidSlot& ) {
        // This slot that has not been allocated
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: InvalidSlot exception raised in receive_request_service_contexts!");
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    } catch (const PortableInterceptor::ForwardRequest & ex) {
        CDMW_DEBUG(FTLogger::GetLogger(),
                   "PortableServer::ForwardRequest exception raised in receive_request_service_contexts!\n");
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    } catch (const CORBA::SystemException& ex) {
        CDMW_DEBUG(FTLogger::GetLogger(),
                   "CORBA System exception raised in receive_request_service_contexts!\n" << ex);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }

#endif /*TAO_HAS_EXTENDED_FT_INTERCEPTORS == 0 */
    
}

void
ServerRequestInterceptor_impl::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    // Do nothing as this is done in receive_request_service_contexts() or tao_ft_interception_point()
}


void
ServerRequestInterceptor_impl::send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
#if TAO_HAS_EXTENDED_FT_INTERCEPTORS == 1
    
    if (m_recorder)
    {
        ::FT::FTRequestServiceContext* context = 0; 
    
        try
        {
            context = get_service_context(ri,this->get_location()); // FIXME: TAO BUG Workaround: 
            // calling get_FTRequestServiceContext(ri) causes an assertion in get_slot() 
            // get_FTRequestServiceContext(ri); 
    
            if (context)
            { // FT Request. Log reply
                CORBA::Any_var result = ri->result ();
    
                TAO_OutputCDR cdr;
                result->impl ()->marshal_value (cdr);
    
                Dynamic::ParameterList_var pl = ri->arguments ();
    
                CORBA::ULong len = pl->length ();

                for (CORBA::ULong index = 0; index != len ; ++index)
                {
                    // Save INOUT & OUT parameters
                    if ((*pl)[index].mode == CORBA::PARAM_INOUT)
                    {
                        (*pl)[index].argument.impl ()->marshal_value (cdr);
                    } else if ((*pl)[index].mode == CORBA::PARAM_OUT)
                    {
                        (*pl)[index].argument.impl ()->marshal_value (cdr);
                    }
                }
    
                CORBA::OctetSeq_var reply;
    
                ACE_NEW (reply, CORBA::OctetSeq (cdr.total_length ()));
    
                reply->length (cdr.total_length ());
    
                CORBA::Octet* buf = reply->get_buffer ();
    
                // @@ What if this throws an exception??
                // We don't have any way to check whether this succeeded
                for (ACE_Message_Block const* mb = cdr.begin ();
                     mb != 0;
                     mb = mb->cont ())
                {
                    ACE_OS::memcpy (buf, mb->rd_ptr (), mb->length ());
                    buf += mb->length ();
                }
                m_recorder->record(context->client_id.in(),
                                   context->retention_id,
                                   context->expiration_time,
                                   reply.in());
                delete context;
            }
        }
        catch (const CORBA::SystemException& ex)
        {
            CDMW_DEBUG(FTLogger::GetLogger(),
                       "ERROR: send_reply FT_REQUEST raises exception " << ex);
            delete context;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
    }
#endif
}

void
ServerRequestInterceptor_impl::send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    // nothing to do!
}

void
ServerRequestInterceptor_impl::send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    // nothing to do!
}

#if TAO_HAS_EXTENDED_FT_INTERCEPTORS == 1
void
ServerRequestInterceptor_impl::tao_ft_interception_point(PortableInterceptor::ServerRequestInfo_ptr ri,
                                                         CORBA::OctetSeq_out ocs)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try {
   
        ::FT::FTRequestServiceContext_var          ft_request_context;
        if (check_incoming_request(ri,ft_request_context.out())) {
            
           // Request is valid. Check if already logged
            if (m_recorder)
                {
                   try
                        {
                           m_recorder->get_reply(ft_request_context->client_id.in(),
                                                  ft_request_context->retention_id, ocs);
                           CDMW_DEBUG(FTLogger::GetLogger(),"reply is recorded, returing previous reply.");
                        }
                    catch (const Cdmw::Common::NotFoundException&)
                        {
                            // Request is not recorded. Do nothing!
                        }
                }
          }
        // set the slot id to indicate that this request has not generated any exception!
        bool is_request_valid = true;
        CORBA::Any slotData;
        slotData <<= CORBA::Any::from_boolean(is_request_valid);
       ri->set_slot(m_is_request_valid_slot_id, slotData);
    } catch (const CORBA::Exception & ex) {
        CDMW_DEBUG(FTLogger::GetLogger(),
                   "CORBA Exception raised in tao_ft_interception_point!" << ex );
      // set the slot id to indicate that this request has generated an exception!
      bool is_request_valid = false;
      CORBA::Any slotData;
 
      slotData <<= CORBA::Any::from_boolean(is_request_valid);
      ri->set_slot(m_is_request_valid_slot_id, slotData);
    }
}
#endif /*TAO_HAS_EXTENDED_FT_INTERCEPTORS*/



void
ServerRequestInterceptor_impl::set_reply_recorder
(Cdmw::FT::ReplyRecording* recorder)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    m_recorder = recorder;
}






/**
 * This operation extracts the (FT-specific) service contexts that may 
 * be attached to the request. It then sets appropriate PI slots
 * (from now on, FTCurrent can be used to get these info)
 *
 * @return true if this an FT request, false otherwise
 *
 * @exception TODO: describe exceptions...
 */
bool
ServerRequestInterceptor_impl::get_service_contexts(PortableInterceptor::ServerRequestInfo_ptr ri,
                                                    const char *                        location_str,
                                                    ::FT::FTRequestServiceContext_out   ft_request_context,
                                                    ::FT::TagFTGroupTaggedComponent_out ft_group_version_context)
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location_str="<<location_str);
    
    bool is_a_ft_request = false;

    //
    // Get FT_REQUEST request service context 
    //
    bool contain_a_ft_request_sc = false;
    try
    {
        IOP::ServiceContext_var sc 
            = ri->get_request_service_context(IOP::FT_REQUEST);

        if (sc->context_id == IOP::FT_REQUEST)
        {
            CDMW_DEBUG(FTLogger::GetLogger()," context_id: FT_REQUEST");
            contain_a_ft_request_sc = true;

            CORBA::OctetSeq data(sc->context_data.length());
            data.length(sc->context_data.length());
            for (unsigned int i=0; i < sc->context_data.length(); i++)
            {
                data[i] = sc->context_data[i];
            }
            //memcpy(data.get_buffer(), sc->context_data.get_buffer(),
            //    sc->context_data.length());
            CORBA::Any_var any = m_cdrCodec->decode_value(data, ::FT::_tc_FTRequestServiceContext);
            
            const ::FT::FTRequestServiceContext* context;
            if (any.in() >>= context)
            {
                CDMW_DEBUG(FTLogger::GetLogger(),
                           "FTRequestServiceContext: "
                           << "\n\t        Client ID: " << context->client_id.in()
                           << "\n\t        Retention ID: " << context->retention_id
                           << "\n\t        Expiration Time: " << context->expiration_time);

                ft_request_context = new ::FT::FTRequestServiceContext(*context);

                //
                // Store Request Service Context in slot 
                //
                CORBA::Any slotData;

                slotData <<= context->client_id.in();
                ri->set_slot(m_client_id_slot_id, slotData);

                slotData <<= context->retention_id;
                ri->set_slot(m_retention_id_slot_id, slotData);

                slotData <<= context->expiration_time;
                ri->set_slot(m_expiration_time_slot_id, slotData);
            }
            else
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "Error occured when reading FTRequestServiceContext");
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
        }
        else
        {
            CDMW_ERROR(FTLogger::GetLogger(),"Bad context id for IOP::FT_REQUEST");
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
    catch(const CORBA::BAD_PARAM&)
    {
        //DEBUG_DUMP(location_str, "No service context FT_REQUEST for that request");
        // This is not an error while a request can come to a single reference
        // with no FT service context.

    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: get_request_service_context FT_REQUEST raises exception " << ex);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }


    //
    // Get FT_GROUP_VERSION request service context 
    //
    bool contain_a_ft_group_version_sc = false;
    try
    {
        IOP::ServiceContext_var sc 
            = ri->get_request_service_context(IOP::FT_GROUP_VERSION);

        if (sc->context_id == IOP::FT_GROUP_VERSION)
        {
            CDMW_DEBUG(FTLogger::GetLogger(), 
                       "Context_id: FT_GROUP_VERSION");
            contain_a_ft_group_version_sc = true;

            CORBA::OctetSeq data(sc->context_data.length());
            data.length(sc->context_data.length());
            for (unsigned int i=0; i < sc->context_data.length(); i++)
            {
                data[i] = sc->context_data[i];
            } 

            //
            // The following code is right but TAO doesn't follow the standard!
            // It replaces the FTGroupVersionServiceContext by the TagFTGroupTaggedComponent.
            //
            //CORBA::Any_var any = m_cdrCodec->decode_value(data, FT::_tc_FTGroupVersionServiceContext);
            //
            //const FT::FTGroupVersionServiceContext* context;
            //if (any.in() >>= context)
            //{
            //    DEBUG_DUMP(location_str, "  FTGroupVersionServiceContext: ");
            //    DEBUG_ECHO("     Object group ref version: " << context->object_group_ref_version);
            //
            
            CORBA::Any_var any = m_cdrCodec->decode_value(data, ::FT::_tc_TagFTGroupTaggedComponent);
            
            const ::FT::TagFTGroupTaggedComponent* context;
                        
            if (any.in() >>= context)
            {
                CDMW_DEBUG(FTLogger::GetLogger(),
                           "FTGroupVersionServiceContext: "
                           << "\n\t        version = " 
                           << (int)context->component_version.major << "." 
                           << (int)context->component_version.minor
                           << "\n\t        ft_domain_id = " << context->group_domain_id.in()
                           << "\n\t        object_group_id = " << context->object_group_id
                           << "\n\t        object_group_ref_version = " << context->object_group_ref_version);       

                ft_group_version_context = new ::FT::TagFTGroupTaggedComponent(*context);
                
                //
                // Store Request Service Context in slot 
                //
                CORBA::Any slotData;

                slotData <<= context->group_domain_id.in();
                ri->set_slot(m_ft_domain_id_slot_id, slotData);

                slotData <<= context->object_group_id;
                ri->set_slot(m_object_group_id_slot_id, slotData);

                slotData <<= context->object_group_ref_version;
                ri->set_slot(m_object_group_ref_version_slot_id, slotData);
            }
            else
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "ERROR: Error occured when reading FTGroupVersionServiceContext");
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }   
        }
        else
        {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "ERROR: Bad context id for IOP::FT_GROUP_VERSION");
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
    catch(const CORBA::BAD_PARAM&)
    {
        //DEBUG_DUMP(location_str, "No service context FT_GROUP_VERSION for that request");

        // This is not an error while a request can come to a single reference.
        // with no FT service context.
    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: get_request_service_context FT_GROUP_VERSION raises exception " << ex );
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
    catch (const CORBA::Exception& ex)
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: get_request_service_context FT_GROUP_VERSION raises CORBA exception " << ex);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw; // FIXME: This may cause an unexpected exception if not a PortableInterceptor::ForwardRequest exception (may abort)
    }
    catch (...)
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: get_request_service_context raises unknown exception ");
        throw; // FIXME: This will cause an unexpected exception (may abort)
    }

    if ((contain_a_ft_group_version_sc) != (contain_a_ft_request_sc))
    {
        // a service context is missing, throw exception
        CDMW_ERROR(FTLogger::GetLogger(),"ERROR: a service context is missing! ");
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    else
    {
        if (contain_a_ft_group_version_sc == true)
        {
            // set the slot id to indicate that this is a ft_request!
            CORBA::Any slotData;
            is_a_ft_request = true;
            
            slotData <<= CORBA::Any::from_boolean(is_a_ft_request);
            ri->set_slot(m_is_a_ft_request_slot_id, slotData);
        }
        else
        {
            // set the slot id to indicate that this is not a ft_request!
            CORBA::Any slotData;
            is_a_ft_request = false; /* default value*/
            
            slotData <<= CORBA::Any::from_boolean(is_a_ft_request);
            ri->set_slot(m_is_a_ft_request_slot_id, slotData);
        }
    }
    return is_a_ft_request;
}

/**
 * This operation insures server-side FT CORBA failover semantics
 */
void 
ServerRequestInterceptor_impl::validate_request(PortableInterceptor::ServerRequestInfo_ptr ri,
                                                const char *                            location_str,
                                                const ::FT::FTRequestServiceContext &   ft_request_context,
                                                const ::FT::TagFTGroupTaggedComponent & ft_group_version_context)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location_str="<<location_str);
    ::FT::ObjectGroupRefVersion object_group_version = 0;
    ::FT::ObjectGroupRefVersion request_object_group_version = ft_group_version_context.object_group_ref_version;
    ::FT::ObjectGroupId object_group_id = ft_group_version_context.object_group_id;

// DEBUG_DUMP(location_str, "  FTRequestServiceContext: ");
//                DEBUG_ECHO("        Client ID: " << context->client_id.in());
//                DEBUG_ECHO("        Retention ID: " << context->retention_id);
//                DEBUG_ECHO("        Expiration Time: " << context->expiration_time);
// const ::FT::TagFTGroupTaggedComponent* context;
//                DEBUG_ECHO("        ft_domain_id = " << context->group_domain_id.in());
//                DEBUG_ECHO("        object_group_id = " << context->object_group_id);
//                DEBUG_ECHO("        object_group_ref_version = " << context->object_group_ref_version); 
    try
    {
        // get group_version from the GroupRepository
        object_group_version =
            m_group_repository->get_object_group_version_from_gid(object_group_id);
        CDMW_DEBUG(FTLogger::GetLogger(), 
                   " group_version from the GroupRepository = " << object_group_version
                   << ", request_object_group_version = " << request_object_group_version);
    }
    catch (const ::FT::ObjectGroupNotFound& )
    {
        CDMW_WARN(FTLogger::GetLogger(), 
                   "ObjectGoup not known yet, throw transient");
        CORBA::TRANSIENT ex(OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_ERROR(FTLogger::GetLogger(), 
                   "ERROR: System exception raised while checking the group version!\n" << ex);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }

    // Now compare the both version
    if (request_object_group_version == object_group_version)
    {
        // we are in a primary backup configuration
        // check if we are the primary member
        bool is_primary;
        
        try
        {
            is_primary = m_group_repository->is_primary_from_gid(object_group_id);
        }
        catch (const ::FT::ObjectGroupNotFound &)
        {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "ERROR: is_primary_from_gid raises ObjectGroupNotFound exception ");
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        catch (const CORBA::SystemException& ex)
        {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "ERROR: System exception raised while checking the primary member!\n"
                       << ex );
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
        
        if (is_primary)
        {
            // all is ok, let the request go up!
            CDMW_DEBUG(FTLogger::GetLogger(), 
                       "Request is on the primary, let the request go up!");
            CDMW_LOG_FUNCTION_RETURN("");
            return;
        }
        else
        {
            // throw transient!
            CDMW_DEBUG(FTLogger::GetLogger(), 
                       "Not the primary, throw transient");
            CORBA::TRANSIENT ex(OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
    else
    {
        if (request_object_group_version < object_group_version)
        {
            // request object group reference is obsolete, 
            // get the local group reference 
            try
            {
                CDMW_DEBUG(FTLogger::GetLogger(), 
                           "Request object group reference is obsolete, get the local group reference and throw a location forward");
                ::FT::ObjectGroup_var object_group =
                    m_group_repository->get_object_group_from_gid(object_group_id);
    
                // throw a forward request permanent (bool = 1) with the local group reference
                CDMW_DEBUG(FTLogger::GetLogger(), 
                           "Throw a location forward permanent");
                PortableInterceptor::ForwardRequest ex(object_group.in(), 1);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            catch (const ::FT::ObjectGroupNotFound &)
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "ERROR: get_object_group_from_gid raises ObjectGroupNotFound exception ");
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            catch (const CORBA::SystemException& ex)
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "ERROR: System exception raised while getting the group reference!\n"
                           << ex);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            }
        }
        else
        {
            // request_object_group_version > object_group_version
            // local object group is obsolete, check with the replication manager
            ::FT::ObjectGroupRefVersion last_object_group_version;
            try
            {
                CDMW_DEBUG(FTLogger::GetLogger(), 
                           "Local object group is obsolete, check with the replication manager");
                CdmwFT::ReplicationManager_var replication_manager
                    = m_group_repository->the_replication_manager();
                last_object_group_version
                    = replication_manager->get_object_group_version_from_gid(object_group_id);
            }
            catch (const ::FT::ObjectGroupNotFound &)
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "ERROR: get_object_group_version_from_gid raises ObjectGroupNotFound exception ");
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            catch (const CORBA::SystemException& ex)
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "ERROR: System exception raised while getting the group reference version from the replication manager!\n"
                           << ex );
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            }

            if (last_object_group_version == request_object_group_version)
            {
                // The ReplicationManager has not updated the local GroupRepository
                // We should wait this update so that the primary will be updated, if a new
                // new one is elected => throw TRANSIENT exception!
                CDMW_DEBUG(FTLogger::GetLogger(), 
                           "The ReplicationManager has not updated the local GroupRepository. "
                           << "We should wait this update so that the primary will be updated, if a new one is elected => throw TRANSIENT exception!");
                CORBA::TRANSIENT ex(OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            else
            {
                if (last_object_group_version < request_object_group_version)
                {
                    // Request object group doesn't exist in the domain
                    CORBA::INV_OBJREF ex(OrbSupport::INV_OBJREFInvalidObjectGroupRefVersion,
                                         CORBA::COMPLETED_NO);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
                else
                {
                    // last_object_group_version > request_object_group_version
                    // A more recent group reference exist for that group,
                    // advise client with a location forward perm
                    try
                    {
                        CDMW_DEBUG(FTLogger::GetLogger(), 
                                   "A more recent group reference exist for that group, advise client with a location forward perm");
                        CdmwFT::ReplicationManager_var replication_manager
                            = m_group_repository->the_replication_manager();
                        ::FT::ObjectGroup_var last_object_group =
                            replication_manager->get_object_group_ref_from_gid(object_group_id);
                        
                        CDMW_DEBUG(FTLogger::GetLogger(), 
                                   "last_object_group_version > request_object_group_version, "
                                   << "group_version from the replication manager = " << last_object_group_version);

                        // throw a forward request permanent (bool = 1) with the local group reference
                        PortableInterceptor::ForwardRequest ex(last_object_group.in(), 1);
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    }
                    catch (const ::FT::ObjectGroupNotFound &)
                    {
                        CDMW_ERROR(FTLogger::GetLogger(),
                                   "ERROR: get_object_group_version_from_gid raises ObjectGroupNotFound exception ");
                        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    }
                    catch (const CORBA::SystemException& ex)
                    {
                        CDMW_ERROR(FTLogger::GetLogger(),
                                   "ERROR: System exception raised while getting the group reference version from the replication manager!\n"
                                   << ex);
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw;
                    }
                }
            }
        }
    }
}


::FT::FTRequestServiceContext*
ServerRequestInterceptor_impl::get_FTRequestServiceContext(PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::INTERNAL)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    ::FT::FTRequestServiceContext* result = 0;
    
    // Check if this is a FT request
    bool is_a_ft_request = false;

    try
    {
        CORBA::Any_var any = ri->get_slot(m_is_a_ft_request_slot_id);

        if (!(any >>= CORBA::Any::to_boolean(is_a_ft_request)))
        {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "ERROR: unexpected data returned by get_slot" );
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        if (is_a_ft_request)
        {
            // this is not an FT request, get the context
            const char* client_id;
            CORBA::Long retention_id;
            TimeBase::TimeT expiration_time;
            // Get client_id
            any = ri->get_slot(m_client_id_slot_id);
            
            if (!(any.in() >>= client_id))
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "ERROR: unexpected data returned by get_slot" );
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            ::FT::FTRequestServiceContext_var ctxt = new ::FT::FTRequestServiceContext();
            
            ctxt->client_id = client_id;

            // Get retention_id
            any = ri->get_slot(m_retention_id_slot_id);
            
            if (!(any.in() >>= retention_id))
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "ERROR: unexpected data returned by get_slot" );
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            ctxt->retention_id = retention_id;

            // Get expiration time
            any = ri->get_slot(m_expiration_time_slot_id);
            
            if (!(any.in() >>= expiration_time))
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "ERROR: unexpected data returned by get_slot" );
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            ctxt->expiration_time = expiration_time;

            result = ctxt._retn();
        }
    } catch (const PortableInterceptor::InvalidSlot& ) {
        // This slot that has not been allocated
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: InvalidSlot exception raised!" );
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
            
    return result;
}

const char*
ServerRequestInterceptor_impl::get_location() throw ()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    try {
        Cdmw::OsSupport::OS::ProcessId pid = Cdmw::OsSupport::OS::get_processId();
//         static CORBA::String_var My_Location = m_group_repository->the_slocation();
        std::stringstream pid_stream;
        pid_stream << pid;
        static CORBA::String_var My_Location = CORBA::string_dup(pid_stream.str().c_str());
        
        CDMW_LOG_FUNCTION_RETURN(My_Location.in());
        return My_Location.in();
    }
    catch (const CORBA::BAD_INV_ORDER & )
    {
        CDMW_FATAL(FTLogger::GetLogger(), 
                   "GroupRepository object not yet initialised!");
        CDMW_ASSERT(false);
        return 0; 
    }
}



::FT::FTRequestServiceContext*
ServerRequestInterceptor_impl::get_service_context(PortableInterceptor::ServerRequestInfo_ptr ri,
                                                   const char * location_str)
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"location_str="<<location_str);
    ::FT::FTRequestServiceContext* result = 0;

    try
    {
        IOP::ServiceContext_var sc 
            = ri->get_request_service_context(IOP::FT_REQUEST);

        if (sc->context_id == IOP::FT_REQUEST)
        {
            CDMW_DEBUG(FTLogger::GetLogger(), "Context_id: FT_REQUEST");

            CORBA::OctetSeq data(sc->context_data.length());
            data.length(sc->context_data.length());
            for (unsigned int i=0; i < sc->context_data.length(); i++)
            {
                data[i] = sc->context_data[i];
            }
            //memcpy(data.get_buffer(), sc->context_data.get_buffer(),
            //    sc->context_data.length());
            CORBA::Any_var any =
                m_cdrCodec->decode_value(data,
                                         ::FT::_tc_FTRequestServiceContext);

            if (any.in() >>= result)
            {
                result = new ::FT::FTRequestServiceContext(*result);

                CDMW_DEBUG(FTLogger::GetLogger(),
                           "FTRequestServiceContext: "
                           << "\n\t        Client ID: " << result->client_id.in()
                           << "\n\t        Retention ID: " << result->retention_id
                           << "\n\t        Expiration Time: " << result->expiration_time);
            }
        }
    }
    catch(const CORBA::BAD_PARAM&)
    {
        CDMW_DEBUG(FTLogger::GetLogger(),"No service context FT_REQUEST for that request");
        // This is not an error while a request can come to a single reference
        // with no FT service context.

    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: get_request_service_context FT_REQUEST raises exception " << ex);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }

    return result;
}





bool
ServerRequestInterceptor_impl::check_incoming_request(PortableInterceptor::ServerRequestInfo_ptr ri,
                                                       ::FT::FTRequestServiceContext_out   ft_request_context)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    bool result=false;
    const char* loc = get_location();  
    try {     
        ::FT::FTRequestServiceContext_var          ft_request_context__;
        ::FT::TagFTGroupTaggedComponent_var        ft_group_version_context__;
        if (result=get_service_contexts(ri,loc,
                                        ft_request_context__.out(),
                                        ft_group_version_context__.out()))
        {
            // This an FT request. Validate it!
            validate_request(ri,loc,
                             ft_request_context__.in(),
                             ft_group_version_context__.in());
            ft_request_context=ft_request_context__._retn();
            // Request is valid. There is no request logging so no handling 
            // of expiration time & request logging
        } 
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    } catch (const PortableInterceptor::ForwardRequest & ex) {
        CDMW_DEBUG(FTLogger::GetLogger(),
                   "PortableServer::ForwardRequest exception raised in check_incoming_request!\n");
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    } catch (const CORBA::SystemException& ex) {
        CDMW_DEBUG(FTLogger::GetLogger(),
                   "CORBA System exception raised in check_incoming_request!\n" << ex);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }
 }


} // End of namespace FTInit

} // End of namespace FT

} // End of namespace Cdmw
