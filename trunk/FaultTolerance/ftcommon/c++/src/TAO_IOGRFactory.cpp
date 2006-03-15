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

#include "FaultTolerance/ftcommon/TAO_IOGRFactory.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>
#include <sstream>
#include "tao/MProfile.h"
#include "tao/Profile.h"
#include "tao/Stub.h"
#include "tao/ORB_Core.h"

namespace Cdmw {
namespace FT {

TAO_IOGRFactory::TAO_IOGRFactory(CORBA::ORB_ptr          orb,
                                 std::ostream &          os)
    throw(CORBA::SystemException)
    : IOGRFactory(orb, os)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    // Initialize the IOR manipulation.
    CORBA::Object_var obj =
        m_orb->resolve_initial_references(TAO_OBJID_IORMANIPULATION);
    m_iogrm = ::TAO_IOP::TAO_IOR_Manipulation::_narrow(obj.in());

}
        
TAO_IOGRFactory::~TAO_IOGRFactory()
    throw()
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
}

/**
 * Build the iogr
 *
 * @param Members          sequence of members & locations.
 * @param primary_location location of the primary members.
 * @param object_group_id  the object group id to use for the search.
 * @param ft_domain_id     The fault tolerance domain id.
 * @param object_group_version the object group reference version.
 * @param replication_manager_ref the reference of the replication manager
 
 * @return the reference to the object created 
 
 * @throw InvalidProperty if one or more of the properties in the
 *        sequence is not valid.
 * @throw UnsupportedProperty if one or more of the properties in
 *        the sequence is not supported.
 */

