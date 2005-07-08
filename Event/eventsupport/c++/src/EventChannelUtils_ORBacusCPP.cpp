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


#include "eventsupport/EventChannelUtils_ORBacusCPP.hpp"

#include <string>



namespace
{
    // Name of the IDL property
    const char* IDL_PROPERTY = "IDL";
};

namespace Cdmw
{
namespace Event
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/

/********************************************************************/
// Purpose:
// <p> Find the supported interface defined for a given event channel. 
// This will read event channel profile is any and extract the 
// corresponding interface name.
std::string 
EventChannelUtils::find_interface(const char* channel_name)
throw (CdmwEvent::CreationError, CORBA::SystemException)
{
    // An alias to a CORBA _var type of the object    
    CdmwEventAdmin::EventChannelProfileManager_var manager;
    CdmwEventAdmin::EventChannelProfile_var        channelProfile;
    std::string interfaceKey = "";
    
    bool is_interfaceKeyDefined = false;
    int i_property = 0;
    int nb_properties = 0;
    std::string propertyName;
    CORBA::Object_var obj;
    
    //
    // Get Event channel properties
    //
    try 
    {
        // i) Get NamingInterface object from RepositoryInterface
        Cdmw::NamingAndRepository::NamingInterface ni = 
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
        // no profile defined for this channel nothing to do!      
    }
    catch (...)
    {
        // Unexpected exception       
    }
    
    // iv) Find interface key
    if (channelProfile)
    {
        // A profile is defined for this channel,
        // read it.
        nb_properties = channelProfile->length();
        
        for (i_property=0; i_property<nb_properties; i_property++)
        {
            propertyName = (*channelProfile)[i_property].name;
            if (propertyName == IDL_PROPERTY)
            {
                is_interfaceKeyDefined = true;
                interfaceKey = (*channelProfile)[i_property].value;
                
                // get out 'for' loop
                break; 
            }
        }
    }
    
    return interfaceKey;
}

// Register (name,object) within the repository.
// In case of a TRANSIENT system exception there will be a retry max_retries times.
// if max_retries == 0, then this will do retry forever.
void
EventChannelUtils::register_object
    (CdmwLifeCycle::ObjectRegistration_ptr                      domain,
     const std::string &                                         domain_name,
     CORBA::Object_ptr                                           the_object,
     const char*                                                 object_name,
     signed long                                                 max_retries)
    throw(CdmwLifeCycle::AlreadyExistant,
          CdmwLifeCycle::InvalidName,
          CORBA::SystemException)
{
    bool forever = (max_retries <= 0);

    signed long counter = max_retries;

    do {
        try {
            domain->register_object (the_object, object_name);
            break;
        } catch (const CosNaming::NamingContext::AlreadyBound &) {
            CdmwLifeCycle::AlreadyExistant ex;
            // Add more information to help the caller in debuging
            std::string reason = domain_name + "/" + object_name;
            ex.name = CORBA::string_dup(reason.c_str());
            throw ex;
        } catch (const CosNaming::NamingContext::InvalidName & e) {
        // no "reason" field available
        throw CdmwLifeCycle::InvalidName();
        } catch (const CORBA::TRANSIENT &) {
            // Retry
        } catch (const CORBA::SystemException &) {
            // Cancel registration ????
            throw;
        }
    } while (forever || (--counter > 0) );

    if (!forever && (counter <= 0))
        throw CORBA::TRANSIENT(Cdmw::OrbSupport::TRANSIENTCannotReachRepository,
                               CORBA::COMPLETED_YES);
}


}; // End namespace Event
}; // End namespace Cdmw


