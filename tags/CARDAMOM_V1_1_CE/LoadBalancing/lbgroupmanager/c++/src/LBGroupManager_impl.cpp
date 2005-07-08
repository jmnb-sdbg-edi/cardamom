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

/**
 * @brief A Group Manager implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "lbgroupmanager/LBGroupManager_impl.hpp"
#include "lbgroupmanager/LBUtils.hpp"

#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <iostream>
#include "Foundation/common/Exception.hpp"

#include "LoadBalancing/idllib/PortableGroup.stub.hpp"

#include <sstream>

#if CDMW_ORB_VDR == tao
#include <LoadBalancing/lbcommon/TAO_LB_IOGRFactory.hpp>

#else
#error "Unsupported ORB"
#endif
#define ECHO_HEADER() \
    "[CDMW LB Group Manager] (file: " << __FILE__ << ", line: " << __LINE__ << ")\n -->"

#define ECHO_ERROR(comment) \
do {\
    std::cerr << ECHO_HEADER() << " --> " << comment << std::endl;\
} while(0)


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
    std::cerr << comment:\
} while(0)

#endif

/**
 *
 * Compare two PortableGroup::Locations. Locations are assumed to be valid CosNaming::Names.
 * The parameter depth allows to limit the comparison to the 'depth' first name
 * components. When 'depth' has the default value 0, the comparison is made on
 * the whole Locations.
 *
 * @param l1 The first location to compare
 * @param l2 The second location to compare
 *
 * @return true if locations are equal, false else
 */
bool
Compare_Locations(const ::PortableGroup::Location& l1, const ::PortableGroup::Location& l2, ::CORBA::ULong depth )
{
    CORBA::ULong l1_len = l1.length();
    CORBA::ULong l2_len = l2.length();
    CORBA::ULong actual_depth;

    if (l1_len != l2_len) {
        if (depth == 0)                           return false;
        if ((l1_len < depth) || (l2_len < depth)) return false;
        actual_depth = depth;
    }
    else {
        actual_depth = (depth == 0)? l1_len: depth;
    }

    for (CORBA::ULong i=0; i<actual_depth; i++) {
        if (strcmp(l1[i].id.in(), l2[i].id.in()))      return false;
        if (strcmp(l1[i].kind.in(), l2[i].kind.in()))  return false;
    }

    return true;
}


namespace Cdmw
{

namespace LB
{

    /**
     * Creates a group manager.
     *
     * @param lb_domain_id The load balancing domain id.
     */
    LBGroupManager_impl::LBGroupManager_impl(CORBA::ORB_ptr orb,
                                             PortableServer::POA_ptr poa,
                                             const PortableGroup::GroupDomainId lb_domain_id,
                                             std::ostream & os)
        throw(CORBA::SystemException)
        : m_object_group_id(0L),
          m_orb(CORBA::ORB::_duplicate(orb)),
          m_lb_domain_id(lb_domain_id),
          m_ostream(os)
        
    {
	PortableServer::POAManager_var poa_mgr = poa->the_POAManager();

        CORBA::PolicyList policies;
        policies.length(6);
        policies[0] =
            poa->create_id_assignment_policy(PortableServer::USER_ID);
        policies[1] =
            poa->create_lifespan_policy(PortableServer::TRANSIENT);
        policies[2] =
            poa->create_servant_retention_policy(PortableServer::NON_RETAIN);
        policies[3] =
            poa->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
	policies[4] = 
	    poa->create_request_processing_policy(PortableServer::USE_DEFAULT_SERVANT);
	policies[5] = 
	    poa->create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);
	

        m_poa = poa->create_POA("ReplicationManagerPOA", poa_mgr.in(), policies);

      
	m_iogr_factory = new TAO_LB_IOGRFactory(std::cout, m_orb.in());
    }
    
    /**
     * Destroys a group manager.
     */
    
    LBGroupManager_impl::~LBGroupManager_impl()
    {
        for (m_group_pos=m_group_map.begin(); m_group_pos!=m_group_map.end();++m_group_pos)
        {
            delete m_group_pos->second;
        }
    }
    
    /**
     * IDL:omg.org/PortableGroup/GenericFactory/create_object:1.0
     *
     * Creates an object, using the <code>type_id</code> parameter to determine
     * which type of object to create and <code>the_criteria</code> parameter to
     * determine restrictions on how and where to create the object. The out
     * parameter, <code>factory_creation_id</code>, allows the entity that
     * invoked the factory, and the factory itself, to identify the object for
     * subsequent deletion. If the application invokes the <code>create_object()
     * </code> operation on the <code>GenericFactory</code> interface, inherited
      * by the Group Manager, then it creates an object group. 
      *
      * @param type_id             The repository identifier of the object to be
      *                            created by the factory.
      * @param the_criteria        Information passed to the factory, which the
      *                            factory evaluates before creating the object.
      * @param factory_creation_id An identifier that allows the factory to
      *                            delete the object subsequently.
      *
      * @return the reference to the object created by the <code>GenericFactory
      *         </code>. When the <code>GenericFactory</code> interface is
      *         inherited by the Group Manager, the <code>create_object()
      *         </code> operation returns an object group reference as a result.
      *
      * @exception NoFactory if the object cannot be created. When the <code>
      *         GenericFactory</code> interface is inherited by the Group
      *         Manager, the raised exception indicates that the Group
      *         Manager cannot create the object group because it cannot find a
      *         factory that is capable of constructing a member of the object
      *         group of the <code>type_id</code> at the location.
      * @exception ObjectNotCreated if the factory cannot create the object.
      * @exception InvalidCriteria if the factory does not understand the criteria.
      * @exception InvalidProperty if a property passed in as criteria is invalid.
      * @exception CannotMeetCriteria if the factory understands the criteria but
      *         cannot satisfy it.
      */
    
