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
#include <FaultTolerance/ftcommon/MacroDef.hpp>
// Local Files
#include <ftreplicationmanager/FTServerRequestInterceptorFTManager_impl.hpp>

#include <idllib/FT.stub.hpp>

#include <FaultTolerance/ftcommon/FTLogger.hpp>

#include <Foundation/logging/FunctionLogger.hpp>


namespace 
{
    const PortableInterceptor::ORBInitInfo::ObjectId FT_SERVER_REQUEST_INTERCEPTOR_NAME 
        = "FTServerRequestInterceptor";
    const PortableInterceptor::ORBInitInfo::ObjectId FT_REPLICATION_MANAGER 
        = "ReplicationManager";
    const PortableInterceptor::ORBInitInfo::ObjectId FT_GROUP_REPOSITORY 
        = "FTGroupRepository";
    const PortableInterceptor::ORBInitInfo::ObjectId FT_REPLICATION_MANAGER_REPOSITORY 
        = "FTReplicationManagerRepository";

    const char* REP_ID_REPL_MNGT = "IDL:thalesgroup.com/CdmwFT/ReplicationManagerPushConsumer:1.0";
    const char* REP_ID_FAULT_NOTIFIER = "IDL:omg.org/FT/FaultNotifier:1.0"; 
    const char* REP_ID_FAULT_DETECTOR = "IDL:thalesgroup.com/CdmwFT/FaultManagement/FaultDetector:1.0";

} // end of anonymous namespace


namespace Cdmw
{
    
namespace FT
{

namespace ReplicationManager
{



ServerRequestInterceptorFTManager_impl::ServerRequestInterceptorFTManager_impl(
                 PortableInterceptor::ORBInitInfo_ptr info,
                 const PortableInterceptor::SlotId  ft_domain_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_id_slot_id,
                 const PortableInterceptor::SlotId  object_group_ref_version_slot_id,
                 const PortableInterceptor::SlotId  client_id_slot_id,
                 const PortableInterceptor::SlotId  retention_id_slot_id,
                 const PortableInterceptor::SlotId  expiration_time_slot_id,
                 const PortableInterceptor::SlotId  is_a_ft_request_slot_id)
    : m_ft_domain_id_slot_id(ft_domain_id_slot_id),
      m_object_group_id_slot_id(object_group_id_slot_id),
      m_object_group_ref_version_slot_id(object_group_ref_version_slot_id),
      m_client_id_slot_id(client_id_slot_id),
      m_retention_id_slot_id(retention_id_slot_id),
      m_expiration_time_slot_id(expiration_time_slot_id),
      m_is_a_ft_request_slot_id(is_a_ft_request_slot_id)
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
ServerRequestInterceptorFTManager_impl::name() 
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    CDMW_LOG_FUNCTION_RETURN(FT_SERVER_REQUEST_INTERCEPTOR_NAME);
    return CORBA::string_dup(FT_SERVER_REQUEST_INTERCEPTOR_NAME);
}

void
ServerRequestInterceptorFTManager_impl::destroy() 
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

void
ServerRequestInterceptorFTManager_impl::receive_request_service_contexts(
        PortableInterceptor::ServerRequestInfo_ptr ri)
    throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");

    std::string location_str;
    try
    {
        ::FT::Location* my_location = m_group_repository->the_location();
        location_str = 
            Cdmw::CommonSvcs::Naming::NamingInterface::to_string(*my_location);
        //DEBUG_DUMP(location_str, " receive_request_service_contexts entering...");
    }
    catch (const CORBA::SystemException& ex)
    {
        // nothing to do, maybe group_repository is not already initialised, or the env is not FT.   
    }

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
            CDMW_DEBUG(FTLogger::GetLogger(),
                       "Context_id: FT_REQUEST");
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
            CDMW_ERROR(FTLogger::GetLogger(),
                       "Bad context id for IOP::FT_REQUEST");
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
    catch(const CORBA::BAD_PARAM& ex)
    {
        //DEBUG_DUMP(location_str, "No service context FT_REQUEST for that request"<<ex<<ex.minor());
        // This is not an error while a request can come to a single reference
        // with no FT service context.

    }
    catch (const CORBA::SystemException& ex)
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: get_request_service_context FT_REQUEST raised exception " << ex);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }


