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


#ifndef INCL_CCM_COMMON_CCMUTILS_HPP
#define INCL_CCM_COMMON_CCMUTILS_HPP

#include <string>
#include <iostream>


// Cdmw Files
#include <Foundation/common/Exception.hpp>
#include <Foundation/orbsupport/CORBA.hpp>

#include <CCMContainer/idllib/Components.skel.hpp>

// Cdmw CCM Files

namespace Cdmw {

namespace CCM {

#define  PRINT_INFO(msg) {\
    std::cout << "INFO :" << msg << std::endl; }
    
#define  PRINT_ERROR(msg) {\
    std::cerr << "ERROR :" << msg << std::endl; \
    std::cerr << " File   : " << __FILE__ << std::endl; \
    std::cerr << " Line   : " << __LINE__ << std::endl; }
    
#define  PRINT_ERROR2(msg1, msg2) {\
    std::cerr << "ERROR :" << msg1 << " "  << msg2 << std::endl; \
    std::cerr << " File   : " << __FILE__ << std::endl; \
    std::cerr << " Line   : " << __LINE__ << std::endl; }

namespace Common {


    /**
     *
     */
    unsigned long create_identifier() throw(std::bad_alloc);

    /**
     *
     * prefix_#
     *
     */
    std::string create_unique_identifier(const std::string & prefix, int fill_width = 0)
        throw(std::bad_alloc);

    /**
     * Conversion between OctetSeq and string
     */ 
    CORBA::OctetSeq *
        string_to_OctetSeq(const char* str);
    
    char*
        OctetSeq_to_string(const CORBA::OctetSeq & seq) throw (CORBA::SystemException);
    


} // End namespace Common

} // End namespace CCM

} // End namespce Cdmw


#endif  // INCL_CCM_COMMON_CCMUTILS_HPP