CORBA::Object_ptr
TAO_IOGRFactory::build_iogr(const ::CdmwFT::MemberInfos & members,
                            const ::FT::Location &      primary_location,
                            ::FT::ObjectGroupId         object_group_id,
                            const char *                type_id,
                            const char *                ft_domain_id,
                            ::FT::ObjectGroupRefVersion & object_group_ref_version,
                            const CdmwFT::Objects & fall_backs)
    throw(::FT::PrimaryNotSet,
          ::FT::ObjectNotCreated,
          CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id<<",type_id="<<type_id
                      <<",ft_domain_id="<<ft_domain_id<<",object_group_ref_version="<<object_group_ref_version);

    // Create fallback profile (Replication Manager) IOR 
    // & increment current group reference version
    ::FT::ObjectGroupRefVersion group_version = object_group_ref_version + 1;

    CORBA::ULong members_length = members.length();
    CORBA::ULong fall_backs_length = fall_backs.length();
    
    ::FT::ObjectGroup_var object_group = ::FT::ObjectGroup::_nil();
    CORBA::Object_var primary_object   = CORBA::Object::_nil();
    
    // Create IOR list for use with merge_iors.
    ::TAO_IOP::TAO_IOR_Manipulation::IORList iors(members_length + fall_backs_length);
    iors.length(members_length + fall_backs_length);
     
    // TODO: Check when there's no primary_location specified
    CORBA::Boolean primary_found = false;

    std::string s_primary_location("");
    bool check_primary_location=false;

    using namespace Cdmw::CommonSvcs::Naming;
    if (primary_location.length() >= 1) {
        check_primary_location=true;
        s_primary_location = NamingInterface::to_string(primary_location);
    }

    CORBA::PolicyList no_policy;
    no_policy.length(0);
    CORBA::ULong primary_index = 0;
    for (CORBA::ULong iMembers = 0; iMembers < members_length; ++iMembers) {
        // FIXME: a workaround!
        CORBA::String_var s_obj_ = m_orb->object_to_string(members[iMembers].the_reference);
        CORBA::Object_var obj_ = m_orb->string_to_object(s_obj_.in());
        //                members[iMembers].the_reference->_set_policy_overrides(no_policy,CORBA::ADD_OVERRIDE);
        iors[iMembers] = obj_;
            
        if (check_primary_location) {
            std::string s_member_loc = 
            NamingInterface::to_string (members[iMembers].the_location);
                
            if (s_member_loc == s_primary_location) {
                primary_object = obj_; // CORBA::Object::_duplicate(members[iMembers].the_reference);
                primary_found = true;
                primary_index = iMembers;
            }
        }
    }

    // Ensure that the profile of the primary backup is always the first profile in the IOGR.
    if (primary_index > 0) {
        CORBA::Object_var obj_temp = CORBA::Object::_duplicate(iors[0]);
        iors[0] = CORBA::Object::_duplicate(iors[primary_index]);
        iors[primary_index] = obj_temp;
    }

    for (CORBA::ULong iMembers = 0; iMembers < fall_backs_length ; ++iMembers) {
        
        // FIXME: a workaround!
        CORBA::String_var s_obj_ = m_orb->object_to_string(fall_backs[iMembers]);
        CORBA::Object_var obj_ = m_orb->string_to_object(s_obj_.in());
        iors[members_length + iMembers] = obj_; 
    }
    


    // Create the object group.
    // merge_iors may throw the following exceptions:
    //        CORBA::SystemException,
    //        TAO_IOP::EmptyProfileList,
    //        TAO_IOP::Duplicate, and
    //        TAO_IOP::Invalid_IOR
    try {
        object_group = m_iogrm->merge_iors(iors);
    } catch (const CORBA::SystemException & ex) {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "build_iogr(" << type_id << ")': Cannot merge IORs.\n"
                   << "Exception : " << ex << "\n"
                   << "Forwarding initial exception.");

        if (FTLogger::GetLogger()->is_enabled_for(Cdmw::Logging::DEBUG_4)) {
            for (CORBA::ULong i = 0L; i < iors.length() ; ++i) {
                CDMW_DEBUG_4(FTLogger::GetLogger(), "Member [" << i << "] type_id = " << iors[i]->_stubobj()->type_id.in());
            }
        }
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    } catch (const CORBA::Exception & ex) {
        CDMW_WARN(FTLogger::GetLogger(),
                  "build_iogr(" << type_id << ")': Cannot merge IORs.\n"
                  << "Exception : " << ex << "\n"
                  << "Raising ObjectNotCreated() exception.");

        if (FTLogger::GetLogger()->is_enabled_for(Cdmw::Logging::DEBUG_4)) {
            for (CORBA::ULong i = 0L; i < iors.length() ; ++i) {
                CDMW_DEBUG_4(FTLogger::GetLogger(), "Member [" << i << "] type_id = " << iors[i]->_stubobj()->type_id.in());
            }
        }
        ::FT::ObjectNotCreated e;
        CDMW_LOG_FUNCTION_EXCEPTION(e);
        throw e;
    }

    // Create the TAG_FT_GROUP.
    ::FT::TagFTGroupTaggedComponent ft_tag_component;
    ::TAO_FT_IOGR_Property property(ft_tag_component);

    // IOGR properties.
        
    // TAG_GROUP_VERSION major and minor revision numbers (always 1.0).
    ft_tag_component.component_version.major = (CORBA::Octet) 1;
    ft_tag_component.component_version.minor = (CORBA::Octet) 0;
        
    // Fault tolerance domain id.
    ft_tag_component.group_domain_id = CORBA::string_dup(ft_domain_id);
        
    // Object group id.
    ft_tag_component.object_group_id = object_group_id;
        
    // Object group reference version.
    ft_tag_component.object_group_ref_version = group_version;
       
    // Set the primary.
    if ( primary_found== true )
    {
        // set_primary may raise the following exceptions:
        //      CORBA::SystemException,
        //      TAO_IOP::Invalid_IOR,
        //      TAO_IOP::Duplicate, and
        //      TAO_IOP::MultiProfileList
        CORBA::Boolean retval = 0;
        try {
            retval = 
            m_iogrm->set_primary(&property, primary_object.in(), object_group.in());
        } catch (const CORBA::SystemException & ex) {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "build_iogr(" << s_primary_location << "," << type_id << ")': "
                       << "Cannot set primary.\n"
                       << "Exception : " << ex << "\n"
                       << "Forwarding initial exception.");
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        } catch (const CORBA::Exception & ex) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "build_iogr(" << s_primary_location << "," << type_id << ")': "
                      << "Cannot set primary.\n"
                      << "Exception : " << ex << "\n"
                      << "Raising PrimaryNotSet() exception.");
            ::FT::PrimaryNotSet e;
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw e;
        }
        if (retval == 0) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "build_iogr(" << s_primary_location << "," << type_id << ")': "
                      << "Failed to set primary.\n"
                      << "Raising PrimaryNotSet() exception.");
            ::FT::PrimaryNotSet e;
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw e;
        }
    }

    // Set the property.
    // set_property may raise the following exceptions:
    //        CORBA::SystemException,
    //        TAO_IOP::Invalid_IOR, and
    //        TAO_IOP::Duplicate
    CORBA::Boolean retval = 0;
    try {
        retval = m_iogrm->set_property(&property, object_group.in());
    } catch (const CORBA::SystemException & ex) {
        CDMW_ERROR(FTLogger::GetLogger(),
                   "build_iogr(" << s_primary_location << "," << type_id << ")': "
                   << "Failed to set TAG_FT_GROUP Component.\n"
                   << "Exception : " << ex << "\n"
                   << "Forwarding initial exception.");
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw;
    } catch (const CORBA::Exception & ex) {
        CDMW_WARN(FTLogger::GetLogger(),
                  "build_iogr(" << s_primary_location << "," << type_id << ")': "
                  << "Failed to set TAG_FT_GROUP Component.\n"
                  << "Exception : " << ex << "\n"
                  << "Raising ObjectNotCreated() exception.");
        ::FT::ObjectNotCreated e;
        CDMW_LOG_FUNCTION_EXCEPTION(e);
        throw e;
    }
    if (retval == 0) {
        CDMW_WARN(FTLogger::GetLogger(),
                  "build_iogr(" << s_primary_location << "," << type_id << ")': "
                  << "Failed to set TAG_FT_GROUP Component.\n"
                  << "Raising ObjectNotCreated() exception.");
        ::FT::ObjectNotCreated e;
        CDMW_LOG_FUNCTION_EXCEPTION(e);
        throw e;
    }
    // Update version now.
    object_group_ref_version = group_version;
    return object_group._retn();
}



