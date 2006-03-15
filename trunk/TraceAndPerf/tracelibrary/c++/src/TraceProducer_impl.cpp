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


#include "tracelibrary/TraceProducer_impl.hpp"
#include "tracelibrary/AreaFlusher.hpp"


namespace Cdmw
{
namespace Trace
{

/**
*Implementation notes:  none 
*<p>
*Portability issues: none
*<p>
*/


TraceProducer_impl::TraceProducer_impl (AreaFlusher* pAreaFlusher, 
                                        FilterMngr*  pFilterMngr)
    throw()
    : m_pAreaFlusher (pAreaFlusher),
      m_pFilterMngr (pFilterMngr)
{

}



TraceProducer_impl::~TraceProducer_impl()
    throw()
{

}



//
//
//
CdmwTrace::TraceProducer::Ident 
             TraceProducer_impl::register_collector(CdmwTrace::Collector_ptr the_collector,
				                                    const char* the_objName,
													const char* the_mnemoName)
	throw(CdmwTrace::TraceProducer::InvalidRegistration,
          CORBA::SystemException)
{
    try 
	{
        return m_pAreaFlusher->register_collector (the_collector, the_objName, the_mnemoName);
    } 
	catch (const OutOfMemoryException&)
	{
        throw CORBA::NO_RESOURCES();
    }
}    




//
//
//
void TraceProducer_impl::unregister_collector(CdmwTrace::TraceProducer::Ident collectorId)
	throw(CdmwTrace::TraceProducer::InvalidRegistration,
          CORBA::SystemException)
{
    m_pAreaFlusher->unregister_collector (collectorId);
}    


//
//
//
CdmwTrace::TraceProducer::RegistrationSeq* TraceProducer_impl::get_registered_collectors ()
    throw(CORBA::SystemException)
{
    try 
	{
        return m_pAreaFlusher->get_registered_collectors ();
    } 
	catch (const OutOfMemoryException&)
	{
        throw CORBA::NO_RESOURCES();
    }
}    

//
//
//
CdmwTrace::TraceFilterSeq* TraceProducer_impl::get_trace_filters()
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


/**
 * Activate the specified level.
 */
// ECR-0123
void
TraceProducer_impl::activate_level(const char* a_componentName,
                                   const char* a_domain,
                                   CdmwTrace::Value a_Value)
    throw(CORBA::SystemException)
{
    m_pFilterMngr->activate_level(a_componentName, a_domain, a_Value);
}


/**
 * Deactivate the specified level.
 */
// ECR-0123
void
TraceProducer_impl::deactivate_level(const char* a_componentName,
                                     const char* a_domain,
                                     CdmwTrace::Value a_Value)
    throw(CORBA::SystemException)
{
    m_pFilterMngr->deactivate_level(a_componentName, a_domain, a_Value);
}


} // End namespace Trace
} // End namespace Cdmw

