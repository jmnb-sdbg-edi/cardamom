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


#ifndef INCL_CDMW_TEST_SERVER_REQUEST_INTERCEPTOR_IMPL_HPP
#define INCL_CDMW_TEST_SERVER_REQUEST_INTERCEPTOR_IMPL_HPP

#include <sstream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/orbsupport/PortableInterceptor.hpp>
#include <Foundation/common/Assert.hpp>
#include <Foundation/ossupport/OS.hpp>

#include <FaultTolerance/idllib/CdmwFTGroupRepository.stub.hpp>
#include <FaultTolerance/ftlocationmanager/PrimaryBackupGroupRepository_impl.hpp>

#include "ftcommon/ReplyRecording.hpp"
#include "ftrequestlogging/FTReplyRecording.hpp"


#if TAO_HAS_EXTENDED_FT_INTERCEPTORS == 1
#include "tao/Any_Impl.h"
#include "tao/DynamicC.h"
#endif /*TAO_HAS_EXTENDED_FT_INTERCEPTORS*/

#define ECHO_HEADER() \
    "[CDMW TEST FTRequestLogging] "


#   define DEBUG_DUMP(location, comment) \
do {\
    std::cout << ECHO_HEADER() << location << " --> " << comment << std::endl;\
} while(0)

#define ECHO_INFO(comment) \
do {\
    std::cout << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)


#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)


namespace
{
  const PortableInterceptor::ORBInitInfo::ObjectId FT_GROUP_REPOSITORY 
        = "FTGroupRepository";

  class FunctionTracer
  {
  public:
    FunctionTracer(const char* location, const char * file, const char* function, int line)
      : m_location(location), m_file(file), m_function(function), m_line(line)
    {
      std::cout << ECHO_HEADER() << m_location << " --> " 
                << "ENTERING " << m_function << "() [" << m_line << "] ..." << std::endl;
    }
    ~FunctionTracer()
    {
      std::cout<< ECHO_HEADER() << m_location << " --> " 
               << "EXITING " << m_function << "() ..." << std::endl;
    }
  private:
    FunctionTracer();

    const char*    m_location;
    const char*    m_file;
    const char*    m_function;
    int            m_line;
  };
};// end anonymous namespace

#   define DO_TRACE_FUNCTION()  FunctionTracer fct##__LINE__(this->get_location(),__FILE__,__FUNCTION__,__LINE__)


namespace Cdmw
{

class TestServerRequestInterceptor_impl : 
   public PortableInterceptor::ServerRequestInterceptor,
   public Cdmw::OrbSupport::RefCountLocalObject
{
public:

    TestServerRequestInterceptor_impl(PortableInterceptor::ORBInitInfo_ptr info,
                                      const std::string& host1,
                                      const std::string& host2,
                                      const std::string& host3)
       : m_info(PortableInterceptor::ORBInitInfo::_duplicate(info)),
         m_host1(host1),
         m_host2(host2),
         m_host3(host3)
    {
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
    }

    //
    // IDL to C++ Mapping
    //
    virtual char*
    name() throw (CORBA::SystemException)
    {
       return CORBA::string_dup("TestServerRequestInterceptor");
    }

    virtual void
    destroy() throw (CORBA::SystemException)
    {
    }

    virtual void
    receive_request_service_contexts(
        PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
       DO_TRACE_FUNCTION();
    }

    virtual void
    receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
       DO_TRACE_FUNCTION();
    }

    virtual void
    send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException)
    {
       DO_TRACE_FUNCTION();

       std::string operation_name = ri->operation();
       std::string host_name = Cdmw::OsSupport::OS::get_hostname();

       if ((host_name == m_host2) &&
           (operation_name == "send_bad_context_exception"))
       {
          CDMW_ASSERT(false);
       }

    }

    virtual void
    send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
       DO_TRACE_FUNCTION();
    }

    virtual void
    send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
       DO_TRACE_FUNCTION();
    }

