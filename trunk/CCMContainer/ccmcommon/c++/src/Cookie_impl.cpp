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


// Std Files
#include <string>
#include <sstream>
#include <iostream>

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <Foundation/ossupport/OS.hpp>

#include <CCMContainer/ccmcommon/Cookie_impl.hpp>
#include <CCMContainer/ccmcommon/CCMUtils.hpp>


namespace Cdmw {

namespace CCM {
    
namespace Common {
    //
    // IDL:omg.org/Components/Cookie:1.0
    // 

    // Constructor
    Cookie_impl::Cookie_impl()
    {
    }

    Cookie_impl::Cookie_impl (const std::string & s)
    {
        PRINT_INFO("Cookie created : s = " << s);
        CORBA::OctetSeq_var cookie_value = string_to_OctetSeq(s.c_str());

        // set cookieValue
        cookieValue(cookie_value.in());
    }

    Cookie_impl::Cookie_impl (const CORBA::OctetSeq & seq)
        : OBV_Components::Cookie()
    {
        cookieValue( seq );
        CORBA::String_var s = OctetSeq_to_string(cookieValue());
        PRINT_INFO("Cookie created : s = " << s.in());
    }

    Cookie_impl* Cookie_impl::create_cookie()
        throw(Components::ExceededConnectionLimit)
    {
        try {
            unsigned long counter = create_identifier();
        
            using namespace Cdmw::OsSupport;
            OS::Timeval tv = OS::get_time();
        
        
            std::ostringstream ostr;
            ostr << tv.seconds << ":" << tv.microseconds << ":" << counter;
            
            return new Cookie_impl(ostr.str());
        } catch (...) {
            throw Components::ExceededConnectionLimit();
        }
    }

    // Copy Constructor
    Cookie_impl::Cookie_impl (const Cookie_impl& right)
    {
        cookieValue(right.cookieValue());
    }

    // Destructor
    Cookie_impl::~Cookie_impl ()
    {
    }

    std::string  
    Cookie_impl::get_cookie_string() const
    {
        CORBA::String_var str = OctetSeq_to_string(cookieValue());
        std::string std_str(str.in());
        return std_str;
    }


    CORBA::ValueBase* 
    Cookie_impl::_copy_value()
    {
        return new Cookie_impl(cookieValue());
    }

    
    bool 
    Cookie_impl::operator<(const Cookie_impl& right) const
        throw()
    {
        CORBA::String_var lv = OctetSeq_to_string(cookieValue());
        CORBA::String_var rv = OctetSeq_to_string(right.cookieValue());
            
        std::string left_value(lv.in());
        std::string right_value(rv.in());

        return (left_value < right_value);        
    }

    bool 
    Cookie_impl::operator==(const Cookie_impl& right) const
        throw()
    {
        CORBA::String_var lv = OctetSeq_to_string(cookieValue());
        CORBA::String_var rv = OctetSeq_to_string(right.cookieValue());;
            
        std::string left_value(lv.in());
        std::string right_value(rv.in());

        return (left_value == right_value);        
    }



    /******************************/
    /*         Factory            */
    /******************************/


    CookieFactory::CookieFactory()
    {
    }
    
    CookieFactory::~CookieFactory()
    {
    }

    CORBA::ValueBase * 
    CookieFactory::create_for_unmarshal ()
    {
        return new Cookie_impl();
    }

} // End namespace CIF
} // End namespace CCM
} // End namespace Cdmw


