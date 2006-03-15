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


#include <sstream>
#include <string>

#include <Foundation/common/Assert.hpp>
#include <Foundation/common/Exception.hpp>
#include <Foundation/common/Options.hpp>
#include <Foundation/common/Locations.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Foundation/commonsvcs/naming/NamingUtil.hpp>
#include <Foundation/logging/LogManager.hpp> // ECR-0169
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <Repository/repositoryinterface/RepositoryInterface.hpp>

#include <TraceAndPerf/tracelibrary/InitUtils.hpp>
#include <TraceAndPerf/tracelibrary/Trace.hpp>

#include "tracelibrary/LogManagerTraceDelegate.hpp" // ECR-0169


namespace
{
    // mnemonic name prefix for the trace collectors.
    const std::string COLLECTOR_INI_MNEMONAME = "collector_ini_";
}; // anonymous namespace


namespace Cdmw
{
namespace Trace
{


/**
 * Initialize the trace library.
 */
void
InitUtils::init_trace_library(
        PortableServer::POA_ptr tracePOA,
        const std::string& application_name,
        const std::string& process_name,
        size_t flushing_time,
        size_t msg_threshold,
        size_t nb_FlushArea,
        size_t size_FlushArea,
        const std::vector<std::string>& collectorNameList,
        long max_log_level // ECR-0169
        )
    throw(Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{
    using Cdmw::Common::Locations;
    using Cdmw::OrbSupport::CORBASystemExceptionWrapperT;
    using Cdmw::NamingAndRepository::RepositoryInterface;
    using Cdmw::Trace::FlushAreaMngr;

    using namespace Cdmw::CommonSvcs::Naming;
    using namespace Cdmw::OrbSupport;

    // the initial list of collectors.
    std::list<CdmwTrace::TraceProducer::CollectorData> collectorList;

    // get the "AdminRootContext" root context.
    CosNaming::NamingContext_var adminRootCtx =
        CosNaming::NamingContext::_nil();

    try {
        CdmwNamingAndRepository::Repository_var rep =
            RepositoryInterface::get_repository();

        adminRootCtx =
            rep->resolve_root_context(Locations::ADMIN_ROOT_CONTEXT_ID);
    }
    catch (const CdmwNamingAndRepository::Repository::NoRootContext&) {
        std::string s(Locations::ADMIN_ROOT_CONTEXT_ID);
        s += " is not a root context.";

        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNALCdmwRepositoryError,
                            CORBA::COMPLETED_NO),
            s);
    }
    catch (const CORBA::SystemException& ex) {
        CDMW_THROW1(
            CORBASystemExceptionWrapperT<CORBA::SystemException>,
            ex);
    }

