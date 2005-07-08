/* =========================================================================== *
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
 * =========================================================================== */


// Standard Files 
#include <string>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>

#include <tao/IORManipulation/IORManip_Loader.h>
#include <orbsvcs/FaultTolerance/FT_ClientService_Activate.h>

#include <faulttolerance/TestUtils.hpp>

#include <FaultTolerance/idllib/FT.stub.hpp>
#include <FaultTolerance/idllib/CdmwFTReplicationManager.stub.hpp>

namespace
{
    const int NO_PRIMARY = -1;
} // end anonymous namespace

namespace Cdmw {
namespace FT {

int TestUtils::M_current_primary_index = NO_PRIMARY;

::FT::ObjectGroup_ptr TestUtils::create_group(
    CORBA::ORB_ptr orb,
    CdmwFT::ReplicationManager_ptr replication_manager,
    const char* rep_id )
throw( CORBA::Exception )
{
    // set default properties
    std::cout << "set_default_properties" << std::endl;

    ::FT::Properties prop;
        prop.length( 4 );
        prop[0].nam.length( 1 );
        prop[0].nam[0].id = "MinimumNumberReplicas";
        prop[0].val <<= (CORBA::UShort) 2;

        prop[1].nam.length( 1 );
        prop[1].nam[0].id = "ReplicationStyle";
        prop[1].val <<= (CORBA::UShort) 2;

        prop[2].nam.length( 1 );
        prop[2].nam[0].id = "MemberShipStyle";
        prop[2].val <<= (CORBA::UShort) 0;

        prop[3].nam.length( 1 );
        prop[3].nam[0].id = "FaultMonitoringStyle";
        prop[3].val <<= (CORBA::UShort) 0;

    const ::FT::Criteria ft_criteria = prop;
    ::FT::GenericFactory::FactoryCreationId_var factory_creation_id;
    ::FT::ObjectGroup_var obj_group;

    // create the object group 
    std::cout << "Create an object group" << std::endl;
    try
    {
        obj_group =  replication_manager->create_object(
            rep_id, ft_criteria, factory_creation_id.out() );
    }
    catch( CORBA::Exception& e )
    {
        std::cerr << e._name() << std::endl;
        throw;
    }

    return obj_group._retn();
}

/**********************************************************************************/

::FT::ObjectGroup_ptr TestUtils::change_primary(
    ::FT::ObjectGroup_ptr object_group,
    CdmwFT::ReplicationManager_ptr replication_manager,
    const char* host_name,
    const char* appl_name,
    const char* proc_name )
throw( CORBA::Exception )
{
    try
    {
        std::cout << "Set a primary on the group" << std::endl;

        ::FT::Location loc;
            loc.length(3);
            loc[0].id = host_name;
            loc[0].kind = "hostname";
            loc[1].id = appl_name;
            loc[1].kind = "applicationname";
            loc[2].id = proc_name;
            loc[2].kind = "processname";   

        ::FT::ObjectGroup_var new_obj_group 
            = replication_manager->set_primary_member( object_group, loc );
        M_current_primary_index = 0;

        return new_obj_group._retn();
    }
    catch( ::FT::ObjectGroupNotFound& e )
    {
        std::cerr << " ObjectGroupNotFound Exception raised! " << std::endl;
        std::cerr << " **** TEST FAILED AT LINE " << __LINE__ 
            << " IN FILE: " << __FILE__ << std::endl; 
        throw;
    }
}

/**********************************************************************************/

::FT::ObjectGroup_ptr
TestUtils::update_group_with_primary_outside_cdmw_ft
                         (CORBA::ORB_ptr orb, 
                          ::FT::ObjectGroup_ptr object_group,
                          CdmwFT::ReplicationManager_ptr replication_manager,
                          const char* host_name,
                          const char* appl_name,
                          const char* proc_name)
   throw (CORBA::Exception)
{
    // Initialize the IOR manipulation.
    CORBA::Object_var obj 
        = orb->resolve_initial_references(TAO_OBJID_IORMANIPULATION);
    ::TAO_IOP::TAO_IOR_Manipulation_var iogrm 
        = ::TAO_IOP::TAO_IOR_Manipulation::_narrow(obj.in());

    // get members of the group
    ::CdmwFT::GroupInfo_var group_info 
        = replication_manager->get_object_group_info(object_group); 

    // create object group
    CORBA::ULong members_length = group_info->members.length();;
    ::FT::ObjectGroup_var new_object_group = ::FT::ObjectGroup::_nil();
    CORBA::Object_var primary_object   = CORBA::Object::_nil();

    // Create IOR list for use with merge_iors.
    ::TAO_IOP::TAO_IOR_Manipulation::IORList iors(members_length);
    iors.length(members_length);
    
    // TODO: Check when there's no primary_location specified
    CORBA::Boolean primary_found = false;
    
    std::string s_primary_location("");
    bool check_primary_location=false;
    
    using namespace Cdmw::NamingAndRepository;
    ::FT::Location primary_location;
    primary_location.length(3);
    primary_location[0].id = host_name;
    primary_location[0].kind = "hostname";
    primary_location[1].id = appl_name;
    primary_location[1].kind = "applicationname";
    primary_location[2].id = proc_name;
    primary_location[2].kind = "processname";   
    
    if (primary_location.length() >= 1) {
        check_primary_location=true;
        s_primary_location = NamingInterface::to_string(primary_location);
    }
    
    CORBA::PolicyList no_policy;
    no_policy.length(0);
    for (CORBA::ULong iMembers = 0; iMembers < members_length; ++iMembers) {
        // FIXME: a workaround!
        CORBA::String_var s_obj_ = orb->object_to_string(group_info->members[iMembers].the_reference.in());
        CORBA::Object_var obj_ = orb->string_to_object(s_obj_.in());

        iors[iMembers] = obj_;
        
        if (check_primary_location) {
            std::string s_member_loc = 
                NamingInterface::to_string (group_info->members[iMembers].the_location);
            
            if (s_member_loc == s_primary_location) {
                primary_object = obj_; // CORBA::Object::_duplicate(group_info->members[iMembers].the_reference);
                primary_found = true;
            }
        }
    }
       
    // Create the object group.
    // merge_iors may throw the following exceptions:
    //        CORBA::SystemException,
    //        TAO_IOP::EmptyProfileList,
    //        TAO_IOP::Duplicate, and
    //        TAO_IOP::Invalid_IOR
    try {
        new_object_group = iogrm->merge_iors(iors);
    } catch (const CORBA::SystemException & ex) {
        std::cerr << "build_iogr()': Cannot merge IORs.\n"
            << "Exception : " << ex << "\n"
            << "Forwarding initial exception." << std::endl;
        throw;
    } catch (const CORBA::Exception & ex) {
        // TODO: Add Trace information
        std::cerr << "build_iogr()': Cannot merge IORs.\n"
            << "Exception : " << ex << "\n"
            << "Raising ObjectNotCreated() exception." << std::endl;
        throw ::FT::ObjectNotCreated();
    }
    
    // Create the TAG_FT_GROUP.
    ::FT::TagFTGroupTaggedComponent ft_tag_component;
    ::TAO_FT_IOGR_Property property(ft_tag_component);
    
    // IOGR properties.
    
    // TAG_GROUP_VERSION major and minor revision numbers (always 1.0).
    ft_tag_component.version.major = (CORBA::Octet) 1;
    ft_tag_component.version.minor = (CORBA::Octet) 0;
    
    // Fault tolerance domain id.
    ft_tag_component.ft_domain_id = CORBA::string_dup("My_FTdomain");
    
    // Object group id.
    ft_tag_component.object_group_id = group_info->object_group_id;
    
    // Object group reference version.
    ft_tag_component.object_group_ref_version = group_info->object_group_ref_version + 1;
    
    
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
                iogrm->set_primary(&property, primary_object.in(), new_object_group.in());
        } catch (const CORBA::SystemException & ex) {
            std::cerr << "build_iogr(" << s_primary_location << ")': "
                << "Cannot set primary.\n"
                << "Exception : " << ex << "\n"
                << "Forwarding initial exception." << std::endl;
            throw;
        } catch (const CORBA::Exception & ex) {
            // TODO: Add Trace information
            std::cerr << "build_iogr(" << s_primary_location << ")': "
                << "Cannot set primary.\n"
                << "Exception : " << ex << "\n"
                << "Raising PrimaryNotSet() exception." << std::endl;
            throw ::FT::PrimaryNotSet();
        }
        if (retval == 0) {
            std::cerr << "build_iogr(" << s_primary_location << ")': "
                << "Failed to set primary.\n"
                << "Raising PrimaryNotSet() exception." << std::endl;
            throw ::FT::PrimaryNotSet();
        }
    }
    