    //
    // Get FT_GROUP_VERSION request service context 
    //
    bool contain_a_ft_group_version_sc = false;
    ::FT::ObjectGroupId object_group_id = 0;
    ::FT::ObjectGroupRefVersion object_group_ref_version = 0;
    
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
                
                object_group_id = context->object_group_id;
                object_group_ref_version = context->object_group_ref_version;
                
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
        throw;
    }
    catch (...)
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: get_request_service_context raises unknown exception ");
    }

    if ((contain_a_ft_group_version_sc) != (contain_a_ft_request_sc))
    {
        // a service context is missing, throw exception
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: a service context is missing! ");
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
            bool is_a_ft_request = true;
            
            slotData <<= CORBA::Any::from_boolean(is_a_ft_request);
            ri->set_slot(m_is_a_ft_request_slot_id, slotData);
        }
        else
        {
            // set the slot id to indicate that this is not a ft_request!
            CORBA::Any slotData;
            bool is_a_ft_request = false;
            
            slotData <<= CORBA::Any::from_boolean(is_a_ft_request);
            ri->set_slot(m_is_a_ft_request_slot_id, slotData);
        }
    }
    bool is_a_ft_request;

    CORBA::Any_var any = ri->get_slot(m_is_a_ft_request_slot_id);
    
    if (!(any >>= CORBA::Any::to_boolean(is_a_ft_request)))
    {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "ERROR: unexpected data returned by get_slot");
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }


    if (!is_a_ft_request || ((object_group_id <= 0xffffff0f) &&(object_group_id > 0xffffff00) ))
    {
    }
    else
    {
        ::FT::ObjectGroupRefVersion last_object_group_version;
        CdmwFT::ReplicationManager_var replication_manager;
        try
        {
            CDMW_DEBUG(FTLogger::GetLogger(), 
                       "Fallback called! Check good version with the replication manager");

            replication_manager = m_repository->get_replication_manager();

            if (CORBA::is_nil(replication_manager.in()))
            {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "ERROR: Error occured when getting ReplicationManager reference");
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }

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
        if ( object_group_ref_version == last_object_group_version) 
        {
            // Check whether it's an is_a() operation. 
            //    is_a() shall still be serviced by DSI to enable narrow() or is_a()
            //    operations on an empty group to succeed.
            const char* opname=ri->operation();
            if (strcmp(opname,"_is_a") == 0) {
                CDMW_DEBUG(FTLogger::GetLogger(), " is_a() operation on empty group");
            } else {
                // Use standard fail-over side semantics if not a call to is_a().
                CDMW_DEBUG(FTLogger::GetLogger(), "Transient");
                CORBA::TRANSIENT ex(::Cdmw::OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
        } 
        else 
        {
            // Get the most recent object group reference.
            ::FT::ObjectGroup_var object_group =
                replication_manager->get_object_group_ref_from_gid(object_group_id);
            
            // Trigger LOCATION_FORWARD_PERM
                CDMW_DEBUG(FTLogger::GetLogger(), "Location forward");
                PortableInterceptor::ForwardRequest ex(object_group.in(),1); 
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            // TODO:
            //  May we check that the primary is ok and do a reconfiguration before
            //  replying with an up to date object group reference?
        }
 
    }
}

void
ServerRequestInterceptorFTManager_impl::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    std::string location_str;
    
    // Check if this is a FT request
    bool is_a_ft_request;
    ::FT::ObjectGroupRefVersion object_group_version = 0;
    ::FT::ObjectGroupRefVersion request_object_group_version = 0;
    ::FT::ObjectGroupId object_group_id;

    try
    {
        CORBA::Any_var any = ri->get_slot(m_is_a_ft_request_slot_id);

        if (!(any >>= CORBA::Any::to_boolean(is_a_ft_request)))
        {
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ERROR: unexpected data returned by get_slot");
            CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        if (!is_a_ft_request)
        {
            // this is not a FT request, do nothing!
            CDMW_LOG_FUNCTION_RETURN("");
            return;
        }
        else
        {
            ::FT::Location* my_location = m_group_repository->the_location();
            location_str = 
                Cdmw::CommonSvcs::Naming::NamingInterface::to_string(*my_location);
            CDMW_DEBUG(FTLogger::GetLogger(), "Receive_request entering... ");

            // get group_id
            any = ri->get_slot(m_object_group_id_slot_id);
            
            if (!(any.in() >>= object_group_id))
            {
                CDMW_ERROR(FTLogger::GetLogger(), 
                           "ERROR: unexpected data returned by get_slot");
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            
            if (ri->target_is_a(REP_ID_REPL_MNGT) 
                || ri->target_is_a(REP_ID_FAULT_NOTIFIER) 
                || ri->target_is_a(REP_ID_FAULT_DETECTOR))
            {                
                CDMW_DEBUG(FTLogger::GetLogger(), 
                           "Repository_id  = " << ri->target_most_derived_interface()); 
                // get group_version from the GroupRepository
                object_group_version =
                    m_group_repository->get_object_group_version_from_gid(object_group_id);
            } 
            else
            {
               // never here
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;

            }
            CDMW_DEBUG(FTLogger::GetLogger(), 
                       "group_version from the GroupRepository = " << object_group_version);                
    
                // get request group_version
                any = ri->get_slot(m_object_group_ref_version_slot_id);
                
            if (!(any.in() >>= request_object_group_version))
            {
                CDMW_ERROR(FTLogger::GetLogger(), 
                           "ERROR: unexpected data returned by get_slot");
                CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            CDMW_DEBUG(FTLogger::GetLogger(), 
                       "request_object_group_version = " << request_object_group_version);
        }
    }
    catch (const PortableInterceptor::InvalidSlot& )
    {
        CDMW_ERROR(FTLogger::GetLogger(), 
                   "ERROR: get_slot raises InvalidSlot exception ");
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    catch (const ::FT::ObjectGroupNotFound& )
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
                   "ERROR: System exception raised while checking the group version!\n" << ex);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    }

    if (ri->target_is_a(REP_ID_REPL_MNGT) 
        || ri->target_is_a(REP_ID_FAULT_NOTIFIER)
        || ri->target_is_a(REP_ID_FAULT_DETECTOR))
    {                
        
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
                // all is ok, let the request going up!
                CDMW_DEBUG(FTLogger::GetLogger(), 
                           "request is on the primary, let the request going up!");
                CDMW_LOG_FUNCTION_RETURN("");
                return;
            }
            else
            {
                // normally, a transient exception must be throw if the version are equals
                // but the object group reference haven't fallback reference, so we realise a location forward of
                // the reference. 
                // throw transient!
                //DEBUG_DUMP(location_str, "not the primary, throw transient");
                //throw CORBA::TRANSIENT(OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);


                ::FT::ObjectGroup_var object_group =
                  m_group_repository->get_object_group_from_gid(object_group_id);
                
                // throw a forward request permanent (bool = 1) with the local group reference
                CDMW_DEBUG(FTLogger::GetLogger(), 
                           "throw a location forward permanent");
                PortableInterceptor::ForwardRequest ex(object_group.in(), 1);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
        }
        // the version of the request is different of the version in the group repository
        else
        {
            if (request_object_group_version < object_group_version)
            {
                // request object group reference is obsolete, 
                // get the local group reference 
                try
                {
                    CDMW_DEBUG(FTLogger::GetLogger(), 
                               "request object group reference is obsolete, get the local group reference and throw a location forward");
                    ::FT::ObjectGroup_var object_group =
                        m_group_repository->get_object_group_from_gid(object_group_id);
                    
                    // throw a forward request permanent (bool = 1) with the local group reference
                    CDMW_DEBUG(FTLogger::GetLogger(), 
                               "throw a location forward permanent");
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
                // Request object group doesn't exist in the domain
                CORBA::INV_OBJREF ex(OrbSupport::INV_OBJREFInvalidObjectGroupRefVersion,
                                        CORBA::COMPLETED_NO);  
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;              
            }
        }
    } 
    else
    {

        // never here
        CORBA::INTERNAL ex(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;              
                
    }    
}
    

void
ServerRequestInterceptorFTManager_impl::send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException)
{
    // nothing to do!
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

void
ServerRequestInterceptorFTManager_impl::send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    // nothing to do!
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

void
ServerRequestInterceptorFTManager_impl::send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
    // nothing to do!
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}


} // End of namespace ReplicationManager

} // End of namespace FT

} // End of namespace Cdmw


