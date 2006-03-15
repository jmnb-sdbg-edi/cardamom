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


//#include "ftreplicationmanager/LocationInfoDataTraits.hpp"
#include "ftreplicationmanager/CurrentData.hpp"
#include "ftreplicationmanager/ReplicationManager_impl.hpp"
#include "ftreplicationmanager/FTUtils.hpp"
#include "ftreplicationmanager/FallbackOid.hpp"
#include "ftreplicationmanager/Fallback_impl.hpp"

#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/common/Assert.hpp>
#include <FaultTolerance/ftcommon/MacroDef.hpp>
#include <FaultTolerance/ftstatemanager/DataStoreContext.hpp>
#include <FaultTolerance/ftcommon/ReplyRecording.hpp>
#include <FaultTolerance/ftcommon/FTLogger.hpp>

#include <Foundation/logging/FunctionLogger.hpp>

#include <algorithm>
#include <sstream>

#if CDMW_ORB_VDR == tao
#include <FaultTolerance/ftcommon/TAO_IOGRFactory.hpp>

#else
#error "Unsupported ORB"
#endif

#define ENABLE_ACCEPT_DELETE_OBJECT_IF_EMPTY_GROUP  1


// TODO: inforcing minimum number of replicas
//       B) remove_member() and delete_object() may elect a new primary replica if needed.

#ifndef ENABLE_DEBUG_DUMP

#   define DEBUG_DUMP(comment,value_type) 
#   define DEBUG_DUMPALL(comment)

#else

#   define DEBUG_DUMP(comment,value_type) \
do {\
    if (FTLogger::GetLogger()->is_debug_enabled()) {\
        std::ostringstream oss__;\
        oss__ << #comment << std::endl;\
        Cdmw::FT::ObjectGroupDataPrinter print(oss__);\
        print(value_type);\
        CDMW_DEBUG(FTLogger::GetLogger(),oss__.str());\
    }\
} while(0)

#   define DEBUG_DUMPALL(comment) \
do {\
    if (FTLogger::GetLogger()->is_debug_enabled()) {\
        std::ostringstream oss__;\
        oss__ << #comment << std::endl;\
        m_ogdStorageHome->for_each(Cdmw::FT::ObjectGroupDataPrinter(oss__)); \
        CDMW_DEBUG(FTLogger::GetLogger(),oss__.str());\
    }\
} while(0)

#endif

std::ostream& operator<< (std::ostream & os, const ::FT::Location & ftloc)
{ // FIXME: Use NamingInterface or FTLocation classes
    for (CORBA::ULong i=0; i<ftloc.length(); i++) {
        os << ftloc[i].kind << "." << ftloc[i].id;
        if (i < (ftloc.length()-1)) os << "/";
    }
    return os;
}

namespace {

    const char* PRIMARY_LOCATION_PROPERTY_NAME = "com.thalesgroup.ft.PrimaryLocation";
    const char* OBJECT_GROUP_NAME = "com.thalesgroup.ft.Name";



    const int DEFAULT_PROPERTIES  = 0;
    const int TYPE_PROPERTIES     = 1;
    const int CREATION_PROPERTIES = 2;
    const int DYNAMIC_PROPERTIES  = 3;
    /*
     * Debug utility: shows the contexts associations and states
     */
    const char* sLocationStateString[]= {
        "INITIAL",
        "INSERTING_PRIMARY",
        "PRIMARY",
        "INSERTING_BACKUP",
        "BACKUP",
        "FAILED"
    };

    const char* sObjectGroupStateString[]= {
        "INITIAL", 
        "FAILED",            
        "LOST_REDUNDANCY", 
        "DEGRADED_REDUNDANCY",
        "FULLY_REDUNDANT"
    };

    const char* sMemberStateString[]= {
        "INITIAL", 
        "BACKUP", 
        "PRIMARY", 
        "FAILED"
  	};

    inline  const char* stateness_to_string(const CdmwFT::LocationStateness & s)
    {
        if (s == CdmwFT::LocationStateness_STATEFULL) return "STATEFULL";
        else return "STATELESS";
    }
    inline  const char* stateness_to_string(const CdmwFT::GroupStateness & s)
    {
        if (s == CdmwFT::GroupStateness_STATEFULL) return "STATEFULL";
        else return "STATELESS";
    }

    /**
     * Servant locator for the ObjectGroup Manager.
     */
    class ReplicationManagerLocator_impl
        : public virtual PortableServer::ServantLocator,
          public virtual Cdmw::OrbSupport::RefCountLocalObject
    {
        
    public:
        /**
         * Creates a servant locator.
         *
         * @param repl_mgr The reference to the replication manager.
         */
        ReplicationManagerLocator_impl(
            Cdmw::FT::ReplicationManager::ReplicationManager_impl * repl_mgr)
            : m_repl_mgr(repl_mgr)
        {            
            CDMW_ASSERT(repl_mgr != 0);
        }
        
        /**
         * Destroys a servant locator.
         */
        ~ReplicationManagerLocator_impl()
        {
            m_repl_mgr = 0;
        }

        /**
         * Returns a servant to dispatch the request to.
         *
         * @param oid The object id of the target object.
         * @param poa The reference to the POA itself.
         * @param operation The name of the operation being invoked.
         * @param the_cookie The cookie.
         *
         * @return A servant to dispatch to request to.
         */
        PortableServer::Servant 
        preinvoke(const PortableServer::ObjectId & oid,
                  PortableServer::POA_ptr poa,
                  const char * operation,
                  PortableServer::ServantLocator::Cookie & the_cookie)
            throw(CORBA::SystemException,
                  PortableServer::ForwardRequest)
        {
            // Should not be here!
            CDMW_NEVER_HERE();

            /*
            // Extract the object group id and the object group reference
            // version from the oid.
            // The oid pattern is : group_id/group_version.
            CORBA::String_var oid_string;
            try {
                oid_string = PortableServer::ObjectId_to_string(oid);
            } catch (const CORBA::BAD_PARAM &) {
                throw CORBA::OBJECT_NOT_EXIST();
            }
            std::istringstream is(oid_string.in());
            CORBA::ULong object_group_id, object_group_ref_version;
            char c; // for the '/' character.
            is >> object_group_id >> c >> object_group_ref_version;

            ::FT::ObjectGroupRefVersion current_version =
                m_repl_mgr->get_object_group_version_from_gid(object_group_id);

            if (object_group_ref_version == current_version) {
                throw CORBA::TRANSIENT(::Cdmw::OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
            } else {
                // FIXME: Potential concurrency problem! It's the POA that
                //        protects the ReplicationManager's data: accessing
                //        the servant directly will bypass POA protection!
                // Get the most recent object group reference.
                ::FT::ObjectGroup_var object_group =
                      m_repl_mgr->get_object_group_ref_from_gid(object_group_id);

                // Trigger LOCATION_FORWARD_PERM
                throw PortableServer::ForwardRequest(object_group.in()); // TODO: We need a PERM one!
                // TODO:
                //  May we check that the primary is ok and do a reconfiguration before
                //  replying with an up to date object group reference?
            }
            */
            return NULL;
        }
        
        /**
         * Allows servant cleanup.
         *
         * @param oid The object id of the target object.
         * @param poa The reference to the POA itself.
         * @param operation The name of the operation being invoked.
         * @param the_cookie The cookie.
         * @param the_servant The servant.
         */
        void 
        postinvoke(const PortableServer::ObjectId & oid,
                   PortableServer::POA_ptr poa,
                   const char * operation,
                   PortableServer::ServantLocator::Cookie the_cookie,
                   PortableServer::Servant the_servant)
            throw(CORBA::SystemException)
        {
            // Should not be here!
            CDMW_NEVER_HERE();
            // the_servant->_remove_ref();
        }
        
    private:
        Cdmw::FT::ReplicationManager::ReplicationManager_impl * m_repl_mgr;
    };
}; // anonymous namespace



namespace Cdmw {
namespace FT {
#if CDMW_ORB_VDR == tao

typedef  TAO_IOGRFactory UnderlyingORB;

#else
#error "Unsupported ORB"
#endif

namespace  {

    bool OGDTrueFunction(const std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                                          Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type > & elt)
    {
        return true;    
    }


    /**
     * PW: this function should really be located in the Repository CSCI.
     *
     * Compare two FT::Locations. Locations are assumed to be valid CosNaming::Names.
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
    Compare_Locations(const ::FT::Location& l1, const ::FT::Location& l2, ::CORBA::ULong depth = 0)
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
          if (strcmp(l1[i].id.in(), l2[i].id.in()) != 0)      return false;
          if (strcmp(l1[i].kind.in(), l2[i].kind.in()) != 0)  return false;
        }

        return true;
    }
           
}


class ObjectGroupDataPrinter
{
public:
    ObjectGroupDataPrinter(std::ostream & os) : m_os(os) { }         
    void operator()(const std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                                     Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type > & x)
    {
        m_os << "GROUP ID : " << x.first << "\n";

        m_os << "\tRepository Id : " << (x.second)->repository_id << "\n"
           << "\tGroup version : " << (x.second)->object_group_ref_version << "\n";

        // Get primary location
        ::FT::Location_var primary_loc =
          get_group_primary_location(x.second.in());
        using namespace Cdmw::CommonSvcs::Naming;
        if (primary_loc->length() > 0L) {
            std::string s_primary
                = NamingInterface::to_string(primary_loc.in());
            m_os << "\tPrimary location :'" << s_primary  << "'\n";
        } else 
            m_os << "\tThere is no primary location" << "\n";
        
        m_os << "\tGroup has " <<  (x.second)->members.length() << " member(s) \n";
        for (CORBA::ULong i = 0L; i < (x.second)->members.length(); ++i) {
            const std::string loc =
                NamingInterface::to_string((x.second)->members[i].the_location);
            m_os << "\t\tLocation : '" <<  loc << "'\n";
				/* FNO_JE added code
            m_os << "\t\t  with " << (x.second)->members[i].data_store_infos.length() << " DataStores:\n\t\t\t";
            for (CORBA::ULong j = 0L; j < (x.second)->members[i].data_store_infos.length(); ++j) {
                m_os << (x.second)->members[i].data_store_infos[j].dsid << " ";
            }
				*/
        }
        m_os << std::endl;
    }
    
private:
    ::FT::Location *
    get_group_primary_location(const ::CdmwFT::ObjectGroupData &x) const
        throw ()
    {
        // This may appear in dynamic_properties
        using namespace Cdmw::CommonSvcs::Naming;
        for (unsigned int i = 0; i<x.dynamic_properties.length(); i++)
        {
            std::string s_property
            = std::string(x.dynamic_properties[i].nam[0].id);
            const std::string property_name = PRIMARY_LOCATION_PROPERTY_NAME;
            
            if(s_property == property_name)
            {
                const ::FT::Location * val;
                // PW 10-04-2004: added any extraction result check
                if (x.dynamic_properties[i].val >>= val) 
                    return new ::FT::Location(*val);
                else
                    break; // Returns empty location
            }                
        }
        ::FT::Location_var no_location = new ::FT::Location();
        no_location->length(0L);
        return no_location._retn();
    }
    
    std::ostream & m_os;
};


class LocationInfosPrinter
{
public:
    LocationInfosPrinter(std::ostream & os) : m_os(os) { }         
    void operator()(const std::pair< Cdmw::OrbSupport::type_traits< std::string >::_var_type, 
                                     Cdmw::OrbSupport::type_traits< ::CdmwFT::LocationInfo >::_var_type > & x)
    {
        m_os << "Location : " << x.first << "\n";
        
        switch ((x.second)->state)
        {
            case CdmwFT::LocationState_INITIAL :
                m_os << "\tState : INITIAL\n";
                break;
                
            case CdmwFT::LocationState_INSERTING_PRIMARY :
                 m_os << "\tState : INSERTING_PRIMARY\n";
                break;
               
            case CdmwFT::LocationState_PRIMARY :
                m_os << "\tState : PRIMARY\n";
                break;
    
            case CdmwFT::LocationState_INSERTING_BACKUP :
                m_os << "\tState : INSERTING_BACKUP\n";
                break;

            case CdmwFT::LocationState_BACKUP :
                m_os << "\tState : BACKUP\n";
                break;
            case CdmwFT::LocationState_FAILED :
                m_os << "\tState : FAILED\n";
                break;

                
        };

        m_os <<"\tIs first baskup : ";
        if ((x.second)->first_backup)
            m_os<<"YES\n";
        else
            m_os<<"NO\n";
        
        m_os <<"\tMember admin was registered :";
        if((x.second)->has_members_admin)
            m_os<<"YES\n";
        else
            m_os<<"NO\n";
        m_os<<std::endl;
    }

private:    
    std::ostream & m_os;
};


namespace ReplicationManager {

    /**
     * Creates a replication manager.
     *
     * @param ft_domain_id The fault tolerance domain id.
     */
    ReplicationManager_impl::ReplicationManager_impl(
            int argc,
            char* argv[],
            CORBA::ORB_ptr               orb,
            PortableServer::POA_ptr      parent,
            const ::FT::FTDomainId &     ft_domain_id,
            const char*                  security_level,
            const char*                  replication_level,
            std::ostream &               os)
        throw(CORBA::SystemException)
        : m_orb(CORBA::ORB::_duplicate(orb)),
          m_parent_poa(PortableServer::POA::_duplicate(parent)),
          m_ft_domain_id(CORBA::string_dup(ft_domain_id)),
          m_fault_notifier(::FT::FaultNotifier::_nil()),
          m_fault_detector(CdmwFT::FaultManagement::FaultDetector::_nil()),
          m_ostream(os),
          m_security_level(security_level),
          m_replication_level(replication_level)
    {

        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"FT Domain=" << ft_domain_id);
        // The list of fault tolerance properties.
        m_ft_properties.push_back("org.omg.ft.ReplicationStyle");
        m_ft_properties.push_back("org.omg.ft.MembershipStyle");
        m_ft_properties.push_back("org.omg.ft.ConsistencyStyle");
        m_ft_properties.push_back("org.omg.ft.FaultMonitoringStyle");
        m_ft_properties.push_back("org.omg.ft.FaultMonitoringGranularity");
        m_ft_properties.push_back("org.omg.ft.Factories");
        m_ft_properties.push_back("org.omg.ft.InitialNumberReplicas");
        m_ft_properties.push_back("org.omg.ft.MinimumNumberReplicas");
        m_ft_properties.push_back("org.omg.ft.FaultMonitoringInterval");
        m_ft_properties.push_back("org.omg.ft.CheckpointInterval");

        // Create a new POA and attach a servant locator to it.
        PortableServer::POAManager_var poa_mgr = parent->the_POAManager();

        CORBA::PolicyList policies;
        policies.length(6);
        policies[0] =
            parent->create_id_assignment_policy(PortableServer::USER_ID);
        policies[1] =
            parent->create_lifespan_policy(PortableServer::TRANSIENT);
        policies[2] =
            parent->create_servant_retention_policy(PortableServer::NON_RETAIN);
        policies[3] =
            parent->create_id_uniqueness_policy(PortableServer::MULTIPLE_ID);
        policies[4] = parent->
            create_request_processing_policy(PortableServer::USE_DEFAULT_SERVANT);
        policies[5] = parent->
            create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);


        m_poa = parent->create_POA("ReplicationManagerPOA", poa_mgr.in(), policies);

        // create Fallback default servant
        Fallback_impl* defaultServant =
            new Fallback_impl(m_orb.in(), m_poa.in());
        
        try {
            // set default servant
            m_poa->set_servant(defaultServant);
        } catch (const PortableServer::POA::WrongPolicy& e) {
            // shouldn't happen since m_poa has USE_DEFAULT_SERVANT policy set
            CDMW_NEVER_HERE();
        }

        m_iogrFactory = new Cdmw::FT::TAO_IOGRFactory(m_orb.in(),
                                                      m_ostream);

        CORBA::Object_var replication_manager_ref = CORBA::Object::_nil();
        
        m_propertiesStorageHome = new PropertiesStorageHome(PROPERTIES_DATA_STORE);
        
        m_ogdStorageHome = new OGDStorageHome(OGD_DATA_STORE);
        
        m_ogidStorageHome = new OGIDStorageHome(OGID_DATA_STORE);

        m_locInfoStorageHome = new LocInfoStorageHome(LOCATIONS_DATA_STORE);
        
        // Register observers on the datastores
        ogdObserver = new OGDObserver(this);

        Cdmw::FT::TXDataStoreBase* dsBase1 = Cdmw::FT::TXDataStoreBase::Get_dataStore(OGD_DATA_STORE);
        CDMW_ASSERT(dsBase1 != 0);
        

        m_OGBdataStoreObj = dynamic_cast< OGDDataStore* >(dsBase1);
        CDMW_ASSERT(m_OGBdataStoreObj != 0);

        //  register the previous observer
        bool registered = m_OGBdataStoreObj->register_observer(ogdObserver);
        if ( ! registered) {
            // ###### ?????
        }



        locInfoObserver = new LocInfoObserver(this);

        Cdmw::FT::TXDataStoreBase* dsBase2 = Cdmw::FT::TXDataStoreBase::Get_dataStore( LOCATIONS_DATA_STORE);
        CDMW_ASSERT(dsBase2 != 0);
        

        LocInfoDataStore* dataStoreObj2 = dynamic_cast< LocInfoDataStore* >(dsBase2);
        CDMW_ASSERT(dataStoreObj2 != 0);

        //  register the previous observer
        registered = dataStoreObj2->register_observer(locInfoObserver);
        if ( ! registered) {
            // ###### ?????
        }

        // Check the CurrentData Ctor error and throw an exception if needed
        if (m_thread_data.xtor_error() < 0) {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "ReplicationManager_impl::Ctor\n"
                       << "Could not initialize the CurrentData instance\n"
                       << "Raising CORBA::INTERNAL exception.");
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL
                               , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }

    /**
     * Destroys a replication manager.
     */
    ReplicationManager_impl::~ReplicationManager_impl(void) throw ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        //????: not destroyed ????"
        // ??? delete m_ogidDataStore;
        // ??? delete m_ogidStorageHome;
        //delete m_locInfoDataStore;
        delete m_locInfoStorageHome;
        LocationCtxts::iterator locpos;
        for (locpos = m_loc_ctxts.begin(); locpos != m_loc_ctxts.end(); ++locpos) {
            Location::Location* ctxt = locpos->second;
            if (ctxt) delete ctxt;
        }
        ObjectGroupCtxts::iterator grppos;
        for (grppos = m_grp_ctxts.begin(); grppos != m_grp_ctxts.end(); ++grppos) {
            ObjectGroup::ObjectGroup* ctxt = grppos->second;
            if (ctxt) delete ctxt;
        }
        ObjectCtxts::iterator objpos;
        for (objpos = m_obj_ctxts.begin(); objpos != m_obj_ctxts.end(); ++objpos) {
            Object::Object* ctxt = objpos->second;
            if (ctxt) delete ctxt;
        }
    }



    /**
     * Implementation of 'get_object_group_data' inherited from abstract
     * class ReplicationManagerDelegate.
     *
     * Returns a pointer on the CdmwFT::ObjectGroupData with the id 'ogid':
     */
    ::CdmwFT::ObjectGroupData*
    ReplicationManager_impl::get_object_group_data (::FT::ObjectGroupId  ogid)
        throw (::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid=" << ogid);
        ::CdmwFT::ObjectGroupData_var ogd;
        try {
            OGDStorageObject objStorageObject = m_ogdStorageHome->find_by_oid(ogid);        
            ogd = objStorageObject.get();
        } catch (const NotFoundException& ) {  
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ReplicationManager_impl::get_object_group_data(" << ogid << ")': OGD not found.\n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNALObjectGroupDataNotFound
                               , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        return ogd._retn();
    }
    

    /**
     * Implementation of 'set_object_group_data' inherited from abstract
     * class ReplicationManagerDelegate.
     *
     */
    void
    ReplicationManager_impl::set_object_group_data (::CdmwFT::ObjectGroupData* ogd)
        throw (::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogd=" << ogd);
        try {
            ObjectGroup::ObjectGroup * grpctxt = NULL;

            ObjectGroupCtxts::iterator grp_iter = m_grp_ctxts.find(ogd->group_id);
            if (grp_iter == m_grp_ctxts.end()) {
                CORBA::INTERNAL ex(OrbSupport::INTERNALObjectGroupDataNotFound, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            grpctxt = grp_iter->second;

            ogd->state = grpctxt->fsm_state();;
            ogd->fsm_internal_state = grpctxt->fsm_internal_state();
            

            // add state into ogd
            OGDStorageObject objStorageObject = m_ogdStorageHome->find_by_oid(ogd->group_id);        
            objStorageObject.set(*ogd);
        } catch (const NotFoundException& ) {  
            CDMW_ERROR(FTLogger::GetLogger(),            
                       "ReplicationManager_impl::set_object_group_data(" 
                       << ogd->group_id << ")': OGD not found.\n"
                       << "Raising CORBA::INTERNAL exception.");
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNALObjectGroupDataNotFound
                               , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
    

    /**
     * Implementation of 'get_location_info' inherited from abstract
     * class ReplicationManagerDelegate.
     *
     * Returns a pointer on the CdmwFT::LocationInfo with the name 'loc_name':
     */
    ::CdmwFT::LocationInfo*
    ReplicationManager_impl::get_location_info (const ::FT::Location&  ftloc)
        throw ( ::CORBA::INTERNAL
              , ::CORBA::BAD_PARAM)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ftloc=" << ftloc);
        ::CdmwFT::LocationInfo_var loc;
        std::string locname;
        try {
            locname = ftloc_to_string(ftloc);
        } catch (const ::CORBA::INTERNAL& ) {     
            CDMW_INFO(FTLogger::GetLogger(),         
                      "ReplicationManager_impl::get_location_info(" << ftloc << ")': malformed Location name.\n"
                      << "Raising CORBA::BAD_PARAM exception.");
            
            ::CORBA::BAD_PARAM ex(::Cdmw::OrbSupport::BAD_PARAMInvalidLocation
                                  , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        try {
            LocInfoStorageObject objStorageObject = m_locInfoStorageHome->find_by_oid(locname);        
            loc = objStorageObject.get();
        } catch (const NotFoundException& ) {            
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "ReplicationManager_impl::get_location_info(" << ftloc << ")': LocationInfo not found.\n"
                       << "Raising CORBA::INTERNAL exception.");
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNALLocationInfoNotFound
                               , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        return loc._retn();
    }
    


    /**
     * Implementation of 'get_location_info' inherited from abstract
     * class ReplicationManagerDelegate.
     *
     * Returns a pointer on the CdmwFT::LocationInfo with the name 'loc_name':
     */
    ::CdmwFT::LocationInfo*
    ReplicationManager_impl::get_location_info (const std::string&  loc_name)
        throw (::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"loc_name=" << loc_name);
        ::CdmwFT::LocationInfo_var loc;
        try {
            LocInfoStorageObject objStorageObject = m_locInfoStorageHome->find_by_oid(loc_name);        
            loc = objStorageObject.get();
        } catch (const NotFoundException& ) {    
            CDMW_ERROR(FTLogger::GetLogger(),          
                       "ReplicationManager_impl::get_location_info(" << loc_name << ")': LocationInfo not found.\n"
                       << "Raising CORBA::INTERNAL exception.");
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNALLocationInfoNotFound
                               , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        return loc._retn();
    }
    


    /**
     * Implementation of 'set_location_info' inherited from abstract
     * class ReplicationManagerDelegate.
     */
    void
    ReplicationManager_impl::set_location_info (
            ::CdmwFT::LocationInfo* locinfo
          , const std::string&      locname)
        throw (::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"locname=" << locname);
        try {
            Location::Location*           locctxt = NULL;
            LocationCtxts::iterator loc_iter = m_loc_ctxts.find(locname);
            if (loc_iter == m_loc_ctxts.end()) {
                CORBA::INTERNAL ex(OrbSupport::INTERNALLocationCtxtNotFound, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            locctxt = loc_iter->second;
            locinfo->state = locctxt->fsm_state();
            locinfo->fsm_internal_state = locctxt->fsm_internal_state();
            locinfo->expected_add_member = locctxt->exp_add_member();
            locinfo->initial_backup = locctxt->backup();
            locinfo->first_backup = locctxt->first_backup();            

            LocInfoStorageObject objStorageObject
                = m_locInfoStorageHome->find_by_oid(locname);        
            objStorageObject.set(*locinfo);
        } catch (const NotFoundException& ) {  
            CDMW_ERROR(FTLogger::GetLogger(),            
                       "ReplicationManager_impl::set_location_info(" << locname << ")': LocationInfo not found.\n"
                       << "Raising CORBA::INTERNAL exception.");
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNALLocationInfoNotFound
                               , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
    


    /**
     * Implementation of 'get_object_group_data' inherited from abstract
     * class ReplicationManagerDelegate.
     *
     * Search the ObjectGroupData with id 'ogid' in the current data. If the
     * item is not found, a fresh copy is extracted from the DataStore and
     * inserted in the current data. If the item is found, returns a pointer
     * on the current item. The current data keeps ownership of the pointed
     * item.
     *
     */
    ::CdmwFT::ObjectGroupData*
    ReplicationManager_impl::get_current_ogd (::FT::ObjectGroupId  ogid)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid=" << ogid);
        // #### FIXME: exception if current == NULL ????
        CurrentData* current = (CurrentData*)m_thread_data.get_value();
        ::CdmwFT::ObjectGroupData* result = NULL;
        if (current != 0) {
            result = current->get_current_ogd(ogid);
        }
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }


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
    ReplicationManager_impl::get_current_loc (const std::string& locname)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"locname=" << locname);
        // #### FIXME: exception if current == NULL ????
        CurrentData* current = (CurrentData*)m_thread_data.get_value();
        ::CdmwFT::LocationInfo* result = NULL;
        if (current != 0) {
            result = current->get_current_loc(locname);
        }
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }
  

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
    ReplicationManager_impl::modified_ogd (::FT::ObjectGroupId ogid)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid=" << ogid);
        // #### FIXME: exception if current == NULL ????
        CurrentData* current = (CurrentData*)m_thread_data.get_value();
        bool result = false;
        if (current != 0) {
            result = current->modified_ogd(ogid);
        }
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }
    
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
    ReplicationManager_impl::modified_loc (const std::string& name)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"name=" << name);
        // #### FIXME: exception if current == NULL ????
        CurrentData* current = (CurrentData*)m_thread_data.get_value();
        bool result = false;
        if (current != 0)
            result = current->modified_loc(name);

        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }
        
    /*
     * Implementation of 'string_to_ftloc' inherited from abstract class
     * ReplicationManagerDelegate.
     *
     * Builds a ::FT::Location from a std::string
     */
    ::FT::Location*
    ReplicationManager_impl::string_to_ftloc(const std::string& loc_name)
        throw (::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"loc_name=" << loc_name);
        ::FT::Location_var ftloc;
        try {
            ftloc = Cdmw::CommonSvcs::Naming::NamingInterface::to_name(loc_name);
        } catch (const Cdmw::CommonSvcs::Naming::InvalidNameException&) {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "string_to_ftloc():"
                       << "Couldn't convert string to FT::Location '" << loc_name << "'.\n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        return ftloc._retn();
    }

    /*
     * Implementation of 'ftloc_to_string' inherited from abstract class
     * ReplicationManagerDelegate.
     *
     * Builds a std::string from a ::FT::Location 
     */
    std::string
    ReplicationManager_impl::ftloc_to_string(const ::FT::Location& loc_name)
        throw (::CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"loc_name=" << loc_name);
        // III) Is the location known? (and get its MemberAdminInfo)
        std::string s_location;
        try {
            s_location = 
                Cdmw::CommonSvcs::Naming::NamingInterface::to_string(loc_name);
        } catch(const Cdmw::CommonSvcs::Naming::InvalidNameException&) {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "ftloc_to_string():"
                       << "add_member(" << s_location << ")': Malformed location.\n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        CDMW_LOG_FUNCTION_RETURN(s_location);
        return s_location;
    }

    /**
     * Implementation of 'remove_locctxt_from_container' inherited from 
     * abstract class ReplicationManagerDelegate.
     * 
     * Removes the item without deleting the pointer
     */
    bool
    ReplicationManager_impl::remove_locctxt_from_container (const std::string& key)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"key=" << key);
        bool result = (m_loc_ctxts.erase(key) == 1)? true: false;
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }
  
    /**
     * Implementation of 'remove_objctxt_from_container' inherited from 
     * abstract class ReplicationManagerDelegate.
     * 
     * Removes the item without deleting the pointer
     */
    bool
    ReplicationManager_impl::remove_objctxt_from_container (::CORBA::ULong key)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"key=" << key);
        bool result = (m_obj_ctxts.erase(key) == 1)? true: false;
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }
  

    /**
     * Implementation of 'remove_grpctxt_from_container' inherited from 
     * abstract class ReplicationManagerDelegate.
     * 
     * Removes the item without deleting the pointer
     */
    bool
    ReplicationManager_impl::remove_grpctxt_from_container (::FT::ObjectGroupId key)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"key=" << key);
        bool result = (m_grp_ctxts.erase(key) == 1)? true: false;
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }
  

    /**
     *
     */
    void
    ReplicationManager_impl::notify_group_state_observers (::FT::ObjectGroupId         object_group_id,
                                                           ::CdmwFT::ObjectGroupState  new_state,
                                                           CORBA::Long                 nb_of_replicas,
                                                           CORBA::Long                 minimum_nb_of_replicas)
        throw()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                          "object_group_id=" << object_group_id
                          << ",new_state=" << sObjectGroupStateString[new_state]
                          << ",nb_of_replicas" << nb_of_replicas
                          << ",minimum_nb_of_replicas=" << minimum_nb_of_replicas);
