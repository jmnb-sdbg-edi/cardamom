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


#ifndef CDMW_CCM_COMPONENT_SERVER_DEFAULT_HOME_REGISTRATION_H
#define CDMW_CCM_COMPONENT_SERVER_DEFAULT_HOME_REGISTRATION_H

#include <CCMContainer/ccmcomponentserver/HomeRegistrationBase.hpp>
#include <Repository/naminginterface/NamingInterface.hpp>

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
    class DefaultHomeRegistration : public HomeRegistrationBase
    {        
    public:
        DefaultHomeRegistration(CosNaming::NamingContext_ptr root)
            throw (CORBA::SystemException);
       
        ~DefaultHomeRegistration()
            throw ();

    protected:
        virtual bool bind(Components::CCMHome_ptr home_ref,
                          const char* home_name);
        virtual bool unbind(const char* home_name);
        
    private:        
        DefaultHomeRegistration();
        
        // Hide copy constructor/assignment operator
        DefaultHomeRegistration(const DefaultHomeRegistration& rhs)
            throw();
        
        DefaultHomeRegistration&
        operator=(const DefaultHomeRegistration& rhs)
            throw();

        Cdmw::NamingAndRepository::NamingInterface m_root;
    };

}; // End namespace ComponentServer
}; // End namespace CCM
}; // End namespace Cdmw

#endif // CDMW_CCM_COMPONENT_SERVER_DEFAULT_HOME_REGISTRATION_H

