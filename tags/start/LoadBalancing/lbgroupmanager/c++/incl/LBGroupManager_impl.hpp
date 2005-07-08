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
 
#ifndef INCL_CDMW_LB_GROUP_MANAGER_H_
#define INCL_CDMW_LB_GROUP_MANAGER_H_
/**
 * @brief A Group Manager implementation
 *
 * @author Raffaella Panella <rpanella@amjsv.it>
 */

#include "Foundation/osthreads/Mutex.hpp"

#include <map>
#include <memory>

#include "LoadBalancing/idllib/CdmwLBGroupManager.skel.hpp"
#include "LoadBalancing/idllib/CdmwLBCommon.stub.hpp"
#include <LoadBalancing/lbcommon/LB_IOGRFactory.hpp>


namespace Cdmw
{

namespace LB {

    class LBGroupManager_impl : public virtual POA_CdmwLB::LBGroupManager
    {
    public:
        LBGroupManager_impl (CORBA::ORB_ptr orb, 
                             PortableServer::POA_ptr poa,
                             const PortableGroup::GroupDomainId,
                             std::ostream & os)
            throw(CORBA::SystemException);
        
        virtual ~LBGroupManager_impl();
        
    public:
        /**
         * IDL:omg.org/PortableGroup/GenericFactory/create_object:1.0
         *
         * Creates an object, using the <code>type_id</code> parameter to
         * determine which type of object to create and <code>the_criteria
         * </code> parameter to determine restrictions on how and where to
         * create the object. The out parameter, <code>factory_creation_id
         * </code>, allows the entity that invoked the factory, and the
         * factory itself, to identify the object for subsequent deletion.
         * If the application invokes the <code>create_object()</code>
         * operation on the <code>GenericFactory</code> interface, inherited
         * by the Group Manager, then it creates an object group.
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
         *         </code> interface is inherited by the Group Manager,
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
        
        virtual PortableGroup::ObjectGroup_ptr create_object(const char * type_id,
                                                             const PortableGroup::Criteria & the_criteria,
                                                             PortableGroup::GenericFactory::FactoryCreationId_out factory_creation_id)
            throw (CORBA::SystemException,
                   PortableGroup::NoFactory,
                   PortableGroup::ObjectNotCreated,
                   PortableGroup::InvalidCriteria,
                   PortableGroup::InvalidProperty,
                   PortableGroup::CannotMeetCriteria);
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
        
        virtual void delete_object(const PortableGroup::GenericFactory::FactoryCreationId &
                                   factory_creation_id)
            throw (CORBA::SystemException,
                   PortableGroup::ObjectNotFound);
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
        
        virtual PortableGroup::ObjectGroup_ptr add_member(PortableGroup::ObjectGroup_ptr object_group,
                                                          const PortableGroup::Location & the_location,
                                                          CORBA::Object_ptr replica)
            throw (CORBA::SystemException,
                   PortableGroup::MemberAlreadyPresent,
                   PortableGroup::ObjectGroupNotFound,
                   PortableGroup::ObjectNotAdded);
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
        
        virtual PortableGroup::ObjectGroup_ptr remove_member(PortableGroup::ObjectGroup_ptr object_group,
                                                             const PortableGroup::Location & the_location)
            throw (CORBA::SystemException,
                   PortableGroup::MemberNotFound,
                   PortableGroup::ObjectGroupNotFound);
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
        virtual PortableGroup::ObjectGroup_ptr
        create_member(PortableGroup::ObjectGroup_ptr object_group,
                      const PortableGroup::Location & the_location,
                      const char * type_id,
                      const PortableGroup::Criteria & the_criteria)
            throw(PortableGroup::ObjectGroupNotFound,
                  PortableGroup::MemberAlreadyPresent,
                  PortableGroup::NoFactory,
                  PortableGroup::ObjectNotCreated,
                  PortableGroup::InvalidCriteria,
                  PortableGroup::CannotMeetCriteria,
                  CORBA::SystemException);
        
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
        virtual ::PortableGroup::Locations *
        locations_of_members(::PortableGroup::ObjectGroup_ptr object_group)
            throw(::PortableGroup::ObjectGroupNotFound,
                  CORBA::SystemException);
        
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
        virtual PortableGroup::ObjectGroupId
        get_object_group_id(PortableGroup::ObjectGroup_ptr object_group)
            throw(PortableGroup::ObjectGroupNotFound,
                  CORBA::SystemException);
        
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
        virtual PortableGroup::ObjectGroup_ptr
        get_object_group_ref(PortableGroup::ObjectGroup_ptr object_group)
            throw(PortableGroup::ObjectGroupNotFound,
                  CORBA::SystemException);
        
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
        virtual CORBA::Object_ptr
        get_member_ref(PortableGroup::ObjectGroup_ptr object_group,
                       const PortableGroup::Location & loc)
            throw(PortableGroup::ObjectGroupNotFound,
                  PortableGroup::MemberNotFound,
                  CORBA::SystemException);
        

        virtual PortableGroup::ObjectGroups* groups_at_location (const PortableGroup::Location & the_location)
            throw (CORBA::SystemException);

        /**
         * IDL:/CdmwLB/
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
        virtual PortableGroup::ObjectGroup_ptr
        get_object_group_ref_from_gid(PortableGroup::ObjectGroupId  object_group_id)
            throw(PortableGroup::ObjectGroupNotFound,
                  CORBA::SystemException);
        
        /**
         *Purpose:
         *<p>
         * Implements the
         * IDL:thalesgroup.com/CdmwPlatformMngt/PlatformObserver/notify:1.0
         * operation
         */
        virtual void
         notify( CdmwPlatformMngt::Event* an_event )
            throw( CORBA::SystemException );

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
        virtual ::PortableGroup::ObjectGroupRefVersion
        get_object_group_version_from_ref(::PortableGroup::ObjectGroup_ptr object_group)
            throw(::PortableGroup::ObjectGroupNotFound,
                  CORBA::SystemException);

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
        virtual ::PortableGroup::ObjectGroupRefVersion
        get_object_group_version_from_gid(::PortableGroup::ObjectGroupId object_group_id)
            throw(::PortableGroup::ObjectGroupNotFound,
                  CORBA::SystemException);

        
        
