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


#ifndef _CDMW_FT_REPLICATION_MANAGER_REPLICATION_MANAGER_IMPL_HPP_
#define _CDMW_FT_REPLICATION_MANAGER_REPLICATION_MANAGER_IMPL_HPP_

#include "ftreplicationmanager/ReplicationManagerDelegate.hpp"
#include "ftreplicationmanager/ObjectGroup.hpp"
#include "ftreplicationmanager/Object.hpp"
#include "ftreplicationmanager/Location.hpp"
#include "ftreplicationmanager/ThreadSpecificData.hpp"

#include "Foundation/osthreads/Mutex.hpp"
#include <orbsvcs/FaultTolerance/FT_ClientService_Activate.h>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <FaultTolerance/ftcommon/IOGRFactory.hpp>
#include "FaultTolerance/ftstatemanager/TXDataStore.hpp"
#include "Foundation/commonsvcs/datastore/StorageHome.hpp"
#include "Foundation/commonsvcs/datastore/StorageObject.hpp"
#include <map>
#include <vector>
#include <FaultTolerance/ftcommon/FTManagerDSID.hpp>


#include "CdmwFTReplicationManagerPushConsumer.skel.hpp"
#include "CdmwFTObjectGroupData.stub.hpp"
//#include "CdmwFTMembersAdminInfo.stub.hpp"
#include "CdmwFTLocationInfo.stub.hpp"


DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::FT::Properties);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::CdmwFT::ObjectGroupData);
DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::CdmwFT::LocationInfo);


namespace Cdmw {
namespace FT {
namespace ReplicationManager {

    /**
     * Actual CdmwFT::ReplicationManager implementation class.
     */
    class ReplicationManager_impl
        : public virtual POA_CdmwFT::ReplicationManagerPushConsumer,
          public virtual Cdmw::OrbSupport::RefCountServantBase,
          public virtual ReplicationManagerDelegate

