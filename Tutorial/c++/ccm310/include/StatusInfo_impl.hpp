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


#ifndef ___HELLO_STATUSINFO_IMPL_HPP__
#define ___HELLO_STATUSINFO_IMPL_HPP__

#include <list>

#include <Foundation/orbsupport/CORBA.hpp>

#include "Hello_cif.skel.hpp"



namespace Hello
{

//
// IDL:omg.org/Components/StatusInfo:1.0
//
class StatusInfo_impl : virtual public OBV_Hello::StatusInfo,
                        virtual public CORBA::DefaultValueRefCountBase
{
  public:

    StatusInfo_impl();
    StatusInfo_impl(const StatusInfo_impl&);
    ~StatusInfo_impl();

    virtual CORBA::ValueBase* _copy_value();
  
  private:
  
    void operator=(const StatusInfo_impl&);
    
};


class StatusInfoFactory : virtual public CORBA::ValueFactoryBase
{
  public:
  
    StatusInfoFactory();
    virtual ~StatusInfoFactory();
    
  private:
  
    virtual CORBA::ValueBase* create_for_unmarshal();
};


} // End namespce Hello

#endif // ___HELLO_STATUSINFO_IMPL_HPP__



