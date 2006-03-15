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


#include "TraceAndPerf/performancelibrary/PerformanceMngr.hpp"
#include "Foundation/common/Exception.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"

#include "performancelibrary/PerformanceAdmin_impl.hpp"
#include "performancelibrary/ProbeCollector.hpp"

namespace Cdmw
{
namespace Performance
{

/**
*Implementation notes:  none 
*<p>
*Portability issues: none
*/

/**
* This variable is set to true, when the PerformanceMngr::init() function
* has been called.
*/
bool PerformanceMngr::M_initDone=false;

/**
* pointer to the Probe Collector
*/
ProbeCollector* PerformanceMngr::M_pProbeCollector=NULL;

/**
* Contains the reference handling for Performance Admin servant
*/
CdmwPerformanceAdmin::PerformanceAdmin_var PerformanceMngr::M_performanceAdmin=
		                                               CdmwPerformanceAdmin::PerformanceAdmin::_nil();

/**
* name of the Performance log file
*/
std::string PerformanceMngr::M_perfoLogFile="";
		
//
// Initialise the performance library
//
CdmwPerformanceAdmin::PerformanceAdmin_ptr
    PerformanceMngr::init(PortableServer::POA_ptr performancePOA,
					      const std::string &performanceFile)
    throw (Cdmw::BadParameterException,
           Cdmw::OutOfMemoryException)
{
	try
	{
      // The user try to initialise twice the performance library
      CDMW_ASSERT (!M_initDone);
    
	  // We create the Probe Collector object 
	  // in charge of recording the API performance
	  M_pProbeCollector = ProbeCollector::GetInstance(); 	            

      // Register Performance log file name
	  M_perfoLogFile = performanceFile;

	  // We must create the Servant to handle performance activation
	  // (ref count is incremented)
	  PerformanceAdmin_impl *pServant = new PerformanceAdmin_impl();
	
	  // create an object var to take pointer ownership
	  // (ref count will be decremented when var will be destroyed at the method end)
	  PortableServer::ServantBase_var servant_var = pServant;

	  // activate servant on POA
	  // (ref count is incremented)
	  PortableServer::ObjectId_var oid = performancePOA->activate_object(pServant);
	  CORBA::Object_var object = performancePOA->id_to_reference(oid.in());
	  M_performanceAdmin = CdmwPerformanceAdmin::PerformanceAdmin::_narrow(object.in());
	}

    catch (const std::bad_alloc&)
    {
	  CDMW_THROW (Cdmw::OutOfMemoryException);
	}
	catch (const PortableServer::POA::WrongPolicy&)
	{
	  CDMW_THROW2(Cdmw::BadParameterException, "Wrong POA Policy for activate_object","NULL");
	}
		
    M_initDone=true;

    // We return the reference on the object managing communication with 
    // collector, it will be registered in the repository by the caller
    return CdmwPerformanceAdmin::PerformanceAdmin::_duplicate (M_performanceAdmin.in());
}

//
// Free memory and terminate the Performance service
//
void PerformanceMngr::cleanup()
    throw (Cdmw::BadParameterException)
{
    if (M_initDone)
    {
      M_initDone = false;

	  // write statistics recorded by Probe Collector in log file
	  M_pProbeCollector->writeStatistic(M_perfoLogFile);

	  // delete Probe Collector
      delete M_pProbeCollector;	 
	  
      M_pProbeCollector = NULL;	 
      
      M_performanceAdmin = CdmwPerformanceAdmin::PerformanceAdmin::_nil();
    }
}

//
// Activate the performance measurements
//
void PerformanceMngr::activate()
    throw (CORBA::SystemException)
{
    if (!M_initDone) {
        throw CORBA::BAD_INV_ORDER(Cdmw::OrbSupport::BAD_INV_ORDERInitNotCalled,
                                   CORBA::COMPLETED_NO);
    }
    M_pProbeCollector->setActivity(true);
}


//
// Deactivate the performance measurements
//
void PerformanceMngr::deactivate()
    throw (CORBA::SystemException)
{
    if (!M_initDone) {
        throw CORBA::BAD_INV_ORDER(Cdmw::OrbSupport::BAD_INV_ORDERInitNotCalled,
                                   CORBA::COMPLETED_NO);
    }
    M_pProbeCollector->setActivity(false);
}


//
// Write the performance measurements into performance file
//
void PerformanceMngr::writeStatistic()
    throw (CORBA::SystemException,
           Cdmw::BadParameterException)
{
    if (!M_initDone) {
        throw CORBA::BAD_INV_ORDER(Cdmw::OrbSupport::BAD_INV_ORDERInitNotCalled,
                                   CORBA::COMPLETED_NO);
    }
    M_pProbeCollector->writeStatistic(M_perfoLogFile);
}



//
// Ctor
// 
PerformanceMngr::PerformanceMngr()
    throw()
{
}


//
// Dtor
// 
PerformanceMngr::~PerformanceMngr()
    throw()
{
}


}; // End namespace Performance
}; // End namespace Cdmw