    PortableGroup::ObjectGroup_ptr
    LBGroupManager_impl::create_object (const char * type_id,
                                        const PortableGroup::Criteria & the_criteria,
                                        PortableGroup::GenericFactory::FactoryCreationId_out factory_creation_id)
        throw (CORBA::SystemException,
               PortableGroup::NoFactory,
               PortableGroup::ObjectNotCreated,
               PortableGroup::InvalidCriteria,
               PortableGroup::InvalidProperty,
               PortableGroup::CannotMeetCriteria)
     {
         CDMW_MUTEX_GUARD (m_mutex);
         
         CORBA::Object_var object_group = CORBA::Object::_nil();
         // Create an ObjectGroupData object
         CdmwLB::ObjectGroupData * group_data = new CdmwLB::ObjectGroupData();
         CORBA::ULong object_group_ref_version = 0L;
         CdmwLB::MemberInfos no_members;
         no_members.length(0 );
         ++m_object_group_id;
	 PortableGroup::ObjectGroupRefVersion group_version = object_group_ref_version + 1;
         
         std::ostringstream oss;

         oss << m_object_group_id << "/" << group_version << "/" << std::string(type_id);
         PortableServer::ObjectId_var oid =
             PortableServer::string_to_ObjectId(oss.str().c_str());

         
         CORBA::Object_var group_manager_ref = CORBA::Object::_nil();
         group_manager_ref= m_poa->create_reference_with_id(oid.in(),type_id);


         CdmwLB::Objects_var fallBack = new CdmwLB::Objects;
         fallBack->length(1);
         fallBack[0L] = CORBA::Object::_duplicate(group_manager_ref.in());



         // Look for a criterion named LBProperties and if it is found
         // then extract load balancing policy
         std::string lb_policy( "default");
         CORBA::ULong i = 0, criteria_length = the_criteria.length();

         while (i < criteria_length)
         {
             if (strcmp(the_criteria[i].nam[0].id,
                        "cdmw.lb.LBProperties") == 0)
             {
                 // Extract the LB properties from the criteria.
                 PortableGroup::Properties * lb_properties;
                 the_criteria[i].val >>= lb_properties;

                 CORBA::ULong props_length = lb_properties->length();
                 for (CORBA::ULong iProps = 0; iProps < props_length; ++iProps)
                 {
                     // Get the property name.
                     PortableGroup::Property property =(*lb_properties)[iProps];
                     std::string propertyName = std::string(property.nam[0].id);
                     if (propertyName == "cdmw.lb.LoadBalancingPolicy")
                     {
                         const char * str;
                         property.val >>= str;
                         lb_policy = CORBA::string_dup (str);
                         break;
                     }
                 }
                 break;
             }
         }

         try
         {
             object_group = m_iogr_factory->build_iogr(no_members,
                                                       m_object_group_id,
                                                       type_id,
                                                       m_lb_domain_id.in(),
                                                       lb_policy.c_str(),
                                                       object_group_ref_version,
                                                       fallBack.in());
         }catch(const PortableGroup::ObjectNotCreated&)
         {
              DEBUG_DUMP("");
              DEBUG_ECHO("create_object(" << type_id << ")': \n");
              DEBUG_ECHO("Raising ObjectNotCreated() exception.\n");
             throw;
         }

         group_data->repository_id = type_id;
         group_data->object_group_ref = CORBA::Object::_duplicate(object_group.in());
         group_data->object_group_ref_version = object_group_ref_version;
         group_data->lb_policy = lb_policy.c_str();
         m_group_map.insert(std::make_pair(m_object_group_id, group_data));
         
         CORBA::Any factory_id;
         factory_id <<= m_object_group_id;
         factory_creation_id = new CORBA::Any(factory_id);
         return object_group._retn();
         
     }
     
     /**
      * IDL:omg.org/PortableGroup/GenericFactory/delete_object:1.0
      *
      * Deletes the object with the given identifier. If the application
      * invokes this operation on the <code>GenericFactory</code>
      * interface, inherited by the Group Manager, then it deletes
      * an object group. When this operation is invoked on it, the
      * Group Manager must first remove each of the members from
      * the object group, and delete each of them, before it deletes the
      * object group itself.
      *
      * @param factory_creation_id An identifier for the object that is
      *                            to be deleted.
      *
      * @exception ObjectNotFound if the object cannot be found.
      */
     
     void
     LBGroupManager_impl::delete_object(const PortableGroup::GenericFactory::FactoryCreationId &
                                      factory_creation_id)
         throw (PortableGroup::ObjectNotFound,
                CORBA::SystemException)
     {
         
         CDMW_MUTEX_GUARD (m_mutex);
         // Get the object group id.
         PortableGroup::ObjectGroupId object_group_id;

         if (factory_creation_id >>= object_group_id)
         {
             m_group_pos = m_group_map.find(object_group_id);
             if(m_group_pos!=m_group_map.end())
                 m_group_map.erase(m_group_pos);
             else
             {
                DEBUG_DUMP("");
                DEBUG_ECHO("delete_object(factory_creation_id=" << object_group_id << ")': \n");
                DEBUG_ECHO("Raising ObjectNotFound() exception.\n");
                 throw PortableGroup::ObjectNotFound();
             }
         }
     }
    
