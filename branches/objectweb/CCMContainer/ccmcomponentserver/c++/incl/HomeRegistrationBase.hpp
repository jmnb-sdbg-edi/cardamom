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


#ifndef CDMW_CCM_COMPONENT_SERVER_HOME_REGISTRATION_BASE_H
#define CDMW_CCM_COMPONENT_SERVER_HOME_REGISTRATION_BASE_H

#include <Foundation/orbsupport/CORBA.hpp>
#include "CCMContainer/idllib/Components.skel.hpp"
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <string>
#include <map>
#include <functional> // for std::less<>

namespace Cdmw
{
namespace CCM
{
namespace ComponentServer
{
    /**
     * The HomeRegistration acts as a singleton factory for the creation of Container
     * objects and is used by an Assembly object during the deployment process.
     */
    class HomeRegistrationBase : public Components::HomeRegistration,
                                 public Cdmw::OrbSupport::RefCountLocalObject
    {        
    public:  
       
        ~HomeRegistrationBase()
            throw ();

        //
        // IDL:omg.org/Components/HomeRegistration/register_home:1.0
        //
        virtual void register_home(Components::CCMHome_ptr home_ref,
                                   const char* home_name)
            throw (CORBA::SystemException);

        //
        // IDL:omg.org/Components/HomeRegistration/unregister_home:1.0
        //
        virtual void unregister_home(Components::CCMHome_ptr home_ref)
            throw (CORBA::SystemException);

    protected:
        HomeRegistrationBase()
            throw();
        virtual bool bind(Components::CCMHome_ptr home_ref,
                          const char* home_name) = 0;
        virtual bool unbind(const char* home_name) = 0;
        
    private:
        
        // Hide copy constructor/assignment operator
        HomeRegistrationBase(const HomeRegistrationBase& rhs)
            throw();
        
        HomeRegistrationBase&
        operator=(const HomeRegistrationBase& rhs)
            throw();

        
        typedef std::map<std::string,Components::CCMHome_var,std::less<std::string> > HomesMap;
        HomesMap m_registered_homes;
    };

}; // End namespace ComponentServer
}; // End namespace CCM
}; // End namespace Cdmw

#endif // CDMW_CCM_COMPONENT_SERVER_HOME_REGISTRATION_BASE_H