    {

    public:
        /**
         * Creates a replication manager.
         *
         * @param ft_domain_id The fault tolerance domain id.
         */
        ReplicationManager_impl(int argc,
                                char* argv[],
                                CORBA::ORB_ptr orb, 
                                PortableServer::POA_ptr parent,
                                const ::FT::FTDomainId & ft_domain_id,
                                const char* security_level,
                                const char* replication_level,
                                std::ostream & os)
            throw(CORBA::SystemException);
        
        /**
         * Destroys a replication manager.
         */
        ~ReplicationManager_impl(void) throw ();

        virtual PortableServer::POA_ptr _default_POA() {
            return PortableServer::POA::_duplicate(m_parent_poa.in());
        }

        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/get_object_group_ref_from_gid:1.0
         *
         * Returns the object group reference from the object group id.
         *
         * @param object_group_id the object group id to use for the search.
         *
         * @return the object group reference.
         *
         * @exception ObjectGroupNotFound if the object group is not found with
         *         the given object group id.
         */
        virtual ::FT::ObjectGroup_ptr
        get_object_group_ref_from_gid(::FT::ObjectGroupId object_group_id)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);
        
        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/get_object_group_ref_from_name:1.0
         *
         * Returns the object group reference from its group name.
         *
         * @param group_name the name which was specified at group creation.
         *
         * @return the object group reference.
         *
         * @exception ObjectGroupNotFound if the object group is not found with
         *         the given object group name.
         */
        virtual ::FT::ObjectGroup_ptr
        get_object_group_ref_from_name(const CdmwFT::GroupName& group_name)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);
        
        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/get_object_group_version_from_ref:1.0
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
        virtual ::FT::ObjectGroupRefVersion
        get_object_group_version_from_ref(::FT::ObjectGroup_ptr object_group)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);

        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/get_object_group_version_from_gid:1.0
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
        virtual ::FT::ObjectGroupRefVersion
        get_object_group_version_from_gid(::FT::ObjectGroupId object_group_id)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);
        
        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/get_ft_domain_id:1.0
         *
         * Returns the fault tolerance domain id in which this Replication
         * Manager is running.
         *
         * @return the fault tolerance domain.
         */
        virtual char *
        get_ft_domain_id(void)
            throw(CORBA::SystemException) 
        {
            return CORBA::string_dup(m_ft_domain_id.in());
        }
        
        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/get_object_group_info:1.0
         *
         * Returns information about the object group with the given object
         * group reference.
         *
         * @param object_group the object group reference.
         *
         * @return a group info.
         *
         * @exception ObjectGroupNotFound if the object group is not found with
         *         the given reference.
         */
        virtual ::CdmwFT::GroupInfo *
        get_object_group_info(::FT::ObjectGroup_ptr object_group)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);
        
        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/get_object_group_info_from_gid:1.0
         *
         * Returns information about the object group with the given object
         * group id.
         *
         * @param object_group_id the object group id.
         *
         * @return a group info.
         *
         * @exception ObjectGroupNotFound if the object group is not found with
         *         the given object group id.
         */
        virtual ::CdmwFT::GroupInfo *
        get_object_group_info_from_gid(::FT::ObjectGroupId object_group_id)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);
        
        /**
         *IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/update_fallback:1.0
         *
         * Change the fallback reference on all the IOGR
         */
         
        virtual void update_fallback(const ::FT::Location & the_location)
            throw(CORBA::SystemException);

        /**
         * IDL:omg.org/FT/ReplicationManager/register_fault_notifier:1.0
         *
         * Registers the Fault Notifier with the Replication Manager.
         *
         * @param fault_notifier The reference of the Fault Notifier that
         *                       is to be registered.
         */
        virtual void
        register_fault_notifier(::FT::FaultNotifier_ptr fault_notifier)
            throw(CORBA::SystemException) 
        {
            m_fault_notifier =
                ::FT::FaultNotifier::_duplicate(fault_notifier);
        }
        
        /**
         * IDL:omg.org/FT/ReplicationManager/get_fault_notifier:1.0
         *
         * Returns the reference of the Fault Notifier.
         *
         * @return the reference of the Fault Notifier.
         *
         * @exception InterfaceNotFound if the Fault Notifier is not found.
         */
        virtual ::FT::FaultNotifier_ptr
        get_fault_notifier(void)
            throw(::FT::InterfaceNotFound,
                  CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/PropertyManager/set_default_properties:1.0
         *
         * Sets the default properties for all object groups that are to be
         * created within the fault tolerance domain.
         *
         * @param props The properties to be set for all newly created
         *              object groups within the fault tolerance domain.
         *
         * @exception InvalidProperty if one or more of the properties in the
         *         sequence is not valid.
         * @exception UnsupportedProperty if one or more of the properties in
         *         the sequence is not supported.
         */
        virtual void
        set_default_properties(const ::FT::Properties & props)
            throw(::FT::InvalidProperty,
                  ::FT::UnsupportedProperty,
                  CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/PropertyManager/get_default_properties:1.0
         *
         * Returns the default properties for the object groups within the
         * fault tolerance domain.
         *
         * @return the default properties that have been set for the object
         *         groups.
         */
        virtual ::FT::Properties *
        get_default_properties(void)
            throw(CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/PropertyManager/remove_default_properties:1.0
         *
         * Removes the given default properties.
         *
         * @param props The properties to be removed.
         *
         * @exception InvalidProperty if one or more of the properties in the
         *         sequence is not valid.
         * @exception UnsupportedProperty if one or more of the properties in
         *         the sequence is not supported.
         */
        virtual void
        remove_default_properties(const ::FT::Properties & props)
            throw(::FT::InvalidProperty,
                  ::FT::UnsupportedProperty,
                  CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/PropertyManager/set_type_properties:1.0
         *
         * Sets the properties that override the default properties of the
         * object groups, with the given type identifier, that are created
         * in the future.
         *
         * @param type_id   The repository id for which the properties, that
         *                  are to override the existing properties, are set.
         * @param overrides The overriding properties.
         *
         * @exception InvalidProperty if one or more of the properties in the
         *         sequence is not valid.
         * @exception UnsupportedProperty if one or more of the properties in
         *         the sequence is not supported.
         */
        virtual void
        set_type_properties(const char * type_id,
                            const ::FT::Properties & overrides)
            throw(::FT::InvalidProperty,
                  ::FT::UnsupportedProperty,
                  CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/PropertyManager/get_type_properties:1.0
         *
         * Returns the properties of the object groups, with the given type
         * identifier, that are created in the future. These properties
         * include the properties determined by <code>set_type_properties()
         * </code>, as well as the default properties that are not overridden
         * by <code>set_type_properties</code>.
         *
         * @param type_id The repository id for which the properties, that
         *                are to override the existing properties, are set.
         *
         * @return the effective properties for the given type identifier.
         */
        virtual ::FT::Properties *
        get_type_properties(const char * type_id)
            throw(CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/PropertyManager/remove_type_properties:1.0
         *
         * Removes the given properties, with the given type identifier.
         *
         * @param type_id The repository id for which the given properties
         *                are to be removed.
         * @param props   The properties to be removed.
         *
         * @exception InvalidProperty if one or more of the properties in the
         *         sequence is not valid.
         * @exception UnsupportedProperty if one or more of the properties in
         *         the sequence is not supported.
         */
        virtual void
        remove_type_properties(const char * type_id,
                               const ::FT::Properties & props)
            throw(::FT::InvalidProperty,
                  ::FT::UnsupportedProperty,
                  CORBA::SystemException);

        /**
         * IDL:omg.org/FT/PropertyManager/set_properties_dynamically:1.0
         *
         * Sets the properties for the object group with the given reference
         * dynamically while the application executes. The properties given
         * as a parameter override the properties for the object when it was
         * created which, in turn, override the properties for the given type
         * which, in turn, override the default properties.
         *
         * @param object_group The reference of the object group for which
         *                     the overriding properties are set.
         * @param overrides    The overriding properties.
         *
         * @exception ObjectGroupNotFound if the object group is not found with
         *         the given reference.
         * @exception InvalidProperty if one or more of the properties in the
         *         sequence is not valid.
         * @exception UnsupportedProperty if one or more of the properties in
         *         the sequence is not supported.
         */
        virtual void
        set_properties_dynamically(::FT::ObjectGroup_ptr object_group,
                                   const ::FT::Properties & overrides)
            throw(::FT::ObjectGroupNotFound,
                  ::FT::InvalidProperty,
                  ::FT::UnsupportedProperty,
                  CORBA::SystemException);

        /**
         * IDL:omg.org/FT/PropertyManager/get_properties:1.0
         *
         * Returns the current properties of the given object group. These
         * properties include those that are set dynamically, those that are
         * set when the object group was created but are not overridden by
         * <code>set_properties_dynamically()</code>, those that are set as
         * properties of a type but are not overridden by <code>
         * create_object()</code> and <code>set_properties_dyamically()
         * </code>, and those that are set as defaults but are not overridden
         * by <code>set_type_properties()</code>, <code>create_object()
         * </code>, and <code>set_properties_dyamically()</code>.
         *
         * @param object_group The reference of the object group for which
         *                     the properties are to be returned.
         *
         * @return the set of current properties for the object group with
         *         the given reference.
         *
         * @exception ObjectGroupNotFound if the object group is not found by
         *         the Replication Manager.
         */
        virtual ::FT::Properties *
        get_properties(::FT::ObjectGroup_ptr object_group)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/ObjectGroupManager/create_member:1.0
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
         *         the Replication Manager.
         * @exception MemberAlreadyPresent if a member of the object group
         *         already exists at the given location.
         * @exception NoFactory if the Replication Manager cannot find a
         *         factory that is capable of constructing a member of the
         *         object group with the given <code>type_id</code> and at
         *         the given location.
         * @exception ObjectNotCreated if the factory or the Replication
         *         Manager cannnot create the member and add it to the
         *         object group.
         * @exception InvalidCriteria if the factory does not understand the
         *         criteria.
         * @exception CannotMeetCriteria if the factory understands the
         *         criteria but cannot satisfy it.
         */
        virtual ::FT::ObjectGroup_ptr
        create_member(::FT::ObjectGroup_ptr object_group,
                      const ::FT::Location & the_location,
                      const char * type_id,
                      const ::FT::Criteria & the_criteria)
            throw(::FT::ObjectGroupNotFound,
                  ::FT::MemberAlreadyPresent,
                  ::FT::NoFactory,
                  ::FT::ObjectNotCreated,
                  ::FT::InvalidCriteria,
                  ::FT::CannotMeetCriteria,
                  CORBA::SystemException);

        /**
         * IDL:omg.org/FT/ObjectGroupManager/add_member:1.0
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
         *         the Replication Manager.
         * @exception MemberAlreadyPresent if a member of the object group
         *         already exists at the given location.
         * @exception ObjectNotAdded if the Replication Manager cannot add the
         *         object to the object group.
         * @exception CORBA::BAD_PARAM if member has a nil object reference
         */
        virtual ::FT::ObjectGroup_ptr
        add_member(::FT::ObjectGroup_ptr object_group,
                   const ::FT::Location & the_location,
                   CORBA::Object_ptr member)
            throw(::FT::ObjectGroupNotFound,
                  ::FT::MemberAlreadyPresent,
                  ::FT::ObjectNotAdded,
                  CORBA::SystemException);

        /**
         * IDL:omg.org/FT/ObjectGroupManager/remove_member:1.0
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
         *         the Replication Manager.
         * @exception MemberNotFound if the Replication Manager cannot find a
         *         member of the object group at the given location.
         */
        virtual ::FT::ObjectGroup_ptr
        remove_member(::FT::ObjectGroup_ptr object_group,
                      const ::FT::Location & the_location)
            throw(::FT::ObjectGroupNotFound,
                  ::FT::MemberNotFound,
                  CORBA::SystemException);

        /**
         * IDL:omg.org/FT/ObjectGroupManager/set_primary_member:1.0
         *
         * Allows the application to exercise explicit control over the
         * selection of the member of the object group that is to be the
         * primary.
         *
         * @param object_group The object group reference of the object
         *                     group whose primary is to be determined.
         * @param the_location The physical location of the member that is
         *                     to become the primary.
         *
         * @return the object group reference of the object group with the
         *         primary member at the given location. This reference may
         *         be the same as that passed in as a parameter.
         *
         * @exception ObjectGroupNotFound if the object group is not found by
         *         the Replication Manager.
         * @exception MemberNotFound if the Replication Manager cannot find a
         *         member of the object group at that location.
         * @exception PrimaryNotSet if the Replication Manager cannot set the
         *         primary member of the object group.
         * @exception BadReplicationStyle if the <code>ReplicationStyle</code>
         *         of the given group is not <code>COLD_PASSIVE</code> or
         *         <code>WARM_PASSIVE</code>.
         */
        virtual ::FT::ObjectGroup_ptr
        set_primary_member(::FT::ObjectGroup_ptr object_group,
                           const ::FT::Location & the_location)
            throw(::FT::ObjectGroupNotFound,
                  ::FT::MemberNotFound,
                  ::FT::PrimaryNotSet,
                  ::FT::BadReplicationStyle,
                  CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/ObjectGroupManager/locations_of_members:1.0
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
         *         Replication Style, the first location in the sequence is
         *         the location of the primary.
         *
         * @exception ObjectGroupNotFound if the object group is not found by
         *         the Replication Manager.
         */
        virtual ::FT::Locations *
        locations_of_members(::FT::ObjectGroup_ptr object_group)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);

        /**
         * IDL:omg.org/FT/ObjectGroupManager/get_object_group_id:1.0
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
         *         the Replication Manager.
         */
        virtual ::FT::ObjectGroupId
        get_object_group_id(::FT::ObjectGroup_ptr object_group)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/ObjectGroupManager/get_object_group_ref:1.0
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
         *         the Replication Manager.
         */
        virtual ::FT::ObjectGroup_ptr
        get_object_group_ref(::FT::ObjectGroup_ptr object_group)
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);

        /**
         * IDL:omg.org/FT/ObjectGroupManager/get_member_ref:1.0
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
         *         the Replication Manager.
         * @exception MemberNotFound if the member is not found by the
         *         Replication Manager.
         */
        virtual CORBA::Object_ptr
        get_member_ref(::FT::ObjectGroup_ptr object_group,
                       const ::FT::Location & loc)
            throw(::FT::ObjectGroupNotFound,
                  ::FT::MemberNotFound,
                  CORBA::SystemException);
        
        /**
         * IDL:omg.org/FT/GenericFactory/create_object:1.0
         *
         * Creates an object, using the <code>type_id</code> parameter to
         * determine which type of object to create and <code>the_criteria
         * </code> parameter to determine restrictions on how and where to
         * create the object. The out parameter, <code>factory_creation_id
         * </code>, allows the entity that invoked the factory, and the
         * factory itself, to identify the object for subsequent deletion.
         * If the application invokes the <code>create_object()</code>
         * operation on the <code>GenericFactory</code> interface, inherited
         * by the Replication Manager, then it creates an object group. For
         * an object group with the <code>MEMB_APP_CTRL MembershipStyle
         * </code>, the Replication Manager returns an object group reference
         * containing only the <code>TAG_MULTIPLE_COMPONENTS</code> profile
         * with the <code>TAG_FT_GROUP</code> component in it.
         *
         * @param type_id             The repository identifier of the
         *                            object to be created by the factory.
         * @param the_criteria        Information passed to the factory,
         *                            which the factory evaluates before
         *                            creating the object.
         * @param factory_creation_id An identifier that allows the factory
         *                            to delete the object subsequently.
         *
         * @return the reference to the object created by the <code>
         *         GenericFactory</code>. When the <code>GenericFactory
         *         </code> interface is inherited by the Replication Manager,
         *         the <code>create_object()</code> operation returns an
         *         object group reference as a result.
         *
         * @exception NoFactory if the object cannot be created. When the
         *         <code>GenericFactory</code> interface is inherited by the
         *         Replication Manager, the raised exception indicates that
         *         the Replication Manager cannot create the object group
         *         because it cannot find a factory that is capable of
         *         constructing a member of the object group of the <code>
         *         type_id</code> at the location.
         * @exception ObjectNotCreated if the factory cannot create the object.
         * @exception InvalidCriteria if the factory does not understand the
         *         criteria.
         * @exception InvalidProperty if a property passed in as criteria is
         *         invalid.
         * @exception CannotMeetCriteria if the factory understands the
         *         criteria but cannot satisfy it.
         */
        virtual CORBA::Object_ptr
        create_object(const char * type_id,
                      const ::FT::Criteria & the_criteria,
                      ::FT::GenericFactory::FactoryCreationId_out
                      factory_creation_id)
            throw(::FT::NoFactory,
                  ::FT::ObjectNotCreated,
                  ::FT::InvalidCriteria,
                  ::FT::InvalidProperty,
                  ::FT::CannotMeetCriteria,
                  CORBA::SystemException);

        /**
         * IDL:omg.org/FT/GenericFactory/delete_object:1.0
         *
         * Deletes the object with the given identifier. If the application
         * invokes this operation on the <code>GenericFactory</code>
         * interface, inherited by the Replication Manager, then it deletes
         * an object group. When this operation is invoked on it, the
         * Replication Manager must first remove each of the members from
         * the object group, and delete each of them, before it deletes the
         * object group itself.
         *
         * @param factory_creation_id An identifier for the object that is
         *                            to be deleted.
         *
         * @exception ObjectNotFound if the object cannot be found.
         */
        virtual void
        delete_object(const ::FT::GenericFactory::FactoryCreationId & 
                      factory_creation_id)
            throw(::FT::ObjectNotFound,
                  CORBA::SystemException);

        /**
         * IDL:thalesgroup.com/CdmwFT/ReplicationManager/register_fault_detector:1.0
         *
         * Register the CDMW Fault Detector object 
         *
         * @param fault_detector Object Reference of the CDMW Fault Notifier object
         *
         */  
        virtual void 
        register_fault_detector(CdmwFT::FaultManagement::FaultDetector_ptr fault_detector)
            throw (CORBA::SystemException);

        /**
         * IDL:thalesgroup.com/CdmwFT/ReplicationManager/get_fault_detector:1.0
         *
         * Returns the reference of the Fault Detector.
         *
         * @return the reference of the Fault Detector.
         *
         * @exception InterfaceNotFound if the Fault Detector is not found.
         *
         */
        virtual CdmwFT::FaultManagement::FaultDetector_ptr get_fault_detector()
            throw(::FT::InterfaceNotFound);

        /**
         * IDL:thalesgroup.com/CdmwFT/ReplicationManager/register_location:1.0
         *
         * Registers a CDMW MembersAdmin object of a specific location. If a location
         * is already registered, the new registration will override the previous one.
         *
         * @param location A CDMW Location
         * @param members_admin Object Reference of the MemebersAdmin object of that location.
         *
         */        
        virtual void register_location (const ::FT::Location &              the_location,
                                        CdmwFT::Location::MembersAdmin_ptr members_admin)
            throw (CORBA::SystemException);

        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/register_statefull_location:1.0
         *
         * Registers a CDMW statefull MembersAdmin object of a specific location. If a
         * location is already registered, the new registration will override the 
         * previous one.
         *
         * @param location A CDMW Location
         * @param members_admin Object Reference of the MemebersAdmin object of that location.
         *
         */
        virtual void 
        register_statefull_location(const  ::FT::Location& the_location,
                                    CdmwFT::Location::StatefullPrimaryBackupAdmin* members_admin, 
                                    const CdmwFT::StateTransfer::LocalDataStoreInfos& local_data_store_infos,
                                    CORBA::Boolean with_reply_logging = false)
           throw(CORBA::SystemException);
        

        /// Report a change in the list of publications.
        /**
         * IDL:omg.org/CosNotifyComm/NotifyPublish/offer_change:1.0
         *
         * Not Yet Implemented!
         *
         * @param added The list of new event types that the consumer can
         *   expect.
         * @param removed The list of event types that the consumer should
         *   no longer expect.
         * @exception InvalidEventType if the one or more event types
         *   provided is invalid
         * @exception CORBA::NO_IMPLEMENT
         */
        virtual void offer_change (const CosNotification::EventTypeSeq & added,
                                   const CosNotification::EventTypeSeq & removed)
            throw (CosNotifyComm::InvalidEventType,
                   CORBA::SystemException);

        /// Receive one structured event
        /**
         * IDL:omg.org/CosNotifyComm/StructuredPushConsumer/push_structered_event:1.0
         *
         *  This operation is invoked to provide one event to the
         * consumer.
         * @exception CosEventComm::Disconnected if the object considers
         *   itself no longer connected to its peer.
         */
        virtual void push_structured_event(const CosNotification::StructuredEvent & notification)
            throw (CosEventComm::Disconnected,
                   CORBA::SystemException);

        /// The peer has disconnected
        /**
         * IDL:omg.org/CosNotifyComm/StructuredPushConsumer/disconnect_structured_push_consumer:1.0
         *
         * Does nothing ...
         * This operation is invoked by the consumer peer when it wishes
         * to disconnect.  The consumer can safely assume that no more
         * events will follow this request.
         */
        virtual void disconnect_structured_push_consumer()
            throw (CORBA::SystemException);

         /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/handle_member_ready:1.0
         * This operation is called by the local replication manager to advise the 
         * primary replication manager that a new inserted member has initialized its 
         * datastore and is ready to be elected primary (BACKUP) or has succeeded 
         * in its initialisation to become PRIMARY.
         * This operation has been created to allow asynchronous initialization 
         * and allows a crash of the Replication Manager: it will then used the IOGR 
         * to advise the new elected Replication Manager that the initialisation 
         * has succeeded.
         *
         * @param object_group_id the group id.
         * @param the_location the CDMW Location embedding the group member.
         *
         * @exception MemberNotFound raised if member is already removed or has not
         * been created by this Replication Manager.
         */
        virtual void 
        handle_member_ready(::FT::ObjectGroupId object_group_id,
				                const ::FT::Location& the_location)
            throw (CORBA::SystemException);

         /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/handle_member_failure:1.0
         *
         * This operation is called by the local replication manager to advise the 
         * primary replication manager that a new inserted member has failed its 
         * initialization process.
         * This operation has been created to allow asynchronous initialization 
         * and allows a crash of the Replication Manager: it will then used the IOGR 
         * to advise the new elected Replication Manager that the initialisation 
         * has not succeeded.
         *
         * @param object_group_id the group id.
         * @param the_location the CDMW Location embedding the group member.
         *
         * @exception MemberNotFound raised if member is already removed or has not
         * been created by this Replication Manager.
         */
        virtual void 
        handle_member_failure(::FT::ObjectGroupId object_group_id, 
				                  const ::FT::Location& the_location)
            throw (CORBA::SystemException);

        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/set_primary_location:1.0
         *
         * This operation ask to all group member in that location to become 
         * primary. This operation is used when Fault Tolerance is managed at
         * process level.
         *
         * @exception BadReplicationStyle if the <code>ReplicationStyle</code>
         *         of the given group is not <code>COLD_PASSIVE</code> or
         *         <code>WARM_PASSIVE</code>.
         */
        virtual void 
        set_primary_location(const ::FT::Location& the_location)
            throw (::FT::PrimaryNotSet, 
                   ::FT::BadReplicationStyle, 
                   ::CORBA::SystemException);

        /**
         * IDL:thalesgroup.com/CdmwFT/
         *      ReplicationManager/set_first_backup:1.0
         *
         * This operation tells the Replication Manager which Location should
         * elected when next primary election occurs.
         *
         * @exception PrimaryNotSet if the Replication Manager cannot set the
         *         primary member of the object group.
         * @exception BadReplicationStyle if the <code>ReplicationStyle</code>
         *         of the given group is not <code>COLD_PASSIVE</code> or
         *         <code>WARM_PASSIVE</code>.
         */
        virtual void set_first_backup (const ::FT::Location& the_location)
        throw (CdmwFT::AlreadyPrimary,
               ::FT::MemberNotFound,
               ::FT::BadReplicationStyle,
               ::CORBA::SystemException);

        virtual CdmwFT::GroupObserverID 
        register_group_state_observer(CdmwFT::GroupObserver* group_observer)
            throw ( ::CORBA::SystemException);

        virtual void 
        unregister_group_state_observer(CdmwFT::GroupObserverID group_observer_id)
            throw ( ::CdmwFT::InvalidGroupObserverId
                  , CORBA::SystemException);

        virtual void
        notify_group_state_observers(::FT::ObjectGroupId       object_group_id,
                                     CdmwFT::ObjectGroupState  new_state,
                                     CORBA::Long               nb_of_replicas,
                                     CORBA::Long               minimum_nb_of_replicas)
            throw();

        /**
         * This operation asks the replication manager to become primary. This 
         * operation is used when Fault Tolerance is managed at process level.
         */
        virtual void activate()
            throw ( ::CORBA::SystemException);

        /*
         * Implementation of 'get_object_group_data' inherited from abstract
         * class ReplicationManagerDelegate.
         *
         * Returns a pointer to a copy of the ObjectGroupData instance
         * associated with the object group with id 'object_group_id'
         */
        virtual ::CdmwFT::ObjectGroupData*
        get_object_group_data (
                ::FT::ObjectGroupId  object_group_id)
            throw (::CORBA::INTERNAL);

        /**
         * Implementation of 'set_object_group_data' inherited from abstract
         * class ReplicationManagerDelegate.
         *
         * Saves the ObjectGroupData in Rep.Mgr. data store
         */
        void
        ReplicationManager_impl::set_object_group_data (::CdmwFT::ObjectGroupData* ogd)
            throw (::CORBA::INTERNAL);

        /**
         * Implementation of 'get_location_info' inherited from abstract
         * class ReplicationManagerDelegate.
         *
         * Returns a pointer on a copy of the LocationInfo with name 'locname':
         *
         */
        virtual ::CdmwFT::LocationInfo*
        get_location_info (const ::FT::Location&  loc_name)
            throw ( ::CORBA::INTERNAL
                  , ::CORBA::BAD_PARAM);

        /**
         * Implementation of 'get_location_info' inherited from abstract
         * class ReplicationManagerDelegate.
         *
         * Returns a pointer on a copy of the LocationInfo with name 'locname':
         *
         */
        virtual ::CdmwFT::LocationInfo*
        get_location_info (const std::string&  loc_name)
            throw ( ::CORBA::INTERNAL);

        /**
         * Implementation of 'set_location_info' inherited from abstract
         * class ReplicationManagerDelegate.
         *
         * Saves the LocationInfo in Rep.Mgr. data store
         */
        void
        set_location_info (
                  ::CdmwFT::LocationInfo*     locinfo
                , const std::string&          locname)
            throw (::CORBA::INTERNAL);

        /**
         * Implementation of 'get_current_ogd' inherited from abstract class
         * ReplicationManagerDelegate.
         *
         * Search the ObjectGroupData with id 'ogid' in the current data. If the
         * item is not found, a fresh copy is extracted from the DataStore and
         * inserted in the current data. If the item is found, returns a pointer
         * on the current item. The current data keeps ownership of the pointed
         * item.
         *
         */
        ::CdmwFT::ObjectGroupData*
        get_current_ogd (::FT::ObjectGroupId  object_group_id);

        /**
         * Implementation of 'get_current_loc' inherited from abstract class
         * ReplicationManagerDelegate.
         *
         * Search the LocationInfo with name 'name' in the current data. If the
         * item is not found, a fresh copy is extracted from the DataStore and
         * inserted in the current data. If the item is found, returns a pointer
         * on the current item. The current data keeps ownership of the pointed
         * item.
         *
         */
        ::CdmwFT::LocationInfo*
        get_current_loc (const std::string&  loc_name);


        /*
         * Implementation of 'modified_ogd' inherited from abstract class
         * ReplicationManagerDelegate.
         *
         * Tells the Rep.Mgr. the the ObjectGroupData 'ogid' has been 
         * modified in the current data
         *
         * Returns false if the item was not found in the current data.
         */
        bool
        modified_ogd (::FT::ObjectGroupId ogid);
        
        /*
         * Implementation of 'modified_loc' inherited from abstract class
         * ReplicationManagerDelegate.
         *
         * Tells the Rep.Mgr. the the LocationInfo 'name' has been 
         * modified in the current data
         *
         * Returns false if the item was not found in the current data.
         */
        bool
        modified_loc (const std::string& name);
        
        /*
         * Builds a ::FT::Location from a std::string
         */
        virtual ::FT::Location*
        string_to_ftloc(const std::string& loc_name)
            throw ( ::CORBA::INTERNAL);

        /*
         * Builds a std::string from a ::FT::Location 
         */
        virtual std::string
        ftloc_to_string(const ::FT::Location& loc_name)
            throw ( ::CORBA::INTERNAL);

        /*
         * Returns a reference on the stream used to display trace data
         */
        std::ostream& get_trace_stream() { return m_ostream; }

        /*
         * Debug utility: shows the contexts associations and states
         */
        void ShowContexts();

        /*
         * Dump the datastore ObjectGroupData
         */
        void dump_ogd_datastore()
            throw ( ::CORBA::SystemException);
        
        /*
         * Dump the datastore LocationInfo
         */
        void dump_location_datastore()
            throw ( ::CORBA::SystemException);
        
    public:
        // Maps a property name to a Property object.
        typedef std::map< std::string,
                          ::FT::Property >          PropertiesMap;
        
        // Maps a repository id to a list of properties.
        typedef std::map< std::string,
                          PropertiesMap>            TypePropertiesMap;

        // Maps an object group id to a GroupInfo structure.
        typedef std::map< ::FT::ObjectGroupId,
                          ::CdmwFT::GroupInfo_var > GroupInfosMap;
        // Defines a list of strings.
        typedef std::vector<std::string> StringList;

    private:
        // The ORB pseudo-object.
        CORBA::ORB_var m_orb;
        
        // The parent POA.
        PortableServer::POA_var m_parent_poa;
        
        // The parent POA for fallback IORs.
        PortableServer::POA_var m_poa;
        
        PortableServer::POA_var multi_poa;
        
        /**
         * The mutex used for changing the functioning mode.
         */
        OsSupport::Mutex m_mutex;

        // The fault tolerance domain.
        CORBA::String_var m_ft_domain_id;
        
        // The Fault Notifier.
        ::FT::FaultNotifier_var m_fault_notifier;       

        // IOGR Factory
        Cdmw::FT::IOGRFactory*  m_iogrFactory;
        
        // The list of Fault Tolerance Properties as stated in the
        // Fault Tolerant CORBA specifications.
        StringList m_ft_properties;
        
        // The list of default properties for all the object groups.
        PropertiesMap m_default_properties;
        
        // The CDMW Fault Detector
        CdmwFT::FaultManagement::FaultDetector_var m_fault_detector;
        
        // the port for the datastore
        std::string m_dataStorePort;
        
        // A reference to an output stream for debug and trace information
        std::ostream & m_ostream;

        // The security level for the add_member
        std::string               m_security_level;
         // The replication level for the add_member
        std::string               m_replication_level;
       
        //Cdmw::FT::DataStoreContext* m_context;
        PropertiesStorageHome*    m_propertiesStorageHome;
        //PropertiesDataStore*      m_propertiesDataStore;

        OGDStorageHome*           m_ogdStorageHome;
        //OGDDataStore*             m_ogdDataStore;

        OGIDStorageHome*          m_ogidStorageHome;
        //OGIDDataStore*            m_ogidDataStore;

        LocInfoStorageHome*       m_locInfoStorageHome;
        //LocInfoDataStore*         m_locInfoDataStore;

        typedef std::map < std::string, Location::Location*, std::less< std::string > >       LocationCtxts;
        typedef std::map < ::FT::ObjectGroupId, ObjectGroup::ObjectGroup* > ObjectGroupCtxts;
        typedef std::map < ::CORBA::ULong, Object::Object* >        ObjectCtxts;

        LocationCtxts     m_loc_ctxts;  // Container for Location contexts 
        ObjectGroupCtxts  m_grp_ctxts;  // Container for ObjectGroup contexts 
        ObjectCtxts       m_obj_ctxts;  // Container for Object contexts
        
        // OBD data store object
        OGDDataStore* m_OGBdataStoreObj;
        
        bool
        remove_locctxt_from_container(const std::string&);

        bool
        remove_grpctxt_from_container(::FT::ObjectGroupId);
        
        bool
        remove_objctxt_from_container(::CORBA::ULong);

        // Thread specific data for CurrentData
        ThreadSpecificData  m_thread_data;

        // Restores the Rep.Mgr. state from the ObjectGroupData list
        void restore_from_ogds(const std::list < ::CdmwFT::ObjectGroupData_var > ogd_list);

        class OGDObserver: public ::Cdmw::CommonSvcs::DataStore::DataStoreObserver
                                  < ::FT::ObjectGroupId, ::CdmwFT::ObjectGroupData >
        {
          public:
            typedef Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId > ::_in_type _Oid_in_type;
            typedef Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData > ::_in_type _Data_in_type;

            OGDObserver(ReplicationManager_impl* rep_mgr)
                : m_rep_mgr(rep_mgr)
            {
            }

            ~OGDObserver() throw() {}

            virtual void on_insert(_Oid_in_type oid, _Data_in_type data)
                throw ()
            {
                try {
                    if (! m_rep_mgr->is_primary())
                        m_rep_mgr->create_grp_ctxt_from_data(oid, data);
                } catch (...) {
                }
            }

            virtual void on_update(_Oid_in_type oid, _Data_in_type data)
                throw ()
            {
                try {
                    if (! m_rep_mgr->is_primary())
                        m_rep_mgr->update_grp_ctxt_from_data(oid, data);
                } catch (...) {
                }
            }

            virtual void on_remove(_Oid_in_type oid)
                throw ()
            {
                try {
                    if (! m_rep_mgr->is_primary())
                        m_rep_mgr->remove_grp_ctxt_from_data(oid);
                } catch (...) {
                }
            }

          private:
            ReplicationManager_impl* m_rep_mgr;
        };


        class LocInfoObserver: public ::Cdmw::CommonSvcs::DataStore::DataStoreObserver
                                  < std::string, ::CdmwFT::LocationInfo >
        {
          public:
            typedef Cdmw::OrbSupport::type_traits< std::string > ::_in_type _Oid_in_type;
            typedef Cdmw::OrbSupport::type_traits< ::CdmwFT::LocationInfo > ::_in_type _Data_in_type;

            LocInfoObserver(ReplicationManager_impl* rep_mgr)
                : m_rep_mgr(rep_mgr)
            {
            }

            ~LocInfoObserver() throw() {}

            virtual void on_insert(_Oid_in_type oid, _Data_in_type data)
                throw ()
            {
                try {
                    if (! m_rep_mgr->is_primary())
                        m_rep_mgr->create_loc_ctxt_from_data(oid, data);
                } catch (...) {
                }
            }

            virtual void on_update(_Oid_in_type oid, _Data_in_type data)
                throw ()
            {
                try {
                    if (! m_rep_mgr->is_primary())
                        m_rep_mgr->update_loc_ctxt_from_data(oid, data);
                } catch (...) {
                }
            }

            virtual void on_remove(_Oid_in_type oid)
                throw ()
            {
                try {
                    if (! m_rep_mgr->is_primary())
                        m_rep_mgr->remove_loc_ctxt_from_data(oid);
                } catch (...) {
                }
            }

          private:
            ReplicationManager_impl* m_rep_mgr;
        };

        // Datastore Observer
        OGDObserver* ogdObserver;
        LocInfoObserver* locInfoObserver;


    private:
        /**
         * Checks if the given properties are valid and supported.
         *
         * @param props       The properties to check.
         * @param type        The type of properties, type can be either
         *                    <code>DEFAULT_PROPERTIES</code> or
         *                    <code>TYPE_PROPERTIES</code> or
         *                    <code>GROUP_PROPERTIES</code>
         * @param check_value If false then bypass the value checking.
         *
         * @throw InvalidProperty if one or more of the properties in the
         *        sequence is not valid.
         * @throw UnsupportedProperty if one or more of the properties in
         *        the sequence is not supported.
         */
        void check_properties(const ::FT::Properties & props,
                              int type,
                              bool check_value = true)
            throw(::FT::InvalidProperty,
                  ::FT::UnsupportedProperty);



        /**
         * Returns the object groupid of the object group.
         *
         * @param object_group the object group reference.
         *
         * @return an ObjectGroupId
         *
         * @exception ObjectGroupNotFound if the object group with
         *   the given reference is not found 
         */
        ::FT::ObjectGroupId
        ReplicationManager_impl::get_object_group_id_from_ref ( ::FT::ObjectGroup_ptr object_group ) const
            throw(::FT::ObjectGroupNotFound,
                  CORBA::SystemException);


        /**
         * Returns the primary location, if any, of an object group.
         *
         * @param ogd The object group data object holding all known
         * data concering an object group.
         *
         * @return a pointer to the location of the primary member.
         *
         * TODO: 
         *     I) Add it as a member function to ObjectGroupData struct
         */

        ::FT::Location *
        get_group_primary_location(const ::CdmwFT::ObjectGroupData & ogd) const
            throw ();
        /**
         * Sets a primary location for the object group.
         *
         * @param ogd The object group data object holding all known
         * data concering an object group.
         * @param primary_location Location of the primary member.
         *
         * @exception CORBA::NO_MEMORY
         *
         * TODO: 
         *      I) Add a check for empty location...
         *     II) Add a ::FT::MemberNotFound exception for empty location!
         *         (or maybe a CORBA::BAD_PARAM system exception).
         *    III) Add it as a member function to ObjectGroupData struct
         */

        void
        set_group_primary_location(::CdmwFT::ObjectGroupData & ogd, 
                             const ::FT::Location & primary_location) const
            throw (CORBA::NO_MEMORY);

        /**
         * Removes the primary location, if any, for the object group.
         *
         * @param ogd The object group data object holding all known
         * data concerning an object group.
         *
         * @return <b>true</b> if a location has been removed, <b>false</b> instead.
         *
         * TODO: 
         *    I) Add it as a member function to ObjectGroupData struct
         */
        bool
        erase_group_primary_location(::CdmwFT::ObjectGroupData & ogd) const
            throw ();
        
        /**
         * Selects a new member to be made primary for the object group.
         *
         * @param ogdgroup_data The object group data object holding all known
         * data concerning an object group.
         *
         * @return A pointer to the 'promoted' member's location. It returns
         * an empty location if no primary available.

         * @exception CORBA::NO_MEMORY...
         *
         * TODO: 
         *    I) Add it as a member function to ObjectGroupData struct
         */
        ::FT::Location* 
        elect_a_new_primary(::CdmwFT::ObjectGroupData & group_data) const
            throw (CORBA::NO_MEMORY);


        /**
         * Selects a new Location where all the contained object groups are to
         * be made primary.
         *
         * @param ogdgroup_data The object group data object holding all known
         * data concerning an object group.
         *
         * @return A pointer to the 'promoted' Location. Returns a NULL pointer
         * if no primary is available.

         * @exception CORBA::SystemException...
         *
         */
        Location::Location* 
        elect_a_primary_location(Location::Location* old_primary) const
            throw (CORBA::SystemException);


        /**
         * Notifies all group member of the new group view (group
         * membership).
         *
         * @param object_group_id The object group's identifier
         * @param group_data The object group data object holding all known
         * data concerning an object group.
         * @param removed_location Object reference of a MembersAdmin object to
         * be notified if it is just removed (if any).
         *
         * @return <b>true</b> if everything went fine, <b>false</b> instead..
         *
         */
        bool 
        notify_update_group_view(::FT::ObjectGroupId                 object_group_id,
                                 const ::CdmwFT::ObjectGroupData &   group_data,
                                 CdmwFT::Location::MembersAdmin_ptr removed_location) const
            throw (CORBA::INTERNAL);

        /**
         * Notifies all group member of the new group view with a new 
         * primary replica.
         *
         * @param object_group_id The object group's identifier
         * @param group_data The object group data object holding all known
         * data concerning an object group.
         * @pram primary_location The location of the primary member of the object's group.
         * @param removed_location Object reference of a MembersAdmin object to
         * be notified if it is just removed (if any).
         *
         * @return <b>true</b> if everything went fine, <b>false</b> instead..
         *
         */
        bool 
        notify_update_primary(::FT::ObjectGroupId                 object_group_id,
                              const ::CdmwFT::ObjectGroupData &   group_data,
                              const ::FT::Location &              primary_location,
                              CdmwFT::Location::MembersAdmin_ptr removed_location) const
            throw (CORBA::INTERNAL);

        /**
         * Handles the failure of a process (location) and performs 
         * elections for primary members if necessary (failure
         * of a primary member!). All group members within the process
         * will be considered faulty.
         *
         * @param the_location The location of the failed process.
         *
         */
        void 
        handle_process_failure(const ::FT::Location & the_location)
            throw (CORBA::SystemException);

        /**
         * Handles the failure of a CDMW application agent (location) and performs 
         * elections for primary members if necessary (failure
         * of a primary member!). All group members within the process managed by the
         * application on the considered host will be considered faulty.
         *
         * @param hostname The Host hosting the failed process
         * @param applicationname The CDMW application managing the failed process
         *
         */
        void 
        handle_application_failure(const std::string & hostname,
                                   const std::string & applicationname)
            throw (CORBA::SystemException);

        /**
         * Handles the failure of a CDMW platform daemon and performs 
         * elections for primary members if necessary (failure
         * of a primary member!). All members of object groups within this host 
         * will be considered faulty.
         *
         * @param hostname The Host hosting the failed process
         *
         */
        void 
        handle_host_failure(const std::string & hostname)
            throw (CORBA::SystemException);

        

        /**
         * This operation removes a member from group_data and updates
         * it with a new object group reference and members list.
         * It also update the primary or the group view.
         *
         * @param object_group_id  The object group id of the object group
         *                     from which the member is to be removed.
         * @param the_location The physical location; that is, a fault
         *                     containment region, host, cluster of hosts,
         *                     etc. of the member to be removed.
         *
         * @return the object group reference for the object group with the
         *         member removed. This reference may be the same as that
         *         passed in as a parameter.
         *
         * @exception MemberNotFound if the Replication Manager cannot find a
         *         member of the object group at the given location.
         */
        ::FT::ObjectGroup_ptr
        remove_member(::FT::ObjectGroupId         object_group_id,
                      const ::FT::Location &      the_location)
            throw(::FT::ObjectGroupNotFound,
                  ::FT::MemberNotFound,
                  CORBA::SystemException);

        /**
         *
         */
        void
        build_iogr (const ::FT::Location&         primary_location,
                    ::CdmwFT::ObjectGroupData&    group_data)
            throw(::FT::PrimaryNotSet,
                  ::FT::ObjectNotCreated,
                  ::CORBA::SystemException);

        /*
         * This operation decode the IOR to recover the repository ID
         *
         * @param object  the decoded object
         *
         * @return the repository ID of the object
         */
        
        std::string
        get_repository_id_from_ior(::CORBA::Object_ptr object)
            throw();
        
        /**
         * The internal set_primary_location called from the IDL
         * operations set_primary_member and set_primary_location.
         * This method assumes that the CurrentData has been set
         * by the caller.
         */
        virtual void 
        set_primary_location (
                Location::Location*   new_primary
              , const ::FT::Location& new_primary_ftloc
              )
            throw (::FT::PrimaryNotSet, 
                   ::CORBA::SystemException);

        /**
         * check if the replication manager is primary
         * it is activated if the datastore inside are activated
         */
        bool is_primary()
        {
            Cdmw::FT::DataStoreBase* dsBase = Cdmw::FT::DataStoreBase::Get_dataStore(OGID_DATA_STORE);
            CDMW_ASSERT(dsBase != 0);

            return dsBase->get_context().is_activated();
        }
        

        /**
         * Creates the ObjectGroup context associated with the ObjectGroupData
         * 'ogd' and ObjectGroupId 'ogid'. As a side effect, Object contexts are
         * also created at the same time.
         *
         * Typically called from a DataStoreObserver 'on_insert' callback.
         *
         */
        virtual void 
        create_grp_ctxt_from_data (
                ::FT::ObjectGroupId              ogid
              , const CdmwFT::ObjectGroupData &  ogd
              )
            throw (CORBA::INTERNAL);

        /**
         * Update the ObjectGroup context associated with the ObjectGroupData
         * 'ogd' and ObjectGroupId 'ogid'.
         *
         * Typically called from a DataStoreObserver 'on_update' callback.
         *
         */
        virtual void
        update_grp_ctxt_from_data(
                ::FT::ObjectGroupId              ogid
              , const CdmwFT::ObjectGroupData &  ogd
              )
            throw (CORBA::INTERNAL);


        /**
         * Remove the ObjectGroup context associated with the ObjectGroupData
         * 'ogd' and ObjectGroupId 'ogid'.
         *
         * Typically called from a DataStoreObserver 'on_remove' callback.
         *
         */
        virtual void
        remove_grp_ctxt_from_data(
                ::FT::ObjectGroupId              ogid
              )
            throw (CORBA::INTERNAL);

        /**
         * Creates the Location context associated with the LocationInfo 'locinfo'
         * and name 'locname'.
         *
         * Typically called from a DataStoreObserver 'on_insert' callback.
         *
         */
        virtual void 
        create_loc_ctxt_from_data (
                const std::string &              locname
              , const CdmwFT::LocationInfo &     locinfo
              )
            throw (CORBA::INTERNAL);

        /**
         * Update the Location context associated with the LocationInfo 'locinfo'
         * and name 'locname'.
         *
         * Typically called from a DataStoreObserver 'on_insert' callback.
         *
         */
        virtual void 
        update_loc_ctxt_from_data (
                const std::string &              locname
              , const CdmwFT::LocationInfo &     locinfo
              )
            throw (CORBA::INTERNAL);

        /**
         * remove the Location context associated with the LocationInfo 'locinfo'
         * and name 'locname'.
         *
         * Typically called from a DataStoreObserver 'on_insert' callback.
         *
         */
        virtual void 
        remove_loc_ctxt_from_data (
                const std::string &              locname
              )
            throw (CORBA::INTERNAL);


            
        // Default constructor.
        ReplicationManager_impl(void)
            throw();
        
        // Hide the copy constructor.
        ReplicationManager_impl(const ReplicationManager_impl & repl_mgr)
            throw();
        
        // Hide the copy assignment operator.
        ReplicationManager_impl &
        operator=(const ReplicationManager_impl & repl_mgr)
            throw();
    };

}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

#endif // _CDMW_FT_REPLICATION_MANAGER_REPLICATION_MANAGER_IMPL_HPP_