#if TAO_HAS_EXTENDED_FT_INTERCEPTORS == 1
    /**
     * This is a TAO-specific operation to enable returning previous logged replies.
     * NOTA: Only the primary actually returns a previous reply!
     */  
    virtual void
    tao_ft_interception_point(PortableInterceptor::ServerRequestInfo_ptr ri,
                              CORBA::OctetSeq_out ocs)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
       DO_TRACE_FUNCTION();
       
       std::string operation_name = ri->operation();
       std::string host_name = Cdmw::OsSupport::OS::get_hostname();

       if ((host_name != m_host2) &&
           (operation_name == "send_bad_context_exception"))
       {
          try 
          {
              ::FT::FTRequestServiceContext_var          ft_request_context;
              check_incoming_request(ri,ft_request_context.out()); 
              ECHO_INFO("wait for record deletion!");
              Cdmw::CommonSvcs::DataStore::DataStoreBase* ds
                 = Cdmw::CommonSvcs::DataStore::DataStoreRegistry::Get_data_store(
                       Cdmw::FT::FT_REPLY_DATASTORE_ID);
              Cdmw::FT::RequestLogging::ReplyRecording::ReplyStore* logging = 
                 dynamic_cast<Cdmw::FT::RequestLogging::ReplyRecording::ReplyStore*>(
                       ds);
              
              try
              {
                  std::ostringstream id_stream;
                  id_stream << ft_request_context->client_id.in() 
                            << "/" 
                            << ft_request_context->retention_id;

                  OsSupport::OS::sleep(10000);
                  ECHO_INFO("if reply is recorded, remove it" << id_stream.str());
                  logging->remove(id_stream.str());
              }
              catch (const Cdmw::Common::NotFoundException&)
              {
                  ECHO_INFO("reply is no more recorded, let the request continu");
              }
          }
          catch (...) 
          {
             // do nothing let FT request interceptor do its job.
          }
       }
    }
