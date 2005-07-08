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

#ifndef INCL_CDMW_LB_IOGR_FACTORY_HPP_
#define INCL_CDMW_LB_IOGR_FACTORY_HPP_

/**
 * @file
 * @brief IOGRFactory implmentation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include <vector>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <LoadBalancing/idllib/CdmwLBIOGRFactory.skel.hpp>

namespace Cdmw
{
    
namespace LB
{

    class IOGRFactory : public virtual POA_CdmwLB::IOGRFactory,
                        public virtual Cdmw::OrbSupport::RefCountServantBase 
    {
    public:
        
        /**
         * Constructor
         */       

        IOGRFactory(std::ostream & os,
                    CORBA::ORB_ptr orb)
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
         * @param lb_domain_id     The fault tolerance domain id.
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
        build_iogr(const ::CdmwLB::MemberInfos & members,
                   PortableGroup::ObjectGroupId         object_group_id,
                   const char *                type_id,
                   const char *                lb_domain_id,
                   const char *                lb_policy,
                   PortableGroup::ObjectGroupRefVersion & object_group_ref_version,
                   const CdmwLB::Objects & fall_backs)
          throw(PortableGroup::ObjectNotCreated,
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
         * decode the specified profile to extract the CdmwLB::TagLBGroupTaggedComponent 
         * structure 
         *
         * @param obj The IOGR.
         * @param the profile to decode
         *
         * @return the CdmwLB::TagLBGroupTaggedComponent structure
         *
         * @exception ProfileNotFound if the profile number is over the limit
         * @exception TagNotFound if the TAG_FT_GROUP is not found into the profile
         */    
        virtual
        CdmwLB::TagLBGroupTaggedComponent * 
        decode_profile_with_group_tag (CORBA::Object_ptr obj,
                                       CORBA::Long profile)
            throw(CORBA::SystemException,
                  CdmwLB::ProfileNotFound,
                  CdmwLB::TagNotFound) = 0;
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
        virtual
        CdmwLB::TagFallbackTaggedComponent
        decode_profile_with_fallback_tag (CORBA::Object_ptr obj,
                                          CORBA::Long profile)
            throw(CORBA::SystemException,
                  CdmwLB::ProfileNotFound,
                  CdmwLB::TagNotFound) = 0;

        /**
         * create the LB_GROUP_VERSION service context for the specified object group
         *
         * @param group_version The group version used to create the Group Version Service Context.
         *
         * @return the IOP::ServiceContext structure
         */
        virtual
        IOP::ServiceContext *
        create_group_version_service_context(CORBA::ULong group_version, PortableGroup::ObjectGroupId)
            throw (CORBA::SystemException) = 0;
        
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
        virtual
        CORBA::Object_ptr
        get_member_ref(PortableGroup::ObjectGroup_ptr object_group, CORBA::Long profile)
            throw (CORBA::SystemException,
                   PortableGroup::MemberNotFound) = 0;
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
        decode_group_version_service_context(const IOP::ServiceContext & context)
            throw (CORBA::SystemException) = 0;
            

        

    protected:
        // The ORB pseudo-object.
        CORBA::ORB_var m_orb;
        
        // The parent POA for fallback IORs.
        PortableServer::POA_var m_poa;

        // A reference to an output stream for debug and trace information
        std::ostream & m_ostream;
    };    
    

 
}; // namespace LB
}; // namespace Cdmw

#endif // INCL_CDMW_LB_IOGR_FACTORY_HPP_

