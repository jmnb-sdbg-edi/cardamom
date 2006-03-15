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

#include <clocale>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/System.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Foundation/commonsvcs/naming/NamingUtil.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>

#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>

#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>
#include <SystemMngt/platforminterface/ServiceNames.hpp>

#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"

#include "tracecollector/ClientThread.hpp"
#include "tracecollector/CollectorMngr.hpp"
#include "tracecollector/ProcessAgent_impl.hpp"


namespace
{
    // default port number used by collector
    const std::string defServicePort = "51234";
    const std::string defProcessPort = "51235";

    const std::string COLLECTOR_NAME_OPTION = "--collector-name";
    const std::string TRACE_FILE_NAME_OPTION = "--traceFile-name";
    // ECR-0123
    const std::string COMPONENT_NAME_OPTION = "--component-name";
    const std::string DOMAIN_NAME_OPTION = "--domain-name";
    const std::string LEVEL_OPTION = "--level";
    const std::string NAMING_REPOS_URL_OPTION = "--namingRepos-url";
    const std::string TRACE_PRODUCER_PATH_OPTION = "--traceProducer-path";

    const std::string TRACE_QUEUE_STRATEGY_OPTION = "--queueStrategy";
    const std::string TRACE_QUEUE_STRATEGY_OPT1 = "drop_latest";
    const std::string TRACE_QUEUE_STRATEGY_OPT2 = "remove_oldest";
    const std::string TRACE_QUEUE_SIZE_OPTION = "--queue-size";
    const std::string TRACE_QUEUE_STRING_SIZE_OPTION = "--queueString-size";
    const std::string TRACE_FILE_BKP_NBR_OPTION = "--traceFileBkp-number";
    const std::string TRACE_FILE_NBR_OPTION = "--traceFile-number";
    const std::string TRACE_MESSAGE_NBR_OPTION = "--traceMessage-number";
    const std::string TRACE_FORMAT_OPTION = "--trace-format";
    const std::string TRACE_FORMAT_OPTV = "V";
    const std::string TRACE_FORMAT_OPTH = "H";

    const std::string INTERACTIVE_OPTION = "--interactive";

    const std::string AGENT_CORBALOC_ID  = "CollectorAgent";

    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;

    const std::string GLOBAL_COLLECTOR_INI_MNEMONAME = "global_collector_ini_";

    static void
    usage(std::ostream & os, const std::string &program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                             Display this information.\n"
           << "--interactive                      Start in interactive mode.\n"
           << "\n"
           << "--collector-name=<name>            Name of Trace Collector.\n"
           << "[--traceFile-name=<name>]          Name of Trace File.\n"
           << "[--component-name=<name>]          Filter on this component name.\n" /* ECR-0123 */
           << "[--domain-name=<name>]             Name of domain to filter.\n"
           << "[--level=<value>]                  Level to filter.\n"
           << "[--trace-format=<H|V>]             Format of Trace : Horizontal or Vertical (default).\n"
           << "[--CdmwLocalisationService=<nbr>]  Service Port of Trace Collector.\n"
           << "[--CdmwProcessPort=<nbr>]          Process Port of Trace Collector.\n"
           << "[" << Cdmw::Common::Options::CALLBACK_IOR_OPTION 
           << "=<IOR>]                            IOR of the ProcessCallback object.\n"
           << "[--namingRepos-url=<URL>]          URL of the CDMW Naming and Repository service.\n"
           << "[--queueStrategy=<strategy>]       Strategy \"drop_latest\" or \"remove_oldest\".\n"
           << "[--queue-size=<size>]              Size of queue in KByte.\n"
           << "[--queueString-size=<size>]        Size of string queue in KByte.\n"
           << "[--traceBkpFile-number=<number>]   Number of backup of trace files.\n"
           << "[--traceFile-number=<number>]      Number of trace circular files.\n"
           << "[--traceMessage-number=<number>]   Number of trace messages in circular files.\n"
           << "[--traceProducer-path  <Path 1> <Path 2> ...] Path of Trace Producers\n"
           << "                                   Path is defined as host/application/process\n"
           << std:: endl;
    }


    /**
     * Default process behaviour.
     */
    class MyProcessBehaviour: public Cdmw::PlatformMngt::ProcessBehaviour
    {
        public:
            MyProcessBehaviour(CORBA::ORB_ptr orb)
                throw(CORBA::SystemException)
                    : m_orb(CORBA::ORB::_duplicate(orb))
            {
            }


            ~MyProcessBehaviour()
                throw()
            {
            }


            virtual void
            initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
                throw(CORBA::SystemException)
            {
                // Do nothing!
            }


            virtual void
            run()
                throw(CdmwPlatformMngt::ProcessDelegate::NotReadyToRun,
                     CORBA::SystemException)
            {
                // Do nothing!
            }


            virtual void
            stop()
                throw(CORBA::SystemException)
            {
                m_orb->shutdown(false);
            }


