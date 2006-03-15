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

#ifndef _CDMW_FT_REPLICATION_MANAGER_IOGR_FACTORY_HPP_
#define _CDMW_FT_REPLICATION_MANAGER_IOGR_FACTORY_HPP_

#include <vector>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include "FaultTolerance/idllib/CdmwFTIOGRFactory.skel.hpp"

namespace Cdmw {
namespace FT {


    class IOGRFactory : public virtual POA_CdmwFT::IOGRFactory,
                        public virtual Cdmw::OrbSupport::RefCountServantBase 
    {
    public:
        
        /**
         * Constructor
         */       

        IOGRFactory(CORBA::ORB_ptr          orb,
                    std::ostream &          os)
            throw (CORBA::SystemException);
        
        /**
         * Destructor
         */
        virtual ~IOGRFactory() throw();

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
        virtual
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
                  CORBA::SystemException) = 0;


    
        /**
         * count the number of profile found into the IOGR
         *
         * @param obj The IOGR.
         *
         * @return the  number of profile found
         */
        virtual
        CORBA::Long 
        profile_count (CORBA::Object_ptr obj)
            throw (CORBA::SystemException) = 0;
    

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
        virtual
        ::FT::TagFTGroupTaggedComponent * 
        decode_profile_with_group_tag (CORBA::Object_ptr obj,
                                       CORBA::Long profile)
            throw(CORBA::SystemException,
                  CdmwFT::ProfileNotFound,
                  CdmwFT::TagNotFound) = 0;

    
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
        virtual 
        ::FT::TagFTPrimaryTaggedComponent
        decode_profile_with_primary_tag (CORBA::Object_ptr obj,
                                         CORBA::Long profile)
            throw(CORBA::SystemException,
                  CdmwFT::ProfileNotFound,
                  CdmwFT::TagNotFound) = 0;

    protected:
        // The ORB pseudo-object.
        CORBA::ORB_var m_orb;
        
        // The parent POA for fallback IORs.
        PortableServer::POA_var m_poa;

        // A reference to an output stream for debug and trace information
        std::ostream & m_ostream;
    };    
    

 
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_REPLICATION_MANAGER_IOGR_FACTORY_HPP_

