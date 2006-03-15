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


#ifndef INCL_NAMINGANDREPOSITORY_PROXY_BEHAVIOUR_HPP 
#define INCL_NAMINGANDREPOSITORY_PROXY_BEHAVIOUR_HPP 

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ServiceNames.hpp"

#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>

#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "namingandrepositoryproxy/RepositoryProxy.hpp"
#include "namingandrepositoryproxy/InitUtils.hpp"

#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"
#include "Foundation/common/Locations.hpp"

#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"
#include "FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtCommon.stub.hpp"
#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"


#include <fstream>
#include <iostream>
#include <string>

#define COUT2(mes) std::cout << "NamingAndRepositoryProxy : " << mes << std::endl;

using namespace Cdmw::PlatformMngt;
using namespace Cdmw::Trace;


namespace Cdmw
{
namespace NamingAndRepository
{

const char *CORBALOC_NAME = "CdmwRepository";
const char *NAME_SERVICE_ID = "NameService";


/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
class NamingAndRepositoryProxyBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    /**
    * Purpose:
    * <p> The constructor.
    */ 
    NamingAndRepositoryProxyBehaviour(CORBA::ORB_ptr orb, 
            const std::string& port,
            const std::string& daemonPort,
            const std::string& processName,
            const std::string& traceProducerName)
        :m_port(port),
         m_daemonPort(daemonPort),
         m_processName(processName),
         m_traceProducerName(traceProducerName)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
    }