    /**
     * IDL:omg.org/PortableGroup/ObjectGroupManager/add_member:1.0
     *
     * Allows an application to exercise explicit control over the
     * addition of an existing object to an object group at a particular
     * location.
     *
     * @param object_group The object group reference if the object group
     *                     to which the existing object is to be added.
     * @param the_location The physical location; that is, a fault
     *                     containment region, host, cluster of hosts,
     *                     etc. of the object to be added. There is at
     *                     most one member of an object group at each
     *                     location.
     * @param member       The reference of the object to be added.
     *
     * @return the object group reference for the object group with the
     *         object added. This reference may be the same as that
     *         passed in as a parameter.
     *
     * @exception ObjectGroupNotFound if the object group is not found by
     *         the Group Manager.
     * @exception MemberAlreadyPresent if a member of the object group
     *         already exists at the given location.
     * @exception ObjectNotAdded if the Replication Manager cannot add the
     *         object to the object group.
     * @exception CORBA::BAD_PARAM if member has a nil object reference
     */
    
    PortableGroup::ObjectGroup_ptr
     LBGroupManager_impl::add_member(PortableGroup::ObjectGroup_ptr object_group,
                                     const PortableGroup::Location & the_location,
                                     CORBA::Object_ptr replica)
         throw (PortableGroup::ObjectGroupNotFound,
                PortableGroup::MemberAlreadyPresent,
                PortableGroup::ObjectNotAdded,
                CORBA::SystemException)
     {
         CDMW_MUTEX_GUARD (m_mutex);
         PortableGroup::ObjectGroup_var new_object_group = PortableGroup::ObjectGroup::_nil();
         CdmwLB::ObjectGroupData * group_data = 0;
         PortableGroup::ObjectGroupId object_group_id = get_object_group_id( object_group );
         m_group_pos = m_group_map.find(object_group_id);

         const std::string s_the_location =
             Cdmw::NamingAndRepository::NamingInterface::to_string(the_location);

         if(m_group_pos!=m_group_map.end())
             group_data = m_group_pos->second;
         else
         {
           DEBUG_DUMP("");
           DEBUG_ECHO("add_member(" << s_the_location << ")': Object group Not Found.\n");
           DEBUG_ECHO("Raising ObjectGroupNotFound() exception.\n");
             throw PortableGroup::ObjectGroupNotFound();
         }
         
         CdmwLB::ObjectGroupData group_data_tmp(*group_data);
         
         // type id
         std::string rep_id = std::string(group_data_tmp.repository_id);
         
         CORBA::ULong locations_length = group_data_tmp.members.length();
         
         for (CORBA::ULong i = 0;i <locations_length;++i)
         {
             std::string s_member_loc =
                 Cdmw::NamingAndRepository::NamingInterface::to_string
                 (group_data_tmp.members[i].the_location);
         
             if (s_member_loc == s_the_location)
             {
                DEBUG_DUMP("");
                DEBUG_ECHO("add_member(" << s_the_location << ")': Member is already present.\n");
                DEBUG_ECHO("Raising MemberAlreadyPresent() exception.\n");
                 throw PortableGroup::MemberAlreadyPresent();
             }
         }

         if (!replica->_is_a(rep_id.c_str()))
         {
             DEBUG_DUMP("");
             DEBUG_ECHO("add_member(" << s_the_location << "): Member not of type '");
             DEBUG_ECHO(rep_id << "'!\n");
             DEBUG_ECHO("Raising ObjectNotAdded() exception.\n");
             throw PortableGroup::ObjectNotAdded();
         }
         //Add the member
         group_data_tmp.members.length(locations_length + 1);
         CdmwLB::MemberInfo member_info;
         member_info.the_reference = CORBA::Object::_duplicate(replica);
         member_info.the_location = the_location;
         group_data_tmp.members[locations_length] = member_info;
         try
         {
             // Build an oid using the pattern : group_id/group_ver.
             PortableGroup::ObjectGroupRefVersion group_version = group_data_tmp.object_group_ref_version + 1;
             std::ostringstream oss;
             oss << object_group_id << "/" << group_version;
             PortableServer::ObjectId_var oid =
                 PortableServer::string_to_ObjectId(oss.str().c_str());
             
             CORBA::Object_var group_manager_ref =
                 m_poa->create_reference_with_id(oid.in(),std::string(group_data_tmp.repository_id).c_str());
             
             CdmwLB::Objects_var fallBack = new ::CdmwLB::Objects;
             fallBack->length(1);
             fallBack[0L] = CORBA::Object::_duplicate(group_manager_ref.in());
             
             new_object_group = m_iogr_factory->build_iogr(group_data_tmp.members,
                                                           object_group_id,
                                                           std::string(group_data_tmp.repository_id).c_str(),
                                                           m_lb_domain_id.in(),
                                                           group_data_tmp.lb_policy,
                                                           group_data_tmp.object_group_ref_version,
                                                           fallBack.in());
         } catch (const PortableGroup::ObjectNotCreated & )
         {
             DEBUG_DUMP("");
             DEBUG_ECHO("add_member(" << s_the_location << ")': Could not build new IOGR.\n");
             DEBUG_ECHO("Raising ObjectNotAdded() exception.\n");
             throw PortableGroup::ObjectNotAdded();
         }
         group_data_tmp.object_group_ref = new_object_group;
         *group_data = group_data_tmp;
         return new_object_group._retn();
     }

     /**
     * IDL:omg.org/PortableGroup/ObjectGroupManager/remove_member:1.0
     *
     * Allows an application to exercise explicit control over the
     * removal of a member from an object group at a particular location.
     *
     * @param object_group The object group reference of the object group
     *                     from which the member is to be removed.
     * @param the_location The physical location; that is, a fault
     *                     containment region, host, cluster of hosts,
     *                     etc. of the member to be removed.
     *
     * @return the object group reference for the object group with the
     *         member removed. This reference may be the same as that
     *         passed in as a parameter.
     *
     * @exception ObjectGroupNotFound if the object group is not found by
     *         the Group Manager.
     * @exception MemberNotFound if the Replication Manager cannot find a
     *         member of the object group at the given location.
     */
    
