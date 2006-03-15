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


#ifndef CDMW_CCM_COMPONENT_SERVER_COMPONENT_SERVER_IMPL_H
#define CDMW_CCM_COMPONENT_SERVER_COMPONENT_SERVER_IMPL_H

#include <Foundation/orbsupport/CORBA.hpp>
#include <CCMContainer/idllib/Components.skel.hpp>
#include <Foundation/orbsupport/RefCountServantBase.hpp>
#include <CCMContainer/idllib/CdmwCcmComponentServer.skel.hpp>
#include <CCMContainer/idllib/CdmwDeployment.stub.hpp>
#include <Event/idllib/CdmwEvent.stub.hpp>
#include <list>
#include <Foundation/osthreads/Mutex.hpp>

namespace Cdmw
{
namespace CCM
{
namespace ComponentServer
{
    /**
     * The ComponentServer acts as a singleton factory for the creation of Container
     * objects and is used by an Assembly object during the deployment process.
     */
    class ComponentServer_impl : virtual public POA_CdmwCcmComponentServer::ComponentServer,
                                 virtual public Cdmw::OrbSupport::RefCountServantBase
    {        
    public:  
        ComponentServer_impl(CORBA::ORB_ptr                      orb,
                             PortableServer::POA_ptr             poa,
                             Components::HomeRegistration_ptr    home_registration,
                             CdmwEvent::EventChannelFactory_ptr event_channel_factory,
                             const std::string &                 process_name,
                             const std::string &                 application_name,
                             const Components::ConfigValues&     config)
            throw (CORBA::SystemException);
       
        ~ComponentServer_impl()
            throw ();

        virtual PortableServer::POA_ptr _default_POA() {
            return PortableServer::POA::_duplicate(m_poa.in());
        }
        //
        // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/supports:1.0
        //
        /**
         * This operation returns whether the ComponentServer supports the 
         * creation of Containers with the specified repository Id.
         *
         * @param repository_id The Repository Id of the desired Container interface.
         *
         * @return <B>true</B> if the component server supports/creates containers of 
         * <I>repository_id</I> type or of derived types. Otherwise it returns
         * <B>false</B>.
         */
        virtual CORBA::Boolean supports(const char* repository_id)
            throw(CORBA::SystemException);
        
        //
        // IDL:thalesgroup.com/CdmwLifeCycle/FactoryBase/get_supported_objects:1.0
        //
        /**
         * This operation returns a sequence of CORBA Repository IDs of Containers 
         * that can be created by the componentserver.
         *
         * @return A sequence of CORBA Repository IDs of containers that 
         * can be created by the component server.
         */ 
        virtual CdmwLifeCycle::RepositoryIdSeq* get_supported_objects()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/configuration:1.0
        //
        /**
         * Purpose:
         * <p>Gives access to the ConfigValue objects passed in to the create_component_server
         * operation at the ComponentServer creation.
         * 
         *@return A sequence of ConfigValue objects passed in to the <I>create_component_server</I>
         * operation at the ComponentServer creation.
         */
        virtual Components::ConfigValues*
        configuration()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/get_server_activator:1.0
        //
        /**
         * Purpose:
         * <p>Gives access to the ServerActivator object that created this ComponentServer.
         *
         *@return Object reference to the ServerActivator object that created this ComponentServer.
         */
        virtual Components::Deployment::ServerActivator_ptr
        get_server_activator()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/create_container:1.0
        //
        /**
         * Purpose:
         * <p>The create_container operation creates a new Container object within the
         * component server process.
         *
         *@param config Sequence of ConfigValue objects that provide name value pairs used
         * to configure the new Container.
         *
         * <p> <B>NOTA:</B> These are the configuration values understood by the component server
         * for container creation.
         * <TABLE>
         *   <TR>
         *      <TD><U><B>Config Name</B></U></TD><TD><U><B>Type</B></U></TD>
         *   </TR>
         *   <TR>
         *      <TD>COMPONENT_KIND (*)</TD><TD>ComponentKindValue</TD>
         *   </TR>
         * </TABLE>
         * <p>
         * <TABLE>
         *   <TR>
         *      <TD>(*)</TD><TD>mandatory</TD>
         *   </TR>
         * </TABLE>
         *
         *@return Object reference to the newly created container object.
         *
         *@exception Components::Deployment::InvalidConfiguration Raise if the configuration
         * values are not valid.
         *@exception Components::CreateFailure Raised if a container could not be created for
         * internal reasons (such as insufficient resources).
         */
        virtual Components::Deployment::Container_ptr
        create_container(const Components::ConfigValues& config)
            throw(Components::CreateFailure,
                  Components::Deployment::InvalidConfiguration,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/remove_container:1.0
        //
        /**
         * Purpose:
         * <p>The remove_container operation removes a previously created container and all
         * homes and components that were created in the container.
         *
         *@param cref Referes to the container that should be removed.
         *
         *@exception CORBA::BAD_PARAM Raised if the <I>cref</I> parameter was not
         * created by this ComponentServer.
         *@exception Components::RemoveFailure Raised if the container or any of its homes
         * could not be removed for any internal reasons.
         */
        virtual void
        remove_container(Components::Deployment::Container_ptr cref)
            throw(Components::RemoveFailure,
                  CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/get_containers:1.0
        //
        /**
         * Purpose:
         * <p> The get_containers operation returns a sequence of all Container objects
         * created by this ComponentServer.
         *
         *@exception CORBA::SystemException TODO:
         */
        virtual Components::Deployment::Containers*
        get_containers()
            throw(CORBA::SystemException);
        
        //
        // IDL:omg.org/Components/Deployment/ComponentServer/remove:1.0
        //
        /**
         * Purpose:
         * <p>The remove operation removes the component server and all containers, homes, and
         * components that were created in the component server.
         *
         *@exception Components::RemoveFailure Raised if the component server or any of its
         * containers could not be removed for any internal reasons.
         *@exception CORBA::SystemException TODO:
         */
        virtual void
        remove()
            throw(Components::RemoveFailure,
                  CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CdmwCcmComponentServer/ComponentServer/set_component_installation:1.0
        //
        virtual void
        set_component_installation(CdmwDeployment::ComponentInstallation_ptr component_installation)
            throw(CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CdmwCcmComponentServer/ComponentServer/set_server_activator:1.0
        //
        virtual void
        set_server_activator(Components::Deployment::ServerActivator_ptr server_activator)
            throw(CORBA::SystemException);
    private:
        ComponentServer_impl()
            throw();
        
        // Hide copy constructor/assignment operator
        ComponentServer_impl(const ComponentServer_impl& rhs)
            throw();
        
        ComponentServer_impl&
        operator=(const ComponentServer_impl& rhs)
            throw();

        void remove_all_containers()
            throw(Components::RemoveFailure,
                  CORBA::SystemException);
        //
        // Attributes
        //
    
        /**
         * A reference to the ORB pseudo-object
         */
        CORBA::ORB_var                                    m_orb;
        /**
         * A parent POA used for the component server
         */
        PortableServer::POA_var                           m_poa;
        /**
         * A POA used for single thread component homes
         */
        PortableServer::POA_var m_homes_serialize_poa;
        /**
         * A POA used for thread-safe component homes
         */
        PortableServer::POA_var m_homes_multithread_poa;
        /**
         * A POA Current object
         */
        PortableServer::Current_var                       m_current;
        /**
         * A sequence of ConfigValue objects passed in to the
         * <I>ServerActivator::create_component_server</I> operation at the Container creation.
         */
        Components::ConfigValues                          m_config_values;
        /**
         * A reference to the ServerActivator object
         */
        Components::Deployment::ServerActivator_var       m_server_activator;
        /**
         * A reference to the ComponentInstallation object
         */
        CdmwDeployment::ComponentInstallation_var        m_component_installation;
        /**
         * A reference to a HomeRegistration for use to register component homes so they may be
         * located by the HomeFinder.
         */
        Components::HomeRegistration_var                  m_home_registration;
        /**
         * The CDMW Event Channel Factory.
         */
        CdmwEvent::EventChannelFactory_var               m_event_channel_factory;
        /**
         * The CDMW Process name.
         */
        std::string                                       m_process_name;
        /**
         * The CDMW Application name.
         */
        std::string                                       m_application_name;
        /**
         * A list of all the containers created by the component server.
         */
        std::list<Components::Deployment::Container_var>  m_containers;
        /**
         * Indicates whether the remove operation has been called or not.
         */ 
        bool m_is_removed;
        /**
         * A mutex to protect from concurrent access to local data.
         */ 
        Cdmw::OsSupport::Mutex m_component_server_mutex;
    };

}; // End namespace ComponentServer
}; // End namespace CCM
}; // End namespace Cdmw

#endif // CDMW_CCM_COMPONENT_SERVER_COMPONENT_SERVER_IMPL_H

