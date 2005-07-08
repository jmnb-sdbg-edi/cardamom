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


#ifndef CDMW_CCM_CONTAINER_MY_COMPONENT_SERVER_H
#define CDMW_CCM_CONTAINER_MY_COMPONENT_SERVER_H

#include <Foundation/orbsupport/CORBA.hpp>
#include "CCMContainer/idllib/Components.skel.hpp"
#include "CCMContainer/idllib/CdmwDeployment.stub.hpp"
#include <Foundation/orbsupport/RefCountServantBase.hpp>

namespace TestCdmwCCM
{
    
    class MyComponentServer : virtual public POA_Components::Deployment::ComponentServer,
                              virtual public Cdmw::OrbSupport::RefCountServantBase
    {        
    public:  
        MyComponentServer(CORBA::ORB_ptr          orb,
                          PortableServer::POA_ptr poa,
                          CdmwDeployment::ComponentInstallation_ptr component_installation)
            throw (CORBA::SystemException);
       
        ~MyComponentServer()
            throw ();

        virtual PortableServer::POA_ptr _default_POA() {
            return PortableServer::POA::_duplicate(m_poa.in());
        }
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/configuration:1.0
        //
        virtual Components::ConfigValues* configuration()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/get_server_activator:1.0
        //
        virtual Components::Deployment::ServerActivator_ptr get_server_activator()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/create_container:1.0
        //
        virtual Components::Deployment::Container_ptr create_container(const Components::ConfigValues& config)
            throw(Components::CreateFailure,
                  Components::Deployment::InvalidConfiguration,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/remove_container:1.0
        //
        virtual void remove_container(Components::Deployment::Container_ptr cref)
            throw(Components::RemoveFailure,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/get_containers:1.0
        //
        virtual Components::Deployment::Containers* get_containers()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/remove:1.0
        //
        virtual void remove()
            throw(Components::RemoveFailure,
                  CORBA::SystemException);
    private:
        MyComponentServer()
            throw();
        
        // Hide copy constructor/assignment operator
        MyComponentServer(const MyComponentServer& rhs)
            throw();
        
        MyComponentServer&
        operator=(const MyComponentServer& rhs)
            throw();

        CORBA::ORB_var                                    m_orb;
        PortableServer::POA_var                           m_poa;
        Components::Deployment::Container_var             m_container;
        PortableServer::Current_var                       m_current;
        CdmwDeployment::ComponentInstallation_var        m_component_installation;
        bool                                              m_is_removed;
    };

}; // End namespace TestCdmwCCM



#endif // CDMW_CCM_CONTAINER_MY_COMPONENT_SERVER_H

