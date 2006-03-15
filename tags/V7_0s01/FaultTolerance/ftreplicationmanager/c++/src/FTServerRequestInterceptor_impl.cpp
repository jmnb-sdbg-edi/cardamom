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
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>

// Local Files
#include <ftreplicationmanager/FTServerRequestInterceptor_impl.hpp>

#include <idllib/FT.stub.hpp>
#include <idllib/CdmwFTReplicationManager.stub.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>


namespace 
{
    const PortableInterceptor::ORBInitInfo::ObjectId FT_SERVER_REQUEST_INTERCEPTOR_NAME 
        = "FTServerRequestInterceptor";
    const PortableInterceptor::ORBInitInfo::ObjectId FT_REPLICATION_MANAGER_REPOSITORY 
        = "FTReplicationManagerRepository";

} // end of anonymous namespace


namespace Cdmw
{
    
namespace FT
{

namespace ReplicationManager
{



ServerRequestInterceptor_impl::ServerRequestInterceptor_impl(
                 PortableInterceptor::ORBInitInfo_ptr info)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
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
        CORBA::Object_var obj = info->resolve_initial_references(FT_REPLICATION_MANAGER_REPOSITORY);
        m_repository 
            = CdmwFT::ReplicationManagerRepository::_narrow(obj.in());
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

    CDMW_ASSERT(!CORBA::is_nil(m_repository.in()));

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
    //
    // Get FT_GROUP_VERSION request service context 
    //
    try
    {
        IOP::ServiceContext_var sc 
            = ri->get_request_service_context(IOP::FT_GROUP_VERSION);

        if (sc->context_id == IOP::FT_GROUP_VERSION)
        {
            CDMW_DEBUG(FTLogger::GetLogger(),
                       "context_id: FT_REQUEST");

            CORBA::OctetSeq data(sc->context_data.length());
            data.length(sc->context_data.length());
            for (unsigned int i=0; i < sc->context_data.length(); i++)
            {
                data[i] = sc->context_data[i];
            } 
           
            CORBA::Any_var any = m_cdrCodec->decode_value(data, ::FT::_tc_TagFTGroupTaggedComponent);
            
            const ::FT::TagFTGroupTaggedComponent* context;
                        
            if (any.in() >>= context)
            {
                CDMW_DEBUG(FTLogger::GetLogger(),
                           "FTGroupVersionServiceContext: "
                           << "\n\t        object_group_id = " << context->object_group_id
                           << "\n\t        object_group_ref_version = " << context->object_group_ref_version);     

                CdmwFT::ReplicationManager_var repl_mgr = m_repository->get_replication_manager();

                if (CORBA::is_nil(repl_mgr.in()))
                {
                    CDMW_ERROR(FTLogger::GetLogger(),
                               "ERROR: Error occured when getting ReplicationManager reference");
                    CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }

                ::FT::ObjectGroupRefVersion current_version =
                    repl_mgr->get_object_group_version_from_gid(context->object_group_id);
                
                if (context->object_group_ref_version == current_version) 
                {
                    CORBA::TRANSIENT ex(::Cdmw::OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                } 
                else 
                {
                    // Get the most recent object group reference.
                    ::FT::ObjectGroup_var object_group =
                        repl_mgr->get_object_group_ref_from_gid(context->object_group_id);
                    
                    // Trigger LOCATION_FORWARD_PERM
                    PortableInterceptor::ForwardRequest ex(object_group.in(),1); 
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                    // TODO:
                    //  May we check that the primary is ok and do a reconfiguration before
                    //  replying with an up to date object group reference?
                }
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
        throw;
    }
    catch (...)
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: get_request_service_context raises unknown exception ");
    }
}

void
ServerRequestInterceptor_impl::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}


void
ServerRequestInterceptor_impl::send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

void
ServerRequestInterceptor_impl::send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

void
ServerRequestInterceptor_impl::send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}


} // End of namespace ReplicationManager

} // End of namespace FT

} // End of namespace Cdmw



