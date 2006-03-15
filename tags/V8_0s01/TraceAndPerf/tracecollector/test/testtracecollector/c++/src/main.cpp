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
#include <sstream>
#include <string>

#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/String.hpp>
#include <Foundation/common/System.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/testutils/Testable.hpp>

#include "tracecollector/TraceCollectorProcessAgent.stub.hpp"
#include "tracelibrary/Message.hpp"

#include "testtracecollector/TestTraceFile.hpp"


namespace
{
    using std::string;

    const string DEF_NAME_SERVICE_PORT       = "5011";
    const string DEF_REPOSITORY_PORT         = "5021";
    const string DEF_TRC_COLLECTOR_SERV_PORT = "5031";
    const string DEF_TRC_COLLECTOR_CDMW_PORT = "5032";
    const string DEF_GBL_TRC_COLL_SERV_PORT  = "5033";
    const string DEF_GBL_TRC_COLL_CDMW_PORT  = "5034";

    const string NAMING_SERVICE_PORT_OPTION  = "--NameServicePort";
    const string REPOSITORY_PORT_OPTION      = "--RepositoryPort";
    const string COLLECTOR_CDMW_PORT_OPTION  = "--CollectorCdmwPort";
    const string COLLECTOR_SERV_PORT_OPTION  = "--CollectorServicePort";

    const string COLLECTOR_NAME_OPTION       = "--collector-name";
    const string TRACE_FILE_NAME_OPTION      = "--traceFile-name";
    // ECR-0123
    const string COMPONENT_NAME_OPTION       = "--component-name";
    const string DOMAIN_NAME_OPTION          = "--domain-name";
    const string LEVEL_OPTION                = "--level";
    const string TRACE_FILE_BKP_NBR_OPTION   = "--traceFileBkp-number";
    const string TRACE_FILE_NBR_OPTION       = "--traceFile-number";
    const string TRACE_MESSAGE_NBR_OPTION    = "--traceMessage-number";
    const string TRACE_FORMAT_OPTION         = "--trace-format";
    const string TRACE_FORMAT_OPTV           = "V";
    const string TRACE_FORMAT_OPTH           = "H";

    const string COLLECTOR_NAME              = "collector_1";
    const string PRODUCER_NAME               = "producer_1";
    const string TRACE_FILE_NAME             = "test_trace";

    const string GLOBAL_COLLECTOR_NAME       = "global_collector_1";
    const string GLOBAL_TRACE_FILE_NAME      = "test_trace_global";

    const int SUCCESS                        = 0;
    const int FAILURE                        = 1;

    const int POA_THREAD_POOL_SIZE           = 5;

    static void
    usage(std::ostream& os, const string& program_name)
    {
        os << "Usage:\n"
           << program_name << " options\n"
           << "\n"
           << "Options:\n"
           << "--help                            Display this information.\n"
           << "\n"
           << "[--NameServicePort=<nbr>          Port of Name Service.\n"
           << "[--RepositoryPort=<nbr>           Port of Name and Repository.\n"
           << "[--CollectorServicePort=<nbr>     Service Port of Trace Collector.\n"
           << "[--CollectorCdmwPort=<nbr>        Cdmw Port of Trace Collector.\n"
           << "\n"
           << "[--collector-name=<name>]         Name of Trace Collector.\n"
           << "[--traceFile-name=<name>]         Name of Trace File.\n"
           << "[--component-name=<name>]         Filter on this component name.\n" /* ECR-0123 */
           << "[--domain-name=<name>]            Name of domain to filter.\n"
           << "[--level=<value>]                 Level to filter.\n"
           << "[--trace-format=<H|V>]            Format of Trace : Horizontal or Vertical (default).\n"
           << "[--traceBkpFile-number=<number>]  Number of backup of trace files.\n"
           << "[--traceFile-number=<number>]     Number of trace circular files.\n"
           << "[--traceMessage-number=<number>]  Number of trace messages in circular files.\n"
           << std:: endl;
    }
};