            virtual bool
            is_alive()
                throw(CORBA::SystemException)
            {
                return true;
            }


        private:
            CORBA::ORB_var m_orb;
    };
}; // anonymous namespace


/**
 * Start the client thread in interactive mode and start the ORB.
 *
 * @param orb the ORB.
 * @param interactiveMode flag to set to true if interactive mode is wanted.
 */
int
run(CORBA::ORB_ptr orb, bool interactiveMode)
{
    int status = SUCCESS;

    // if interactive mode not set.
    if (!interactiveMode) {
        // just run the ORB.
        orb->run();
    }
    else {
        // in interactive mode, create a new client thread.
        Cdmw::Trace::ClientThread client(orb, std::cin, std::cout);

        // start the client and run the ORB.
        client.start();
        orb->run();

        // wait for the completion of the client thread
        // once the CORBA loop has stopped.
        client.join();
    }

    return status;
}


/**
 * Process initialization:
 *
 * - create the POA for the collector servant under RootPOA.
 * - create the CollectorMngr object:
 *       + the collector servant is created and activated on its POA.
 *       + the flushing data background thread is started.
 * - export the collector servant to the IOR file.
 * - create ProcessAgentImpl object: the process agent servant is created
 *   and activated on the RootPOA.
 * - activate the POA Manager.
 * - retrieve the Naming & Repository service.
 * - register the collector servant in Naming & Repository.
 * - register the process agent servant as URL under corbaloc.
 */