     PortableGroup::ObjectGroup_ptr
     LBGroupManager_impl::remove_member(PortableGroup::ObjectGroup_ptr object_group,
                                        const PortableGroup::Location & the_location)
         throw (PortableGroup::MemberNotFound,
                PortableGroup::ObjectGroupNotFound,
                CORBA::SystemException)
     {
         CDMW_MUTEX_GUARD (m_mutex);
         // Get object group's known data
         // May raise ObjectGroupNotFound.
         PortableGroup::ObjectGroupId object_group_id = get_object_group_id( object_group );
         CdmwLB::ObjectGroupData * group_data = 0;

         using namespace Cdmw::NamingAndRepository;
         
         const std::string s_loc = NamingInterface::to_string(the_location);
         
         m_group_pos = m_group_map.find(object_group_id);
         if(m_group_pos!=m_group_map.end())
             group_data = m_group_pos->second;
         else
         {
             DEBUG_DUMP("");
             DEBUG_ECHO("remove_member(" << s_loc << ")': \n");
             DEBUG_ECHO("Raising ObjectNotFound() exception.\n");
             throw PortableGroup::ObjectGroupNotFound();
         }
         CdmwLB::ObjectGroupData group_data_tmp(*group_data);
         //Remove the member from group_data
         // May raise PortableGroup::MemberNotFound
         try
         {
             remove_member(object_group_id,
                           group_data_tmp,
                           the_location);
         }catch(const ::PortableGroup::MemberNotFound &)
         {
            DEBUG_DUMP("");
            DEBUG_ECHO("remove_member(" << s_loc << ")': \n");
            DEBUG_ECHO("Raising MemberNotFound() exception.\n");
             throw ;
         }
         
         PortableGroup::ObjectGroup_var new_object_group = PortableGroup::ObjectGroup::_duplicate(group_data_tmp.object_group_ref.in());
         *group_data = group_data_tmp;   
         return new_object_group._retn();
     }

    /**
     * IDL:omg.org/PortableGroup/ObjectGroupManager/create_member:1.0
     *
     * Allows the application to exercise explicit control over the
     * creation of a member of an object group, and to determine where
     * the member is created.
     *
     * @param object_group The object group reference for the object
     *                     group to which the member is to be added.
     * @param the_location The physical location; that is, a fault
     *                     containment region, host, cluster of hosts,
     *                     etc. at which the new member is to be
     *                     created. There is at most one member of an
     *                     object group at each location.
     * @param type_id      The repository identifier for the type of the
     *                     object.
     * @param the_criteria Parameters to be passed to the factory,
     *                     which the factory evaluates before creating
     *                     the object.
     *
     * @return the object group reference of the object group with the
     *         member added. This reference may be the same as that
     *         passed in as a parameter.
     *
     * @exception ObjectGroupNotFound if the object group is not found by
     *         the Group Manager.
     * @exception MemberAlreadyPresent if a member of the object group
     *         already exists at the given location.
     * @exception NoFactory if the Group Manager cannot find a
     *         factory that is capable of constructing a member of the
     *         object group with the given <code>type_id</code> and at
     *         the given location.
     * @exception ObjectNotCreated if the factory or the Group
     *         Manager cannnot create the member and add it to the
     *         object group.
     * @exception InvalidCriteria if the factory does not understand the
     *         criteria.
     * @exception CannotMeetCriteria if the factory understands the
     *         criteria but cannot satisfy it.
     */
     PortableGroup::ObjectGroup_ptr
     LBGroupManager_impl::create_member(PortableGroup::ObjectGroup_ptr object_group,
                   const PortableGroup::Location & the_location,
                   const char * type_id,
                   const PortableGroup::Criteria & the_criteria)
         throw(PortableGroup::ObjectGroupNotFound,
              PortableGroup::MemberAlreadyPresent,
               PortableGroup::NoFactory,
               PortableGroup::ObjectNotCreated,
               PortableGroup::InvalidCriteria,
               PortableGroup::CannotMeetCriteria,
               CORBA::SystemException)
     {
         CDMW_MUTEX_GUARD (m_mutex);

         // NOTE: factories are not supported yet.
         throw CORBA::NO_IMPLEMENT(
             ::Cdmw::OrbSupport::NO_IMPLEMENTNotYetImplemented,
             CORBA::COMPLETED_NO);

         return PortableGroup::ObjectGroup::_nil();
     }


     /**
     * IDL:omg.org/PortableGroup/ObjectGroupManager/locations_of_members:1.0
     *
     * Allows the application to determine the locations of the members
     * of the given object group, and the location of the primary member
     * of the group.
     *
     * @param object_group The object group reference of the object
     *                     group.
     *
     * @return a sequence of locations at which the members of the
     *         object group currently exist. If the object group has the
     *         <code>COLD_PASSIVE</code> or <code>WARM_PASSIVE</code>
     *         Group Style, the first location in the sequence is
     *         the location of the primary.
     *
     * @exception ObjectGroupNotFound if the object group is not found by
     *         the Group Manager.
     */
     PortableGroup::Locations *
     LBGroupManager_impl::locations_of_members(PortableGroup::ObjectGroup_ptr object_group)
         throw(PortableGroup::ObjectGroupNotFound,
               CORBA::SystemException)
     {
         CDMW_MUTEX_GUARD (m_mutex);
         PortableGroup::ObjectGroupId object_group_id;
         // Get object group's known data
         // May raise ObjectGroupNotFound.
         // Is the object group known?
         try
         {
             object_group_id = get_object_group_id( object_group );
         }
         catch(const PortableGroup::ObjectGroupNotFound &)
         {
            DEBUG_DUMP("");
            DEBUG_ECHO("locations_of_members(object_group_id=" << object_group_id << ")': \n");
            DEBUG_ECHO("Raising ObjectNotFound() exception.\n");
             throw ;
         }
             
         CdmwLB::ObjectGroupData * group_data = 0;
              
         m_group_pos = m_group_map.find(object_group_id);
         if(m_group_pos!=m_group_map.end())
             group_data = m_group_pos->second;
         
         // Get the location of each member and return the list of locations.
         const CORBA::ULong members_length = group_data->members.length();
         PortableGroup::Locations_var locations = new PortableGroup::Locations(members_length);
         locations->length(members_length);
         for (CORBA::ULong iMembers = 0; iMembers < members_length; ++iMembers)
         {
             locations[iMembers] = group_data->members[iMembers].the_location;
         }

         return locations._retn();
     }

