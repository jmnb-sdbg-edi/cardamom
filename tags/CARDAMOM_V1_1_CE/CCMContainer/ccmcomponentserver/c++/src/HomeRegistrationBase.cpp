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


#include <CCMContainer/ccmcomponentserver/HomeRegistrationBase.hpp>
#include <Foundation/orbsupport/ExceptionMinorCodes.hpp>

#include <algorithm> // for std::find_if
// #include <functional> // for std::bind1st

namespace
{
    class CCMHomeFinder
    {
    public:
        
        CCMHomeFinder(Components::CCMHome_ptr home)
            throw (CORBA::SystemException)
            : m_home(Components::CCMHome::_duplicate(home))
        {
        }
        CCMHomeFinder(const CCMHomeFinder & rhs)
            throw (CORBA::SystemException)
            : m_home(rhs.m_home)
        {
        }
        
        bool operator()(const std::pair<std::string,Components::CCMHome_var> & val)
        {
            if (val.second->_is_equivalent(m_home.in()))
                return true;
            else
                return false;
        }
    private:
        Components::CCMHome_var m_home;
    };
}; // End of anonymous namespace

namespace Cdmw
{
namespace CCM
{
namespace ComponentServer
{
    
HomeRegistrationBase::HomeRegistrationBase()
    throw ()
{
}

    
HomeRegistrationBase::~HomeRegistrationBase()
    throw ()
{
}

//
// IDL:omg.org/Components/HomeRegistration/register_home:1.0
//
void
HomeRegistrationBase::register_home(Components::CCMHome_ptr home_ref,
                                     const char* home_name)
    throw (CORBA::SystemException)
{
    if (bind(home_ref,home_name)) {
        Components::CCMHome_var home(Components::CCMHome::_duplicate(home_ref));
        m_registered_homes.insert(std::make_pair(std::string(home_name),home));
    }
}

//
// IDL:omg.org/Components/HomeRegistration/unregister_home:1.0
//
void
HomeRegistrationBase::unregister_home(Components::CCMHome_ptr home_ref)
    throw (CORBA::SystemException)
{
    HomesMap::iterator i =
        std::find_if(m_registered_homes.begin(),
                     m_registered_homes.end(),
                     CCMHomeFinder(home_ref));
    if (i != m_registered_homes.end()) {        
        unbind((*i).first.c_str());
        m_registered_homes.erase(i);
    } else 
        throw CORBA::BAD_PARAM(Cdmw::OrbSupport::BAD_PARAMHomeNotRegistered,
                               CORBA::COMPLETED_NO);
}

}; // End namespace ComponentServer
}; // End namespace CCM
}; // End namespace Cdmw

