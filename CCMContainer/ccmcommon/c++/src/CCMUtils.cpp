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


#include <CCMContainer/ccmcommon/CCMUtils.hpp>

#include <sstream>
#include <iomanip>

// Cdmw Files
#include <Foundation/osthreads/Mutex.hpp>
#include <Foundation/osthreads/MutexGuard.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

// Cdmw CCM Files

namespace Cdmw {

namespace CCM {

namespace Common {

unsigned long
create_identifier()
    throw(std::bad_alloc)
{
    try {
        const unsigned long MAX_COUNTER = 0xFFFFFFFF;
        static Cdmw::OsSupport::Mutex count_mutex;
        static unsigned long counter=0L;
        CDMW_MUTEX_GUARD(count_mutex);
        
        if (counter == MAX_COUNTER)
            counter = 0L;
        else
            counter++;
        return counter;
    } catch (...) {
        throw std::bad_alloc();
    }
}

std::string
create_unique_identifier(const std::string & prefix, int fill_width)
    throw(std::bad_alloc)
{
    try {
        unsigned long counter = create_identifier();
            
        std::stringstream ostr;
        if (fill_width > 0) {
            ostr << prefix << "_" << std::setw(fill_width) << std::setfill('0') << counter;
        } else 
            ostr << prefix << "_" << counter;
        
        return ostr.str();
    } catch (...) {
        throw std::bad_alloc();
    }
}
    
// Helper function char* to CORBA::OctetSeq
CORBA::OctetSeq *
string_to_OctetSeq(const char* str)
{
    // Convert char* to OctetSeq.
    size_t len = ::strlen(str);
    CORBA::OctetSeq_var oid = new CORBA::OctetSeq(len);
    oid -> length(len);
    memcpy(oid -> get_buffer(), str, len);
    return oid._retn();
};

// Helper function CORBA::OctetSeq to CORBA::ULong.

char*
OctetSeq_to_string(const CORBA::OctetSeq & seq)
throw (CORBA::SystemException)
{
    // Convert OctetSeq to unsigned long.
    //
    // IF we have any '\0' characters in the id then throw a BAD_PARAM
    // exception
    //
    if(::memchr(seq.get_buffer(), 0, seq.length()) != 0)
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAM,
                               CORBA::COMPLETED_NO);
    
    CORBA::String_var str = CORBA::string_alloc(seq.length());
    memcpy(str.inout(), seq.get_buffer(), seq.length());
    str[seq.length()] = '\0';

    return str._retn();
};

} // End namespace Container

} // End namespace CCM

} // End namespce Cdmw