    try {
        /* ECR-0145
        // set trace collector location under default root context
        // Pattern is :CORBA::string_dup(collectorNa
        //    CDMW/SERVICES/TRACE/COLLECTORS/"
        std::string collectorDomainStr;
        collectorDomainStr = Locations::CDMW_SERVICES_NAME_DOMAIN;
        collectorDomainStr += "/";
        collectorDomainStr += Locations::TRACE_COLLECTORS_NAME_DOMAIN;

        // get Collector domain Naming Interface via Repository Interface
        Cdmw::CommonSvcs::Naming::NamingInterface ni_collector =
            RepositoryInterface::get_domain_naming_interface(collectorDomainStr);

        // for each input collector name, search in repository the corresponding
        // trace collector object
        int collector_nb = collectorNameList.size();
        bool collector_found;
        */

        // ECR-0145: there is only one local collector per node.
        //
        // the pattern is:
        // hostname/SERVICES/TRACE/COLLECTORS/<collector_name>
        //
        // where collector_name is either the first name in collectorNameList
        // or "LocalCollector" if collectorNameList is empty.
        const std::string collectorName((collectorNameList.size() > 0)
                                            ? collectorNameList[0]
                                            : "LocalCollector");

        CosNaming::Name collectorName_;
        collectorName_.length(5);
        collectorName_[0].id =
            CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
        collectorName_[1].id = CORBA::string_dup("SERVICES");
        collectorName_[2].id = CORBA::string_dup("TRACE");
        collectorName_[3].id = CORBA::string_dup("COLLECTORS");
        collectorName_[4].id = CORBA::string_dup(collectorName.c_str());

        Cdmw::CommonSvcs::Naming::NamingInterface
        ni_collector(adminRootCtx.in());

        bool collector_found = true;

        /* ECR-0145
        for (int collector_ix=0 ; collector_ix < collector_nb ; collector_ix++) {
            collector_found = true;

            // get collector name
            std::string collectorName = collectorNameList[collector_ix];
        */

            CdmwTrace::Collector_var collector;

            try {
                // ECR-0145
                // CORBA::Object_var obj = ni_collector.resolve (collectorName);

                CORBA::Object_var obj =
                    ni_collector.resolve(
                        ni_collector.to_string(collectorName_));
                collector = CdmwTrace::Collector::_narrow(obj.in());
            }
            catch (const CosNaming::NamingContext::NotFound&) {
                // ignore if collector not registered in repository
                collector_found = false;
                /* ECR-0145
                std::cerr << "Trace Collector " << collectorName.c_str()
                          << " is not found in repository" << std::endl;
                */

                std::cerr << "The local trace collector "
                          << ni_collector.to_string(collectorName_)
                          << " could not be found in the repository."
                          << std::endl;
            }

            if (collector_found) {
                // create collector mnemonic name
                std::ostringstream mnemo_name_str;
                /* ECR-0145
                mnemo_name_str << COLLECTOR_INI_MNEMONAME << collector_ix;
                */
                mnemo_name_str << COLLECTOR_INI_MNEMONAME << 0;

                // create collector data structure
                CdmwTrace::TraceProducer::CollectorData collector_data;
                collector_data.the_collectorMnemoName =
                       CORBA::string_dup(mnemo_name_str.str().c_str());
                collector_data.the_collectorObjName =
                       CORBA::string_dup(collectorName.c_str());

                // (collector is a _var, _duplicate is implicit)
                collector_data.the_collector = collector;

                // insert collector data in list
                collectorList.insert(collectorList.end(), collector_data);
            }
        // }
    }
    catch (const std::bad_alloc& ex) {
        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
            CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,
                             CORBA::COMPLETED_NO),
            ex.what());
    }
    catch (const CdmwNamingAndRepository::NoNameDomain& ex) {
        CDMW_THROW1(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNALCdmwRepositoryError,
                            CORBA::COMPLETED_NO));
    }
    catch (const CdmwNamingAndRepository::InvalidName& ex) {
        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNALInvalidTraceCollectorsLocation,
                            CORBA::COMPLETED_NO),
            ex.reason.in());
    }
    catch (const Cdmw::CommonSvcs::Naming::InvalidNameException& ex) {
        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNALInvalidTraceCollectorsLocation,
                            CORBA::COMPLETED_NO),
            ex.what());
    }
    catch (const CosNaming::NamingContext::InvalidName& ex) {
        CDMW_THROW1(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNALInvalidTraceCollectorsLocation,
                            CORBA::COMPLETED_NO));
    }
    catch (const CosNaming::NamingContext::CannotProceed& ex) {
        CDMW_THROW1(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNALInvalidTraceCollectorsLocation,
                            CORBA::COMPLETED_NO));
    }
    catch (const CORBA::SystemException& ex) {
        CDMW_THROW1(
            CORBASystemExceptionWrapperT<CORBA::SystemException>,
            ex);
    }

    CdmwTrace::TraceProducer_var trace_producer =
        CdmwTrace::TraceProducer::_nil();

    try {
        // create a trace producer.
        trace_producer =
            FlushAreaMngr::init(tracePOA,
                                collectorList,
                                CdmwTrace::ALL_COMPONENT_NAMES, // ECR-0123
                                CdmwTrace::ALL_DOMAINS,
                                CdmwTrace::ALL_VALUES,
                                application_name,
                                process_name,
                                flushing_time,
                                nb_FlushArea,
                                size_FlushArea,
                                FlushAreaMngr::DEFAULT_AREA_NB_MESSAGE,
                                msg_threshold);
    }
    catch (const Cdmw::OutOfMemoryException& ex) {
        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
            CORBA::NO_MEMORY(NO_MEMORY,
                             CORBA::COMPLETED_NO),
            ex.what());
    }
    catch (const Cdmw::BadParameterException& ex) {
        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
            CORBA::BAD_PARAM(BAD_PARAM,
                             CORBA::COMPLETED_NO),
            ex.what());
    }
    catch (const Cdmw::InternalErrorException& ex) {
        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNAL,
                            CORBA::COMPLETED_NO),
            ex.what());
    }


