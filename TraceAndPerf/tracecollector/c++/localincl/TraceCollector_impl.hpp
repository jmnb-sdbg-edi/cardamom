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


#ifndef INCL_TRACECOLLECTOR_IMPL_HPP 
#define INCL_TRACECOLLECTOR_IMPL_HPP 

#include <string>

#include "Foundation/orbsupport/CORBA.hpp"
#include "TraceAndPerf/idllib/CdmwTraceCollector.skel.hpp"

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

class CollectorMngr;
class FilterMngr;

/**
*Purpose:
*<p>    Default Collector implementation
*
*/
class TraceCollector_impl : public POA_CdmwTrace::Collector,
                            public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
		*@param pCollectorMngr  CollectorMngr in charge of collect trace messages
		*@param pFfilterMngr    FilterMngr in charge of determining if a Level/Domain
        *                       must be trace or not.
        */ 
        TraceCollector_impl(CollectorMngr* pCollectorMngr, FilterMngr* pFilterMngr)
                throw();


        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~TraceCollector_impl()
                throw();


		/**
		* Purpose:
		* <p> returns the list of all specified levels for filtering
		* 
		*@return the filters
		*
		*@exception CORBA::SystemException
		*/ 
		virtual 
		CdmwTrace::TraceFilterSeq* get_trace_filters()
			    throw(CORBA::SystemException);


	    /**
		* Purpose:
		* <p> Activate the specified level. All Trace specified
		*     as in this Domain/Level are sent to Collectors
		* 
		*@param a_domain the domain name
		*@param a_Value the level value
		*
		*@exception CORBA::SystemException
		*/ 
		virtual 
		void activate_level (const char* a_domain,
		                     CdmwTrace::Value a_Value)
			    throw(CORBA::SystemException);

				        

	    /**
	    * Purpose:
	    * <p> Deactivate the specified level. All Trace specified
	    *   as in this Domina/Level are no more sent to Collectors
	    * 
	    *@param a_domain the domain name
	    *@param a_Value the level value
	    *
	    *@exception CORBA::SystemException
	    */ 
	    virtual 
	    void deactivate_level (const char* a_domain,
	                           CdmwTrace::Value a_Value)
	            throw(CORBA::SystemException);

						
        /**
        * Purpose:
        * <p> transmit trace messages to the collector
        * 
	    *@param the_messages_header contains the general header for messages
	    *@param the_messages contains sequence of formatted messages
		*
	    *@exception CORBA::SystemException
        */ 
        virtual 
        void put_formatted_messages (const CdmwTrace::MessagesHeader& the_messages_header,
                                     const CdmwTrace::FormattedMessageSeq& the_messages)
                throw(CORBA::SystemException);




        /**
        * Purpose:
        * <p>
        * 
        *@param trace_producer  trace producer object
        *
	    *@exception CORBA::SystemException
        */ 
        virtual 
        void update (CdmwTrace::TraceProducer* p_trace_producer)
                throw(CORBA::SystemException);


    protected:


    private:
        
        TraceCollector_impl(const TraceCollector_impl& rhs)
                throw();


        TraceCollector_impl&
        operator=(const TraceCollector_impl& rhs)
                throw();

		/**
		* Contains the reference on the object in charge of 
		* collecting the trace message
		*/
		CollectorMngr*  m_pCollectorMngr;

		/**
		* Contains the reference on the object in charge of 
		* informing the trace system, if an information, from
		* a domain must be traced or not
		*/
		FilterMngr*  m_pFilterMngr;
				

}; // End class TraceCollector 

}; // End namespace Trace
}; // End namespace Cdmw
#endif // INCL_TRACECOLLECTOR_IMPL_HPP

