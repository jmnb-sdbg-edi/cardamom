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


#ifndef INCL_PERFORMANCELIBRARY_PROBE_HPP 
#define INCL_PERFORMANCELIBRARY_PROBE_HPP 

#include <string>

#include "Foundation/common/Exception.hpp"
#include "Foundation/ossupport/OS.hpp"

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
*<p>This class is used to store the time to process one API call.
*
*Features:
*<p>Time is stored in a thread-safe way
*
*@see ProbeCollector 
*@link 
*
*/
class Probe 
{

    public:

        /**
        * Purpose:
        * <p> Constructor and start time measure if auto set
        * 
        *@param pApiName  A string to indicate the API name to measure
		*       autoStart A boolean to indicate to store start time automatically by constructor
		*                 and to stop time automatically by destructor.
        *@exception BadParameterException
		*           OutOfMemoryException
		*           InternalErrorException
        */ 
        Probe(const char *pApiName, bool autoStart=true)
                throw(Cdmw::BadParameterException,
				      Cdmw::OutOfMemoryException,
				      Cdmw::InternalErrorException);

        /**
        * Purpose:
        * <p> Destructor, stop time measure if auto set
        * 
        */ 
        virtual 
        ~Probe()
                throw();

        /**
        * Purpose:
        * <p>  Copy constructor
        * 
        *@param rhs A Probe object to assign to current object.
        *@exception
        */ 
        Probe (const Probe& rhs)
                throw();

        /**
        * Purpose:
        * <p> Assignement operator
        * 
        *@param  rhs A Probe object to assign to current object.
        *@return A reference to the current object.
        *@exception
        */ 
        Probe&
        operator=(const Probe& rhs)
                throw();

        /**
        * Purpose:
        * <p> start the time measure
        * 
        *@param
        *@return
        *@exception InternalErrorException
        */
        void start()
			    throw(Cdmw::InternalErrorException);

        /**
        * Purpose:
        * <p> stop the time measure
        * 
        *@param
        *@return
        *@exception InternalErrorException
		*           OutOfMemoryException
        */
        void stop()
			    throw(Cdmw::InternalErrorException,
			          Cdmw::OutOfMemoryException);

        /**
        * Purpose:
        * <p> get the elapsed time
        * 
        *@param
        *@return the elapsed time in micro seconds
        *@exception
        */
		long getElapsedTime() const
			    throw();

        /**
        * Purpose:
        * <p> get the API name
        * 
        *@param
        *@return A string containing the API Name
        *@exception
        */
        const std::string &getApiName () const;

        /**
        * Purpose:
        * <p> set the API name
        * 
        *@param value A string containing the API name
        *@return
        *@exception
        */
        void setApiName (const std::string &value);


    protected:


    private:

		enum ProbeStatus 
        {
          PROBE_IDLE,
          PROBE_STARTED,
		  PROBE_STOPPED
        };

		/**
        * flag to indicate that time is recorded by constructor and destructor
        */
		bool m_autoStart;

		/**
        * pointer to the Probe collector singleton
        */
		ProbeCollector *m_pCollector;

		/**
        * name of the API
        */
		std::string m_apiName;

		/**
        * probe status : idle or started
        */
	   	ProbeStatus m_probeStatus;
        
        /**
		* time of start
		*/
		OsSupport::OS::Timeval m_startTime;

		/**
		* time of stop
		*/
		OsSupport::OS::Timeval m_stopTime;

}; // End class Probe 

//## Get and Set Operations for Class Attributes (inline)

inline const std::string &Probe::getApiName () const
{
  return m_apiName;
}

inline void Probe::setApiName (const std::string &value)
{
  m_apiName = value;
}


}; // End namespace Performance
}; // End namespace Cdmw
#endif // INCL_PERFORMANCELIBRARY_PROBE_HPP

