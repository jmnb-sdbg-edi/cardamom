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


#ifndef INCL_EVENT_EVENTCHANNELUTILS_HPP 
#define INCL_EVENT_EVENTCHANNELUTILS_HPP 


// Cdmw files
#include "Foundation/orbsupport/CORBA.hpp"
#include <Foundation/orbsupport/ObjectTraits.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include "Foundation/common/Locations.hpp"
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"

// IDL files
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "Event/idllib/CdmwEvent.skel.hpp"
#include "Event/idllib/CdmwEventAdmin.stub.hpp"

// Orbacus file
#include "Event/idllib/OBTypedEventChannelFactory.stub.hpp"
#include "Event/idllib/OBEventChannelFactory.stub.hpp"

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
        * <p> Find the ORBACUS factory able to create the event channel 
        * This will read event channel profile is any and choose the 
        * corresponding OB factory
        * This template will be instanciate with typed and untyped
        * OBEventChannelFactory.
        **/
        template <class T> static  typename Cdmw::OrbSupport::object_traits<T>::_ptr_type
        find_OBFactory(CORBA::ORB_ptr orb,
                       const char* channel_name, 
                       const char* default_factory_name)
            throw(CdmwEvent::CreationError, 
                  CORBA::SystemException);

        /**
        * Purpose:
        * <p> Find the supported interface defined for a given event channel. 
        * This will read event channel profile is any and extract the 
        * corresponding interface name.
        **/
        static std::string 
        find_interface(const char* channel_name)
            throw (CdmwEvent::CreationError, 
                   CORBA::SystemException);

        /**
        * Purpose:
        * <p> Register (name,object) within the repository.
        * In case of a TRANSIENT system exception There will be a retry max_retries times.
        * if max_retries == 0, then this will do retry forever.
        **/
        static void
        register_object
            (CdmwLifeCycle::ObjectRegistration_ptr                      domain,
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

/********************************************************************/
// Find the ORBACUS factory able to create the event channel 
// This will read event channel profile if any and choose the 
// corresponding OB factory.
template <class T> typename Cdmw::OrbSupport::object_traits<T>::_ptr_type
EventChannelUtils::find_OBFactory(CORBA::ORB_ptr orb,
                                  const char* channel_name, 
                                  const char* default_factory_name)
    throw(CdmwEvent::CreationError, 
          CORBA::SystemException)
{
    typedef Cdmw::OrbSupport::object_traits<T> OT;
    
    /**
    * An alias to a CORBA _var type of the object
    */
    typedef typename OT::_var_type _var_type;
    
    const char* URL_PROPERTY = "URL";
    CdmwEventAdmin::EventChannelProfileManager_var manager;
    CdmwEventAdmin::EventChannelProfile_var        channelProfile;
    _var_type  factory;
    CORBA::Object_var obj;
    
    bool is_urlDefined = false;
    std::string url;
    int i_property = 0;
    int nb_properties = 0;
    std::string propertyName;
    
    //
    // Get Event channel properties
    //
    try 
    {
        // i) Get NamingInterface object from RepositoryInterface
        Cdmw::CommonSvcs::Naming::NamingInterface ni = 
            Cdmw::NamingAndRepository::RepositoryInterface::get_domain_naming_interface(
            Cdmw::Common::Locations::CDMW_EVENT_SERVICE_NAME_DOMAIN);
        
        // ii) Get a reference on the EventChannelProfileManager
        obj = ni.resolve(CdmwEventAdmin::EventChannelProfileManagerName);
    }
    catch (CdmwNamingAndRepository::NoNameDomain&)
    {
        throw CdmwEvent::CreationError("Cdmw event service domain not specified.");
    }
    catch (CdmwNamingAndRepository::InvalidName&)
    {
        throw CdmwEvent::CreationError("Cdmw event service domain invalid.");
    }
    catch (CosNaming::NamingContext::NotFound&)
    {
        throw CdmwEvent::CreationError("EventChannelProfileManager not found.");
    }
    catch (CosNaming::NamingContext::CannotProceed&)
    {
        throw CdmwEvent::CreationError("Can not proceed resolve on EventChannelProfileManager.");
    }
    catch (CosNaming::NamingContext::InvalidName&)
    {       
        throw CdmwEvent::CreationError("Invalid name for EventChannelProfileManager.");
    }
    
    
    manager = CdmwEventAdmin::EventChannelProfileManager::_narrow(obj);
    if (CORBA::is_nil(manager.in())) 
    {
        throw CdmwEvent::CreationError("EventChannelProfileManager is a nil object reference.");
    }
    
    //
    // iii) Find channel properties
    //
    try 
    {
        channelProfile = manager->find_profile(channel_name);
    }
    catch (CdmwEventAdmin::NotExist&)
    {
        // No profile defined for this channel
        // iv) Find Factory
        // create it in the event server defined in the orb configuration.
        try
        {
            obj = orb -> resolve_initial_references(default_factory_name);
            if (CORBA::is_nil(obj.in()))
            {
                throw CdmwEvent::CreationError("No default event channel factory exists.");
            }
        }
        catch (CORBA::ORB::InvalidName&)
        {
            throw CdmwEvent::CreationError("No default event channel factory exists.");
        }
        
        factory = OT::_narrow(obj);
    }
    catch (...)
    {
        throw CdmwEvent::CreationError("Unexpected exception raised!");
    }
    
    // iv) Find Factory
    if (channelProfile)
    {
        // A profile is defined for this channel,
        // read it.
        nb_properties = channelProfile->length();
        
        for (i_property=0; i_property<nb_properties; i_property++)
        {
            propertyName = (*channelProfile)[i_property].name;
            if (propertyName == URL_PROPERTY)
            {
                is_urlDefined = true;
                url = (*channelProfile)[i_property].value;
                
                // get out 'for' loop
                break; 
            }
        }
        
        if (is_urlDefined)
        {
            try 
            {
                std::cout << "INFO: create channel named " << channel_name
                          << " using the factory URL= " << url.c_str() << std::endl;

                obj = orb->string_to_object(url.c_str());
                
                if (CORBA::is_nil(obj))
                {
                    CdmwEvent::CreationError ex;
                    std::string reason = "Invalid factory url for channel ";
                    reason += channel_name;
                    ex.reason = CORBA::string_dup(reason.c_str());
                    
                    throw ex;
                }
                
                factory = OT::_narrow(obj);
                
                if (CORBA::is_nil(factory))
                {
                    CdmwEvent::CreationError ex;
                    std::string reason = "Invalid factory url for channel ";
                    reason += channel_name;
                    ex.reason = CORBA::string_dup(reason.c_str());
                    
                    throw ex;
                }
            }
            catch (CORBA::TRANSIENT&)
            {
                throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachEventServer,
                                       CORBA::COMPLETED_NO);
            }
            catch (const CORBA::OBJECT_NOT_EXIST &) 
            {
                CdmwEvent::CreationError ex;
                std::string reason = "Invalid factory url for channel ";
                reason += channel_name;
                ex.reason = CORBA::string_dup(reason.c_str());
                
                throw ex;
            }

        }
        else
        {
            CdmwEvent::CreationError ex;
            std::string reason = "No factory URL defined for channel ";
            reason += channel_name;
            ex.reason = CORBA::string_dup(reason.c_str());
            
            throw ex;
        }
    }
    else
    {
        // No profile defined, check if a local factory has been found
        if (CORBA::is_nil(factory))
        {
            throw CdmwEvent::CreationError("Default event channel factory is nil.");
        }
    }
    
    return factory._retn();
} // End find_OBfactory() for generic channel factory

}; // End namespace Event
}; // End namespace Cdmw
#endif // INCL_EVENT_EVENTCHANNELUTILS_HPP


