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


#include "tracecollector/TraceCollector_impl.hpp"
#include "tracecollector/CollectorMngr.hpp"

#include <iostream>


#include "TraceAndPerf/tracelibrary/FilterMngr.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes: none 
*<p>
*Portability issues:  none
*<p>
*/


//
// Ctor
//
TraceCollector_impl::TraceCollector_impl(CollectorMngr* pCollectorMngr,
				                         FilterMngr* pFilterMngr)
    throw()
	: m_pCollectorMngr (pCollectorMngr),
	  m_pFilterMngr (pFilterMngr)
{

}


//
// Dtor
//
TraceCollector_impl::~TraceCollector_impl()
    throw()
{

}

//
// get levels for the collector
//
CdmwTrace::TraceFilterSeq* TraceCollector_impl::get_trace_filters()
    throw(CORBA::SystemException)
{
    try 
	{
        return m_pFilterMngr->get_trace_filters();
    } 
	catch (const OutOfMemoryException&)
	{
        throw CORBA::NO_RESOURCES();
    }
}    




//
// activate specific level for the collector
//
void TraceCollector_impl::activate_level (const char* a_domain,
                                          CdmwTrace::Value a_Value)
    throw(CORBA::SystemException)
{
    m_pFilterMngr->activate_level (a_domain, a_Value);
}    



//
// deactivate specific level for the collector
//
void TraceCollector_impl::deactivate_level(const char* a_domain,
                                           CdmwTrace::Value a_Value)
    throw(CORBA::SystemException)
{
    m_pFilterMngr->deactivate_level (a_domain, a_Value);
}    



void TraceCollector_impl::put_formatted_messages (  
                      const CdmwTrace::MessagesHeader& the_messages_header,
                      const CdmwTrace::FormattedMessageSeq& the_messages)
    throw(CORBA::SystemException)                       
{
    try
    {
        for (size_t iMessage = 0 ; iMessage < the_messages.length() ; iMessage++)
	    {
          m_pCollectorMngr->writeMessage (the_messages_header,the_messages[iMessage]);
        }
    }
    catch(const OutOfMemoryException &)
    {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES, CORBA::COMPLETED_NO);
    }
}                      


void TraceCollector_impl::update (CdmwTrace::TraceProducer* p_trace_producer)
    throw(CORBA::SystemException)
{

}




}; // End namespace Trace
}; // End namespace Cdmw