    /**
     * IDL:omg.org/PortableGroup/ObjectGroupManager/get_object_group_id:1.0
     *
     * Takes a reference for an object group as an in parameter, and
     * returns the identifier of the object group.
     *
     * @param object_group The object group reference for the object
     *                     group.
     *
     * @return the object group identifier for the object group.
     *
     * @exception ObjectGroupNotFound if the object group is not found by
     *         the Group Manager.
     */
     PortableGroup::ObjectGroupId
     LBGroupManager_impl::get_object_group_id(PortableGroup::ObjectGroup_ptr object_group)
         throw(PortableGroup::ObjectGroupNotFound,
               CORBA::SystemException)
     {
         CdmwLB::TagLBGroupTaggedComponent* lb_group_tag = 0;
         try
         {
             lb_group_tag = m_iogr_factory->decode_profile_with_group_tag(CORBA::Object::_duplicate(object_group), 0);
         }catch(const CdmwLB::ProfileNotFound&)
         {
             throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
         }
         catch(const CdmwLB::TagNotFound&)
         {
             throw CORBA::INV_OBJREF(OrbSupport::INV_OBJREF, CORBA::COMPLETED_NO);
         }
         PortableGroup::ObjectGroupId object_group_id = lb_group_tag->object_group_id;

         m_group_pos = m_group_map.find(object_group_id);

         if(m_group_pos==m_group_map.end())
         {
             throw PortableGroup::ObjectGroupNotFound();
         }
         
         return object_group_id;
     }
     
     
    /**
     * IDL:omg.org/PortableGroup/ObjectGroupManager/get_object_group_ref:1.0
     *
     * Takes a reference for an object group as an in parameter, and
     * returns the current reference for the object group.
     *
     * @param object_group An object group reference for the object
     *                     group.
     *
     * @return the current object group reference for the object group.
     *         The returned reference may be the same as the reference
     *         passed in as a parameter.
     *
     * @exception ObjectGroupNotFound if the object group is not found by
     *         the Group Manager.
     */
     PortableGroup::ObjectGroup_ptr
     LBGroupManager_impl::get_object_group_ref(PortableGroup::ObjectGroup_ptr object_group)
         throw(PortableGroup::ObjectGroupNotFound,
               CORBA::SystemException)
     {
         CDMW_MUTEX_GUARD (m_mutex);
         PortableGroup::ObjectGroupId object_group_id;
         // Get object group's known data
         // May raise ObjectGroupNotFound.
         // Is the object group known?
         try
         {
             object_group_id = get_object_group_id( object_group );
         }catch(const PortableGroup::ObjectGroupNotFound &)
         {
             DEBUG_DUMP("");
             DEBUG_ECHO("get_object_group_ref(): \n");
             DEBUG_ECHO("Raising ObjectNotFound() exception.\n");
             throw PortableGroup::ObjectGroupNotFound();
         }
         CdmwLB::ObjectGroupData* group_data = 0;
        
         m_group_pos = m_group_map.find(object_group_id);
         if(m_group_pos!=m_group_map.end())
             group_data = m_group_pos->second;
         else
         {
             DEBUG_DUMP("");
             DEBUG_ECHO("get_object_group_ref(object_group_id=" << object_group_id << ")': \n");
             DEBUG_ECHO("Raising ObjectNotFound() exception.\n");
             throw PortableGroup::ObjectGroupNotFound();
         }
         
         PortableGroup::ObjectGroup_var obj = group_data->object_group_ref;
         return obj._retn();
     }