    // Set the property.
    // set_property may raise the following exceptions:
    //        CORBA::SystemException,
    //        TAO_IOP::Invalid_IOR, and
    //        TAO_IOP::Duplicate
    CORBA::Boolean retval = 0;
    try {
        retval = iogrm->set_property(&property, new_object_group.in());
    } catch (const CORBA::SystemException & ex) {
        std::cerr << "build_iogr(" << s_primary_location << ")': "
            << "Failed to set TAG_FT_GROUP Component.\n"
            << "Exception : " << ex << "\n"
            << "Forwarding initial exception." << std::endl;
        throw;
    } catch (const CORBA::Exception & ex) {
        // TODO: Add Trace information
        std::cerr << "build_iogr(" << s_primary_location << ")': "
            << "Failed to set TAG_FT_GROUP Component.\n"
            << "Exception : " << ex << "\n"
            << "Raising ObjectNotCreated() exception." << std::endl;
        throw ::FT::ObjectNotCreated();
    }
    if (retval == 0) {
        std::cerr << "build_iogr(" << s_primary_location << ")': "
            << "Failed to set TAG_FT_GROUP Component.\n"
            << "Raising ObjectNotCreated() exception." << std::endl;
        throw ::FT::ObjectNotCreated();
    }

    return new_object_group._retn();
}



} // end namespace FT
} // end namespace Cdmw

