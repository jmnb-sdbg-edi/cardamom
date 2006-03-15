/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
 * 
 * This file and the information it contains are confidential and proprietary. 
 * They shall not be reproduced nor disclosed to any person except to those 
 * having a need to know them without prior written consent of the owner.
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
       //DO_TRACE_FUNCTION();
    }

    virtual void
    receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
       //DO_TRACE_FUNCTION();
    }

    virtual void
    send_reply(PortableInterceptor::ServerRequestInfo_ptr ri)
        throw (CORBA::SystemException)
    {
       //DO_TRACE_FUNCTION();

       std::string operation_name = ri->operation();
       std::string host_name = Cdmw::OsSupport::OS::get_hostname();

       if ((host_name == m_host2) &&
           (operation_name == "do_something_more"))
       {
          OsSupport::OS::sleep(1000);
          CDMW_ASSERT(false);
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

    PortableInterceptor::ORBInitInfo_var m_info;
    IOP::Codec_var m_cdrCodec;
    const std::string& m_host1;
    const std::string& m_host2;
    const std::string& m_host3;


};

}; // End of namespace Cdmw

#endif // INCL_CDMW_TEST_SERVER_REQUEST_INTERCEPTOR_IMPL_HPP
