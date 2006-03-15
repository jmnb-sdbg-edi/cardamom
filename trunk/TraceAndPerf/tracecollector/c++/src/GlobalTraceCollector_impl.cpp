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


#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <TraceAndPerf/tracelibrary/FilterMngr.hpp>

#include "tracecollector/GlobalCollectorMngr.hpp"
#include "tracecollector/GlobalTraceCollector_impl.hpp"


namespace Cdmw
{
namespace Trace
{


/**
 * Constructor.
 */
GlobalTraceCollector_impl::GlobalTraceCollector_impl(
        GlobalCollectorMngr* pCollectorMngr,
        FilterMngr* pFilterMngr)
    throw()
        : m_pCollectorMngr(pCollectorMngr),
          m_pFilterMngr(pFilterMngr)
{
    // NOOP
}


/**
 * Destructor.
 */
GlobalTraceCollector_impl::~GlobalTraceCollector_impl()
    throw()
{
    // NOOP
}


/**
 * Get the filters.
 */
CdmwTrace::TraceFilterSeq*
GlobalTraceCollector_impl::get_trace_filters()
    throw(CORBA::SystemException)
{
    try {
        return m_pFilterMngr->get_trace_filters();
    }
    catch (const OutOfMemoryException&) {
        throw CORBA::NO_RESOURCES();
    }
}


/**
 * Activate the specified level.
 */
void
GlobalTraceCollector_impl::activate_level(const char* a_componentName,
                                          const char* a_domain,
                                          CdmwTrace::Value a_Value)
    throw(CORBA::SystemException)
{
    m_pFilterMngr->activate_level(a_componentName, a_domain, a_Value);
}


/**
 * Deactivate the specified level.
 */
void
GlobalTraceCollector_impl::deactivate_level(const char* a_componentName,
                                            const char* a_domain,
                                            CdmwTrace::Value a_Value)
    throw(CORBA::SystemException)
{
    m_pFilterMngr->deactivate_level(a_componentName, a_domain, a_Value);
}


/**
 * Write a message.
 */
void
GlobalTraceCollector_impl::put_formatted_messages(
        const CdmwTrace::MessagesHeader& the_messages_header,
        const CdmwTrace::FormattedMessageSeq& the_messages)
    throw(CORBA::SystemException)
{
    try {
        for (size_t iMessage = 0;
             iMessage < the_messages.length();
             ++iMessage)
        {
            m_pCollectorMngr->writeMessage(the_messages_header,
                                           the_messages[iMessage]);
        }
    }
    catch(const OutOfMemoryException&) {
        throw CORBA::NO_RESOURCES(OrbSupport::NO_RESOURCES,
                                  CORBA::COMPLETED_NO);
    }
}


/**
 * Update.
 */
void
GlobalTraceCollector_impl::update(CdmwTrace::TraceProducer* p_trace_producer)
    throw(CORBA::SystemException)
{
}


} // namespace Trace
} // namespace Cdmw
