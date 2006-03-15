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
 * @brief A class to set IOGR Property
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "lbcommon/TAO_LB_IOGRProperty.hpp"
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>


namespace Cdmw
{
    
namespace LB
{

    /**
     * Constructor
     */
    TAO_LB_IOGRProperty::TAO_LB_IOGRProperty (
        CdmwLB::TagLBGroupTaggedComponent &lb_group)
        :m_lb_group_tagged_component (&lb_group)
    {
    }

    /**
     * Destructor
     */
    TAO_LB_IOGRProperty::~TAO_LB_IOGRProperty (void)
    {
    }

    
    /**
     * Add LB_TAG_GROUP Tagged Component to all profiles in the iogr
     * @param ior The iogr to set
     */
        
    CORBA::Boolean
    TAO_LB_IOGRProperty::set_property(CORBA::Object_ptr ior)
        throw (CORBA::SystemException)
    {
        // make sure we have some profiles
        if (ior->_stubobj ()->base_profiles ().profile_count () == 0)
            throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
        // We need to apply the property for every profile in the IOR
        
        // Make profiles from the sent <ior>
        TAO_MProfile &tmp_pfiles =
            ior->_stubobj ()->base_profiles ();
        
        // Get the profile count.
        CORBA::ULong count =
            ior->_stubobj ()->base_profiles ().profile_count ();
        
        // Make the output CDR stream
        TAO_OutputCDR cdr;
        CORBA::Boolean retval = 0;
        retval =
            cdr << ACE_OutputCDR::from_boolean (TAO_ENCAP_BYTE_ORDER);
        
        IOP::TaggedComponent tagged_components;
        tagged_components.tag = CdmwLB::TAG_LB_GROUP;
        
        // Encode the property in to the tagged_components
        retval = retval &&
            this->encode_properties (cdr,
                                     tagged_components);
        
        const IOP::TaggedComponent &tmp_tc = tagged_components;
        // Go through every profile and set the TaggedComponent field
        for (CORBA::ULong i = 0; i < count ; i++)
        {
            // Get the tagged components in the profile
            TAO_Tagged_Components &tag_comp =
                tmp_pfiles.get_profile (i)->tagged_components ();
            
            // Finally set the <tagged_component> in the
            // <TAO_Tagged_Component>
            tag_comp.set_component (tmp_tc);
        }
        
        // Success
        return 1;
    }

    /**
     * Encode the properties in a tagged component
     * @param cdr The output stream containing the properties
     * @param tagged_components The Tagged Component to set
     */
    
    CORBA::Boolean
    TAO_LB_IOGRProperty::encode_properties (
        TAO_OutputCDR &cdr,
        IOP::TaggedComponent &tagged_components)
    {
        // the version info
        CORBA::Boolean status =
            cdr << this->m_lb_group_tagged_component->version;
        
        // the domain id
        status = status &&
            cdr << this->m_lb_group_tagged_component->lb_domain_id.in ();
        
        // Object group id
        status = status &&
            cdr << this->m_lb_group_tagged_component->object_group_id;
        
        // Object group reference version
        status = status &&
            cdr << this->m_lb_group_tagged_component->object_group_ref_version;
        
        status = status &&
            cdr << this->m_lb_group_tagged_component->lb_policy.in();
        
        // Get the length of the CDR stream
        CORBA::ULong length =ACE_static_cast (CORBA::ULong, cdr.total_length ());
        
        // Set the length
        tagged_components.component_data.length (length);
        
        // Get the pointer to the underlying buffer
        CORBA::Octet *buf =
            tagged_components.component_data.get_buffer ();
        
        for (const ACE_Message_Block *i = cdr.begin ();
             i != 0;
             i = i->cont ())
        {
            ACE_OS::memcpy (buf, i->rd_ptr (), i->length ());
            buf += i->length ();
        }
        
        
        return status;
    }
}//clode namespace LB
    
}//close namespace Cdmw
