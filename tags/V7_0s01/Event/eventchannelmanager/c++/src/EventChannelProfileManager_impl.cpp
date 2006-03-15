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


#include <Foundation/orbsupport/CORBA.hpp>
#include "Foundation/commonsvcs/naming/NamingInterface.hpp"
#include "Foundation/orbsupport/ExceptionMinorCodes.hpp"
#include <Foundation/orbsupport/OrbSupport.hpp>


#include "eventchannelmanager/EventChannelProfileManager_impl.hpp"
#include "eventchannelmanager/EventChannelConfiguration.hpp"



namespace Cdmw
{
namespace EventAdmin
{

/**
*Implementation notes: [if no pertinent write none ]
*<p>
*Portability issues: [if no pertinent write none ]
*<p>
*/

EventChannelProfileManager_impl* EventChannelProfileManager_impl::mp_instance = 0;
const std::string EventChannelProfileManager_impl::m_name = "EventChannelProfileManager";

const char *EVENT_CHANNEL_PROFILE_MANAGER_POA = "EventChannelProfileManagerPOA";
const char *EVENT_CHANNEL_PROFILE_MANAGER = "EventChannelProfileManager";


EventChannelProfileManager_impl::EventChannelProfileManager_impl(
      CORBA::ORB_ptr           orb, 
      PortableServer::POA_ptr  poa,
      const std::string&       XMLConfigurationFile,
      bool                     do_validate)
    throw(ConfigurationException, OutOfMemoryException, InternalErrorException)
    : m_orb(CORBA::ORB::_duplicate(orb)),
    m_poa(PortableServer::POA::_duplicate(poa))
{
    int channel_i = 0; // event channel index
        
    try
    {
    // get a reference on the entity responsible to read the XML configuration file.
       mp_eventChannelConf = 
        EventChannelConfiguration::readConfiguration(XMLConfigurationFile, do_validate);

        std::string channelName;
        CdmwEventAdmin::EventChannelProfile* channelProfile;
        ChannelProperties* channelProperties;
        int nbProfile = mp_eventChannelConf->getNbEventChannelProfile();

        for (channel_i=0; channel_i < nbProfile; channel_i++)
        {
            int property_i = 0;
            int nbProperty = 0;

            // get one by one event channel profile
            channelName = mp_eventChannelConf->getNextEventChannelName(channel_i);
            channelProperties = mp_eventChannelConf->getEventChannelProfileValue(channelName);
                
            // store data
            nbProperty = channelProperties->size();
            channelProfile = new CdmwEventAdmin::EventChannelProfile(nbProperty+1);
            channelProfile->length(nbProperty);

            for (property_i=0; property_i<nbProperty; property_i++)
            {
                (*channelProfile)[property_i].name = 
                    CORBA::string_dup((*channelProperties)[property_i].name.c_str());
                (*channelProfile)[property_i].value = 
                    CORBA::string_dup((*channelProperties)[property_i].value.c_str());
            }

            m_channelProfiles.insert(ChannelProfiles::value_type(channelName, channelProfile));
        }
    } 
    catch (const std::bad_alloc & ) 
    {
        _set_ref_count(0);
        CDMW_THROW(OutOfMemoryException);
    } 
    catch (const OutOfMemoryException & ) 
    {
        _set_ref_count(0);
        throw;
    } 
    catch (const XMLErrorException & ) 
    {
        _set_ref_count(0);
        throw;
    } 
    catch (const BadParameterException& e) 
    {
        _set_ref_count(0);
        CDMW_THROW(InternalErrorException);
    }
    catch (...)
    {
        _set_ref_count(0);
        CDMW_THROW(InternalErrorException);
    }
}


EventChannelProfileManager_impl::~EventChannelProfileManager_impl()
    throw()
{
   delete mp_eventChannelConf;
}

EventChannelProfileManager_impl* 
EventChannelProfileManager_impl::init(CORBA::ORB_ptr           orb, 
                                      PortableServer::POA_ptr  parent,
                                      const std::string&       XMLConfigurationFile,
                                      bool                     do_validate)
     throw(ConfigurationException, 
           XMLErrorException,
           OutOfMemoryException, 
           AlreadyActiveException,
           InternalErrorException)
{
    if (mp_instance)
        return mp_instance;
    else
    {
        try
        {
            // Create policy list for simple persistence
            CORBA::PolicyList pl;
            const CORBA::ULong len = 3;
            pl.length(len);
            
            pl[0] = parent->create_id_assignment_policy(PortableServer::USER_ID);
            pl[1] = parent->create_lifespan_policy(PortableServer::PERSISTENT);
            pl[2] = parent->create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);
            
            // Get a reference to the POA manager.
            PortableServer::POAManager_var manager = parent -> the_POAManager();
            
            if (CORBA::is_nil(manager.in()))
            {
                // No POAmanager
                CDMW_THROW(InternalErrorException);
            }
            
            // Create new POA
            Cdmw::OrbSupport::StrategyList poaStrategies;
            // Strategies with MULTITHREAD policy will accept Single Thread and
            // Multi Thread strategies. However, SERIALIZE policy will only allow
            // Single Thread policy.
            
            PortableServer::POA_var poa;
            
            try
            {
                poa = Cdmw::OrbSupport::OrbSupport::create_POA (parent,
                    EVENT_CHANNEL_PROFILE_MANAGER_POA,
                    manager.in(), 
                    pl,
                    poaStrategies); 
            }
            catch (const PortableServer::POA::AdapterAlreadyExists& )
            {
                CDMW_THROW1(ConfigurationException, "POA Already Exists!");
            }
            catch (const PortableServer::POA::InvalidPolicy& )
            {
                CDMW_THROW1(ConfigurationException, "Invalid Policy for the new POA");
            }
            catch (const CORBA::SystemException& e)
            {
                CDMW_THROW1(ConfigurationException, e._name());
            }
            
            // Clean up
            for (CORBA::ULong i = 0; i < len; ++i)
                pl[i]->destroy();
            
            // create instance
            mp_instance = new EventChannelProfileManager_impl(orb,
                poa.in(),
                XMLConfigurationFile, 
                do_validate);
            
            // Creation OK, activate object
            PortableServer::ObjectId_var oid
                //                = PortableServer::string_to_ObjectId(m_name.c_str());
                = PortableServer::string_to_ObjectId(EVENT_CHANNEL_PROFILE_MANAGER);
            mp_instance->m_poa->activate_object_with_id(oid.in(), mp_instance);
            
            // Drop ref count
            mp_instance->_remove_ref();
        } 
        catch (const PortableServer::POA::ObjectAlreadyActive& )
        {
            CDMW_THROW1(AlreadyActiveException, m_name.c_str());
        }
        catch (const PortableServer::POA::ServantAlreadyActive& )
        {
            CDMW_THROW1(AlreadyActiveException, m_name.c_str());
        }
        catch (const PortableServer::POA::WrongPolicy& )
        {
            CDMW_THROW(InternalErrorException);
        }
        catch (const std::bad_alloc & ) 
        {
            CDMW_THROW(OutOfMemoryException);
        } 
        catch (const OutOfMemoryException & ) 
        {
            throw;
        } 
        catch (const XMLErrorException & ) 
        {
            throw;
        } 
        catch (const BadParameterException& e) 
        {
            CDMW_THROW(InternalErrorException);
        }
        catch (const ConfigurationException& e)
        {
            throw;
        }
        
    }
    