    /**
     * IDL:omg.org/PortableGroup/ObjectGroupManager/get_member_ref:1.0
     *
     * Takes a reference for an object group and a location as in
     * parameters, and returns a reference for the member.
     *
     * @param object_group An object group reference for the object
     *                     group.
     * @param loc          The location of the member.
     *
     * @return the reference for the member.
     *
     * @exception ObjectGroupNotFound if the object group is not found by
     *         the Group Manager.
     * @exception MemberNotFound if the member is not found by the
     *         Group Manager.
     */
    CORBA::Object_ptr
    LBGroupManager_impl::get_member_ref(PortableGroup::ObjectGroup_ptr object_group,
                   const PortableGroup::Location & loc)
        throw(PortableGroup::ObjectGroupNotFound,
              PortableGroup::MemberNotFound,
              CORBA::SystemException)
    {
         CDMW_MUTEX_GUARD (m_mutex);
         // Get object group's known data
         // May raise ObjectGroupNotFound.
         // Is the object group known?
         using namespace Cdmw::NamingAndRepository;
         PortableGroup::ObjectGroupId object_group_id;
         const std::string s_loc = NamingInterface::to_string(loc);

         try
         {
             object_group_id = get_object_group_id( object_group );
         }catch(const PortableGroup::ObjectGroupNotFound &)
         {
             DEBUG_DUMP("");
             DEBUG_ECHO("get_member_ref(" << s_loc << ")': \n");
             DEBUG_ECHO("Raising ObjectNotFound() exception.\n");
             throw PortableGroup::ObjectGroupNotFound();
         }
         CdmwLB::ObjectGroupData* group_data = 0;
         
         m_group_pos = m_group_map.find(object_group_id);
         if(m_group_pos!=m_group_map.end())
             group_data = m_group_pos->second;
         else
         {
             DEBUG_DUMP("");
             DEBUG_ECHO("get_member_ref(" << s_loc << ")': \n");
             DEBUG_ECHO("Raising ObjectNotFound() exception.\n");
             throw PortableGroup::ObjectGroupNotFound();
             
         }
         // Find the member at the given location.
         CORBA::Object_var member = CORBA::Object::_nil();
         CORBA::ULong i = 0;
         const CORBA::ULong members_length = group_data->members.length();
         bool member_found = false;
         
         while (!member_found && (i < members_length))
         {
             std::string s_member_loc =
                 NamingInterface::to_string(group_data->members[i].the_location);
             if (s_member_loc == s_loc)
             {
                 member_found = true;
                 member = group_data->members[i].the_reference;
             }
             ++i;
         }
         if (!member_found)
         {
             throw PortableGroup::MemberNotFound();
         }

         return member._retn();
    }

/**
 * IDL:omg.org/PortableGroup/ObjectGroupManager/get_object_group_ref_from_id:1.0
 * 
 *
 * Returns the object group reference from the object group id.
 *
 * @param object_group_id the object group id to use for the search.
 *
 * @return the object group reference.
 *
 * @exception ObjectGroupNotFound if the object group is not found with the
 *         given object group id.
 */
    PortableGroup::ObjectGroup_ptr
    LBGroupManager_impl::get_object_group_ref_from_gid(
        PortableGroup::ObjectGroupId  object_group_id)
        throw(PortableGroup::ObjectGroupNotFound,
              CORBA::SystemException)
    {
        CDMW_MUTEX_GUARD (m_mutex);
        
         // Get object group's known data
         // May raise ObjectGroupNotFound.
         // Is the object group known?
        CdmwLB::ObjectGroupData* group_data = 0;
        
        m_group_pos = m_group_map.find(object_group_id);
        if(m_group_pos!=m_group_map.end())
            group_data = m_group_pos->second;
        else
        {
            DEBUG_DUMP("");
            DEBUG_ECHO("get_object_group_ref_from_gid(object_group_id=" << object_group_id << ")': \n");
            DEBUG_ECHO("Raising ObjectNotFound() exception.\n");
            throw PortableGroup::ObjectGroupNotFound();
            
        }
        PortableGroup::ObjectGroup_var obj = group_data->object_group_ref;
        return obj._retn();       
    }
    
    PortableGroup::ObjectGroups* LBGroupManager_impl::groups_at_location (const PortableGroup::Location& the_location)
        throw (CORBA::SystemException)
    {
        return 0;
     }


        /**
         * IDL:CdmwLB/
         *      LBGroupManager/get_object_group_version_from_ref:1.0
         *
         * Returns the object group version from the object group reference.
         *
         * @param object_group the object group reference.
         *
         * @return the object group reference version.
         *
         * @exception ObjectGroupNotFound if the object group is not found with
         *         the given reference.
         */
        ::PortableGroup::ObjectGroupRefVersion
        LBGroupManager_impl::get_object_group_version_from_ref(::PortableGroup::ObjectGroup_ptr object_group)
            throw(::PortableGroup::ObjectGroupNotFound,
                  CORBA::SystemException)
        {
            CDMW_MUTEX_GUARD (m_mutex);
            CdmwLB::ObjectGroupData * group_data = 0;
            PortableGroup::ObjectGroupId object_group_id = get_object_group_id( object_group );
            m_group_pos = m_group_map.find(object_group_id);
            if(m_group_pos!=m_group_map.end())
                group_data = m_group_pos->second;
            else
            {
                DEBUG_DUMP("");
                DEBUG_ECHO("get_object_group_version_from_ref(" << object_group_id << ")': Object group Not Found.\n");
                DEBUG_ECHO("Raising ObjectGroupNotFound() exception.\n");
                throw PortableGroup::ObjectGroupNotFound();
            }
            PortableGroup::ObjectGroupRefVersion group_version = group_data->object_group_ref_version;
            return group_version;
            
        }

