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

#ifndef INCL_CDMW_FT_SUPERVISION_COMMON_LIVELINESSREPOSITORY_HPP
#define INCL_CDMW_FT_SUPERVISION_COMMON_LIVELINESSREPOSITORY_HPP

// private include files
#include "common/CdmwFTMulticastMonitorable.stub.hpp"

// include files
#include "Foundation/osthreads/ReaderWriterLock.hpp"

#include "ftmonitoring/common/CommonTypeDefinitions.hpp"

#include <map>

namespace Cdmw {
namespace FT {
namespace Supervision {
  
    /**
    *Purpose:
    *<p> The type used to enumerate the different messages that
    * can be sent by the supervision
    */
    enum ResponseType {
      UNKNOWN,
      ACKNOWLEDGEMENT,
      CANDIDATURE
    };

// class definition
/**
*Purpose:
*<p>    This class implements operations to handle the liveliness repository
*       used to record answers of the monitored hosts (both simple and mutual)
*
*Features:
*<p> Thread safe, exception safe
*
*/
class LivelinessRepository {

public:

    /**
    * Purpose:
    * <p> Constructor
    */
    LivelinessRepository();
   
    /**
    * Purpose:
    * <p> Destructor
    */
    virtual ~LivelinessRepository();

    /**
    * Purpose: 
    * <p> Retrieve (in the repository) the list of faulty slaves since last master
    * interrogation, suppress them from the liveliness map and reset the other
    * entry responses in the map
    * Counter is used to set to UNKNOWN only the entries from last previous
    * interrogation (for mutual monitoring to avoid double failure notif)
    *
    *@param LocationList& faultySlaves the returned list of faulty slaves
    *@param unsigned int counterVal the counter explained above
    */
    void get_faultySlaves(LocationList& faultySlaves,
		  	 unsigned int counterVal);

    /**
    * Purpose: 
    * <p> Update the liveliness map with the response of the location
    * 
    *@param const char * location to update
    *@param CdmwFT::MonitoringLibrary::RequestType response of the location
    *@param unsigned int counterVal : see previous explanation
    *
    *@return bool true if the location is new in the map
    */ 
    bool update_liveliness(const char * location,
		          CdmwFT::MonitoringLibrary::RequestType requestType,
			  unsigned int counterVal);

    /**
    * Purpose: 
    * <p> Set response to UNKNOWN for the given location
    *
    *@param const char * location to update
    */ 
    void reset_liveliness(const CdmwFT::MonitoringLibrary::Location location)
      throw (CdmwFT::MonitoringLibrary::NotFound, CORBA::SystemException);

    /**
    * Purpose: 
    * <p> Erase the given location from the liveliness map
    *
    *@param const char * location to update
    */ 
    void remove_liveliness(const CdmwFT::MonitoringLibrary::Location location)
      throw (CORBA::SystemException);

    /**
    * Purpose: 
    * <p> Set response to UNKNOWN for all the locations in the map
    * (i.e. all started locations)
    */ 
    void reset_livelinesses()
      throw (CORBA::SystemException);

    /**
    * Purpose: 
    * <p> Clear the liveliness map
    *
    */ 
    void remove_livelinesses()
      throw (CORBA::SystemException);

    /**
    * Purpose: 
    * <p> Get the list of all the locations in the map (Even the one with
    * response=UNKNOWN because they have started and they are not yet declared FAILED
    * 
    *@return CdmwFT::MonitoringLibrary::Locations * the list of all the locations in the map
    */ 
    virtual CdmwFT::MonitoringLibrary::Locations * get_started_locations ()
      throw (CORBA::SystemException);

    // virtual methods defined in herited classes

    virtual void add_location(const CdmwFT::MonitoringLibrary::Location location)
      throw (CdmwFT::MonitoringLibrary::AlreadyExists, CORBA::SystemException)= 0;
    virtual void remove_location(const CdmwFT::MonitoringLibrary::Location location)
      throw (CdmwFT::MonitoringLibrary::NotFound, CORBA::SystemException)= 0;
    virtual void set_locations(const CdmwFT::MonitoringLibrary::Locations& locations)
      throw (CdmwFT::MonitoringLibrary::NotEmpty, CORBA::SystemException)= 0;
    virtual void reset_locations()
      throw (CORBA::SystemException)= 0;
    virtual const CdmwFT::MonitoringLibrary::Location get_masterCandidate()
      throw(Cdmw::InternalErrorException) = 0;

protected:

    //The type used to identify a response
    struct Response {
	ResponseType responseType;
	unsigned int counterVal;
    };

    /**
    *Purpose:
    *<p> The type used to return the internal list of liveliness
    *    which are pair of Location, Response
    */
    typedef std::map<std::string, Response> LivelinessMap;

    // The map with of <location, response> defining the system liveliness
    LivelinessMap m_livelinessMap;

    // The mutex used to maintain liveliness map coherency
    Cdmw::OsSupport::ReaderWriterLock m_livelinessMutex;

private:
    // default copy constructor
    LivelinessRepository(const LivelinessRepository&);
    // assignement operator
    LivelinessRepository& operator=(const LivelinessRepository&);


}; // end class definition

}; // end namespace Supervision
}  // End namespace FT
}  // End namespace Cdmw

#endif

