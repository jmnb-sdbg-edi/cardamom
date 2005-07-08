/* =========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI.
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003.
 * All rights reserved.
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Library General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General
 * Public License along with CARDAMOM; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * =========================================================================== */


#include <cstdlib>
#include <string>
#include <sstream>

#include "TraceAndPerf/tracelibrary/InitUtils.hpp"
#include "TraceAndPerf/tracelibrary/Trace.hpp"

#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Options.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/common/Locations.hpp"

#include "Repository/naminginterface/NamingInterface.hpp"
#include "Repository/naminginterface/NamingUtil.hpp"
#include "Repository/repositoryinterface/RepositoryInterface.hpp"

namespace
{    
    /**
     * Name of the collector found registered during init
     */
	const std::string COLLECTOR_INI_MNEMONAME = "collector_ini_";

}; // End anonymous namespace



namespace Cdmw
{

namespace Trace
{


void 
InitUtils::init_trace_library (PortableServer::POA_ptr tracePOA,
			                   const std::string & application_name,
                               const std::string & process_name,
                               size_t flushing_time, 
                               size_t nb_FlushArea, 
                               size_t size_FlushArea,
			                   const std::vector<std::string> & collectorNameList)
    throw (Cdmw::OrbSupport::CORBASystemExceptionWrapper)
{

    using Cdmw::Trace::FlushAreaMngr;
    using Cdmw::NamingAndRepository::RepositoryInterface;
    using Cdmw::Common::Locations;

    using namespace Cdmw::NamingAndRepository;
    using namespace Cdmw::OrbSupport;

    // create the initial collector list
    std::list<CdmwTrace::TraceProducer::CollectorData> collectorList;
    
    try 
    {	
	    // set trace collector location under default root context
	    // Pattern is :
	    //    CDMW/SERVICES/TRACE/COLLECTORS/"
	    std::string collectorDomainStr;
	    collectorDomainStr = Locations::CDMW_SERVICES_NAME_DOMAIN;
	    collectorDomainStr += "/";
	    collectorDomainStr += Locations::TRACE_COLLECTORS_NAME_DOMAIN;
	
	    // get Collector domain Naming Interface via Repository Interface
	    Cdmw::NamingAndRepository::NamingInterface ni_collector =
	        RepositoryInterface::get_domain_naming_interface (collectorDomainStr);

        // for each input collector name, search in repository the corresponding
        // trace collector object
	    int collector_nb = collectorNameList.size();
	    bool collector_found;
	
	    for (int collector_ix=0 ; collector_ix < collector_nb ; collector_ix++) 
	    {
	        collector_found = true;
	    
	        // get collector name
	        std::string collectorName = collectorNameList[collector_ix];
	       
	        CdmwTrace::Collector_var collector;
	    
	        try 
	        {
		        CORBA::Object_var obj = ni_collector.resolve (collectorName);
		        collector = CdmwTrace::Collector::_narrow(obj.in());
	        } 
	        catch (const CosNaming::NamingContext::NotFound & ) 
	        {
		        // ignore if collector not registered in repository
		        collector_found = false;
		        std::cerr << "Trace Collector " << collectorName.c_str() 
			              << " is not found in repository" << std::endl;
	        }
		                                                       
	        if (collector_found) 
	        {
		        // create collector mnemonic name
		        std::ostringstream mnemo_name_str;
		        mnemo_name_str << COLLECTOR_INI_MNEMONAME << collector_ix;
		
	            // create collector data structure
		        CdmwTrace::TraceProducer::CollectorData collector_data;
		        collector_data.the_collectorMnemoName = 
		               CORBA::string_dup(mnemo_name_str.str().c_str());
		        collector_data.the_collectorObjName = 
		               CORBA::string_dup(collectorName.c_str());
		               
		        // (collector is a _var, _duplicate is implicit)
		        collector_data.the_collector = collector;

	            // insert collector data in list
		        collectorList.insert (collectorList.end(), collector_data);
	        }
	    }
	
	
    } 
    catch (const std::bad_alloc& ex) 
    {
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                      CORBA::NO_MEMORY(Cdmw::OrbSupport::NO_MEMORY,CORBA::COMPLETED_NO),
                      ex.what());
    } 
    catch (const CdmwNamingAndRepository::NoNameDomain & ex) 
    {
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO));
    } 
    catch (const CdmwNamingAndRepository::InvalidName & ex) 
    {
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALInvalidTraceCollectorsLocation,CORBA::COMPLETED_NO),
                      ex.reason.in());
    } 
    catch (const Cdmw::NamingAndRepository::InvalidNameException & ex) 
    {
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALInvalidTraceCollectorsLocation,CORBA::COMPLETED_NO),
                      ex.what());
    } 
    catch (const CosNaming::NamingContext::InvalidName & ex) 
    {
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALInvalidTraceCollectorsLocation,CORBA::COMPLETED_NO));
    } 
    catch (const CosNaming::NamingContext::CannotProceed & ex) 
    {
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALInvalidTraceCollectorsLocation,CORBA::COMPLETED_NO));
    } 
    catch (const CORBA::SystemException & ex) 
    {
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex);
    }

    // init of FlushAreaMngr
    CdmwTrace::TraceProducer_var trace_producer = CdmwTrace::TraceProducer::_nil();
    
    try 
    {
        trace_producer = FlushAreaMngr::init (tracePOA,
					                          collectorList,
					                          CdmwTrace::ALL_DOMAINS, 
					                          CdmwTrace::ALL_VALUES,
					                          application_name,
					                          process_name,
                                              flushing_time,
                                              nb_FlushArea,
                                              size_FlushArea);
	
    } 
    catch (const Cdmw::OutOfMemoryException & ex) 
    {
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::NO_MEMORY>,
                      CORBA::NO_MEMORY(NO_MEMORY,CORBA::COMPLETED_NO),
                      ex.what());
    } 
    catch (const Cdmw::BadParameterException & ex) 
    {
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAM,CORBA::COMPLETED_NO),
                      ex.what());
    } 
    catch (const Cdmw::InternalErrorException & ex) 
    {
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNAL,CORBA::COMPLETED_NO),
                      ex.what());
    }
    
    // Register TraceProducer object
    std::string trace_producer_name;        
    
    // Build the name of the Trace Producer object
    // Pattern is :
    //    hostname/appli_name/process_name/TRACE/TraceProducer

    trace_producer_name = Cdmw::OsSupport::OS::get_hostname();
    trace_producer_name += std::string("/") + application_name;
    trace_producer_name +=  std::string("/") 
	                        + process_name 
	                        + std::string("/TRACE/TraceProducer");
        
        
    // Get "AdminRootContext" root context and then
    // register TraceProducer object
    CosNaming::NamingContext_var admin_root = CosNaming::NamingContext::_nil();
    
    try 
    {
        CdmwNamingAndRepository::Repository_var rep =
	                         RepositoryInterface::get_repository();
	
        admin_root = rep->resolve_root_context(Locations::ADMIN_ROOT_CONTEXT_ID);
	
    } 
    catch (const CdmwNamingAndRepository::Repository::NoRootContext &) 
    {
        std::string s(Locations::ADMIN_ROOT_CONTEXT_ID);
        s += " is not a Root Context!";
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO),
                      s);
    } 
    catch (const CORBA::SystemException & ex) 
    {
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex);
    }    
    
    try 
    {
        Cdmw::NamingAndRepository::NamingInterface ni (admin_root.in());
        
        // If already registered, force its registration
        ni.bind(trace_producer_name,trace_producer.in(),true);
    } 
    catch (const CosNaming::NamingContext::NotFound & ) 
    {
        // Should not happen!
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMInvalidTraceProducerLocation,CORBA::COMPLETED_NO));
    } 
    catch (const CosNaming::NamingContext::CannotProceed & ) 
    {
        // Should not happen!
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO));
    } 
    catch (const CosNaming::NamingContext::InvalidName & ex) 
    {
        std::string s(trace_producer_name);
        s += " is not valid!";
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::BAD_PARAM>,
                      CORBA::BAD_PARAM(BAD_PARAMInvalidTraceProducerName,CORBA::COMPLETED_NO),
                      s );
    } 
    catch (const CosNaming::NamingContext::AlreadyBound & ) 
    {
        // Should not happen because force is set to true!
        std::string s(trace_producer_name);
        s += " is already bound!";
        CDMW_THROW2 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::INTERNAL>,
                      CORBA::INTERNAL(INTERNALCdmwRepositoryError,CORBA::COMPLETED_NO),
                      s);
    } 
    catch (const CORBA::SystemException & ex) 
    {
        CDMW_THROW1 (Cdmw::OrbSupport::CORBASystemExceptionWrapperT<CORBA::SystemException>,ex);
    }
}


}; // namespace Trace
}; // namespace Cdmw


