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

#include "ftreplicationmanager/Location.hpp"
#include "ftreplicationmanager/ObjectGroup.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <FaultTolerance/ftcommon/FTLogger.hpp>
#include <Foundation/logging/FunctionLogger.hpp>

namespace Cdmw {
namespace  FT {
namespace ReplicationManager {
namespace Location {

    Location::Location( const std::string &           name
                      , ReplicationManagerDelegate*   rep_mgr)
    : _fsm(*this)
    , m_members_count(0)
    , m_exp_add_member(0)
    , m_is_backup(true)
    , m_stateness(::CdmwFT::LocationStateness_STATELESS)
    , m_fsm_state(::CdmwFT::LocationState_INITIAL)
    , m_locname(name)
    , m_rep_mgr(rep_mgr)
    , m_first_backup(false)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"name="<<name);
        // Uncomment to see debug output.
        //_fsm.setDebugFlag(true);
        
        // Saving the "private" SMC id in order to be able to force
        // _fsm to the INITIAL state when 'reset()' is called. 
        m_fsm_initial_state = _fsm.getState().getId();
    }

    Location::~Location ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        remove_from_container();
    }


    void 
    Location::ev_create_object()
        throw( ::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            _fsm.ev_create_object();
        } catch (::statemap::StateUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedState
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::TransitionUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedTransition
                                 , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::IndexOutOfBoundsException) {
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

    void  
    Location::ev_set_backup()
        throw( ::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            _fsm.ev_set_backup();
        } catch (::statemap::StateUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedState
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::TransitionUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedTransition
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::IndexOutOfBoundsException) {
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

    void  
    Location::ev_add_member(
               ObjectGroup::ObjectGroup&       grpctxt
             , Object::Object&                 objctxt
             , ::CORBA::Object_ptr             member
             , const ::FT::Location&           primary_loc)
        throw( ::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            _fsm.ev_add_member(grpctxt, objctxt, member, primary_loc);
        } catch (::statemap::StateUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedState
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::TransitionUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedTransition
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::IndexOutOfBoundsException) {
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

    void  
    Location::ev_set_primary()
        throw( ::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            _fsm.ev_set_primary();
        } catch (::statemap::StateUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedState
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::TransitionUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedTransition
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::IndexOutOfBoundsException) {
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

    void  
    Location::ev_failure()
        throw( ::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            _fsm.ev_failure();
        } catch (::statemap::StateUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedState
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::TransitionUndefinedException) {
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALSMUndefinedTransition
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (::statemap::IndexOutOfBoundsException) {
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

    void  
    Location::failure()
        throw( ::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        // #### TODO
    }

    /*
     * Removes the pair (this->get_key, this) from the ContextContainer.
     */
    bool
    Location::remove_from_container ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        return m_rep_mgr->remove_locctxt_from_container(m_locname);
    }

    /*
     * Silently overide possibly existing value or create a new map item.
     */
    void 
    Location::associate_object_group (
            ::FT::ObjectGroupId             ogid
          , ObjectGroup::ObjectGroup&       objgrp)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        m_infos[ogid].grp = &objgrp;
    }
    
    /*
     * If the Info (grp, obj) contains a non null obj pointer, simply
     * overide grp with NULL, else removes (grp, obj) from the map.
     */
    bool 
    Location::disassociate_object_group (::FT::ObjectGroupId ogid)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        InfosMap::iterator pos;
        pos = m_infos.find(ogid);
        bool result=false;
        if (pos != m_infos.end()) {
            if (m_infos[ogid].obj == NULL) m_infos.erase(pos);
            else                           m_infos[ogid].grp = NULL;
            result=true;
        }
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }
    

    /*
     * Silently overide possibly existing value or create a new map item.
     */
    void 
    Location::associate_object (
            ::FT::ObjectGroupId             ogid
          , Object::Object&                 object)
        throw(::CORBA::BAD_INV_ORDER)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        m_infos[ogid].obj = &object;
    }
    

    /*
     * If the pair (grp, obj) contains a non null grp pointer, simply
     * overide obj with NULL, else removes the pair from the map.
     */
    bool 
    Location::disassociate_object (::FT::ObjectGroupId ogid)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        InfosMap::iterator pos;
        pos = m_infos.find(ogid);
        bool result=false;
        
        if (pos != m_infos.end()) {
            if (m_infos[ogid].grp == NULL) 
            {
                m_infos.erase(pos);
            }
            else {
                m_infos[ogid].obj = NULL;
            }
            result=true;
        }
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }
    

    /*
     * Disassociate the current Location from all its peer ObjectGroups
     * and Objects
     */
    void 
    Location::disassociate ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        for ( InfosMap::iterator pos = m_infos.begin()
            ; pos != m_infos.end()
            ; ++pos) {
            if (pos->second.grp != NULL)
                pos->second.grp->disassociate_location(m_locname);
            if (pos->second.obj != NULL)
                pos->second.obj->clear_parent_location();
        }
    }