    private:
        
        /**
         * Handles the failure of a process (location).
         * All group members within the process
         * will be considered faulty.
         *
         * @param hostname The Host hosting the failed process
         * @param applicationname The CDMW application managing the failed process
         * @param processname The CDMW processname of the failed process
         * within its managing application.
         * @pram the_location The location of the failed process.
         *
         */
        void 
        handle_process_failure(const std::string &    hostname,
                               const std::string &    applicationname,
                               const std::string &    processname,
                               const ::PortableGroup::Location & the_location)
            throw (CORBA::SystemException);
        /**
         * Handles the failure of a CDMW application agent (location).
         * All group members within the process managed by the
         * application on the considered host will be considered faulty.
         *
         * @param hostname The Host hosting the failed process
         * @param applicationname The CDMW application managing the failed process
         *
         */
        void 
        handle_application_failure(const std::string & hostname,
                                   const std::string & applicationname,
                                   const ::PortableGroup::Location & the_location)
            throw (CORBA::SystemException);

        /**
         * Handles the failure of a CDMW platform daemon.
         * All members of object groups within this host 
         * will be considered faulty.
         *
         * @param hostname The Host hosting the failed process
         *
         */
        void 
        handle_host_failure(const std::string & hostname,
                            const ::PortableGroup::Location & the_location)
            throw (CORBA::SystemException);

        

    private:
        /**
         * The mutex used for changing the functioning mode.
         */
        OsSupport::Mutex m_mutex;  
        
        // The parent POA for fallback IORs.
        PortableServer::POA_var m_poa;
        
        // The current object group id
        PortableGroup::ObjectGroupId m_object_group_id;
        
        // The ORB pseudo-object.
        CORBA::ORB_var m_orb;
        
        // IOGR Factory
        Cdmw::LB::IOGRFactory* m_iogr_factory;
        
        // The load balancing domain
        CORBA::String_var m_lb_domain_id;
        
        // A reference to an output stream for debug and trace information
        std::ostream & m_ostream;
        
        
        // Maps an object group id to an ObjectGroupData structure.
        typedef std::map<PortableGroup::ObjectGroupId,CdmwLB::ObjectGroupData*> GroupMap;
        GroupMap m_group_map;
        GroupMap::iterator m_group_pos;
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
        remove_member(PortableGroup::ObjectGroupId          object_group_id,
                      CdmwLB::ObjectGroupData &             group_data,
                      const PortableGroup::Location &       the_location)                
            throw(PortableGroup::MemberNotFound,
                  CORBA::SystemException);
    };
    
} /* LB*/
    
} /* Cdmw */

#endif /* INCL_CDMW_LB_GROUP_MANAGER_H_ */