//     // ECR-0169
//     // register the log manager delegate.
//     Cdmw::Logging::LogManager* logMngr = Cdmw::Logging::LogManager::Instance();
//     Cdmw::Trace::LogManagerTraceDelegate* alt =
//         new Cdmw::Trace::LogManagerTraceDelegate(max_log_level);
//     logMngr->register_alternate_delegate(alt);


    // Register TraceProducer object
    std::string trace_producer_name;

    /* ECR-0145
    // Build the name of the Trace Producer object
    // Pattern is :
    //    hostname/appli_name/process_name/TRACE/TraceProducer

    trace_producer_name =
        Cdmw::CommonSvcs::Naming::NamingInterface::addEscape(
            Cdmw::OsSupport::OS::get_hostname());

    //trace_producer_name = Cdmw::OsSupport::OS::get_hostname();

    trace_producer_name += std::string("/") + application_name;
    trace_producer_name += std::string("/") + process_name;
    trace_producer_name += std::string("/TRACE/TraceProducer");
    */

    CosNaming::Name producerName_;
    producerName_.length(5);
    producerName_[0].id =
        CORBA::string_dup(Cdmw::OsSupport::OS::get_hostname().c_str());
    producerName_[1].id = CORBA::string_dup(application_name.c_str());
    producerName_[2].id = CORBA::string_dup(process_name.c_str());
    producerName_[3].id = CORBA::string_dup("TRACE");
    producerName_[4].id = CORBA::string_dup("TraceProducer");

    /* ECR-0145
    // Get "AdminRootContext" root context and then
    // register TraceProducer object
    CosNaming::NamingContext_var adminRootCtx =
        CosNaming::NamingContext::_nil();

    try {
        CdmwNamingAndRepository::Repository_var rep =
            RepositoryInterface::get_repository();

        adminRootCtx =
            rep->resolve_root_context(Locations::ADMIN_ROOT_CONTEXT_ID);
    }
    catch (const CdmwNamingAndRepository::Repository::NoRootContext &) {
        std::string s(Locations::ADMIN_ROOT_CONTEXT_ID);
        s += " is not a Root Context!";
        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNALCdmwRepositoryError,
                            CORBA::COMPLETED_NO),
            s);
    }
    catch (const CORBA::SystemException & ex) {
        CDMW_THROW1(
            CORBASystemExceptionWrapperT<CORBA::SystemException>,
            ex);
    }
    */

    try {
        Cdmw::CommonSvcs::Naming::NamingInterface ni(adminRootCtx.in());

        trace_producer_name = ni.to_string(producerName_);

        // if already registered, force its registration.
        ni.bind(trace_producer_name, trace_producer.in(), true);
    }
    catch (const CosNaming::NamingContext::NotFound&) {
        // should not happen!
        CDMW_THROW1(
            CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
            CORBA::BAD_PARAM(BAD_PARAMInvalidTraceProducerLocation,
                             CORBA::COMPLETED_NO));
    }
    catch (const CosNaming::NamingContext::CannotProceed&) {
        // should not happen!
        CDMW_THROW1(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNALCdmwRepositoryError,
                            CORBA::COMPLETED_NO));
    }
    catch (const CosNaming::NamingContext::InvalidName& ex) {
        std::string s(trace_producer_name);
        s += " is not valid.";
        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
            CORBA::BAD_PARAM(BAD_PARAMInvalidTraceProducerName,
                             CORBA::COMPLETED_NO),
            s);
    }
    catch (const CosNaming::NamingContext::AlreadyBound&) {
        // should not happen because force is set to true!
        std::string s(trace_producer_name);
        s += " is already bound.";
        CDMW_THROW2(
            CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
            CORBA::INTERNAL(INTERNALCdmwRepositoryError,
                            CORBA::COMPLETED_NO),
            s);
    }
    catch (const CORBA::SystemException& ex) {
        CDMW_THROW1(
            CORBASystemExceptionWrapperT<CORBA::SystemException>,
            ex);
    }
}


}; // namespace Trace
}; // namespace Cdmw
