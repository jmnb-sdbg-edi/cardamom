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


#ifndef INCL_TRACE_SIM_COLLECTOR_HPP 
#define INCL_TRACE_SIM_COLLECTOR_HPP 

#include <string>

#include "Foundation/orbsupport/CORBA.hpp"
#include "TraceAndPerf/idllib/CdmwTraceCollector.skel.hpp"

namespace Cdmw
{
namespace Trace
{

/**
*Purpose:
*<p>    Simulate a Collector implementation
*
*/
class SIM_Collector : public virtual POA_CdmwTrace::Collector,
		              public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        SIM_Collector()
                throw();


        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~SIM_Collector()
                throw();


		virtual 
		CdmwTrace::TraceFilterSeq* get_trace_filters()
			throw(CORBA::SystemException);


        virtual	void
        activate_level(const char* a_componentName, // ECR-0123
                       const char* a_domain,
                       CdmwTrace::Value a_Value)
            throw(CORBA::SystemException);


        virtual void
        deactivate_level(const char* a_componentName, // ECR-0123
                         const char* a_domain,
                         CdmwTrace::Value a_Value)
            throw(CORBA::SystemException);


        virtual 
        void put_formatted_messages (const CdmwTrace::MessagesHeader& the_messages_header,
                                     const CdmwTrace::FormattedMessageSeq& the_messages)
            throw(CORBA::SystemException);


        virtual
        void update (CdmwTrace::TraceProducer* p_trace_producer)
            throw(CORBA::SystemException);


        
        const CdmwTrace::MessagesHeader& get_MessagesHeader();
        
        
        const CdmwTrace::FormattedMessageSeq& get_FormattedMessageSeq();
        


    protected:


    private:
        
        /**
        * Purpose:
        * <p>  [Copy constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        SIM_Collector(const SIM_Collector& rhs)
                throw();


        /**
        * Purpose:
        * <p> [ Assignement operator description if necessary]
        * 
        *@param
        *@return
        *@exception
        */ 
        SIM_Collector&
        operator=(const SIM_Collector& rhs)
                throw();



        CdmwTrace::MessagesHeader_var m_the_messages_header;
            
        CORBA::ULong m_nbMessages;
        CdmwTrace::FormattedMessageSeq_var m_the_messages;



}; // End class SIM_Collector 

}; // End namespace Trace
}; // End namespace Cdmw
#endif // INCL_TRACE_SIM_COLLECTOR_HPP

