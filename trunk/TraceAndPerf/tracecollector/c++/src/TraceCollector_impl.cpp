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

#include "tracecollector/CollectorMngr.hpp"
#include "tracecollector/TraceCollector_impl.hpp"


namespace Cdmw
{
namespace Trace
{


/**
 * Constructor.
 */
TraceCollector_impl::TraceCollector_impl(CollectorMngr* pCollectorMngr,
                                         FilterMngr* pFilterMngr)
    throw()
        : m_pCollectorMngr(pCollectorMngr),
          m_pFilterMngr(pFilterMngr)
{
}


/**
 * Destructor.
 */
TraceCollector_impl::~TraceCollector_impl()
    throw()
{
}


/**
 * Get the list of filters.
 */
CdmwTrace::TraceFilterSeq*
TraceCollector_impl::get_trace_filters()
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
TraceCollector_impl::activate_level(const char* a_componentName, // ECR-0123
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
TraceCollector_impl::deactivate_level(const char* a_componentName, // ECR-0123
                                      const char* a_domain,
                                      CdmwTrace::Value a_Value)
    throw(CORBA::SystemException)
{
    m_pFilterMngr->deactivate_level(a_componentName, a_domain, a_Value);
}


/**
 * Write messages.
 */
void
TraceCollector_impl::put_formatted_messages(
        const CdmwTrace::MessagesHeader& the_messages_header,
        const CdmwTrace::FormattedMessageSeq& the_messages)
    throw(CORBA::SystemException)
{
    try {
        // write the message locally.
        for (size_t iMsg = 0; iMsg < the_messages.length(); ++iMsg) 
        {
            m_pCollectorMngr->writeMessage(the_messages_header,
                                           the_messages[iMsg]);
        }

        // resend the messages to the global collectors.
        
        // filter the messages to send to the global trace collector
        CdmwTrace::FormattedMessageSeq_var messagesToTransfer =
                   new CdmwTrace::FormattedMessageSeq();
                   
        unsigned int inxTransfMessage = 0;
        
        // for each message to transfer
        for (size_t iMsg = 0; iMsg < the_messages.length(); ++iMsg) 
        {
            // write out in the sequence the message only 
            // if it passes at least one filter
            
            const CdmwTrace::FormattedMessage& fmtMessage = the_messages[iMsg];
            std::string domain = fmtMessage.header.the_level.the_domain.in();

            std::string componentName =
                 fmtMessage.header.the_level.the_component_name.in();
                 
            if (CollectorMngr::Is_to_be_traced(
                                       componentName, domain,
                                       fmtMessage.header.the_level.the_value))
            {
                inxTransfMessage++;
                messagesToTransfer->length(inxTransfMessage);
                messagesToTransfer[inxTransfMessage-1] = fmtMessage;
            }
        }
        
        // transfer messages if any
        if (messagesToTransfer->length() > 0)
        {
            typedef std::list<CdmwTrace::TraceProducer::CollectorData>::const_iterator list_iter;
            list_iter it_end = m_globalCollectors.end();

            for (list_iter it = m_globalCollectors.begin(); it != it_end; ++it) 
            {
                (*it).the_collector->put_formatted_messages(the_messages_header,
                                                            messagesToTransfer.in());
            }
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
TraceCollector_impl::update(CdmwTrace::TraceProducer* p_trace_producer)
    throw(CORBA::SystemException)
{
}


/**
 * Set the global trace collectors.
 */
void
TraceCollector_impl::set_global_collectors(
    std::list<CdmwTrace::TraceProducer::CollectorData>& collectorList)
{
    m_globalCollectors = collectorList;
}


} // namespace Trace
} // namespace Cdmw