    /*
     * Returns the pointer to the ObjectGroup with id 'ogid' or NULL
     * if this ogid doesn't match any in the internal map.
     */
    ObjectGroup::ObjectGroup* 
    Location::get_grpctxt (::FT::ObjectGroupId ogid)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        InfosMap::iterator pos;
        pos = m_infos.find(ogid);
        if (pos == m_infos.end()) return NULL;
        else                      return pos->second.grp;
    }
    

    /*
     * Returns the pointer to the ObjectGroup with id 'ogid' or NULL
     * if this ogid doesn't match any in the internal map.
     */
    Object::Object* 
    Location::get_objctxt (::FT::ObjectGroupId ogid)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        InfosMap::iterator pos;
        pos = m_infos.find(ogid);
        if (pos == m_infos.end()) return NULL;
        else                      return pos->second.obj;
    }
    

    /*
     * Clears the internal map and forces the SMC machine to INITIAL state.
     * Location name and parent replication manager are not modified. This
     * method sets the Location to the state it would have after a call to
     * the only valid Ctor.
     */
    void 
    Location::reset ()
    {

        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        int nb_create_object = m_members_count + m_exp_add_member;
        // Disassociate all objects
        disassociate();
        
        // Clear FSM counters
        m_members_count  = 0;
        m_exp_add_member = nb_create_object;
        m_is_backup = true;
        m_fsm_state = ::CdmwFT::LocationState_INITIAL;
        
        // Forces the actual state machine state to INITIAL
        _fsm.setState(_fsm.valueOf(m_fsm_initial_state));
    }


    /*
     * Called within an add_member transition action when the state  
     * machine is either in INSERTING_PRIMARY or PRIMARY state.
     */
    void
    Location::add_primary(
                   ObjectGroup::ObjectGroup&       grpctxt
                 , Object::Object&                 objctxt
                 , ::CORBA::Object_ptr             member
                 , const ::FT::Location&           primary_loc)
        throw( CdmwFT::Location::InitFailed
             , CdmwFT::Location::AlreadyDone
             , CdmwFT::Location::UnknownDataStoreID
             , CdmwFT::StateTransfer::InvalidDataStoreState
             , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        associate_object(grpctxt.ogid(), objctxt);
        try {
            grpctxt.add_primary(this, &objctxt, member);

            // Update the IOGR with the new member profile
            // #### FIXME: should be in ObjectGroup::add_primary
            CdmwFT::ObjectGroupData* ogd = m_rep_mgr->get_current_ogd(grpctxt.ogid());
            if (! ogd) {
                CDMW_ERROR(FTLogger::GetLogger(),"Location::add_primary():"
                           << "ObjectGroupData " << grpctxt.ogid() << " not found \n"
                           << "Raising CORBA::INTERNAL exception." );
                CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjectGroupDataNotFound
                                    , CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            m_rep_mgr->build_iogr(primary_loc, *ogd);

            // If the Location is STATEFULL, initialize the primary member's data stores
            if (m_stateness == CdmwFT::LocationStateness_STATEFULL) {

                ::CdmwFT::LocationInfo* locinfo;
                try {
                    locinfo = m_rep_mgr->get_current_loc(m_locname);
                } catch (const ::CORBA::INTERNAL &) {
                    CDMW_ERROR(FTLogger::GetLogger(),"Location::add_primary():"
                               << "LocationInfo " << m_locname << " not found \n"
                               << "Raising CORBA::INTERNAL exception." );
                    CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }

                const ::CdmwFT::StatefullMembersAdminInfo& 
                    info = locinfo->members_admin_info.statefull();
                CdmwFT::StateTransfer::LocalDataStoreInfos_var grp_local_dsinfos
                    = grpctxt.get_local_data_store_infos (info.local_datastore_infos);
                info.admin->init_primary_data_stores (grp_local_dsinfos);
            } 
            // Initialization OK, send the primary_ready to the ObjectGroup. This
            // will switch the ObjectGroup to LOST_REDUNDANCY, DEGARDED_REDUNDANCY
            // or FULLY_REDUNDANT state and the Object to the PRIMARY state.
            grpctxt.ev_primary_ready(objctxt);

            m_rep_mgr->modified_ogd(grpctxt.ogid());
            m_rep_mgr->modified_loc(m_locname);
            
        } catch(const CdmwFT::Location::InitFailed& ex) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::add_primary(" << m_locname << ")': InitFailed : "<< ex.reason <<"\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL e(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw e;
        }
        catch(const CdmwFT::Location::AlreadyDone&) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::add_primary(" << m_locname << ")': AlreadyDone : DataStore are already initialised\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL e(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw e;
        }
        catch(const CdmwFT::Location::UnknownDataStoreID&) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::add_primary(" << m_locname << ")': UnknownDataStoreID : DataStore ID is unknown\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL e(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw e;
        }
        catch(const CdmwFT::StateTransfer::InvalidDataStoreState&) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::add_primary(" << m_locname << ")': InvalidDataStoreState\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL e(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw e;
        }
    }

    /*
     * Called within an add_member transition action when the state  
     * machine is either in INSERTING_BACKUP or BACKUP state.
     */
    void
    Location::add_backup (
                   ObjectGroup::ObjectGroup&       grpctxt
                 , Object::Object&                 objctxt
                 , ::CORBA::Object_ptr             member
                 , const ::FT::Location&           primary_loc)
        throw( ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        associate_object(grpctxt.ogid(), objctxt);
        try {
            grpctxt.add_backup(this, &objctxt, member);

            // Update the IOGR with the new member profile
            // #### FIXME: should be in ObjectGroup::add_backup
            CdmwFT::ObjectGroupData* ogd = m_rep_mgr->get_current_ogd(grpctxt.ogid());
            if (! ogd) {
                CDMW_ERROR(FTLogger::GetLogger(),"Location::add_backup():"
                           << "ObjectGroupData " << grpctxt.ogid() << " not found \n"
                           << "Raising CORBA::INTERNAL exception." );
                throw CORBA::INTERNAL( ::Cdmw::OrbSupport::INTERNALObjectGroupDataNotFound
                                     , CORBA::COMPLETED_NO);
            }
            m_rep_mgr->build_iogr(primary_loc, *ogd);

            // If the Location is STATEFULL, initialize the backup member's data stores
            if (m_stateness == CdmwFT::LocationStateness_STATEFULL) {
                
                // We need the primary location local data stores infos
                std::string             primary_locname;
                ::CdmwFT::LocationInfo* primary_locinfo;
                try {
                    primary_locname = m_rep_mgr->ftloc_to_string(primary_loc);
                } catch (const ::CORBA::INTERNAL &) {
                    CDMW_ERROR(FTLogger::GetLogger(),"Location::add_backup():"
                               << "LocationInfo " << primary_loc << " not found \n"
                               << "Raising CORBA::BAD_PARAM exception." );
                    throw ::CORBA::BAD_PARAM(::Cdmw::OrbSupport::BAD_PARAMInvalidLocation
                                         , CORBA::COMPLETED_NO);
                }

                try {
                    primary_locinfo = m_rep_mgr->get_current_loc(primary_locname);
                } catch (const ::CORBA::INTERNAL &) {
                    CDMW_ERROR(FTLogger::GetLogger(),"Location::add_backup():"
                               << "LocationInfo " << primary_loc << " not found \n"
                               << "Raising CORBA::INTERNAL exception." );
                    throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                }

                // Get the current LocationInfo
                ::CdmwFT::LocationInfo* current_locinfo;
                try {
                    current_locinfo = m_rep_mgr->get_current_loc(m_locname);
                } catch (const ::CORBA::INTERNAL &) {
                    CDMW_ERROR(FTLogger::GetLogger(),"Location::add_backup():"
                               << "LocationInfo " << m_locname << " not found \n"
                               << "Raising CORBA::INTERNAL exception." );
                    throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                }

                // Request the statefull admin to initialize the backup DataStores 
                // from the primary DataStores
//                std::cerr << "*** before statefull" << std::endl;
                const ::CdmwFT::StatefullMembersAdminInfo& 
                    info = current_locinfo->members_admin_info.statefull();
//                std::cerr << "*** before get_local_data_store_infos" << std::endl;
                CdmwFT::StateTransfer::DataStoreInfo primary_dsi;
                primary_dsi.the_location = primary_loc;
                CdmwFT::StateTransfer::LocalDataStoreInfos_var local_dsinfos
                     = grpctxt.get_local_data_store_infos 
                          (primary_locinfo->members_admin_info.statefull().local_datastore_infos);
                primary_dsi.local_data_store_infos = local_dsinfos;
//                std::cerr << "*** before init_backup_data_stores info.admin = " << info.admin << std::endl;
                info.admin->init_backup_data_stores (primary_dsi);
//                std::cerr << "*** after init_backup_data_stores" << std::endl;
            }
            grpctxt.ev_backup_ready(objctxt);
            m_rep_mgr->modified_ogd(grpctxt.ogid());
            m_rep_mgr->modified_loc(m_locname);
            
        } catch (const ::CORBA::INTERNAL& ex) {
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        } catch (const ::CORBA::BAD_PARAM& ex) {
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
    }


    /*
     * 
     */
    ::FT::ObjectGroup_ptr
    Location::remove_member (::FT::ObjectGroupId  ogid)
        throw( ::FT::ObjectGroupNotFound
             , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        // Because the FT is now handled at the process level (Location),
        // a remove_member ends up in removing *all* members of the owner
        // Location and then switch this Location to INITIAL.

        InfosMap::iterator iter = m_infos.find(ogid);
        if (iter == m_infos.end()) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::remove_member():"
                       << "ObjectGroupData " << ogid << " not found \n"
                       << "Raising ::FT::ObjectGroupNotFound exception." );
            ::FT::ObjectGroupNotFound ex; 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;           
        }

        // Get object group's known data: may raise ObjectGroupNotFound.
        ::CdmwFT::ObjectGroupData* ogd;
        try {
            ogd = m_rep_mgr->get_current_ogd(ogid);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::remove_member():"
                       << "ObjectGroupData " << ogid << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALObjGroupCtxtNotFound
                                , CORBA::COMPLETED_NO); 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        /*
         * Ask each group to remove its member
         */
        for (iter = m_infos.begin(); iter != m_infos.end(); ++iter) {
            /*
             * Asks the owner group to remove the member
             * - Removes the member MemberOverallInfo from the ObjectGroupData
             * - Erases the ::FT::Location in the ObjectGroupData if the member
             *   belongs to a PRIMARY Location (ie is PRIMARY itself)
             * - Disassociate the member from the ObjectGroup
             * - Updates the IOGR
             */
            Object::Object*           member = (*iter).second.obj;
            if (member) {
                ObjectGroup::ObjectGroup* objgrp = (*iter).second.grp;
                objgrp->remove_member(member, is_primary());
                (*iter).second.obj = NULL;
            }
            
        }
        
        /*
         * Return the internal SMC machine to the INITIAL state
         */
        //reset();
        m_rep_mgr->modified_ogd(ogid);

        // #### FIXME: Issue a warning if the number of replicas falls bellow
        // the minimum number of replicas

        return ::FT::ObjectGroup::_duplicate(ogd->object_group_ref.in());
    }
    
    
    /*
     * Called back when 'ev_set_primary' event is issued on a BACKUP Location.
     * It calls update_primary and switches location's objects' state machines
     * to PRIMARY state.
     */
    void
    Location::set_primary()
      throw (CORBA::INTERNAL
	     , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        ::FT::Location_var ftloc;
        try {
            ftloc = m_rep_mgr->string_to_ftloc(m_locname);
        } catch (const ::CORBA::INTERNAL & ex) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::set_primary(" << m_locname 
                       << "): Invalid location name \"" << m_locname << "\"\n"
                       << "Raising CORBA::INTERNAL exception." );
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }


        InfosMap::iterator iter;
        for (iter = m_infos.begin(); iter != m_infos.end(); ++iter) {
            if (iter->second.obj != NULL)
            {
                iter->second.obj->ev_set_primary();
               iter->second.grp->set_primary(ftloc.in());
            }
        }
        update_primary(ftloc.in());
    }


    /*
     * Switches all the members to BACKUP
     */
    void
    Location::set_backup()
        throw( ::CORBA::INTERNAL
	     , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        // #### FIXME: TODO
    }


    /*
     * Called by the replication manager on each location belonging to the same 
     * group of replicated location when one of these locations  is about to switch
     * to PRIMARY state.
     * Actual action Depends on the Location stateness:
     * LOCATION IS STATELESS:
     * - Calls ObjectGroup::update_primary on each group member of the current
     *   location (ObjectGroup::update_primary in turn calls update_primary on
     *   the location members admin).
     * LOCATION IS STATEFULL:
     * - Each group member of the location is requested to update an instance
     *   of a StatefullGroupInfos (ObjectGroup::update_statefull_infos),
     * - update_statefull_primary is called on the statefull members admin.
     */
    void
    Location::update_primary (const ::FT::Location& primary_loc)
        throw(CORBA::INTERNAL
	     , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        using namespace Cdmw::CommonSvcs::Naming;

        ::CdmwFT::LocationInfo* locinfo;
        try {
            locinfo = m_rep_mgr->get_current_loc(m_locname);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::update_primary():"
                       << "LocationInfo " << m_locname << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALLocationCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        // reset the first backup flag
        locinfo->first_backup = false;
        m_first_backup = false;
        m_rep_mgr->modified_loc(m_locname);

        if (! locinfo->has_members_admin) {
            // Silently returns. The LocationInfo may be incomplete if it
            // has not yet been registered and created when create_object
            // was called.
            CDMW_LOG_FUNCTION_RETURN("");
            return;
        }
  
        if (!(fsm_state() == ::CdmwFT::LocationState_PRIMARY) &&
            !(fsm_state() == ::CdmwFT::LocationState_BACKUP))
        {
            // if the location is not in the PRIMARY and BACKUP, do nothing
            CDMW_LOG_FUNCTION_RETURN("");
            return;
        }

        if (m_stateness == ::CdmwFT::LocationStateness_STATELESS) {
            
            // ==== PROCESSING THE STATELESS LOCATION ====
            
            ::CdmwFT::Location::MembersAdmin_ptr members_admin =
                locinfo->members_admin_info.stateless().admin.in();
            ::CdmwFT::Location::PrimaryBackupAdmin_var stateless_admin =
                ::CdmwFT::Location::PrimaryBackupAdmin::_narrow(members_admin);
            if (CORBA::is_nil(stateless_admin.in())) {
                CDMW_ERROR(FTLogger::GetLogger(),"Location::update_primary():"
                           << "Couldn't narrow the MembersAdmin to a PrimaryBackupAdmin\n"
                           << "Raising CORBA::INTERNAL exception.");
                CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }

            // For each group of this location, get the GroupInfo and call  
            // update_primary on the Location MembersAdmin
            for ( InfosMap::iterator iter = m_infos.begin()
                ; iter != m_infos.end()
                ; ++iter
                ) {
               ::CdmwFT::GroupInfo_var grp_info
                    = iter->second.grp->get_object_group_info();
                stateless_admin->update_primary (grp_info.in(), primary_loc);
            }

        } else {

           // ==== PROCESSING THE STATEFULL LOCATION ====

            // Requests each group of this location to fill a statefull infos 
            // with its specific data.
            ::CdmwFT::Location::StatefullGroupInfos statefull_infos;
            statefull_infos.length(0L);
            for ( InfosMap::iterator iter = m_infos.begin()
                      ; iter != m_infos.end()
                      ; ++iter
                ) {
                iter->second.grp->update_statefull_infos (statefull_infos);          
            }

            ::CdmwFT::Location::StatefullPrimaryBackupAdmin_ptr
                  statefull_admin = locinfo->members_admin_info.statefull().admin.in();
            try
            {
                statefull_admin->update_statefull_primary(statefull_infos, primary_loc);
            }
            catch(const CORBA::COMM_FAILURE& ex)
            {
                CDMW_INFO(FTLogger::GetLogger(),"Location::update_primary():"
                          << "LocationInfo " << m_locname << "\n"
                          << "*************************************\n"
                          << "Exception "<<ex<<"\n"
                          << "*************************************");
            }
            catch(const CORBA::TRANSIENT& ex)
            {
                // FIXME: This is not safe!
                CDMW_FATAL(FTLogger::GetLogger(),"Location::update_primary():"
                          << "LocationInfo " << m_locname << "\n"
                          << "*************************************\n"
                          << "Exception "<<ex<<"\n"
                          << "*************************************");
            }
            catch(const CORBA::Exception& ex)
            {
                CDMW_FATAL(FTLogger::GetLogger(),"Location::update_primary():"
                          << "LocationInfo " << m_locname << "\n"
                          << "*************************************\n"
                          << "Exception "<<ex<<"\n"
                          << "*************************************");
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            }
            
        }
    }


    /*
     * Called by a backup location when it is about to switch to BACKUP state.
     * 
     * Pseudo code: based on the hypothesis that Locations are truly replicated
     * processes, which implies that all groups share the same list of locations.
     *
     * FOR EACH group belonging to the current Location
     * - IF STATELESS 
     *   - FOR EACH member of the group
     *     - append the GroupInfo to a local GroupInfo sequence
     *     END FOR EACH
     *   END IF
     * - IF STATEFULL
     *   - build and append the StatefullGroupInfo to a local StatefullGroupInfo
     *     sequence
     *   END IF
     * FOR EACH Locations (we arbitrarily use the Locations list embedded in the
     * first group of the current Location).
     * - IF STATELESS 
     *   - call MembersAdmin::update_group_view passing each GroupInfo previously
     *     saved in the local GroupInfo sequence
     *   END IF
     * - IF STATEFULL
     *   - call PrimaryBackupStatefullAdmin::update_statefull_group_view passing
     *     StatefullGroupInfo sequence previously build
     *   END IF
     * END FOR EACH
     */
    void
    Location::update_all_group_views ()
      throw (CORBA::INTERNAL
	     , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        ::CdmwFT::LocationInfo* locinfo;
        try {
            locinfo = m_rep_mgr->get_current_loc(m_locname);
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::update_all_group_views():"
                       << "LocationInfo " << m_locname << " not found \n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        if (! locinfo->has_members_admin) {
            // Silently returns. The LocationInfo may be incomplete if it
            // has not yet been registered and created when create_object
            // was called.
            CDMW_LOG_FUNCTION_RETURN("");
            return;
        }

       if (!(fsm_state() == ::CdmwFT::LocationState_PRIMARY) &&
           !(fsm_state() == ::CdmwFT::LocationState_BACKUP))
       {
           // if the location is not in the PRIMARY and BACKUP, do nothing
           CDMW_LOG_FUNCTION_RETURN("");
           return;
       }


        ::CdmwFT::Location::StatefullGroupInfos_var full_inf = new ::CdmwFT::Location::StatefullGroupInfos() ;
        full_inf->length(0L);
        
        ::CdmwFT::GroupInfo_var grp_info;
        for ( InfosMap::iterator iter = m_infos.begin()
            ; iter != m_infos.end()
            ; ++iter
            ) {

            ObjectGroup::ObjectGroup* grpctxt = iter->second.grp;
            // #### TODO: ???? bool result = true;
            try {
                if (m_stateness == ::CdmwFT::LocationStateness_STATELESS) {
//                    std::cout<<m_locname<<" m_stateness = STATELESS"<<std::endl;
                    grp_info = grpctxt->get_object_group_info();
                    CdmwFT::Location::MembersAdmin_ptr admin
                        = locinfo->members_admin_info.stateless().admin.in();
                    try { 
                        admin->update_group_view(grp_info.in());

                    } catch (const CORBA::INTERNAL &ex) {
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw;
                    } catch (...) {
                        // TODO: Trigger a reconfiguration? (check that member is non faulty?)
                        // #### TODO: ???? result = false;
                        CDMW_ERROR(FTLogger::GetLogger(),"Location::update_all_group_views():"
                                   << "LocationInfo " << m_locname );
                    }
                }
                else {
//                    std::cout<<m_locname<<" m_stateness = STATEFULL"<<std::endl;
                    grpctxt->update_statefull_infos (full_inf.inout());
                }
            } catch (const CORBA::INTERNAL & ex) {
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw;
            } catch (...) {
                // #### TODO: ???? result = false;
                CDMW_ERROR(FTLogger::GetLogger(),"Location::update_all_group_views():"
                           << "LocationInfo " << m_locname );
            }
        }

        if (m_stateness == ::CdmwFT::LocationStateness_STATEFULL) {
           CdmwFT::Location::StatefullPrimaryBackupAdmin_ptr admin
                = locinfo->members_admin_info.statefull().admin.in();
            admin->update_statefull_group_view(full_inf.in());
        }

    }


    /*
     *
     */
    // #### FIXME: this may not be useful anymore with the CurrentData mechanism
    void
    Location::get_members_ogd (std::list < ::CdmwFT::ObjectGroupData_var > & ogd_list)
        throw( ::CORBA::INTERNAL)
// #### TODO: more exceptions ????
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        InfosMap::iterator iter;
        for (iter = m_infos.begin(); iter != m_infos.end(); ++iter) {
            ::CdmwFT::ObjectGroupData* the_ogd
                = m_rep_mgr->get_current_ogd (iter->second.grp->ogid());
            ogd_list.push_back(the_ogd);
        }
    }


    /*
     *
     */
    void
    Location::remove_members ()
        throw( ::FT::MemberNotFound,
               ::CORBA::INTERNAL,
               ::CORBA::SystemException)
