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


#ifndef _CDMW_FT_UTILS_HPP_
#define _CDMW_FT_UTILS_HPP_


#include <Foundation/orbsupport/CORBA.hpp>

#include "FaultTolerance/idllib/CdmwFTCommon.stub.hpp"
#include "CdmwFTObjectGroupData.stub.hpp"

namespace  {

    template <class T>
    struct is_object_group_from_name
        : std::binary_function<T, std::string, bool> 
    {
        bool
        operator() (const T & x, const std::string & y) const {
            if (y.compare(x.alias) == 0)
                return true;
            else
                return false;
        }
    };

    template <>
    bool
    is_object_group_from_name<std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type,
                                Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type > 
                    >::operator() (
        const std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                         Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type > & pair,
        const std::string & name) const
    {
            if (name.compare((pair.second)->alias) == 0)
                return true;
            else
                return false;
    };
    



        template <class T>
    struct is_equivalent_to
        : std::binary_function<T, ::FT::ObjectGroup_ptr, bool> 
    {
        bool
        operator() (const T & x, const ::FT::ObjectGroup_ptr & y) const {
            return x->_is_equivalent(y);
        }
    };

    template <>
    bool
    is_equivalent_to<std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type,
                                Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type > 
                    >::operator() (
        const std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                         Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type > & pair,
        const ::FT::ObjectGroup_ptr & object_group) const
    {
        return ((pair.second)->object_group_ref)->_is_equivalent(object_group);
    };
    


    template <>
    bool
    is_equivalent_to<Cdmw::FT::ReplicationManager::ReplicationManager_impl::
    GroupInfosMap::value_type>::operator() (
        const Cdmw::FT::ReplicationManager::ReplicationManager_impl::
        GroupInfosMap::value_type & group_info,
        const ::FT::ObjectGroup_ptr & object_group) const
    {
        return ((group_info.second)->object_group_ref)->_is_equivalent(object_group);
    };
    


    


    /**
     * Finds all objects with members on a specific location.
     */
    struct in_process
        : std::binary_function< std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                                           Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type >, 
                                ::FT::Location, 
                                bool> 
    {
        /**
         * Checks whether the object group in <i>val</i> has a member on <i>loc</i>
         */
        bool
        operator() (const std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                                     Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type > & val, 
                    const ::FT::Location & loc) const 
        {
            // location format is
            // host/application/process
            const char* host = loc[0].id;
            const char* appl = loc[1].id;
            const char* proc = loc[2].id;
            bool found_member = false;
            for (CORBA::ULong i = 0L;
                 i < val.second->members.length();
                 ++i) {
                const char* mhost = val.second->members[i].the_location[0].id;
                const char* mappl = val.second->members[i].the_location[1].id;
                const char* mproc = val.second->members[i].the_location[2].id;
                if ((strcmp(host,mhost) == 0) &&
                    (strcmp(appl,mappl) == 0) &&
                    (strcmp(proc,mproc) == 0)) {
                    found_member = true;
                    break;
                }
            }
            return found_member;
        }
    };
    
    struct in_application
        : std::binary_function< std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                                           Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type >, 
                                ::FT::Location, 
                                bool> 
    {
        /**
         * Checks whether the object group in <i>val</i> has a member on <i>loc</i>
         */
        bool
        operator() (const std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                                     Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type > & val, 
                    const ::FT::Location & loc) const 
        {
            // location format is
            // host/application
            const char* host = loc[0].id;
            const char* appl = loc[1].id;
            bool found_member = false;
            for (CORBA::ULong i = 0L;
                 i < val.second->members.length();
                 ++i) {
                const char* mhost = val.second->members[i].the_location[0].id;
                const char* mappl = val.second->members[i].the_location[1].id;
                if ((strcmp(host,mhost) == 0) &&
                    (strcmp(appl,mappl) == 0)) {
                    found_member = true;
                    break;
                }
            }
            return found_member;
        }
    };
    
    struct in_host
        : std::binary_function< std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                                           Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type >, 
                                ::FT::Location, 
                                bool> 
    {
        /**
         * Checks whether the object group in <i>val</i> has a member on <i>loc</i>
         */
        bool
        operator() (const std::pair< Cdmw::OrbSupport::type_traits< ::FT::ObjectGroupId >::_var_type, 
                                     Cdmw::OrbSupport::type_traits< ::CdmwFT::ObjectGroupData >::_var_type > & val, 
                    const ::FT::Location & loc) const 
        {
            // location format is
            // host
            const char* host = loc[0].id;
            bool found_member = false;
            for (CORBA::ULong i = 0L;
                 i < val.second->members.length();
                 ++i) {
                const char* mhost = val.second->members[i].the_location[0].id;
                if (strcmp(host,mhost) == 0) {
                    found_member = true;
                    break;
                }
            }
            return found_member;
        }
    };


}; //namespace
#endif // _CDMW_FT_UTILS_HPP_

