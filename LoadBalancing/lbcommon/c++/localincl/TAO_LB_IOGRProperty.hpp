/* ========================================================================== *
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. All rights reserved.
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
 * ========================================================================= */

#ifndef INCL_CDMW_LB_TAO_LB_IOGR_PROPERTY_H
#define INCL_CDMW_LB_TAO_LB_IOGR_PROPERTY_H

/**
 * @brief A class to set IOGR Property
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "idllib/CdmwLBCommon.stub.hpp"

//TAO specific files
#include "tao/IOPC.h"
#include "tao/MProfile.h"
#include "tao/Profile.h"
#include "tao/Stub.h"
#include "tao/ORB_Core.h"
#include "tao/Tagged_Components.h"
#include "ace/Auto_Ptr.h"

namespace Cdmw
{

namespace LB
{

    class TAO_LB_IOGRProperty
    {
    public:
        
        //constructor
        TAO_LB_IOGRProperty (CdmwLB::TagLBGroupTaggedComponent &lb_group);
        
        //destructor
        ~TAO_LB_IOGRProperty (void);
        
        
        /**
         * Reset the Tagged Component attribute in this class
         * @param lb_group The new TAG_LB_GROUP Tagged Component
         */
        
        CORBA::Boolean
        reset_tagged_components (CdmwLB::TagLBGroupTaggedComponent &lb_group);
        
        /**
         * Add LB_TAG_GROUP Tagged Component to all profiles in the iogr
         * @param ior The iogr to set
         */
        
        CORBA::Boolean
        set_property(CORBA::Object_ptr ior)
            throw (CORBA::SystemException);
        
        /**
         * Encode the properties in a tagged component
         * @param cdr The output stream containing the properties
         * @param tagged_components The Tagged Component to set
         */
        
        CORBA::Boolean
        encode_properties (TAO_OutputCDR &cdr,
                           IOP::TaggedComponent &tagged_components);
        
        
        // The Tagged Component containing IOGR properties
        CdmwLB::TagLBGroupTaggedComponent * m_lb_group_tagged_component;
    };
    
};//close namespace LB
    
};//close namespace Cdmw

#endif// INCL_CDMW_LB_TAO_LB_IOGR_PROPERTY_H