    /**
    * Purpose:
    * <p> The destructor.
    */ 
    virtual ~NamingAndRepositoryProxyBehaviour() throw ()
    {
    }


    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/get_service:1.0
    * operation
    */
    virtual CORBA::Object_ptr get_service() throw(CORBA::SystemException)
    {
        return CdmwNamingAndRepository::Repository::_duplicate(m_repository.in());
    }


    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/initialise:1.0
    * operation
    */
    virtual void on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CORBA::SystemException)
    {
        COUT2("Init requested");

        // Get the Replication Manager
        CORBA::Object_var replication_mgr_obj
           = m_orb->resolve_initial_references("ReplicationManager");

        m_replication_mgr = 
           ::CdmwFT::ReplicationManager::_narrow(replication_mgr_obj.in() );
        CDMW_ASSERT(!CORBA::is_nil(m_replication_mgr.in()));

        // Get the root POA
        CORBA::Object_var obj = m_orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = 
           PortableServer::POA::_narrow(obj.in());
        CDMW_ASSERT(!CORBA::is_nil(rootPOA.in()));

        // Find the RepositoryExt reference
        // TODO use XML config?

        ::FT::Name TestOGDName;
        TestOGDName.length(1);
        TestOGDName[0].id=CdmwNamingAndRepository::REPOSITORY_GROUP_ID;
        
        obj = m_replication_mgr->get_object_group_ref_from_name(TestOGDName);

        CdmwNamingAndRepository::RepositoryExt_var repository =
            CdmwNamingAndRepository::RepositoryExt::_narrow(obj.in());
        CDMW_ASSERT(!CORBA::is_nil(repository.in()));

        try
        {
            repository->resolve_root_context(
                  CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
        }
        catch (const CORBA::SystemException& ex)
        {
            std::cerr << "File: " << __FILE__ 
                      << ", Line: " << __LINE__
                      << ex <<std::endl;
            throw;
        }
        catch (const CORBA::Exception & ex)
        {
            std::cerr << "File: " << __FILE__ 
                      << ", Line: " << __LINE__
                      << ex << std::endl;
            throw CORBA::INTERNAL(Cdmw::OrbSupport::INTERNAL, 
                                  CORBA::COMPLETED_NO);
        }

        std::string application_name =
            Cdmw::PlatformMngt::PlatformInterface::Get_application_name();
        
        try
        {
            COUT2("create the repository proxy");
            // create the repository proxy
            m_repository = InitUtils::create_repository_proxy(m_orb.in(),
                                                              rootPOA.in(),
                                                              application_name,
                                                              m_processName,
                                                              repository.in());
        }
        catch (const CORBA::SystemException& ex)
        {
            std::cerr << "File: " << __FILE__ 
                      << ", Line: " << __LINE__
                      << ex <<std::endl;
            notifyFatalError("CDMW_REPOSITORY_PROXY", 
                             "Cannot create the repository proxy");
        }
        catch (const CORBA::Exception & ex)
        {
            std::cerr << "File: " << __FILE__ 
                      << ", Line: " << __LINE__
                      << ex << std::endl;
            notifyFatalError("CDMW_REPOSITORY_PROXY", 
                             "Cannot create the repository proxy");
        }

                              
         // get the daemon reference                    
        std::ostringstream daemonCorbaloc;
        try
        {
            daemonCorbaloc 
                  << "corbaloc::localhost:"
                  << m_daemonPort
                  <<  "/" << CdmwPlatformMngt::DAEMON_NAME;
                  
            CORBA::Object_var obj =
                m_orb->string_to_object (daemonCorbaloc.str().c_str());
            
            if (CORBA::is_nil (obj.in()))
            {
                std::cerr << "NamingAndRepositoryProxyBehavior : Daemon local host not found using " 
                          << daemonCorbaloc.str().c_str()
                          << std::endl;
                notifyFatalError("CDMW_REPOSITORY_PROXY", 
                                 "Daemon local host not found");
            }
        
        
            CdmwPlatformMngt::Daemon_var daemon =
                CdmwPlatformMngt::Daemon::_narrow (obj.in());
            
            if (CORBA::is_nil (daemon.in()))
            {
                std::cerr << "NamingAndRepositoryProxyBehavior : Daemon local host not found using"
                          << daemonCorbaloc.str().c_str()
                          << std::endl;
                notifyFatalError("CDMW_REPOSITORY_PROXY", 
                                 "Daemon local host not found");
            }
            
            // register the proxy as daemon local service
            daemon->register_local_service(
                  ServiceNames::NAMING_AND_REPOSITORY_SERVICE,
                  m_repository.in());
        }
        catch (const CORBA::BAD_PARAM& ex)
        {
            std::cerr << "NamingAndRepositoryProxyBehavior : Bad parameter during proxy registration in daemon " 
                      << ex
                      << std::endl;
        }
        catch (const CORBA::SystemException& ex)
        {
            std::cerr << "NamingAndRepositoryProxyBehavior : Daemon local host cannot be reachable using " 
                      << daemonCorbaloc.str().c_str()
                      << " " << ex
                      << std::endl;
        }
        catch (const CORBA::Exception& ex)
        {
            std::cerr << "NamingAndRepositoryProxyBehavior : Daemon local host cannot be reachable using " 
                      << daemonCorbaloc.str().c_str()
                      << " " << ex
                      << std::endl;
        }


#ifndef CDMW_DEACTIVATE_TRACE
        
        // create an empty registered collector list
        std::list<CdmwTrace::TraceProducer::CollectorData> collectorList;
        
        // Initializes the trace library
        CdmwTrace::TraceProducer_var traceProducer =
            FlushAreaMngr::init(rootPOA.in(),
                                collectorList, 
                                "", 
                                0,
                                "",
                                m_processName,
                                5000,    // We flush each 5 seconds
                                10,      // 10 FlushArea
                                512000,  // Each FlushArea has 0.5 Mo
                                Cdmw::Trace::FlushAreaMngr::DEFAULT_AREA_NB_MESSAGE,
                                100     // threshold for repetitive messages
                );

        if (CORBA::is_nil(traceProducer.in()))
        {
            notifyFatalError("CDMW_REPOSITORY_PROXY", 
                             "Trace library initialisation failed");
        }

#endif

#ifndef CDMW_DEACTIVATE_TRACE

#if 0
        // enable the traces if required
        if (!m_traceProducerName.empty())
        {
            // trace is enabled
            COUT2("trace enabled");
            CDMW_TRACE_ACTIVE_FLUSHING();

            // activate INF and WRN levels
            traceProducer->activate_level(Cdmw::CDMW_NREP, Cdmw::INF );
            traceProducer->activate_level(Cdmw::CDMW_NREP, Cdmw::WRN );
            COUT2("INF and WRN trace levels activated");

            // bind the trace producer within the admin root context
            std::string trace_producer_pathname;

            try
            {
                // set TraceProducer location under admin root context
                // Normal Pattern for TraceProducer is :
                //   "<host_name>/<application_name>/<process_name>/TRACE/TraceProducer"
                //
                // For the repository trace producer registration, host_name is not used
                // and process_name if used is included in m_traceProducerName variable
                //
                // Repository Trace Producer pathname is                 
                //   "<traceProducerName>/TRACE/TraceProducer"
                //
                //trace_producer_pathname = Cdmw::OsSupport::OS::get_hostname();
                //trace_producer_pathname += "/";
                trace_producer_pathname += m_traceProducerName;
                trace_producer_pathname += "/TRACE/TraceProducer";

                // create naming interface on admin root context
                Cdmw::NamingAndRepository::NamingInterface ni (adminRootContext.in());
                           
                // If object already registered, force its registration
                ni.bind (trace_producer_pathname,traceProducer.in(),true);
            }
            catch(const Cdmw::NamingAndRepository::InvalidNameException &)
            {
                std::string message("'");
                message += m_traceProducerName;
                message += "' is an invalid name for the trace producer";

                notifyFatalError("CDMW_NAMREP", message.c_str());
            }

            COUT2("trace producer interface registered as \n   \"" 
                  << trace_producer_pathname 
                  << "\" \n within " 
                  << Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID 
                  << " root context");

        }

#endif
#endif

        COUT2("started with port : " << m_port);
        // the repository is published via corbaloc 
        Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc(m_orb.in(),
                                                              CORBALOC_NAME, m_repository.in());
        COUT2("Corbaloc name : " << CORBALOC_NAME);

        // and via an IOR file
        CORBA::String_var repositoryIOR = m_orb->object_to_string(m_repository.in());

        std::ofstream repositoryIORFile("Repository.IOR");
        repositoryIORFile << repositoryIOR.in();
        repositoryIORFile.close();
        COUT2("IOR in Repository.IOR file");
        
        // Bind the Default RootContext to corbaloc Id NameService
        // to be accessible by corbaname
        try
        {
            CosNaming::NamingContext_var default_root_ctxt = 
            m_repository->resolve_root_context (CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
        
            Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc (m_orb.in(),
                                                                   NAME_SERVICE_ID,
                                                                   default_root_ctxt.in());
            COUT2(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT 
                  << "'s corbaloc name : " << NAME_SERVICE_ID);
        }
        catch (const CORBA::SystemException& ex)
        {
            std::cerr << "File: " << __FILE__ 
                      << ", Line: " << __LINE__
                      << ex << std::endl;
            notifyFatalError("CDMW_REPOSITORY_PROXY", 
                             "Cannot create the repository proxy");

        }
        catch (const CORBA::Exception& ex)
        {
            std::cerr << "File: " << __FILE__ 
                      << ", Line: " << __LINE__
                      << ex << std::endl;
            notifyFatalError("CDMW_REPOSITORY_PROXY", 
                             "Cannot bind default root context.");

        }
    }


    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
    * operation
    */
    virtual void on_run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CORBA::SystemException)
    {
        COUT2("Run requested");
    }

    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/stop:1.0
    * operation
    */
    virtual void on_stop() throw(CORBA::SystemException)
    {
        COUT2("Stop requested");

#ifndef CDMW_DEACTIVATE_TRACE

        // cleans up the trace library
        FlushAreaMngr::cleanup();

        COUT2("Trace library cleanup performed");

#endif
        m_orb->shutdown(true);

    }


private:

    /**
    * Notifies the fatal error via the platform interface or
    * output in cerr.
    */ 
    void notifyFatalError(const char* issuer, const char* message)
        throw(CORBA::SystemException)
    {

        if (m_platformManaged)
        {
            PlatformInterface::Notify_fatal_error(issuer, message);
        }
        else
        {
            std::cerr << "File: " << __FILE__ 
                      << ", Line: " << __LINE__
                      << "FAILURE : " << message << std::endl;
        }
        
        throw CORBA::INTERNAL(OrbSupport::INTERNAL, CORBA::COMPLETED_NO);

    }

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;

    /**
    * Indicates whether the repository is managed by the platform management.
    */
    bool m_platformManaged;

    /**
    * The port of the repository.
    */
    std::string m_port;
    
    /**
    * The port of the daemon.
    */
    std::string m_daemonPort;

    /**
    * The name of the process.
    */
    std::string m_processName;
    
    /**
    * The name of the trace producer.
    */
    std::string m_traceProducerName;

    /**
    * The repository implementation.
    */
    RepositoryProxy *m_repository_impl;

    /**
    * The naming and repository object;
    */
    CdmwNamingAndRepository::Repository_var m_repository;

    /**
     * The replication manager.
     */
    ::CdmwFT::ReplicationManager_var m_replication_mgr;


};

} // End namespace NamingAndRepository
} // End namespace Cdmw

#endif // INCL_NAMINGANDREPOSITORY_PROXY_BEHAVIOUR_HPP

