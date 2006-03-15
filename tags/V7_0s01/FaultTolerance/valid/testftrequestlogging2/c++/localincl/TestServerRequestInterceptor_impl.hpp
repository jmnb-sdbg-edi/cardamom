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
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>


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
       //DO_TRACE_FUNCTION();
    }

    virtual void
    receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
       DO_TRACE_FUNCTION();
       std::string operation_name = ri->operation();
       ECHO_INFO("operation_name=" << operation_name);
    }

    virtual void
    send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException)
    {
       DO_TRACE_FUNCTION();

       std::string operation_name = ri->operation();
       std::string host_name = Cdmw::OsSupport::OS::get_hostname();

       ECHO_INFO("operation_name=" << operation_name);
       try
       {
           if (operation_name == "do_something")
           {
               ::FT::FTRequestServiceContext_var  ft_request_context;
               const char* loc = get_location();
               
               bool result=get_service_contexts(ri,
                                                loc,
                                                ft_request_context.out());

               CDMW_ASSERT(result);

               ECHO_INFO("find logger");
               // find object group
               CdmwNamingAndRepository::Repository_var repository
                   = Cdmw::NamingAndRepository::RepositoryInterface::get_repository();
           
               Cdmw::CommonSvcs::Naming::NamingInterface ni =
                      Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface ("dom1/dom2");
           
               std::string logger_full_name = "dom3/HelloLogger";
               CORBA::Object_var obj;
               
               try
               {
                   obj = ni.resolve (logger_full_name);
               }
               catch(const CosNaming::NamingContext::NotFound& e)
               {
                   std::cerr<< e << std::endl;
               }
               catch(const CosNaming::NamingContext::CannotProceed& e)
               {
                   std::cerr<< e << std::endl;
               }
               catch(const CosNaming::NamingContext::InvalidName& e)
               {
                   std::cerr<< e << std::endl;
               }
               catch (const CORBA::SystemException& e)
               {
                   std::cerr<< e << std::endl;
               }
           
               CdmwRequestLogging::HelloLogger_var hello_logger_ref =
                       CdmwRequestLogging::HelloLogger::_narrow(obj.in());
    
               std::string trace = host_name 
                  + "TestServerRequestInterceptor => send_reply with client_id/retention_id=" 
                  + ft_request_context->client_id.in()
                  + "/";
               std::ostringstream os;
               os << host_name
                  << " TestServerRequestInterceptor => send_reply "
                  << "with client_id/retention_id="
                  << ft_request_context->client_id.in()
                  << "/"
                  << ft_request_context->retention_id
                  << std::ends;
               hello_logger_ref->set_log_trace(os.str().c_str());
           }
       }
       catch (const CORBA::SystemException& e)
       {
           std::cerr<< e << std::endl;
       }
       catch (const CORBA::Exception& e)
       {
           std::cerr<< e << std::endl;
       }
    }

    virtual void
    send_other(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
       //DO_TRACE_FUNCTION();
    }

    virtual void
    send_exception(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
       //DO_TRACE_FUNCTION();
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
       //DO_TRACE_FUNCTION();
       
    }
#endif // TAO_HAS_EXTENDED_FT_INTERCEPTORS


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

    bool
    get_service_contexts(PortableInterceptor::ServerRequestInfo_ptr ri,
                         const char *                        location_str,
                         ::FT::FTRequestServiceContext_out   ft_request_context)
        throw (CORBA::SystemException)
    {
        //DO_TRACE_FUNCTION();
        
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
                CORBA::Any_var any = 
                   m_cdrCodec->decode_value(data, 
                                            ::FT::_tc_FTRequestServiceContext);
                
                const ::FT::FTRequestServiceContext* context;
                if (any.in() >>= context)
                {
                    ft_request_context = 
                       new ::FT::FTRequestServiceContext(*context);
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

        return contain_a_ft_request_sc;
    }

    PortableInterceptor::ORBInitInfo_var m_info;
    IOP::Codec_var m_cdrCodec;
    const std::string& m_host1;
    const std::string& m_host2;
    const std::string& m_host3;


};


}; // End of namespace Cdmw

#endif // INCL_CDMW_TEST_SERVER_REQUEST_INTERCEPTOR_IMPL_HPP
