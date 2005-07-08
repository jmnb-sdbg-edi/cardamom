/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES 
 * and SELEX-SI. 
 * 
 * It is derivative work based on PERCO Copyright (C) THALES 2000-2003. 
 * All rights reserved.
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
*/
/* ===================================================================== */


#ifndef INCL_CCM_CIF_CCMUTILS_HPP
#define INCL_CCM_CIF_CCMUTILS_HPP

#include <string>
#include <vector>


// Cdmw CCM Files


namespace {

} // End anonymous space

namespace Cdmw {

namespace CCM {

namespace CIF {

// Creation of facet oid and read facet oid
std::string
create_oid(const std::string& component_oid, const std::string& name, const std::string& ck_value="");

// return a list of string: [0] = component oid, 
// [1] = facet_oid / local consumer oid,
// [2] = subscription cookie string  for consumer oid
typedef std::vector<std::string> NameVector;
NameVector
read_oid(const std::string& oid);
 


} // End namespace CIF

} // End namespace CCM

} // End namespce Cdmw


#endif  // INCL_CCM_CIF_CCMUTILS_HPP

