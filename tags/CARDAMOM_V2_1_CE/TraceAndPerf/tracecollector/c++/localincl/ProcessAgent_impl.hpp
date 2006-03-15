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


#ifndef INCL_TRACE_COLLECTOR_PROCESS_AGENT_IMPL_HPP 
#define INCL_TRACE_COLLECTOR_PROCESS_AGENT_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "tracecollector/TraceCollectorProcessAgent.skel.hpp"

#include <string>

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW Trace types and data.
*/
namespace Trace
{

/**
*Purpose:
*<p>    The ProcessAgent_impl class provides implementation
*       to stop the process
*
*Features:
*<p> 
*
*@see   
*@link  
*
*/
class ProcessAgent_impl :
        public virtual POA_TraceCollector::ProcessAgent,
        public PortableServer::RefCountServantBase
{
public:
    ProcessAgent_impl (CORBA::ORB_ptr orb,
                       PortableServer::POA_ptr parent) 
        throw(CORBA::SystemException);   
    
    virtual PortableServer::POA_ptr _default_POA() 
	{
      return PortableServer::POA::_duplicate(m_poa.in());
    }

    virtual void stop_process()
        throw(CORBA::SystemException);

private:
    /**
    * ORB reference
    */ 
    CORBA::ORB_var m_orb;

    /**
    * POA reference
    */ 
    PortableServer::POA_var m_poa;

}; // End class ProcessAgent_impl


}; // End namespace Trace


}; // End namespace Cdmw


#endif // INCL_TRACE_COLLECTOR_PROCESS_AGENT_IMPL_HPP 

