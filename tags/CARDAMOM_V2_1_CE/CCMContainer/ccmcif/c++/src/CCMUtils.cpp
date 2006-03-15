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


// std files
#include <string>
#include <iostream>

// Cdmw files
#include <Foundation/orbsupport/CORBA.hpp>

// Cdmw CCM files
#include <CCMContainer/ccmcif/CCMUtils.hpp>
#include <CCMContainer/ccmcommon/CCMUtils.hpp>

namespace 
{

} // end anonymous namespace


namespace Cdmw {

namespace CCM {

namespace CIF {


// Helper function to create facet/consumer oid and local pushConsumer oid 
std::string
create_oid(const std::string& component_oid, const std::string& name, const std::string& ck_value)
{
   std::string oid_str = component_oid + "/" + name;

   if (!ck_value.empty())
   {
       PRINT_INFO("ck_value not empty");
       // creation of a local push consumer oid: add cookie value
       oid_str = oid_str + "/" + ck_value;
   }

   return oid_str;
}


// Helper function to read oid, return a list of string: [0] = component oid, 
// [1] = facet_oid / local consumer oid,
// [2] = subscription cookie string  for consumer oid
NameVector
read_oid(const std::string& oid)
{
   NameVector name_vector;
   std::string component_oid = oid;
   size_t end_comp_oid_pos = component_oid.rfind('/');

   while (end_comp_oid_pos < component_oid.size())
   {
        // get name after '/'
        std::string tmp_str = component_oid.substr(end_comp_oid_pos + 1, component_oid.size());
        name_vector.insert(name_vector.begin(), tmp_str);

        component_oid = component_oid.substr(0, end_comp_oid_pos);
        end_comp_oid_pos = component_oid.rfind('/');
   }

   name_vector.insert(name_vector.begin(), component_oid);

   return name_vector;
}


} // end namespace CIF
} // end namespace CCM
} // end namespace Cdmw

