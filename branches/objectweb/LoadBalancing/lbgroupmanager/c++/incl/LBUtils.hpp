/* =========================================================================== *
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
 * =========================================================================== */


#ifndef _CDMW_LB_UTILS_HPP_
#define _CDMW_LB_UTILS_HPP_


#include <Foundation/orbsupport/CORBA.hpp>

#include "LoadBalancing/idllib/CdmwLBCommon.stub.hpp"
#include "Foundation/orbsupport/TypeTraits.hpp"

DEFINE_VARIABLE_LENGTH_DATA_TRAITS(::CdmwLB::ObjectGroupData);
namespace Cdmw {
    namespace LB {
            
            /**
             * Finds all objects with members on a specific location.
             */
            struct in_process
                : std::binary_function< std::pair< Cdmw::OrbSupport::type_traits< ::PortableGroup::ObjectGroupId >::_var_type, 
                                                   Cdmw::OrbSupport::type_traits< ::CdmwLB::ObjectGroupData >::_var_type >, 
                                        ::PortableGroup::Location, 
                                        bool> 
            {
                /**
                 * Checks whether the object group in <i>val</i> has a member on <i>loc</i>
                 */
                bool
                operator() (const std::pair< Cdmw::OrbSupport::type_traits< ::PortableGroup::ObjectGroupId >::_var_type, 
                            Cdmw::OrbSupport::type_traits< ::CdmwLB::ObjectGroupData >::_var_type > & val, 
                            const ::PortableGroup::Location & loc) const 
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
                : std::binary_function< std::pair< Cdmw::OrbSupport::type_traits< ::PortableGroup::ObjectGroupId >::_var_type, 
                                                   Cdmw::OrbSupport::type_traits< ::CdmwLB::ObjectGroupData >::_var_type >, 
                                        ::PortableGroup::Location, 
                                        bool> 
            {
                /**
                 * Checks whether the object group in <i>val</i> has a member on <i>loc</i>
                 */
                bool
                operator() (const std::pair< Cdmw::OrbSupport::type_traits< ::PortableGroup::ObjectGroupId >::_var_type, 
                            Cdmw::OrbSupport::type_traits< ::CdmwLB::ObjectGroupData >::_var_type > & val, 
                            const ::PortableGroup::Location & loc) const 
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
                : std::binary_function< std::pair< Cdmw::OrbSupport::type_traits< ::PortableGroup::ObjectGroupId >::_var_type, 
                                                  Cdmw::OrbSupport::type_traits< ::CdmwLB::ObjectGroupData >::_var_type >, 
                                        ::PortableGroup::Location,                                 bool> 
            {
                /**
                 * Checks whether the object group in <i>val</i> has a member on <i>loc</i>
                 */
                bool
                operator() (const std::pair< Cdmw::OrbSupport::type_traits< ::PortableGroup::ObjectGroupId >::_var_type, 
                            Cdmw::OrbSupport::type_traits< ::CdmwLB::ObjectGroupData >::_var_type > & val, 
                            const ::PortableGroup::Location & loc) const 
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
            
            
        } //namespace Cdmw
    } //namespace LB
#endif // _CDMW_LB_UTILS_HPP_