int
auto_check(const std::string& file,
           bool logCreationSuccess,
           bool horizontalFormat)
{
    int status = SUCCESS;

    std::string trace_file = file;

    // build name of backup info
    std::string backUpInfoFileName = trace_file;
    backUpInfoFileName += "_bkpinfo";

    short current_bkp = 0;

    // open the file for read backup info
    std::ifstream bkp_file_in_str(backUpInfoFileName.c_str(), std::ios::in);

    // if file exist and can be opened
    if (bkp_file_in_str) {
        // get current backup from info file
        bkp_file_in_str >> current_bkp;

        // if current backup not retrieve
        // set no backup
        if (!bkp_file_in_str.good()) {
            current_bkp = 0;
        }

        // close the file
        bkp_file_in_str.close();
    }

    if (current_bkp > 0) {
        // update the file name base with the backup nbr
        std::ostringstream name_str;
        name_str << file << "_" << current_bkp;

        trace_file = name_str.str();
    }

    // add extension for 1st circular file
    trace_file += ".0";

    Cdmw::Trace::TestTraceFile test_check(trace_file,
                                          logCreationSuccess,
                                          horizontalFormat);

    test_check.start();

    return status;
}


void exceptionHandler(void)
{
    std::cout << "Trace Test: UNEXPECTED EXCEPTION HANDLER" << std::endl;
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
    using namespace Cdmw::Common;

    // get arguments

    // help argument
    if (argc >= 2) {
        if(strcmp(argv[1], "--help") == 0) {
            usage(std::cout, argv[0]);
            return SUCCESS;
        }
    }

    // launch Nameservice and SimRepository for tests
    OS::ProcessId idNaming, idRepository; 
    // launch TraceCollector and TraceLibrary for tests
    OS::ProcessId idGlobalTraceCollector, idTraceCollector, idTraceLibrary; 

    // ORB arguments
    char** ORB_init_argv = 0;
    int ORB_init_argc(0);

    // get the Name Service port.
    std::string nameServicePort =
        OS::get_option_value(argc, argv, NAMING_SERVICE_PORT_OPTION);
    if (nameServicePort == "no") {
        nameServicePort = DEF_NAME_SERVICE_PORT;
    } 

    // get the Repository port.
    std::string repositoryPort =
        OS::get_option_value (argc, argv, REPOSITORY_PORT_OPTION);
    if (repositoryPort == "no") {
        repositoryPort = DEF_REPOSITORY_PORT;
    } 

    // get the TraceCollector Service port.
    std::string collectorServPort =
        OS::get_option_value(argc, argv, COLLECTOR_SERV_PORT_OPTION);
    if (collectorServPort == "no") {
        collectorServPort = DEF_TRC_COLLECTOR_SERV_PORT;
    } 

    // get the TraceCollector Cdmw port.
    std::string collectorCdmwPort =
        OS::get_option_value(argc, argv, COLLECTOR_CDMW_PORT_OPTION);
    if (collectorCdmwPort == "no") {
        collectorCdmwPort = DEF_TRC_COLLECTOR_CDMW_PORT;
    }
    
    // get Trace collector options
    
    //
    // get Trace Collector Name
    //
    std::string collectorName =
        OS::get_option_value(argc, argv, COLLECTOR_NAME_OPTION);
    if (collectorName == "no" || collectorName == "yes") {
        collectorName = COLLECTOR_NAME;
    }
    
    //
    // get Trace File Name
    //
    std::string traceFileName =
        OS::get_option_value(argc, argv, TRACE_FILE_NAME_OPTION);
    if (traceFileName == "no" || traceFileName == "yes") {
        traceFileName = TRACE_FILE_NAME; 
    }

    // get the component name
    // ECR-0123
    std::string componentName =
        OS::get_option_value(argc, argv, COMPONENT_NAME_OPTION);

    //
    // get Filter Domain Name
    //
    std::string domainName =
        OS::get_option_value (argc, argv, DOMAIN_NAME_OPTION);
    
    //
    // get Filter Level value
    //
    std::string levelOption =
        OS::get_option_value(argc, argv, LEVEL_OPTION);
    
    //
    // get Format of trace
    //
    Cdmw::Trace::Message::TraceFormat traceFormat =
        Cdmw::Trace::Message::V_FORMAT;
    std::string formatOption =
        OS::get_option_value(argc, argv, TRACE_FORMAT_OPTION);
    if (formatOption != "no") {
        if (formatOption == TRACE_FORMAT_OPTV.c_str()) {
            traceFormat = Cdmw::Trace::Message::V_FORMAT;
        } 
        else if (formatOption == TRACE_FORMAT_OPTH.c_str()) {
            traceFormat = Cdmw::Trace::Message::H_FORMAT;
        } 
        else {
            std::cout << "bad value for " << TRACE_FORMAT_OPTION
                      << " option" << std::endl
                      << "  options are : " << TRACE_FORMAT_OPTV
                      << " or " << TRACE_FORMAT_OPTH
                      << std::endl << std::endl;
            return FAILURE;
        }
    }
    
    //
    // get number of trace file backup
    //
    std::string traceFileBkpNbrOption =
        OS::get_option_value (argc, argv, TRACE_FILE_BKP_NBR_OPTION);
    
    //
    // get Trace Circular File number
    //
    std::string traceFileNbrOption =
        OS::get_option_value (argc, argv, TRACE_FILE_NBR_OPTION);

    //
    // get Trace Message number to store in file
    //
    std::string traceMessageNbrOption =
        OS::get_option_value (argc, argv, TRACE_MESSAGE_NBR_OPTION);

    // Starting the Name Service : "nameserv -OAport <port>"
    std::cout << "start name service" << std::endl;

#if (CDMW_ORB_VDR == orbacus )
    std::string namingservice = "nameserv";
    idNaming = OS::create_process(namingservice,
                                  " -OAport " + nameServicePort);
#elif (CDMW_ORB_VDR == tao )
    std::string namingservice = "Naming_Service";
    idNaming = OS::create_process(
                   namingservice,
                   "-ORBEndpoint iiop://localhost:" + nameServicePort);
#endif

    int timescale = Cdmw::TestUtils::Testable::get_timescale();
    OS::sleep(timescale * 10000);
    
    // Starting the Repository Simulator :
    // "simulated_repository -OAport <port> 
    //  -ORBnaming corbaloc::localhost:<port>/NameService
    //  -DOM CDMW/SERVICES -DOM CDMW/ADMIN -DOM CDMW/ADMIN/TRACE/COLLECTORS
    //  -ROOT AdminRootContext"
    std::string repository = "cdmw_simulated_repository";
    std::string namingCorbaloc =
#if (CDMW_ORB_VDR == orbacus)
        " -ORBnaming corbaloc::localhost:" + nameServicePort + "/NameService";
#elif (CDMW_ORB_VDR == tao)
        " -ORBInitRef NameService=corbaloc:iiop:localhost:" +
        nameServicePort + "/NameService";
#endif
    
    std::string repositoryArgs ("");
#if (CDMW_ORB_VDR == orbacus)
    repositoryArgs += " -OAport ";
#elif (CDMW_ORB_VDR == tao )
    repositoryArgs += " -ORBEndpoint iiop://localhost:";
#endif

    repositoryArgs += repositoryPort;
    repositoryArgs += namingCorbaloc;
    repositoryArgs += " -DOM ";
    repositoryArgs += Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN;
    repositoryArgs += " -DOM ";
    repositoryArgs += Cdmw::Common::Locations::CDMW_SERVICES_NAME_DOMAIN;
    repositoryArgs += "/";
    repositoryArgs += Cdmw::Common::Locations::TRACE_COLLECTORS_NAME_DOMAIN;
    repositoryArgs += " -ROOT ";
    repositoryArgs += Cdmw::Common::Locations::ADMIN_ROOT_CONTEXT_ID;
        
    std::cout << "start repository simulator" << std::endl
              << repository << repositoryArgs << std::endl;

    idRepository = OS::create_process(repository, repositoryArgs);

    OS::sleep(timescale * 10000);
        
    // set repository url
    std::string repositoryCorbaloc =
        " --namingRepos-url=corbaloc::localhost:" +
        repositoryPort + "/SimRepository";

    // ECR-0145
    // start the global trace collector.
    std::string gblTraceCltor("cdmw_global_trace_collector");

    std::ostringstream oss;
    oss << " --collector-name=" << GLOBAL_COLLECTOR_NAME
        << " --trace-file-name=" << GLOBAL_TRACE_FILE_NAME
        << " --CdmwLocalisationService=" << DEF_GBL_TRC_COLL_SERV_PORT
        << " --CdmwProcessPort=" << DEF_GBL_TRC_COLL_CDMW_PORT
        << " --namingRepos-url=corbaloc::localhost:" << repositoryPort
        << "/SimRepository";
    if (formatOption != "no") {
        oss << " " << TRACE_FORMAT_OPTION << "=" << formatOption;
    }
    std::string gblTraceCltorArgs(oss.str());

    std::cout << "start global trace collector" << std::endl;
    std::cout << gblTraceCltor << gblTraceCltorArgs << std::endl;

    idGlobalTraceCollector =
        OS::create_process(gblTraceCltor, gblTraceCltorArgs);

    OS::sleep(timescale * 10000);

    // Starting the Trace Collector :
    // "tracecollector --collector-name=<name>
    //                 --traceFile-name=<name>
    //                 --CdmwLocalisationService=<port>
    //                 --CdmwProcessPort=<port>
    //                 --namingRepos_url=<url>"
    std::string traceCollector = "cdmw_trace_collector";
    std::string traceCollectorArgs ("");
    traceCollectorArgs += " --collector-name=";
    traceCollectorArgs += collectorName;
    traceCollectorArgs += " --traceFile-name=";
    traceCollectorArgs += traceFileName;
    traceCollectorArgs += " --CdmwLocalisationService=";
    traceCollectorArgs += collectorServPort;
    traceCollectorArgs += " --CdmwProcessPort=";
    traceCollectorArgs += collectorCdmwPort;
    traceCollectorArgs += repositoryCorbaloc;

    // ECR-0123
    if ((componentName != "no") && (componentName != "yes")) {
        traceCollectorArgs +=
            " " + COMPONENT_NAME_OPTION + "=" + componentName;
    }

    if (domainName != "no" && domainName != "yes") {
        traceCollectorArgs += " ";
        traceCollectorArgs += DOMAIN_NAME_OPTION;
        traceCollectorArgs += "=";
        traceCollectorArgs += domainName;
    }
    
    if (levelOption != "no") {
        traceCollectorArgs += " ";
        traceCollectorArgs += LEVEL_OPTION;
        traceCollectorArgs += "=";
        traceCollectorArgs += levelOption;
    }
    
    if (formatOption != "no") {
        traceCollectorArgs += " ";
        traceCollectorArgs += TRACE_FORMAT_OPTION;
        traceCollectorArgs += "=";
        traceCollectorArgs += formatOption;
    }
    
    if (traceFileBkpNbrOption != "no") {
        traceCollectorArgs += " ";
        traceCollectorArgs += TRACE_FILE_BKP_NBR_OPTION;
        traceCollectorArgs += "=";
        traceCollectorArgs += traceFileBkpNbrOption;
    }
    
    if (traceFileNbrOption != "no") {
        traceCollectorArgs += " ";
        traceCollectorArgs += TRACE_FILE_NBR_OPTION;
        traceCollectorArgs += "=";
        traceCollectorArgs += traceFileNbrOption;
    }
    
    if (traceMessageNbrOption != "no") {
        traceCollectorArgs += " ";
        traceCollectorArgs += TRACE_MESSAGE_NBR_OPTION;
        traceCollectorArgs += "=";
        traceCollectorArgs += traceMessageNbrOption;
    }

    std::cout << "start trace collector" << std::endl
              << traceCollector << traceCollectorArgs << std::endl;

    idTraceCollector =
        OS::create_process(traceCollector, traceCollectorArgs);

    OS::sleep(timescale * 10000);
        
    // Starting the Trace Library Test :
    // "trace --producer_name=<name> --namingRepos_url=<url>"
    std::string traceLibrary = "cdmw_test_trace";
    std::string traceLibraryArgs("");
    traceLibraryArgs += " --application-name=";
    traceLibraryArgs += PRODUCER_NAME;
    traceLibraryArgs += repositoryCorbaloc;
    
    std::cout << "start trace library" << std::endl
              << traceLibrary << traceLibraryArgs << std::endl;

    idTraceLibrary = OS::create_process(traceLibrary, traceLibraryArgs);

    // Wait end of test
    OS::sleep(timescale * 15000);
        
    // build ORB argument with process name and naming corbaloc
    std::string strArgv = argv[0] + namingCorbaloc;
    ORB_init_argv = String::to_char_array(String::to_strings(strArgv));
    ORB_init_argc = 3;
    
    int status = FAILURE;
    CORBA::ORB_var orb;

    try {
        // Initialize the ORB
        Cdmw::OrbSupport::StrategyList orb_strategies;
        orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);

        orb = Cdmw::OrbSupport::OrbSupport::ORB_init(ORB_init_argc,
                                                     ORB_init_argv, 
                                                     orb_strategies);

        status = SUCCESS;
    }
    catch (const Cdmw::Exception& ex) {
      std::cerr << ex.what() << std::endl;
    } 
    catch(const CORBA::Exception& ex) {        
      std::cerr << ex << std::endl;
    }

    std::cout << "stop all processes" << std::endl;

    // stop the trace collectors
    try {
        if (status == SUCCESS) {
            // set traceCollector agent url
            std::string collectorAgentURL =
                "corbaloc::localhost:" + collectorServPort + "/CollectorAgent";

            // get collector agent object to stop tracecollector process
            CORBA::Object_var collectorAgtObj = CORBA::Object::_nil();
            TraceCollector::ProcessAgent_var collectorAgtRef =
                TraceCollector::ProcessAgent::_nil();

            try {
                collectorAgtObj =
                    orb->string_to_object(collectorAgentURL.c_str());
            }
            catch(const CORBA::Exception& ex) {
                std::cerr << "Invalid Trace Collector agent URL" << std::endl;
                std::cerr << ex << std::endl;
                status = FAILURE;
            }

            // get collector agent servant
            if(CORBA::is_nil(collectorAgtObj.in())) {
                std::cerr << "Trace Collector agent URL denotes "
                          << "a nil object reference" << std::endl;
                status = FAILURE;
            }

            try {
                if (status == SUCCESS) {
                    // Get reference to the TraceCollector agent interface
                    collectorAgtRef =
                        TraceCollector::ProcessAgent::_narrow(
                            collectorAgtObj.in());
                            
                    if (CORBA::is_nil(collectorAgtRef.in())) {
                        std::cerr << "Couldn't retrieve reference of "
                                  << "the TraceCollector agent" << std::endl;
                        status = FAILURE;
                    }
                }
            } 
            catch(const CORBA::Exception& ex) {
                std::cerr << "Couldn't retrieve reference of the "
                        << "TraceCollector agent" << std::endl
                        << ex << std::endl;
                status = FAILURE;
            }

            try {
                if (status == SUCCESS) {
                    // then stop the trace collector process
                    collectorAgtRef->stop_process();
                }
            }
            catch (const CORBA::Exception& ex) {
                std::cerr << "Trace Collector Process Agent access error : " 
                          << collectorAgentURL << std::endl;
                std::cerr << ex << std::endl;   
                status = FAILURE;
            }
        }

        // ECR-0145
        if (status == SUCCESS) {
            // set the global trace collector agent url
            std::string collectorAgentURL =
                "corbaloc::localhost:" +
                DEF_GBL_TRC_COLL_SERV_PORT + "/GlobalCollectorAgent";

            // get collector agent object to stop tracecollector process
            CORBA::Object_var collectorAgtObj = CORBA::Object::_nil();
            TraceCollector::ProcessAgent_var collectorAgtRef =
                TraceCollector::ProcessAgent::_nil();

            try {
                collectorAgtObj =
                    orb->string_to_object(collectorAgentURL.c_str());
            }
            catch(const CORBA::Exception& ex) {
                std::cerr << "Invalid Global Trace Collector agent URL"
                          << std::endl << ex << std::endl;
                status = FAILURE;
            }

            // get collector agent servant
            if (CORBA::is_nil(collectorAgtObj.in())) {
                std::cerr << "Global Trace Collector agent URL denotes "
                          << "a nil object reference" << std::endl;
                status = FAILURE;
            }

            try {
                if (status == SUCCESS) {
                    // Get reference to the TraceCollector agent interface
                    collectorAgtRef =
                        TraceCollector::ProcessAgent::_narrow(
                            collectorAgtObj.in());
                            
                    if (CORBA::is_nil(collectorAgtRef.in())) {
                        std::cerr << "Couldn't retrieve reference of "
                                  << "the Global Trace Collector agent"
                                  << std::endl;
                        status = FAILURE;
                    }
                }
            } 
            catch(const CORBA::Exception& ex) {
                std::cerr << "Couldn't retrieve reference of the "
                          << "Global Trace Collector agent" << std::endl
                          << ex << std::endl;
                status = FAILURE;
            }

            try {
                if (status == SUCCESS) {
                    // then stop the trace collector process
                    collectorAgtRef->stop_process();
                }
            }
            catch (const CORBA::Exception& ex) {
                std::cerr << "Global Trace Collector Process Agent "
                          << "access error : " << collectorAgentURL
                          << std::endl << ex << std::endl;   
                status = FAILURE;
            }
        }
    }
    catch(...) {
        std::cerr << "Catch exception during trace "
                  << "collector stopping" << std::endl;
        status = FAILURE;
    }

    // check statistics result (if Vertical trace format)
    OS::sleep(timescale * 1000);
    
    bool horizontalFormat = false;
    if (formatOption != "no") {
        if (traceFormat == Cdmw::Trace::Message::H_FORMAT) {
            horizontalFormat = true;
        }
    }
    
    status = auto_check(TRACE_FILE_NAME, true, horizontalFormat);

    status = auto_check(GLOBAL_TRACE_FILE_NAME, true, horizontalFormat);

    // then kill the Repository simulator and Name Server
    try {
        // And we kill the Repository simulator
        OS::kill_process(idRepository);
    }
    catch(const std::exception & ex) {
        std::cerr << "Cannot kill Repository process.:" << std::endl;
        status = FAILURE;
    }

    try {
        // And we kill the Name Server
        OS::kill_process(idNaming);
    }
    catch(const std::exception & ex) {
        std::cerr << "Cannot kill Name Server process.:" << std::endl;
        status = FAILURE;
    }

    // free memory allocated by String::to_char_array
    for (int iArgc = 0; iArgc < ORB_init_argc; ++iArgc) {
        delete[] ORB_init_argv[iArgc];
    }
    delete[] ORB_init_argv;

    // end of corba processing
    if (!CORBA::is_nil(orb.in())) {
        try {
            orb->shutdown(false);

            Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());

            orb->destroy();
            std::cerr << std::endl;
        }
        catch(const CORBA::Exception& ex) {
            std::cerr << ex << std::endl;
            status = FAILURE;
        }
    }

    return status;
}


