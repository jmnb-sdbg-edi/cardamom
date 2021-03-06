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


#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/common/System.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Foundation/commonsvcs/naming/NamingUtil.hpp>
// #include <Foundation/logging/LogManager.hpp> // ECR-0169
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>

#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include <TraceAndPerf/tracelibrary/FlushAreaMngr.hpp>

// #include "tracelibrary/LogManagerTraceDelegate.hpp" // ECR-0169

#include "test1/ClientThread.hpp"
#include "test1/TargetThread.hpp"


namespace {
    const int SUCCESS = 0;
    const int FAILURE = 1;

    const int POA_THREAD_POOL_SIZE = 5;

    const std::string APPLICATION_NAME_OPTION = "--application-name";
    const std::string NAMING_REPOS_URL_OPTION = "--namingRepos-url";
    const std::string INTERACTIVE_OPTION      = "--interactive";

    const std::string COLLECTOR_INI_MNEMONAME = "collector_ini_";

    // Max number of target thread
    const int max_target = 2;


    /**
     * Print the usage.
     */
    static void
    usage(std::ostream& os, const std::string& program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                    Display this information.\n"
           << "--interactive             Start in interactive mode.\n"
           << "\n"
           << "--application-name=<name> Name of application.\n"
           << "--namingRepos-url=<URL>   "
              "URL of the CDMW Naming and Repository service.\n"
           << std:: endl;
    }


    /**
     * Get Cdmw repository object pointer from URL passed as program argument.
     */
    CdmwNamingAndRepository::Repository_ptr
    get_cdmw_repository(CORBA::ORB_ptr orb, const std::string &namingReposURL)
    {
        CDMW_ASSERT(!CORBA::is_nil(orb));

        CORBA::Object_var repositoryObj = CORBA::Object::_nil();

        // get the naming service.
        CdmwNamingAndRepository::Repository_var repositoryRef =
                                  CdmwNamingAndRepository::Repository::_nil();

        try {
            repositoryObj = orb->string_to_object(namingReposURL.c_str());
        }
        catch(...) {
            std::cerr << "Invalid Naming & Repository URL" << std::endl;
        }

        if(CORBA::is_nil(repositoryObj.in())) {
            std::cerr << "Naming & Repository URL denotes "
                      << "a nil object reference" << std::endl;
        }

        try {
            // Get reference to the repository interface
            repositoryRef =
                CdmwNamingAndRepository::Repository::_narrow(
                    repositoryObj.in());

            if (CORBA::is_nil(repositoryRef.in())) {
                std::cerr << "Couldn't retrieve reference of the Repository"
                          << "\n(file:" << __FILE__
                          << " line:" << __LINE__ << ")" << std::endl;
            }
        }
        catch (...) {
            std::cerr << "Couldn't retrieve reference of the Repository"
                      << "\n(file:" << __FILE__
                      << " line:" << __LINE__ << ")" << std::endl;
        }

        return repositoryRef._retn();
    }
}; // anonymous namespace


/**
 * Run the client application.
 */
int
run(CORBA::ORB_ptr orb,
    int& argc,
    char** argv,
    CdmwNamingAndRepository::Repository_ptr pRepository,
    bool interactive_mode)
{
    int status = SUCCESS;

    if (!interactive_mode) {
        // Do all the tests
        std::string command("start_stop_targets\n"
                            "sleep_process 10000\n"
                            "x\n");

        // convert string command to stream
        std::istringstream is(command);

        // create client thread object
        Cdmw::Trace::ClientThread client(orb, is, std::cout, pRepository);

        // start client and run orb
        client.start();
        orb->run();

        // wait end of client thread after corba loop stopping
        client.join();
    }
    else {
        // Enter interactive mode
        Cdmw::Trace::ClientThread client(orb,
                                         std::cin,
                                         std::cout,
                                         pRepository);

        // start client and run orb
        client.start();
        orb->run();

        // wait end of client thread after corba loop stopping
        client.join();
    }

    return status;
}


/**
 * Initialize the process.
 */
