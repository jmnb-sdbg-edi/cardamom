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

#include "ftreplicationmanager/CurrentData.hpp"

#include <string>


#include <FaultTolerance/ftcommon/FTLogger.hpp>

#include <Foundation/logging/FunctionLogger.hpp>

namespace Cdmw {
namespace FT {
namespace ReplicationManager {

    ::CdmwFT::ObjectGroupData*
    CurrentData::get_current_ogd (::FT::ObjectGroupId ogid)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        RCFlgdGrpDataMap::iterator iter = m_ogd_map.find(ogid);
        if (iter != m_ogd_map.end()) {
            return &((*iter).second.ogd.inout());
        } else {
            try {
                m_ogd_map[ogid].ogd = m_rep_mgr->get_object_group_data(ogid);
            } catch (...) {
                return NULL;
            }
            return &(m_ogd_map[ogid].ogd.inout());
        }
    }

    ::CdmwFT::LocationInfo*
    CurrentData::get_current_loc (std::string name)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"name="<<name);
        RCFlgdLocInfoMap::iterator iter = m_loc_map.find(name);
        if (iter != m_loc_map.end()) {
            return &((*iter).second.loc.inout());
        } else {
            try {
                // m_loc_map[name].modified=true;
                m_loc_map[name].loc = m_rep_mgr->get_location_info(name);
            } catch (...) {
                return NULL;
            }
            return &(m_loc_map[name].loc.inout());
        }
    }

    bool
    CurrentData::modified_ogd (::FT::ObjectGroupId ogid)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"ogid="<<ogid);
        RCFlgdGrpDataMap::iterator iter = m_ogd_map.find(ogid);
        bool result=false;
        if (iter != m_ogd_map.end()) {
            (*iter).second.modified = true;
            result=true;
        }
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }

    bool
    CurrentData::modified_loc (std::string name)
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"name="<<name);
        RCFlgdLocInfoMap::iterator iter = m_loc_map.find(name);
        bool result=false;
        if (iter != m_loc_map.end()) {
            (*iter).second.modified = true;
            result=true;
        }
        CDMW_LOG_FUNCTION_RETURN(result);
        return result;
    }

    void
    CurrentData::save_modified_items ()
        throw (::CORBA::INTERNAL)
        // #### FIXME: exception clauses
    {
        CDMW_LOG_FUNCTION(FTLogger::GetLogger(),"");
        try
        {
            m_dsBase->get_manager().begin();
            RCFlgdGrpDataMap::iterator ogdpos;
            for (ogdpos = m_ogd_map.begin(); ogdpos != m_ogd_map.end(); ++ogdpos) {
                if ((*ogdpos).second.modified) {
                    // Item has been modified, save it.
                    m_rep_mgr->set_object_group_data ((*ogdpos).second.ogd);
                }
            }
            RCFlgdLocInfoMap::iterator locpos;
            for (locpos = m_loc_map.begin(); locpos != m_loc_map.end(); ++locpos) {
                if ((*locpos).second.modified) {
                    // Item has been modified, save it.
                    m_rep_mgr->set_location_info ( &(*locpos).second.loc.inout()
                                                   , (*locpos).first);
                }
            }
            m_dsBase->get_manager().commit();
        }
        catch(const CORBA::Exception& e)
        {
            std::cerr<<"CurrentData::save_modified_items : CORBA::exception: "<<e<<std::endl;
            m_dsBase->get_manager().rollback();
            throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL
                                  , CORBA::COMPLETED_NO);
        }
        catch(const std::exception& e)
        {
            std::cerr<<"CurrentData::save_modified_items : std::exception: "<<e.what()<<std::endl;
            m_dsBase->get_manager().rollback();
            throw CORBA::INTERNAL(::Cdmw::OrbSupport::INTERNAL
                                  , CORBA::COMPLETED_NO);
        }
    }

}; // namespace ReplicationManager
}; // namespace FT
}; // namespace Cdmw

