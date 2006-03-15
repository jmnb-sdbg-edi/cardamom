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


#ifndef INCL_CCM_COMMON_COOKIE_IMPL_HPP
#define INCL_CCM_COMMON_COOKIE_IMPL_HPP

#include <string>

#include <CCMContainer/idllib/Components.skel.hpp>

namespace {


} // End anonymous space


namespace Cdmw {

namespace CCM {
    
//  Forward declaration
    namespace CIF {
        class CCMObject_impl;
    }

namespace Common {

//  Forward declaration
class CookieFactory;
    
//
// IDL:omg.org/Components/Cookie:1.0
//
class Cookie_impl : virtual public OBV_Components::Cookie,
                    virtual public CORBA::DefaultValueRefCountBase
{
    void operator=(const Cookie_impl&);

    friend class CookieFactory;
    friend class Cdmw::CCM::CIF::CCMObject_impl;

protected:
    Cookie_impl(const std::string & s);
    Cookie_impl();
    Cookie_impl(const CORBA::OctetSeq & seq);

public:

    static Cookie_impl* create_cookie()
        throw(Components::ExceededConnectionLimit);
    Cookie_impl(const Cookie_impl&);
    ~Cookie_impl();

    std::string get_cookie_string() const;

    virtual CORBA::ValueBase* _copy_value();
    
    // operator needed to use Cookie as a map key
    bool operator<(const Cookie_impl& right) const
        throw();
    bool operator==(const Cookie_impl& right) const
        throw();
};


class CookieFactory : virtual public CORBA::ValueFactoryBase
{
    virtual CORBA::ValueBase* create_for_unmarshal();

public:
    CookieFactory();
    virtual ~CookieFactory();
};

} // End namespace Common

} // End namespace CCM

} // End namespce Cdmw

#endif // INCL_CCM_COMMON_COOKIE_IMPL_HPP



