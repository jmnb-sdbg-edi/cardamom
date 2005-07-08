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


#ifndef INCL_PERFORMANCELIBRARY_FUNCTIONSTAT_HPP 
#define INCL_PERFORMANCELIBRARY_FUNCTIONSTAT_HPP 

#include <string>

#include "Foundation/common/Exception.hpp"

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
*<p> This class is used as a container to store the Probe measure for an API call
*
*Features:
*<p> none
*
*@see ProbeCollector 
*@link  
*
*/
class FunctionStat 
{

    public:

        /**
        * Purpose:
        * <p> Constructor
        * 
        *@param apiName  A string to indicate the API name
        *@exception OutOfMemoryException
        */ 
        FunctionStat(const std::string &apiName)
                throw(Cdmw::OutOfMemoryException);

        /**
        * Purpose:
        * <p> Destructor
        * 
        */ 
        virtual 
        ~FunctionStat()
                throw();

		/**
        * Purpose:
        * <p>  Copy constructor
        * 
        *@param rhs A FunctionStat object to assign to current object.
        *@exception
        */ 
        FunctionStat (const FunctionStat& rhs)
                throw();

		/**
        * Purpose:
        * <p> Assignement operator
        * 
        *@param  rhs A FunctionStat object to assign to current object.
        *@return A reference to the current object.
        *@exception
        */ 
        FunctionStat&
        operator=(const FunctionStat& rhs)
                throw();

        
		/**
        * Purpose:
        * <p> update the object with the new elapsed time spent in the API
        * 
        *@param elapsedTime A long containing the new elapsed time
        *@return
        *@exception
        */
		void update(long elapsedTime)
			     throw();

        /**
        * Purpose:
        * <p> get the API function name
        * 
        *@param
        *@return A string containing the API function Name
        *@exception
        */
        const std::string &getFunctionName () const;

		/**
        * Purpose:
        * <p> get the minimum time spent in the API
        * 
        *@param
        *@return A long containing the minimum time spent in the API
        *@exception
        */
        const long getMinElapsedTime () const;

		/**
        * Purpose:
        * <p> get the maximum time spent in the API
        * 
        *@param
        *@return A long containing the maximum time spent in the API
        *@exception
        */
        const long getMaxElapsedTime () const;

		/**
        * Purpose:
        * <p> get the total of time spent in the API by all the threads
        * 
        *@param
        *@return A long containing the total time spent in the API
        *@exception
        */
        const long getSumElapsedTime () const;

		/**
        * Purpose:
        * <p> get the number of time spent the API has been by all the threads
        * 
        *@param
        *@return A long containing the number of time the API has been called
        *@exception
        */
        const long getHitCount () const;


    protected:


    private:

		/**
        * name of the API function
        */
		std::string m_functionName;

		/**
        * minimum elapsed time spent in API in microseconds
        */
		long m_minElapsedTime;

		/**
        * maximum elapsed time spent in API in microseconds
        */
		long m_maxElapsedTime;

		/**
        * total elapsed time spent in API in microseconds by all the threads
        */
		long m_sumElapsedTime;

		/**
        * number of times this API has been called by all the threads
        */
		long m_hitCount;
			
}; // End class FunctionStat 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string &FunctionStat::getFunctionName () const
{
  return m_functionName;
}

inline const long FunctionStat::getMinElapsedTime () const
{
  return m_minElapsedTime;
}

inline const long FunctionStat::getMaxElapsedTime () const
{
  return m_maxElapsedTime;
}

inline const long FunctionStat::getSumElapsedTime () const
{
  return m_sumElapsedTime;
}

inline const long FunctionStat::getHitCount () const
{
  return m_hitCount;
}

}; // End namespace Performance
}; // End namespace Cdmw
#endif // INCL_PERFORMANCELIBRARY_FUNCTIONSTAT_HPP

