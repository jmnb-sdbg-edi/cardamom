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


#ifndef INCL_TRACE_ADMIN_TOOL_HPP
#define INCL_TRACE_ADMIN_TOOL_HPP


#include "Foundation/orbsupport/CORBA.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.stub.hpp"
#include "TraceAndPerf/idllib/CdmwTraceCollector.stub.hpp"

#include <iostream>
#include <string>

namespace Cdmw
{
namespace TraceAdmin
{
    
//--------------------------------------------------------------------------------------//
class TraceAdminTool
{
  public:
    
    TraceAdminTool(CORBA::ORB_ptr, CdmwNamingAndRepository::Repository_ptr,
				   const std::string& commandFile);
    
    ~TraceAdminTool() throw();

    void run(std::istream&,std::ostream&,bool commandFile);
    
  protected:
    
    // Available commands
    CORBA::Long print_help (const std::string &, std::ostream &);
    CORBA::Long exit (const std::string &, std::ostream &);
    CORBA::Long toggle_verbose(const std::string &, std::ostream &);
    
    CORBA::Long get_collector_serv(const std::string &, bool, std::ostream &);
    CORBA::Long get_producer_serv(const std::string &, bool, std::ostream &);

    CORBA::Long set_collector_context(const std::string &, std::ostream &);
    CORBA::Long set_producer_context(const std::string &, std::ostream &);
    CORBA::Long activ_collector_level(const std::string &, std::ostream &);
    CORBA::Long deactiv_collector_level(const std::string &, std::ostream &);
    CORBA::Long activ_producer_level(const std::string &, std::ostream &);
    CORBA::Long deactiv_producer_level(const std::string &, std::ostream &);
    CORBA::Long process_level(bool , int , const std::string &, std::ostream &);
    CORBA::Long get_collector_levels(const std::string &, std::ostream &);
    CORBA::Long get_producer_levels(const std::string &, std::ostream &);
    CORBA::Long get_levels(int, const std::string &, std::ostream &);
    CORBA::Long register_collector(const std::string &, std::ostream &);
    CORBA::Long unregister_collector(const std::string &, std::ostream &);
    CORBA::Long get_registered_collectors(const std::string &, std::ostream &);
    CORBA::Long get_collectors(const std::string &, std::ostream &);
    CORBA::Long get_producers(const std::string &, std::ostream &);	

private:
    
    typedef CORBA::Long (TraceAdminTool::* operation)(const std::string &, std::ostream &);
    
    struct command_def
    {
        const char* name;
        operation   op;
    };
    
    static const command_def commands[];
    static const CORBA::ULong nb_commands;
    
    CORBA::Long runOperation(const std::string &, const std::string &, std::ostream &);
    
    TraceAdminTool();
    TraceAdminTool(const TraceAdminTool&);
    TraceAdminTool& operator = (const TraceAdminTool&);
    
    void unknown_command(std::ostream &);    
    void print_prompt(std::ostream&); 
    void print_info(std::ostream&);
	
	// orb reference
    CORBA::ORB_var  m_orb;

	// Repository object reference
    CdmwNamingAndRepository::Repository_var m_repository;

	// command file name
	std::string m_commandFile;

	// verbose flag
    bool m_verbose;

	// Root context
	CosNaming::NamingContext_var m_defaultRootContext;
	CosNaming::NamingContext_var m_adminRootContext;

	// Servant context
	std::string m_collector_context;
	std::string m_producer_context;

	// Servant object reference
	CdmwTrace::TraceProducer_var m_traceProducerRef;
	CdmwTrace::Collector_var m_collectorRef;
};


}; // namespace TraceAdmin
}; // namespace Cdmw

#endif // INCL_TRACEADMIN_TOOL_HPP

