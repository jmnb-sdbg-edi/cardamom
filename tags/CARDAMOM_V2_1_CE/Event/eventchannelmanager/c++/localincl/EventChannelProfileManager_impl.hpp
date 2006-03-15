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


#ifndef INCL_EVENTADMIN_EVENTCHANNELPROFILEMANAGER_IMPL_HPP 
#define INCL_EVENTADMIN_EVENTCHANNELPROFILEMANAGER_IMPL_HPP 

// Std include files
#include <map>
#include <string>

#include "Foundation/orbsupport/RefCountServantBase.hpp"

#include "Event/idllib/CdmwEventAdmin.skel.hpp"
#include "eventchannelmanager/EventChannelConfiguration.hpp"

namespace Cdmw
{
namespace EventAdmin
{
    // Type definition for the map of profiles stored by event channel name
    typedef std::map<std::string, CdmwEventAdmin::EventChannelProfile*, std::less<std::string> >    ChannelProfiles;

/**
* Purpose:
* AlreadyActiveException.
*/

class AlreadyActiveException  : public Exception 
{    
    public:
        AlreadyActiveException(const std::string& reason) 
            : Exception( Exception::SAFE, "Already active service :" + reason )
        {
        }

    protected:

    private:

};
/**
*Purpose:
*<p>    [class description]
*    This class implement the EventChannelProfileManager interface.
*    This is a singleton class.
*
*/
class EventChannelProfileManager_impl : virtual public POA_CdmwEventAdmin::EventChannelProfileManager,
                                        virtual public Cdmw::OrbSupport::RefCountServantBase 
{

    public:

         /**
        * Purpose:
        * <p>  
          *  Return the single instance of the EventchannelProfileManager 
          *
        *@param  none
        *@exception none
        */ 
        static 
        EventChannelProfileManager_impl* 
        init(CORBA::ORB_ptr           orb, 
             PortableServer::POA_ptr  parent,
             const std::string&       XMLConfigurationFile,
             bool                     do_validate=false)
            throw(ConfigurationException, 
                        XMLErrorException, 
                        OutOfMemoryException, 
                        AlreadyActiveException,
                        InternalErrorException);


        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~EventChannelProfileManager_impl()
            throw();

        /**
        * Purpose:
        * <p>
        *  override this function because its poa is different from
        * the rootPOA.
        *
        */
        virtual
        PortableServer::POA_ptr 
        _default_POA()
        {
            return PortableServer::POA::_duplicate(m_poa.in());
        }


        /**
        * Purpose:
        * <p>
        *   add an event channel profile that is a list of properties.
        * 
        *@param channel_name The event channel name as declared in the Cdmw Naming&Repository.
        *@param profile      A set of properties associated to this event channel.
        *
        *@return void
        *@exception CdmwEventAdmin::AlreadyExistant if a set of properties has already exist
        *                                            for this channel.
        *@exception CdmwEventAdmin::InvalidName if the event channel name has a bad format.
        */ 
        //
        // IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/create_profile:1.0
        //
        virtual void 
        create_profile(const char* channel_name,
                       const CdmwEventAdmin::EventChannelProfile& profile)
            throw(CdmwEventAdmin::AlreadyExistant,
                  CdmwEventAdmin::InvalidName,
                  CORBA::SystemException);
     
        /**
        * Purpose:
        * <p>
        *   remove an event channel profile.
        * 
        *@param channel_name The event channel name.
        *
        *@return void
        *@exception CdmwEventAdmin::InvalidName if the event channel name has a bad format.
        *@exception CdmwEventAdmin::NotExist if the event channel doesn't have any profile.
        */ 
        //
        // IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/remove_profile:1.0
        //
        virtual void 
        remove_profile(const char* channel_name)
            throw(CdmwEventAdmin::NotExist,
                  CORBA::SystemException);

        /**
        * Purpose:
        * <p>
        *   update an event channel profile with a new one.
        * 
        *@param channel_name The event channel name.
        *
        *@return void
        *@exception CdmwEventAdmin::InvalidName if the event channel name has a bad format.
        *@exception CdmwEventAdmin::NotExist if the event channel doesn't have any profile.
        */   
        //
        // IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/update_profile:1.0
        //
        virtual void 
        update_profile(const char* channel_name,
                       const CdmwEventAdmin::EventChannelProfile& profile)
            throw(CdmwEventAdmin::NotExist,
                  CORBA::SystemException);

        /**
        * Purpose:
        * <p>
        *   This operation return the profile associated to an event channel.
        * 
        *@param channel_name The event channel name.
        *
        *@return CdmwEventAdmin::EventChannelProfile*  A pointer on the profile of the
        *                                               specified event channel
        *@exception CdmwEventAdmin::InvalidName if the event channel name has a bad format.
        *@exception CdmwEventAdmin::NotExist if the event channel doesn't have any profile.
        */
        //
        // IDL:thalesgroup.com/CdmwEventAdmin/EventChannelProfileManager/find_profile:1.0
        //
        virtual CdmwEventAdmin::EventChannelProfile* 
        find_profile(const char* channel_name)
            throw(CdmwEventAdmin::NotExist,
                  CORBA::SystemException);
        
        /**
        * This operation return the list of managed event channel.
        *
        * @return A set of event channel name having a profile.
        * 
        */
        virtual CdmwEventAdmin::EventChannelNameList* 
        managed_event_channels()
             throw(CORBA::SystemException);
        
        /**
        * This operation save all the current managed profiles in an XML file.
        *
        * @param file_name Name of the XML file where event channel profile 
        * should be saved.
        * @param write_over Write over an existant file if true.
        *
        * @exception AlreadyExistant if the XML file already exists and the 
        * write_over flag is set to false.
        */
        virtual void 
        save_profiles() 
            throw(CdmwEventAdmin::AlreadyExistant,
                  CORBA::SystemException);
        
        /**
        * This operation deactivates the servant and permanently removes its 
        * persistent state.
        */
        virtual void
        destroy()
             throw(CORBA::SystemException);
            
        /**
        *[Attribute description]
        */ 
        static const std::string   m_name;     // name


    protected:
        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        * 
        *@param
        *@exception
        */ 
        EventChannelProfileManager_impl(CORBA::ORB_ptr           orb, 
                                        PortableServer::POA_ptr  poa,
                                        const std::string&       XMLConfigurationFile,
                                        bool                     do_validate)
              throw(ConfigurationException, OutOfMemoryException, InternalErrorException);

       /**
        * Purpose:
        * <p> 
        * check if the name is a valid name
        *
        *@param s The string to check
        */ 
        bool is_valid(const std::string& s) const throw();



    private:
        CORBA::ORB_var                          m_orb;             // A reference to the ORB pseudo-object
        PortableServer::POA_var                 m_poa;             // its POA
        ChannelProfiles                         m_channelProfiles; // map of channel profiles ordered 
                                                                   // by event channel name
        static EventChannelProfileManager_impl* mp_instance;       // singleton instance
        EventChannelConfiguration*              mp_eventChannelConf;

}; // End class EventChannelProfileManager_impl 

}; // End namespace EventAdmin
}; // End namespace Cdmw
#endif // INCL_EVENTADMIN_EVENTCHANNELPROFILEMANAGER_IMPL_HPP