        /**
         * IDL:CdmwLB/
         *      LBGroupManager/get_object_group_version_from_gid:1.0
         *
         * Returns the object group version from the object group id.
         *
         * @param object_group_id the object group id.
         *
         * @return the object group reference version.
         *
         * @exception ObjectGroupNotFound if the object group is not found with
         *         the given object group id.
         */
        ::PortableGroup::ObjectGroupRefVersion
        LBGroupManager_impl::get_object_group_version_from_gid(::PortableGroup::ObjectGroupId object_group_id)
            throw(::PortableGroup::ObjectGroupNotFound,
                  CORBA::SystemException)
        {
            CDMW_MUTEX_GUARD (m_mutex);
            CdmwLB::ObjectGroupData * group_data = 0;
            m_group_pos = m_group_map.find(object_group_id);
            if(m_group_pos!=m_group_map.end())
                group_data = m_group_pos->second;
            else
            {
                DEBUG_DUMP("");
                DEBUG_ECHO("get_object_group_version_from_gid(" << object_group_id << ")': Object group Not Found.\n");
                DEBUG_ECHO("Raising ObjectGroupNotFound() exception.\n");
                throw PortableGroup::ObjectGroupNotFound();
            }
            PortableGroup::ObjectGroupRefVersion group_version = group_data->object_group_ref_version;
            return group_version;

        }


    
    /**
     * This operation removes a member from group_data and updates
     * it with a new object group reference and members list.
     *
     * @param object_group_id  The object group id of the object group
     *                     from which the member is to be removed.
     * @param group_data   An ObjectGroupDataObject with all known info
     *                     on the object group.
     * @param the_location The physical location; that is, a fault
     *                     containment region, host, cluster of hosts,
     *                     etc. of the member to be removed.
     *
     * @exception MemberNotFound if the Group Manager cannot find a
     *         member of the object group at the given location.
     */

     
    void
    LBGroupManager_impl::remove_member(PortableGroup::ObjectGroupId          object_group_id,
                                       CdmwLB::ObjectGroupData &             group_data_tmp,
                                       const PortableGroup::Location &       the_location)                
        throw(PortableGroup::MemberNotFound,
              CORBA::SystemException)
    {
        // Working on a copy to insure some exception safety!
        // I) Is there a member on that location?
        const CORBA::ULong members_length = group_data_tmp.members.length();
        CdmwLB::MemberInfos new_members;
        CORBA::ULong idx = 0L;
        bool location_found=false;
        CORBA::ULong l1_len = the_location.length();
        if (l1_len == 0)
            throw PortableGroup::MemberNotFound();
            
        for (CORBA::ULong i = 0L;i <members_length;++i)
        {
        
            ::PortableGroup::Location member_location = group_data_tmp.members[i].the_location;
            if (Compare_Locations(the_location, member_location, l1_len))
            {
                location_found=true;
            } else
            {

                new_members.length(idx + 1);
                new_members[idx++] = group_data_tmp.members[i];
            }
        }
        if (!location_found) 
            throw PortableGroup::MemberNotFound();

            //Build a new IOGR
            // may throw PortableGroup::ObjectNotCreated
            // Build an oid using the pattern : group_id/group_ver.
            PortableGroup::ObjectGroupRefVersion group_version = group_data_tmp.object_group_ref_version + 1;
            std::ostringstream oss;
            oss << object_group_id << "/" << group_version;
            PortableServer::ObjectId_var oid =
                PortableServer::string_to_ObjectId(oss.str().c_str());

            CORBA::Object_var group_manager_ref =
                m_poa->create_reference_with_id(oid.in(),std::string(group_data_tmp.repository_id).c_str());
            CdmwLB::Objects_var fallBack = new CdmwLB::Objects;
            fallBack->length(1);
            fallBack[0L] = CORBA::Object::_duplicate(group_manager_ref.in());

            group_data_tmp.members = new_members;
            group_data_tmp.object_group_ref =
                m_iogr_factory->build_iogr(group_data_tmp.members,
                                           object_group_id,
                                           std::string(group_data_tmp.repository_id).c_str(),
                                           m_lb_domain_id.in(),
                                           group_data_tmp.lb_policy,
                                           group_data_tmp.object_group_ref_version,
                                           fallBack.in());
            
    }   

    void
    LBGroupManager_impl::notify( CdmwPlatformMngt::Event* an_event )
        throw( CORBA::SystemException )
    {
        
        CORBA::String_var message = an_event->to_string();
        std::cout << message.in() << std::endl;
        
        ::PortableGroup::Location location;
        
        switch( an_event->event_kind() )
        {
        case CdmwPlatformMngt::HOST_STATUS_CHANGE :
        {
            CdmwPlatformMngt::HostStatusChange* event
                = CdmwPlatformMngt::HostStatusChange::_downcast(
                    an_event );
            
            if( event->host_status() == CdmwPlatformMngt::HOST_NOT_RESPONDING )
            {
                location.length( 1 );
                location[0].kind = CORBA::string_dup( "hostname" );
                location[0].id = CORBA::string_dup( event->host_name() );
                const std::string hostname(location[0].id);
                
                handle_host_failure(hostname, (location));
            }
            else
                return;
        }
        break;
        
        case CdmwPlatformMngt::APPLICATION_AGENT_DEATH :
        {
            CdmwPlatformMngt::ApplicationAgentDeath* event
                = CdmwPlatformMngt::ApplicationAgentDeath::_downcast(
                    an_event );
            location.length( 2 );
            location[0].kind = CORBA::string_dup( "hostname" );
            location[0].id = CORBA::string_dup( event->host_name() );
            location[1].kind = CORBA::string_dup( "applicationname" );
            location[1].id = CORBA::string_dup(  event->application_name() );

            const std::string applicationname(location[1].id);
            const std::string hostname(location[0].id);
                
            handle_application_failure(hostname,applicationname, location);
        }
        break;
        
        case CdmwPlatformMngt::PROCESS_STATUS_CHANGE :
        {
            CdmwPlatformMngt::ProcessStatusChange* event
                = CdmwPlatformMngt::ProcessStatusChange::_downcast(
                    an_event );
            
            switch( event->process_status() )
            {
            case CdmwPlatformMngt::PROCESS_STOPPED:
            case CdmwPlatformMngt::PROCESS_ENDED:
            case CdmwPlatformMngt::PROCESS_FAILED_ERROR:
            case CdmwPlatformMngt::PROCESS_FAILED_TIMEOUT:
            case CdmwPlatformMngt::PROCESS_FAILED_DISCREPANCY:
            case CdmwPlatformMngt::PROCESS_FAILED_INVALID:
            case CdmwPlatformMngt::PROCESS_FAILED_NO_RESPONSE:
            case CdmwPlatformMngt::PROCESS_FAILED_DEATH:
            {
                location.length( 3 );
                location[0].kind = CORBA::string_dup( "hostname" );
                location[0].id = CORBA::string_dup( event->host_name() );
                location[1].kind = CORBA::string_dup( "applicationname" );
                location[1].id = CORBA::string_dup(  event->process_id().application_name );
                location[2].kind = CORBA::string_dup( "processname" );
                location[2].id = CORBA::string_dup( event->process_id().process_name );
                const std::string processname(location[2].id);
                const std::string applicationname(location[1].id);
                const std::string hostname(location[0].id);
                handle_process_failure(hostname,applicationname,processname,location);
            }
                break;
            default:
                return;
            }
        }
        break;
        
        default:
            return;
        }
    }


