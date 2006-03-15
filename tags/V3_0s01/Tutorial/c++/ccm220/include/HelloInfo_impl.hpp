/* =========================================================================== *
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
 * =========================================================================== */


#ifndef ___HELLO_HELLOINFO_IMPL_HPP__
#define ___HELLO_HELLOINFO_IMPL_HPP__

#include <list>

#include <Foundation/orbsupport/CORBA.hpp>

#include "Hello_cif.skel.hpp"



namespace Hello
{

//
// IDL:omg.org/Components/HelloInfo:1.0
//
class HelloInfo_impl : virtual public OBV_Hello::HelloInfo,
                       virtual public CORBA::DefaultValueRefCountBase
{
  public:

    HelloInfo_impl();
    HelloInfo_impl (const char* hello_text);
    
    HelloInfo_impl(const HelloInfo_impl&);
    
    ~HelloInfo_impl();

    virtual CORBA::ValueBase* _copy_value();
  
  private:
  
    void operator=(const HelloInfo_impl&);
    
};


class HelloInfoFactory : virtual public CORBA::ValueFactoryBase
{
  public:
  
    HelloInfoFactory();
    virtual ~HelloInfoFactory();
    
  private:
  
    virtual CORBA::ValueBase* create_for_unmarshal();
};


} // End namespace Hello

#endif // ___HELLO_HELLOINFO_IMPL_HPP__



