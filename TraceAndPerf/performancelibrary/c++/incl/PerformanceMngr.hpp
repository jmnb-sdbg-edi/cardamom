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


#ifndef INCL_PERFORMANCELIBRARY_PERFORMANCEMNGR_HPP 
#define INCL_PERFORMANCELIBRARY_PERFORMANCEMNGR_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "TraceAndPerf/idllib/CdmwPerformanceAdmin.stub.hpp"

/**
* Root namespace for CDMW runtime.
*/
namespace Cdmw
{
/**
* Namespace for CDMW Performance types and data.
*/		
namespace Performance
{

	
class ProbeCollector;

/**
*Purpose:
*<p>    This class is the central class of the Cdmw Performance service. This
*       class is used as a singleton to provide access to the performance system
*       from everywhere. The PerformanceMngr is in charge of:
*           - allocate and initialize the Probe Collector managing the api performance 
*             statistics
*           - initialised the CORBA object in charge of handling client Performance activation
*             requests (PerformanceAdmin).
*
*Features:
*<p>   none
*
*@see  PerformanceAdmin_impl
*@see  ProbeCollector
*
*/
class PerformanceMngr
{

    public:

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~PerformanceMngr()
            throw();

        /**
        * Purpose:
        * <p> Initialize the Performance library. This function must be called
        *   before any other call. The Performance service must be closed
        *   by a call to cleanup().
        * 
        *@param pPerfomancePOA  A POA ptr the performance admin servant is registered
		*       (POA Policy must be set to RETAIN and SYSTEM_ID)
		*@param pPerfomanceFile  A string to indicate the Performance file name to log
		*       performance statistics from Probe Collector
		*       
		*
		*@exception BadParameterException 
        *           OutOfMemoryException      
        */
        static
        CdmwPerformanceAdmin::PerformanceAdmin_ptr init(PortableServer::POA_ptr performancePOA, 
						                                 const std::string &performanceFile)                
            throw (Cdmw::BadParameterException,
                   Cdmw::OutOfMemoryException);

        /**
        * Purpose:
        * <p> Write the performance statistics into the performance file,
        * free all allocated memory and stop the performance service.
        * 
        *@exception BadParameterException Indicates ahtt the performance file
        *           specified as parameter of the init() operation cannot be open.
        */ 
        static
        void cleanup()
            throw (Cdmw::BadParameterException);

            
        /**
        * Purpose:
        * <p> Activate the performance measurements.
        *     (by default, they are not active)
        *
        *@exception CORBA::BAD_INV_ORDER with BAD_INV_ORDERInitNotCalled
        *              Indicates that the init() operation has not been called.
        */
        static
        void activate()
            throw (CORBA::SystemException);
            
        /**
        * Purpose:
        * <p> Deactivate the performance measurements.
        *
        *@exception CORBA::BAD_INV_ORDER with BAD_INV_ORDERInitNotCalled
        *              Indicates that the init() operation has not been called.
        */
        static 
        void deactivate()
            throw (CORBA::SystemException);
            

        /**
        * Purpose:
        * <p> Write current performance measurements into performance file.
        *
        *@exception CORBA::BAD_INV_ORDER with BAD_INV_ORDERInitNotCalled
        *              Indicates that the init() operation has not been called.
        *@exception BadParameterException Indicates ahtt the performance file
        *           specified as parameter of the init() operation cannot be open.
        */
        static
        void writeStatistic()
            throw (CORBA::SystemException,
                   Cdmw::BadParameterException);
            
            
    protected: 
        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param
        *@exception
        */ 
        PerformanceMngr()               
			   	throw();


    private:
		
        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        *@param
        *@exception
        */ 
        PerformanceMngr(const PerformanceMngr& rhs)
                throw();


        /**
        * Purpose:
        * <p> Assignement operator
        * 
        *@param
        *@return
        *@exception
        */ 
        PerformanceMngr&
        operator=(const PerformanceMngr& rhs)
                throw();



        /**
        * This variable is set to true, since the init() function has been called.
        * The cleanup() function reset this variable to false.
        */
        static  
        bool M_initDone;

		/**
		* Contains a pointer to the Probe Collector
		*/
		static
		ProbeCollector *M_pProbeCollector;
		
        /**
        * Contains the reference handling for Performance Admin servant
        */
		static
        CdmwPerformanceAdmin::PerformanceAdmin_var M_performanceAdmin;

        /**
        * Contains name of the Performance log file
        */
		static
		std::string M_perfoLogFile;


}; // End class PerformanceMngr 

}; // End namespace Performance
}; // End namespace Cdmw

#endif // INCL_PERFORMANCELIBRARY_PERFORMANCEMNGR_HPP