int
process_init(CORBA::ORB_ptr orb,
             const std::string& applicationName,
             const std::string& processName,
             const std::string& namingReposURL,
             CdmwNamingAndRepository::Repository_ptr& pRepository,
             std::vector<std::string>& collectorNames)
{
    int status = FAILURE;

    using namespace Cdmw::Common;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::CommonSvcs::Naming;
    using namespace Cdmw::Trace;

    CdmwTrace::TraceProducer_var traceProducerRef =
        CdmwTrace::TraceProducer::_nil();

    CdmwNamingAndRepository::Repository_var rep =
        CdmwNamingAndRepository::Repository::_nil();

    // std::string collectorDomainStr;
    std::string collectorNamingStr;

    try {
        try {
            // Retrieve Repository object reference
            rep = get_cdmw_repository(orb, namingReposURL);

            /* ECR-0145
            // set trace collector location under default root context
            // Pattern is :init_trace_library
            //    CDMW/SERVICES/TRACE/COLLECTORS/"
            collectorDomainStr = Locations::CDMW_SERVICES_NAME_DOMAIN;
            collectorDomainStr += "/";
            collectorDomainStr += Locations::TRACE_COLLECTORS_NAME_DOMAIN;

            // Check that collector domain name is really a Name Domain
            CdmwNamingAndRepository::NameDomain_var collectorDomain =
                rep->resolve_name_domain(collectorDomainStr.c_str());

            // get default root context
            CosNaming::NamingContext_var nc_root_context =
                rep->resolve_root_context(
                    CdmwNamingAndRepository::DEFAULT_ROOT_CONTEXT);

            // It is a name domain, so we should be able to get its naming
            // context.
            NamingInterface ni_root(nc_root_context.in());

            // create Collector domain naming context
            CosNaming::NamingContext_var nc_collector_dom =
                CosNaming::NamingContext::_nil();

            typedef NamingUtil<CosNaming::NamingContext> Util;
            nc_collector_dom = Util::resolve_name(ni_root,
                                                  collectorDomainStr);
            */

            CosNaming::NamingContext_var adminRootCtx =
                CosNaming::NamingContext::_nil();
            try {
                adminRootCtx =
                    rep->resolve_root_context(Locations::ADMIN_ROOT_CONTEXT_ID);
            }
            catch (const CdmwNamingAndRepository::Repository::NoRootContext&) {
                std::cerr << "AdminRootContext does not exist" << std::endl;
            }

            NamingInterface ni_root(adminRootCtx.in());

            CosNaming::Name collector_nc_;
            collector_nc_.length(4);
            collector_nc_[0].id =
                CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
            collector_nc_[1].id = CORBA::string_dup("SERVICES");
            collector_nc_[2].id = CORBA::string_dup("TRACE");
            collector_nc_[3].id = CORBA::string_dup("COLLECTORS");

            CosNaming::NamingContext_var collector_nc =
                CosNaming::NamingContext::_nil();

            typedef NamingUtil<CosNaming::NamingContext> Util;
            collector_nc = Util::resolve_name(ni_root,
                                              ni_root.to_string(collector_nc_));

            // create the initial collector list.
            std::list<CdmwTrace::TraceProducer::CollectorData> collectorList;

            CosNaming::BindingIterator_var bindingItr;
            CosNaming::BindingList_var bindingList;

            /* ECR-0145
            NamingInterface ni_collector(nc_collector_dom.in());

            // All binding in this list are Collector, so we try to get all
            */

            NamingInterface ni_collector(collector_nc.in());

            ni_collector.list(NamingInterface::LIST_CHUNK_SIZE,
                              bindingList,
                              bindingItr);

            // We stop when there is no more binding
            bool isMoreBinding = false;
            int collector_ix = 0;

            do {
                for (size_t iBinding = 0;
                     iBinding < bindingList->length();
                     ++iBinding)
                {
                    CosNaming::Binding binding = bindingList[iBinding];

                    // We call each Collector,
                    // and we notify them of our creation
                    std::string collectorName =
                        ni_collector.to_string(binding.binding_name);

                    collectorNames.push_back(collectorName);

                    CORBA::Object_var obj =
                        ni_collector.resolve(collectorName);
                    CdmwTrace::Collector_var collector =
                        CdmwTrace::Collector::_narrow(obj.in());

                    // create collector mnemonic name
                    std::ostringstream mnemo_name_str;
                    mnemo_name_str << COLLECTOR_INI_MNEMONAME
                                   << collector_ix;
                    collector_ix++;

                    // create collector data structure
                    CdmwTrace::TraceProducer::CollectorData collector_data;
                    collector_data.the_collectorMnemoName =
                        CORBA::string_dup(mnemo_name_str.str().c_str());
                    collector_data.the_collectorObjName =
                        CORBA::string_dup(collectorName.c_str());
                    // (collector is a _var, _duplicate is implicit)
                    collector_data.the_collector = collector;

                    // insert collector data in list
                    collectorList.insert(collectorList.end(),
                                         collector_data);
                }

                // If the iterator is not NULL, there is more binding
                // to get back.
                if (!CORBA::is_nil(bindingItr.in())) {
                    isMoreBinding =
                        bindingItr->next_n(NamingInterface::LIST_CHUNK_SIZE,
                                           bindingList);
                }
            } while (isMoreBinding);

            // get a reference to the root POA
            CORBA::Object_var rootObj =
                orb->resolve_initial_references("RootPOA");
            PortableServer::POA_var rootPOA =
                PortableServer::POA::_narrow(rootObj.in());

            // create and initialize the FlushArea manager
            traceProducerRef =
                FlushAreaMngr::init(rootPOA.in(),
                                    collectorList,
                                    CdmwTrace::ALL_COMPONENT_NAMES, // ECR-0123
                                    CdmwTrace::ALL_DOMAINS,
                                    CdmwTrace::ALL_VALUES,
                                    applicationName,
                                    processName);

            // export the object reference to a file
            CORBA::String_var ref_string =
                orb->object_to_string(traceProducerRef.in());
            std::ofstream os("traceLibrary.ior");
            os << ref_string.in();
            os.close();

            // activate the POA Manager
            PortableServer::POAManager_var POAMngr = rootPOA->the_POAManager();
            POAMngr->activate();

            status = SUCCESS;
        }
        catch (const CosNaming::NamingContext::NotFound&) {
            // should not happen!
            std::cerr << "Trace Collector Name Domain not found \n"
                      // << collectorDomainStr.c_str()
                      << collectorNamingStr.c_str()
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::Repository::NoRootContext&) {
            std::cerr << "Trace Collector Root Context does not exist"
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::NoNameDomain&) {
            std::cerr << "Trace Collector Name Domain does not exist \n"
                      // << collectorDomainStr.c_str()
                      << collectorNamingStr.c_str()
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::InvalidName&) {
            std::cerr << "Trace Collector Name Domain has illegal form \n"
                      // << collectorDomainStr.c_str()
                      << collectorNamingStr.c_str()
                      << std::endl;
        }
        catch (const CosNaming::NamingContext::InvalidName&) {
            std::cerr << "Trace Collector Name invalid \n"
                      // << collectorDomainStr.c_str()
                      << collectorNamingStr.c_str()
                      << std::endl;
        }
        catch (const Cdmw::Common::TypeMismatchException& e) {
            std::cerr << "TypeMismatchException (Not a NamingContext) <"
                      << e.what() << ">" << std::endl;
        }

        // Register TraceProducer object

        if (status == SUCCESS)
        {
            status = FAILURE;

            const std::string hostname(
                Cdmw::CommonSvcs::Naming::NamingInterface::addEscape(
                    Cdmw::OsSupport::OS::get_hostname()));

            // std::string trace_producer_name;
            std::string trace_producer_name(hostname);

            // set TraceProducer location under admin root context
            // Pattern is :
            // hostname/appliName/procName/TRACE/TraceProducer

            // Build the name of the Trace Producer object
            // trace_producer_name = Cdmw::OsSupport::OS::get_hostname();
            trace_producer_name += "/";
            trace_producer_name += applicationName;
            trace_producer_name += "/";
            trace_producer_name += processName;
            trace_producer_name += "/TRACE/TraceProducer";

            // get the "AdminRootContext" root context and then
            // register TraceProducer object.
            CosNaming::NamingContext_var adminRootCtx =
                CosNaming::NamingContext::_nil();
            try {
                adminRootCtx =
                    rep->resolve_root_context(Locations::ADMIN_ROOT_CONTEXT_ID);
            }
            catch (const CdmwNamingAndRepository::Repository::NoRootContext&) {
                std::cerr << "Trace Producer Root Context does not exist"
                          << std::endl;
            }

            try {
                // create naming interface on admin root context.
                NamingInterface ni(adminRootCtx.in());

                // if object already registered, force its registration.
                ni.bind(trace_producer_name, traceProducerRef.in(), true);
            }
            catch (const CosNaming::NamingContext::NotFound&) {
                // should not happen!
                std::cerr << "Trace Producer Name context not found \n"
                          << trace_producer_name.c_str() << std::endl;
            }
            catch (const CosNaming::NamingContext::InvalidName&) {
                std::cerr << "Trace Producer Name invalid \n"
                          << trace_producer_name.c_str()
                          << std::endl;
            }
            catch (const CosNaming::NamingContext::AlreadyBound&) {
                // Should not happen because force is set to true!
                std::cerr << "Trace Producer already bound" << std::endl;
            }

            std::cout << "traceProducer is registered in "
                      << trace_producer_name.c_str() << std::endl;

            // export the repository
            pRepository = rep._retn();

            status = SUCCESS;
        }
    }
    catch (const std::bad_alloc&) {
        std::cerr << "Process Initialisation: Bad Allocation Exception\n" ;
    }
    catch (const CosNaming::NamingContext::CannotProceed&) {
        // should not happen!
        std::cerr << "Unexpected Error (CannotProceed exception)!"
                  << std::endl;
    }
    catch (const Cdmw::Exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
    catch (const CORBA::SystemException& ex) {
        std::cerr << ex << std::endl;
    }
    catch(const CORBA::Exception& ex) {
        std::cerr << ex << std::endl;
    }

    return status;
}

void process_end(const std::string &applicationName,
                 const std::string &processName,
                 const CdmwNamingAndRepository::Repository_ptr &pRepository,
                 const std::vector<std::string>& collectorNames)
{
    using namespace Cdmw::Common;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::CommonSvcs::Naming;
    using namespace Cdmw::Trace;

    // clean up the flush area manager
    Cdmw::Trace::FlushAreaMngr::cleanup();

    try {
        /* ECR-0145
        // set TraceProducer location under admin root context
        // Pattern is :
        // "<host_name>/<application_name>/<process_name>/TRACE/TraceProducer"

        // set the name of the Trace Producer context
        std::string trace_producer_name = processName;

        // Build the name of the Trace Producer object
        // trace_producer_name = Cdmw::OsSupport::OS::get_hostname();
        trace_producer_name += "/";
        trace_producer_name += applicationName;
        trace_producer_name += "/";
        trace_producer_name += processName;
        trace_producer_name += "/TRACE/TraceProducer";
        */

        CosNaming::Name producerName_;
        producerName_.length(5);
        producerName_[0].id =
            CORBA::string_dup(
                Cdmw::OsSupport::OS::get_hostname().c_str());
        producerName_[1].id = CORBA::string_dup(applicationName.c_str());
        producerName_[2].id = CORBA::string_dup(processName.c_str());
        producerName_[3].id = CORBA::string_dup("TRACE");
        producerName_[4].id = CORBA::string_dup("TraceProducer");
        std::string trace_producer_name;

        // get the "AdminRootContext" root context.
        CosNaming::NamingContext_var adminRootCtx =
            CosNaming::NamingContext::_nil();

        try {
            adminRootCtx =
                pRepository->resolve_root_context(
                    Locations::ADMIN_ROOT_CONTEXT_ID);

            // create naming interface on admin root context.
            NamingInterface ni(adminRootCtx.in());

            // remove the trace producer binding
            trace_producer_name = ni.to_string(producerName_);
            ni.unbind(trace_producer_name);
        }
        catch (const CdmwNamingAndRepository::Repository::NoRootContext&) {
            std::cerr << "Trace Producer Root Context does not exist"
                      << std::endl;
        }
        catch (const CosNaming::NamingContext::NotFound&) {
            std::cerr << "Trace Producer Name context not found \n"
                      << trace_producer_name.c_str()
                      << std::endl;
        }
        catch (const CosNaming::NamingContext::CannotProceed&) {
            std::cerr << "Trace Producer Name context cannot proceed"
                      << std::endl;
        }
        catch (const CosNaming::NamingContext::InvalidName&) {
            std::cerr << "Trace Producer Name invalid \n"
                      << trace_producer_name.c_str()
                      << std::endl;
        }

        // ECR-0145
        std::vector<std::string>::const_iterator it = collectorNames.begin();
        for (; it != collectorNames.end(); ++it) {
            CosNaming::Name collectorName_;
            collectorName_.length(5);
            collectorName_[0].id =
                CORBA::string_dup(
                    Cdmw::OsSupport::OS::get_hostname().c_str());
            collectorName_[1].id = CORBA::string_dup("SERVICES");
            collectorName_[2].id = CORBA::string_dup("TRACE");
            collectorName_[3].id = CORBA::string_dup("COLLECTORS");
            collectorName_[4].id = CORBA::string_dup((*it).c_str());

            std::string collectorName;

            try {
                // create naming interface on admin root context.
                NamingInterface ni(adminRootCtx.in());

                // remove the trace producer binding.
                collectorName = ni.to_string(collectorName_);
                ni.unbind(collectorName);
            }
            catch (const CdmwNamingAndRepository::Repository::NoRootContext&) {
                std::cerr << "no root context" << std::endl;
            }
            catch (const CosNaming::NamingContext::NotFound&) {
                std::cerr << "trace collector name context not found"
                          << std::endl << collectorName << std::endl;
            }
            catch (const CosNaming::NamingContext::CannotProceed&) {
                std::cerr << "trace collector name context cannot proceed"
                          << std::endl;
            }
            catch (const CosNaming::NamingContext::InvalidName&) {
                std::cerr << "invalid trace collector name" << std::endl
                          << collectorName << std::endl;
            }
        }
    }
    catch (const std::bad_alloc&) {
        std::cerr << "Process End : Bad Allocation Exception \n" ;
    }
    catch (const Cdmw::Exception &ex) {
        std::cerr << ex.what() << std::endl;
    }
    catch (const CORBA::SystemException& ex) {
        std::cerr << ex << std::endl;
    }
    catch(const CORBA::Exception &ex) {
        std::cerr << ex << std::endl;
    }

}


void exceptionHandler(void)
{
    std::cout << "Trace Admin : UNEXPECTED EXCEPTION HANDLER" << std::endl;
}


int main(int argc, char* argv[])
{
#ifndef _MSC_VER
    std::set_unexpected(&exceptionHandler);

    // VC++ contrary to the clause 18.6 of the C++ standard,
    // set_unexpected, is not inside the std:: namespace
#else
    set_unexpected(&exceptionHandler);
#endif

    using namespace Cdmw::OsSupport;

    // get arguments
    if(argc == 1) {
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    // help argument
    if(strcmp(argv[1], "--help") == 0) {
        usage(std::cout, argv[0]);
        return SUCCESS;
    }

    std::string allTests = OS::get_option_value(argc, argv, INTERACTIVE_OPTION);

    // does it use interactive mode?
    bool interactive_mode = false;
    if (allTests != "no") {
        interactive_mode = true;
    }

    // set Process Name
    std::string processName = argv[0];

    // suppress process absolute path
    std::string::iterator begin = processName.begin();
    int i = processName.rfind('/') + 1;

    std::string::iterator j = begin + i;
    processName.erase(begin,j);

    // get Application Name
    std::string applicationName =
        OS::get_option_value(argc, argv, APPLICATION_NAME_OPTION);
    if (applicationName == "no") {
        std::cout << APPLICATION_NAME_OPTION.c_str()
                  << "=<name> option must be defined"
                  << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    // get Naming & Repository URL argument
    std::string namingReposURL =
        OS::get_option_value(argc, argv, NAMING_REPOS_URL_OPTION);
    if (namingReposURL == "no") {
        std::cout << NAMING_REPOS_URL_OPTION.c_str()
                  << "=<URL> option must be defined"
                  << std::endl;
        usage(std::cout,argv[0]);
        return FAILURE;
    }


    int status = SUCCESS;

    // ORB reference
    CORBA::ORB_var orb;

    // Name and Repository
    CdmwNamingAndRepository::Repository_var repositoryRef =
        CdmwNamingAndRepository::Repository::_nil();

    try {
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc,
                                                     argv,
                                                     orb_strategies);

        std::vector<std::string> collectorNames;
        status = process_init(orb.in(),
                              applicationName,
                              processName,
                              namingReposURL,
                              repositoryRef.out(),
                              collectorNames);
            
//     // ECR-0169
//     // initialize the log manager. Done in  orb_init()
//             //    Cdmw::Logging::LogManager::Init(argc, argv); 
//     // register the log manager delegate.
//     Cdmw::Logging::LogManager* logMngr = Cdmw::Logging::LogManager::Instance();
//     Cdmw::Trace::LogManagerTraceDelegate* alt =
//         new Cdmw::Trace::LogManagerTraceDelegate(Cdmw::Logging::INFO);

        if (status == SUCCESS) {
//             logMngr->register_alternate_delegate(alt);

            // run processing
            status = run(orb.in(),
                         argc,
                         argv,
                         repositoryRef.in(),
                         interactive_mode);
        }

        // end of process
        if (status == SUCCESS) {
//             logMngr->unregister_alternate_delegate(alt);
            process_end(applicationName,
                        processName,
                        repositoryRef.in(),
                        collectorNames);
        }

//         delete alt; // ECR-0169
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

    return status;
}