#endif // TAO_HAS_EXTENDED_FT_INTERCEPTORS

    bool 
    check_incoming_request(PortableInterceptor::ServerRequestInfo_ptr ri,
                           ::FT::FTRequestServiceContext_out   ft_request_context)
       throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
        DO_TRACE_FUNCTION();
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
            return result;
        } catch (const PortableInterceptor::ForwardRequest &) {
            ECHO_INFO("PortableServer::ForwardRequest exception raised in check_incoming_request!\n");
            throw;
        } catch (const CORBA::SystemException& ex) {
            ECHO_INFO("CORBA System exception raised in check_incoming_request!\n" << ex);
            throw;
        }
    }

    bool
    get_service_contexts(PortableInterceptor::ServerRequestInfo_ptr ri,
                         const char *                        location_str,
                         ::FT::FTRequestServiceContext_out   ft_request_context,
                         ::FT::TagFTGroupTaggedComponent_out ft_group_version_context)
        throw (CORBA::SystemException)
    {
        DO_TRACE_FUNCTION();
        
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
                DEBUG_DUMP(location_str, " context_id: FT_REQUEST");
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
                    DEBUG_DUMP(location_str, "  FTRequestServiceContext: ");
                    ECHO_INFO("        Client ID: " << context->client_id.in());
                    ECHO_INFO("        Retention ID: " << context->retention_id);
                    ECHO_INFO("        Expiration Time: " << context->expiration_time);
    
                    ft_request_context = new ::FT::FTRequestServiceContext(*context);
                }
                else
                {
                    ECHO_ERROR("Error occured when reading FTRequestServiceContext");
                    throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                }
            }
            else
            {
                ECHO_ERROR("Bad context id for IOP::FT_REQUEST");
                throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            }
        }
        catch(const CORBA::BAD_PARAM&)
        {
            DEBUG_DUMP(location_str, "No service context FT_REQUEST for that request");
            // This is not an error while a request can come to a single reference
            // with no FT service context.
    
        }
        catch (const CORBA::SystemException& ex)
        {
            ECHO_ERROR("ERROR: get_request_service_context FT_REQUEST raises exception " << ex);
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
                DEBUG_DUMP(location_str, "context_id: FT_GROUP_VERSION");
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
                //    ECHO_INFO("     Object group ref version: " << context->object_group_ref_version);
                //
                
                CORBA::Any_var any = m_cdrCodec->decode_value(data, ::FT::_tc_TagFTGroupTaggedComponent);
                
                const ::FT::TagFTGroupTaggedComponent* context;
                            
                if (any.in() >>= context)
                {
                    DEBUG_DUMP(location_str, "FTGroupVersionServiceContext: ");
                    ECHO_INFO("        version = " 
                              << (int)context->component_version.major << "." 
                              << (int)context->component_version.minor);
                    ECHO_INFO("        ft_domain_id = " << context->group_domain_id.in());
                    ECHO_INFO("        object_group_id = " << context->object_group_id);
                    ECHO_INFO("        object_group_ref_version = " << context->object_group_ref_version);               
    
                    ft_group_version_context = new ::FT::TagFTGroupTaggedComponent(*context);
                }
                else
                {
                    ECHO_ERROR("ERROR: Error occured when reading FTGroupVersionServiceContext");
                    throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
                }   
            }
            else
            {
                ECHO_ERROR("ERROR: Bad context id for IOP::FT_GROUP_VERSION");
                throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
            }
        }
        catch(const CORBA::BAD_PARAM&)
        {
            DEBUG_DUMP(location_str, "No service context FT_GROUP_VERSION for that request");
    
            // This is not an error while a request can come to a single reference.
            // with no FT service context.
        }
        catch (const CORBA::SystemException& ex)
        {
            ECHO_ERROR("ERROR: get_request_service_context FT_GROUP_VERSION raises exception " << ex );
            throw;
        }
        catch (const CORBA::Exception& ex)
        {
            ECHO_ERROR("ERROR: get_request_service_context FT_GROUP_VERSION raises CORBA exception " << ex);
            throw; // FIXME: This may cause an unexpected exception if not a PortableInterceptor::ForwardRequest exception (may abort)
        }
        catch (...)
        {
            ECHO_ERROR("ERROR: get_request_service_context raises unknown exception ");
            throw; // FIXME: This will cause an unexpected exception (may abort)
        }
    
        if ((contain_a_ft_group_version_sc) != (contain_a_ft_request_sc))
        {
            // a service context is missing, throw exception
            ECHO_ERROR("ERROR: a service context is missing! ");
            throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, CORBA::COMPLETED_NO);
        }
        return contain_a_ft_group_version_sc;
    }


    void 
    validate_request(PortableInterceptor::ServerRequestInfo_ptr ri,
                     const char *                            location_str,
                     const ::FT::FTRequestServiceContext &   ft_request_context,
                     const ::FT::TagFTGroupTaggedComponent & ft_group_version_context)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
        DO_TRACE_FUNCTION();
        ::FT::ObjectGroupRefVersion object_group_version = 0;
        ::FT::ObjectGroupRefVersion request_object_group_version = 
           ft_group_version_context.object_group_ref_version;
        ::FT::ObjectGroupId object_group_id = ft_group_version_context.object_group_id;
        ::CdmwFT::Location::PrimaryBackupGroupRepository_var group_repository;
    
        try
        {
            group_repository 
                = ::CdmwFT::Location::PrimaryBackupGroupRepository::_duplicate(
                      ::Cdmw::FT::Location::PrimaryBackupGroupRepository_impl::Get_instance());
        }
        catch (const PortableInterceptor::ORBInitInfo::InvalidName& )
        {
            ECHO_ERROR("Impossible to resolve initial reference: InvalidName raised!");
            CDMW_ASSERT(false);
        }
        catch (const CORBA::SystemException& ex)
        {
            ECHO_ERROR("Impossible to resolve initial reference: CORBA::SystemException raised!\n" 
                       << ex );
            CDMW_ASSERT(false);
        }
    
        CDMW_ASSERT(!CORBA::is_nil(group_repository.in()));

        try
        {
            // get group_version from the GroupRepository
            object_group_version =
                group_repository->get_object_group_version_from_gid(object_group_id);
            DEBUG_DUMP(location_str, "group_version from the GroupRepository = " 
                  << object_group_version);
            
            ECHO_INFO("request_object_group_version = " 
                  << request_object_group_version);
        }
        catch (const ::FT::ObjectGroupNotFound& )
        {
            DEBUG_DUMP(location_str, "ObjectGoup not known yet, throw transient");
            throw CORBA::TRANSIENT(OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
        }
        catch (const CORBA::SystemException& ex)
        {
            ECHO_ERROR("ERROR: System exception raised while checking the group version!\n" 
                  << ex);
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
                is_primary = group_repository->is_primary_from_gid(object_group_id);
            }
            catch (const ::FT::ObjectGroupNotFound &)
            {
                ECHO_ERROR("ERROR: is_primary_from_gid raises ObjectGroupNotFound exception ");
                throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, 
                                      CORBA::COMPLETED_NO);
            }
            catch (const CORBA::SystemException& ex)
            {
                ECHO_ERROR("ERROR: System exception raised while checking the primary member!\n"
                           << ex );
                throw;
            }
            
            if (is_primary)
            {
                // all is ok, let the request go up!
                DEBUG_DUMP(location_str, "request is on the primary, let the request go up!");
                return;
            }
            else
            {
                // throw transient!
                DEBUG_DUMP(location_str, "not the primary, throw transient");
                throw CORBA::TRANSIENT(OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
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
                    DEBUG_DUMP(location_str, 
                               "request object group reference is obsolete, get the local group reference and throw a location forward");
                    ::FT::ObjectGroup_var object_group =
                        group_repository->get_object_group_from_gid(object_group_id);
        
                    // throw a forward request permanent (bool = 1) with the local group reference
                    DEBUG_DUMP(location_str, "throw a location forward permanent");
                    throw PortableInterceptor::ForwardRequest(object_group.in(), 1);
                }
                catch (const ::FT::ObjectGroupNotFound &)
                {
                    ECHO_ERROR("ERROR: get_object_group_from_gid raises ObjectGroupNotFound exception ");
                    throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, 
                                          CORBA::COMPLETED_NO);
                }
                catch (const CORBA::SystemException& ex)
                {
                    ECHO_ERROR("ERROR: System exception raised while getting the group reference!\n"
                               << ex);
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
                    DEBUG_DUMP(location_str, 
                               "local object group is obsolete, check with the replication manager");
                    CdmwFT::ReplicationManager_var replication_manager
                        = group_repository->the_replication_manager();
                    last_object_group_version
                        = replication_manager->get_object_group_version_from_gid(
                              object_group_id);
                }
                catch (const ::FT::ObjectGroupNotFound &)
                {
                    ECHO_ERROR("ERROR: get_object_group_version_from_gid raises ObjectGroupNotFound exception ");
                    throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, 
                                          CORBA::COMPLETED_NO);
                }
                catch (const CORBA::SystemException& ex)
                {
                    ECHO_ERROR("ERROR: System exception raised while getting the group reference version from the replication manager!\n"
                               << ex );
                    throw;
                }
    
                if (last_object_group_version == request_object_group_version)
                {
                    // The ReplicationManager has not updated the local GroupRepository
                    // We should wait this update so that the primary will be updated, 
                    // if a new new one is elected => throw TRANSIENT exception!
                    DEBUG_DUMP(location_str, 
                               "The ReplicationManager has not updated the local GroupRepository");
                    ECHO_INFO("      We should wait this update so that the primary will be updated, if a new one is elected => throw TRANSIENT exception!");
                    throw CORBA::TRANSIENT(OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
                }
                else
                {
                    if (last_object_group_version < request_object_group_version)
                    {
                        // Request object group doesn't exist in the domain
                        throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREFInvalidObjectGroupRefVersion,
                                                CORBA::COMPLETED_NO);
                    }
                    else
                    {
                        // last_object_group_version > request_object_group_version
                        // A more recent group reference exist for that group,
                        // advise client with a location forward perm
                        try
                        {
                            DEBUG_DUMP(location_str, 
                                       "A more recent group reference exist for that group, advise client with a location forward perm");
                            CdmwFT::ReplicationManager_var replication_manager
                                = group_repository->the_replication_manager();
                            ::FT::ObjectGroup_var last_object_group =
                                replication_manager->get_object_group_ref_from_gid(object_group_id);
                            ECHO_INFO("last_object_group_version > request_object_group_version ");
                            ECHO_INFO("group_version from the replication manager = " 
                                       << last_object_group_version);
    
                            // throw a forward request permanent (bool = 1) with the 
                            // local group reference
                            throw PortableInterceptor::ForwardRequest(last_object_group.in(), 1);
                        }
                        catch (const ::FT::ObjectGroupNotFound &)
                        {
                            ECHO_ERROR("ERROR: get_object_group_version_from_gid raises ObjectGroupNotFound exception ");
                            throw CORBA::INTERNAL(OrbSupport::INTERNALFaultToleranceError, 
                                                  CORBA::COMPLETED_NO);
                        }
                        catch (const CORBA::SystemException& ex)
                        {
                            ECHO_ERROR("ERROR: System exception raised while getting the group reference version from the replication manager!\n"
                                       << ex);
                            throw;
                        }
                    }
                }
            }
        }
    }


    private:
    TestServerRequestInterceptor_impl(const TestServerRequestInterceptor_impl&);
    TestServerRequestInterceptor_impl& operator=(const TestServerRequestInterceptor_impl&);

    const char*
    TestServerRequestInterceptor_impl::get_location() throw ()
    {
        try {
            Cdmw::OsSupport::OS::ProcessId pid = Cdmw::OsSupport::OS::get_processId();
            std::stringstream pid_stream;
            pid_stream << pid;
            static CORBA::String_var My_Location = 
               CORBA::string_dup(pid_stream.str().c_str());
            
            return My_Location.in();
        }
        catch (const CORBA::BAD_INV_ORDER & )
        {
            std::cerr << ECHO_HEADER()
                      << "GroupRepository object not yet initialised!"
                      << std::endl;
            CDMW_ASSERT(false);
            return 0; 
        }
    }

    PortableInterceptor::ORBInitInfo_var m_info;
    IOP::Codec_var m_cdrCodec;
    const std::string& m_host1;
    const std::string& m_host2;
    const std::string& m_host3;


};

}; // End of namespace Cdmw

#endif // INCL_CDMW_TEST_SERVER_REQUEST_INTERCEPTOR_IMPL_HPP
