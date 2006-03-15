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

#include <iostream>

#include "testccmcif/CCM_Display_impl.hpp"
#include "testccmcif/Event_impl.hpp"

//
// IDL:thalesgroup.com/BasicDemo/CCM_Display:1.0
//
BasicDemo::CCM_Display_impl::CCM_Display_impl()
{
}

BasicDemo::CCM_Display_impl::~CCM_Display_impl()
{
    std::cout << "CCM_Display_impl::~CCM_Display_impl called!" << std::endl;
}

//
// IDL:thalesgroup.com/BasicDemo/Display/print:1.0
//
void
BasicDemo::CCM_Display_impl::print(const char* text)
    throw(CORBA::SystemException)
{
    // print the message on the default output
    std::cout << text << std::endl;
}


//
// IDL:thalesgroup.com/BasicDemo/Display/print_event:1.0
//
void 
BasicDemo::CCM_Display_impl::print_event(BasicDemo::Event * event)
    throw(CORBA::SystemException)
{
    // print the message on the default output
    std::cout << event->text() << std::endl;
}


//
// IDL:thalesgroup.com/BasicDemo/Display/test_short:1.0
//
CORBA::Short 
BasicDemo::CCM_Display_impl::test_short (CORBA::Short p1,
                                         CORBA::Short & p2,
                                         CORBA::Short_out p3)
    throw(CORBA::SystemException)
{
  p2 = (CORBA::Short) (p1 * 2);
  p3 = (CORBA::Short) (p1 * 3);
  return (CORBA::Short) (p1 * 4);
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_long:1.0
//
CORBA::Long 
BasicDemo::CCM_Display_impl::test_long (CORBA::Long p1,
                                        CORBA::Long & p2,
                                        CORBA::Long_out p3)
    throw(CORBA::SystemException)
{
  p2 = p1 * 2;
  p3 = p1 * 3;
  return p1 * 4;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_long_long:1.0
//
CORBA::LongLong
BasicDemo::CCM_Display_impl::test_long_long (CORBA::LongLong p1,
                                             CORBA::LongLong & p2,
                                             CORBA::LongLong_out p3)
    throw(CORBA::SystemException)
{
  p2 = p1 * 2;
  p3 = p1 * 3;
  return p1 * 4;
}
    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_unsigned_short:1.0
//
CORBA::UShort
BasicDemo::CCM_Display_impl::test_unsigned_short (CORBA::UShort p1,
                                                  CORBA::UShort & p2,
                                                  CORBA::UShort_out p3)
    throw(CORBA::SystemException)
{
  p2 = p1 * 2;
  p3 = p1 * 3;
  return p1 * 4;
}
    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_unsigned_long:1.0
//
CORBA::ULong 
BasicDemo::CCM_Display_impl::test_unsigned_long (CORBA::ULong p1,
                                                 CORBA::ULong & p2,
                                                 CORBA::ULong_out p3)
    throw(CORBA::SystemException)
{
  p2 = p1 * 2;
  p3 = p1 * 3;
  return p1 * 4;
}
    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_unsigned_long_long:1.0
//
CORBA::ULongLong 
BasicDemo::CCM_Display_impl::test_unsigned_long_long (CORBA::ULongLong p1,
                                                      CORBA::ULongLong_out p2,
                                                      CORBA::ULongLong_out p3)
    throw(CORBA::SystemException)
{
  p2 = p1 * 2;
  p3 = p1 * 3;
  return p1 * 4;
}
    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_float:1.0
//
CORBA::Float 
BasicDemo::CCM_Display_impl::test_float (CORBA::Float p1,
                                         CORBA::Float & p2,
                                         CORBA::Float_out p3)
    throw(CORBA::SystemException)
{
  p2 = p1 * 2;
  p3 = p1 * 3;
  return p1 * 4;
}
    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_double:1.0
//
CORBA::Double 
BasicDemo::CCM_Display_impl::test_double (CORBA::Double p1,
                                          CORBA::Double_out p2,
                                          CORBA::Double_out p3)
    throw(CORBA::SystemException)
{
  p2 = p1 * 2;
  p3 = p1 * 3;
  return p1 * 4;
}
    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_boolean:1.0
//
CORBA::Boolean 
BasicDemo::CCM_Display_impl::test_boolean (CORBA::Boolean p1,
                                           CORBA::Boolean & p2,
                                           CORBA::Boolean_out p3)
    throw(CORBA::SystemException)
{
  p2 = p1;
  p3 = p1;
  return p1;
}
    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_char:1.0
//
CORBA::Char 
BasicDemo::CCM_Display_impl::test_char (CORBA::Char p1,
                                        CORBA::Char & p2,
                                        CORBA::Char_out p3)
    throw(CORBA::SystemException)
{
    p2 = p1;
    p3 = p1;
    return p1;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_wchar:1.0
//
CORBA::WChar 
BasicDemo::CCM_Display_impl::test_wchar (CORBA::WChar p1,
                                         CORBA::WChar & p2,
                                         CORBA::WChar_out p3)
    throw(CORBA::SystemException)
{
    p2 = p1;
    p3 = p1;
    return p1;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_octet:1.0
//
CORBA::Octet 
BasicDemo::CCM_Display_impl::test_octet (CORBA::Octet p1,
                                         CORBA::Octet & p2,
                                         CORBA::Octet_out p3)
    throw(CORBA::SystemException)
{
    p2 = p1;
    p3 = p1;
    return p1;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_enum:1.0
//
::BasicDemo::Display::enum1 
BasicDemo::CCM_Display_impl::test_enum (BasicDemo::Display::enum1 p1,
                                        BasicDemo::Display::enum1 & p2,
                                        BasicDemo::Display::enum1_out p3)
    throw(CORBA::SystemException)
{
    return p1;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_function:1.0
//
::BasicDemo::Display_ptr 
BasicDemo::CCM_Display_impl::test_function (BasicDemo::Display_ptr p1,
                                            BasicDemo::Display_ptr & p2,
                                            BasicDemo::Display_out p3)
    throw(CORBA::SystemException)
{
    ::BasicDemo::Display_ptr ret = ::BasicDemo::Display::_nil();
    if (!CORBA::is_nil (p2))
        CORBA::release (p2);
    BasicDemo::Display_var myobj = BasicDemo::Display::_duplicate (p1);
    CORBA::Boolean equiv = myobj->_is_equivalent (p1);
    
    if (equiv)
    {
        p2 = ::BasicDemo::Display::_duplicate (myobj.in ());
        p3 = ::BasicDemo::Display::_duplicate (myobj.in ());
        ret = ::BasicDemo::Display::_duplicate (myobj.in ());
        p2->set(1);
        p3->set(1);
        ret->set(1);
    }
    else
    {
        p2 = ::BasicDemo::Display::_nil ();
        p3 = ::BasicDemo::Display::_nil ();
    }
    return ret;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_fixed_struct:1.0
//
::BasicDemo::s1 
BasicDemo::CCM_Display_impl::test_fixed_struct (const BasicDemo::s1 & p1,
                                                BasicDemo::s1 & p2,
                                                BasicDemo::s1_out p3)
    throw(CORBA::SystemException)
{
  p2 = p1;
  p3 = p1;
  return p1;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_var_struct:1.0
//
::BasicDemo::s2 * 
BasicDemo::CCM_Display_impl::test_var_struct (const BasicDemo::s2 & p1,
                                              BasicDemo::s2 & p2,
                                              BasicDemo::s2_out p3)
    throw(CORBA::SystemException)
{
    BasicDemo::s2 *ret = new BasicDemo::s2;
    BasicDemo::s2 *out = new BasicDemo::s2;
    // now copy all elements of s1 into the others
    p2 = p1;
    *out = p1;
    *ret = p1;
    p3 = out;
    return ret;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_fixed_union:1.0
//
::BasicDemo::Display::union1 
BasicDemo::CCM_Display_impl::test_fixed_union (const BasicDemo::Display::union1 & p1,
                                               BasicDemo::Display::union1 & p2,
                                               BasicDemo::Display::union1_out p3)
    throw(CORBA::SystemException)
{
    p2 = p1;
    p3 = p1;
    return p1;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_var_union:1.0
//
::BasicDemo::Display::union2 * 
BasicDemo::CCM_Display_impl::test_var_union (const BasicDemo::Display::union2 & p1,
                                             BasicDemo::Display::union2 & p2,
                                             BasicDemo::Display::union2_out p3)
    throw(CORBA::SystemException)
{
    ::BasicDemo::Display::union2_var ret (new ::BasicDemo::Display::union2 (p1));
    p2 = p1;
    p3 = new ::BasicDemo::Display::union2 (p1);
    return ret._retn (); 
}
    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_seq_struct:1.0
//
::BasicDemo::s1s * 
BasicDemo::CCM_Display_impl::test_seq_struct (const BasicDemo::s1s & p1,
                                              BasicDemo::s1s & p2,
                                              BasicDemo::s1s_out p3)
    throw(CORBA::SystemException)
{
    ::BasicDemo::s1s *ret = new ::BasicDemo::s1s;
    ::BasicDemo::s1s *out = new ::BasicDemo::s1s;

    // now copy all elements of s1 into the others using the assignment operator
    p2 = p1;
    *out = p1;
    *ret = p1;
    p3 = out;
    return ret;
}
    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_string:1.0
//
char * BasicDemo::CCM_Display_impl::test_string (const char * p1,
                                                 char *& p2,
                                                 CORBA::String_out p3)
    throw(CORBA::SystemException)
{
    char *retstr = CORBA::string_dup (p1);
    p3 = CORBA::string_dup (p1);
    p2 = CORBA::string_dup (p1);
    return retstr;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_wstring:1.0
//
CORBA::WChar * 
BasicDemo::CCM_Display_impl::test_wstring (const CORBA::WChar * p1,
                                           CORBA::WChar *& p2,
                                           CORBA::WString_out p3)
    throw(CORBA::SystemException)
{
    CORBA::WChar *retstr = CORBA::wstring_dup (p1);
    p3 = CORBA::wstring_dup (p1);
    p2 = CORBA::wstring_dup (p1);
    return retstr;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_fixed_array:1.0
//
::BasicDemo::Display::array1_slice * 
BasicDemo::CCM_Display_impl::test_fixed_array (const BasicDemo::Display::array1 p1,
                                               BasicDemo::Display::array1 p2,
                                               BasicDemo::Display::array1_out p3)
    throw(CORBA::SystemException)
{
    BasicDemo::Display::array1_slice *ret;

    BasicDemo::Display::array1_copy (p2, p1);
    BasicDemo::Display::array1_copy (p3, p1);
    ret = BasicDemo::Display::array1_dup (p1);
    return ret;
}

//
// IDL:thalesgroup.com/BasicDemo/Display/test_var_array:1.0
//
::BasicDemo::Display::array2_slice * 
BasicDemo::CCM_Display_impl::test_var_array (const BasicDemo::Display::array2 p1,
                                             BasicDemo::Display::array2 p2,
                                             BasicDemo::Display::array2_out p3)
    throw(CORBA::SystemException)
{
    BasicDemo::Display::array2_slice *ret;
    BasicDemo::Display::array2_copy (p2, p1);
    p3 = BasicDemo::Display::array2_dup (p1);
    ret = BasicDemo::Display::array2_dup (p1);
    return ret;
}
  
//
// IDL:thalesgroup.com/BasicDemo/Display/test_any:1.0
//
CORBA::Any * 
BasicDemo::CCM_Display_impl::test_any (const CORBA::Any & p1,
                                       CORBA::Any & p2,
                                       CORBA::Any_out p3)
    throw(CORBA::SystemException)
{
    CORBA::Any *ret;
    p2 = p1;
    p3 = new CORBA::Any (p1);
    ret = new CORBA::Any (p1);
    return ret;
}

    
//
// IDL:thalesgroup.com/BasicDemo/Display/test_event:1.0
//
::BasicDemo::Event * 
BasicDemo::CCM_Display_impl::test_event (BasicDemo::Event * p1,
                                         BasicDemo::Event *& p2,
                                         BasicDemo::Event_out p3)
    throw(CORBA::SystemException)
{
    p2->text("Hello");
    p3 = new BasicDemo::Event_impl();
    p3->text("Hello");
    BasicDemo::Event* ret = new BasicDemo::Event_impl();
    ret->text("Hello");
    return ret ;
}



//
// IDL:thalesgroup.com/BasicDemo/Display/set:1.0
//
void BasicDemo::CCM_Display_impl::set (int l)
    throw(CORBA::SystemException)
{
    val = l;
}


    
//
// IDL:thalesgroup.com/BasicDemo/Display/set:1.0
//
int BasicDemo::CCM_Display_impl::get ()
    throw(CORBA::SystemException)
{
    return val;
}