// #### FIXME: to implement
//        throw CORBA::NO_IMPLEMENT(
//                  ::Cdmw::OrbSupport::NO_IMPLEMENTNotYetImplemented,
//                  CORBA::COMPLETED_NO);
            CDMW_INFO(FTLogger::GetLogger(),     
                      "Object Group [" << object_group_id << "] "
                      << "changing state to " << sObjectGroupStateString[new_state] 
                      << "\n     - Group has " << nb_of_replicas << " replica(s), "
                      << "minimum number set to " << minimum_nb_of_replicas);
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
     * @exception ObjectGroupNotFound if the object group is not found with the
     *         given object group id.
     */
    ::FT::ObjectGroup_ptr
    ReplicationManager_impl::get_object_group_ref_from_gid(
            ::FT::ObjectGroupId object_group_id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id=" << object_group_id);
        CDMW_MUTEX_GUARD (m_mutex);
        try
        {            
            OGDStorageObject obj = m_ogdStorageHome->find_by_oid(object_group_id);
            CdmwFT::ObjectGroupData_var ogd = obj.get();
            return ::FT::ObjectGroup::_duplicate((ogd->object_group_ref).in());
        }
        catch (const Cdmw::FT::NotFoundException&)
        {
            CDMW_INFO(FTLogger::GetLogger(),
                      "get_object_group_ref_from_gid("
                      <<  object_group_id<< "): Object group not found.\n"
                      << "Raising ObjectGroupNotFound() exception.");
            ::FT::ObjectGroupNotFound ex; 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }

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
    ::FT::ObjectGroup_ptr
    ReplicationManager_impl::get_object_group_ref_from_name(
            const CdmwFT::GroupName& group_name)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"group_name=" << group_name);
        CDMW_MUTEX_GUARD (m_mutex);
        
        std::string s_the_name;
        try {
            s_the_name = 
            Cdmw::CommonSvcs::Naming::NamingInterface::to_string(group_name);
        }
        catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
            CDMW_INFO(FTLogger::GetLogger(), 
                      "add_member(" << s_the_name << ")': Malformed location.\n"
                      << "Raising BAD_PARAMInvalidLocation() exception.");
            CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMInvalidLocation, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        std::vector<OGDStorageObject> ogd_list;
        m_ogdStorageHome->find_if(std::bind2nd(
                                               is_object_group_from_name < std::pair < Cdmw::OrbSupport::type_traits < ::FT::ObjectGroupId >::_var_type, 
                                               Cdmw::OrbSupport::type_traits < ::CdmwFT::ObjectGroupData >::_var_type 
                                                > 
                                  > (),
        s_the_name),
          std::back_inserter(ogd_list));
    
        if (ogd_list.size() != 1) {
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    
    //CdmwFT::ObjectGroupData_var ogd = obj_list[0L].get();

    return ::FT::ObjectGroup::_duplicate((ogd_list[0L].get()->object_group_ref).in());
}
    

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
     * @exception ObjectGroupNotFound if the object group is not found with the
     *         given reference.
     */
    ::FT::ObjectGroupRefVersion
    ReplicationManager_impl::get_object_group_version_from_ref(
            ::FT::ObjectGroup_ptr object_group)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // Get all known data of the object group
        // May raise ::FT::ObjectGroupNotFound
        ::FT::ObjectGroupId object_group_id = get_object_group_id_from_ref( object_group );
        ::FT::ObjectGroupRefVersion version;        
        try
        {            
            OGDStorageObject obj = m_ogdStorageHome->find_by_oid(object_group_id);  
            CdmwFT::ObjectGroupData_var ogd = obj.get();
            version = ogd->object_group_ref_version;
        }
        catch (const Cdmw::FT::NotFoundException&)
        {
            CDMW_INFO(FTLogger::GetLogger(),  
                      "get_object_group_version_from_ref("
                      <<  object_group_id<< "): Object group not found.\n"
                      << "Raising ObjectGroupNotFound() exception." );
            ::FT::ObjectGroupNotFound ex; 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        CDMW_INFO(FTLogger::GetLogger(),  
                  "get_object_group_version_from_ref() called for '"
                  <<  object_group_id<< "'. Current version is '"
                  << version << "'." );
        CDMW_LOG_FUNCTION_RETURN(version);
        return version;
    }

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
     * @exception ObjectGroupNotFound if the object group is not found with the
     *         given object group id.
     */
    ::FT::ObjectGroupRefVersion
    ReplicationManager_impl::get_object_group_version_from_gid(
            ::FT::ObjectGroupId object_group_id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id=" << object_group_id);
        CDMW_MUTEX_GUARD (m_mutex);
        // Get the group info associated with this object group id.
        try
        {            
            OGDStorageObject obj = m_ogdStorageHome->find_by_oid(object_group_id); 
            CdmwFT::ObjectGroupData_var ogd = obj.get();            
            return ogd->object_group_ref_version;
        }
        catch (const Cdmw::FT::NotFoundException&)
        {
            CDMW_INFO(FTLogger::GetLogger(),        
                      "get_object_group_version_from_gid("
                      <<  object_group_id<< "): Object group not found.\n"
                      << "Raising ObjectGroupNotFound() exception." );
            ::FT::ObjectGroupNotFound ex; 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }


    /**
     * IDL:thalesgroup.com/CdmwFT/
     *      ReplicationManager/get_object_group_info:1.0
     *
     * Returns information about the object group with the given object group
     * reference.
     *
     * @param object_group the object group reference.
     *
     * @return a group info.
     *
     * @exception ObjectGroupNotFound if the object group is not found with the
     *         given reference.
     */
    ::CdmwFT::GroupInfo *
    ReplicationManager_impl::get_object_group_info(
            ::FT::ObjectGroup_ptr object_group)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // Get all known data of the object group
        // May raise ::FT::ObjectGroupNotFound

        // May raise ::FT::ObjectGroupNotFound
        ::FT::ObjectGroupId object_group_id = get_object_group_id_from_ref(object_group);
        CdmwFT::ObjectGroupData_var ogd;
        CdmwFT::GroupInfo_var       grpinfo;
        try {            
            OGDStorageObject obj = m_ogdStorageHome->find_by_oid(object_group_id);
            ogd = obj.get();
            grpinfo = ObjectGroup::ObjectGroup::get_object_group_info(ogd.in());            
        } catch (const Cdmw::FT::NotFoundException& ) {
            CDMW_INFO(FTLogger::GetLogger(),
                      "get_object_group_info("
                      <<  object_group_id << "): Object group not found.\n"
                      << "Raising ObjectGroupNotFound() exception.");
            
            ::FT::ObjectGroupNotFound ex; 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::CORBA::NO_MEMORY& ex) {
            CDMW_FATAL(FTLogger::GetLogger(),
                       "get_object_group_info("
                       <<  object_group_id<< "): MEMORY error.\n"
                       << "Raising CORBA::NO_MEMORY exception.");
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw; 
        }
        
        return grpinfo._retn();
    }

    /**
     * IDL:thalesgroup.com/CdmwFT/
     *      ReplicationManager/get_object_group_info_from_gid:1.0
     *
     * Returns information about the object group with the given object group id.
     *
     * @param object_group_id the object group id.
     *
     * @return a group info.
     *
     * @exception ObjectGroupNotFound if the object group is not found with the
     *         given object group id.
     */
    ::CdmwFT::GroupInfo *
    ReplicationManager_impl::get_object_group_info_from_gid(
            ::FT::ObjectGroupId object_group_id)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException) 
    {
        
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id=" << object_group_id);
        CDMW_MUTEX_GUARD (m_mutex);
        // Get the group info associated with this object group id.
        try {          
            OGDStorageObject obj =  m_ogdStorageHome->find_by_oid(object_group_id);
            CdmwFT::ObjectGroupData_var ogd = obj.get();            
            return ObjectGroup::ObjectGroup::get_object_group_info(ogd.in());
        } catch (const Cdmw::FT::NotFoundException&) {
            CDMW_INFO(FTLogger::GetLogger(), 
                      "get_object_group_info_from_gid("
                      <<  object_group_id<< "): Object group not found.\n"
                      << "Raising ObjectGroupNotFound() exception.");
            ::FT::ObjectGroupNotFound ex; 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::CORBA::NO_MEMORY& ex) {
            CDMW_FATAL(FTLogger::GetLogger(), 
                      "get_object_group_info_from_gid("
                       <<  object_group_id<< "): MEMORY error.\n"
                       << "Raising CORBA::NO_MEMORY exception.");
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        }
    }


    /**
     *IDL:thalesgroup.com/CdmwFT/
     *      ReplicationManager/update_fallback:1.0
     *
     * Change the fallback reference on all the IOGR
     */
    void
    ReplicationManager_impl::update_fallback(const ::FT::Location & the_location)
        throw(CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            CDMW_MUTEX_GUARD (m_mutex);


            // this next line must be added if we consider the the host failed 
            // when the ftmanager is failed
            
            //handle_host_failure(std::string(the_location[0].id));

            CurrentData current(this, &this->m_thread_data);

            // Build the new IOGR for each group
            for ( ObjectGroupCtxts::iterator grppos  = m_grp_ctxts.begin()
                ; grppos != m_grp_ctxts.end()
                ; ++grppos
                ) {
                
                // Get a pointer to the current group ObjectGroupData
                ::FT::ObjectGroupId ogid = grppos->second->ogid();
                ::CdmwFT::ObjectGroupData* ogd = current.get_current_ogd(ogid);

                // Get primary_loc if any
                ::FT::Location_var primary_loc
                    = get_group_primary_location(*ogd);

                // Update the IOGR (sets the 'ogd modified' flag in current)
                build_iogr (primary_loc.in(), *ogd);
            }
            
            // Update the group views each Location
            for ( LocationCtxts::iterator locpos  = m_loc_ctxts.begin()
                ; locpos != m_loc_ctxts.end()
                ; ++locpos
                ) {
                ::FT::Location_var ftloc;
                try {
                    ftloc = string_to_ftloc(locpos->first);
                } catch (const ::CORBA::INTERNAL &ex ) {
                    CDMW_ERROR(FTLogger::GetLogger(), 
                               "Raising CORBA::INTERNAL() exception.");
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
                const char* mhost = (ftloc.inout())[0].id;
                const char* failed_host = the_location[0].id;

                // if the location doesn't contains the host failed, update it
                // else the location was already updated by the handle_host_failure
                if (strcmp(mhost,failed_host))
                    locpos->second->update_all_group_views();
            }

            current.save_modified_items();
            //show the context
            ShowContexts();

        } catch (const ::FT::PrimaryNotSet &) {
            CDMW_ERROR(FTLogger::GetLogger(), 
                      "update_fallback: "
                       << "Could not set primary.\n"
                       << "Raising CORBA::INTERNAL() exception.");
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL,
                                  CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::FT::ObjectNotCreated &) {
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "update_fallback: "
                       << "Could not create group reference.\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL,
                               CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::FT::MemberNotFound &) {
            CDMW_ERROR(FTLogger::GetLogger(), 
                       "update_fallback: "
                       << "Caught a MemberNotFound exception.\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL,
                               CORBA::COMPLETED_NO); 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;               
        } catch (const CORBA::INTERNAL & ex) {
            CDMW_INFO(FTLogger::GetLogger(), 
                      "update_fallback: "
                      << "Caught a CORBA::INTERNAL exception.\n"
                       << "Rethrowing it..." );
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        } catch (const CORBA::NO_MEMORY & ex) {
            CDMW_FATAL(FTLogger::GetLogger(), 
                       "update_fallback: "
                       << "Caught a CORBA::NO_MEMORY exception.\n"
                       << "Rethrowing it..." );
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        } catch (const CORBA::SystemException & ex) {
            CDMW_WARN(FTLogger::GetLogger(), 
                      "update_fallback: "
                      << "Caught a system exception.\n"
                      << "Exception:" << ex );
            // TODO: Trigger a reconfiguration? (check that member is non faulty?)
        }
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
    ::FT::FaultNotifier_ptr
    ReplicationManager_impl::get_fault_notifier(void)
        throw(::FT::InterfaceNotFound,
              CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        
        if (CORBA::is_nil(m_fault_notifier.in())) {
            ::FT::InterfaceNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        return ::FT::FaultNotifier::_duplicate(m_fault_notifier.in());
    }


    /**
     * IDL:omg.org/FT/PropertyManager/set_default_properties:1.0
     *
     * Sets the default properties for all object groups that are to be created
     * within the fault tolerance domain.
     *
     * @param props The properties to be set for all newly created object groups
     *              within the fault tolerance domain.
     *
     * @exception InvalidProperty if one or more of the properties in the sequence
     *         is not valid.
     * @exception UnsupportedProperty if one or more of the properties in the
     *         sequence is not supported.
     */
    void
    ReplicationManager_impl::set_default_properties(const ::FT::Properties & props)
        throw(::FT::InvalidProperty,
              ::FT::UnsupportedProperty,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);

        // Sanity check.
        check_properties(props, DEFAULT_PROPERTIES);

        //store default properties
        ::FT::Properties default_properties(props);        
        try
        {
			//get the previous default properties
            PropertiesStorageObject obj = m_propertiesStorageHome->find_by_oid("default_properties");
            ::FT::Properties_var current_properties = obj.get();
			
			// copy current default properties into a map
            PropertiesMap properties;
            CORBA::ULong property_length = current_properties->length();
            for (CORBA::ULong i = 0; i < property_length; ++i) {
                properties[std::string(current_properties[i].nam[0].id)] = current_properties[i];
            }
            // overwrite the current default properties by the news
            property_length = default_properties.length();
            for (CORBA::ULong i = 0; i < property_length; ++i) {
                properties.insert(make_pair(std::string(default_properties[i].nam[0].id), default_properties[i]));
            }

            // Finally, return the result list of properties.
            PropertiesMap::size_type size = properties.size();
            ::FT::Properties result_properties;// = new ::FT::Properties(size);
            result_properties.length(size);
            CORBA::ULong i = 0;
            PropertiesMap::iterator properties_iter;
            for (properties_iter = properties.begin();
                  properties_iter != properties.end(); ++properties_iter) {                
                  result_properties[i++] = (*properties_iter).second;
             }
            
			// set the default properties
            obj.set(result_properties);
        }
        catch (const NotFoundException& )
        {            
            m_propertiesStorageHome->create("default_properties",default_properties);
        }
        
    }

    /**
     * IDL:omg.org/FT/PropertyManager/get_default_properties:1.0
     *
     * Returns the default properties for the object groups within the fault
     * tolerance domain.
     *
     * @return the default properties that have been set for the object groups.
     */
    ::FT::Properties *
    ReplicationManager_impl::get_default_properties(void)
        throw(CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // get the storage object for the default properties
        PropertiesStorageObject obj = m_propertiesStorageHome->find_by_oid("default_properties");
        
        // get copy
        ::FT::Properties_var default_properties = obj.get();

        // return copy
        return default_properties._retn();
    }

    /**
     * IDL:omg.org/FT/PropertyManager/remove_default_properties:1.0
     *
     * Removes the given default properties.
     *
     * @param props The properties to be removed.
     *
     * @exception InvalidProperty if one or more of the properties in the sequence
     *         is not valid.
     * @exception UnsupportedProperty if one or more of the properties in the
     *         sequence is not supported.
     */
    void
    ReplicationManager_impl::remove_default_properties(const ::FT::Properties & props)
        throw(::FT::InvalidProperty,
              ::FT::UnsupportedProperty,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // Sanity check.
        // Properties values are not checked.
        check_properties(props, DEFAULT_PROPERTIES, false);

		//get the previous default properties
        PropertiesStorageObject obj = m_propertiesStorageHome->find_by_oid("default_properties");
        ::FT::Properties_var current_properties = obj.get();
			
     	// copy current default properties into a map
        PropertiesMap properties;
        CORBA::ULong property_length = current_properties->length();
        
        for (CORBA::ULong i = 0; i < property_length; ++i) {
            properties.insert(make_pair(std::string(current_properties[i].nam[0].id), current_properties[i]));
        }
        
        // each properties into the sequence, find if is is arlready into the map of the current properties
        for (CORBA::ULong i = 0; i < props.length(); ++i)
        {
            PropertiesMap::iterator iter = properties.find(std::string(props[i].nam[0].id));
            // if the properties is find, erase this properties 
            if (iter != properties.end())
                properties.erase(iter);
            // else throw invlaid property
            else {
                ::FT::InvalidProperty ex;            
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
        }
        
        // Finally, return the result list of properties.
        PropertiesMap::size_type size = properties.size();
        ::FT::Properties result_properties;// = new ::FT::Properties(size);
        result_properties.length(size);
        CORBA::ULong i = 0;
        PropertiesMap::iterator properties_iter;
        for (properties_iter = properties.begin();
             properties_iter != properties.end(); ++properties_iter) {                
            result_properties[i++] = (*properties_iter).second;
        }
 

        if (size == 0)
        {            
            // remove the default properties
            m_propertiesStorageHome->remove("default_properties");
        } else {
            // set the properties rest
            obj.set(result_properties);            
        }
        
    }

    /**
     * IDL:omg.org/FT/PropertyManager/set_type_properties:1.0
     *
     * Sets the properties that override the default properties of the object
     * groups, with the given type identifier, that are created in the future.
     *
     * @param type_id   The repository id for which the properties, that are to
     *                  override the existing properties, are set.
     * @param overrides The overriding properties.
     *
     * @exception InvalidProperty if one or more of the properties in the sequence
     *         is not valid.
     * @exception UnsupportedProperty if one or more of the properties in the
     *         sequence is not supported.
     */
    void
    ReplicationManager_impl::set_type_properties(
            const char * type_id,
            const ::FT::Properties & overrides)
        throw(::FT::InvalidProperty,
              ::FT::UnsupportedProperty,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // Sanity check.
        check_properties(overrides, TYPE_PROPERTIES);

        //store default properties
        ::FT::Properties type_properties(overrides);
        try
        {
			//get the previous default properties
            PropertiesStorageObject obj = m_propertiesStorageHome->find_by_oid(type_id);
            ::FT::Properties_var current_properties = obj.get();
			
			// copy current default properties into a map
            PropertiesMap properties;
            CORBA::ULong property_length = current_properties->length();
            for (CORBA::ULong i = 0; i < property_length; ++i) {
                properties[std::string(current_properties[i].nam[0].id)] = current_properties[i];
            }
            // overwrite the current default properties by the news
            property_length = type_properties.length();
            for (CORBA::ULong i = 0; i < property_length; ++i) {
                properties.insert(make_pair(std::string(type_properties[i].nam[0].id), type_properties[i]));
            }

            // Finally, return the result list of properties.
            PropertiesMap::size_type size = properties.size();
            ::FT::Properties result_properties;// = new ::FT::Properties(size);
            result_properties.length(size);
            CORBA::ULong i = 0;
            PropertiesMap::iterator properties_iter;
            for (properties_iter = properties.begin();
                  properties_iter != properties.end(); ++properties_iter) {                
                  result_properties[i++] = (*properties_iter).second;
             }
            
			// set the default properties
            obj.set(result_properties);
        }
        catch (const NotFoundException& )
        {            
            m_propertiesStorageHome->create(type_id,type_properties);
        }
    }

    /**
     * IDL:omg.org/FT/PropertyManager/get_type_properties:1.0
     *
     * Returns the properties of the object groups, with the given type
     * identifier, that are created in the future. These properties include
     * the properties determined by <code>set_type_properties()</code>, as well
     * as the default properties that are not overridden by <code>
     * set_type_properties</code>.
     *
     * @param type_id The repository id for which the properties, that are to
     *                override the existing properties, are set.
     *
     * @return the effective properties for the given type identifier.
     */
    ::FT::Properties *
    ReplicationManager_impl::get_type_properties(const char * type_id)
        throw(CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"type_id="<<type_id);
        CDMW_MUTEX_GUARD (m_mutex);
        // get the storage object for the default properties
        PropertiesStorageObject obj_default_properties = m_propertiesStorageHome->find_by_oid("default_properties");
        ::FT::Properties_var default_property = obj_default_properties.get();

        // transfert the sequence into a map
        PropertiesMap default_properties;
        CORBA::ULong default_property_length = default_property->length();
        for (CORBA::ULong i = 0; i < default_property_length; ++i) {
            default_properties.insert(make_pair(std::string(default_property[i].nam[0].id), default_property[i]));  
            }
        
        // get the storage object for the type properties
        PropertiesStorageObject obj_type_properties = m_propertiesStorageHome->find_by_oid(type_id);
        ::FT::Properties_var type_property = obj_type_properties.get();

        PropertiesMap type_properties;
        CORBA::ULong type_property_length = type_property->length();
        for (CORBA::ULong i = 0; i < type_property_length; ++i) {
            type_properties.insert(make_pair(std::string(type_property[i].nam[0].id), type_property[i]));
            }
        
        PropertiesMap properties = default_properties;
        PropertiesMap::iterator type_properties_iter;
        for (type_properties_iter = type_properties.begin();
             type_properties_iter != type_properties.end();
             ++type_properties_iter) {
            properties[(*type_properties_iter).first] = (*type_properties_iter).second;
        }

        // Finally, return the result list of properties.
        PropertiesMap::size_type size = properties.size();
        ::FT::Properties_var result_properties = new ::FT::Properties(size);
        result_properties->length(size);
        CORBA::ULong i = 0;
        PropertiesMap::iterator properties_iter;
        for (properties_iter = properties.begin();
             properties_iter != properties.end(); ++properties_iter) {

            result_properties[i++] = (*properties_iter).second;
        }

        return result_properties._retn();
    }

    /**
     * IDL:omg.org/FT/PropertyManager/remove_type_properties:1.0
     *
     * Removes the given properties, with the given type identifier.
     *
     * @param type_id The repository id for which the given properties are to
     *                be removed.
     * @param props   The properties to be removed.
     *
     * @exception InvalidProperty if one or more of the properties in the sequence
     *         is not valid.
     * @exception UnsupportedProperty if one or more of the properties in the
     *         sequence is not supported.
     */
    void
    ReplicationManager_impl::remove_type_properties(
            const char * type_id,
            const ::FT::Properties & props)
        throw(::FT::InvalidProperty,
              ::FT::UnsupportedProperty,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"type_id="<<type_id);
        CDMW_MUTEX_GUARD (m_mutex);
        // Sanity check.
        // Properties values are not checked.
        check_properties(props, TYPE_PROPERTIES, false);

        //get the previous default properties
        PropertiesStorageObject obj = m_propertiesStorageHome->find_by_oid(type_id);
        ::FT::Properties_var current_properties = obj.get();
			
     	// copy current default properties into a map
        PropertiesMap properties;
        CORBA::ULong property_length = current_properties->length();
        for (CORBA::ULong i = 0; i < property_length; ++i) {
            properties.insert(make_pair(std::string(current_properties[i].nam[0].id), current_properties[i]));
        }
        
        // each properties into the sequence, find if is is arlready into the map of the current properties
        for (CORBA::ULong i = 0; i < props.length(); ++i)
        {
            PropertiesMap::iterator iter = properties.find(std::string(props[i].nam[0].id));
            // if the properties is find, erase this properties 
            if (iter != properties.end())
                properties.erase(iter);
            // else throw invlaid property
            else
                throw ::FT::InvalidProperty();            
        }
        
        // Finally, return the result list of properties.
        PropertiesMap::size_type size = properties.size();
        ::FT::Properties result_properties;// = new ::FT::Properties(size);
        result_properties.length(size);
        CORBA::ULong i = 0;
        PropertiesMap::iterator properties_iter;
        for (properties_iter = properties.begin();
             properties_iter != properties.end(); ++properties_iter) {                
            result_properties[i++] = (*properties_iter).second;
        }
 

        if (size == 0)
        {            
            // remove the default properties
            m_propertiesStorageHome->remove(type_id);
        } else {
            // set the properties rest
            obj.set(result_properties);            
        }
    }

    /**
     * IDL:omg.org/FT/PropertyManager/set_properties_dynamically:1.0
     *
     * Sets the properties for the object group with the given reference
     * dynamically while the application executes. The properties given as a
     * parameter override the properties for the object when it was created
     * which, in turn, override the properties for the given type which, in
     * turn, override the default properties.
     *
     * @param object_group The reference of the object group for which the
     *                     overriding properties are set.
     * @param overrides    The overriding properties.
     *
     * @exception ObjectGroupNotFound if the object group is not found with the
     *         given reference.
     * @exception InvalidProperty if one or more of the properties in the sequence
     *         is not valid.
     * @exception UnsupportedProperty if one or more of the properties in the
     *         sequence is not supported.
     */
    void
    ReplicationManager_impl::set_properties_dynamically (
            ::FT::ObjectGroup_ptr object_group,
            const ::FT::Properties & overrides)
        throw(::FT::ObjectGroupNotFound,
              ::FT::InvalidProperty,
              ::FT::UnsupportedProperty,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // Sanity check.
        check_properties(overrides, DYNAMIC_PROPERTIES);

        try
        {            
            // Get all known data of the object group
            // May raise ::FT::ObjectGroupNotFound
            ::FT::ObjectGroupId object_group_id = get_object_group_id_from_ref( object_group );
            OGDStorageObject obj = m_ogdStorageHome->find_by_oid(object_group_id);        
            ::CdmwFT::ObjectGroupData_var object_group_data = obj.get();
            


            //store default properties
            ::FT::Properties dynamic_properties(overrides);
            try
            {
                ::FT::Properties current_properties = object_group_data->dynamic_properties;                
                
                // copy current default properties into a map
                PropertiesMap properties;
                CORBA::ULong property_length = current_properties.length();
                for (CORBA::ULong i = 0; i < property_length; ++i) {
                    properties.insert(make_pair(std::string(current_properties[i].nam[0].id), current_properties[i]));
                }
                // overwrite the current default properties by the news
                property_length = dynamic_properties.length();
                for (CORBA::ULong i = 0; i < property_length; ++i) {
                    properties[std::string(dynamic_properties[i].nam[0].id)] =  dynamic_properties[i];
                }

                // Finally, return the result list of properties.
                PropertiesMap::size_type size = properties.size();
                ::FT::Properties result_properties;// = new ::FT::Properties(size);
                result_properties.length(size);
                CORBA::ULong i = 0;
                PropertiesMap::iterator properties_iter;
                for (properties_iter = properties.begin();
                     properties_iter != properties.end(); ++properties_iter) {                
                    result_properties[i++] = (*properties_iter).second;
                }
            
                // set the default properties
                object_group_data->dynamic_properties=result_properties;            
                obj.set(object_group_data.in());
            }
            catch (const NotFoundException& )
            {            
           
            }

        }
        catch (const NotFoundException& e)
        {            
            ::FT::ObjectGroupNotFound ex;   
            CDMW_INFO(FTLogger::GetLogger(),
                      "set_properties_dynamically(): " 
                      << "Received " << e << " exception. "
                      <<"\nThrowing " << ex << " exception. ");
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;         
        }

    }

    /**
     * IDL:omg.org/FT/PropertyManager/get_properties:1.0
     *
     * Returns the current properties of the given object group. These
     * properties include those that are set dynamically, those that are set
     * when the object group was created but are not overridden by <code>
     * set_properties_dynamically()</code>, those that are set as properties of
     * a type but are not overridden by <code>create_obj*ect()</code> and <code>
     * set_properties_dyamically()</code>, and those that are set as defaults
     * but are not overridden by <code>set_type_properties()</code>, <code>
     * create_object()</code>, and <code>set_properties_dyamically()</code>.
     *
     * @param object_group The reference of the object group for which the
     *                     properties are to be returned.
     *
     * @return the set of current properties for the object group with the given
     *         reference.
     *
     * @exception ObjectGroupNotFound if the object group is not found by the
     *         Replication Manager.
     */
    ::FT::Properties *
    ReplicationManager_impl::get_properties(
            ::FT::ObjectGroup_ptr object_group)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // Get all known data of the object group
        // May raise ::FT::ObjectGroupNotFound
        ::FT::ObjectGroupId object_group_id = get_object_group_id_from_ref( object_group );
        OGDStorageObject obj = m_ogdStorageHome->find_by_oid(object_group_id);        
        ::CdmwFT::ObjectGroupData_var object_group_data = obj.get();


        // get the storage object for the default properties
        PropertiesStorageObject obj_default_properties = m_propertiesStorageHome->find_by_oid("default_properties");
        ::FT::Properties_var default_property = obj_default_properties.get();

        // transfert the sequence into a map
        PropertiesMap default_properties;
        CORBA::ULong default_property_length = default_property->length();
        for (CORBA::ULong i = 0; i < default_property_length; ++i) {
            default_properties.insert(make_pair(std::string(default_property[i].nam[0].id), default_property[i]));
            }
        
        // get the storage object for the type properties
        PropertiesStorageObject obj_type_properties = m_propertiesStorageHome->find_by_oid(std::string(object_group_data->repository_id));
        ::FT::Properties_var type_property = obj_type_properties.get();

        PropertiesMap type_properties;
        CORBA::ULong type_property_length = type_property->length();
        for (CORBA::ULong i = 0; i < type_property_length; ++i) {
            type_properties.insert(make_pair(std::string(type_property[i].nam[0].id), type_property[i]));
            }
        
        PropertiesMap properties = default_properties;

        PropertiesMap::iterator type_properties_iter;
        for (type_properties_iter = type_properties.begin();
             type_properties_iter != type_properties.end();
             ++type_properties_iter) {
            properties[(*type_properties_iter).first] = (*type_properties_iter).second;
        }


        // get the creation properties
        ::FT::Properties creation_property = object_group_data->creation_properties;
        PropertiesMap creation_properties;
        CORBA::ULong creation_property_length = creation_property.length();
        for (CORBA::ULong i = 0; i < creation_property_length; ++i) {
            creation_properties.insert(make_pair(std::string(creation_property[i].nam[0].id),creation_property[i]));
        }
        

        PropertiesMap::iterator creation_properties_iter;
        for (creation_properties_iter = creation_properties.begin();
             creation_properties_iter != creation_properties.end();
             ++creation_properties_iter) {
            properties[(*creation_properties_iter).first] = (*creation_properties_iter).second;
        }


        // get the dynamic properties
        ::FT::Properties dynamic_property = object_group_data->dynamic_properties;
        PropertiesMap dynamic_properties;
        CORBA::ULong dynamic_property_length = dynamic_property.length();
        for (CORBA::ULong i = 0; i < dynamic_property_length; ++i) {
            dynamic_properties.insert(make_pair(std::string(dynamic_property[i].nam[0].id), dynamic_property[i]));
        }
        

        PropertiesMap::iterator dynamic_properties_iter;
        for (dynamic_properties_iter = dynamic_properties.begin();
             dynamic_properties_iter != dynamic_properties.end();
             ++dynamic_properties_iter) {
            properties[(*dynamic_properties_iter).first] =  (*dynamic_properties_iter).second;
        }

        // Finally, return the result list of properties.
        PropertiesMap::size_type size = properties.size();
        ::FT::Properties_var result_properties = new ::FT::Properties(size);
        result_properties->length(size);
        CORBA::ULong i = 0;
        PropertiesMap::iterator properties_iter;
        for (properties_iter = properties.begin();
             properties_iter != properties.end(); ++properties_iter) {

            result_properties[i++] = (*properties_iter).second;
        }

        return result_properties._retn();
    }
    
    /**
     * IDL:omg.org/FT/ObjectGroupManager/create_member:1.0
     *
     * Allows the application to exercise explicit control over the creation of
     * a member of an object group, and to determine where the member is
     * created.
     *
     * @param object_group The object group reference for the object group to
     *                     which the member is to be added.
     * @param the_location The physical location; that is, a fault containment
     *                     region, host, cluster of hosts, etc. at which the new
     *                     member is to be created. There is at most one member
     *                     of an object group at each location.
     * @param type_id      The repository identifier for the type of the object.
     * @param the_criteria Parameters to be passed to the factory, which the
     *                     factory evaluates before creating the object.
     *
     * @return the object group reference of the object group with the member
     *         added. This reference may be the same as that passed in as a
     *         parameter.
     *
     * @exception ObjectGroupNotFound if the object group is not found by the
     *         Replication Manager.
     * @exception MemberAlreadyPresent if a member of the object group already 
     *         exists at the given location.
     * @exception NoFactory if the Replication Manager cannot find a factory 
     *         that is capable of constructing a member of the object group
     *         with the given <code>type_id</code> and at the given location.
     * @exception ObjectNotCreated if the factory or the Replication Manager
     *         cannnot create the member and add it to the object group.
     * @exception InvalidCriteria if the factory does not understand the criteria.
     * @exception CannotMeetCriteria if the factory understands the criteria but
     *         cannot satisfy it.
     */
    ::FT::ObjectGroup_ptr
    ReplicationManager_impl::create_member(
            ::FT::ObjectGroup_ptr object_group,
            const ::FT::Location & the_location,
            const char * type_id,
            const ::FT::Criteria & the_criteria)
        throw(::FT::ObjectGroupNotFound,
              ::FT::MemberAlreadyPresent,
              ::FT::NoFactory,
              ::FT::ObjectNotCreated,
              ::FT::InvalidCriteria,
              ::FT::CannotMeetCriteria,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                          "the_location=" << the_location
                          << ",type_id=" << type_id);
        CDMW_MUTEX_GUARD (m_mutex);
        // NOTE: factories are not supported yet.
        CDMW_WARN(FTLogger::GetLogger(),
                  "FT CORBA Local factories are not yet supported!"
                  << "Raising a CORBA::NO_IMPLEMENT exception.");
        CORBA::NO_IMPLEMENT ex (::Cdmw::OrbSupport::NO_IMPLEMENTNotYetImplemented,
                                CORBA::COMPLETED_NO); 
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;         

        return ::FT::ObjectGroup::_nil();
    }


    /**
     * IDL:omg.org/FT/ObjectGroupManager/add_member:1.0
     *
     * Allows an application to exercise explicit control over the addition of
     * an existing object to an object group at a particular location.
     *
     * TODO: ADD SUPPORT FOR CONCURRENT INSERTION OF BACKUPS
     *
     * @param object_group The object group reference if the object group to
     *                     which the existing object is to be added.
     * @param the_location The physical location; that is, a fault containment
     *                     region, host, cluster of hosts, etc. of the object to
     *                     be added. There is at most one member of an object
     *                     group at each location.
     * @param member       The reference of the object to be added.
     *
     * @return the object group reference for the object group with the object
     *         added. This reference may be the same as that passed in as a
     *         parameter.
     *
     * @exception ObjectGroupNotFound if the object group is not found by the
     *         Replication Manager.
     * @exception MemberAlreadyPresent if a member of the object group already
     *         exists at the given location.
     * @exception ObjectNotAdded if the Replication Manager cannot add the object
     *         to the object group.
     * @exception CORBA::BAD_PARAM if member has a nil object reference
     */

    ::FT::ObjectGroup_ptr
    ReplicationManager_impl::add_member(
            ::FT::ObjectGroup_ptr  object_group,
            const ::FT::Location & the_location,
            CORBA::Object_ptr      member)
        throw(::FT::ObjectGroupNotFound,
              ::FT::MemberAlreadyPresent,
              ::FT::ObjectNotAdded,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                          "the_location=" << the_location);
        CurrentData current(this,&this->m_thread_data);
        ShowContexts();

        // Temp local variables, no need to destroy if exception
        ObjectGroup::ObjectGroup*     grpctxt = NULL;
        Location::Location*           locctxt = NULL;

        // Temp local variables, automatically destroyed if exception
        ::FT::Location_var            primary_loc;
        ::FT::ObjectGroup_var         new_object_group;

        // Temp local variables, need to be destroyed if exception
        Object::Object*               objctxt = NULL;

        try {
            CDMW_MUTEX_GUARD (m_mutex);
            new_object_group = ::FT::ObjectGroup::_nil();
            
            // III) Is the location known? (and get its MemberAdminInfo)
            std::string s_the_location;
            try {
                s_the_location = 
                    Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location);
            }
            catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
                CDMW_INFO(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << ")': Malformed location.\n"
                          << "Raising BAD_PARAMInvalidLocation() exception." );
                CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMInvalidLocation, CORBA::COMPLETED_NO); 
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }
            

            // I) Check that member is non nil
            if (CORBA::is_nil(member)) {
                CDMW_INFO(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << ")': Adding a NIL member.\n"
                          << "Raising BAD_PARAMNilObjectReference exception." );
                CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMNilObjectReference, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }
            

            // Get the location info from the datastore
            ::CdmwFT::LocationInfo* loc_info = current.get_current_loc(s_the_location);
            if (! loc_info) {
                CDMW_INFO(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << ")': Location is unknown.\n"
                          << "Raising ObjectNotAdded() exception." );
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }
            
            if (!loc_info->has_members_admin){
                CDMW_INFO(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << ")': Location is not registered.\n"
                          << "Raising ObjectNotAdded() exception." );
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }
                

            // Now that we have the LocationInfo, get the Location context 
            LocationCtxts::iterator loc_iter = m_loc_ctxts.find(s_the_location);
            if (loc_iter == m_loc_ctxts.end()) {
                CDMW_ERROR(FTLogger::GetLogger(),
                          "Nil location context!"
                          << "Raising a CORBA::INTERNALLocationCtxtNotFound exception.");
                CORBA::INTERNAL ex(OrbSupport::INTERNALLocationCtxtNotFound, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }
            locctxt = loc_iter->second;
            
 

            // Get the object group's known data: may raise ObjectGroupNotFound.
            ::FT::ObjectGroupId ogid = get_object_group_id_from_ref(object_group);
            ::CdmwFT::ObjectGroupData* ogd = current.get_current_ogd(ogid);
            if (! ogd) {
                CDMW_ERROR(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << ")': ObjectGroup.\n"
                           << "Raising CORBA::INTERNAL() exception." );
                CORBA::INTERNAL ex(OrbSupport::INTERNAL, CORBA::COMPLETED_NO); 
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }

            // And then, get the ObjectGroup state machine context 
            ObjectGroupCtxts::iterator grp_iter = m_grp_ctxts.find(ogid);
            if (grp_iter == m_grp_ctxts.end()) {
                CDMW_ERROR(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << ")': ObjectGroup not found!\n"
                           << "Raising CORBA::INTERNALObjGroupCtxtNotFound exception." );
                CORBA::INTERNAL ex(OrbSupport::INTERNALObjGroupCtxtNotFound, CORBA::COMPLETED_NO); 
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }
            grpctxt = grp_iter->second;

            if (grpctxt->get_locctxt(s_the_location) == NULL)
            {                
                CDMW_INFO(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << ")': the location was not defined for the ObjectGroup.\n"
                          << "Raising ::FT::ObjectNotAdded() exception." );
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }
            

            // Check if member already exists according the replication level
            if (m_replication_level == "STRICT") {
                for (CORBA::ULong i=0; i < ogd->members.length(); ++i) {
                    // Compare the first NameComponent, i.e. the host part of the name.
                    if (Compare_Locations(ogd->members[i].the_location, the_location, 1)) {       
                        CDMW_INFO(FTLogger::GetLogger(),
                                  "add_member(" << s_the_location << ")': A member with the same type is already present on the host.\n"
                                  << "Raising MemberAlreadyPresent() exception." );
                        ::FT::MemberAlreadyPresent ex;
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    }
                }
            }

            // Check if member already exists in this location
            for (CORBA::ULong i = 0;i <ogd->members.length();++i) {
                // Compare the whole locations.
                if (Compare_Locations(ogd->members[i].the_location, the_location)) {
                    CDMW_INFO(FTLogger::GetLogger(),
                              "add_member(" << s_the_location << ")': Member is already present.\n"
                              << "Raising MemberAlreadyPresent() exception." );
                    ::FT::MemberAlreadyPresent ex;
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
            }

            // there are three different security level
            // SAFE_SECURITY_LEVEL : the verification of the type_id of the object is realised all the time 
            // with a call to the is_a method
            // MEDIUM_SECURITY_LEVEL : a first verification is realised by the verification of the type_id 
            // from the IOR => if the type_id is equal, the is_a is not called if not, the is_a is called
            // TRUST_USER_SECURITY_LEVEL : the replication manager don't check the type_id of the object
            //
            // by default, the SAFE_SECUTRITY_LEVEL is used

            // type id
            std::string rep_id = std::string(ogd->repository_id);

            if (m_security_level == "SAFE_SECURITY_LEVEL") {
                // V) Is the member of the right type?
                if (!member->_is_a(rep_id.c_str())) {
                    CDMW_INFO(FTLogger::GetLogger(),
                              "add_member(" << s_the_location << "): Member not of type '"
                              << rep_id << "'!\n"
                              << "Raising ObjectNotAdded() exception." );
                    ::FT::ObjectNotAdded ex;
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }     
            }
            else if (m_security_level == "MEDIUM_SECURITY_LEVEL") {
                if (::Cdmw::OrbSupport::OrbSupport::get_repository_id_from_ior(m_orb.in(), member) 
                    != ::Cdmw::OrbSupport::OrbSupport::get_repository_id_from_ior(m_orb.in(), object_group))
                {
                    // V) Is the member of the right type?
                    if (!member->_is_a(rep_id.c_str())) {      
                        CDMW_INFO(FTLogger::GetLogger(),
                                  "add_member(" << s_the_location << "): Member not of type '"
                                  << rep_id << "'!\n"
                                  << "Raising ObjectNotAdded() exception." );
                        ::FT::ObjectNotAdded ex;
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    }   
                }    
            }

            //
            // Check that the Location DataStoreID set contains the member DataStoreID set:
            // We build a Set < DataStoreIDs > from the object group data, and another Set
            // < DataStoreIDs > from the admin_info and then call std::includes.
            //
            if (loc_info->members_admin_info._d() == CdmwFT::LocationStateness_STATEFULL) {

                std::set < CdmwFT::Transaction::DataStoreID > loc_set;
                std::set < CdmwFT::Transaction::DataStoreID > obj_set;
                
                // Populates the Location set
                const CdmwFT::StateTransfer::LocalDataStoreInfos& loc_ldsi
                    = loc_info->members_admin_info.statefull().local_datastore_infos;
                CORBA::ULong loc_ldsi_len = loc_ldsi.length();
                for (CORBA::ULong i=0; i<loc_ldsi_len; i++)
                {
                    for (CORBA::ULong j = 0;
                         j < loc_ldsi[i].dsids.length();
                         j++)
                    {
                        loc_set.insert(loc_ldsi[i].dsids[j]);
                    }
                }

                // Populates the ObjectGroup set
                const CdmwFT::StateTransfer::DataStoreIDs& obj_ids = ogd->data_store_ids;
                CORBA::ULong obj_ids_len = obj_ids.length();
                for (CORBA::ULong i=0; i<obj_ids_len; i++) {
                    obj_set.insert(obj_ids[i]);
                }

                // Check wether obj_set is included in loc_set (using std::includes)
                bool is_included = std::includes ( loc_set.begin(), loc_set.end()
                                                 , obj_set.begin(), obj_set.end());

                if (! is_included) {  
                    std::ostringstream oss__;
                    oss__ << "add_member(" << s_the_location 
                          << "): some object DataStore IDs are not declared for the location:\n"
                          << "  object DS IDs:\n  ";
                    for (CORBA::ULong i=0; i<obj_ids_len; i++) {
                        oss__ << " " << obj_ids[i];
                    }
                    oss__ << "\n  location DS IDs:\n  ";
                    for (CORBA::ULong i=0; i<loc_ldsi_len; i++)
                    {
                        for (CORBA::ULong j = 0;
                             j < loc_ldsi[i].dsids.length();
                             j++)
                        {
                            oss__ << " " << loc_ldsi[i].dsids[j];
                        }
                    }
                    oss__ << std::endl;
                    oss__ << "Raising ObjectNotAdded() exception.";

                    CDMW_WARN(FTLogger::GetLogger(),oss__.str());
                    ::FT::ObjectNotAdded ex;
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
            }

            // Check for the stateness consistency of the object group
            if (! grpctxt->match_stateness(*locctxt)) {
                CDMW_WARN(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << ")': Location expected to be "
                          << stateness_to_string(ogd->stateness) << ".\n"
                          << "Raising ObjectNotAdded() exception." );
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }

            // Get primary_loc if any
            if (ogd->members.length() == 0) {
                primary_loc = new ::FT::Location(the_location);
                set_group_primary_location(*ogd, primary_loc.in());
                current.modified_ogd(ogid);
            } else {
                primary_loc = get_group_primary_location(*ogd);
            }

            // if all location are in isitial or failed status, the add_member is the first
            // -> it will be the primary
            std::list < Location::Location* > peers_list;
            locctxt->get_peer_locations (peers_list);
            if (peers_list.size() == 0)
                locctxt->backup(false);


            // Create the Object context
            Object::Object* objctxt = new Object::Object(locctxt, grpctxt, this);
            m_obj_ctxts[objctxt->local_id()] = objctxt;

            // Associate the new member to ist group and location
            try {
                grpctxt->associate_object(s_the_location, objctxt);
                locctxt->associate_object(ogid, *objctxt);
            } catch (const ::CORBA::INTERNAL&) {     
                CDMW_WARN(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << "): Could not add member to Location.\n"
                          << "Raising ObjectNotAdded() exception." );
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }

            try {
                // 'ev_add_member' will call add_primary/add_backup on the ObjectGroup context
                locctxt->ev_add_member(*grpctxt, *objctxt, member, primary_loc.in());

               // Get the new IOGR to return it later
                new_object_group = ::FT::ObjectGroup::_duplicate(ogd->object_group_ref.in());
                // Save the new ogd with the member in INITIAL state
                current.save_modified_items();

                // #### This is where the mutex should be unlocked to enable
                //      ReplicationManager method to be reentrant
                if (locctxt->fsm_state() == ::CdmwFT::LocationState_PRIMARY) {
                    // Call update_primary on all Locations containing the group
                    ObjectGroup::ObjectGroup::InfosMap& infos = grpctxt->get_infos();
                    ObjectGroup::ObjectGroup::InfosMap::iterator pos;
                    for (pos = infos.begin(); pos != infos.end(); ++pos) {
                        pos->second.loc->update_primary(primary_loc.in());
                    }
                } else if (locctxt->fsm_state() == ::CdmwFT::LocationState_BACKUP) {
                    // Call update_all_group_views on all Locations containing the group
                    ObjectGroup::ObjectGroup::InfosMap& infos = grpctxt->get_infos();
                    ObjectGroup::ObjectGroup::InfosMap::iterator pos;
                    for (pos = infos.begin(); pos != infos.end(); ++pos) {
                       pos->second.loc->update_all_group_views();
                    }
                }
                // #### This is where the mutex should be relocked

            } catch (const ::FT::PrimaryNotSet & ) {
                CDMW_WARN(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << ")': Could not build new IOGR.\n"
                          << "Raising ObjectNotAdded() exception." );
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            } catch (const ::FT::ObjectNotCreated & ) {
                CDMW_WARN(FTLogger::GetLogger(),
                          "add_member(" << s_the_location << "): Could not build new IOGR.\n"
                          << "Raising ObjectNotAdded() exception." );
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            } catch (const ::CORBA::INTERNAL&) {
                CDMW_WARN(FTLogger::GetLogger(),
                          "add_member(" << locctxt->name() << "): Could not add member to Location.\n"
                          << "Raising ObjectNotAdded() exception." );
                ::FT::ObjectNotAdded ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }

            // Saves the new ogd with the member with its new state in the DataStore
             current.save_modified_items();

             //DEBUG_DUMP("Added member", std::make_pair(ogd->group_id, *ogd));
        } catch (const ::FT::ObjectGroupNotFound& ex) {
            // Rollback to leave the rep.mgr. in a consistent state
            std::ostringstream oss__;
            oss__ << "add_member(";
            if (objctxt) {
                oss__ <<locctxt->name();
                locctxt->disassociate_object(objctxt->ogid());
                grpctxt->disassociate_object(objctxt->locname());
                delete objctxt;
            }
            oss__ << ")\n" << "Raising ObjectGroupNotFound() exception.";
            CDMW_WARN(FTLogger::GetLogger(),oss__.str());
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::FT::PrimaryNotSet & ex) {
            // Rollback to leave the rep.mgr. in a consistent state
            std::ostringstream oss__;
            oss__ << "add_member(";
            if (objctxt) {
                oss__ <<locctxt->name();
                locctxt->disassociate_object(objctxt->ogid());
                grpctxt->disassociate_object(objctxt->locname());
                delete objctxt;
            }
            oss__  << ")\n" << "Raising PrimaryNotSet() exception.";
            CDMW_WARN(FTLogger::GetLogger(),oss__.str());
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::FT::MemberAlreadyPresent& ex) {
            // Rollback to leave the rep.mgr. in a consistent state
            std::ostringstream oss__;
            oss__ << "add_member(";
            if (objctxt) {
                oss__ <<locctxt->name();
                locctxt->disassociate_object(objctxt->ogid());
                grpctxt->disassociate_object(objctxt->locname());
                delete objctxt;
            }
            oss__  << ")\n" << "Raising MemberAlreadyPresent() exception.";
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch ( const CORBA::Exception & ex) {
            // Rollback to leave the rep.mgr. in a consistent state
            std::ostringstream oss__;
            oss__ << "add_member(";
            if (objctxt) {
                oss__ <<locctxt->name();
                locctxt->disassociate_object(objctxt->ogid());
                grpctxt->disassociate_object(objctxt->locname());
                delete objctxt;
            }
            oss__  << ")\n" << "Caught CORBA exception : " << ex << "\nRaising ObjectNotAdded() exception.";
            ::FT::ObjectNotAdded e;
            CDMW_LOG_FUNCTION_EXCEPTION(e);
            throw e;
        } catch (...) {
            // Rollback to leave the rep.mgr. in a consistent state
            std::ostringstream oss__;
            oss__ << "add_member(";
            if (objctxt) {
                oss__ <<locctxt->name();
                locctxt->disassociate_object(objctxt->ogid());
                grpctxt->disassociate_object(objctxt->locname());
                delete objctxt;
            }
            oss__  << ")\n" << "Raising ObjectNotAdded() exception." ;
            ::FT::ObjectNotAdded ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

    ShowContexts();

        return new_object_group._retn();
    }


    /**
     * IDL:omg.org/FT/ObjectGroupManager/remove_member:1.0
     *
     * Allows an application to exercise explicit control over the removal of
     * a member from an object group at a particular location.
     *
     * @param object_group The object group reference of the object group from
     *                     which the member is to be removed.
     * @param the_location The physical location; that is, a fault containment
     *                     region, host, cluster of hosts, etc. of the member
     *                     to be removed.
     *
     * @return the object group reference for the object group with the member
     *         removed. This reference may be the same as that passed in as a
     *         parameter.
     *
     * @exception ObjectGroupNotFound if the object group is not found by the
     *         Replication Manager.
     * @exception MemberNotFound if the Replication Manager cannot find a member
     *         of the object group at the given location.
     */
    ::FT::ObjectGroup_ptr
    ReplicationManager_impl::remove_member (
            ::FT::ObjectGroup_ptr   object_group
          , const ::FT::Location &  the_location)
        throw( ::FT::ObjectGroupNotFound
             , ::FT::MemberNotFound
             , ::CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                          "the_location=" << the_location);
        CDMW_MUTEX_GUARD (m_mutex);

        // Create an empty current data
        CurrentData current(this, &this->m_thread_data);

        // Get the object group id: may raise ObjectGroupNotFound.
        ::FT::ObjectGroupId ogid = get_object_group_id_from_ref(object_group);

        // Actually remove the member
        ::FT::ObjectGroup_var retval = remove_member(ogid, the_location);
        
        // Save the possibly modified ogd and locinfo
        current.save_modified_items();

    ShowContexts();

        return retval._retn();
    }

    /**
     * IDL:omg.org/FT/ObjectGroupManager/set_primary_member:1.0
     *
     * Allows the application to exercise explicit control over the selection
     * of the member of the object group that is to be the primary.
     *
     * @param object_group The object group reference of the object group whose
     *                     primary is to be determined.
     * @param the_location The physical location of the member that is to become
     *                     the primary.
     *
     * @return the object group reference of the object group with the primary
     *         member at the given location. This reference may be the same as
     *         that passed in as a parameter.
     *
     * @exception ObjectGroupNotFound if the object group is not found by the
     *         Replication Manager.
     * @exception MemberNotFound if the Replication Manager cannot find a member of
     *         the object group at that location.
     * @exception PrimaryNotSet if the Replication Manager cannot set the primary
     *         member of the object group.
     * @exception BadReplicationStyle if the <code>ReplicationStyle</code> of the
     *         given group is not <code>COLD_PASSIVE</code> or <code>
     *         WARM_PASSIVE</code>.
     */
    ::FT::ObjectGroup_ptr
    ReplicationManager_impl::set_primary_member(
            ::FT::ObjectGroup_ptr   object_group,
            const ::FT::Location &  the_location)
        throw(::FT::ObjectGroupNotFound,
              ::FT::MemberNotFound,
              ::FT::PrimaryNotSet,
              ::FT::BadReplicationStyle,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),
                          "the_location=" << the_location);
        CDMW_MUTEX_GUARD (m_mutex);
        // NOTE: the replication style is checked in 'set_primary_location'

        CurrentData current(this, &this->m_thread_data);

        // Get the Location context
        std::string s_location;
        try {
            s_location = 
                Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location);
        } catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_primary_member(" << the_location 
                      << ")': Malformed location.\n"
                      << "Raising BAD_PARAMInvalidLocation() exception." );
            CORBA::BAD_PARAM ex ( OrbSupport::BAD_PARAMInvalidLocation
                                  , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;         
        }

        LocationCtxts::iterator locctxtpos = m_loc_ctxts.find(s_location);
        if (locctxtpos == m_loc_ctxts.end()) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_primary_member():"
                      << "Location context \"" << s_location << "\" not found\n"
                      << "Raising ::FT::MemberNotFound exception." );
            ::FT::MemberNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;         
        }
        Location::Location* locctxt = locctxtpos->second;        
        

        if (locctxt->is_primary())
        {
            ::FT::ObjectGroup_var same_primary = ::FT::ObjectGroup::_duplicate(object_group);
             return same_primary._retn();
        }
        else
        {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_primary_location():"
                      << "Raising ::FT::PrimaryNotSet() exception.");
            ::FT::PrimaryNotSet ex; 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;         
        }




        // Check if the requested member is known on that Location
        // May raise ObjectGroupNotFound.
        ::FT::ObjectGroupId ogid = get_object_group_id_from_ref(object_group);

        Object::Object* member = locctxt->get_objctxt(ogid);
        if (! member) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_primary_member(\"" << the_location << "\")\n"
                      << "ObjectGroup context '" << ogid << "' not found\n"
                      << "Raising ::FT::MemberNotFound exception.");
            ::FT::MemberNotFound ex; 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;         
        }

        // Because we want to keep all members of a given Location in
        // the same state, finally switch the whole Location to PRIMARY.
        set_primary_location(locctxt, the_location);

        ::FT::ObjectGroup_var new_iogr;
        try {
            ::CdmwFT::ObjectGroupData* ogd = current.get_current_ogd(ogid);
            new_iogr = ::FT::ObjectGroup::_duplicate(ogd->object_group_ref.in());
        } catch (const ::CORBA::INTERNAL &) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_primary_member(\"" << the_location << "\")\n"
                      << "ObjectGroup context '" << ogid << "' not found\n"
                      << "Raising ::FT::ObjectGroupNotFound() exception.");
            ::FT::ObjectGroupNotFound ex; 
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;         
        }

        current.save_modified_items();
    ShowContexts();
        return new_iogr._retn();
    }

    /**
     * IDL:omg.org/FT/ObjectGroupManager/locations_of_members:1.0
     *
     * Allows the application to determine the locations of the members of the
     * given object group, and the location of the primary member of the group.
     *
     * @param object_group The object group reference of the object group.
     *
     * @return a sequence of locations at which the members of the object group
     *         currently exist. If the object group has the <code>COLD_PASSIVE
     *         </code> or <code>WARM_PASSIVE</code> Replication Style, the first
     *         location in the sequence is the location of the primary.
     *
     * @exception ObjectGroupNotFound if the object group is not found by the
     *         Replication Manager.
     */
    ::FT::Locations *
    ReplicationManager_impl::locations_of_members(
            ::FT::ObjectGroup_ptr object_group)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // Get object group's known data
        // May raise ObjectGroupNotFound.
        // Is the object group known?
        ::FT::ObjectGroupId object_group_id = get_object_group_id_from_ref( object_group );
        OGDStorageObject objStorageObject = m_ogdStorageHome->find_by_oid(object_group_id);        
        ::CdmwFT::ObjectGroupData_var ogd = objStorageObject.get();

        // Get the location of each member and return the list of locations.
        const CORBA::ULong members_length = ogd->members.length();
        ::FT::Locations_var locations = new ::FT::Locations(members_length);
        locations->length(members_length);
        for (CORBA::ULong iMembers = 0; iMembers < members_length; ++iMembers) {
            locations[iMembers] = ogd->members[iMembers].the_location;
        }

        return locations._retn();
    }

    /**
     * IDL:omg.org/FT/ObjectGroupManager/get_object_group_id:1.0
     *
     * Takes a reference for an object group as an in parameter, and returns the
     * identifier of the object group.
     *
     * @param object_group The object group reference for the object group.
     *
     * @return the object group identifier for the object group.
     *
     * @exception ObjectGroupNotFound if the object group is not found by the
     *         Replication Manager.
     */
    ::FT::ObjectGroupId
    ReplicationManager_impl::get_object_group_id(
            ::FT::ObjectGroup_ptr object_group)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        ::FT::ObjectGroupId ogid = get_object_group_id_from_ref(object_group);
        CDMW_LOG_FUNCTION_RETURN(ogid);
        return ogid;
    }

    /**
     * IDL:omg.org/FT/ObjectGroupManager/get_object_group_ref:1.0
     *
     * Takes a reference for an object group as an in parameter, and returns the
     * current reference for the object group.
     *
     * @param object_group An object group reference for the object group.
     *
     * @return the current object group reference for the object group. The
     *         returned reference may be the same as the reference passed in as
     *         a parameter.
     *
     * @exception ObjectGroupNotFound if the object group is not found by the
     *         Replication Manager.
     */
    ::FT::ObjectGroup_ptr
    ReplicationManager_impl::get_object_group_ref(
            ::FT::ObjectGroup_ptr object_group)
        throw(::FT::ObjectGroupNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // Get all known data of the object group
        // May raise ::FT::ObjectGroupNotFound
        ::FT::ObjectGroupId object_group_id = get_object_group_id_from_ref( object_group );
        OGDStorageObject objStorageObject = m_ogdStorageHome->find_by_oid(object_group_id);        
        ::CdmwFT::ObjectGroupData_var ogd = objStorageObject.get();

        ::FT::ObjectGroup_var obj = ogd->object_group_ref;
        return obj._retn();
    }

    /**
     * IDL:omg.org/FT/ObjectGroupManager/get_member_ref:1.0
     *
     * Takes a reference for an object group and a location as in parameters,
     * and returns a reference for the member.
     *
     * @param object_group An object group reference for the object group.
     * @param loc          The location of the member.
     * 
     * @return the reference for the member.
     *
     * @exception ObjectGroupNotFound if the object group is not found by the
     *         Replication Manager.
     * @exception MemberNotFound if the member is not found by the Replication
     *         Manager.
     */
    CORBA::Object_ptr
     ReplicationManager_impl::get_member_ref(
            ::FT::ObjectGroup_ptr object_group,
            const ::FT::Location & loc)
        throw(::FT::ObjectGroupNotFound,
              ::FT::MemberNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        // Get all known data of the object group
        // May raise ::FT::ObjectGroupNotFound
        ::FT::ObjectGroupId object_group_id = get_object_group_id_from_ref( object_group );
        OGDStorageObject objStorageObject = m_ogdStorageHome->find_by_oid(object_group_id);        
        ::CdmwFT::ObjectGroupData_var ogd = objStorageObject.get();

        // Find the member at the given location.
        CORBA::Object_var member;
        CORBA::ULong i = 0; 
        const CORBA::ULong members_length = ogd->members.length();
        bool member_found = false;
        using namespace Cdmw::CommonSvcs::Naming;
        const std::string s_loc = NamingInterface::to_string(loc);
        while (!member_found && (i < members_length)) {
            std::string s_member_loc = 
                NamingInterface::to_string(ogd->members[i].the_location);
            if (s_member_loc == s_loc) {
                member_found = true;
                member = ogd->members[i].the_reference;
            }
            ++i;
        }
        if (!member_found) {
            throw ::FT::MemberNotFound();
        }
        
        return member._retn();
    }

    /**
     * IDL:omg.org/FT/GenericFactory/create_object:1.0
     *
     * Creates an object, using the <code>type_id</code> parameter to determine
     * which type of object to create and <code>the_criteria</code> parameter to
     * determine restrictions on how and where to create the object. The out
     * parameter, <code>factory_creation_id</code>, allows the entity that
     * invoked the factory, and the factory itself, to identify the object for
     * subsequent deletion. If the application invokes the <code>create_object()
     * </code> operation on the <code>GenericFactory</code> interface, inherited
     * by the Replication Manager, then it creates an object group. For an
     * object group with the <code>MEMB_APP_CTRL MembershipStyle</code>, the
     * Replication Manager returns an object group reference containing only the
     * <code>TAG_MULTIPLE_COMPONENTS</code> profile with the <code>TAG_FT_GROUP
     * </code> component in it.
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
     *         inherited by the Replication Manager, the <code>create_object()
     *         </code> operation returns an object group reference as a result.
     *
     * @exception NoFactory if the object cannot be created. When the <code>
     *         GenericFactory</code> interface is inherited by the Replication
     *         Manager, the raised exception indicates that the Replication
     *         Manager cannot create the object group because it cannot find a
     *         factory that is capable of constructing a member of the object
     *         group of the <code>type_id</code> at the location.
     * @exception ObjectNotCreated if the factory cannot create the object.
     * @exception InvalidCriteria if the factory does not understand the criteria.
     * @exception InvalidProperty if a property passed in as criteria is invalid.
     * @exception CannotMeetCriteria if the factory understands the criteria but
     *         cannot satisfy it.
     */

    struct TmpLocInfo {

        TmpLocInfo() : locctxt(NULL) {
        }

        ~TmpLocInfo() {
        if ((!found) && (locctxt))  delete locctxt;
        };

        bool                     found;        
        std::string              locname;
        CdmwFT::LocationInfo_var locinfo;
        Location::Location*      locctxt;
    };

    CORBA::Object_ptr
    ReplicationManager_impl::create_object(
            const char * type_id,
            const ::FT::Criteria & the_criteria,
            ::FT::GenericFactory::FactoryCreationId_out factory_creation_id)
        throw(::FT::NoFactory,
              ::FT::ObjectNotCreated,
              ::FT::InvalidCriteria,
              ::FT::InvalidProperty,
              ::FT::CannotMeetCriteria,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"type_id="<<type_id);
        CDMW_MUTEX_GUARD (m_mutex);
        ::FT::ObjectGroupId object_group_id;
        CORBA::Object_var object_group = CORBA::Object::_nil();
        // Create an empty current data
        // AFAIK we don't need it here...
        // CurrentData current(this, &this->m_thread_data);

        // TODO: check that type_id is not empty!
        try {

            // Create an ObjectGroupData object
            CdmwFT::ObjectGroupData_var ogd = new CdmwFT::ObjectGroupData();
            
            // The stateness of the group is determined if datastore id are present 
            // on the criteria. Set to STATELESS
            // by default.
            ogd->stateness = CdmwFT::GroupStateness_STATELESS;
            ::FT::ObjectGroupId current_gid;
            try {
                OGIDStorageObject ogid = m_ogidStorageHome->find_by_oid("ogid"); 
                current_gid = ogid.get();
            } catch( Cdmw::FT::NotFoundException) {
                OGIDStorageObject ogid = m_ogidStorageHome->create("ogid", 0);
                current_gid = ogid.get();
            }

            // the object_group_if from ffffff01 to ffffff0f are reserved for internal used        
            if ( current_gid == 0xffffff00) {            
                current_gid = 0xffffff0f;           
            }
            
            object_group_id = ++current_gid;
            ogd->group_id   = object_group_id;
            CORBA::ULong object_group_ref_version = 0L;
            
            ::CdmwFT::MemberInfos no_members;
            no_members.length(0 );
            ::FT::Location no_location;
            no_location.length(0 );
          
            // Build an oid using the pattern : group_id/group_ver.
            ::FT::ObjectGroupRefVersion group_version = object_group_ref_version + 1;
            FallbackOid fallbackOid(object_group_id, group_version, type_id);
            PortableServer::ObjectId_var oid = fallbackOid.to_ObjectId();
            
            CORBA::Object_var replication_manager_ref
                = m_poa->create_reference_with_id(oid.in(),type_id);
            
            ::CdmwFT::Objects_var fallBack = new ::CdmwFT::Objects;     
            fallBack->length(1);
            fallBack[0L] = CORBA::Object::_duplicate(replication_manager_ref.in());
            
            // may throw ::FT::PrimaryNotSet or ::FT::ObjectNotCreated
            object_group = m_iogrFactory->build_iogr(no_members,
                                                     no_location,
                                                     object_group_id,
                                                     type_id,
                                                     m_ft_domain_id.in(),
                                                     object_group_ref_version,
                                                     fallBack.in());
            

            ogd->repository_id    = type_id;
            ogd->object_group_ref = CORBA::Object::_duplicate(object_group.in());
            ogd->object_group_ref_version = object_group_ref_version;
            
            
            // Create an ObjectGroup context instance.
            ObjectGroup::ObjectGroup* objgrp = new ObjectGroup::ObjectGroup(object_group_id, this);
            std::auto_ptr < ObjectGroup::ObjectGroup > auto_objgrp(objgrp);

            ogd->state = objgrp->fsm_state();;
            ogd->fsm_internal_state = objgrp->fsm_internal_state();


            //
            // Look for criteria named:
            // - "org.omg.ft.FTProperties",
            // - "com.thalesgroup.cdmw.ft.DataStoreIdentifiers",
            // - "com.thalesgroup.cdmw.ft.Name"
            //
            // Following loop may construct some local data instances. They 
            // are temporarily saved in adequate std::list or var types so
            // that they are automatically released in case of an exception.
            // These data are saved either in members containers or datastore
            // later if no exception occured. This way the code has a better
            // (though not complete) exception safetyness.
            //

           std::list < TmpLocInfo > locinfo_list;
            const ::FT::Properties*  ft_properties = NULL;
            ::CdmwFT::StateTransfer::DataStoreIDs* dsids = NULL;
            CORBA::UShort minimumNumberReplicas = 0;

            CORBA::ULong i = 0, criteria_length = the_criteria.length();
            bool factories_found = false;

            bool reply_logging_is_used = false;

            while (i < criteria_length) {
                if (strcmp(the_criteria[i].nam[0].id,
                       "org.omg.ft.FTProperties") == 0) {

                    // Criteria "org.omg.ft.FTProperties" found: the extractor
                    // returns a pointer to the sequence embedded in the any,
                    // no need to save the pointer in a _var type.
                    if (the_criteria[i].val >>= ft_properties) {
                        try {
                            // Sanity check.
                            check_properties(*ft_properties, CREATION_PROPERTIES);
                            

                            //extract properties
                            CORBA::ULong props_length = ft_properties->length();
                            for (CORBA::ULong iProps = 0; iProps < props_length; ++iProps) {
                                // Get the property name.
                                ::FT::Property property = (*ft_properties)[iProps];
                                std::string propertyName = std::string(property.nam[0].id);
                                
                                if (propertyName == "org.omg.ft.MinimumNumberReplicas"){
                                    if(property.val >>= minimumNumberReplicas){
                                        objgrp->min_members_count(minimumNumberReplicas);
                                        ogd->min_nb_replicas = minimumNumberReplicas;
                                    } else {
                                        CDMW_WARN(FTLogger::GetLogger(),
                                                  "create_object(" << type_id << ") : Bad type in criteria["
                                                  << i << "]\n"
                                                  << "raising InvalidProperty('org.omg.ft.MinimumNumberReplicas') exception.");
                                        ::FT::InvalidProperty ex(property.nam,property.val);
                                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                                        throw ex;         
                                    }

                                } else if (propertyName == "org.omg.ft.Factories") {
                                    // Extract the FT::Location from the criteria
                                  const ::FT::FactoryInfos* factoryInfos = NULL;
                                    if (property.val >>= factoryInfos) {

                                        CORBA::ULong factoryInfos_length = factoryInfos->length();
                                        for (  CORBA::ULong iFactories = 0
                                            ; iFactories < factoryInfos_length
                                            ; ++iFactories
                                            )
                                        {
                                            factories_found = true;
                                            std::string str_location;
                                            try {
                                                str_location
                                                  = Cdmw::CommonSvcs::Naming::NamingInterface::to_string((*factoryInfos)[iFactories].the_location);
                                            }
                                            catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
                                                CDMW_WARN(FTLogger::GetLogger(),
                                                          "create_object(" << str_location << ")': Malformed location.\n"
                                                          << "Raising BAD_PARAMInvalidLocation() exception.");
                                                CORBA::BAD_PARAM ex( OrbSupport::BAD_PARAMInvalidLocation
                                                                     , CORBA::COMPLETED_NO);
                                                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                                                throw ex;         
                                            }
                                            LocationCtxts::iterator iter = m_loc_ctxts.find(str_location);
                                            TmpLocInfo tmp_locinfo;
                                            locinfo_list.push_back(tmp_locinfo);
                                            locinfo_list.back().locname = str_location;

                                            if (iter != m_loc_ctxts.end()) {

                                                CdmwFT::LocationInfo_var loc_info_ = m_locInfoStorageHome->find_data_by_oid(str_location);
                                                if (loc_info_->reply_logging)
                                                {
                                                    reply_logging_is_used = true;
                                                }
                                                
                                                locinfo_list.back().found = true;
                                                locinfo_list.back().locinfo = NULL;
                                                locinfo_list.back().locctxt = (*iter).second;                                                
                                            } else {
                                                locinfo_list.back().found = false;
                                                locinfo_list.back().locinfo =  new ::CdmwFT::LocationInfo(); 
                                                locinfo_list.back().locinfo->has_members_admin = false;
                                                locinfo_list.back().locinfo->fsm_internal_state = 0;
                                                locinfo_list.back().locinfo->reply_logging = false;
                                                locinfo_list.back().locinfo->state
                                                    = ::CdmwFT::LocationState_INITIAL;
                                                CdmwFT::StatelessMembersAdminInfo stateless_admin_info;
                                                // Initialize the union (discriminant set to STATELESS)
                                                locinfo_list.back().locinfo->members_admin_info.stateless(stateless_admin_info);
                                                locinfo_list.back().locctxt
                                                    = new Location::Location(str_location, this);
                                            }
                                        }
                                    } else {
                                        CDMW_WARN(FTLogger::GetLogger(),
                                                  "create_object(" << type_id << ") : Bad type in criteria["
                                                  << i << "]\n"
                                                  << "raising InvalidProperty('org.omg.ft.Factories') exception.");
                                        ::FT::InvalidProperty ex(the_criteria[i].nam,the_criteria[i].val);
                                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                                        throw ex;         
                                    }
                                }
                            }
                            
                        }
                        catch (const ::FT::UnsupportedProperty & ex) {
                            CDMW_WARN(FTLogger::GetLogger(),
                                      "create_object(" << type_id << ") : Unsupported property : '"
                                      << ex.nam[0].id << "' \n"
                                      << "raising InvalidProperty() exception.");
                            ::FT::InvalidProperty ex(ex.nam,ex.val);
                            CDMW_LOG_FUNCTION_EXCEPTION(ex);
                            throw ex;         
                        }
                    }
                    else {
                        CDMW_WARN(FTLogger::GetLogger(),
                                  "create_object(" << type_id << ") : Bad type in criteria[" << i << "]\n"
                                  << "raising InvalidProperty('" 
                                  << the_criteria[i].nam[0].id << "') exception." );
                        ::FT::InvalidProperty ex(the_criteria[i].nam,the_criteria[i].val);
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;         
                    }
                }

                if (strcmp(the_criteria[i].nam[0].id,
                       "com.thalesgroup.cdmw.ft.DataStoreIdentifiers") == 0) {
                
                    if (the_criteria[i].val >>= dsids) {
                        ogd->stateness = CdmwFT::GroupStateness_STATEFULL;
                    }
                    else {
                        CDMW_WARN(FTLogger::GetLogger(),
                                  "create_object(" << type_id << ") : Bad type in criteria[" << i << "]\n"
                                  << "raising InvalidProperty('om.thalesgroup.cdmw.ft.DataStoreIdentifiers') exception." );
                        ::FT::InvalidProperty ex(the_criteria[i].nam,the_criteria[i].val);
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;         
                    }
                }

                if (reply_logging_is_used)
                {
                    if (dsids == NULL)
                    {
                        dsids = new ::CdmwFT::StateTransfer::DataStoreIDs(1);
                        dsids->length(1);
                        (*dsids)[0] = Cdmw::FT::FT_REPLY_DATASTORE_ID;
                        ogd->stateness = CdmwFT::GroupStateness_STATEFULL;
                    }
                    else
                    {
                        unsigned long length = dsids->length();
                        dsids->length(length + 1);
                        (*dsids)[length] = Cdmw::FT::FT_REPLY_DATASTORE_ID;
                    }
                    ogd->init_reply_logging = true;
                } else {
                    ogd->init_reply_logging = false;
                }
                

                if (strcmp(the_criteria[i].nam[0].id,
                           "com.thalesgroup.cdmw.ft.Name") == 0) {
                    const ::FT::Name* objectGroupName;
                    if (the_criteria[i].val >>= objectGroupName) {

                        std::string s_name;
                        try {
                            s_name = 
                            Cdmw::CommonSvcs::Naming::NamingInterface::to_string(*objectGroupName);
                        }
                        catch (Cdmw::CommonSvcs::Naming::InvalidNameException& e) {
                            CDMW_WARN(FTLogger::GetLogger(),
                                      "create_object(" << type_id << ") : Invalid name '" << e << "' for 'com.thalesgroup.cdmw.ft.Name' property\n"
                                      << "raising CORBA::BAD_PARAM exception." );
                                      
                            
                            CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMInvalidLocation, CORBA::COMPLETED_NO);
                            CDMW_LOG_FUNCTION_EXCEPTION(ex);
                            throw ex;         
                        }
                        
                        std::vector<OGDStorageObject> ogd_list;
                        m_ogdStorageHome->find_if
                            (std::bind2nd
                             (is_object_group_from_name < std::pair < Cdmw::OrbSupport::type_traits < ::FT::ObjectGroupId >::_var_type, 
                              Cdmw::OrbSupport::type_traits < ::CdmwFT::ObjectGroupData >::_var_type > > (),
                              s_name),
                             std::back_inserter(ogd_list));
                        
                        if (ogd_list.size() != 0)
                            throw ::FT::ObjectNotCreated(); 


                        ogd->alias = s_name.c_str();
                    }
                    else {
                        CDMW_WARN(FTLogger::GetLogger(),
                                  "create_object(" << type_id << ") : Bad type in criteria[" << i << "]\n"
                                  << "raising InvalidProperty() exception.");
                        ::FT::InvalidProperty ex(the_criteria[i].nam,the_criteria[i].val);
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;         
                    }
                }


                ++i;
            }

           if ( factories_found == false)
            {
                CDMW_WARN(FTLogger::GetLogger(),
                          "create_object(" << type_id << ") : Bad type in criteria[" << i << "]\n"
                          << "org.omg.ft.Factories was not defined" 
                          << "raising ::FT::NoFactory() exception." );
                ::FT::NoFactory ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }
            

            // Assign the object group id to Set the FactoryCreationId to the 
            // ObjectGroupId to simplify the implementation of delete_object().
            CORBA::Any factory_id;
            factory_id <<= object_group_id;
            factory_creation_id = new CORBA::Any(factory_id);

            // NOTE: all locally saved data are now saved either in "standard"
            //       containers members or data stores. Operations on "standard"
            //       containers are unlikely to throw exception (although it is
            //       not guaranteed). Grouping all savings here gives a better
            //       exception safetyness.
            //       
            
            // Save the ObjectGroup pointer in our map
            m_grp_ctxts[object_group_id] = auto_objgrp.release();

            // Save the FT propertie sequence (deep copy)
            if (ft_properties) ogd->creation_properties = *ft_properties;

            // Save the DataStoreIdentifiers sequence (deep copy)
            if (dsids) ogd->data_store_ids = *dsids;

            // Associates ObjectGroups and Locations.
            // Save the location infos in the DataStore. 
            while (! locinfo_list.empty()) {
                TmpLocInfo& tmplocinfo = locinfo_list.front();

                // Associate the ObjectGroup to the Location
                tmplocinfo.locctxt->associate_object_group(object_group_id, *objgrp);

                // Associate the Location to the ObjectGroup
                objgrp->associate_location ( tmplocinfo.locname
                                           , tmplocinfo.locctxt);

                // Send an event "create_object" to the Location in order to
                // increment the number of expected add_member calls
                tmplocinfo.locctxt->ev_create_object();

                if (!tmplocinfo.found) { 
                    // Save the Location in our map
                    m_loc_ctxts[tmplocinfo.locname] = tmplocinfo.locctxt;

                    try {
                    // Save the LocationInfo instance in the DataStore
                    m_locInfoStorageHome->create ( tmplocinfo.locname
                                                 , tmplocinfo.locinfo.in());
                    } catch(const Cdmw::FT::AlreadyExistException& e) {
                        LocInfoStorageObject obj = m_locInfoStorageHome->find_by_oid(tmplocinfo.locname);
                        obj.set(tmplocinfo.locinfo.in());
                    }
                } 
                
                // Clear the locctxt pointer to avoid its deletion when
                // locinfo_list is destroyed.
                tmplocinfo.locctxt = NULL;

                // Remove the TmPpLocInfo from the temporary list
                locinfo_list.pop_front();
            }

            // Save the new current ObjectGroupID
            OGIDStorageObject ogid = m_ogidStorageHome->find_by_oid("ogid"); 
            ogid.set(current_gid);

            // Copy the ObjectGroupData local instance
            m_ogdStorageHome->create(object_group_id, ogd.in());

            DEBUG_DUMP("Object created",std::make_pair(object_group_id,ogd));
    ShowContexts();
        }        
        catch(const Cdmw::FT::AlreadyExistException& e)
        {
            ::FT::ObjectNotCreated ex;  
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;                   
        }
        catch(const Cdmw::FT::NotFoundException& e)
        {
            ::FT::ObjectNotCreated ex;    
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;                 
        }
        
        return object_group._retn();
    }


    /**
     * IDL:omg.org/FT/GenericFactory/delete_object:1.0
     *
     * Deletes the object with the given identifier. If the application invokes
     * this operation on the <code>GenericFactory</code> interface, inherited
     * by the Replication Manager, then it deletes an object group. When this
     * operation is invoked on it, the Replication Manager must first remove
     * each of the members from the object group, and delete each of them,
     * before it deletes the object group itself.
     *
     * @param factory_creation_id An identifier for the object that is to be
     *                            deleted.
     *
     * @exception ObjectNotFound if the object cannot be found.
     */
    void
    ReplicationManager_impl::delete_object(
            const ::FT::GenericFactory::FactoryCreationId & factory_creation_id)
        throw(::FT::ObjectNotFound,
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        //
        // 1- find the ObjectGroup context
        // 2- check if the group is empty
        //    IF ! empty 
        //      THROW BAD_INV_ORDER
        // 3- completely disassociate the group
        // 4- cleanup the ObjectGroupData data store
        // 5- update the group view on all replicated Locations
        //
        CDMW_MUTEX_GUARD (m_mutex);

        // Create an empty current data
        CurrentData current(this, &this->m_thread_data);

 ShowContexts();
        // Find the ObjectGroup context
        ::FT::ObjectGroupId ogid;
        if (factory_creation_id >>= ogid) {
            try {                

                // 1- find the ObjectGroup context
                ObjectGroupCtxts::iterator pos = m_grp_ctxts.find(ogid);
                if (pos == m_grp_ctxts.end()) {
                    // Internal error, should not happen
                    CDMW_WARN(FTLogger::GetLogger(),
                              "delete_object():"
                              << "ObjectGroup " << ogid << " not found \n"
                              << "Raising ::FT::ObjectNotFound exception." );
                    ::FT::ObjectNotFound ex;
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;         
                }
                ObjectGroup::ObjectGroup* objgrp = pos->second;
                if (! objgrp) {
                    // Internal error, should not happen
                    CDMW_ERROR(FTLogger::GetLogger(),
                               "delete_object():"
                               << "ObjectGroup " << ogid << " NULL pointer\n"
                               << "Raising CORBA::INTERNAL exception." );
                    CORBA::INTERNAL ex( ::Cdmw::OrbSupport::INTERNALNullObjGroupCtxtPtr
                                        , CORBA::COMPLETED_NO);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;         
                }

#if ENABLE_ACCEPT_DELETE_OBJECT_IF_EMPTY_GROUP
                // 2- check if the group is empty
                
                ObjectGroup::ObjectGroup::InfosMap::iterator iter;
                ObjectGroup::ObjectGroup::InfosMap& map = objgrp->get_infos();
                for (iter = map.begin();
                     iter != map.end();
                     ++iter
                    ) {
                    if ((*iter).second.obj != 0) {
                        // Illegal sequence of invocations: group should be empty
                        CDMW_WARN(FTLogger::GetLogger(),
                                  "delete_object():"
                                  << "ObjectGroup " << ogid << " is not empty\n"
                                  << "Raising CORBA::INTERNAL exception.");
                        CORBA::BAD_INV_ORDER ex(::Cdmw::OrbSupport::BAD_INV_ORDERObjectGroupNotEmpty
                                                , CORBA::COMPLETED_NO);
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;         
                    }
                }
                
#endif
                // 3- completely disassociate the group: this does not 
                //    change state of containing Locations
                objgrp->disassociate();

                // 4- cleanup the ObjectGroupData data store
                m_ogdStorageHome->remove(ogid);

                // 5- update the group views for each replicated Location
                std::list < Location::Location* > peers_list;
                objgrp->get_peer_locations (peers_list);
                std::list < Location::Location* > :: iterator peer_pos;
                peer_pos = peers_list.begin();
                while (peer_pos != peers_list.end()) {
                    (*peer_pos)->disassociate_object_group(ogid);
//                    (*peer_pos)->update_all_group_views();
                    ++peer_pos;
                }
                delete objgrp; // Removes the item from its container
                pos->second=NULL;
            }
            catch (const ::Cdmw::FT::NotFoundException& e) {
                CDMW_DEBUG(FTLogger::GetLogger(),e);
                ::FT::ObjectNotFound ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;         
            }
            
            DEBUG_DUMPALL("Object deleted");
            
        } else {
            ::FT::ObjectNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;         
        }

        // Saves the new ogd with the member with its new state in the DataStore
        current.save_modified_items();
        
        ShowContexts();
    }


    /**
     * IDL:thalesgroup.com/CdmwFT/ReplicationManager/register_fault_detector:1.0
     *
     * Register the CDMW Fault Detector object 
     *
     * @param fault_detector Object Reference of the CDMW Fault Notifier object
     *
     */  
     void 
     ReplicationManager_impl::register_fault_detector
     (CdmwFT::FaultManagement::FaultDetector_ptr fault_detector)
         throw (CORBA::SystemException)
     {
         CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
         CDMW_MUTEX_GUARD (m_mutex);
         m_fault_detector = CdmwFT::FaultManagement::FaultDetector::_duplicate(fault_detector);
     }


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
    CdmwFT::FaultManagement::FaultDetector_ptr 
    ReplicationManager_impl::get_fault_detector()
        throw(::FT::InterfaceNotFound)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        if (CORBA::is_nil(m_fault_detector.in())) {
            ::FT::InterfaceNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        return CdmwFT::FaultManagement::FaultDetector::_duplicate(m_fault_detector.in());
    }


    /**
     * Registers a CDMW MemebersAdmin object of a specific location. If a location
     * is already registered, the new registration will override the previous one.
     *
     * @param location A CDMW Location
     * @param members_admin Object Reference of the MembersAdmin object of that location.
     *
     */        
    void 
    ReplicationManager_impl::register_location (
            const ::FT::Location &               the_location,
            ::CdmwFT::Location::MembersAdmin_ptr members_admin)
        throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
        CDMW_MUTEX_GUARD (m_mutex);

        std::string s_location;
        try {
            s_location = 
                Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location);
        }
        catch (Cdmw::CommonSvcs::Naming::InvalidNameException& e) {
            CDMW_DEBUG(FTLogger::GetLogger(),e);
            CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMInvalidLocation, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        /*
         * Check if the Location state machine already exists,
         * if yes, simply keeps it. If no, creates a new one.
         *
         * NOTE: if the Location already exists, this is because it has been declared
         *       in the criteria passed to create_object. In that case, the Location
         *       has been associated to its ObjectGroups. If the Location does not
         *       exist, then it means that it is registered before any ObjectGroup is
         *       created, and it is therefore not needed to try to associate.
         */
        Location::Location* locsm;      
        LocationCtxts::iterator pos = m_loc_ctxts.find(s_location);
        if (pos == m_loc_ctxts.end()) {
            // Create a new Location context
            locsm = new Location::Location(s_location, this);
            locsm->stateness(::CdmwFT::LocationStateness_STATELESS);
        } else {
            locsm = (*pos).second;
            if ((locsm->fsm_state() != ::CdmwFT::LocationState_INITIAL) &&
                (locsm->fsm_state() != ::CdmwFT::LocationState_FAILED)
                )
            {
                CDMW_WARN(FTLogger::GetLogger(),
                          "register_location():"
                          << "Location :" << s_location << " is still running!\n"
                          << "Raising CORBA::TRANSIENT exception." );
                CORBA::TRANSIENT ex( ::Cdmw::OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
                
            }

//            if (locsm->fsm_state() == ::CdmwFT::LocationState_FAILED)
                locsm->reset();
            locsm->stateness(::CdmwFT::LocationStateness_STATELESS);
        }


        CdmwFT::LocationInfo_var loc_info;
        bool location_is_found = false;
        // check if the LocationInfo already exists
        try {
            LocInfoStorageObject obj = m_locInfoStorageHome->find_by_oid(s_location);
            loc_info = obj.get();
            location_is_found = true;
        }
        catch(const ::Cdmw::FT::NotFoundException&) {
            // do nothing
            // "#### FIXME: Check that the admin has still the correct type"
            loc_info = new CdmwFT::LocationInfo();
        }
        
        // Set the stateless location info
        loc_info->state    = ::CdmwFT::LocationState_INITIAL;
        locsm->fsm_state(::CdmwFT::LocationState_INITIAL);
        loc_info->fsm_internal_state = 0;
        loc_info->has_members_admin = true;
        CdmwFT::StatelessMembersAdminInfo stateless_admin_info;
        // Initialize the union (discriminant set to STATELESS)
        loc_info->members_admin_info.stateless(stateless_admin_info);
        loc_info->members_admin_info.stateless().admin
            = ::CdmwFT::Location::MembersAdmin::_duplicate(members_admin);
        loc_info->stateness = CdmwFT::LocationStateness_STATELESS;
        loc_info->reply_logging = false;
        // Save the Location in the Rep. Mgr. map
        m_loc_ctxts[s_location] = locsm;

        // Save the location info in the data store
        if (location_is_found)
        {
            LocInfoStorageObject obj = m_locInfoStorageHome->find_by_oid(s_location);
            obj.set(loc_info.in());
        } else {
            m_locInfoStorageHome->create(s_location, loc_info.in());
        }
        
        ShowContexts();
    }


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
    void 
    ReplicationManager_impl::register_statefull_location (
                           const ::FT::Location&                               the_location,
                           ::CdmwFT::Location::StatefullPrimaryBackupAdmin_ptr statefull_admin,
                           const CdmwFT::StateTransfer::LocalDataStoreInfos&   local_datastore_infos,
                           CORBA::Boolean                                      with_reply_logging)
        throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location<<",with_reply_logging="<<with_reply_logging);
        CDMW_MUTEX_GUARD (m_mutex);
        
        std::string s_location;
        try {
            s_location = 
                Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location);
            
        }
        catch (Cdmw::CommonSvcs::Naming::InvalidNameException& e) {
            CDMW_DEBUG(FTLogger::GetLogger(),e);
            CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMInvalidLocation, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        Location::Location* locsm;      
        LocationCtxts::iterator pos = m_loc_ctxts.find(s_location);
        if (pos == m_loc_ctxts.end()) {
            // Create a new Location context
            locsm = new Location::Location(s_location, this);
            locsm->stateness(::CdmwFT::LocationStateness_STATEFULL);
        } else {
           locsm = (*pos).second;
            if ((locsm->fsm_state() != ::CdmwFT::LocationState_INITIAL) &&
                (locsm->fsm_state() != ::CdmwFT::LocationState_FAILED)
                )
            {
                CDMW_WARN(FTLogger::GetLogger(),
                          "register_location():"
                          << "Location :" << s_location << " is still running!\n"
                          << "Raising CORBA::TRANSIENT exception." );
                CORBA::TRANSIENT ex( ::Cdmw::OrbSupport::TRANSIENT, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
//            if (locsm->fsm_state() == ::CdmwFT::LocationState_FAILED)
            locsm->reset();
            locsm->stateness(::CdmwFT::LocationStateness_STATEFULL);

            // if the location used the reply logging, find all the ogd for this location
            // and add the disd for the logging datastore
            if (with_reply_logging)
            {
                bool reply_datastore_register = false;
                Location::Location::InfosMap& locmap = locsm->get_infos();
                Location::Location::InfosMap::iterator loc_iter;
                for (loc_iter = locmap.begin(); loc_iter != locmap.end(); ++loc_iter) {
                    ::CdmwFT::ObjectGroupData_var ogd;;
                    OGDStorageObject objStorageObject = m_ogdStorageHome->find_by_oid((*loc_iter).first);
                    ogd = objStorageObject.get();
                    if (!ogd->init_reply_logging)
                    {
                        if (!reply_datastore_register)
                        {
                            if (ogd->stateness == CdmwFT::GroupStateness_STATELESS)
                            {
                                ogd->data_store_ids.length(1);
                                ogd->data_store_ids[0] = Cdmw::FT::FT_REPLY_DATASTORE_ID;
                                ogd->stateness = CdmwFT::GroupStateness_STATEFULL;
                            } else {
                                unsigned long length = ogd->data_store_ids.length();
                                ogd->data_store_ids.length(length + 1);
                                ogd->data_store_ids[length] = Cdmw::FT::FT_REPLY_DATASTORE_ID;
                            }
                            reply_datastore_register = true;
                        }
                        ogd->init_reply_logging = true;
                        objStorageObject.set(*ogd);
                    }
                    
                    
                }
            }
        }

        CdmwFT::LocationInfo_var loc_info;
        bool location_is_found = false;
        // check if the LocationInfo already exists
        try {
            LocInfoStorageObject obj = m_locInfoStorageHome->find_by_oid(s_location);
            loc_info = obj.get();
            location_is_found = true;
        }
        catch(const ::Cdmw::FT::NotFoundException&) {
            // do nothing
            // "#### FIXME: Check that the admin has still the correct type"
            loc_info = new CdmwFT::LocationInfo();
        }


        // Set the statefull location info
        loc_info->state = ::CdmwFT::LocationState_INITIAL;
        locsm->fsm_state(::CdmwFT::LocationState_INITIAL);
        loc_info->has_members_admin = true;
        CdmwFT::StatefullMembersAdminInfo statefull_admin_info;
        loc_info->members_admin_info.statefull(statefull_admin_info);
        loc_info->members_admin_info.statefull().admin
            = ::CdmwFT::Location::StatefullPrimaryBackupAdmin::_duplicate(statefull_admin);
        loc_info->members_admin_info.statefull().local_datastore_infos = local_datastore_infos;
        loc_info->stateness = CdmwFT::LocationStateness_STATEFULL;
        loc_info->reply_logging = with_reply_logging;
        

        // Save the Location in the Rep. Mgr. map
        m_loc_ctxts[s_location] = locsm;

        // Save the location info in the data store
        if (location_is_found)
        {
            LocInfoStorageObject obj = m_locInfoStorageHome->find_by_oid(s_location);
            obj.set(loc_info.in());
        } else {
            m_locInfoStorageHome->create(s_location, loc_info.in());
        }

    }


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
    void 
    ReplicationManager_impl::offer_change (const CosNotification::EventTypeSeq & added,
                                           const CosNotification::EventTypeSeq & removed)
        throw (CosNotifyComm::InvalidEventType,
               CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        CORBA::NO_IMPLEMENT ex(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }

    /// Receive one structured event
    /**
     * IDL:omg.org/CosNotifyComm/StructuredPushConsumer/push_structered_event:1.0
     *
     *  This operation is invoked to provide one event to the
     * consumer.
     * @exception CosEventComm::Disconnected if the object considers
     *   itself no longer connected to its peer.
     */
    void 
    ReplicationManager_impl::push_structured_event
    (const CosNotification::StructuredEvent & notification)
        throw (CosEventComm::Disconnected,
               CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        std::string _location_str ="";
        try
        {
            CDMW_MUTEX_GUARD (m_mutex);
        // Expected format is :
//         CosNotification::StructuredEvent fault_event;
//         fault_event.header.fixed_header.event_type.domain_name = "FT_CORBA";
//         fault_event.header.fixed_header.event_type.type_name = "ObjectCrashFault";
//         fault_event.filterable_data.length(2);
//         fault_event.filterable_data[0].name = "FTDomainId";
//         fault_event.filterable_data[0].value = < Value of FTDomainId bundled into any >;
//         fault_event.filterable_data[1].name = "Location";
//         fault_event.filterable_data[1].value = < Value of Location bundled into any >;
        
            // TODO: Check the format is OK!
            if ((notification.filterable_data.length() >= 2) &&
                (strcmp(notification.filterable_data[1].name.in(),"Location") == 0)) {
                const ::FT::Location * location;
                // PW 10-05-2004: added any extraction result check
                if (! (notification.filterable_data[1].value >>= location)) {
                    CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMBadEventType, CORBA::COMPLETED_NO);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
                
                const std::string s_location = 
                    Cdmw::CommonSvcs::Naming::NamingInterface::to_string(*location);
                _location_str = s_location;
                CDMW_DEBUG(FTLogger::GetLogger(), "push_structured_event(): Location is '" << s_location << "'." );
                
                

                // Handle host failure, application failure, and process failure
                std::string hostname(""), applicationname(""), processname("");
                const CORBA::ULong lg = location->length();
                std::string type_name_message = CORBA::string_dup(notification.header.fixed_header.event_type.type_name);
                
                DEBUG_DUMPALL("Dump before"); 
                
                if ( type_name_message == "ObjectCrashFault")
                    {
                        switch (lg) {
                        case 3: ;
                        case 2:   {
                            // check if the location is known, if the location is known,
                            // it's a FT process else it a normal process, so do nothing
                            try {
                                LocInfoStorageObject obj
                                    = m_locInfoStorageHome->find_by_oid(s_location);     
                            }
                            catch (const NotFoundException& ) {            
                                return;
                            } 
                            handle_process_failure((*location));
                        }
                            break;
                            
                        case 1: {
                            const std::string hostname((*location)[0].id);
                            handle_host_failure(hostname);
                        }
                            break;
                            
                        default:
                            // Do nothing for the moment
                            break;
                        }
                        
                    } 
                DEBUG_DUMPALL("Dump after.");
            }
        }
        catch(...)
            {
                throw;
            }
    }

    /// The peer has disconnected
    /**
     * IDL:omg.org/CosNotifyComm/StructuredPushConsumer/disconnect_structured_push_consumer:1.0
     *
     * Does nothing ...
     * This operation is invoked by the consumer peer when it wishes
     * to disconnect.  The consumer can safely assume that no more
     * events will follow this request.
     */
    void ReplicationManager_impl::disconnect_structured_push_consumer()
        throw (CORBA::SystemException)
    {
        // No operation
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
    }


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
    void 
    ReplicationManager_impl::handle_member_ready(
                           ::FT::ObjectGroupId object_group_id,
                           const ::FT::Location& the_location)
              throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id<<",the_location="<<the_location);
        CDMW_MUTEX_GUARD (m_mutex);
        CORBA::NO_IMPLEMENT ex(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
                             
                             
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
    void 
    ReplicationManager_impl::handle_member_failure(
                              ::FT::ObjectGroupId object_group_id, 
                              const ::FT::Location& the_location)
              throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"object_group_id="<<object_group_id<<",the_location="<<the_location);
        CDMW_MUTEX_GUARD (m_mutex);
        CORBA::NO_IMPLEMENT ex(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
                           

    /**
     * IDL:thalesgroup.com/CdmwFT/
     *      ReplicationManager/set_primary_location:1.0
     *
     * This operation ask to all group member in that location to become 
     * primary. This operation is used when Fault Tolerance is managed at
     * process level.
     *
     * @exception PrimaryNotSet if the Replication Manager cannot set the
     *         primary member of the object group.
     * @exception BadReplicationStyle if the <code>ReplicationStyle</code>
     *         of the given group is not <code>COLD_PASSIVE</code> or
     *         <code>WARM_PASSIVE</code>.
     */
    void 
    ReplicationManager_impl::set_primary_location(const ::FT::Location& the_location)
              throw (::FT::PrimaryNotSet, 
                     ::FT::BadReplicationStyle, 
                     ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),",the_location="<<the_location);
        CDMW_MUTEX_GUARD (m_mutex);


        // #### FIXME: check the ReplicationStyle
        
        // Create an empty current data
        CurrentData current(this, &this->m_thread_data);
        
        // Get the new PRIMARY Location context
        std::string s_new_primary_loc;
        try {
            s_new_primary_loc = 
                Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location);
        } catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_primary_location(" << the_location << ")': Malformed location.\n"
                      << "Raising BAD_PARAMInvalidLocation() exception." );
            CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMInvalidLocation, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        LocationCtxts::iterator locctxtpos = m_loc_ctxts.find(s_new_primary_loc);
        if (locctxtpos == m_loc_ctxts.end()) {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "set_primary_location():"
                       << "Location context \"" << s_new_primary_loc << "\" not found\n"
                       << "Raising CORBA::INTERNAL exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        Location::Location* new_primary_locctxt = locctxtpos->second;

        if (new_primary_locctxt->is_primary())
            return;
        else
        {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_primary_location():"
                      << "Raising ::FT::PrimaryNotSet() exception." );
            ::FT::PrimaryNotSet ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        

        set_primary_location(new_primary_locctxt, the_location);

        // Save the possibly modified ogd and locinfo
        current.save_modified_items();
    }
         
         
    /**
     * This is the internal 'set_primary_location' method called from the
     * IDL operations 'set_primary_member' and 'set_primary_location'. It
     * assumes that the CurrentData has been created by the caller.
     */
    void 
    ReplicationManager_impl::set_primary_location (
            Location::Location*   new_primary
          , const ::FT::Location& new_primary_ftloc
          )
        throw ( ::FT::PrimaryNotSet
              , ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"new_primary="<<new_primary);
        try
        {
            // If the Location requested to change to PRIMARY is already PRIMARY,
            // there's nothing to do. Silently returns.
            Location::Location* old_primary
                = new_primary->get_peer_primary();
            if (old_primary == new_primary) return;

            ::FT::PrimaryNotSet ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;

            // Update the primary first on the old primary Location
            old_primary->update_primary(new_primary_ftloc);
            
            // Actually switch the newly elected Location to PRIMARY:
            // It calls update_primary and switches objects and location
            // state machines to PRIMARY state.
            new_primary->ev_set_primary();
            
            // Update the primary view for the other replicated Locations
            if (new_primary->peer_locations_number() > 2) {
                std::list < Location::Location* > peers_list;
                new_primary->get_peer_locations (peers_list);
                std::list < Location::Location* > :: iterator peer_pos;
                for ( peer_pos = peers_list.begin()
                      ; peer_pos != peers_list.end()
                      ; ++peer_pos
                      ) {
                    if ( ((*peer_pos) != new_primary) && ((*peer_pos) != old_primary))
                        (*peer_pos)->update_primary(new_primary_ftloc);
                }
            }
            
            // Save the possibly modified ogd and locinfo
            CurrentData* current = (CurrentData*)m_thread_data.get_value();
            current->save_modified_items();
        }
        catch(const ::CORBA::INTERNAL& e)
        {
            CDMW_DEBUG(FTLogger::GetLogger(),e);
            ::FT::PrimaryNotSet ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }    
    }
    
         
         
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
      
    void ReplicationManager_impl::set_first_backup (const ::FT::Location& the_location)
        throw (CdmwFT::AlreadyPrimary,
               ::FT::MemberNotFound,
               ::FT::BadReplicationStyle,
               ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
        CDMW_MUTEX_GUARD (m_mutex);

        // Create an empty current data
        CurrentData current(this, &this->m_thread_data);
        
        // Get the Location context
        std::string s_loc;
        try {
            s_loc = 
                Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location);
        } catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_first_backup(" << the_location << ")': Malformed location.\n"
                      << "Raising CdmwFT::AlreadyPrimary() exception." );
            CdmwFT::AlreadyPrimary ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        LocationCtxts::iterator iter = m_loc_ctxts.find(s_loc);
        if (iter == m_loc_ctxts.end()) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_first_backup():"
                      << "Location context \"" << s_loc << "\" not found\n"
                      << "Raising ::FT::MemberNotFound exception.");
            ::FT::MemberNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        Location::Location* locctxt = iter->second;
        
        if ((locctxt->fsm_state() == CdmwFT::LocationState_PRIMARY) ||
            (locctxt->fsm_state() == CdmwFT::LocationState_INSERTING_PRIMARY))
        {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_first_backup(" << the_location << ")': the location is already a primary\n"
                      << "Raising CdmwFT::AlreadyPrimary() exception." );
            CdmwFT::AlreadyPrimary ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        
        if ((locctxt->fsm_state() == CdmwFT::LocationState_INITIAL) ||
            (locctxt->fsm_state() == CdmwFT::LocationState_FAILED))
        {
            CDMW_WARN(FTLogger::GetLogger(),
                      "set_first_backup(" << the_location << ")': location is empty\n"
                      << "Raising ::FT::MemberNotFound exception." );
            ::FT::MemberNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }


        std::list < Location::Location* > peer_locations;
        locctxt->get_peer_locations(peer_locations);
        std::list < Location::Location* > :: iterator peers_iter;
        for ( peers_iter = peer_locations.begin()
            ; peers_iter != peer_locations.end()
            ; ++peers_iter
            ) {
            Location::Location* peerloc = (*peers_iter);
            if (peerloc->name() == s_loc)
                peerloc->set_first_backup(true);
            else
                peerloc->set_first_backup(false);
        }

        // Save the possibly modified ogd and locinfo
        current.save_modified_items();
    }


    CdmwFT::GroupObserverID 
    ReplicationManager_impl::register_group_state_observer(CdmwFT::GroupObserver* group_observer)
            throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        CORBA::NO_IMPLEMENT ex(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
    
    void 
    ReplicationManager_impl::unregister_group_state_observer(CdmwFT::GroupObserverID group_observer_id)
            throw (CdmwFT::InvalidGroupObserverId, CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        CORBA::NO_IMPLEMENT ex(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }


    /**
     * This operation asks the replication manager to become primary. This 
     * operation is used when Fault Tolerance is managed at process level.
     */
    void ReplicationManager_impl::activate()
            throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CDMW_MUTEX_GUARD (m_mutex);
        CORBA::NO_IMPLEMENT ex(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }
        

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
    void
    ReplicationManager_impl::check_properties(
            const ::FT::Properties & props,
            int type,
            bool check_value)
        throw(::FT::InvalidProperty,
              ::FT::UnsupportedProperty) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"type="<<type<<",check_value="<<check_value);
        CORBA::ULong props_length = props.length();
        for (CORBA::ULong iProps = 0; iProps < props_length; ++iProps) {
            // Get the property name.
            ::FT::Property property = props[iProps];
            std::string propertyName = std::string(property.nam[0].id);
            
            // Check the property name against the list of FT properties.
            StringList::iterator ft_properties_iter =
                std::find(m_ft_properties.begin(),
                          m_ft_properties.end(),
                          propertyName);

            if (ft_properties_iter != m_ft_properties.end()) {
                bool is_valid = true;
                bool is_supported = true;

                if (propertyName == "org.omg.ft.ReplicationStyle") {
                    // STATELESS = 0
                    // COLD_PASSIVE = 1
                    // WARM_PASSIVE = 2
                    // ACTIVE = 3
                    // ACTIVE_WITH_VOTING = 4

                    // (see paragraph 23.3.2 of the specifications)
                    // ReplicationStyle cannot be set dynamically.
                    if (type == DYNAMIC_PROPERTIES) {
                        is_valid = false;
                    } else if (check_value) {
                        CORBA::UShort value = 0;
                        property.val >>= value;
                        if (value > 4) {
                            is_valid = false;
                        } else {
                            // Only WARM_PASSIVE is supported right now.
                            if (value != 2) {
                                is_supported = false;
                            }
                        }
                    }
                } else if (propertyName == "org.omg.ft.MembershipStyle") {
                    // MEMB_APP_CTRL = 0
                    // MEMB_INF_CTRL = 1

                    // (see paragraph 23.3.2 of the specifications)
                    // MembershipStyle cannot be set dynamically.
                    if (type == DYNAMIC_PROPERTIES) {
                        is_valid = false;
                    } else if (check_value) {
                        CORBA::UShort value = 0;
                        property.val >>= value;
                        if (value > 1) {
                            is_valid = false;
                        } else {
                            // Only MEMB_APP_CTRL is supported at the moment.
                            if (value != 0) {
                                is_supported = false;
                            }
                        }
                    }
                } else if (propertyName == "org.omg.ft.ConsistencyStyle") {
                    // CONS_APP_CTRL = 0
                    // CONS_INF_CTRL = 1

                    // Not supported at the moment.
                    is_supported = false;

                    // (see paragraph 23.3.2 of the specifications)
                    // ConsistencyStyle cannot be set at creation time
                    // nor dynamically.
                    if ((type == CREATION_PROPERTIES) ||
                        (type == DYNAMIC_PROPERTIES)) {

                        is_valid = false;
                    }
                } else if (propertyName == "org.omg.ft.FaultMonitoringStyle") {
                    // PULL = 0
                    // PUSH = 1
                    // NOT_MONITORED = 2

                    // (see paragraph 23.3.2 of the specifications)
                    // FaultMonitoringStyle cannot be set at creation time
                    // nor dynamically.
                    if ((type == CREATION_PROPERTIES) ||
                        (type == DYNAMIC_PROPERTIES)) {
                        is_valid = false;
                    } else if (check_value) {
                        CORBA::UShort value = 0;
                        property.val >>= value;
                        if (value > 2) {
                            is_valid = false;
                        } else {
                            // Only PULL is supported right now.
                            if (value != 0) {
                                is_supported = false;
                            }
                        }
                    }
                } else if (propertyName == "org.omg.ft.FaultMonitoringGranularity") {
                    // MEMB = 0
                    // LOC = 1
                    // LOC_AND_TYPE = 2

                    is_supported = false;
                } else if (propertyName == "org.omg.ft.Factories") {
                    // Not supported at the moment.
                    is_supported = true;

                    // (see paragraph 23.3.2 of the specifications)
                    // Factories cannot be set as default properties.
                    if (type == DEFAULT_PROPERTIES) {
                        is_valid = false;
                    }
                } else if (propertyName == "org.omg.ft.InitialNumberReplicas") {
                    // (see paragraph 23.3.2 of the specifications)
                    // InitialNumberReplicas cannot be set dynamically.
                    if (type == DYNAMIC_PROPERTIES) {
                        is_valid = false;
                    } else if (check_value) {
                        CORBA::UShort value = 0;
                        property.val >>= value;

                        // This is not supported as the Factories property
                        // is not currently supported.
                        is_supported = false;
                    }
                } else if (propertyName == "org.omg.ft.MinimumNumberReplicas") {
                    if (check_value) {
                        try {
                            CORBA::UShort value = 0;
                            property.val >>= value;
                        } catch (...) {
                            is_valid = false;
                        }
                    }
                } else if (propertyName == "org.omg.ft.FaultMonitoringInterval") {
                    // Not sure whether we support this property.
                    is_supported = false;
                } else if (propertyName == "org.omg.ft.CheckpointInterval") {
                    is_supported = false;
                }

                // If this property is either invalid or unsupported, then raise
                // the appropriate exception.
                if (!is_valid) {
                    CDMW_WARN(FTLogger::GetLogger(),"Invalid Property '" << property.nam[0].id  << "'");
                    ::FT::InvalidProperty ex(property.nam, property.val);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                } else if (!is_supported) {
                    CDMW_WARN(FTLogger::GetLogger(),"Unsupported Property '" << property.nam[0].id  << "'");
                    ::FT::UnsupportedProperty ex(property.nam, property.val);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }
            } else {
                // We couldn't check this property against the FT properties,
                // we'll consider it as invalid.
                CDMW_WARN(FTLogger::GetLogger(),"Invalid Property '" << property.nam[0].id << "'");
                ::FT::InvalidProperty ex(property.nam, property.val);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
        }
    }

    
    /**
     * Returns all known data for a specific object group.
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
              CORBA::SystemException) 
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        std::vector<OGDStorageObject> ogd_list;
        m_ogdStorageHome->find_if(
            std::bind2nd( is_equivalent_to< std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                                                       Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type 
                                                     > 
                                          > (),
                          object_group),
            std::back_inserter(ogd_list));
            
        if (ogd_list.size() != 1) {
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        ::FT::ObjectGroupId ogid= ogd_list[0L].get_oid();
        CDMW_LOG_FUNCTION_RETURN(ogid);
        return ogid;
    }


    /*
     * Get the primary location from the ObjectGroupData dynamic properties
     */
    ::FT::Location *
    ReplicationManager_impl::get_group_primary_location(const ::CdmwFT::ObjectGroupData & ogd) const
        throw ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        // This may appear in dynamic_properties
        using namespace Cdmw::CommonSvcs::Naming;
        for (unsigned int i = 0; i<ogd.dynamic_properties.length(); i++) {
            std::string s_property
            = std::string(ogd.dynamic_properties[i].nam[0].id);
            const std::string property_name = PRIMARY_LOCATION_PROPERTY_NAME;
            if(s_property == property_name) {
                const ::FT::Location * val;
                // PW 10-04-2004: added any extraction result check
                if (ogd.dynamic_properties[i].val >>= val) 
                    return new ::FT::Location(*val);
                else
                    break; // Returns empty location
            }                
        }
        ::FT::Location_var no_location = new ::FT::Location();
        no_location->length(0L);
        return no_location._retn();
    }


    /*
     * Set the primary location in the ObjectGroupData dynamic properties
     */
    void
    ReplicationManager_impl::set_group_primary_location (
              ::CdmwFT::ObjectGroupData & ogd
            , const ::FT::Location &      primary_loc) const
        throw (CORBA::NO_MEMORY)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"primary_loc="<<primary_loc);
        try {
            std::string property_name = PRIMARY_LOCATION_PROPERTY_NAME;
            
            ::FT::Property_var prop = new ::FT::Property();
            prop->nam.length(1);
            prop->nam[0].id = CORBA::string_dup(property_name.c_str());
            
            CORBA::Any the_any;
            the_any <<= primary_loc;
            prop->val=the_any;



            // transfer the sequence into a map
            PropertiesMap dynamic_properties;
            CORBA::ULong dynamic_property_length = ogd.dynamic_properties.length();
            for (CORBA::ULong i = 0; i < dynamic_property_length; ++i) {
                dynamic_properties[std::string(ogd.dynamic_properties[i].nam[0].id)] = ogd.dynamic_properties[i];            
            }
            
            // erase the primary
            dynamic_properties[property_name] = prop;
            
            // Finally, return the result list of properties.
            PropertiesMap::size_type size = dynamic_properties.size();
            ::FT::Properties_var result_properties = new ::FT::Properties(size);
            result_properties->length(size);
            CORBA::ULong i = 0;
            PropertiesMap::iterator properties_iter;
            for (  properties_iter = dynamic_properties.begin()
                ; properties_iter != dynamic_properties.end()
                ; ++properties_iter
                ) {
                result_properties[i++] = (*properties_iter).second;
            }
            ogd.dynamic_properties = result_properties;
            
        } catch (const std::bad_alloc & ) {
            CDMW_FATAL(FTLogger::GetLogger(),"No Memory!");
            CORBA::NO_MEMORY ex(::Cdmw::OrbSupport::NO_MEMORY,
                                CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }


    /*
     * Removes the primary location from the ObjectGroupData dynamic properties
     */
    bool
    ReplicationManager_impl::erase_group_primary_location(::CdmwFT::ObjectGroupData & ogd) const
        throw ()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        // transfert the sequence into a map
        PropertiesMap dynamic_properties;
        CORBA::ULong dynamic_property_length = ogd.dynamic_properties.length();
        for (CORBA::ULong i = 0; i < dynamic_property_length; ++i) {
            dynamic_properties[std::string(ogd.dynamic_properties[i].nam[0].id)] = ogd.dynamic_properties[i];            
        }

        // erase the primary
        const std::string property_name = PRIMARY_LOCATION_PROPERTY_NAME;
        PropertiesMap::size_type result = dynamic_properties.erase(property_name);
        

        // Finally, return the result list of properties.
        PropertiesMap::size_type size = dynamic_properties.size();
        ::FT::Properties_var result_properties = new ::FT::Properties(size);
        result_properties->length(size);
        CORBA::ULong i = 0;
        PropertiesMap::iterator properties_iter;
        for (properties_iter = dynamic_properties.begin();
            properties_iter != dynamic_properties.end();
            ++properties_iter
            ) {
            result_properties[i++] = (*properties_iter).second;
        }
        ogd.dynamic_properties = result_properties;
        bool erased = (result == 1);
        CDMW_LOG_FUNCTION_RETURN(erased);
        return erased;
    }


    /*
     *
     */
    Location::Location* 
    ReplicationManager_impl::elect_a_primary_location
        (Location::Location* old_primary) const
            throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger()," old_primary="<< old_primary);
        // We choose the peer BACKUP Location flagged as 'first backup'
        // #### FIXME: use a more efficient mechanism than the Location list
        Location::Location* new_primary_loc = NULL;
        Location::Location* aux_primary_loc = NULL;
        std::list < Location::Location* > peers_list;
        old_primary->get_peer_locations (peers_list);
        std::list < Location::Location* > :: iterator peer_pos;
        for ( peer_pos = peers_list.begin()
            ; peer_pos != peers_list.end()
            ; ++peer_pos
            ) {
            Location::Location* curloc = *peer_pos;
            if (curloc != old_primary) {
                if (curloc->fsm_state() == ::CdmwFT::LocationState_BACKUP)
                {
                    if (curloc->first_backup())
                    {
                        new_primary_loc = curloc;
                        break;
                    }
                    if (aux_primary_loc == NULL)
                    {
                        aux_primary_loc = curloc;
                    }
                }
            }
        }
        
        if (new_primary_loc != NULL)
            return new_primary_loc;
        else
            return aux_primary_loc;
    }
    

    /*
     *
     */
    ::FT::Location* 
    ReplicationManager_impl::elect_a_new_primary
    (::CdmwFT::ObjectGroupData & ogd) const
        throw (CORBA::NO_MEMORY)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try {
            ::FT::Location primary_loc;
            primary_loc.length(0);
            
            // Choosing a primary is simple for the moment...
            if (ogd.members.length() > 0L) {
                primary_loc 
                    = ogd.members[0L].the_location;
                set_group_primary_location(ogd, primary_loc);
            }
            return new ::FT::Location(primary_loc);
        } catch (const std::bad_alloc & ) {
            CDMW_FATAL(FTLogger::GetLogger(),"No Memory!");
            CORBA::NO_MEMORY ex(::Cdmw::OrbSupport::NO_MEMORY,
                                CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
    }
    


    /*
     * Restores the Rep.Mgr. state from the ObjectGroupData list
     * 
     * NOTE: this a private method that *must* be called while the
     *       Rep.Mgr. mutex is locked.
     */
    void restore_from_ogds(const std::list < ::CdmwFT::ObjectGroupData_var > ogd_list)
    // #### FIXME: exceptions list
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        CORBA::NO_IMPLEMENT ex(OrbSupport::NO_IMPLEMENT, CORBA::COMPLETED_NO);
        CDMW_LOG_FUNCTION_EXCEPTION(ex);
        throw ex;
    }


    /*
     * Called back by a FaultNotifier "process failure" notification. 
     */
    void 
    ReplicationManager_impl::handle_process_failure(const ::FT::Location & the_location)
        throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"the_location="<<the_location);
        //
        // TODO: Exception safety! How to ensure that we leave a consistent state
        //       following an exception?

        // Trying to be exception safe:
        // Create an empty current data: it will keep track of modified
        // OGDs. Modified OGDs are saved just before returning if no 
        // exception occurs in the meantime.
        CurrentData current(this, &this->m_thread_data);

        // Find the LocationInfo in the data store
        std::string s_location;
        try {
            s_location = 
                Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location);
        }
        catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "handle_process_failure(" << s_location << ")': Malformed location.\n"
                      << "Raising BAD_PARAMInvalidLocation() exception." );
            CORBA::BAD_PARAM ex(OrbSupport::BAD_PARAMInvalidLocation, CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        std::cout<<"handle_process_failure on the location "<< s_location<<std::endl;

        ::CdmwFT::LocationInfo* locinf = current.get_current_loc (s_location);
        // #### use * not _var: 'current' keeps ownership of the LocationInfo
        if (! locinf) {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "handle_process_failure(" << s_location << ")':LocationInfo not found.\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL ex( OrbSupport::INTERNALLocationInfoNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        try {

            // Find the Location instance
            LocationCtxts::iterator ctxtpos = m_loc_ctxts.find(s_location);
            if (ctxtpos == m_loc_ctxts.end()) {
                CDMW_ERROR(FTLogger::GetLogger(),
                           "handle_process_failure():"
                           << "Location context \"" << s_location << "\" not found\n"
                           << "Raising CORBA::INTERNAL exception." );
                CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL, CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            Location::Location* locctxt = ctxtpos->second;

            // if the location is already failed, no nothing
            if (locctxt->fsm_state() ==   CdmwFT::LocationState_FAILED)
                return;
            

            // Remove members (ie Objects not ObjectGroups) from the location and
            // removes members' related data from the ogd
            locctxt->remove_members();
           
            // If the failed location is PRIMARY, then we need to elect a new
            // PRIMARY location, and to update all remaining peer locations.
            if (locctxt->is_primary()) {

                // Elect a new primary location
                Location::Location* primary_locctxt = elect_a_primary_location(locctxt);
                if (primary_locctxt != NULL)
                {
                   // Get the ::FT::Location of the new primary
                    ::FT::Location_var primary_ftloc;
                    try {
                        primary_ftloc = string_to_ftloc(primary_locctxt->name());
                    } catch (const ::CORBA::INTERNAL & ex) {
                        CDMW_ERROR(FTLogger::GetLogger(),
                                   "handle_process_failure(" << the_location 
                                   << "): Invalid primary name \"" << primary_locctxt->name() << "\"\n"
                                   << "Raising CORBA::INTERNAL exception.");
                        CDMW_LOG_FUNCTION_EXCEPTION(ex);
                        throw ex;
                    }

                    // Actually switch the newly elected Location to PRIMARY:
                    // It calls update_primary and switches objects and location
                    // state machines to PRIMARY state.
                    primary_locctxt->ev_set_primary();

                    // Update the primary view for the other replicated Locations
                    std::list < Location::Location* > peers_list;
                    primary_locctxt->get_peer_locations (peers_list);
                    std::list < Location::Location* > :: iterator peer_pos;
                    for ( peer_pos = peers_list.begin()
                          ; peer_pos != peers_list.end()
                          ; ++peer_pos) {
                        if ((*peer_pos) != primary_locctxt)
                        {
                          try
                            {
                               (*peer_pos)->update_primary(primary_ftloc.in());
                           }			   
                            catch (const CORBA::SystemException & ex) {
                                CDMW_ERROR(FTLogger::GetLogger(),
                                           "update_all_group_views on the location " <<(*peer_pos)->name()
                                           << ": Caught a system exception.\n"
                                           << "Exception:" << ex );
                            }
                        }
                    }
                } else {
                    CDMW_WARN(FTLogger::GetLogger(),
                              "handle_process_failure(" << the_location 
                              << "): No primary elected because no more location \n";);
                }
                
            } else {     
                // Update all group views for the other replicated Locations
                std::list < Location::Location* > peers_list;
                locctxt->get_peer_locations (peers_list);
                std::list < Location::Location* > :: iterator peer_pos;
                for ( peer_pos = peers_list.begin()
                      ; peer_pos != peers_list.end()
                      ; ++peer_pos) {
                    if ((*peer_pos) != locctxt)
                    {
                        try
                        {
                            (*peer_pos)->update_all_group_views();
                            
                        } catch (const CORBA::SystemException & ex) {
                            CDMW_ERROR(FTLogger::GetLogger(),
                                       "update_all_group_views on the location " <<(*peer_pos)->name()
                                       << ": Caught a system exception.\n"
                                       << "Exception:" << ex );
                        }
                    }
                }
            }
            
        locctxt->reset();
        locinf->has_members_admin=false;
        // set members_admin to nil()
        if (locinf->members_admin_info._d() == CdmwFT::LocationStateness_STATEFULL) {
            CdmwFT::StatefullMembersAdminInfo statefull_admin_info;
            statefull_admin_info.admin=::CdmwFT::Location::StatefullPrimaryBackupAdmin::_nil();
            statefull_admin_info.local_datastore_infos.length(0L);
            locinf->members_admin_info.statefull(statefull_admin_info);
        } else {
            CdmwFT::StatelessMembersAdminInfo stateless_admin_info;
            stateless_admin_info.admin = ::CdmwFT::Location::MembersAdmin::_nil();
            // Initialize the union (discriminant set to STATELESS)
            locinf->members_admin_info.stateless(stateless_admin_info);
        }
        locctxt->ev_failure();
        modified_loc(s_location);
        
        // Save the modified OGDs
        current.save_modified_items();


        } catch (const ::FT::PrimaryNotSet &) {
            //restore_from_ogds(ogd_list);
            CDMW_ERROR(FTLogger::GetLogger(),
                       "handle_process_failure("
                       << s_location
                       << "): Could not set primary.\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL,
                               CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::FT::ObjectNotCreated &) {
            //restore_from_ogds(ogd_list);
            CDMW_ERROR(FTLogger::GetLogger(),
                       "handle_process_failure("
                       << s_location
                       << "): Could not create group reference.\n"
                       << "Raising CORBA::INTERNAL() exception.");
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL,
                               CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const ::FT::MemberNotFound &) {
            //restore_from_ogds(ogd_list);
            CDMW_ERROR(FTLogger::GetLogger(),
                      "handle_process_failure("
                       << s_location
                       << "): Caught a MemberNotFound exception.\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL ex(::Cdmw::OrbSupport::INTERNAL,
                               CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        } catch (const CORBA::INTERNAL & ex) {
            //restore_from_ogds(ogd_list);
            CDMW_ERROR(FTLogger::GetLogger(),ex);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        } catch (const CORBA::NO_MEMORY & ex) {
            //restore_from_ogds(ogd_list);
            CDMW_FATAL(FTLogger::GetLogger(),ex);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
        } catch (const CORBA::SystemException & ex) {
            //restore_from_ogds(ogd_list);
            CDMW_WARN(FTLogger::GetLogger(),
                      "handle_process_failure("
                      << s_location
                      << "): Caught a system exception.\n"
                      << "Exception:" << ex );
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw;
            // TODO: Trigger a reconfiguration? (check that member is non faulty?)
        }
        ShowContexts();
    }

    /*
     * 
     */
    void 
    ReplicationManager_impl::handle_application_failure(const std::string & hostname,
                                                        const std::string & applicationname)
        throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"hostname="<<hostname<<",applicationname="<<applicationname);
        // #### FIXME: this method loops on call to handle_process_failure which
        //      modifies OGDs and removes the LocationInfo. If an exception occurs
        //      while looping, then process_failure will be completely processed
        //      for some process and not for the others.
          
        // Walk through the list of Location and remove the ones belonging
        // to the faulty application
        const char* host = hostname.c_str();
        const char* appl = applicationname.c_str();
        // Since handle_process_failure() removes the location context 
        // we will first put locations on a separate container and the
        // handle locations 
        std::map<std::string,::FT::Location_var> failed_locations;
        LocationCtxts::iterator locpos;
        for ( locpos = m_loc_ctxts.begin()
            ; locpos != m_loc_ctxts.end()
            ; ++locpos
            ) {
            Location::Location* locsm = locpos->second;
            // #### FIXME: check exception
            ::FT::Location_var ftloc = string_to_ftloc(locsm->name());
            const char* mhost = (ftloc.inout())[0].id;
            const char* mappl = (ftloc.inout())[1].id;
            if ((strcmp(host,mhost) == 0) && (strcmp(appl,mappl) == 0)) {
                failed_locations.insert(make_pair(std::string(locsm->name()), ftloc));

                //handle_process_failure(ftloc.in());
            }
        }
        for (std::map<std::string,::FT::Location_var>::const_iterator it = failed_locations.begin();
             it != failed_locations.end();
             ++it) {
            try {
                handle_process_failure(it->second.in());
            } catch (const CORBA::SystemException & ex) {
                CDMW_WARN(FTLogger::GetLogger(),
                          "handle_application_failure() "
                          << "Error in call handle_process_failure(" << it->first << "): "
                          << "Caught a system exception.\n"
                          << "Exception:" << ex);
                // continue any way!
            }
        }
    }


    /*
     * 
     */
    void 
    ReplicationManager_impl::handle_host_failure(const std::string & hostname)
        throw (CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"hostname="<<hostname);
        // #### FIXME: this method loops on call to handle_process_failure which
        //      modifies OGDs and removes the LocationInfo. If an exception occurs
        //      while looping, then process_failure will be completely processed
        //      for some process and not for the others.
          
        // Walk through the list of Location and remove the ones belonging
        // to the faulty application
        const char* host = hostname.c_str();
        // Since handle_process_failure() removes the location context 
        // we will first put locations on a separate container and the
        // handle locations 
        std::map<std::string,::FT::Location_var> failed_locations;
        LocationCtxts::iterator locpos;
        for ( locpos = m_loc_ctxts.begin()
            ; locpos != m_loc_ctxts.end()
            ; ++locpos
            ) {
            Location::Location* locsm = locpos->second;
            // #### FIXME: check exception
            ::FT::Location_var ftloc = string_to_ftloc(locsm->name());
            const char* mhost = (ftloc.inout())[0].id;
            if (strcmp(host,mhost) == 0) {
                failed_locations.insert(make_pair(std::string(locsm->name()), ftloc));
                //handle_process_failure(ftloc.in());
            }
        }
        for (std::map<std::string,::FT::Location_var>::const_iterator it = failed_locations.begin();
             it != failed_locations.end();
             ++it)
            handle_process_failure(it->second.in());
    }


    /**
     * This operation removes a member from ogd and updates
     * it with a new object group reference and members list.
     * It return <b>true<b> if the removed member is a primary member,
     * <b>false</b> otherwise.
     *
     * @param object_group_id  The object group id of the object group
     *                     from which the member is to be removed.
     * @param ogd   An ObjectGroupDataObject with all known info
     *                     on the object group.
     * @param the_location The physical location; that is, a fault
     *                     containment region, host, cluster of hosts,
     *                     etc. of the member to be removed.
     * @param removed_location Holds the object reference of the MembersAdmin
     *                     managing the removed location.
     *
     * @return <b>true<b> if the removed member is a primary member,
     * <b>false</b> otherwise.
     *
     * @exception MemberNotFound if the Replication Manager cannot find a
     *         member of the object group at the given location.
     */
    ::FT::ObjectGroup_ptr
    ReplicationManager_impl::remove_member (
            ::FT::ObjectGroupId         ogid
           , const ::FT::Location &     the_location)
        throw(::FT::ObjectGroupNotFound,
              ::FT::MemberNotFound,
              ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid<<",the_location="<<the_location);
        // Because the FT is now handled at the process level (Location),
        // a remove_member ends up in removing *all* members of the owner
        // Location and then switch this Location to INITIAL.

        // Get object group's known data: may raise NotFoundException.
        ::CdmwFT::ObjectGroupData* ogd = get_current_ogd(ogid);
        if (! ogd) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "remove_member( ogid = " << ogid << "):"
                      " ObjectGroupData not found.\n"
                      << "Raising ::FT::ObjectGroupNotFound." );
            ::FT::ObjectGroupNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        // Get Location's known data: may raise NotFoundException.
        std::string s_location;
        try {
            s_location = 
                Cdmw::CommonSvcs::Naming::NamingInterface::to_string(the_location);
        } catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "remove_member(\"" << the_location << "\"): Malformed location.\n"
                      << "Raising BAD_PARAMInvalidLocation() exception." );
            CORBA::BAD_PARAM ex( OrbSupport::BAD_PARAMInvalidLocation
                                 , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        ::CdmwFT::LocationInfo* locinfo = get_current_loc(s_location);
        if (! locinfo) {
            CDMW_WARN(FTLogger::GetLogger(),
                      "remove_member( ogid = " << ogid << "):"
                      " LocationInfo \"" << s_location << "\" not found.\n"
                      << "Raising ::FT::MemberNotFound exception.");
            ::FT::MemberNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        if (!locinfo->has_members_admin){
            CDMW_WARN(FTLogger::GetLogger(),
                      "remove_member(" << s_location << ")': Location is not registered.\n"
                      << "Raising MemberNotFound() exception." );
            ::FT::MemberNotFound ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        // Get the Location context owning the removed member
        LocationCtxts::iterator loc_iter = m_loc_ctxts.find(s_location);
        if (loc_iter == m_loc_ctxts.end()) {
            CDMW_ERROR(FTLogger::GetLogger(),
                       "remove_member(" << s_location << ")': Location is not registered.\n"
                       << "Raising CORBA::INTERNAL() exception." );
            CORBA::INTERNAL ex( OrbSupport::INTERNALLocationCtxtNotFound
                                , CORBA::COMPLETED_NO);
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        Location::Location* locctxt = loc_iter->second;
        
        bool remove_location_is_primary = locctxt->is_primary();

        // Delegate the job to the Location context
        ::FT::ObjectGroup_var new_iogr = locctxt->remove_member(ogid);

        // If the current Location is PRIMARY, then
        if (remove_location_is_primary) {
          
            // - elect a new PRIMARY
            Location::Location* new_primary_locctxt;
            new_primary_locctxt = elect_a_primary_location(locctxt);

 
            if (new_primary_locctxt != NULL)
            {

                std::string primary_loc_s = new_primary_locctxt->name();
                ::FT::Location_var primary_loc;
                
                try {
                    primary_loc =
                    Cdmw::CommonSvcs::Naming::NamingInterface::to_name( primary_loc_s );
                } catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
                    CDMW_WARN(FTLogger::GetLogger(),
                              "remove_member(\"" << the_location << "\"): Malformed location.\n"
                              << "Raising BAD_PARAMInvalidLocation() exception." );
                    CORBA::BAD_PARAM ex( OrbSupport::BAD_PARAMInvalidLocation
                                         , CORBA::COMPLETED_NO);
                    CDMW_LOG_FUNCTION_EXCEPTION(ex);
                    throw ex;
                }


                // Update the primary first on the current Location
                locctxt->update_primary(primary_loc);
                locctxt->reset();
                // Actually switch the newly elected Location to PRIMARY:
                // It calls update_primary and switches objects and location
                // state machines to PRIMARY state.
                new_primary_locctxt->ev_set_primary();

                // Update the primary view for the other replicated Locations
                if (new_primary_locctxt->peer_locations_number() > 2) {
                    std::list < Location::Location* > peers_list;
                    locctxt->get_peer_locations (peers_list);
                    std::list < Location::Location* > :: iterator peer_pos;
                    for ( peer_pos = peers_list.begin()
                          ; peer_pos != peers_list.end()
                          ; ++peer_pos
                          ) {
                        if (  ((*peer_pos) != new_primary_locctxt)
                              && ((*peer_pos) != locctxt)
                              )
                            (*peer_pos)->update_primary(the_location);
                    }
                }
            } else {
                locctxt->reset();
            }            
        } else {
            locctxt->reset();

            // Update the group views on each Location 
            std::list < Location::Location* > peers_list;
            locctxt->get_peer_locations (peers_list);
            std::list < Location::Location* > :: iterator peer_pos;
            for ( peer_pos = peers_list.begin()
                ; peer_pos != peers_list.end()
                ; ++peer_pos
                ) {
                (*peer_pos)->update_all_group_views();
            }
        }
        return new_iogr._retn();
    }


    /**
     * A utility method that builds a group IOR and returns it in the ObjectGroupData
     * passed as an inout parameter.
     */
    void
    ReplicationManager_impl::build_iogr (
              const ::FT::Location&       primary_loc
            , ::CdmwFT::ObjectGroupData&  ogd)
        throw(::FT::PrimaryNotSet,
              ::FT::ObjectNotCreated,
              ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"primary_loc="<<primary_loc);
        // #### FIXME: exception if current == NULL ????
        CurrentData* current = (CurrentData*)m_thread_data.get_value();

        // may throw ::FT::PrimaryNotSet or ::FT::ObjectNotCreated
        // Build an oid using the pattern : group_id/group_ver.
        ::FT::ObjectGroupRefVersion group_version = ogd.object_group_ref_version + 1;
        FallbackOid fallbackOid(ogd.group_id, group_version, ogd.repository_id.in());
        PortableServer::ObjectId_var oid = fallbackOid.to_ObjectId();
        
        CORBA::Object_var replication_manager_ref =
            m_poa->create_reference_with_id(oid.in(), ogd.repository_id.in());     
        ::CdmwFT::Objects_var fallBack = new ::CdmwFT::Objects;            
        fallBack->length(1);
        fallBack[0L] = CORBA::Object::_duplicate(replication_manager_ref.in());

        CdmwFT::MemberInfos members(ogd.members.length());
        members.length(ogd.members.length());
        for (CORBA::ULong i=0; i<ogd.members.length(); i++) {
            members[i].the_reference = ogd.members[i].the_reference;
            members[i].the_location  = ogd.members[i].the_location;
        }

        ogd.object_group_ref = m_iogrFactory->build_iogr (
                                      members,
                                      primary_loc,
                                      ogd.group_id,
                                      std::string(ogd.repository_id).c_str(),
                                      m_ft_domain_id.in(),
                                      ogd.object_group_ref_version,
                                      fallBack.in());

        set_group_primary_location(ogd, primary_loc);
        current->modified_ogd(ogd.group_id);
    }
	 
    void 
    ReplicationManager_impl::ShowContexts()
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        if (!FTLogger::GetLogger()->is_debug_enabled()) return;
        std::ostringstream oss__;
        oss__ << ">>>> Showing Locations:" << "\n";
        
        CurrentData * current = (CurrentData*)m_thread_data.get_value();
        if (current == NULL)
            oss__<<"Null current"<<"\n";


        int loc_idx = 1;
        LocationCtxts::iterator locpos;
        for (locpos = m_loc_ctxts.begin(); locpos != m_loc_ctxts.end(); ++locpos) {
            Location::Location* curloc = (*locpos).second;
            CdmwFT::LocationInfo * locinfo = NULL;
            if (current != NULL)
                locinfo = current->get_current_loc(curloc->name());

            oss__ << "  [" << loc_idx++ << "] Location: " ;
            if (! curloc) {
                oss__ << "NULL pointer" << "\n";
            } else {
                const char* backup_str = (curloc->backup())? "YES": "NO";
                const char* registered_str = "???";
                if (locinfo != NULL)
                    registered_str = (locinfo->has_members_admin)? "YES": "NO";
                oss__  << "(" << curloc << ")" << "\n"
                       << "    - name                : \"" << curloc->name() << "\"" << "\n" 
                       << "    - state               : " << sLocationStateString[curloc->fsm_state()] << "\n"   
                       << "    - backup flag         : " << backup_str << "\n"
                       << "    - Expected add_member : " << curloc->exp_add_member() << "\n"
                       << "    - registered          : " << registered_str << "\n"
                       << "    - stateness           : " << stateness_to_string(curloc->stateness()) << "\n";

                Location::Location::InfosMap& locmap = curloc->get_infos();
                int grp_idx = 1;
                Location::Location::InfosMap::iterator loc_iter;
                for (loc_iter = locmap.begin(); loc_iter != locmap.end(); ++loc_iter) {
                    ObjectGroup::ObjectGroup* locgrp = (*loc_iter).second.grp;
                    Object::Object*           locobj = (*loc_iter).second.obj;
                    oss__ << "    -> ogid  : "<<(*loc_iter).first;
                    
                    oss__ << "    [" << grp_idx++ << "] Infos:" << "\n";

                    // Show member info
                    oss__ << "      - Member: ";
                    if (! locobj) {
                        oss__ << "NULL pointer" << "\n";
                    } else {
                        oss__ << "(" << locobj << "), " 
                              << "local_id " << locobj->local_id() << ", state " << sMemberStateString[locobj->fsm_state()] << "\n";
                    }

                    // Show group info
                    oss__ << "      - ObjectGroup: ";
                    if (! locgrp) {
                        oss__ << "NULL pointer" << "\n";
                    } else {
                        oss__ << "(" << locgrp << ")" << "\n"
                                  << "        - ogid    : " << locgrp->ogid() << "\n"
                                  << "        - state   : " << sObjectGroupStateString[locgrp->fsm_state()] << "\n"
                                  << "        - Members (count " << locgrp->members_count() << "):" << "\n";
                        
                        ObjectGroup::ObjectGroup::InfosMap::iterator grp_iter;
                        ObjectGroup::ObjectGroup::InfosMap& grpmap = locgrp->get_infos();
                        int obj_idx = 1;
                        for (grp_iter = grpmap.begin(); grp_iter != grpmap.end(); ++grp_iter) {
                            Location::Location* grploc = (*grp_iter).second.loc;
                            Object::Object*     grpobj = (*grp_iter).second.obj;
                            oss__ << "        [" << obj_idx++ << "] Infos:" << "\n";

                            // Show member info
                            oss__ << "          - Member  : ";
                            if (! grpobj) {
                                oss__ << "NULL pointer" << "\n";
                            } else {
                                oss__ << "(" << grpobj << ")" 
                                << "local_id " << grpobj->local_id() << ", state " << sMemberStateString[grpobj->fsm_state()] << "\n";
                            }

                           // Show location info
                            oss__ << "          - Location: ";
                            if (! grploc) {
                                oss__ << "NULL pointer" << "\n";
                            } else {
                                oss__ << "(" << grploc << ")" 
                                << "name \"" << grploc->name() << "\", state " << sLocationStateString[grploc->fsm_state()] << "\n";
                            }

                        }
                    }
                }
            }
        }
        CDMW_DEBUG(FTLogger::GetLogger(),oss__.str());
    }




    /*
     * Dump the datastore ObjectGroupData
     */
    void ReplicationManager_impl::dump_ogd_datastore()
        throw ( ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");

        CDMW_MUTEX_GUARD (m_mutex);
        std::ostringstream oss__;
        oss__<<"Dump ObjectGroupData datastore "<<std::endl;
        m_ogdStorageHome->for_each(Cdmw::FT::ObjectGroupDataPrinter(oss__));
        CDMW_INFO(FTLogger::GetLogger(),oss__.str());
    }
    
    /*
     * Dump the datastore LocationInfo
     */
    void ReplicationManager_impl::dump_location_datastore()
        throw ( ::CORBA::SystemException)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");

        CDMW_MUTEX_GUARD (m_mutex);
        std::ostringstream oss__;
        oss__<<"Dump Location datastore "<<std::endl;
        m_locInfoStorageHome->for_each(Cdmw::FT::LocationInfosPrinter(oss__));
        CDMW_INFO(FTLogger::GetLogger(),oss__.str());
    }

    /**
     * Creates the ObjectGroup context associated with the ObjectGroupData
     * 'ogd' and ObjectGroupId 'ogid'. As a side effect, Object contexts are
     * also created at the same time.
     *
     * Typically called from a DataStoreObserver 'on_insert' callback.
     *
     * 1- creates a fresh ObjectGroup instance
     * 2- insert that empty ObjectGroup in the m_grp_ctxts map
     * 3- FOR EACH members found in the ogd
     *    DO
     *    - inserts an empty ObjectGroup::Infos in the ObjectGroup internal map
     *    - search for an already existing Location (using the ::FT::Location
     *      saved in the MemberOverallInfo) in the rep.mgr map:
     *      IF not found
     *        - creates a fresh Location context and itialize following members:
     *           - std::string                    m_locname;
     *           - ReplicationManagerDelegate*    m_rep_mgr;
     *        ENDIF
     *    - saves the pointer to this Location in the ObjectGroup::Infos
     *    - creates a fresh Object context fully initialized and saves the pointer
     *      to this Object in the ObjectGroup::Infos
     *    - inserts an empty Location::Infos in the Location internal map mInfosMap
     *      using the key 'ogid'
     *    - saves the current ObjectGroup and the Object pointers into the inserted
     *      Location::Infos
     *    END FOR EACH
     *
     * At return:
     * - the ObjectGroup and its Objects are associated in both ways (ObjectGroup
     *   to Object, and Object to ObjectGroup),
     * - the ObjectGroup and its Locations are associated in both ways
     * - Objects and Locations are associated in both ways
     * - the ObjectGroup and its Objects are fully initialized
     * - the Locations that existed before (not created here) are fully initialized
     * - the Locations created here are uncompletely initialized:
     *   - initialized members:
     *      m_locname;
     *      m_rep_mgr;
     *      m_infos;
     *   - uninitialized members (to be deduced from LocationInfo):
     *      m_members_count;
     *      m_exp_add_member;
     *      m_is_backup;
     *      m_stateness;
     *      m_fsm_state;
     *      m_fsm_initial_state;
     *      m_first_backup;
     *
     */
    void 
    ReplicationManager_impl::create_grp_ctxt_from_data (::FT::ObjectGroupId ogid,
                                                        const CdmwFT::ObjectGroupData & ogd)
        throw (CORBA::INTERNAL)
    // #### FIXME: exceptions clause
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        // 1- creates a fresh ObjectGroup instance
        ObjectGroup::ObjectGroup* objgrp
            = new ObjectGroup::ObjectGroup (ogid, this);

        // - m_fsm_state
        objgrp->fsm_state(ogd.state);
        // - m_fsm_internal_state
        objgrp->fsm_internal_state(ogd.fsm_internal_state);
        // - m_stateness
        objgrp->stateness(ogd.stateness);
        // - m_min_members_count
        objgrp->min_members_count(ogd.min_nb_replicas);

        CDMW_INFO(FTLogger::GetLogger(),
                  " ogid     : "<< ogid
                  <<"\n fsm_state    : "<<ogd.state
                  <<"\n fsm_intgernale_state    : "<<ogd.fsm_internal_state
                  <<"\n min_members_count        : "<<ogd.min_nb_replicas);



        // 2- insert that ObjectGroup in the m_grp_ctxts map
        m_grp_ctxts[ogid] = objgrp;
        

        
        // 3- extract properties to realise association between location and the objectgroup
        CORBA::ULong props_length = ogd.creation_properties.length();
        for (CORBA::ULong iProps = 0; iProps < props_length; ++iProps) {
            // Get the property name.
            ::FT::Property property = ogd.creation_properties[iProps];
            std::string propertyName = std::string(property.nam[0].id);
            
            if (propertyName == "org.omg.ft.Factories") {
                // Extract the FT::Location from the criteria
                const ::FT::FactoryInfos* factoryInfos = NULL;
                if (property.val >>= factoryInfos) {
                    
                    CORBA::ULong factoryInfos_length = factoryInfos->length();
                    for (  CORBA::ULong iFactories = 0
                           ; iFactories < factoryInfos_length
                           ; ++iFactories
                           )
                    {
                        std::string str_location;
                        try {
                            str_location
                            = Cdmw::CommonSvcs::Naming::NamingInterface::to_string((*factoryInfos)[iFactories].the_location);
                        }
                        catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
                            CDMW_WARN(FTLogger::GetLogger(),
                                      "create_object(" << str_location << ")': Malformed location.\n"
                                      << "Raising BAD_PARAMInvalidLocation() exception." );
                            CORBA::BAD_PARAM ex( OrbSupport::BAD_PARAMInvalidLocation
                                                 , CORBA::COMPLETED_NO);
                            CDMW_LOG_FUNCTION_EXCEPTION(ex);
                            throw ex;
                        }
                        Location::Location* locctxt;
                        LocationCtxts::iterator iter = m_loc_ctxts.find(str_location);
                        if (iter == m_loc_ctxts.end()) {
                            //   IF not found
                            //   - creates a fresh Location context and itialize following members:
                            //     - std::string                    m_locname;
                            //     - ReplicationManagerDelegate*    m_rep_mgr;
                            locctxt = new Location::Location(str_location, this);
                        } else {
                            locctxt = (*iter).second;
                        }


                        objgrp->associate_location(str_location,locctxt);
                        locctxt->associate_object_group(ogid, *objgrp);
                        locctxt->inc_exp_add_member();
                        m_loc_ctxts.insert(make_pair(str_location, locctxt));
                    }
                }
            }
        }


        // 4- FOR EACH members found in the ogd
        CORBA::ULong members_number = ogd.members.length();
        for (CORBA::ULong each = 0; each < members_number; each++) {

            // - inserts an empty ObjectGroup::Infos in the ObjectGroup internal map
            //   (using as the key the ::FT::Location saved in the MemberOverallInfo)
            std::string s_loc;
            try {
                s_loc = Cdmw::CommonSvcs::Naming::NamingInterface::to_string
                            (ogd.members[each].the_location);
            } catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
                CDMW_WARN(FTLogger::GetLogger(),
                          "create_grp_ctxt_from_ogd(\"" << s_loc << "\"): Malformed location.\n"
                          << "Raising BAD_PARAMInvalidLocation() exception." );
                ::CORBA::BAD_PARAM ex(::Cdmw::OrbSupport::BAD_PARAMInvalidLocation
                                      , CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }
            
            // - search for an already existing Location (using the ::FT::Location
            //   saved in the MemberOverallInfo) in the rep.mgr map:
            LocationCtxts::iterator loc_iter = m_loc_ctxts.find(s_loc);
            
            //   IF not found
            Location::Location* locctxt;
            if (loc_iter == m_loc_ctxts.end()) {
                CORBA::INTERNAL ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            } else {
                locctxt = (*loc_iter).second;
            }

            // - creates a fresh Object context fully initialized and saves the pointer
            //   to this Object in the ObjectGroup::Infos
            //    #### NOTE: m_local_id_count is incremented by the Ctor, so we save it
            //               before and handle it in such a way that it is always the
            //               greater restored local_id.
            ::CORBA::Long save_local_id_count = Object::Object::m_local_id_count;
            Object::Object* new_obj       = new Object::Object(locctxt, objgrp, this);
            new_obj->local_id(ogd.members[each].local_id);
            new_obj->fsm_state(ogd.members[each].the_state);
            new_obj->fsm_internal_state(ogd.members[each].the_state);
            if (save_local_id_count < new_obj->local_id())
                Object::Object::m_local_id_count = new_obj->local_id();
            
            // - saves the pointer to this Object in the rep.mgr. ObjectCtxts map
            m_obj_ctxts[new_obj->local_id()] = new_obj;

            
            /*
             * Associate the Location and the Object to the ObjectGroup
             */

            objgrp->associate_location(s_loc,locctxt);
            objgrp->associate_object(s_loc,new_obj);
            objgrp->members_count(members_number);

            /*
             * Associate the ObjectGroup and the Object to the Location
             */
            locctxt->associate_object(ogid, *new_obj);
            locctxt->members_count(members_number);
            //  END FOR EACH
        }
        ShowContexts();
        
    }

    /**
     * Update the ObjectGroup context associated with the ObjectGroupData
     * 'ogd' and ObjectGroupId 'ogid'.
     *
     * Typically called from a DataStoreObserver 'on_update' callback.
     *
     */
    void
    ReplicationManager_impl::update_grp_ctxt_from_data(::FT::ObjectGroupId              ogid
                              , const CdmwFT::ObjectGroupData &  ogd)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        // 1- find the ObjectGroup 
        ObjectGroupCtxts::iterator grp_iter =  m_grp_ctxts.find(ogid);
        
        
        ObjectGroup::ObjectGroup* objgrp;
        if (grp_iter != m_grp_ctxts.end()) {
            objgrp = (*grp_iter).second;
            
        } else {
            //    IF not found
            CORBA::INTERNAL ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        // - m_fsm_state
        objgrp->fsm_state(ogd.state);
        // - m_fsm_internal_state
        objgrp->fsm_internal_state(ogd.fsm_internal_state);
        // - m_stateness
        objgrp->stateness(ogd.stateness);
        // - m_min_members_count
        objgrp->min_members_count(ogd.min_nb_replicas);

        CDMW_INFO(FTLogger::GetLogger(),
                  " ogid     : "<< ogid
                  <<"\n fsm_state    : "<<ogd.state
                  <<"\n fsm_intgernale_state    : "<<ogd.fsm_internal_state
                  <<"\n min_members_count        : "<<ogd.min_nb_replicas);


        // 2- insert that ObjectGroup in the m_grp_ctxts map
        m_grp_ctxts[ogid] = objgrp;
        
        // 3- FOR EACH members found in the ogd
        CORBA::ULong members_number = ogd.members.length();
        typedef std::set< ::CORBA::Long > _ObjectList;
        _ObjectList existing_object;
        
        for (CORBA::ULong each = 0; each < members_number; each++) {

            // - inserts an empty ObjectGroup::Infos in the ObjectGroup internal map
            //   (using as the key the ::FT::Location saved in the MemberOverallInfo)
            std::string s_loc;
            try {
                s_loc = Cdmw::CommonSvcs::Naming::NamingInterface::to_string
                            (ogd.members[each].the_location);
            } catch(Cdmw::CommonSvcs::Naming::InvalidNameException&) {
                CDMW_WARN(FTLogger::GetLogger(),
                          "create_grp_ctxt_from_ogd(\"" << s_loc << "\"): Malformed location.\n"
                          << "Raising BAD_PARAMInvalidLocation() exception.");
                ::CORBA::BAD_PARAM ex(::Cdmw::OrbSupport::BAD_PARAMInvalidLocation
                                      , CORBA::COMPLETED_NO);
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            }            

            // - search for an already existing Location (using the ::FT::Location
            //   saved in the MemberOverallInfo) in the rep.mgr map:
            LocationCtxts::iterator loc_iter = m_loc_ctxts.find(s_loc);
            
            //   IF not found
            Location::Location* locctxt;
            if (loc_iter == m_loc_ctxts.end()) {
                //    IF not found
                CORBA::INTERNAL ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            } else {
                locctxt = (*loc_iter).second;
            }

            // - search for an already existing Object (using the local_id
            //   saved in the MemberOverallInfo) in the rep.mgr map:
            ObjectCtxts::iterator obj_iter =  m_obj_ctxts.find(ogd.members[each].local_id);

            Object::Object* new_obj;
            if (obj_iter == m_obj_ctxts.end()) {
                new_obj = new Object::Object(locctxt, objgrp, this);
                // - increment the Location members_count
                int members_count = locctxt->members_count();
                locctxt->members_count(members_count++);
            } else {
                new_obj = (*obj_iter).second;
            }
            

            // - creates a fresh Object context fully initialized and saves the pointer
            //   to this Object in the ObjectGroup::Infos
            //    #### NOTE: m_local_id_count is incremented by the Ctor, so we save it
            //               before and handle it in such a way that it is always the
            //               greater restored local_id.
            ::CORBA::Long save_local_id_count = Object::Object::m_local_id_count;
            new_obj->local_id(ogd.members[each].local_id);
            new_obj->fsm_state(ogd.members[each].the_state);
            new_obj->fsm_internal_state(ogd.members[each].the_state);            
            if (save_local_id_count < new_obj->local_id())
                Object::Object::m_local_id_count = new_obj->local_id();
            existing_object.insert(new_obj->local_id());

            // - saves the pointer to this Object in the rep.mgr. ObjectCtxts map
            m_obj_ctxts[new_obj->local_id()] = new_obj;            
            
            /*
             * Associate the Location and the Object to the ObjectGroup
             */
            objgrp->associate_location(s_loc,locctxt);
            objgrp->associate_object(s_loc,new_obj);
            objgrp->members_count(members_number);

            /*
             * Associate the ObjectGroup and the Object to the Location
             */
            locctxt->associate_object_group(ogid, *objgrp);
            locctxt->associate_object(ogid, *new_obj);
            locctxt->members_count(members_number);
            m_loc_ctxts.insert(make_pair(s_loc,locctxt));


            //  END FOR EACH
        }

        // create the list of the remove object
        _ObjectList remove_object;
        for (ObjectCtxts::iterator object_iter = m_obj_ctxts.begin();
             object_iter != m_obj_ctxts.end();
             object_iter++)
        {
            if(((*object_iter).second)->ogid() == ogid)
            {
                // if the object_id is not found in the datastore update
                if (existing_object.find((*object_iter).first) == existing_object.end())
                {
                    // save it
                    remove_object.insert((*object_iter).first);
                } else {
                    // do nothing
                }
            } else {
                // do nothing
            }
            
        }
        
        // realise the deassociation for the removed object
        for (_ObjectList::iterator objectlist_iter = remove_object.begin();
             objectlist_iter != remove_object.end();
             objectlist_iter++)
        {
            ObjectCtxts::iterator object_iter = m_obj_ctxts.find((*objectlist_iter));
            std::string locname = (*object_iter).second->locname();

            // - search for an already existing Location (using the ::FT::Location
            //   saved in the MemberOverallInfo) in the rep.mgr map:
            LocationCtxts::iterator loc_iter = m_loc_ctxts.find(locname);
            
            //   IF not found
            Location::Location* locctxt;
            if (loc_iter == m_loc_ctxts.end()) {
                //    IF not found
                CORBA::INTERNAL ex;
                CDMW_LOG_FUNCTION_EXCEPTION(ex);
                throw ex;
            } else {
                locctxt = (*loc_iter).second;
            }

            /*
             * deassociate the Object to the ObjectGroup
             */
            objgrp->disassociate_object(locname);

            /*
             * deassociate the Object to the Location
             */
            locctxt->disassociate_object(ogid);

         
            Object::Object* object = (*object_iter).second;
            m_obj_ctxts.erase(object_iter);
            delete object;
        }
        ShowContexts();
    }

    /**
     * Remove the ObjectGroup context associated with the ObjectGroupData
     * 'ogd' and ObjectGroupId 'ogid'.
     *
     * Typically called from a DataStoreObserver 'on_remove' callback.
     *
     */
    void
    ReplicationManager_impl::remove_grp_ctxt_from_data(::FT::ObjectGroupId              ogid)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        // To remove the objectGoup, all the inserting object must be
        // removed before => in this part, no deassociation must be 
        // realise in the part (they must be realise in the upate part

        // 1- find the ObjectGroup 
        ObjectGroupCtxts::iterator grp_iter =  m_grp_ctxts.find(ogid);
        
        
        ObjectGroup::ObjectGroup* objgrp;
        if (grp_iter != m_grp_ctxts.end()) {
            objgrp = (*grp_iter).second;
            
        } else {
            //    IF not found
            CORBA::INTERNAL ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        // 2- remove that ObjectGroup from the m_grp_ctxts map
        m_grp_ctxts.erase(grp_iter);
        delete objgrp;
        ShowContexts();
    }
    


    /**
     * Creates the Location context associated with the LocationInfo 'locinfo' 
     * and name 'locname'.
     *
     * Typically called from a DataStoreObserver 'on_insert' callback.
     *
     * - search for an already existing Location in the m_loc_ctxts map
     *   using the key 'locname'.
     *   IF found, it means that the Location has been created when 
     *      create_grp_ctxt_from_ogd() was called. Only need to complete
     *      the members not yet initialized.
     *     - initialize the following members from the LocationInfo
     *       - m_members_count
     *       - m_exp_add_member
     *       - m_is_backup
     *       - m_stateness
     *       - m_fsm_state
     *       - m_first_backup
     *   IF not found
     *     - creates a fresh Location context and itialize following members:
     *        - m_locname with 'locname' (made by the Ctor)
     *        - m_rep_mgr with 'this' (made by the Ctor)
     *     - initialize the following members from the LocationInfo (same than
     *       in the case 'Location found' above.
     *       - m_exp_add_member
     *       - m_is_backup
     *       - m_stateness
     *       - m_fsm_state
     *       - m_first_backup
     *     ENDIF
     *   - saves the pointer to this Location in the rep.mgr. LocationCtxts map
     */
    void 
    ReplicationManager_impl::create_loc_ctxt_from_data (const std::string & locname,
                                                        const CdmwFT::LocationInfo & locinfo)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"locname="<<locname);
        
        // - search for an already existing Location in the m_loc_ctxts map
        //    using the key 'locname'.
        //    IF found, it means that the Location has been created when 
        //       create_grp_ctxt_from_ogd() was called. Only need to complete
        //       the members not yet initialized.
        LocationCtxts::iterator loc_iter = m_loc_ctxts.find(locname);
        Location::Location* locctxt;
        if (loc_iter != m_loc_ctxts.end()) {
            //      - initialize the following members from the LocationInfo
            locctxt = (*loc_iter).second;
        } else {
            //    IF not found
            //      - creates a fresh Location context and itialize following members:
            //         - m_locname with 'locname' (made by the Ctor)
            //         - m_rep_mgr with 'this' (made by the Ctor)
            locctxt = new Location::Location(locname, this);
        }

        // - m_is_backup
        locctxt->backup(locinfo.initial_backup);
        // - m_fsm_state
        locctxt->fsm_state(locinfo.state);
        // - m_fsm_internal_state
        locctxt->fsm_internal_state(locinfo.fsm_internal_state);
        // - m_stateness
        locctxt->stateness(locinfo.stateness);
        // - m_first_backup
        locctxt->first_backup(locinfo.first_backup);

        // - saves the pointer to this Location in the rep.mgr. LocationCtxts map
        m_loc_ctxts.insert(make_pair(locname,locctxt));
        //m_loc_ctxts[locname] = locctxt;

        if (FTLogger::GetLogger()->is_info_enabled()) {
            std::ostringstream oss__;
            oss__ << " location name         : "<< locname
                  <<"\n exp_add_member      : "<< locinfo.expected_add_member
                  <<"\n backup              : "<< locinfo.initial_backup
                  <<"\n fsm_state           : "<< locinfo.state
                  <<"\n fsm_internal_state  : "<< locinfo.fsm_internal_state
                  <<"\n first_backup        : "<< locinfo.first_backup<<std::endl;
            if (locinfo.stateness == CdmwFT::LocationStateness_STATEFULL)
                oss__ << "\n stateness           : " << "STATEFULL" <<std::endl;
            else
                oss__ << "\n stateness           : " << "STATELESS" <<std::endl;

            CDMW_INFO(FTLogger::GetLogger(),oss__.str());
        }
        ShowContexts();
    }

    /**
     * Update the Location context associated with the LocationInfo 'locinfo'
     * and name 'locname'.
     *
     * Typically called from a DataStoreObserver 'on_insert' callback.
     *
     */
    void 
    ReplicationManager_impl::update_loc_ctxt_from_data (const std::string & locname,
                                                        const CdmwFT::LocationInfo & locinfo)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"locname="<<locname);

        // - search for an already existing Location in the m_loc_ctxts map
        //    using the key 'locname'.
        //    IF not found, throw CORBA::INTERNAL
        LocationCtxts::iterator loc_iter = m_loc_ctxts.find(locname);
        Location::Location* locctxt;
        if (loc_iter != m_loc_ctxts.end()) {
            //      - initialize the following members from the LocationInfo
            locctxt = (*loc_iter).second;
        } else {
            //    IF not found
            CORBA::INTERNAL ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }

        // - m_is_backup
        locctxt->backup(locinfo.initial_backup);
        // - m_fsm_state
        locctxt->fsm_state(locinfo.state);
        // - m_fsm_internal_state
        locctxt->fsm_internal_state(locinfo.fsm_internal_state);
        // - m_stateness
        locctxt->stateness(locinfo.stateness);
        // - m_first_backup
        locctxt->first_backup(locinfo.first_backup);

        // - saves the pointer to this Location in the rep.mgr. LocationCtxts map
        m_loc_ctxts.insert(make_pair(locname,locctxt));
