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


#ifndef INCL_TRACE_TRACEPRODUCER_IMPL_HPP 
#define INCL_TRACE_TRACEPRODUCER_IMPL_HPP 

#include "Foundation/orbsupport/CORBA.hpp"
#include "TraceAndPerf/idllib/CdmwTraceTraceProducer.skel.hpp"
#include "TraceAndPerf/idllib/CdmwTraceCollector.stub.hpp"
#include "TraceAndPerf/tracelibrary/FilterMngr.hpp"


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


class AreaFlusher;    


/**
*Purpose:
*<p>    This class implements the TraceProducer interface. This 
*       class allows collector to register to receive message
*       and register the process into the Repository
*
*Features:
*<p> Thread safe
*
*
*/
class TraceProducer_impl : public virtual POA_CdmwTrace::TraceProducer,
		                   public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param pAreaFlusher  In charge of flushing data to Collectors
        *@param filterMngr    FilterMngr in charge of determining if a Level/Domain
        *                     must be trace or not.
        *
        */ 
        TraceProducer_impl (AreaFlusher* pAreaFlusher,
                            FilterMngr* pFilterMngr)
                throw();

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~TraceProducer_impl()
                throw();



        /**
        * Purpose:
        * <p> A Collector wishing receive data must call this function 
        * 
        *@param the_collector The Collector to be notified when data are
        *       available.
		*@param the_objName   The name of the collector object
		*@param the_mnemoName The mnemonic name that may be used as a help
		*                      to identify the collector.
        *@param the_name The name of notified Collector
        *
        *@return The id to be used to unregister the Collector
        *
        *@exception CdmwTrace::TraceProducer::InvalidRegistration
		*           CORBA::SystemException
        */ 
        virtual 
        CdmwTrace::TraceProducer::Ident 
		             register_collector(CdmwTrace::Collector_ptr the_collector,
						                const char* the_objName,
										const char* the_mnemoName)
            throw(CdmwTrace::TraceProducer::InvalidRegistration,
				  CORBA::SystemException);


        

        /**
        * Purpose:
        * <p> A Collectors no more interested in receiving data must
        *   called this function with the ide received from the
        *   register_collector() call.
        * 
        *@param collectorId the id assigned to the Collector
        *
        *@exception CdmwTrace::TraceProducer::InvalidRegistration
		*           CORBA::SystemException
        *
        */ 
        virtual 
        void unregister_collector(CdmwTrace::TraceProducer::Ident collectorId)
            throw(CdmwTrace::TraceProducer::InvalidRegistration,
				  CORBA::SystemException);


        /**
        * Purpose:
        * <p> get the sequence of registered collectors 
        * 
        *@return The sequence of Collectors
        *
        *@exception CORBA::SystemException
        */ 
        virtual 
		CdmwTrace::TraceProducer::RegistrationSeq* get_registered_collectors ()
            throw(CORBA::SystemException);
		

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
        *     as in this Domain/Level are no more sent to Collectors
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


    protected:


    private:

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        */ 
        TraceProducer_impl(const TraceProducer_impl& rhs)
                throw();



        /**
        * Purpose:
        * <p> Assignement operator 
        * 
        */ 
        TraceProducer_impl& operator=(const TraceProducer_impl& rhs)
                throw();


        

        /**
        * The area flusher in charge sending information to collector
        */
        AreaFlusher* m_pAreaFlusher;


        
        /**
        * Contains the reference on the object in charge of 
        * informing the trace system, if an information, from
        * a domain must be traced or not
        */
        FilterMngr*  m_pFilterMngr;
        

}; // End class TraceProducer_impl 

} // End namespace Trace
} // End namespace Cdmw
#endif // INCL_TRACE_TRACEPRODUCER_IMPL_HPP

