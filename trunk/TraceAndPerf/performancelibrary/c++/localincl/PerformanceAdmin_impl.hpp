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


#ifndef INCL_PERFORMANCEADMIN_PERFORMANCEADMIN_IMPL_HPP 
#define INCL_PERFORMANCEADMIN_PERFORMANCEADMIN_IMPL_HPP 

#include "Foundation/common/Exception.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "TraceAndPerf/idllib/CdmwPerformanceAdmin.skel.hpp"
#include "performancelibrary/ProbeCollector.hpp"

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

/**
*Purpose:
*<p>    This class is the implementation of the Performance Admin servant
*
*Features:
*<p> none
*
*@see ProbeCollector 
*@link 
*
*/

class PerformanceAdmin_impl : public POA_CdmwPerformanceAdmin::PerformanceAdmin,
                              public PortableServer::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param
        *@exception OutOfMemoryException
        */ 
        PerformanceAdmin_impl()
		        throw(Cdmw::OutOfMemoryException);

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~PerformanceAdmin_impl()
                throw();


        /**
        * Purpose:
        * <p> activate the Probe Collector measure
        * 
        *@param
        *@return
        *@exception
        */ 
		void activate ()
			    throw();

		/**
        * Purpose:
        * <p> deactivate the Probe Collector measure
        * 
        *@param
        *@return
        *@exception
        */ 
		void deactivate ()
			    throw();


    protected:


    private:

		/**
        * pointer to the Probe collector singleton
        */
		Performance::ProbeCollector *m_pcollector;

}; // End class PerformanceAdmin_impl 

}; // End namespace Performance
}; // End namespace Cdmw
#endif // INCL_PERFORMANCEADMIN_PERFORMANCEADMIN_IMPL_HPP

