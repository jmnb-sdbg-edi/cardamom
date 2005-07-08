/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#ifndef INCL_EVENT_EVENTCHANNELUTILS_HPP 
#define INCL_EVENT_EVENTCHANNELUTILS_HPP 


// Cdmw files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ObjectTraits.hpp>
#include <Repository/repositoryinterface/RepositoryInterface.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Locations.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>

// IDL files
#include <Repository/idllib/CdmwNamingAndRepository.stub.hpp>
#include <Event/idllib/CdmwEvent.skel.hpp>
#include <Event/idllib/CdmwEventAdmin.stub.hpp>

// TAO file

/**
* Root namespace for CDMW runtime.
*/

namespace Cdmw
{

/**
* Namespace for CDMW Event Support types and data.
*/

namespace Event
{

/**
*Purpose:
*<p>    This class provides utils used by event channel factory.
*
*
*Features:
*<p> 
*    None
*
*/
class EventChannelUtils 
{

    public:
        /**
        * Purpose:
        * <p> Find the supported interface defined for a given event channel. 
        * This will read event channel profile is any and extract the 
        * corresponding interface name.
        **/
        static std::string 
        get_factory_properties(const char* channel_name)
            throw (CdmwEvent::CreationError, 
                   CORBA::SystemException);


        /**
        * Purpose:
        * <p> Register (name,object) within the repository.
        * In case of a TRANSIENT system exception there will be a retry max_retries times.
        * If max_retries == 0, then this will do retry forever.
        **/
        static void
        register_object
           (CdmwLifeCycle::ObjectRegistration_ptr                       domain,
             const std::string &                                         domain_name,
             CORBA::Object_ptr                                           the_object,
             const char*                                                 object_name,
             signed long                                                 max_retries)
            throw(CdmwLifeCycle::AlreadyExistant,
                  CdmwLifeCycle::InvalidName,
                  CORBA::SystemException);
          
    protected:


    private:
        // hide  constructor and destructor 
        EventChannelUtils()
            throw();

        virtual 
        ~EventChannelUtils()
                throw();

        // hide copy constructor and assigment operation 
        EventChannelUtils(const EventChannelUtils& rhs)
                throw();

        EventChannelUtils&
        operator=(const EventChannelUtils& rhs)
                throw();

        /**
        *[Attribute description]
        */ 

}; // End class EventChannelUtils 

}; // End namespace Event
}; // End namespace Cdmw
#endif // INCL_EVENT_EVENTCHANNELUTILS_HPP


