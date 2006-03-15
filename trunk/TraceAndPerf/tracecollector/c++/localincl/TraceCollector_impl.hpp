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


#ifndef _CDMW_TRC_TRACECOLLECTOR_IMPL_HPP_
#define _CDMW_TRC_TRACECOLLECTOR_IMPL_HPP_


#include <Foundation/orbsupport/CORBA.hpp>

#include <TraceAndPerf/idllib/CdmwTraceCollector.skel.hpp>
#include <TraceAndPerf/idllib/CdmwTraceTraceProducer.skel.hpp>


namespace Cdmw
{
namespace Trace
{


class CollectorMngr;
class FilterMngr;


/**
 * Default implementation of the local trace collector.
 */
class TraceCollector_impl: public POA_CdmwTrace::Collector,
                           public PortableServer::RefCountServantBase
{
    public:
        /**
         * Constructor.
         *
         * @param pCollectorMngr the collector manager.
         * @param pFfilterMngr the filter manager.
         */
        TraceCollector_impl(CollectorMngr* pCollectorMngr,
                            FilterMngr* pFilterMngr)
            throw();


        /**
         * Destructor.
         */
        virtual
        ~TraceCollector_impl()
            throw();


        /**
         * Get the list of filters.
         *
         * @return the list of filters.
         *
         * @exception CORBA::SystemException.
         */
        virtual CdmwTrace::TraceFilterSeq*
        get_trace_filters()
            throw(CORBA::SystemException);


        /**
         * Activate the specified level.
         * Traces that match the filter are sent to the Collectors.
         *
         * @param a_componentName the component name
         * @param a_domain the domain name
         * @param a_Value the level value
         *
         * @exception CORBA::SystemException
         */
        virtual void
        activate_level(const char* a_componentName, // ECR-0123
                       const char* a_domain,
                       CdmwTrace::Value a_Value)
            throw(CORBA::SystemException);


        /**
         * Deactivate the specified level.
         * Traces that match the filter are discarded and not sent
         * to the Collectors.
         *
         * @param a_componentName the component name
         * @param a_domain the domain name
         * @param a_Value the level value
         *
         * @exception CORBA::SystemException
         */
        virtual void
        deactivate_level(const char* a_componentName, // ECR-0123
                         const char* a_domain,
                         CdmwTrace::Value a_Value)
            throw(CORBA::SystemException);


        /**
         * Write messages.
         *
         * @param the_messages_header the general header for messages.
         * @param the_messages sequence of formatted messages.
         *
         * @exception CORBA::SystemException.
         */
        virtual void
        put_formatted_messages(
                const CdmwTrace::MessagesHeader& the_messages_header,
                const CdmwTrace::FormattedMessageSeq& the_messages)
            throw(CORBA::SystemException);


        /**
         * Update.
         *
         * @param trace_producer the trace producer object.
         *
         * @exception CORBA::SystemException.
         */
        virtual void
        update(CdmwTrace::TraceProducer* p_trace_producer)
            throw(CORBA::SystemException);


        /**
         * Set the global trace collectors.
         *
         * @param collectorList the list of global trace collectors.
         */
        // ECR-0145
        virtual void
        set_global_collectors(
            std::list<CdmwTrace::TraceProducer::CollectorData>& collectorList);


    private:
        /**
         * Copy constructor.
         */
        TraceCollector_impl(const TraceCollector_impl& rhs)
            throw();


        /**
         * Assignment operator.
         */
        TraceCollector_impl&
        operator=(const TraceCollector_impl& rhs)
            throw();


    private:
        // the collector manager (collects the messages).
        CollectorMngr* m_pCollectorMngr;

        // the filter manager (determines whether a message should
        // be traced, i.e. saved into the log files).
        FilterMngr* m_pFilterMngr;

        // list of global collectors (ECR-0145).
        std::list<CdmwTrace::TraceProducer::CollectorData> m_globalCollectors;
};


} // namespace Trace
} // namespace Cdmw


#endif // _CDMW_TRC_TRACECOLLECTOR_IMPL_HPP_
