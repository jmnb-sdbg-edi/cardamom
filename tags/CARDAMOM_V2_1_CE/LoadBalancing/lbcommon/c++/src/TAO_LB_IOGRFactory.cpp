/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
 * 
 * Copyright (C) SELEX-SI 2004-2005. All rights reserved
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

/**
 * @file
 * @brief A TAO specific class implementing IOGRFactory interface
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "lbcommon/TAO_LB_IOGRFactory.hpp"
#include "lbcommon/TAO_LB_IOGRProperty.hpp"
#include <sstream>


#define ECHO_HEADER() \
   "[IOGR Factory] (file: " << __FILE__ << ", line: " << __LINE__ << ")\n -->"


#ifndef ENABLE_LB_DEBUG_DUMP

#   define DEBUG_DUMP(comment)
#   define DEBUG_ECHO(comment)

#else

#   define DEBUG_DUMP(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)

#   define DEBUG_ECHO(comment) \
do {\
    std::cerr << comment ;\
} while(0)

#endif


namespace Cdmw
{

namespace LB
{
    /**
     * Constructor
     */
    
    TAO_LB_IOGRFactory::TAO_LB_IOGRFactory(std::ostream & os,
                                           CORBA::ORB_ptr  orb)
        throw(CORBA::SystemException)
        : IOGRFactory(os, orb)
    {
        // Initialize the IOR manipulation.
        CORBA::Object_var obj =
            m_orb->resolve_initial_references(TAO_OBJID_IORMANIPULATION);
        m_iogrm = ::TAO_IOP::TAO_IOR_Manipulation::_narrow(obj.in());
        
    }

    /**
     * Destructor
     */
    
    TAO_LB_IOGRFactory::~TAO_LB_IOGRFactory()
        throw()
    {}
    

    /**
     * Build the iogr
     *
     * @param Members          sequence of members & locations.
     * @param object_group_id  the object group id to use for the search.
     * @param lb_domain_id     The fault tolerance domain id.
     * @param object_group_version the object group reference version.
     * @param group_manager_ref the reference of the LBGroupManager
     
     * @return the reference to the object created 
     
     * @throw InvalidProperty if one or more of the properties in the
     *        sequence is not valid.
     * @throw UnsupportedProperty if one or more of the properties in
     *        the sequence is not supported.
     */
    
    CORBA::Object_ptr
    TAO_LB_IOGRFactory::build_iogr(const ::CdmwLB::MemberInfos & members,
                                   PortableGroup::ObjectGroupId         object_group_id,
                                   const char *                type_id,
                                   const char *                lb_domain_id,
                                   const char *                lb_policy,
                                   PortableGroup::ObjectGroupRefVersion & object_group_ref_version,
                                   const CdmwLB::Objects & fall_backs)
        throw(PortableGroup::ObjectNotCreated,
              CORBA::SystemException)
    {
        
        // Create fallback profile (LBGroupManager) IOR 
        // & increment current group reference version
        PortableGroup::ObjectGroupRefVersion group_version = object_group_ref_version + 1;
        
        CORBA::ULong members_length = members.length();
        CORBA::ULong fall_backs_length = fall_backs.length();
        
        PortableGroup::ObjectGroup_var object_group = PortableGroup::ObjectGroup::_nil();
        
        // Create IOR list for use with merge_iors.
        ::TAO_IOP::TAO_IOR_Manipulation::IORList iors(members_length + fall_backs_length);
        iors.length(members_length + fall_backs_length);
        
        CORBA::PolicyList no_policy;
        no_policy.length(0);
        
        for (CORBA::ULong iMembers = 0; iMembers < members_length; ++iMembers)
        {
            // FIXME: a workaround!
            CORBA::String_var s_obj_ = m_orb->object_to_string(members[iMembers].the_reference);
            CORBA::Object_var obj_ = m_orb->string_to_object(s_obj_.in());
            iors[iMembers] = obj_;
        }
        
        for (CORBA::ULong iMembers = 0; iMembers < fall_backs_length ; ++iMembers)
        {
            set_fallback(fall_backs[iMembers]);
            
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
        try
        {
            object_group = m_iogrm->merge_iors(iors);
        } catch (const CORBA::SystemException & ex)
        {
            DEBUG_DUMP("");
            DEBUG_ECHO("build_iogr(" << type_id << ")': Cannot merge IORs.\n");
            DEBUG_ECHO("Exception : " << ex << "\n");
            DEBUG_ECHO("Forwarding initial exception.\n");
            throw;
        } catch (const CORBA::Exception & ex)
        {
            // TODO: Add Trace information
            DEBUG_ECHO(ECHO_HEADER());
            DEBUG_ECHO("build_iogr(" << type_id << ")': Cannot merge IORs.\n");
            DEBUG_ECHO("Exception : " << ex << "\n");
            DEBUG_ECHO("Raising ObjectNotCreated() exception.\n" );
            throw PortableGroup::ObjectNotCreated();
        }
        
        // Create the TAG_LB_GROUP.
        CdmwLB::TagLBGroupTaggedComponent lb_tag_component;
        TAO_LB_IOGRProperty property(lb_tag_component);  
        
        // IOGR properties.
        
        // TAG_GROUP_VERSION major and minor revision numbers (always 1.0).
        lb_tag_component.version.major = (CORBA::Octet) 1;
        lb_tag_component.version.minor = (CORBA::Octet) 0;
        
        // Load Balancing domain id.
        lb_tag_component.lb_domain_id = CORBA::string_dup(lb_domain_id);
        
        // Object group id.
        lb_tag_component.object_group_id = object_group_id;
        
        // Object group reference version.
        lb_tag_component.object_group_ref_version = group_version;
        
        lb_tag_component.lb_policy = CORBA::string_dup(lb_policy);
        CORBA::Boolean retval = 0;
        try {
            retval = property.set_property(object_group.in());
        } catch (const CORBA::SystemException & ex)
        {
            DEBUG_ECHO(ECHO_HEADER());
            DEBUG_ECHO("build_iogr(" << type_id << ")': ");
            DEBUG_ECHO("Failed to set TAG_FT_GROUP Component.\n");
            DEBUG_ECHO("Exception : " << ex << "\n");
            DEBUG_ECHO("Forwarding initial exception.\n");
            throw;
        } catch (const CORBA::Exception & ex)
        {
            // TODO: Add Trace information
            DEBUG_ECHO(ECHO_HEADER());
            DEBUG_ECHO("build_iogr(" << type_id << ")': ");
            DEBUG_ECHO("Failed to set TAG_FT_GROUP Component.\n");
            DEBUG_ECHO("Exception : " << ex << "\n");
            DEBUG_ECHO("Raising ObjectNotCreated() exception.\n");
            throw PortableGroup::ObjectNotCreated();
        }
        if (retval == 0)
        {
            DEBUG_ECHO(ECHO_HEADER());
            DEBUG_ECHO("build_iogr(" << type_id << ")': ");
            DEBUG_ECHO("Failed to set TAG_FT_GROUP Component.\n");
            DEBUG_ECHO("Raising ObjectNotCreated() exception.\n");
            throw PortableGroup::ObjectNotCreated();
        }
        
        
        
        // Update version now.
        object_group_ref_version = group_version;
        return object_group._retn();
    }
    
    /**
     * count the number of profile found into the IOGR
     *
     * @param obj The IOGR.
     *
     * @return the  number of profile found
     */
    
    CORBA::Long 
    TAO_LB_IOGRFactory::profile_count (CORBA::Object_ptr obj)
        throw (CORBA::SystemException)
    {
        // Get the MProfile
        TAO_MProfile &mprofile =
            obj->_stubobj ()->base_profiles ();
        
        return (mprofile.profile_count ());
    }    

    /**
     * create the LB_GROUP_VERSION service context for the specified object group
     *
     * @param group_version The group version used to create the Group Version Service Context.
     *
     * @return the IOP::ServiceContext structure
     */
        
    IOP::ServiceContext *
    TAO_LB_IOGRFactory::create_group_version_service_context(CORBA::ULong group_version, PortableGroup::ObjectGroupId id)
        throw (CORBA::SystemException)
    {
        IOP::ServiceContext_var sc = new IOP::ServiceContext();
        sc->context_id = CdmwLB::LB_GROUP_VERSION;
        
        TAO_OutputCDR ocdr;
        if (!(ocdr << ACE_OutputCDR::from_boolean (TAO_ENCAP_BYTE_ORDER)))
            throw CORBA::MARSHAL ();
        
        if (!(ocdr << group_version))
            throw CORBA::MARSHAL ();
        
        if (!(ocdr << id))
            throw CORBA::MARSHAL ();
        CORBA::ULong length =
            ACE_static_cast (CORBA::ULong, ocdr.total_length ());
        sc->context_data.length (length);
        CORBA::Octet *buf = sc->context_data.get_buffer ();
        
        for (const ACE_Message_Block *i = ocdr.begin ();
             i != 0;
             i = i->cont ())
        {
            ACE_OS::memcpy (buf, i->rd_ptr (), i->length ());
            buf += i->length ();
        }
        return sc._retn();
    }

    /**
     * Get the object reference of specified replica in the object group
     *
     *
     * @param object_group The object group
     * @param profile The profile number of the replica
     *
     * @return the object reference
     *
     * @exception PortableGroup::MemberNotFound if the profile does'n exist
     */
        
    CORBA::Object_ptr
    TAO_LB_IOGRFactory::get_member_ref(PortableGroup::ObjectGroup_ptr object_group, CORBA::Long profile)
        throw (CORBA::SystemException,
               PortableGroup::MemberNotFound)
    {
        CORBA::Object_var obj = CORBA::Object::_nil ();
      
        TAO_MProfile &tmp_pfiles = object_group->_stubobj ()->base_profiles ();
        unsigned int nb_profile = profile;
        if ((tmp_pfiles.profile_count ()-1) < nb_profile)
        {
           DEBUG_ECHO(ECHO_HEADER());
           DEBUG_ECHO("get_member_ref (profile n. " << profile << ")': ");
           DEBUG_ECHO("Failed to find specified member.\n");
           DEBUG_ECHO("Raising MemberNotFound() exception.\n");
            throw  PortableGroup::MemberNotFound();
        }

        TAO_ORB_Core *orb_core = TAO_ORB_Core_instance ();
        CORBA::String_var id =
            CORBA::string_dup (object_group->_stubobj ()->type_id.in ());
        // Make a stub Object
        TAO_Stub *stub = 0;
        TAO_Profile *pfile = tmp_pfiles.get_profile(profile);
        if(pfile->tag() == IOP::TAG_INTERNET_IOP)
        {
            CORBA::ULong sz = 1;
            TAO_MProfile mpfile (sz);
            if (mpfile.add_profile (pfile) != -1)
            {
                stub = new TAO_Stub (id._retn (),
                                     mpfile,
                                     orb_core);
            }
            // Make the stub memory allocation exception safe for
            // the duration of this method.
            TAO_Stub_Auto_Ptr safe_stub (stub);
            
            // Create the CORBA level proxy
            CORBA::Object_ptr temp_obj = CORBA::Object::_nil ();
            temp_obj = new CORBA::Object (safe_stub.get ());
            obj = temp_obj;
            
            // Release ownership of the pointers protected by the auto_ptrs since they
            // no longer need to be protected by this point.
            stub = safe_stub.release ();
        }
        else throw PortableGroup::MemberNotFound(); 
        
        return obj._retn();
    }

    /**
     * decode the specified profile to extract the CdmwLB::TagLBGroupTaggedComponent
     * structure
     *
     * @param obj The IOGR.
     * @param the profile to decode
     *
     * @return the FT::TagFTGroupTaggedComponent structure
     *
     * @exception ProfileNotFound if the profile number is over the limit
     * @exception TagNotFound if the TAG_LB_GROUP is not found into the profile
     */
        
    CdmwLB::TagLBGroupTaggedComponent * 
    TAO_LB_IOGRFactory::decode_profile_with_group_tag (CORBA::Object_ptr obj,
                                                       CORBA::Long profile)
    throw(CORBA::SystemException,
          CdmwLB::ProfileNotFound,
          CdmwLB::TagNotFound)
    {
        // Get the MProfile
        TAO_MProfile &mprofile =
            obj->_stubobj ()->base_profiles ();
        unsigned int nb_profile = profile;
        if ((mprofile.profile_count ()-1) < nb_profile)
        {
            DEBUG_ECHO(ECHO_HEADER());
            DEBUG_ECHO("decode_profile_with_group_tag (profile n. " << profile << ")': ");
            DEBUG_ECHO("Failed to decode TAG_LB_GROUP Component.\n");
            DEBUG_ECHO("Raising ProfileNotFound() exception.\n");
            throw  CdmwLB::ProfileNotFound();
        }
             
        
        const TAO_Tagged_Components &tagged_components =
            mprofile.get_profile (profile)->tagged_components ();
        
        IOP::TaggedComponent tagged;
        tagged.tag = CdmwLB::TAG_LB_GROUP;
        
        if (tagged_components.get_component (tagged) == 1)
        {
            TAO_InputCDR cdr (ACE_reinterpret_cast (const char*,
                                                    tagged.component_data.get_buffer ()),
                              tagged.component_data.length ());
            CORBA::Boolean byte_order;
            // decode the cdr
            if ((cdr >> ACE_InputCDR::to_boolean (byte_order)) == 0)
            {
		DEBUG_ECHO(ECHO_HEADER());
                DEBUG_ECHO("decode_profile_with_group_tag (profile n. " << profile << ")': ");
                DEBUG_ECHO("Failed to decode TAG_LB_GROUP Component.\n");
                DEBUG_ECHO("Raising TagNotFound() exception.\n");
                throw CdmwLB::TagNotFound();
            }
            
            cdr.reset_byte_order (ACE_static_cast(int,byte_order));
            CdmwLB::TagLBGroupTaggedComponent* result = new CdmwLB::TagLBGroupTaggedComponent();
            // Extract the group component
            cdr >> *result;
            return result;
            
        }else
         {
	     DEBUG_ECHO(ECHO_HEADER());
             DEBUG_ECHO("decode_profile_with_group_tag (profile n. " << profile << ")': ");
             DEBUG_ECHO("Failed to decode TAG_LB_GROUP Component.\n");
             DEBUG_ECHO("Raising TagNotFound() exception.\n");
             throw CdmwLB::TagNotFound(); 
         }
    }

    /**
     * Extract the LBGroupTaggedComponent structure from the TaggedComponent
     *
     * @param tag_cmp The TaggedComponent to decode
     *
     * @return the CdmwLB::TagLBGroupTaggedComponent structure
     *
     * @exception TagNotFound if the TAG_LB_GROUP is not found 
     * 
     */

    
  
CdmwLB::TagLBGroupTaggedComponent*  TAO_LB_IOGRFactory::get_lb_tagged_components(const IOP::TaggedComponent & tag_cmp)
    throw (CORBA::SystemException, CdmwLB::TagNotFound)
{
    TAO_Tagged_Components tagged_components;
    tagged_components.set_component (tag_cmp);
    IOP::TaggedComponent tagged;
    tagged.tag = CdmwLB::TAG_LB_GROUP;
    if (tagged_components.get_component (tagged) == 1)
    {
	TAO_InputCDR cdr ( reinterpret_cast <const char*> (tagged.component_data.get_buffer ()), tagged.component_data.length ());
	CORBA::Boolean byte_order;
	if ((cdr >> ACE_InputCDR::to_boolean (byte_order)) == 0)
	{
	    DEBUG_ECHO(ECHO_HEADER());
	    DEBUG_ECHO("get_lb_tagged_components ");
	    DEBUG_ECHO("Failed to decode TAG_LB_GROUP Component.\n");
	    DEBUG_ECHO("Raising TagNotFound() exception.\n");
	    throw CdmwLB::TagNotFound(); 
	} 
	
	cdr.reset_byte_order (static_cast<int> (byte_order));
	CdmwLB::TagLBGroupTaggedComponent* result = new CdmwLB::TagLBGroupTaggedComponent();   
 	// Extract the group component
	cdr >> *result;
	return result;
	
    }
    else   
    {
	DEBUG_ECHO(ECHO_HEADER());
	DEBUG_ECHO("get_lb_tagged_components ");
	DEBUG_ECHO("Failed to decode TAG_LB_GROUP Component.\n");
	DEBUG_ECHO("Raising TagNotFound() exception.\n");
	throw CdmwLB::TagNotFound(); 
    }
}



    /**
     * decode the specified profile to extract the CdmwLB::TagFallbackTaggedComponent
     * structure
     *
     * @param obj The IOGR.
     * @param the profile to decode
     *
     * @return the CdmwLB::TagFallbackTaggedComponent structure
     *
     * @exception ProfileNotFound if the profile number is over the limit
     * @exception TagNotFound if the TAG_FALLBACK is not found into the profile
     */
    
    CdmwLB::TagFallbackTaggedComponent
    TAO_LB_IOGRFactory::decode_profile_with_fallback_tag (CORBA::Object_ptr obj,
                                                          CORBA::Long profile)
        throw(CORBA::SystemException,
              CdmwLB::ProfileNotFound,
              CdmwLB::TagNotFound)
    {
        // Get the MProfile
        TAO_MProfile &mprofile =
            obj->_stubobj ()->base_profiles ();
        unsigned int nb_profile = profile;
        if ((mprofile.profile_count ()-1) < nb_profile)
        {
            DEBUG_ECHO(ECHO_HEADER());
            DEBUG_ECHO("decode_profile_with_fallback_tag (profile n. " << profile << ")': ");
            DEBUG_ECHO("Failed to decode TAG_FALLBACK Component.\n");
            DEBUG_ECHO("Raising ProfileNotFound() exception.\n");
            throw  CdmwLB::ProfileNotFound();
        }
        
        CdmwLB::TagFallbackTaggedComponent result;
        
        
        const TAO_Tagged_Components &tagged_components =
            mprofile.get_profile (profile)->tagged_components ();
        
        IOP::TaggedComponent tagged;
        tagged.tag = CdmwLB::TAG_FALLBACK;
        
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
                DEBUG_ECHO(ECHO_HEADER());
                DEBUG_ECHO("decode_profile_with_fallback_tag (profile n. " << profile << ")': ");
                DEBUG_ECHO("Failed to decode TAG_FALLBACK Component.\n");
                DEBUG_ECHO("Raising TagNotFound() exception.\n");
                throw CdmwLB::TagNotFound();
            }
            
            cdr.reset_byte_order (ACE_static_cast(int,byte_order));
            
            // Extract the group component
            cdr >> result;
            return result;
            
        } else
          {
              DEBUG_ECHO(ECHO_HEADER());
              DEBUG_ECHO("decode_profile_with_fallback_tag (profile n. " << profile << ")': ");
              DEBUG_ECHO("Failed to decode TAG_FALLBACK Component.\n");
              DEBUG_ECHO("Raising TagNotFound() exception.\n");
              throw CdmwLB::TagNotFound(); 
          }
    }

    /**
     * decode an IOP::ServiceCOntext in order to extract the CdmwLB::LBGroupVersionServiceContext
     * structure
     *
     * @param the context to decode
     *
     * @return the CdmwLB::LBGroupVersionServiceContext structure
     *
     */

    CdmwLB::LBGroupVersionServiceContext
    TAO_LB_IOGRFactory::decode_group_version_service_context(const IOP::ServiceContext & context)
        throw (CORBA::SystemException)
    {
        CdmwLB::LBGroupVersionServiceContext result;
        TAO_InputCDR cdr (ACE_reinterpret_cast (const char*,
                                                context.context_data.get_buffer ()),
                          context.context_data.length ());
        CORBA::Boolean byte_order;

        if ((cdr >> ACE_InputCDR::to_boolean (byte_order)) == 0)
            return result;

        cdr.reset_byte_order (ACE_static_cast (int,byte_order));

        cdr >> result;
        return result;
    }
    
    /**
     * Set fallback replica
     */
    
    CORBA::Boolean
    TAO_LB_IOGRFactory::set_fallback(CORBA::Object_ptr fallback)
        throw (CORBA::SystemException)
    {
        IOP::TaggedComponent tagged_components;
        tagged_components.tag = CdmwLB::TAG_FALLBACK;
        
        CORBA::Boolean val = 1;
        
        TAO_OutputCDR cdr;
        cdr << ACE_OutputCDR::from_boolean (TAO_ENCAP_BYTE_ORDER);
        cdr << ACE_OutputCDR::from_boolean (val);
        
        // Get the length of the CDR stream
        CORBA::ULong length = ACE_static_cast (CORBA::ULong,
                                               cdr.total_length ());
        
        // Set the length
        tagged_components.component_data.length (length);
        
        // Get the pointer to the underlying buffer
        CORBA::Octet *buf =
            tagged_components.component_data.get_buffer ();
        
        for (const ACE_Message_Block *mb = cdr.begin ();
             mb != 0;
             mb = mb->cont ())
            {
                ACE_OS::memcpy (buf, mb->rd_ptr (), mb->length ());
                buf += mb->length ();
            }
        
        
        // Set the <tagged_component>
        TAO_Tagged_Components &tagg =
            fallback->_stubobj ()->base_profiles ().get_profile (0)->tagged_components ();
        
        tagg.set_component (tagged_components);
        return 1;
    }
    
}; // namespace LB
}; // namespace Cdmw

