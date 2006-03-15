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

#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/System.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>

#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include <SystemMngt/platforminterface/PlatformInterface.hpp>
#include <SystemMngt/platforminterface/ProcessBehaviour.hpp>
#include <SystemMngt/platforminterface/ServiceNames.hpp>

#include "tracecollector/ClientThread.hpp"
#include "tracecollector/GlobalCollectorMngr.hpp"
#include "tracecollector/ProcessAgent_impl.hpp"


namespace
{
    using std::string;

    // default ports used by the global collector.
    const string DFLT_SERVICE_PORT = "52345";
    const string DFLT_PROCESS_PORT = "52356";

    const string INTERACTIVE_OPT         = "--interactive";
    const string COLLECTOR_NAME_OPT      = "--collector-name";
    const string TRACE_FILE_NAME_OPT     = "--trace-file-name";
    const string COMPONENT_NAME_OPT      = "--component-name";
    const string DOMAIN_NAME_OPT         = "--domain-name";
    const string LEVEL_OPT               = "--level";
    const string NAMING_REPOS_URL_OPT    = "--namingRepos-url";
    const string TRACE_Q_STRATEGY_OPT    = "--queue-strategy";
    const string TRACE_Q_STRATEGY_VAL_DL = "drop_latest";
    const string TRACE_Q_STRATEGY_VAL_RO = "remove_oldest";
    const string TRACE_Q_SIZE_OPT        = "--queue-size";
    const string TRACE_Q_STRING_SIZE_OPT = "--queue-str-size";
    const string TRACE_FILE_BAK_NUM_OPT  = "--trace-file-bak-num";
    const string TRACE_FILE_NUM_OPT      = "--trace-file-num";
    const string TRACE_MSG_NUM_OPT       = "--trace-msg-num";
    const string TRACE_FORMAT_OPT        = "--trace-format";
    const string TRACE_FORMAT_VAL_V      = "V";
    const string TRACE_FORMAT_VAL_H      = "H";

    const string AGENT_CORBALOC_ID       = "GlobalCollectorAgent";

    const unsigned short SUCCESS = 0;
    const unsigned short FAILURE = 1;

    const unsigned short POA_THREAD_POOL_SIZE = 5;


