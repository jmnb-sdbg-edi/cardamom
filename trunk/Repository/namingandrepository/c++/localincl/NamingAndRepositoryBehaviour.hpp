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


#ifndef INCL_NAMINGANDREPOSITORYBEHAVIOUR_HPP 
#define INCL_NAMINGANDREPOSITORYBEHAVIOUR_HPP 

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include "SystemMngt/platforminterface/ProcessBehaviour.hpp"

#include "Foundation/osthreads/ThreadHandle.hpp"
#include "Foundation/ossupport/OS.hpp"

#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"

#include "namingandrepository/Repository_impl.hpp"
// namingandrepository/RepositoryExt_impl.hpp"
#include "namingandrepository/StandardRepository_impl.hpp"

#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "TraceAndPerf/tracelibrary/CdmwTrace.hpp"
#include "Foundation/common/Locations.hpp"

#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"

#include <TraceAndPerf/idllib/CdmwTraceCommon.stub.hpp>

#include "namingandrepository/Configurator.hpp"

#include <fstream>
#include <iostream>
#include <string>

#define COUT2(mes) std::cout << "NamingAndRepository : " << mes << std::endl;

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
class NamingAndRepositoryBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
{

public:

    /**
    * Purpose:
    * <p> The constructor.
    */ 
    NamingAndRepositoryBehaviour(CORBA::ORB_ptr orb, bool platformManaged,
            const std::string& port,
            const std::string& physicalId,
            const std::string& configurationFile, bool validateConfigurationFile,
            const std::string& processName,
            const std::string& traceProducerName)
      :   m_platformManaged(platformManaged),
          m_port(port),
          m_physicalId(physicalId),
          m_configurationFile(configurationFile),
          m_validateConfigurationFile(validateConfigurationFile),
          m_processName(processName),
          m_traceProducerName(traceProducerName)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
    }


    /**
    * Purpose:
    * <p> The destructor.
    */ 
    virtual ~NamingAndRepositoryBehaviour() throw ()
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
    virtual void initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw(CdmwPlatformMngt::ProcessDelegate::BadOrder, CORBA::SystemException)
    {

        std::string configFileErrorHeader("In file '");
        configFileErrorHeader += m_configurationFile;
        configFileErrorHeader += "' - ";

        // Creates the repository
        m_repository_impl = new StandardRepository_impl();
        m_repository = m_repository_impl->_this();

#ifndef CDMW_DEACTIVATE_TRACE

        // Get the root POA
        CORBA::Object_var obj = m_orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());
        
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
            notifyFatalError("CDMW_TRACE", "Trace library initialisation failed");
        }

#endif

        // Initializes the repository
        if (!m_repository_impl->initialize(m_orb.in()))
        {
            notifyFatalError("CDMW_NAMREP", "Repository initialisation failed");
        }

        // Starts the repository
        if (!m_repository_impl->start(m_orb.in()))
        {
            notifyFatalError("CDMW_NAMREP", "Repository initialisation failed");
        }

        // configure the repository via the XML file
        try
        {
            RepositoryConfigurator config(m_repository.in(), m_validateConfigurationFile);
            config.configure(m_configurationFile);
        }
        catch(const ConfigurationException& e)
        {
            std::string message(configFileErrorHeader);
            message += e.what();

            notifyFatalError("CDMW_NAMREP", message.c_str());
        }

        // register the admin root context within the CDMW admin name domain
        // TODO : Replace with a user name domain ?
        CosNaming::NamingContext_var adminRootContext;

        try
        {
            adminRootContext = m_repository_impl->resolve_root_context(
                Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID);

            CdmwNamingAndRepository::NameDomain_var adminNameDomain =
                m_repository_impl->resolve_name_domain(Cdmw::Common::Locations::CDMW_ADMIN_NAME_DOMAIN);

            adminNameDomain->register_new_object(m_physicalId.c_str(), adminRootContext.in());
        }
        catch(const CdmwNamingAndRepository::Repository::NoRootContext &)
        {
            std::string message(configFileErrorHeader);
            message += "AdminRootContext not found";

            notifyFatalError("CDMW_NAMREP", message.c_str());
        }
        catch(const CdmwNamingAndRepository::NoNameDomain &)
        {
            std::string message(configFileErrorHeader);
            message += "Name domain '";
            message += Cdmw::Common::Locations::CDMW_ADMIN_NAME_DOMAIN;
            message += "' not found";

            notifyFatalError("CDMW_NAMREP", message.c_str());
        }
        catch(const CdmwNamingAndRepository::InvalidName &)
        {
            std::string message("'");
            message += m_physicalId;
            message += "' is an invalid physical identifier";

            notifyFatalError("CDMW_NAMREP", message.c_str());
        }
        catch(...)
        {
            notifyFatalError("CDMW_NAMREP", "Something caught!!");
        }

        // Register the FactoryFinder
        try
        {
            CosNaming::NamingContext_var rootCtx = 
            m_repository_impl->resolve_root_context(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
            
            CosNaming::Name_var name =
            Cdmw::CommonSvcs::Naming::NamingInterface::to_name(CdmwNamingAndRepository::FACTORY_FINDER);
            
            rootCtx->rebind(name.in(), m_repository_impl->get_factory_finder());
        } 
        catch (...)
        {
            notifyFatalError("CDMW_NAMREP", 
                             "Failed to rebind factory finder reference");
        }

#ifndef CDMW_DEACTIVATE_TRACE

        // enable the traces if required
        if (!m_traceProducerName.empty())
        {
            // trace is enabled
            COUT2("trace enabled");
            CDMW_TRACE_ACTIVE_FLUSHING();

            // activate INF and WRN levels
            traceProducer->activate_level(CdmwTrace::ALL_COMPONENT_NAMES, // ECR-0123
                                          Cdmw::CDMW_NREP,
                                          Cdmw::INF);
            traceProducer->activate_level(CdmwTrace::ALL_COMPONENT_NAMES, // ECR-0123
                                          Cdmw::CDMW_NREP,
                                          Cdmw::WRN);
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
				Cdmw::CommonSvcs::Naming::NamingInterface ni (adminRootContext.in());
				           
				// If object already registered, force its registration
				ni.bind (trace_producer_pathname,traceProducer.in(),true);
            }
            catch(const Cdmw::CommonSvcs::Naming::InvalidNameException &)
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
        CosNaming::NamingContext_var default_root_ctxt = 
            m_repository->resolve_root_context (CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);
        Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc (m_orb.in(),
                                                                NAME_SERVICE_ID,
                                                                default_root_ctxt.in());
        COUT2(CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT << "'s corbaloc name : " << NAME_SERVICE_ID);

    }


    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/ProcessDelegate/run:1.0
	* operation
	*/
    virtual void run()
        throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
              CdmwPlatformMngt::ProcessDelegate::AlreadyDone,
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
    virtual void stop() throw(CORBA::SystemException)
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
            std::cerr << "FAILURE : " << message << std::endl;
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
    * The physical identifier of the repository.
    */
    std::string m_physicalId;

    /**
    * The configuration file of the repository.
    */
    std::string m_configurationFile;

    /**
    * Indicates whether the configuration file shall be validated.
    */
    bool m_validateConfigurationFile;

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
    Repository_impl *m_repository_impl;

    /**
    * The naming and repository object;
    */
    CdmwNamingAndRepository::Repository_var m_repository;

};

} // End namespace NamingAndRepository
} // End namespace Cdmw

#endif // INCL_NAMINGANDREPOSITORYBEHAVIOUR_HPP