//        m_loc_ctxts[locname] = locctxt;

        if (FTLogger::GetLogger()->is_info_enabled()) {
            std::ostringstream oss__;
            oss__ << " location name         : "<< locname
                  <<"\n exp_add_member      : "<< locinfo.expected_add_member
                  <<"\n backup              : "<< locinfo.initial_backup
                  <<"\n fsm_state           : "<< locinfo.state
                  <<"\n fsm_internal_state  : "<< locinfo.fsm_internal_state
                  <<"\n first_backup        : "<< locinfo.first_backup<<std::endl;
            if (locinfo.stateness == CdmwFT::LocationStateness_STATEFULL)
                oss__ << "\n stateness           : " << "STATEFULL" <<std::endl;
            else
                oss__ << "\n stateness           : " << "STATELESS" <<std::endl;

            CDMW_INFO(FTLogger::GetLogger(),oss__.str());
        }

        ShowContexts();
    }
    
    
    /**
     * remove the Location context associated with the LocationInfo 'locinfo'
     * and name 'locname'.
     *
     * Typically called from a DataStoreObserver 'on_insert' callback.
     *
     */
    void 
    ReplicationManager_impl::remove_loc_ctxt_from_data (const std::string & locname)
        throw (CORBA::INTERNAL)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"locname="<<locname);
        // - search for an already existing Location in the m_loc_ctxts map
        //    using the key 'locname'.
        //    IF not found, throw CORBA::INTERNAL
        LocationCtxts::iterator loc_iter = m_loc_ctxts.find(locname);
        if (loc_iter != m_loc_ctxts.end()) {
            //      - initialize the following members from the LocationInfo
            m_loc_ctxts.erase(loc_iter);
        } else {
            //    IF not found
            CORBA::INTERNAL ex;
            CDMW_LOG_FUNCTION_EXCEPTION(ex);
            throw ex;
        }
        ShowContexts();
    }
    
    
}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