    void 
    LBGroupManager_impl::handle_process_failure(const std::string &    hostname,
                                                const std::string &    applicationname,
                                                const std::string &    processname,
                                                const ::PortableGroup::Location & the_location)
        throw (CORBA::SystemException)
    {
        GroupMap::iterator resultObject = m_group_map.begin();

        while(resultObject != m_group_map.end())
        {
            PortableGroup::ObjectGroupId object_group_id = resultObject->first;
            CdmwLB::ObjectGroupData* resultOGD = resultObject->second;
            CdmwLB::ObjectGroupData group_data_tmp(*resultOGD);
            
            //Remove the member from group_data
            // May raise PortableGroup::MemberNotFound
            try
            {
                remove_member(object_group_id,
                              group_data_tmp,
                              the_location);

 
            *resultOGD = group_data_tmp;
            
            } catch (const ::PortableGroup::ObjectGroupNotFound &) {
            // TODO: Add trace information
                DEBUG_DUMP("");
                DEBUG_ECHO("handle_process_failure("
                           << hostname << "," << applicationname << "," << processname
                           << "): Caught an ObjectGroupNotFound exception.\n"
                           << "Raising CORBA::INTERNAL() exception." << std::endl);
                throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL,
                                      CORBA::COMPLETED_NO);
        } 
            catch (const ::PortableGroup::MemberNotFound &) {
                // TODO: Add trace information
                DEBUG_DUMP("");
                DEBUG_ECHO("handle_process_failure("
                           << hostname << "," << applicationname << "," << processname
                           << "): Caught a MemberNotFound exception.\n"
                           << "Raising CORBA::INTERNAL() exception." << std::endl);
                throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL,
                                      CORBA::COMPLETED_NO);
            } catch (const CORBA::INTERNAL &) {
                // TODO: Add trace information
                throw;
            } catch (const CORBA::NO_MEMORY &) {
                // TODO: Add trace information
                throw;
            } catch (const CORBA::SystemException & ex) {
                // TODO: Add trace information
                DEBUG_DUMP("");
                DEBUG_ECHO("handle_process_failure("
                           << hostname << "," << applicationname << "," << processname
                           << "): Caught a system exception.\n"
                           << "Exception:" << ex << std::endl);
                // TODO: Trigger a reconfiguration? (check that member is non faulty?)
            }
            
            resultObject++;
        }
    }
    
    void 
    LBGroupManager_impl::handle_application_failure(const std::string & hostname,
                                                        const std::string & applicationname,
                                                        const ::PortableGroup::Location & the_location)
        throw (CORBA::SystemException)
    {
        GroupMap::iterator resultObject = m_group_map.begin();

        while(resultObject != m_group_map.end())
        {
            CdmwLB::ObjectGroupData * resultOGD = resultObject->second;
            // Found a group!
            try {
                // Remove the member from ogd
                // May raise ::Portable::MemberNotFound
                remove_member((resultOGD->object_group_ref).in(), the_location);
            } catch (const ::PortableGroup::ObjectGroupNotFound &) {
            // TODO: Add trace information
                DEBUG_DUMP("");
                DEBUG_ECHO("handle_application_failure("
                           << hostname << "," << applicationname 
                           << "): Caught an ObjectGroupNotFound exception.\n"
                           << "Raising CORBA::INTERNAL() exception." << std::endl);
                throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL,
                                      CORBA::COMPLETED_NO);
            } catch (const ::PortableGroup::MemberNotFound &) {
            } catch (const CORBA::INTERNAL &) {
                // TODO: Add trace information
                throw;
            } catch (const CORBA::NO_MEMORY &) {
                // TODO: Add trace information
                throw;
            } catch (const CORBA::SystemException & ex) {
                // TODO: Add trace information
                DEBUG_DUMP("");
                DEBUG_ECHO("handle_application_failure("
                           << hostname << "," << applicationname 
                           << "): Caught a system exception.\n"
                           << "Exception:" << ex << std::endl);
                // TODO: Trigger a reconfiguration? (check that member is non faulty?)
            }
            resultObject++;
        }


    }


    void 
    LBGroupManager_impl::handle_host_failure(const std::string & hostname,
                                                 const ::PortableGroup::Location & the_location)

        throw (CORBA::SystemException)
    {
        GroupMap::iterator resultObject = m_group_map.begin();

        while(resultObject != m_group_map.end())
        {
            CdmwLB::ObjectGroupData * resultOGD = resultObject->second;
            // Found a group!
            try {
                // Remove the member from ogd
                // May raise ::Portable::MemberNotFound
                remove_member((resultOGD->object_group_ref).in(), the_location);
            } catch (const ::PortableGroup::ObjectGroupNotFound &) {
            // TODO: Add trace information
                DEBUG_DUMP("");
                DEBUG_ECHO("handle_host_failure("
                           << hostname 
                           << "): Caught an ObjectGroupNotFound exception.\n"
                           << "Raising CORBA::INTERNAL() exception." << std::endl);
                throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL,
                                      CORBA::COMPLETED_NO);
            } catch (const ::PortableGroup::MemberNotFound &) {
                // TODO: Add trace information
            } catch (const CORBA::INTERNAL &) {
                // TODO: Add trace information
                throw;
            } catch (const CORBA::NO_MEMORY &) {
                // TODO: Add trace information
                throw;
            } catch (const CORBA::SystemException & ex) {
                // TODO: Add trace information
                DEBUG_DUMP("");
                DEBUG_ECHO("handle_host_failure("
                           << hostname  
                           << "): Caught a system exception.\n"
                           << "Exception:" << ex << std::endl);
                // TODO: Trigger a reconfiguration? (check that member is non faulty?)
            }
            resultObject++;
        }
    }
}//close namespace LB

}// close nampespace Cdmw