// #### TODO: more exceptions ????
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        // For each group of this location removes the member
        InfosMap::iterator iter;
        for (iter = m_infos.begin(); iter != m_infos.end(); ++iter) {
            // Clean the ogd with member related data
            if (iter->second.obj != NULL)
            {
                iter->second.grp->remove_member(iter->second.obj, is_primary());
                // Disassociate the member from the Location
                bool success = disassociate_object (iter->second.grp->ogid());
                if (! success) {
                    CDMW_ERROR(FTLogger::GetLogger(),"Location::remove_members():"
                               << "Did not find Location::Infos [" << iter->second.grp->ogid() << "].\n"
                               << "Raising CORBA::INTERNAL exception." );
                    CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
            }
        }
    }
    


    /*
     * Return the list of the replicated Locations *including* the caller.
     * It does not modify the list if it is called before a create_object
     * involving this Location has not yet been processed by the Rep.Mgr.
     */
    void
    Location::get_peer_locations (std::list < Location* > & peers_list)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        // If no associated group, return
        if (m_infos.size() == 0) return;

        // Find any group (the first one is a good one), and get  
        // the peers Locations from its own Location list
        ObjectGroup::ObjectGroup* objgrp = (*m_infos.begin()).second.grp;
        objgrp->get_peer_locations(peers_list, true);
    }


    /*
     * Returns the number of replicated Locations *including* 'this'.
     * belonging to the same Location group as 'this'.
     */
    unsigned
    Location::peer_locations_number ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        // If no associated group, return
        if (m_infos.size() == 0) return 0;

        // Find any group (the first one is a good one), and get  
        // the peers Locations from its own Location list
        ObjectGroup::ObjectGroup* objgrp = (*m_infos.begin()).second.grp;
        ObjectGroup::ObjectGroup::InfosMap& grpinfos = objgrp->get_infos();
        return grpinfos.size();
    }


    /*
     * Try to set the flag 'first_backup'.
     * If successfull, saves the new value in the DataStore and returns.
     * If unsuccessfull, throws a < to be defined > exception
     */
    void 
    Location::set_first_backup(bool first_backup_flag)
        throw ( ::CORBA::INTERNAL)
        // #### FIXME: define an adequate exception if the flag could not be set.
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        // If current and requested 'first backup' values are equal, silently returns 
        // #### FIXME: if a true flag is not 1 it may not work.
        if (m_first_backup == first_backup_flag)
            return;        
            
        // Actually set the Location as 'first backup'
        CdmwFT::LocationInfo* locinfo = m_rep_mgr->get_current_loc(m_locname);
        if (! locinfo) {
            CDMW_ERROR(FTLogger::GetLogger(),"Location::set_first_backup():"
                       << "LocationInfo \"" << m_locname << "\" not found.\n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALLocationInfoNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        // Save the new flag value in the data store and sets the modified flag
        locinfo->first_backup = first_backup_flag;
        m_first_backup = first_backup_flag;
        m_rep_mgr->modified_loc(m_locname);
    }

    /*
     * Called by the Location which receives the first add_member: tells
     * the peer Locations to insert their future members as BACKUP.
     */
    void 
    Location::set_peers_to_initial_backup()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
       /* This operation is not needed any more. It will be removed when the
        * location state machine will be updated.
        
        // If no associated group, return
        if (m_infos.size() == 0) return;
        
        // - Find any group (the first one is a good one), 
        // - Walk through the group Location list
        // - call 'ev_set_backup' on each Location except the current one. 
        ObjectGroup::ObjectGroup* objgrp = (*m_infos.begin()).second.grp;
        ObjectGroup::ObjectGroup::InfosMap& grpinfos = objgrp->get_infos();
        ObjectGroup::ObjectGroup::InfosMap::iterator iter;
        for (iter = grpinfos.begin(); iter != grpinfos.end(); ++iter) {
            Location* curloc = (*iter).second.loc;
            if (curloc != this)
                curloc->ev_set_backup();
        }
        */
    }


    /*
     * Returns the peer Location in the PRIMARY state, or NULL if
     * there's currently no PRIMARY Location, or if called before
     * a create_object involving this Location has not yet been
     * processed by the Rep.Mgr.
     */
    Location* 
    Location::get_peer_primary ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        // If no associated group, return
        if (m_infos.size() == 0) return NULL;

        // Find any group (the first one is a good one), and walk
        // through the group Location list
        ObjectGroup::ObjectGroup* objgrp = (*m_infos.begin()).second.grp;
        ObjectGroup::ObjectGroup::InfosMap& grpinfos = objgrp->get_infos();
        ObjectGroup::ObjectGroup::InfosMap::iterator iter;
        for (iter = grpinfos.begin(); iter != grpinfos.end(); ++iter) {
            Location* curloc = (*iter).second.loc;
            if (curloc->is_primary())
                return curloc;
        }
        return NULL; // Currently no PRIMARY Location
    }


}; // namespace Location
}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

