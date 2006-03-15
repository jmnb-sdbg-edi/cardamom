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

#ifndef _CDMW_FT_REPLICATION_MANAGER_TAO_IOGR_FACTORY_HPP_
#define _CDMW_FT_REPLICATION_MANAGER_TAO_IOGR_FACTORY_HPP_

#include <vector>
#include <Foundation/orbsupport/CORBA.hpp>

#include "FaultTolerance/idllib/CdmwFTCommon.stub.hpp"
#if CDMW_ORB_VER < 14
#include <orbsvcs/FaultTolerance/FT_Service_Activate.h>
#else
#include "tao/IORManipulation/IORManip_Loader.h"
#include <orbsvcs/FaultTolerance/FT_ClientService_Activate.h>
#include <orbsvcs/FaultTolerance/FT_IOGR_Property.h>
#endif
#include <tao/MProfile.h>
#include <tao/Profile.h>
#include <tao/Stub.h>
#include <FaultTolerance/ftcommon/IOGRFactory.hpp>

namespace Cdmw {
namespace FT {
    
    class TAO_IOGRFactory : public IOGRFactory
    {
    public:
                
        /**
         * Constructor
         */
        TAO_IOGRFactory(CORBA::ORB_ptr          orb,
                        std::ostream &          os)
            throw (CORBA::SystemException);
        
        
        /**
         * Destructor
         */
        ~TAO_IOGRFactory() throw();
        
        /**
         * Build the iogr
         *
         * @param Members          sequence of members & locations.
         * @param primary_location location of the primary member.
         * @param object_group_id  the object group id to use for the search.
         * @param ft_domain_id     The fault tolerance domain id.
         * @param object_group_version the object group reference version.
         *
         * @return the reference to the object created 
         *
         * @exception InvalidProperty if one or more of the properties in the
         *        sequence is not valid.
         * @exception UnsupportedProperty if one or more of the properties in
         *        the sequence is not supported.
         */
        CORBA::Object_ptr
        build_iogr(const ::CdmwFT::MemberInfos & members,
                   const ::FT::Location &      primary_location,
                   ::FT::ObjectGroupId         object_group_id,
                   const char *                type_id,
                   const char *                ft_domain_id,
                   ::FT::ObjectGroupRefVersion & object_group_ref_version, 
                   const CdmwFT::Objects & fall_backs)
            throw(::FT::PrimaryNotSet,
                  ::FT::ObjectNotCreated,
                  CORBA::SystemException);

    
        /**
         * count the number of profile found into the IOGR
         *
         * @param obj The IOGR.
         *
         * @return the  number of profile found
         */
        CORBA::Long 
        profile_count (CORBA::Object_ptr obj)
            throw (CORBA::SystemException);
    

        /**
         * decode the specified profile to extract the FT::TagFTGroupTaggedComponent 
         * structure 
         *
         * @param obj The IOGR.
         * @param the profile to decode
         *
         * @return the FT::TagFTGroupTaggedComponent structure
         *
         * @exception ProfileNotFound if the profile number is over the limit
         * @exception TagNotFound if the TAG_FT_GROUP is not found into the profile
         */    
        ::FT::TagFTGroupTaggedComponent * 
        decode_profile_with_group_tag (CORBA::Object_ptr obj,
                                       CORBA::Long profile)
            throw(CORBA::SystemException,
                  CdmwFT::ProfileNotFound,
                  CdmwFT::TagNotFound);

    
        /**
         * decode the specified profile to extract the FT::TagFTPrimaryTaggedComponent 
         * structure 
         *
         * @param obj The IOGR.
         * @param the profile to decode
         *
         * @return the FT::TagFTPrimaryTaggedComponent structure
         *
         * @exception ProfileNotFound if the profile number is over the limit
         * @exception TagNotFound if the TAG_FT_PRIMARY is not found into the profile
         */        
        ::FT::TagFTPrimaryTaggedComponent
        decode_profile_with_primary_tag (CORBA::Object_ptr obj,
                                         CORBA::Long profile)
            throw(CORBA::SystemException,
                  CdmwFT::ProfileNotFound,
                  CdmwFT::TagNotFound);
        
    private:
        
        // For IOGR Manipulation.
        ::TAO_IOP::TAO_IOR_Manipulation_var m_iogrm;
        
    };
    
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_REPLICATION_MANAGER_TAO_IOGR_FACTORY_HPP_

