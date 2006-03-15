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


#ifndef INCL_PERFORMANCELIBRARY_PROBECOLLECTOR_HPP 
#define INCL_PERFORMANCELIBRARY_PROBECOLLECTOR_HPP 

#include <string>
#include <map>

#include "Foundation/common/Exception.hpp"
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/osthreads/ReaderWriterLock.hpp"
#include "TraceAndPerf/performancelibrary/Probe.hpp"
#include "performancelibrary/FunctionStat.hpp"

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

typedef std::map<std::string, FunctionStat*, std::less<std::string> > FuncStatMap;

class Probe;

/**
*Purpose:
*<p>    This singleton class is used to store the time spent for each registered API name.
*       Statistics are recorded in Functionstat object for one API name. 
*       FunctionStat object is created if it is the first time the API is measured or is updated with
*       new measures. FunctionStat objects are holded in a map with API name as key.
*       Statistics from all FunctionStat objects can be written in a log file.
*
*Features:
*<p> Thread safety
*
*@see FunctionStat 
*@link  
*
*/
class ProbeCollector 
{

    public:

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~ProbeCollector()
                throw();

		/**
        * Purpose:
        * <p> get the singleton instance of object
        * 
        *@param
        *@return A ProbeCollector pointer on singleton object
        *@exception OutOfMemoryException
        */
		static ProbeCollector* GetInstance()
			     throw(Cdmw::OutOfMemoryException);

		/**
        * Purpose:
        * <p> manage the input Probe object or update it if already managed
        * 
        *@param A Probe object to manage or to update
        *@return
        *@exception OutOfMemoryException
        */
		void addProbe (const Probe &probe)
			     throw(Cdmw::OutOfMemoryException);

		/**
        * Purpose:
        * <p> write the statistics recorded in Probe objects into file
        * 
        *@param A string indicating the name of file to write
        *@return
        *@exception BadParameterException
        */
		void writeStatistic (const std::string &logFileName)
			     throw(Cdmw::BadParameterException);

		/**
        * Purpose:
        * <p> get the FunctionStat corresponding to the Probe
        * 
        *@param A Probe object
        *@return A pointer to a FunctionStat object corresponding to input Probe object
        *@exception
        */
		const Performance::FunctionStat* getFunctionStat (const Probe &probe)
			     throw();

		/**
        * Purpose:
        * <p> get the FunctionStat corresponding to the API name
        * 
        *@param A string corresponding to the API name
        *@return A pointer to a FunctionStat object corresponding to input API name
        *@exception
        */
		const Performance::FunctionStat* getFunctionStat (const std::string &apiName)
			     throw();

		/**
        * Purpose:
        * <p> to get if performance collector is activated
        * 
        *@param
        *@return A boolean to indicate the activation
        *@exception
        */
		bool isActivated ()
			     throw();

		/**
		** Purpose:
		** <p> store activation setting
	    ** 
		**@param value containing a boolean 
		**@return
		**@exception
		**/
	    void setActivity (bool value)
			     throw();

    protected:

		/**
        * Purpose:
        * <p> Constructor
        * 
        *@param
        *@exception
        */ 
        ProbeCollector()
			     throw();

    private:

		/**
        * Purpose:
        * <p>  Copy constructor
        * 
        *@param rhs A ProbeCollector object to assign to current object.
        *@exception
        */ 
        ProbeCollector (const ProbeCollector& rhs)
                throw();

		/**
        * Purpose:
        * <p> Assignement operator
        * 
        *@param  rhs A ProbeCollector object to assign to current object.
        *@return A reference to the current object.
        *@exception
        */ 
        ProbeCollector&
        operator=(const ProbeCollector& rhs)
                throw();


		/**
        * singleton instance
        */
		static ProbeCollector *M_pInstance;

		/**
        * singleton mutex
        */
		static OsSupport::Mutex M_mutex;

		/**
        * performance activation
        */
		bool m_active;

		/**
        * map of Function Stat objects indexed by API function name
        */
		FuncStatMap m_functionStat;

		/**
        * Function Stat map read/writer lock guard
        */
		OsSupport::ReaderWriterLock m_readWriteMapLock;

}; // End class ProbeCollector 

}; // End namespace Performance
}; // End namespace Cdmw
#endif // INCL_PERFORMANCELIBRARY_PROBECOLLECTOR_HPP