    /**
     * Print the usage.
     *
     * @param out the output stream.
     * @param programName the program name.
     */
    static void
    usage(std::ostream& out, const string& programName)
    {
        using std::endl;
        using Cdmw::Common::Options;

        out << "usage: " << programName << " options..." << endl << endl
            << "options:" << endl
            << "  --help                             "
            << "Display the help." << endl
            << "  --interactive                      "
            << "Start in interactive mode." << endl << endl
            << "  --collector-name=NAME              "
            << "Name of the trace collector." << endl
            << "  [--trace-file-name=NAME]           "
            << "Name of the trace file." << endl
            << "  [--component-name=NAME]            "
            << "Filter on this component name." << endl
            << "  [--domain-name=NAME]               "
            << "Filter on this domain." << endl
            << "  [--level=NUM]                      "
            << "Filter on this filter." << endl
            << "  [--trace-format=<H|V>]             "
            << "Format of a trace: Horizontal or Vertical (default)." << endl
            << "  [--cdmw-localisation-service=PORT] "
            << "Service port of the trace collector." << endl
            << "  [--cdmw-process-port=PORT]         "
            << "Process port of the trace collector." << endl
            << "  [" << Options::CALLBACK_IOR_OPTION << "=IOR]            "
            << "IOR of the ProcessCallback object." << endl
            << "  [--namingRepos-url=URL]           "
            << "URL of the CDMW Naming and Repository service." << endl
            << "  [--queue-strategy=NAME]            "
            << "Strategy \"drop_latest\" or \"remove_oldest\"." << endl
            << "  [--queue-size=NUM]                 "
            << "Size of the queue in KBytes." << endl
            << "  [--queue-str-size=NUM]             "
            << "Size of the string queue in KBytes." << endl
            << "  [--trace-file-bak-num=NUM]         "
            << "Number of backup(s) of trace files." << endl
            << "  [--trace-file-num=NUM]             "
            << "Number of circular trace files." << endl
            << "  [--trace-msg-num=NUM]              "
            << "Number of messages in the circular trace files." << endl;
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
} // anonymous namespace


using namespace Cdmw;
using namespace Cdmw::OsSupport;


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
 * - create the GlobalCollectorMngr object:
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
process_init(CORBA::ORB_ptr orb,
             PortableServer::POA_ptr rootPOA,
             const std::string& collectorName,
             const std::string& traceFileName,
             const std::string& componentName,
             const std::string& domainName,
             long levelValue,
             Cdmw::Trace::Message::TraceFormat traceFormat,
             CdmwNamingAndRepository::Repository_ptr repositoryRef,
             int argc,
             char* argv[],
             Cdmw::Trace::Message::CircularMode queueStrategy,
             long queueSize,
             long queueStrSize,
             long traceFileBakNum,
             long traceFileNum,
             long traceMsgNum)
{
    int status = FAILURE;

    using Cdmw::Common::Locations;
    using namespace Cdmw::OrbSupport;

    CdmwTrace::Collector_var collectorRef = CdmwTrace::Collector::_nil();
    TraceCollector::ProcessAgent_var agentRef =
        TraceCollector::ProcessAgent::_nil();

    try {
        // create a new POA.
        // the trace collector framework uses the following POA policies.
        //
        // Lifespan policy            = PERSISTENT
        // Id Assignment policy       = USER_ID
        // Id Uniqueness policy       = UNIQUE_ID
        // Implicit Activation policy = NO_IMPLICIT_ACTIVATION
        // Request Processing policy  = USE_ACTIVE_OBJECT_MAP_ONLY;
        // Servant Retention policy   = RETAIN;

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

        PortableServer::POAManager_var poaManager = rootPOA->the_POAManager();
        PortableServer::POA_var collectorsPOA = PortableServer::POA::_nil();

        collectorsPOA =
            Cdmw::OrbSupport::OrbSupport::create_POA(
                    rootPOA,
                    collectorName.c_str(),
                    poaManager.in(),
                    policies,
                    poaStrategies
            );

        // create and initialize the collector manager.
        collectorRef =
            Cdmw::Trace::GlobalCollectorMngr::Init(
                    collectorsPOA.in(),
                    componentName,
                    domainName,
                    levelValue,
                    traceFormat,
                    queueStrategy,
                    queueSize,
                    queueStrSize,
                    traceFileName,
                    traceFileBakNum,
                    traceFileNum,
                    traceMsgNum,
                    Cdmw::OsSupport::OS::LOCAL_TIME);

        // export the object reference to a file.
        CORBA::String_var ref_string =
            orb->object_to_string(collectorRef.in());
        std::ofstream os("traceCollector.ior");
        os << ref_string.in();
        os.close();

        // create the ProcessAgent Servant.
        Cdmw::Trace::ProcessAgent_impl* pProcessAgentServant =
            new Cdmw::Trace::ProcessAgent_impl(orb, rootPOA);
        PortableServer::ServantBase_var servant_var = pProcessAgentServant;

        // activate servant on POA.
        PortableServer::ObjectId_var oid =
            rootPOA->activate_object(pProcessAgentServant);
        CORBA::Object_var object = rootPOA->id_to_reference(oid.in());
        agentRef = TraceCollector::ProcessAgent::_narrow(object.in());

        status = SUCCESS;

        CdmwNamingAndRepository::NameDomain_var collectorDomain;

        // register the collector object in the Naming & repository
        // under DefaultRootContext.
        // trace collector location:
        // CDMW/SERVICES/TRACE/COLLECTORS/collectorName

        try {
            if (status == SUCCESS) {
                status = FAILURE;

                // get the collector domain.
                std::string collectorDomainStr(
                    Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN);
                collectorDomainStr += "/";
                collectorDomainStr +=
                    Cdmw::Common::Locations::TRACE_COLLECTORS_NAME_DOMAIN;

                collectorDomain =
                    repositoryRef->resolve_name_domain(
                        collectorDomainStr.c_str());

                status = SUCCESS;
            }
        }
        catch (const CdmwNamingAndRepository::NoNameDomain&) {
            std::cerr << "the trace collector name domain does not exist"
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::InvalidName&) {
            std::cerr << "the trace collector name domain is ill-formed"
                      << std::endl;
        }

        try {
            if (status == SUCCESS) {
                status = FAILURE;

                // register the object in the trace collector domain.
                collectorDomain->register_new_object(collectorName.c_str(),
                                                     collectorRef.in());

                status = SUCCESS;
            }
        }
        catch (const CdmwNamingAndRepository::InvalidName&) {
            std::cerr << "the trace collector name is ill-formed"
                      << std::endl;
        }
        catch (const CdmwNamingAndRepository::NameDomain::AlreadyExists&) {
            std::cerr << "the collector object already registered in "
                      << "the repository" << std::endl;
            // the process must continue in this case.
            status = SUCCESS;
        }
        catch (const CORBA::BAD_PARAM&) {
            std::cerr << "attempt to register a nil object" << std::endl;
        }

        if (status == SUCCESS) {
            status = FAILURE;

            // bind the ProcessAgent to a corbaloc name.
            Cdmw::OrbSupport::OrbSupport::bind_object_to_corbaloc(
                    orb,
                    AGENT_CORBALOC_ID,
                    agentRef.in());

            status = SUCCESS;
        }
    }
    catch (const std::bad_alloc&) {
        std::cerr << "Process initialisation: bad allocation exception"
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


/**
 * Process termination.
 */
void
process_end(const std::string& collectorName)
{
    Cdmw::Trace::GlobalCollectorMngr::Cleanup();
}


/**
 * Unexpected exception handler.
 */
void
exceptionHandler(void)
{
    std::cout << "Global trace collector: UNEXPECTED EXCEPTION RAISED"
              << std::endl;
}


/**
 * Main.
 */
int
main(int argc, char* argv[])
{
    ::setlocale(LC_ALL, "C");

#ifndef _MSC_VER
    // in VC++ contrary to the clause 18.6 of the C++ standard,
    // set_unexpected() is not defined within the std namespace.
    std::set_unexpected(&exceptionHandler);
#else
    set_unexpected(&exceptionHandler);
#endif

    // check the number of arguments.
    if (argc == 1) {
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    // --help
    if (strcmp(argv[1], "--help") == 0) {
        usage(std::cout, argv[0]);
        return SUCCESS;
    }

    using Cdmw::OsSupport::OS;

    // --interactive
    std::string allTests(OS::get_option_value(argc, argv, INTERACTIVE_OPT));
    bool interactive_mode = false;
    if (allTests != "no") {
        interactive_mode = true;
    }

    // get the trace collector name.
    std::string collectorName(
        OS::get_option_value(argc, argv, COLLECTOR_NAME_OPT));
    if (collectorName == "no" || collectorName == "yes") {
        std::cout << COLLECTOR_NAME_OPT << "=NAME option must be defined"
                  << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    // get the trace file name.
    std::string traceFileName(
        OS::get_option_value(argc, argv, TRACE_FILE_NAME_OPT));
    if (traceFileName == "no") {
        traceFileName = "";
    }
    else if (traceFileName == "yes") {
        std::cout << TRACE_FILE_NAME_OPT << "=NAME option must be defined"
                  << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    // get the name of the component to filter on.
    std::string componentName(
        OS::get_option_value(argc, argv, COMPONENT_NAME_OPT));
    if (componentName == "no") {
        componentName = CdmwTrace::ALL_COMPONENT_NAMES;
    }
    else if (componentName == "yes") {
        std::cout << "missing value for the "
                  << COMPONENT_NAME_OPT << " option" << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    // get the name of the domain to filter on.
    std::string domainName(OS::get_option_value(argc, argv, DOMAIN_NAME_OPT));
    if (domainName == "no") {
        domainName = CdmwTrace::ALL_DOMAINS;
    }
    else if (domainName == "yes") {
        std::cout << DOMAIN_NAME_OPT << "=NAME option must be defined"
                  << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    // get the level to filter on.
    std::string levelOption(OS::get_option_value(argc, argv, LEVEL_OPT));
    long levelValue;
    if (levelOption == "no") {
        levelValue = CdmwTrace::ALL_VALUES;
    }
    else {
        std::istringstream levelOption_strm(levelOption);
        levelOption_strm >> levelValue;
        if (levelOption_strm.fail()) {
            std::cout << "bad value for the " << LEVEL_OPT << "=NUM option"
                      << std::endl;
            return FAILURE;
        }
    }

    // get the format of a trace (default: vertical).
    Cdmw::Trace::Message::TraceFormat traceFormat =
        Cdmw::Trace::Message::V_FORMAT;
    std::string formatOption(
        OS::get_option_value(argc, argv, TRACE_FORMAT_OPT));
    if (formatOption != "no") {
        if (formatOption == TRACE_FORMAT_VAL_V) {
            traceFormat = Cdmw::Trace::Message::V_FORMAT;
        }
        else if (formatOption == TRACE_FORMAT_VAL_H) {
            traceFormat = Cdmw::Trace::Message::H_FORMAT;
        }
        else {
            std::cout << "bad value for the "
                      << TRACE_FORMAT_OPT << " option" << std::endl
                      << "the possible values are : "
                      << TRACE_FORMAT_VAL_V << " or " << TRACE_FORMAT_VAL_H
                      << std::endl;
            return FAILURE;
        }
    }

    // get the trace collector port numbers
    // (these values are passed to ORB_init via ORBsupport::ORB_init).
    bool arg_rebuilt = false;
    char** myArgv = argv;
    int myArgc = argc;
    int arg_ini_inx = argc;
    int arg_inx = argc;
    std::string servicePortOption;
    std::string processPortOption;

    std::string servicePort(
        OS::get_option_value(
            argc,
            argv,
            Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION));
    if (servicePort != "no") {
        int portNbr;

        std::istringstream servicePort_strm(servicePort);
        servicePort_strm >> portNbr;

        if (servicePort_strm.fail()) {
            std::cout << "bad value for the "
                      << Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION
                      << "=PORT option" << std::endl;
            return FAILURE;
        }
    }

    std::string processPort(
        OS::get_option_value(argc,
                             argv,
                             Cdmw::Common::Options::PROCESS_PORT_OPTION));
    if (processPort != "no") {
        int portNbr;

        std::istringstream processPort_strm(processPort);
        processPort_strm >> portNbr;

        if (processPort_strm.fail()) {
            std::cout << "bad value for the "
                      << Cdmw::Common::Options::PROCESS_PORT_OPTION
                      << "=PORT option" << std::endl;
            return FAILURE;
        }
    }

    if (servicePort == "no" || processPort == "no") {
        try {
            // allocate enough place to contain each argument string
            // plus the port option arguments
            // and a NULL string to indicate the end.
            myArgv = new char*[argc + 3];

            for (int i = 0; i < argc; ++i) {
                myArgv[i] = argv[i];
            }

            myArgv[argc] = NULL;
            myArgv[argc + 1] = NULL;
            myArgv[argc + 2] = NULL;
            arg_rebuilt = true;

            // use the default ports if necessary.
            if (servicePort == "no") {
                servicePortOption =
                    Cdmw::Common::Options::LOCALISATION_SERVICE_OPTION;
                servicePortOption += "=" + DFLT_SERVICE_PORT;
                myArgv[arg_inx] = new char[servicePortOption.size() + 1];
                ::strcpy(myArgv[arg_inx], servicePortOption.c_str());
                arg_inx++;
                servicePort = DFLT_SERVICE_PORT;
            }

            if (processPort == "no") {
                processPortOption = Cdmw::Common::Options::PROCESS_PORT_OPTION;
                processPortOption += "=" + DFLT_PROCESS_PORT;
                myArgv[arg_inx] = new char[processPortOption.size() + 1];
                ::strcpy(myArgv[arg_inx], processPortOption.c_str());
                arg_inx++;
                processPort = DFLT_PROCESS_PORT;
            }
        }
        catch (const std::bad_alloc&) {
            std::cout << "bad allocation for new arguments" << std::endl;
            return FAILURE;
        }

        // set number of arguments
        myArgc = arg_inx;
    }

    std::cout << "The global trace collector " << collectorName
              << " is listening on port " << servicePort << std::endl
              << " (" << Cdmw::Common::Options::PROCESS_PORT_OPTION
              << " is set to " << processPort << ")" << std::endl;

    // get the IOR of the Naming & Repository service.

    // we are either launched by the PlatformManagement and in which case
    // we can query for the reference,
    // or the reference is directly passed as command line argument.

    using namespace Cdmw::PlatformMngt;
    bool platformManaged =
        PlatformInterface::Is_launched_by_PlatformManagement(argc, argv);

    // get the Naming & Repository URL.
    std::string namingReposURL(
        OS::get_option_value (argc, argv, NAMING_REPOS_URL_OPT));
    if (namingReposURL == "no") {
        namingReposURL = "";
    }

    if (!platformManaged && namingReposURL == "") {
        std::cout << "the " << Cdmw::Common::Options::CALLBACK_IOR_OPTION
                  << "=IOR option or the " << NAMING_REPOS_URL_OPT
                  << "=URL option must be defined" << std::endl;
        usage(std::cout, argv[0]);
        return FAILURE;
    }

    // get the queue strategy (default: remove_oldest)
    Cdmw::Trace::Message::CircularMode queueStrategy =
        Cdmw::Trace::Message::REMOVE_OLDEST;
    std::string queueStategyOption(
        OS::get_option_value (argc, argv, TRACE_Q_STRATEGY_OPT));
    if (queueStategyOption != "no") {
        if (queueStategyOption == TRACE_Q_STRATEGY_VAL_DL) {
            queueStrategy = Cdmw::Trace::Message::DROP_LATEST;
        }
        else if (queueStategyOption == TRACE_Q_STRATEGY_VAL_RO) {
            queueStrategy = Cdmw::Trace::Message::REMOVE_OLDEST;
        }
        else {
            std::cout << "bad value for the " << TRACE_Q_STRATEGY_OPT
                      << " option" << std::endl
                      << "the possible values are: "
                      << TRACE_Q_STRATEGY_VAL_DL << " or "
                      << TRACE_Q_STRATEGY_VAL_RO << std::endl;
            return FAILURE;
        }
    }

    // get the queue size.
    std::string queueSizeOption(
        OS::get_option_value (argc, argv, TRACE_Q_SIZE_OPT));
    long queueSize;
    if (queueSizeOption == "no") {
        queueSize = 1000; // default to 1000 Kb.
    }
    else {
        std::istringstream queueSizeOption_strm(queueSizeOption);
        queueSizeOption_strm >> queueSize;
        if (queueSizeOption_strm.fail()) {
            std::cout << "bad value for the "
                      << TRACE_Q_SIZE_OPT << "=NUM option"
                      << std::endl;
            return FAILURE;
        }
    }

    // get the queue string size.
    std::string queueStrSizeOption(
        OS::get_option_value (argc, argv, TRACE_Q_STRING_SIZE_OPT));
    long queueStrSize;
    if (queueStrSizeOption == "no") {
        queueStrSize = 100; // default to 100 Kb.
    }
    else {
        std::istringstream queueStrSizeOption_strm(queueStrSizeOption);
        queueStrSizeOption_strm >> queueStrSize;
        if (queueStrSizeOption_strm.fail()) {
            std::cout << "bad value for the "
                      << TRACE_Q_STRING_SIZE_OPT << "=NUM option"
                      << std::endl;
            return FAILURE;
        }
    }

    // get the number of backups.
    std::string traceFileBakNumOption(
        OS::get_option_value (argc, argv, TRACE_FILE_BAK_NUM_OPT));
    long traceFileBakNum;
    if (traceFileBakNumOption == "no") {
        traceFileBakNum = 2; // default to 2 backup files.
    }
    else {
        std::istringstream traceFileBakNumOption_strm(traceFileBakNumOption);
        traceFileBakNumOption_strm >> traceFileBakNum;
        if (traceFileBakNumOption_strm.fail()) {
            std::cout << "bad value for the "
                      << TRACE_FILE_BAK_NUM_OPT << "=NUM option"
                      << std::endl;
            return FAILURE;
        }
    }

    // get the number of circular files.
    std::string traceFileNumOption(
        OS::get_option_value (argc, argv, TRACE_FILE_NUM_OPT));
    long traceFileNum;
    if (traceFileNumOption == "no") {
        traceFileNum = 2; // default to 2 files.
    }
    else {
        std::istringstream traceFileNumOption_strm(traceFileNumOption);
        traceFileNumOption_strm >> traceFileNum;
        if (traceFileNumOption_strm.fail()) {
            std::cout << "bad value for the "
                      << TRACE_FILE_NUM_OPT << "=NUM option"
                      << std::endl;
            return FAILURE;
        }
    }

    // get the number of messages to store in a file.
    std::string traceMsgNumOption(
        OS::get_option_value (argc, argv, TRACE_MSG_NUM_OPT));
    long traceMsgNum;
    if (traceMsgNumOption == "no") {
        traceMsgNum = 50; // default to 50 messages per file.
    }
    else {
        std::istringstream traceMsgNumOption_strm(traceMsgNumOption);
        traceMsgNumOption_strm >> traceMsgNum;
        if (traceMsgNumOption_strm.fail()) {
            std::cout << "bad value for the "
                      << TRACE_MSG_NUM_OPT << "=NUM option"
                      << std::endl;
            return FAILURE;
        }
    }

    int status = SUCCESS;

    CORBA::ORB_var orb;

    try {
        // initialize the ORB.
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(myArgc,
                                                     myArgv,
                                                     orb_strategies);

        // get a reference to the root POA.
        CORBA::Object_var rootObj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var rootPOA =
            PortableServer::POA::_narrow(rootObj.in());

        // activate the POA Manager.
        PortableServer::POAManager_var POAMngr = rootPOA->the_POAManager();
        POAMngr->activate();

        CdmwNamingAndRepository::Repository_var repositoryRef =
            CdmwNamingAndRepository::Repository::_nil();

        // if run from the PlatformManagement then init the PlatformInterface.
        using namespace Cdmw::OrbSupport;
        if (platformManaged) {
            // initialise the platform interface.
            PlatformInterface::Setup(orb.in(), argc, argv);

            // create a process behaviour and
            // acknowledge the creation of the process.
            PlatformInterface::Acknowledge_creation(
                new MyProcessBehaviour(orb.in()));

            // get the repository service.
            try {
                CORBA::Object_var obj =
                    PlatformInterface::Get_service(
                        ServiceNames::NAMING_AND_REPOSITORY_SERVICE);

                repositoryRef =
                    CdmwNamingAndRepository::Repository::_narrow(obj.in());
            }
            catch (const CdmwPlatformMngtService::ServiceNotFound&) {
                throw CORBA::BAD_PARAM(BAD_PARAMRepositoryNotFound,
                                       CORBA::COMPLETED_NO);
            }
            catch (const CdmwPlatformMngtService::ServiceUnavailable&) {
                throw CORBA::BAD_PARAM(BAD_PARAMCannotGetRepositoryReference,
                                       CORBA::COMPLETED_NO);
            }
            catch (const CORBA::SystemException& ex) {
                throw;
            }
            catch (const Cdmw::BadOrderException& ex) {
                throw;
            }
            catch (const Cdmw::BadParameterException& ex) {
                throw;
            }
        }
        else {
            // get the repository from url.
            try {
                CORBA::Object_var obj =
                    orb->string_to_object(namingReposURL.c_str());
                repositoryRef =
                    CdmwNamingAndRepository::Repository::_narrow(obj.in());
            }
            catch (...) {
                std::cerr << "invalid URL for the Naming & Repository "
                          << "service." << std::endl;
                throw;
            }
        }

        if (CORBA::is_nil(repositoryRef.in())) {
            throw CORBA::BAD_PARAM(BAD_PARAMInvalidRepositoryReference,
                                   CORBA::COMPLETED_NO);
        }

        // init the process.
        status = process_init(orb.in(),
                              rootPOA.in(),
                              collectorName,
                              traceFileName,
                              componentName,
                              domainName,
                              levelValue,
                              traceFormat,
                              repositoryRef.in(),
                              argc,
                              argv,
                              queueStrategy,
                              queueSize,
                              queueStrSize,
                              traceFileBakNum,
                              traceFileNum,
                              traceMsgNum);

        if (status == SUCCESS) {
            status = run(orb.in(), interactive_mode);
        }


        // stop the process.
        process_end(collectorName);

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

    // if program arguments table has been rebuilt.
    if (arg_rebuilt) {
        // release any allocated memory
        for (int i=arg_ini_inx ; i < myArgc; ++i) {
            delete[] myArgv[i];
        }

        delete[] myArgv;
    }

    return status;
}