int
process_init(CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA,
             const std::string &collectorName,
             const std::string &traceFileName,
             const std::string& componentName, // ECR-0123
             const std::string &domainName,
             long levelValue,
             Cdmw::Trace::Message::TraceFormat traceFormat,
             CdmwNamingAndRepository::Repository_ptr repositoryRef,
             const std::string &traceProducerContext,
             int traceProducerInx, int argc, char* argv[],
             /* ECR-0145 CdmwNamingAndRepository::NameDomain_ptr &pTraceCollectorDom,*/
             Cdmw::Trace::Message::CircularMode queueStrategy,
             long queueSize,
             long queueStringSize,
             long traceFileBkpNbr,
             long traceFileNbr,
             long traceMessageNbr,
             std::vector<std::string>& collectorNames)
{
    int status = FAILURE;

    using Cdmw::Common::Locations;
    using namespace Cdmw::OrbSupport;

    // Collector servant reference
    CdmwTrace::Collector_var collectorRef = CdmwTrace::Collector::_nil();

    // Process Agent servant reference
    TraceCollector::ProcessAgent_var agentRef = TraceCollector::ProcessAgent::_nil();

    try {
        // ECR-0145
        // look for the global collectors.
        // the global collectors are registered in the following Name domain:
        // CDMW/SERVICES/TRACE/COLLECTORS/
        std::string collectorDomainStr(Locations::CDMW_SERVICES_NAME_DOMAIN);
        collectorDomainStr += "/";
        collectorDomainStr += Locations::TRACE_COLLECTORS_NAME_DOMAIN;

        std::list<CdmwTrace::TraceProducer::CollectorData> globalCollectors;
        try {
            // check if the collector domain name is a Name domain.
            CdmwNamingAndRepository::NameDomain_var collectorDomain =
                repositoryRef->resolve_name_domain(collectorDomainStr.c_str());

            // get the default root context.
            CosNaming::NamingContext_var nc_root_context =
                repositoryRef->resolve_root_context(
                    CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

            using namespace Cdmw::CommonSvcs::Naming;
            NamingInterface ni_root(nc_root_context.in());
            CosNaming::NamingContext_var nc_collector_dom =
                CosNaming::NamingContext::_nil();

            // get the collector domain name.
            typedef NamingUtil<CosNaming::NamingContext> Util;
            nc_collector_dom = Util::resolve_name(ni_root,
                                                  collectorDomainStr);

            CosNaming::BindingIterator_var bindingItr;
            CosNaming::BindingList_var bindingList;

            NamingInterface ni_collector(nc_collector_dom.in());

            // get all the global collectors registered in this domain.
            ni_collector.list(NamingInterface::LIST_CHUNK_SIZE,
                              bindingList,
                              bindingItr);

            bool hasMoreBinding = false;
            int collectorIndex = 0;

            do {
                for (size_t iBinding = 0;
                     iBinding < bindingList->length();
                     ++iBinding)
                {
                    CosNaming::Binding binding = bindingList[iBinding];

                    // get the collector name from the binding.
                    std::string collectorName =
                        ni_collector.to_string(binding.binding_name);

                    // store the collector name.
                    collectorNames.push_back(collectorName);

                    CORBA::Object_var obj =
                        ni_collector.resolve(collectorName);
                    CdmwTrace::Collector_var collector =
                        CdmwTrace::Collector::_narrow(obj.in());

                    if (!CORBA::is_nil(collector.in())) {
                        // create a mnemonic name for the collector.
                        std::ostringstream mnemo_name_str;
                        mnemo_name_str << GLOBAL_COLLECTOR_INI_MNEMONAME
                                       << collectorIndex;
                        collectorIndex++;

                        // create a CollectorData structure.
                        CdmwTrace::TraceProducer::CollectorData collector_data;
                        collector_data.the_collectorMnemoName =
                            CORBA::string_dup(mnemo_name_str.str().c_str());
                        collector_data.the_collectorObjName =
                            CORBA::string_dup(collectorName.c_str());
                        collector_data.the_collector = collector;

                        // append it to the list.
                        globalCollectors.insert(globalCollectors.end(),
                                                collector_data);
                    }
                }

                // should we continue?
                if (!CORBA::is_nil(bindingItr.in())) {
                    hasMoreBinding =
                        bindingItr->next_n(NamingInterface::LIST_CHUNK_SIZE,
                                           bindingList);
                }
            } while (hasMoreBinding);
        }
        catch (const CosNaming::NamingContext::NotFound&) {
            // should not happen!
            std::cerr << "Trace Collector Name Domain not found \n"
                      << collectorDomainStr.c_str()
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::Repository::NoRootContext&) {
            std::cerr << "Trace Collector Root Context does not exist"
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::NoNameDomain&) {
            std::cerr << "Trace Collector Name Domain does not exist \n"
                      << collectorDomainStr.c_str()
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::InvalidName&) {
            std::cerr << "Trace Collector Name Domain has illegal form \n"
                      << collectorDomainStr.c_str()
                      << std::endl;
        }
        catch (const CosNaming::NamingContext::InvalidName&) {
            std::cerr << "Trace Collector Name invalid \n"
                      << collectorDomainStr.c_str()
                      << std::endl;
        }
        catch (const Cdmw::Common::TypeMismatchException& e) {
            std::cerr << "TypeMismatchException (Not a NamingContext) <"
                      << e.what() << ">" << std::endl;
        }

        // create a new POA
        // Trace Collector framework use the following POA policies
        //
        // Lifespan policy            = PERSISTENT
        // Id Assignment policy       = USER_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = NO_IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY;
        // Servant Retention policy   = RETAIN;
        //
        CORBA::PolicyList policies;
        policies.length(6);

        policies[0] =
            rootPOA->create_id_assignment_policy(PortableServer::USER_ID);
        policies[1] =
            rootPOA->create_lifespan_policy(PortableServer::PERSISTENT);
        policies[2] =
            rootPOA->create_servant_retention_policy(PortableServer::RETAIN);
        policies[3] =
            rootPOA->create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
        policies[4] =
            rootPOA->create_request_processing_policy(
                PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY);
        policies[5] =
            rootPOA->create_implicit_activation_policy(
                PortableServer::NO_IMPLICIT_ACTIVATION);

        Cdmw::OrbSupport::StrategyList poaStrategies;

        // Strategies with MULTITHREAD policy will accept Single Thread and
        // Multi Thread strategies. However, SERIALIZE policy will only allow
        // Single Thread policy.

        PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
        PortableServer::POA_var collectorsPOA = PortableServer::POA::_nil();

        collectorsPOA = Cdmw::OrbSupport::OrbSupport::create_POA(
                            rootPOA,
                            collectorName.c_str(),
                            poaManager.in(),
                            policies,
                            poaStrategies);

        // create and initialize the collector manager
        collectorRef =
            Cdmw::Trace::CollectorMngr::Init(
                collectorsPOA.in(),
                globalCollectors, // ECR-0145
                componentName, // ECR-0123
                domainName,
                levelValue,
                traceFormat,
                queueStrategy,
                queueSize,
                queueStringSize,
                traceFileName,
                traceFileBkpNbr,
                traceFileNbr,
                traceMessageNbr,
                Cdmw::OsSupport::OS::LOCAL_TIME
                );

        // export the object reference to a file
        CORBA::String_var ref_string =
            orb->object_to_string(collectorRef.in());
        std::ofstream os("traceCollector.ior");
        os << ref_string.in();
        os.close();

        // create the ProcessAgent Servant (ref count is incremented)
        Cdmw::Trace::ProcessAgent_impl* pProcessAgentServant =
            new Cdmw::Trace::ProcessAgent_impl (orb,rootPOA);

        // create an object var to take pointer ownership
        // (ref count will be decremented when var will be destroyed at
        // the method end)
        PortableServer::ServantBase_var servant_var = pProcessAgentServant;

        // activate servant on POA (ref count is incremented)
        PortableServer::ObjectId_var oid =
            rootPOA->activate_object(pProcessAgentServant);
        CORBA::Object_var object = rootPOA->id_to_reference(oid.in());
        agentRef = TraceCollector::ProcessAgent::_narrow(object.in());

        status = SUCCESS;

        // ECR-0145
        // Register Collector object in Naming & repository
        // under AdminRootContext.
        // trace collector location:
        // <hostname>/SERVICES/TRACE/COLLECTORS/collectorName
        
        try {
            if (status == SUCCESS) {
                status = FAILURE;

                CosNaming::Name collectorName_;
                collectorName_.length(5);
                collectorName_[0].id =
                    CORBA::string_dup(
                        Cdmw::OsSupport::OS::get_hostname().c_str());
                collectorName_[1].id = CORBA::string_dup("SERVICES");
                collectorName_[2].id = CORBA::string_dup("TRACE");
                collectorName_[3].id = CORBA::string_dup("COLLECTORS");
                collectorName_[4].id = CORBA::string_dup(collectorName.c_str());

                CosNaming::NamingContext_var adminRootCtx =
                    repositoryRef->resolve_root_context(
                        Locations::ADMIN_ROOT_CONTEXT_ID);

                Cdmw::CommonSvcs::Naming::NamingInterface ni(adminRootCtx.in());
                ni.bind(ni.to_string(collectorName_).c_str(),
                        collectorRef.in(),
                        true);

                status = SUCCESS;
            }
        }
        catch (const CdmwNamingAndRepository::Repository::NoRootContext&) {
            std::cerr << "Trace Collector Root Context does not exist"
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::NoNameDomain&) {
            std::cerr << "Trace Collector Name Domain does not exist"
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::InvalidName&) {
            std::cerr << "Trace Collector Name Domain has illegal form"
                      << std::endl;
        }

        // path of trace producer extracted from arguments list
        std::string traceProducerObjectPath = "";

        // Register Collector object to each Trace Producer passed as argument
        try {
            if (status == SUCCESS) {
                // set TraceProducer location under admin root context
                // Pattern is :
                //   "[<host_name>/<application_name>/<process_name>]/TRACE/TraceProducer"

                // Get trace producer root naming context
                CosNaming::NamingContext_var producer_root_nc =
                    repositoryRef->resolve_root_context(
                        traceProducerContext.c_str());

                // NamingInterface on Root context
                Cdmw::CommonSvcs::Naming::NamingInterface
                producer_ni(producer_root_nc.in());

                typedef Cdmw::CommonSvcs::Naming::NamingUtil<CdmwTrace::TraceProducer> Util;

                while (traceProducerInx < argc) {
                    // Get the path of the Trace Producer object
                    traceProducerObjectPath = argv[traceProducerInx];

                    // Build the complete name of the Trace Producer object
                    std::string producer_context_name = traceProducerObjectPath;
                    //traceProducerObjectPath contains the complete object name
                    //  producer_context_name += "/TRACE/TraceProducer";

                    try {
                        // get TraceProducer object using Root context
                        // NamingInterface
                        CdmwTrace::TraceProducer_var traceProducer =
                            Util::resolve_name(producer_ni,
                                               producer_context_name);

                        if (CORBA::is_nil (traceProducer.in())) {
                            std::cerr << traceProducerObjectPath
                                      << " is not a valid trace producer"
                                      << std::endl;
                        }
                        else {
                            // register the collector in the trace producer
                            traceProducer->register_collector(
                                collectorRef.in(),
                                collectorName.c_str(),
                                collectorName.c_str());
                        }
                    }
                    catch (const CosNaming::NamingContext::NotFound&) {
                        std::cerr << "Trace Producer Naming Context NotFound : "
                                  << traceProducerObjectPath.c_str()
                                  << std::endl;
                    }
                    catch (const CosNaming::NamingContext::CannotProceed&) {
                        std::cerr
                            << "Unexpected Error (CannotProceed exception)!"
                            << std::endl;
                    }
                    catch (const CosNaming::NamingContext::InvalidName&) {
                        std::cerr << "Invalid Name <"
                                  << traceProducerObjectPath << ">!"
                                  << std::endl;
                    }
                    catch (const Cdmw::Common::TypeMismatchException& e) {
                        std::cerr << "TypeMismatchException "
                                  << "(Not a Trace Producer object) <"
                                  << e.what() << ">" << std::endl;
                    }
                    catch (const CORBA::Exception& ex) {
                        std::cerr << "Trace Producer access error : "
                                  << traceProducerObjectPath.c_str()
                                  << std::endl << ex << std::endl;
                    }

                    // get next Trace Producer
                    traceProducerInx++;
                }
            }
        }
        catch (const CdmwNamingAndRepository::Repository::NoRootContext&) {
            std::cerr << "Trace Root Context does not exist" << std::endl;
            status = FAILURE;
        }

        // Register Agent object to Corbaloc
        if (status == SUCCESS) {
            status = FAILURE;

            // Bind the ProcessAgent to a Corbaloc name
            Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc(
                    orb,
                    AGENT_CORBALOC_ID,
                    agentRef.in());

            status = SUCCESS;
        }
    }
    catch (const std::bad_alloc&) {
        std::cerr << "Process Initialisation : Bad Allocation Exception \n" ;
    }
    catch (const Cdmw::Exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    catch (const CORBA::SystemException &ex) {
        std::cerr << ex << std::endl;
    }
    catch(const CORBA::Exception& ex) {
        std::cerr << ex << std::endl;
    }

    return status;
}


//
// end of processing
//    release the Collector name in the domain
//
void process_end(const std::string &collectorName /* ECR-0145 ,
                 const CdmwNamingAndRepository::NameDomain_ptr pTraceCollectorDom*/)
{
    using namespace Cdmw::OrbSupport;

    Cdmw::Trace::CollectorMngr::Cleanup();
}

void exceptionHandler(void)
{
    std::cout << "Trace Collector : UNEXPECTED EXCEPTION HANDLER" << std::endl;
}



// ORB reference
CORBA::ORB_var orb = CORBA::ORB::_nil();

void exit_handler(int sig) {
    if (!CORBA::is_nil(orb.in())) {
        try {
            orb->shutdown(false);
        } catch (const CORBA::SystemException& e) {
            std::cerr << "Error while shuting ORB down in exit_handler:\n"
                      << e << " - minor code: " << e.minor() << std::endl;
        }
    }
}


//
// main module
//
int main(int argc, char* argv[])
{
    // for international char management
    // set for all locale categories the traditional unix system behavior
    ::setlocale (LC_ALL,"C");
    
#   ifndef _MSC_VER    

    std::set_unexpected(&exceptionHandler);

    // VC++ contrary to the clause 18.6 of the C++ standard, 
     //  set_unexpected, is not inside the std:: namespace
#   else

    set_unexpected(&exceptionHandler);

#   endif  



    using namespace Cdmw::OsSupport;

    // get arguments
    if(argc == 1) 
    {
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    //
    // help argument
    //
    if(strcmp(argv[1], "--help") == 0) 
    {
        usage(std::cout, argv[0]);
        return SUCCESS;
    }
    
    std::string allTests = OS::get_option_value(argc, argv, INTERACTIVE_OPTION) ;
     
    // ---- Does it use interactive mode? ----
    bool interactive_mode = false;
    if (allTests != "no") 
    {
        interactive_mode = true;
    } 


    //
    // get Trace Collector Name
    //
    std::string collectorName = OS::get_option_value (argc, argv, COLLECTOR_NAME_OPTION);
    if (collectorName == "no" || collectorName == "yes") 
    {
        /* ECR-0145
        std::cout << COLLECTOR_NAME_OPTION.c_str() << "=<name> option must be defined\n";
        std::cout << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
        */
        collectorName = "LocalCollector";
    }
    
    //
    // get Trace File Name
    //
    std::string traceFileName = OS::get_option_value (argc, argv, TRACE_FILE_NAME_OPTION);
    if (traceFileName == "no") 
    {
        traceFileName = ""; 
    }
    else if (traceFileName == "yes") 
    {
        std::cout << TRACE_FILE_NAME_OPTION.c_str() << "=<name> option must be defined\n";
        std::cout << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    // get the component name
    // ECR-0123
    std::string componentName = OS::get_option_value(argc, argv, COMPONENT_NAME_OPTION);
    if (componentName == "no") {
        componentName = CdmwTrace::ALL_COMPONENT_NAMES;
    }
    else if (componentName == "yes") {
        std::cout << "missing value for option "
                  << COMPONENT_NAME_OPTION.c_str() << "\n" << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    //
    // get Filter Domain Name
    //
    std::string domainName = OS::get_option_value (argc, argv, DOMAIN_NAME_OPTION);
    if (domainName == "no") 
    {
        domainName = CdmwTrace::ALL_DOMAINS;
    }
    else if (domainName == "yes") 
    {
        std::cout << DOMAIN_NAME_OPTION.c_str() << "=<name> option must be defined\n";
        std::cout << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
    }
    

    //
    // get Filter Level value
    //
    std::string levelOption = OS::get_option_value (argc, argv, LEVEL_OPTION);
    long levelValue;
    if (levelOption == "no") 
    {
        levelValue = CdmwTrace::ALL_VALUES;
    } 
    else 
    {
        std::istringstream levelOption_strm(levelOption);
        levelOption_strm >> levelValue;
        if (levelOption_strm.fail()) 
        {
            std::cout << "bad value for " << LEVEL_OPTION.c_str() << "=<value> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }
    
    //
    // get Format of trace
    //
    Cdmw::Trace::Message::TraceFormat traceFormat = Cdmw::Trace::Message::V_FORMAT;
    std::string formatOption = OS::get_option_value (argc, argv, TRACE_FORMAT_OPTION);
    if (formatOption != "no") 
    {
        if (formatOption == TRACE_FORMAT_OPTV.c_str()) 
        {
            traceFormat = Cdmw::Trace::Message::V_FORMAT;
        } 
        else if (formatOption == TRACE_FORMAT_OPTH.c_str()) 
        {
            traceFormat = Cdmw::Trace::Message::H_FORMAT;
        } 
        else 
        {
            std::cout << "bad value for " << TRACE_FORMAT_OPTION.c_str() << " option\n";
            std::cout << "  options are : " << TRACE_FORMAT_OPTV.c_str() 
                      << " or " << TRACE_FORMAT_OPTH.c_str() << "\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }

    //
    // get Trace collector Port numbers
    // (this option is passed to ORB_init via ORBsupport::ORB_init)
    // 
    
    bool arg_rebuilt = false;
    char** myArgv = argv;
    int myArgc = argc;
    int arg_ini_inx = argc;
    int arg_inx = argc;
    std::string servicePortOption;
    std::string processPortOption;

    std::string servicePort = 
             OS::get_option_value (argc, argv, Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION);
    if (servicePort != "no") 
    {
        int portNbr;
        std::istringstream servicePort_strm(servicePort);
        servicePort_strm >> portNbr;
        if (servicePort_strm.fail()) 
        {
            std::cout << "bad value for "
                      << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION
                      << "=<value> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }
    
    std::string processPort = 
             OS::get_option_value (argc, argv, Cdmw::Common::Options::PROCESS_PORT_OPTION);
    if (processPort != "no") 
    {
        int portNbr;
        std::istringstream processPort_strm(processPort);
        processPort_strm >> portNbr;
        if (processPort_strm.fail()) 
        {
            std::cout << "bad value for "
                      << Cdmw::Common::Options::PROCESS_PORT_OPTION
                      << "=<value> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }

    // if one port is not supplied as program arguments
    // add in arguments the corresponding option with default value
    if (servicePort == "no" || processPort == "no") 
    {
        // allocate enough place to contain each argument string
        //  plus the port option arguments
        //  and a NULL string to indicate the end
    
        try 
        {
            myArgv = new char*[argc+3];
        
            for (int i=0 ; i < argc ; i++) 
            {
                myArgv[i] = argv[i];
            }

            myArgv[argc] = NULL;
            myArgv[argc+1] = NULL;
            myArgv[argc+2] = NULL;
            arg_rebuilt = true;
      
            if (servicePort == "no") 
            {
                servicePortOption = Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
                servicePortOption += "=" + defServicePort;
                myArgv[arg_inx] = new char [servicePortOption.size() + 1];
                ::strcpy (myArgv[arg_inx], servicePortOption.c_str());
                arg_inx ++;
                servicePort = defServicePort;
            }

            if (processPort == "no") 
            {
                processPortOption = Cdmw::Common::Options::PROCESS_PORT_OPTION;
                processPortOption += "=" + defProcessPort;
                myArgv[arg_inx] = new char [processPortOption.size() + 1];
                ::strcpy (myArgv[arg_inx], processPortOption.c_str());
                arg_inx ++;
                processPort = defProcessPort;
            }
        } 
        catch (const std::bad_alloc&) 
        {
            std::cout << "bad allocation for new arguments" << std::endl;
            return FAILURE;
        }
    
        // set number of arguments
        myArgc = arg_inx;
    }
    
    std::cout << "Trace Collector " << collectorName.c_str() 
              << " is listening on port " << servicePort << "\n"; 
    std::cout << "   (" << Cdmw::Common::Options::PROCESS_PORT_OPTION
              << " is set to " << processPort << ")" << "\n";
    std::cout << std::endl;

    //
    // Get the NamingAndRepository IOR. We are either launched by the PlatformManagement
    // in which case we can query for the reference, or the reference is directly passed
    // in program arguments.
    //
    
    using namespace Cdmw::PlatformMngt;
    bool platformManaged =  PlatformInterface::Is_launched_by_PlatformManagement(argc, argv);

    //
    // get Naming & Repository URL argument
    //
    std::string namingReposURL = OS::get_option_value (argc, argv, NAMING_REPOS_URL_OPTION);
    if (namingReposURL == "no") 
    {
        namingReposURL = "";
    }
    
    if (!platformManaged && namingReposURL == "") 
    {
        std::cout << Cdmw::Common::Options::CALLBACK_IOR_OPTION << "=<IOR> option or "
                  << NAMING_REPOS_URL_OPTION.c_str() << "=<URL> option must be defined\n";
        std::cout << std::endl;
        usage(std::cout,argv[0]);
        return FAILURE;
    }

    //
    // get Trace Queue Strategy
    //
    Cdmw::Trace::Message::CircularMode queueStrategy = Cdmw::Trace::Message::REMOVE_OLDEST;
    std::string queueStategyOption = OS::get_option_value (argc, argv, TRACE_QUEUE_STRATEGY_OPTION);
    if (queueStategyOption != "no") 
    {
        if (queueStategyOption == TRACE_QUEUE_STRATEGY_OPT1.c_str()) 
        {
            queueStrategy = Cdmw::Trace::Message::DROP_LATEST;
        } 
        else if (queueStategyOption == TRACE_QUEUE_STRATEGY_OPT2.c_str()) 
        {
            queueStrategy = Cdmw::Trace::Message::REMOVE_OLDEST;
        } 
        else 
        {
            std::cout << "bad value for " << TRACE_QUEUE_STRATEGY_OPTION.c_str() << " option\n";
            std::cout << "  options are : " << TRACE_QUEUE_STRATEGY_OPT1.c_str() 
                      << " or " << TRACE_QUEUE_STRATEGY_OPT2.c_str() << "\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }

    //
    // get Trace Queue size
    //
    std::string queueSizeOption = OS::get_option_value (argc, argv, TRACE_QUEUE_SIZE_OPTION);
    long queueSize;
    if (queueSizeOption == "no") 
    {
        queueSize = 1000;            // set 1000 KB size
    } 
    else 
    {
        std::istringstream queueSizeOption_strm(queueSizeOption);
        queueSizeOption_strm >> queueSize;
        if (queueSizeOption_strm.fail()) 
        {
            std::cout << "bad value for " << TRACE_QUEUE_SIZE_OPTION.c_str() << "=<value> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }
    
    //
    // get Trace Queue String size
    //
    std::string queueStringSizeOption = OS::get_option_value (argc, argv, TRACE_QUEUE_STRING_SIZE_OPTION);
    long queueStringSize;
    if (queueStringSizeOption == "no") 
    {
        queueStringSize = 100;       // set 100 KB size
    } 
    else 
    {
        std::istringstream queueStringSizeOption_strm(queueStringSizeOption);
        queueStringSizeOption_strm >> queueStringSize;
        if (queueStringSizeOption_strm.fail()) 
        {
            std::cout << "bad value for " << TRACE_QUEUE_STRING_SIZE_OPTION.c_str() << "=<value> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }

    //
    // get number of trace file backup
    //
    std::string traceFileBkpNbrOption = OS::get_option_value (argc, argv, TRACE_FILE_BKP_NBR_OPTION);
    long traceFileBkpNbr;
    if (traceFileBkpNbrOption == "no") 
    {
        traceFileBkpNbr = 2;       // set 2 backup of files
    } 
    else 
    {
        std::istringstream traceFileBkpNbrOption_strm(traceFileBkpNbrOption);
        traceFileBkpNbrOption_strm >> traceFileBkpNbr;
        if (traceFileBkpNbrOption_strm.fail()) 
        {
            std::cout << "bad value for " << TRACE_FILE_BKP_NBR_OPTION.c_str() << "=<value> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }
    
    
    //
    // get Trace Circular File number
    //
    std::string traceFileNbrOption = OS::get_option_value (argc, argv, TRACE_FILE_NBR_OPTION);
    long traceFileNbr;
    if (traceFileNbrOption == "no") 
    {
        traceFileNbr = 2;       // set 2 files
    } 
    else 
    {
        std::istringstream traceFileNbrOption_strm(traceFileNbrOption);
        traceFileNbrOption_strm >> traceFileNbr;
        if (traceFileNbrOption_strm.fail()) 
        {
            std::cout << "bad value for " << TRACE_FILE_NBR_OPTION.c_str() << "=<value> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }

    //
    // get Trace Message number to store in file
    //
    std::string traceMessageNbrOption = OS::get_option_value (argc, argv, TRACE_MESSAGE_NBR_OPTION);
    long traceMessageNbr;
    if (traceMessageNbrOption == "no") 
    {
        traceMessageNbr = 50;       // set 50 messages by file
    } 
    else  
    {
        std::istringstream traceMessageNbrOption_strm(traceMessageNbrOption);
        traceMessageNbrOption_strm >> traceMessageNbr;
        if (traceMessageNbrOption_strm.fail()) 
        {
            std::cout << "bad value for " << TRACE_MESSAGE_NBR_OPTION.c_str() << "=<value> option\n";
            std::cout << std::endl;
            return FAILURE;
        }
    }

    // set Trace producer context
    std::string traceProducerContext =
        Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID;

    // skip all arguments before the TRACE_PRODUCER_PATH_OPTION, after it there are
    // only path in the repository to find TraceProducer objects

    int traceProducerInx = 1;
    while (traceProducerInx < argc && TRACE_PRODUCER_PATH_OPTION != argv[traceProducerInx]) 
    {
        traceProducerInx++;
    }
    
    // marker argument is found, the next argument must be a path in the Repository
    if (traceProducerInx != argc) 
    {
        traceProducerInx++;
    }

    int status = SUCCESS;
    
    // Trace Collector domain
    /* ECR-0145
    CdmwNamingAndRepository::NameDomain_var traceCollectorDom = 
                        CdmwNamingAndRepository::NameDomain::_nil();
    */
    
    try 
    {
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
    
        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(myArgc, myArgv, orb_strategies);


        // get a reference to the root POA
        CORBA::Object_var rootObj = orb->resolve_initial_references ("RootPOA");
        PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(rootObj.in());


        // activate the POA Manager
        PortableServer::POAManager_var POAMngr = rootPOA->the_POAManager();
        POAMngr->activate();


        using namespace Cdmw::OrbSupport;

        CdmwNamingAndRepository::Repository_var repositoryRef =
                                  CdmwNamingAndRepository::Repository::_nil();
                                  
        // Are we launched by the PlatformManagement? Initialise PlatformInterface library then.
        if (platformManaged) 
        {
            // initialise the platform interface
            PlatformInterface::Setup(orb.in(), argc, argv);
            
            // Create a Process Behaviour
            // acknowledge the creation of the process
            PlatformInterface::Acknowledge_creation(
                new MyProcessBehaviour(orb.in()) );
    
    
            // get the repository service
            try 
            {
                CORBA::Object_var obj =
                      PlatformInterface::Get_service(ServiceNames::NAMING_AND_REPOSITORY_SERVICE);
                      
                repositoryRef = CdmwNamingAndRepository::Repository::_narrow(obj.in());
            } 
            catch (const CdmwPlatformMngtService::ServiceNotFound &) 
            {
                // Cannot Proceed with no CDMW Repository Ior
                throw CORBA::BAD_PARAM(BAD_PARAMRepositoryNotFound,
                                       CORBA::COMPLETED_NO);
            } 
            catch (const CdmwPlatformMngtService::ServiceUnavailable &) 
            {
                // Cannot Proceed with no CDMW Repository Ior
                throw CORBA::BAD_PARAM(BAD_PARAMCannotGetRepositoryReference,
                                       CORBA::COMPLETED_NO);
            } 
            catch (const CORBA::SystemException & ex) 
            {
                throw;
            } 
            catch (const Cdmw::BadOrderException & ex) 
            {
                throw;
            } 
            catch (const Cdmw::BadParameterException & ex) 
            {
                throw;
            }

        }
        // else process not started by PlatformManagement
        else {
            // get repository from url
            try {
                CORBA::Object_var obj =
                    orb->string_to_object(namingReposURL.c_str());
                repositoryRef =
                    CdmwNamingAndRepository::Repository::_narrow(obj.in());
            }
            catch (...) {
                std::cerr << "Invalid Naming & Repository URL" << std::endl;
                throw;
            }
        }

        if (CORBA::is_nil(repositoryRef.in())) {
            throw CORBA::BAD_PARAM(BAD_PARAMInvalidRepositoryReference,
                                   CORBA::COMPLETED_NO);
        }

        // Init the process
        std::vector<std::string> collectorNames;
        status = process_init(orb.in(), rootPOA.in(), collectorName,
                              traceFileName,
                              componentName, // ECR-0123
                              domainName, levelValue,
                              traceFormat,
                              repositoryRef.in(),
                              traceProducerContext,
                              traceProducerInx, argc, argv,
                              /* ECR-0145 traceCollectorDom.out(), */
                              queueStrategy,
                              queueSize,
                              queueStringSize,
                              traceFileBkpNbr,
                              traceFileNbr,
                              traceMessageNbr,
                              collectorNames);

        if (status == SUCCESS) {
            struct sigaction action;
            action.sa_handler=exit_handler;
            sigemptyset(&action.sa_mask);
            sigaction(SIGTERM, &action, NULL);
            sigaction(SIGINT, &action, NULL);
            sigaction(SIGQUIT, &action, NULL);
            sigaction(SIGABRT, &action, NULL);

            // run processing
            status = run(orb.in(), interactive_mode);
        }

        // end of process
        process_end(collectorName/* ECR-0145 , traceCollectorDom.in()*/);

        if (platformManaged) 
        {
            // cleanup the platform interface
            PlatformInterface::Cleanup();
        }
    }
    catch (const Cdmw::Exception& ex) {
        std::cerr << ex.what() << std::endl;
        status = FAILURE;
    }
    catch(const CORBA::Exception& ex) {
        std::cerr << ex << std::endl;
        status = FAILURE;
    }
    catch (...) {
        std::cerr << "unknown exception in main" << std::endl;
        status = FAILURE;
    }

    // end of corba processing

    if(!CORBA::is_nil(orb.in())) {
        try {
            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
            orb->destroy();
        }
        catch(const CORBA::Exception& ex) {
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }

    // if program arguments table has been rebuilt
    if (arg_rebuilt) {
        // release any allocated memory
        for (int i=arg_ini_inx ; i < myArgc; ++i) {
            delete[] myArgv[i];
        }

        delete[] myArgv;
    }

    return status;
}