CORBA::Long 
TAO_IOGRFactory::profile_count (CORBA::Object_ptr obj)
    throw (CORBA::SystemException)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    // Get the MProfile
    TAO_MProfile &mprofile =
    obj->_stubobj ()->base_profiles ();
    CORBA::Long result = mprofile.profile_count ();
    CDMW_LOG_FUNCTION_RETURN(result);
    return result;
}    

::FT::TagFTGroupTaggedComponent * 
TAO_IOGRFactory::decode_profile_with_group_tag (CORBA::Object_ptr obj,
                                                CORBA::Long profile)
    throw(CORBA::SystemException,
          CdmwFT::ProfileNotFound,
          CdmwFT::TagNotFound)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"profile="<<profile);
    // Get the MProfile
    TAO_MProfile &mprofile =
    obj->_stubobj ()->base_profiles ();
    unsigned int nb_profile = profile;
    if ((mprofile.profile_count ()-1) < nb_profile)
        throw  CdmwFT::ProfileNotFound();

    ::FT::TagFTGroupTaggedComponent* result = new ::FT::TagFTGroupTaggedComponent();
    
    
    const TAO_Tagged_Components &tagged_components =
    mprofile.get_profile (profile)->tagged_components ();
    
    IOP::TaggedComponent tagged;
    tagged.tag = IOP::TAG_FT_GROUP;
    
    if (tagged_components.get_component (tagged) == 1)
    {
        TAO_InputCDR cdr (ACE_reinterpret_cast (const char*,
                                                tagged.component_data.get_buffer ()
                                                ),
                          tagged.component_data.length ());
        CORBA::Boolean byte_order;
        // decode the cdr
        if ((cdr >> ACE_InputCDR::to_boolean (byte_order)) == 0)
        {
            CdmwFT::TagNotFound e;
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw e;
        }
        
        cdr.reset_byte_order (ACE_static_cast(int,byte_order));
        
        // Extract the group component
        cdr >> *result;
        return result;
        
    } else {
        CdmwFT::TagNotFound e; 
        CDMW_LOG_FUNCTION_EXCEPTION(e);
        throw e;
    }
}


::FT::TagFTPrimaryTaggedComponent
TAO_IOGRFactory::decode_profile_with_primary_tag (CORBA::Object_ptr obj,
                                                  CORBA::Long profile)
    throw(CORBA::SystemException,
          CdmwFT::ProfileNotFound,
          CdmwFT::TagNotFound)
{
    CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"profile="<<profile);
    // Get the MProfile
    TAO_MProfile &mprofile =
    obj->_stubobj ()->base_profiles ();
    unsigned int nb_profile = profile;
    if ((mprofile.profile_count ()-1) < nb_profile)
        throw  CdmwFT::ProfileNotFound();

    ::FT::TagFTPrimaryTaggedComponent result;
    
    const TAO_Tagged_Components &tagged_components =
    mprofile.get_profile (profile)->tagged_components ();
    
    IOP::TaggedComponent tagged;
    tagged.tag = IOP::TAG_FT_PRIMARY;
    
    if (tagged_components.get_component (tagged) == 1)
    {
        TAO_InputCDR cdr (ACE_reinterpret_cast (const char*,
                                                tagged.component_data.get_buffer ()
                                                ),
                          tagged.component_data.length ());
        CORBA::Boolean byte_order;
        // decode the cdr
        if ((cdr >> ACE_InputCDR::to_boolean (byte_order)) == 0)
        {
            CdmwFT::TagNotFound e; 
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw e;
        }
        
        cdr.reset_byte_order (ACE_static_cast(int,byte_order));
        
        // Extract the group component
        cdr >> result;
        return result;
        
    } else {
        CdmwFT::TagNotFound e; 
        CDMW_LOG_FUNCTION_EXCEPTION(e);
        throw e;
    }
}

}; // namespace FT
}; // namespace Cdmw