    return mp_instance;
}

//
// IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/create_profile:1.0
//
void
EventChannelProfileManager_impl::create_profile(const char* channel_name,
                                                const CdmwEventAdmin::EventChannelProfile& profile)
                                                throw(CdmwEventAdmin::AlreadyExistant,
                                                CdmwEventAdmin::InvalidName,
                                                CORBA::SystemException)
{
    // check if a profile doesn't already exist for the specified channel
    ChannelProfiles::iterator profileIter = m_channelProfiles.find(channel_name);

    if (profileIter != m_channelProfiles.end())
    {
        // a profile already exist for this channel
        throw CdmwEventAdmin::AlreadyExistant("This channel already has a profile.");
    }
    else
    {
        // check the validity of the name
        if (!is_valid(channel_name))
        {
           throw CdmwEventAdmin::InvalidName("The name of the event channel has a bad format.");
        }
        else
        {
            // add profile in the map
            CdmwEventAdmin::EventChannelProfile* storedProfile = 
                new CdmwEventAdmin::EventChannelProfile(profile);

            m_channelProfiles.insert(ChannelProfiles::value_type(channel_name, storedProfile));
        }
    }
}

//
// IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/remove_profile:1.0
//
void
EventChannelProfileManager_impl::remove_profile(const char* channel_name)
    throw(CdmwEventAdmin::NotExist,
          CORBA::SystemException)
{
    // try to erase data in the map
    if (!m_channelProfiles.erase(channel_name))
    {
        // the key doesn't exist in the map
        throw CdmwEventAdmin::NotExist("No profile for this event channel.");
    }
}

//
// IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/update_profile:1.0
//
void
EventChannelProfileManager_impl::update_profile(const char* channel_name,
                                                const CdmwEventAdmin::EventChannelProfile& profile)
    throw(CdmwEventAdmin::NotExist,
          CORBA::SystemException)
{
    // try to erase data in the map
    if (!m_channelProfiles.erase(channel_name))
    {
        // the key doesn't exist in the map
        throw CdmwEventAdmin::NotExist("No profile for this event channel.");
    }
    else
    {
        // profile has been deleted, insert the new one!
        CdmwEventAdmin::EventChannelProfile* storedProfile = 
            new CdmwEventAdmin::EventChannelProfile(profile);

        m_channelProfiles.insert(ChannelProfiles::value_type(channel_name, storedProfile));
    }
}

//
// IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/find_profile:1.0
//
CdmwEventAdmin::EventChannelProfile*
EventChannelProfileManager_impl::find_profile(const char* channel_name)
    throw(CdmwEventAdmin::NotExist,
          CORBA::SystemException)
{
    CdmwEventAdmin::EventChannelProfile_var pProfile;
    
    // check if a profile exists for the specified channel
    ChannelProfiles::iterator profileIter = m_channelProfiles.find(channel_name);

    if (profileIter != m_channelProfiles.end())
    {
        // profile exists, create it 
        pProfile = new CdmwEventAdmin::EventChannelProfile(*(m_channelProfiles[channel_name]));
    }
    else
    {
        throw CdmwEventAdmin::NotExist("No profile found for this event channel.");
    }

    return pProfile._retn();
}


//
// IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/list_event_channel:1.0
//
CdmwEventAdmin::EventChannelNameList* 
EventChannelProfileManager_impl::managed_event_channels()
    throw(CORBA::SystemException)
{
    //return new CdmwEventAdmin::EventChannelNameList(m_managed_event_channels);
    CdmwEventAdmin::EventChannelNameList_var managedEventChannels;
    ChannelProfiles::iterator channelIterator = m_channelProfiles.begin();
    unsigned int i = 0;   // channel index

    managedEventChannels = new CdmwEventAdmin::EventChannelNameList();
    managedEventChannels->length(m_channelProfiles.size());

    while (channelIterator != m_channelProfiles.end())
    {
        // iterator points on a pair<key=channelname, profile> 
        ChannelProfiles::value_type value = *channelIterator;
        std::string name = value.first;
        managedEventChannels[i] = CORBA::string_dup(name.c_str());

        i++;
        channelIterator++;
    }

    return managedEventChannels._retn();
}
      
//
// IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/save_profiles:1.0
//  
void 
EventChannelProfileManager_impl::save_profiles() 
    throw(CdmwEventAdmin::AlreadyExistant,
          CORBA::SystemException)
{
    throw CORBA::NO_IMPLEMENT(Cdmw::OrbSupport::NO_IMPLEMENTNotYetImplemented, 
                              CORBA::COMPLETED_NO);
}

//
// IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/destroy:1.0
//  
void
EventChannelProfileManager_impl::destroy()
    throw(CORBA::SystemException)
{
    PortableServer::ObjectId_var oid
//        = PortableServer::string_to_ObjectId(m_name.c_str());
        = PortableServer::string_to_ObjectId(EVENT_CHANNEL_PROFILE_MANAGER);
    m_poa->deactivate_object(oid.in());

    // Terminate event loop
    m_orb->shutdown(false);
}

// Checks that the name contains a single name component
bool 
EventChannelProfileManager_impl::is_valid(const std::string& s) const throw()
{
     bool valid = false;

    try
    {
        CosNaming::Name_var name = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(s);

        if (name->length() == 1)
            valid = true;
    } 
    catch (const Cdmw::CommonSvcs::Naming::InvalidNameException & ) 
    {
    }
    return valid;
}

}; // End namespace EventAdmin
}; // End namespace Cdmw

