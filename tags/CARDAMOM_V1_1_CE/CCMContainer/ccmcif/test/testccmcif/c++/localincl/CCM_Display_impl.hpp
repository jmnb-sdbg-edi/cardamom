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


#ifndef ___BASICDEMO_CCM_DISPLAY_IMPL_HPP__
#define ___BASICDEMO_CCM_DISPLAY_IMPL_HPP__

#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include "testccmcif/Basic_cif.skel.hpp"


namespace BasicDemo
{


//
// IDL:thalesgroup.com/BasicDemo/CCM_Display:1.0
//
class CCM_Display_impl : virtual public ::BasicDemo::CCM_Display,
                         virtual public Cdmw::OrbSupport::RefCountLocalObject
{
    CCM_Display_impl(const CCM_Display_impl&);
    void operator=(const CCM_Display_impl&);

public:

    CCM_Display_impl();
    ~CCM_Display_impl();

    //
    // IDL:thalesgroup.com/BasicDemo/Display/print:1.0
    //
    virtual void print(const char* text)
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/BasicDemo/Display/print_event:1.0
    //
    virtual void print_event(BasicDemo::Event *)
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_short:1.0
    //
    virtual CORBA::Short test_short (CORBA::Short p1,
                                     CORBA::Short & p2,
                                     CORBA::Short_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_long:1.0
    //
    virtual CORBA::Long test_long (CORBA::Long p1,
                                   CORBA::Long & p2,
                                   CORBA::Long_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_long_long:1.0
    //
    virtual CORBA::LongLong test_long_long (CORBA::LongLong p1,
                                            CORBA::LongLong & p2,
                                            CORBA::LongLong_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_unsigned_short:1.0
    //
    virtual CORBA::UShort test_unsigned_short (CORBA::UShort p1,
                                               CORBA::UShort & p2,
                                               CORBA::UShort_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_unsigned_long:1.0
    //
    virtual CORBA::ULong test_unsigned_long (CORBA::ULong p1,
                                             CORBA::ULong & p2,
                                             CORBA::ULong_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_unsigned_long_long:1.0
    //
    virtual CORBA::ULongLong test_unsigned_long_long (CORBA::ULongLong p1,
                                                      CORBA::ULongLong_out p2,
                                                      CORBA::ULongLong_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_float:1.0
    //
    virtual CORBA::Float test_float (CORBA::Float p1,
                                     CORBA::Float & p2,
                                     CORBA::Float_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_double:1.0
    //
    virtual CORBA::Double test_double (CORBA::Double p1,
                                       CORBA::Double_out p2,
                                       CORBA::Double_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_boolean:1.0
    //
    virtual CORBA::Boolean test_boolean (CORBA::Boolean p1,
                                         CORBA::Boolean & p2,
                                         CORBA::Boolean_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_char:1.0
    //
    virtual CORBA::Char test_char (CORBA::Char p1,
                                   CORBA::Char & p2,
                                   CORBA::Char_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_wchar:1.0
    //
    virtual CORBA::WChar test_wchar (CORBA::WChar p1,
                                     CORBA::WChar & p2,
                                     CORBA::WChar_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_octet:1.0
    //
    virtual CORBA::Octet test_octet (CORBA::Octet p1,
                                     CORBA::Octet & p2,
                                     CORBA::Octet_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_enum:1.0
    //
    virtual ::BasicDemo::Display::enum1 test_enum (BasicDemo::Display::enum1 p1,
                                                   BasicDemo::Display::enum1 & p2,
                                                   BasicDemo::Display::enum1_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_function:1.0
    //
    virtual ::BasicDemo::Display_ptr test_function (BasicDemo::Display_ptr p1,
                                                    BasicDemo::Display_ptr & p2,
                                                    BasicDemo::Display_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_fixed_struct:1.0
    //
    virtual ::BasicDemo::s1 test_fixed_struct (const BasicDemo::s1 & p1,
                                               BasicDemo::s1 & p2,
                                               BasicDemo::s1_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_var_struct:1.0
    //
    virtual ::BasicDemo::s2 * test_var_struct (const BasicDemo::s2 & p1,
                                               BasicDemo::s2 & p2,
                                               BasicDemo::s2_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_fixed_union:1.0
    //
    virtual ::BasicDemo::Display::union1 test_fixed_union (const BasicDemo::Display::union1 & p1,
                                                           BasicDemo::Display::union1 & p2,
                                                           BasicDemo::Display::union1_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_var_union:1.0
    //
    virtual ::BasicDemo::Display::union2 * test_var_union (const BasicDemo::Display::union2 & p1,
                                                           BasicDemo::Display::union2 & p2,
                                                           BasicDemo::Display::union2_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_seq_struct:1.0
    //
    virtual ::BasicDemo::s1s * test_seq_struct (const BasicDemo::s1s & p1,
                                                BasicDemo::s1s & p2,
                                                BasicDemo::s1s_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_string:1.0
    //
    virtual char * test_string (const char * p1,
                                char *& p2,
                                CORBA::String_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_wstring:1.0
    //
    virtual CORBA::WChar * test_wstring (const CORBA::WChar * p1,
                                         CORBA::WChar *& p2,
                                         CORBA::WString_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_fixed_array:1.0
    //
    virtual ::BasicDemo::Display::array1_slice * test_fixed_array (const BasicDemo::Display::array1 p1,
                                                                   BasicDemo::Display::array1 p2,
                                                                   BasicDemo::Display::array1_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_array:1.0
    //
    virtual ::BasicDemo::Display::array2_slice * test_var_array (const BasicDemo::Display::array2 p1,
                                                                 BasicDemo::Display::array2 p2,
                                                                 BasicDemo::Display::array2_out p3)
        throw(CORBA::SystemException);

    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_any:1.0
    //
    virtual CORBA::Any * test_any (const CORBA::Any & p1,
                                   CORBA::Any & p2,
                                   CORBA::Any_out p3)
        throw(CORBA::SystemException);
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/test_event:1.0
    //
    virtual ::BasicDemo::Event * test_event (BasicDemo::Event * p1,
                                             BasicDemo::Event *& p2,
                                             BasicDemo::Event_out p3)
        throw(CORBA::SystemException);
  
    //
    // IDL:thalesgroup.com/BasicDemo/Display/set:1.0
    //
    virtual void set (int l)
        throw(CORBA::SystemException);
  
    
    //
    // IDL:thalesgroup.com/BasicDemo/Display/set:1.0
    //
    virtual int get ()
        throw(CORBA::SystemException);
  
private:
    int val;

};

}; // End of namespace BasicDemo

#endif // ___BASICDEMO_CCM_DISPLAY_IMPL_HPP__

