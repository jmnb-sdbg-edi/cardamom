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

#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/common/Assert.hpp>
#include "ftreplicationmanager/CdmwFTLocationInfo.stub.hpp"
#include "ftreplicationmanager/ObjectGroup.hpp"
#include "ftreplicationmanager/Location.hpp"
#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

namespace Cdmw {
namespace FT {
namespace ReplicationManager {
namespace ObjectGroup {

    ObjectGroup::ObjectGroup ( ::FT::ObjectGroupId          ogid
                             , ReplicationManagerDelegate*  rep_mgr
                             )
        : _fsm(*this)
        , m_members_count(0)
        , m_min_members_count(0)
        , m_rep_mgr(rep_mgr)
        , m_stateness(::CdmwFT::GroupStateness_STATELESS)
        , m_ogid(ogid)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
    }

    ObjectGroup::~ObjectGroup ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        remove_from_container();
    }

    /*
     * Removes the pair (this->get_key, this) from the ContextContainer.
     */
    bool
    ObjectGroup::remove_from_container ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        return m_rep_mgr->remove_grpctxt_from_container(m_ogid);
    }


    /*
     * Silently overide possibly existing value or create a new map item.
     */
    void
    ObjectGroup::associate_location (
            const std::string&        locname
          , Location::Location*       location)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_infos[locname].loc = location;
    }
 

    /*
     * If the Info (loc, obj) contains a non null obj pointer, simply
     * overide loc with NULL, else removes (loc, obj) from the map.
     */
    bool 
    ObjectGroup::disassociate_location (
            const std::string&        locname)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"locname="<<locname);
        InfosMap::iterator pos;
        pos = m_infos.find(locname);
        if (pos == m_infos.end()) return false;
        // We do not remove map entry since association is created at object group creation
        // ReplicatioManager.create_object();
        CDMW_ASSERT(m_infos[locname].obj == NULL);
        
        m_infos[locname].obj = NULL;

        return true;
    }

    bool ObjectGroup::remove_location (const std::string&        locname)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"locname="<<locname);
        InfosMap::iterator pos;
        pos = m_infos.find(locname);
        if (pos == m_infos.end()) 
        {
            return false;
        }
        else
        {
            m_infos.erase(pos);
            return true;
        }
    }
    


    /*
     * Silently overide possibly existing value or create a new map item.
     *
     * throw BAD_INV_ORDER if attempting to associate an object while
     * the Location does not exist (NULL loc pointer)
     */
    void 
    ObjectGroup::associate_object (
            const std::string&        locname
          , Object::Object*           object)
        throw(::CORBA::BAD_INV_ORDER)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"locname="<<locname);
        InfosMap::iterator pos;
        pos = m_infos.find(locname);
        if (pos == m_infos.end()) {
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::associate_object(" << locname << "):"
                       << "Could not associate Object (location not known to the group)\n"
                       << "Raising CORBA::BAD_INV_ORDER exception." );
            CORBA::BAD_INV_ORDER ex( ::Cdmw::OrbSupport::BAD_INV_ORDERNullLocationPtr
                                     , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        if (pos->second.loc == NULL) {
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::associate_object(" << locname << "):"
                       << "Could not associate Object (location is NULL)\n"
                       << "Raising CORBA::BAD_INV_ORDER exception.");
            CORBA::BAD_INV_ORDER ex( ::Cdmw::OrbSupport::BAD_INV_ORDERNullLocationPtr
                                     , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        pos->second.obj = object;
    }
    
    /*
     * If the Info (loc, obj) contains a non null loc pointer, simply
     * overide obj with NULL, else removes (loc, obj) from the map.
     */
    bool 
    ObjectGroup::disassociate_object (
            const std::string&        locname)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"locname="<<locname);
        InfosMap::iterator pos;
        pos = m_infos.find(locname);
        if (pos == m_infos.end()) return false;
        if (m_infos[locname].loc == NULL) 
        {
            m_infos.erase(pos);
        }
        
        else                               m_infos[locname].obj = NULL;
        return true;
    }


    /*
     * Disassociate the current ObjectGroup from all its peer Locations and Objects
     */
    void 
    ObjectGroup::disassociate ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        for ( InfosMap::iterator pos = m_infos.begin()
            ; pos != m_infos.end()
            ; ++pos) {
            if (pos->second.loc)	 
                pos->second.loc->disassociate_object_group(m_ogid);
            if (pos->second.obj)
                pos->second.obj->clear_parent_group();
        }
        m_infos.clear();
        
    }


    /*
     * Returns the pointer to the Location with name 'name' or NULL
     * if this name doesn't match any in the internal map.
     */
    Location::Location* 
    ObjectGroup::get_locctxt (const std::string& name)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"name="<<name);
        InfosMap::iterator pos;
        pos = m_infos.find(name);
        if (pos == m_infos.end()) return NULL;
        else                      return pos->second.loc;
    }
    

    /*
     * Returns the pointer to the Object at location with name
     * 'name' or NULL if this name doesn't match any in the map.
     */
    Object::Object* 
    ObjectGroup::get_objctxt (const std::string& name)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"name="<<name);
        InfosMap::iterator pos;
        pos = m_infos.find(name);
        if (pos == m_infos.end()) return NULL;
        else                      return pos->second.obj;
    }
    

    /*
     *
     */
    bool
    ObjectGroup::match_stateness (
				    Location::Location&  location)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        //
        // Set the stateness of the group: an ObjectGroup is statefull if its locations
        // are themselves all statefull (ie registered with register_statefull_location). 
        // A group is stateless if all its locations are themselves all stateless (ie
        // registered with register_location). 
        // Having locations with inconsistent stateness is illegal.
        //
        ::CdmwFT::ObjectGroupData* ogd;
        try {
            ogd = m_rep_mgr->get_current_ogd(m_ogid);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::match_stateness():"
                       << "ObjectGroupData " << m_ogid << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        CdmwFT::LocationStateness loc_stateness = location.stateness();
        CORBA::ULong members_list_length = ogd->members.length();
        if (members_list_length == 0) {
            // First add_member for this group, set m_ogd->stateness according with the
            // members admin type.
            ogd->stateness
                = (loc_stateness == CdmwFT::LocationStateness_STATEFULL)? 
                                                      CdmwFT::GroupStateness_STATEFULL:
                                                      CdmwFT::GroupStateness_STATELESS; 
        }
        else {
            // Subsequent add_member for this group. Check that the stateness does
            // not change.
            CdmwFT::GroupStateness new_stateness 
                = (loc_stateness == CdmwFT::LocationStateness_STATEFULL)? 
                                                      CdmwFT::GroupStateness_STATEFULL:
                                                      CdmwFT::GroupStateness_STATELESS; 
            if (new_stateness != ogd->stateness) {
                CDMW_LOG_FUNCTION_RETURN(false);
                return false;
            }
        }
        m_rep_mgr->modified_ogd(m_ogid);
        CDMW_LOG_FUNCTION_RETURN(true);
        return true;
    }


    /*
     *
     */
    void 
    ObjectGroup::ev_primary_ready (Object::Object& objctxt)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            _fsm.ev_primary_ready(objctxt);
        } catch (const ::statemap::StateUndefinedException &) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedState
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::statemap::TransitionUndefinedException &) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedTransition
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::statemap::IndexOutOfBoundsException &) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMIndexOutOfBounds
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch(...) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUnexpectedException
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }


    /*
     * Set objctxt state to PRIMARY 
     */
    void 
    ObjectGroup::primary_ready (Object::Object& objctxt)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            objctxt.ev_init_primary_done();


            ::CdmwFT::ObjectGroupData* ogd;
            try {
                ogd = m_rep_mgr->get_current_ogd(objctxt.ogid());
            } catch (const ::CORBA::INTERNAL &) {
                CDMW_ERROR(FTLogger::GetLogger(),  "ObjectGroup::add_primary():"
                           << "ObjectGroupData " << m_ogid << " not found \n"
                           << "Raising CORBA::INTERNAL exception.");
                throw CORBA::INTERNAL( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                       , CORBA::COMPLETED_NO);
            }


            ::CORBA::ULong size = ogd->members.length();
            for (::CORBA::ULong index = 0;
                 index < size;
                 index++)
            {
                if (ogd->members[index].local_id == objctxt.local_id())
                {
                    ogd->members[index].the_state     = objctxt.fsm_state();
                    break;
                }
            }
            
            // The ogd has been modified, set the 'modify' flag in current
            m_rep_mgr->modified_ogd(m_ogid);
            

        } catch(const CORBA::INTERNAL & ex) {
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
    }


    /*
     *
     */
    void 
    ObjectGroup::ev_backup_ready (Object::Object& objctxt)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            _fsm.ev_backup_ready(objctxt);
        } catch (const ::statemap::StateUndefinedException &) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedState
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::statemap::TransitionUndefinedException &) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedTransition
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::statemap::IndexOutOfBoundsException &) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMIndexOutOfBounds
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch(...) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUnexpectedException
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }


    /*
     * Set objctxt state to BACKUP 
     */
    void 
    ObjectGroup::backup_ready (Object::Object& objctxt)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            objctxt.ev_init_backup_done();


            ::CdmwFT::ObjectGroupData* ogd;
            try {
                ogd = m_rep_mgr->get_current_ogd(objctxt.ogid());
            } catch (const ::CORBA::INTERNAL &) { 
                CDMW_ERROR(FTLogger::GetLogger(), "ObjectGroup::add_primary():"
                           << "ObjectGroupData " << m_ogid << " not found \n"
                           << "Raising CORBA::INTERNAL exception.");
                throw CORBA::INTERNAL( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                       , CORBA::COMPLETED_NO);
            }


            ::CORBA::ULong size = ogd->members.length();
            for (::CORBA::ULong index = 0;
                 index < size;
                 index++)
            {
                if (ogd->members[index].local_id == objctxt.local_id())
                {
                    ogd->members[index].the_state     = objctxt.fsm_state();
                    break;
                }
            }
            
            // The ogd has been modified, set the 'modify' flag in current
            m_rep_mgr->modified_ogd(m_ogid);

        } catch(const CORBA::INTERNAL & ex) {
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
    }


    /*
     *
     */
    void 
    ObjectGroup::ev_member_removed (Object::Object& objctxt)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            _fsm.ev_member_removed(objctxt);
        } catch (const ::statemap::StateUndefinedException &) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedState
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::statemap::TransitionUndefinedException &) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedTransition
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::statemap::IndexOutOfBoundsException &) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMIndexOutOfBounds
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch(...) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUnexpectedException
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }


    /*
     * Removes the Object from the internal map and delete it 
     */
    void 
    ObjectGroup::member_removed (Object::Object& objctxt)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            disassociate_object(objctxt.locname());
        } catch(const CORBA::INTERNAL & ex) {
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
    }


    /*
     *
     */
    void
    ObjectGroup::notify_observers ()
        throw()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_rep_mgr->notify_group_state_observers(m_ogid,
                                                m_fsm_state, 
                                                m_members_count,
                                                m_min_members_count);
    }


    /*
     *
     */
    void 
    ObjectGroup::add_primary (
                     Location::Location*   locctxt
                   , Object::Object*       objctxt
                   , ::CORBA::Object_ptr   member)
        throw ( ::FT::ObjectNotAdded
              , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        ::CdmwFT::ObjectGroupData* ogd;
        try {
            ogd = m_rep_mgr->get_current_ogd(m_ogid);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::add_primary():"
                       << "ObjectGroupData " << m_ogid << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        // Associate the Object to the current ObjectGroup: no need to associate
        // Location locctxt, this has already been done at create_object or
        // register_xxx_location time.
        try {
            associate_object (locctxt->name(), objctxt);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::add_primary():"
                       << "Could not associate Object (likely due to a NULL ObjectGroup)\n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        
        // Tell the Location involved in this group (except locctxt) that from 
        // now on they will have to add their members as backup.
        try {
            for ( InfosMap::iterator iter = m_infos.begin()
                ; iter != m_infos.end()
                ; ++iter
                ) {
                if (iter->second.loc != locctxt)
                    iter->second.loc->backup(true);
            }
        } catch (const ::CORBA::INTERNAL & e) {
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::add_primary():"
                       << "Location FSM transition set_backup failed\n"
                       << ::Cdmw::OrbSupport::OrbSupport::exception_to_string(e) );
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw;
        }

        // Update the ObjectGroupData with member's info
        ogd->members.length(1);
        ::CdmwFT::MemberOverallInfo member_info;
        member_info.local_id      = objctxt->local_id();
        member_info.the_reference = CORBA::Object::_duplicate(member);
        ::FT::Location_var ftloc  = m_rep_mgr->string_to_ftloc(locctxt->name());
        member_info.the_location  = ftloc;
        member_info.the_state     = objctxt->fsm_state();
        ogd->members[0]           = member_info;
        
        // The ogd has been modified, set the 'modify' flag in current
        m_rep_mgr->modified_ogd(m_ogid);
    }


    /*
     *
     */
    void 
    ObjectGroup::add_backup (
                     Location::Location*   locctxt
                   , Object::Object*       objctxt
                   , ::CORBA::Object_ptr   member)
        throw ( ::FT::ObjectNotAdded
              , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        ::CdmwFT::ObjectGroupData* ogd;
        try {
            ogd = m_rep_mgr->get_current_ogd(m_ogid);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(), "ObjectGroup::add_backup():"
                       << "ObjectGroupData " << m_ogid << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        // Associate the Object to the current ObjectGroup: no need to associate
        // Location locctxt, this has already been done at create_object or
        // register_xxx_location time.
        try {
            associate_object (locctxt->name(), objctxt);
        } catch (::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::add_backup():"
                       << "Could not associate Object (likely due to a NULL ObjectGroup)\n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        // Update the ObjectGroupData with member's info
        ::CORBA::ULong current_index = ogd->members.length();
        ogd->members.length(current_index + 1);
        ::CdmwFT::MemberOverallInfo member_info;
        member_info.local_id      = objctxt->local_id();
        member_info.the_reference = CORBA::Object::_duplicate(member);
        ::FT::Location_var ftloc  = m_rep_mgr->string_to_ftloc(locctxt->name());
        member_info.the_location  = ftloc;
        member_info.the_state     = objctxt->fsm_state();
        ogd->members[current_index] = member_info;
        
        // The ogd has been modified, set the 'modify' flag in current
        m_rep_mgr->modified_ogd(m_ogid);
    }


    /*
     * Called by the owner Location when a location failure is processed.
     *
     * - Removes the member MemberOverallInfo from the ObjectGroupData
     * - Erases the ::FT::Location in the ObjectGroupData if the member
     *   belongs to a PRIMARY Locatio  (ie is PRIMARY itself)
     * - Disassociate the member from the ObjectGroup
     * - Updates the IOGR
     *
     * Other Locations are not updated here as it is a Rep.Mgr. level job.
     */
    void 
    ObjectGroup::remove_member (
                  Object::Object*          member
                , bool                     is_primary)
         throw ( ::FT::PrimaryNotSet
               , ::FT::ObjectNotCreated
               , ::FT::MemberNotFound
               , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"is_primary="<<is_primary);
        CDMW_ASSERT(member != 0);
        
        ::CdmwFT::ObjectGroupData* ogd;
        try {
            ogd = m_rep_mgr->get_current_ogd(m_ogid);
        } catch (const ::CORBA::INTERNAL &) {  
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::remove_member():"
                       << "ObjectGroupData " << m_ogid << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        // Build the list of objects that will remain members of the group
        const CORBA::ULong members_number = ogd->members.length();
        ::CdmwFT::MemberOverallInfos new_members(members_number);
        new_members.length(members_number);
       CORBA::ULong idx = 0;
        bool location_found = false;
        for (::CORBA::ULong i=0; i < members_number; i++) {
            if (ogd->members[i].local_id == member->local_id()) {
                location_found = true;
            } else {
                new_members[idx++] = ogd->members[i];
            }
       }
        
        if (! location_found) {
            throw ::FT::MemberNotFound();
        }

        new_members.length(members_number - 1);
        ::FT::Location_var primary_loc = m_rep_mgr->get_group_primary_location(*ogd);
        if (is_primary) {
           m_rep_mgr->erase_group_primary_location(*ogd);
        }
        
        // Disassociate the member from the ObjectGroup: calling ev_member_removed
        // because it may modify the state of the ObjectGroup fsm
        try {
            ev_member_removed (*member);
            delete member;
        } catch (CORBA::INTERNAL) { 
            CDMW_ERROR(FTLogger::GetLogger(), "ObjectGroup::remove_member():"
                       << "Could not remove the member (likely due to a parent Location not found)\n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        
        // Build a new IOGR
        // may throw ::FT::PrimaryNotSet or ::FT::ObjectNotCreated
        ogd->members = new_members;
        m_rep_mgr->build_iogr(primary_loc.in(), *ogd);
        
        // The ogd has been modified, set the 'modify' flag in current
        m_rep_mgr->modified_ogd(m_ogid);
    }



    /**
     *
     */
    void ObjectGroup::set_primary(const ::FT::Location& primary_loc)
        throw(::FT::PrimaryNotSet
              , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        ::CdmwFT::ObjectGroupData* ogd;
        try {
            ogd = m_rep_mgr->get_current_ogd(m_ogid);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(), "ObjectGroup::remove_member():"
                       << "ObjectGroupData " << m_ogid << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        m_rep_mgr->build_iogr(primary_loc, *ogd);
        
        // The ogd has been modified, set the 'modify' flag in current
        m_rep_mgr->modified_ogd(m_ogid);
    }
    
        

    /**
     * Appends a StatefullGroupInfo build from the data that  are
     * specific to the current object group to the sequence passed
     * as an inout parameter.
     *
     * @param object_group_id
     *
     * @return A pointer to a group_info structure 
     *
     * @exception CORBA::NO_MEMORY Failure to allocate memory.
     */
    void
    ObjectGroup::update_statefull_infos (
            ::CdmwFT::Location::StatefullGroupInfos& group_infos)
        throw (::CORBA::SystemException)
    // #### FIXME: more exceptions ????
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        ::CdmwFT::ObjectGroupData* ogd;
        try {
            ogd = m_rep_mgr->get_current_ogd(m_ogid);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(), "ObjectGroup::update_statefull_infos():"
                       << "ObjectGroupData " << m_ogid << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        // 1. Add one slot in the out sequence for the new infos
        ::CORBA::ULong infos_idx = group_infos.length();
        group_infos.length(infos_idx+1);

        // 1. Get and save the GroupInfo
        group_infos[infos_idx].group_info = *get_object_group_info();
        
        // 2. Get the DataStoreInfo items matching the ID used by this group

        // 2-1. set the StatefullGroupInfo::data_store_infos sequence length
        CORBA::ULong members_list_len = ogd->members.length();
        group_infos[infos_idx].data_store_infos.length(members_list_len);
        // 2-2. for each group member location
        for (CORBA::ULong m=0; m<members_list_len; m++) {
          
            // 2-2-1. extract the LocalDataStoreInfos from the location data store
            std::string s_the_location;
            try {
                s_the_location = 
                    m_rep_mgr->ftloc_to_string(ogd->members[m].the_location);
            }
            catch(const ::CORBA::INTERNAL&) { 
                CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::update_statefull_infos(): "
                           << ogd->members[m].the_location << ")': Malformed location.\n"
                           << "Raising CORBA::INTERNAL() exception." );
                CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALLocationCtxtNotFound
                                    , CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            
            ::CdmwFT::LocationInfo_var locinfo = 
                    m_rep_mgr->get_location_info (s_the_location);
            if (! locinfo) { 
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::update_statefull_infos():"
                       << " LocationInfo \"" << ogd->members[m].the_location
                       << "\" has no MembersAdmin set\n"
                       << "Raising CORBA::INTERNAL exception." );
                CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALLocationCtxtNotFound
                                    , CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }

            if (!locinfo->has_members_admin) { 
                CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::update_statefull_infos():"
                           << " LocationInfo \"" << ogd->members[m].the_location
                           << "\" has no MembersAdmin set\n"
                           << "Raising CORBA::INTERNAL exception.");
                CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALIncompleteLocationInfo
                                    , CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            if ( locinfo->members_admin_info._d() 
               != ::CdmwFT::LocationStateness_STATEFULL) {
                CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::update_statefull_infos():"
                           << " LocationInfo \"" << ogd->members[m].the_location
                           << "\" is not STATEFULL\n"
                           << "Raising CORBA::INTERNAL exception." );
                CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALUnexpectedLocInfoState
                                    , CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            
            const CdmwFT::StateTransfer::LocalDataStoreInfos& local_dsinfos
                = locinfo->members_admin_info.statefull().local_datastore_infos;

            // 2-2-2. deep-copy the Location
            group_infos[infos_idx].data_store_infos[m].the_location
                = ogd->members[m].the_location;

            // 2-2-3. extract group LocalDataStoreInfos
            CdmwFT::StateTransfer::LocalDataStoreInfos_var grp_local_dsinfos
                = get_local_data_store_infos (local_dsinfos);

            CORBA::ULong max = grp_local_dsinfos->maximum();
            CORBA::ULong len = grp_local_dsinfos->length();
            CdmwFT::StateTransfer::LocalDataStoreInfo* buf 
                =  grp_local_dsinfos->get_buffer(true);
            // NOTE: using replace to avoid operator= deep copy
            group_infos[infos_idx].data_store_infos[m].local_data_store_infos.replace
                                                                (max, len, buf, true);
        }
    }


    /**
     * Extracts a GroupInfo structure from the current ObjectGroup
     *
     * @return A pointer to a group_info structure 
     *
     * @exception CORBA::NO_MEMORY Failure to allocate memory.
     */
    ::CdmwFT::GroupInfo*
    ObjectGroup::get_object_group_info ()
        throw ( ::CORBA::NO_MEMORY
              , ::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        ::CdmwFT::ObjectGroupData* ogd;
        try {
            ogd = m_rep_mgr->get_current_ogd(m_ogid);
        } catch (const ::CORBA::INTERNAL &) { 
            CDMW_ERROR(FTLogger::GetLogger(), "ObjectGroup::get_object_group_info():"
                       << "ObjectGroupData " << m_ogid << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        return get_object_group_info(*ogd);
    }
        

    /**
     * Static method: Extracts a GroupInfo structure from the ObjectGroupData
     * instance passed as an in parameter.
     *
     * @return A pointer to a group_info structure 
     *
     * @exception CORBA::NO_MEMORY Failure to allocate memory.
     */
    ::CdmwFT::GroupInfo*
    ObjectGroup::get_object_group_info (const ::CdmwFT::ObjectGroupData& ogd)
        throw (::CORBA::NO_MEMORY)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        ::CdmwFT::GroupInfo_var info;
        try {
            info = new ::CdmwFT::GroupInfo();
            info->object_group_ref = ogd.object_group_ref;
            info->object_group_id  = ogd.group_id;
            info->object_group_ref_version = ogd.object_group_ref_version;

  		      info->members.length(ogd.members.length());
  		      for (CORBA::ULong i=0; i<ogd.members.length(); i++) {
  			        info->members[i].the_reference = ogd.members[i].the_reference;
  			  	    info->members[i].the_location  = ogd.members[i].the_location;
  		      }
        } catch (const std::bad_alloc & ) {
            CORBA::NO_MEMORY ex(::Cdmw::OrbSupport::NO_MEMORY,
                                CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        return info._retn();
    }

        
    /**
     * Builds a LocalDataStoreInfo sequence containing only the items with a
     * DataStoreID that matches one of the IDs passed at create_object time.
     *
     * @param ogd The object group data object holding all known
     * data concering an object group.
     *
     * @param local_dsinfos The LocalDataStoreInfo sequence associated to the
     * location (passed at register_statefull_location time).
     *
     * @return A pointer to a LocalDataStoreInfos sequence 
     *
     * @exception CORBA::NO_MEMORY Failure to allocate memory.
     */
    CdmwFT::StateTransfer::LocalDataStoreInfos *
    ObjectGroup::get_local_data_store_infos (
            const CdmwFT::StateTransfer::LocalDataStoreInfos & local_dsinfos
          ) const
        throw (CORBA::NO_MEMORY)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        ::CdmwFT::ObjectGroupData* ogd;
        try {
            ogd = m_rep_mgr->get_current_ogd(m_ogid);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(),"ObjectGroup::get_local_data_store_infos():"
                       << "ObjectGroupData " << m_ogid << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        ::CORBA::ULong memb_dsid_len = ogd->data_store_ids.length();
        ::CdmwFT::StateTransfer::LocalDataStoreInfos_var ret;
        CORBA::ULong info_count = 0;
        ret = new ::CdmwFT::StateTransfer::LocalDataStoreInfos();

        for (CORBA::ULong i = 0; i < local_dsinfos.length(); i++)
        {
            CORBA::ULong id_count = 0;
            ::CdmwFT::StateTransfer::DataStoreIDs ids;

            // find intersection of ids
            for (CORBA::ULong j = 0; j < local_dsinfos[i].dsids.length(); j++)
            {
                for (CORBA::ULong k = 0; k < memb_dsid_len; k++)
                {
                    if (local_dsinfos[i].dsids[j] == ogd->data_store_ids[k])
                    {
                        ids.length(id_count + 1);
                        ids[id_count++] = ogd->data_store_ids[k];
                    }
                }
            }

            if (id_count > 0)
            {
                ret->length(info_count + 1);
                (*ret)[info_count] = local_dsinfos[i];
                (*ret)[info_count].dsids = ids;
                info_count++;
            }
        }

        return ret._retn();
    }

    /*
     * Return the list of the replicated Locations containing this group.
     *
     * @param running indicates wether a member is present on a location
     */
    void
    ObjectGroup::get_peer_locations (std::list < Location::Location* > & peers_list, bool running)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"running="<<running);
        InfosMap::iterator iter;
        if (!running)
        {
            for (iter = m_infos.begin(); iter != m_infos.end(); ++iter) {
                    Location::Location* curloc = (*iter).second.loc;
                    peers_list.push_back(curloc);
            }
        } else {
            for (iter = m_infos.begin(); iter != m_infos.end(); ++iter) {
                if (((*iter).second.obj != NULL)) {
                    Location::Location* curloc = (*iter).second.loc;
                    peers_list.push_back(curloc);
                }
            }        
        }
    }
    

}; // namespace ObjectGroup
}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

