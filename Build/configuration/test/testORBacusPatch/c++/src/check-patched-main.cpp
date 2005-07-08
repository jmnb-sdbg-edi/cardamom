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


#include <iostream>
#include <cstdlib>
#include <string>

#include <OB/CORBA.h>

#include "check-patched_skel.h"


//#define EXPLICIT_DEACTIVATE
//#define FORCE_POA_DESTROY

namespace
{
    const int SUCCESS = 0;
    const int FAILURE = 1;
}

; // End anonymous namespace


// ----------------------------------------------------------------------
// Resource Servant activator
// ----------------------------------------------------------------------

class ResourceServantActivator :
            public virtual POA_PortableServer::ServantActivator,
            public virtual PortableServer::RefCountServantBase
{

    public:

        ResourceServantActivator()
        {
            //  std::cerr << "ResourceServantActivator()" << std::endl;
        }

        ~ResourceServantActivator()
        {
            //  std::cerr << "~ResourceServantActivator()" << std::endl;
        }

        virtual PortableServer::Servant
        incarnate(const PortableServer::ObjectId& oid,
                  PortableServer::POA_ptr poa)
        throw(PortableServer::ForwardRequest, CORBA::SystemException)
        {
            //std::cerr << "ResourceServantActivator::incarnate()" << std::endl;
            throw CORBA::OBJECT_NOT_EXIST();
        }

        virtual void
        etherealize(const PortableServer::ObjectId& oid,
                    PortableServer::POA_ptr poa,
                    PortableServer::Servant servant,
                    CORBA::Boolean cleanup_in_progress,
                    CORBA::Boolean remaining_activations)
        throw(CORBA::SystemException)
        {
            //std::cerr << "ResourceServantActivator::etherealize()" << std::endl;

            CORBA::String_var s = PortableServer::ObjectId_to_string(oid);

            if (!remaining_activations)
            {
                servant->_remove_ref();
            }
        }
};

// ----------------------------------------------------------------------
// Resource_impl
// ----------------------------------------------------------------------

class Resource_impl :
            public virtual POA_ResFac::Resource,
            public virtual PortableServer::RefCountServantBase
{

    public:
        Resource_impl (PortableServer::POA_ptr poa)
        throw(CORBA::SystemException)
                : m_poa(PortableServer::POA::_duplicate(poa))
        {
            //    std::cerr << "Resource_impl()" << std::endl;
        }

        virtual ~Resource_impl() throw()
        {
            //   std::cerr << "~Resource_impl()" << std::endl;
        }

        virtual PortableServer::POA_ptr _default_POA()
        {
            return PortableServer::POA::_duplicate(m_poa.in());
        }

        virtual void start()
        throw(CORBA::SystemException)
        {}

        virtual void stop()
        throw(CORBA::SystemException)
        {
#ifdef EXPLICIT_DEACTIVATE
            PortableServer::ObjectId_var oid = m_poa->servant_to_id(this);
            m_poa->deactivate_object(oid.in());
#endif

        }

    private:
        PortableServer::POA_var m_poa;
}

; // End class Resource_impl

// ----------------------------------------------------------------------
// The Resource Factory
// ----------------------------------------------------------------------

class Factory :
            public virtual POA_ResFac::ResourceFactory,
            public virtual PortableServer::RefCountServantBase
{

    public:
        Factory (CORBA::ORB_ptr orb,
                 PortableServer::POA_ptr parent)
        throw(CORBA::SystemException)
                : m_orb(CORBA::ORB::_duplicate(orb)),
                m_poa(PortableServer::POA::_duplicate(parent)),
                m_objects_poa(PortableServer::POA::_nil())
        {
            //std::cerr << "Factory()" << std::endl;

            PortableServer::POAManager_var poaManager = parent -> the_POAManager();
            CORBA::PolicyList policies;
            policies.length(6);
            policies[0] =
                parent -> create_id_assignment_policy(PortableServer::USER_ID);
            policies[1] =
                parent -> create_lifespan_policy(PortableServer::TRANSIENT); //PERSISTENT);
            policies[2] =
                parent -> create_servant_retention_policy(PortableServer::RETAIN);
            policies[3] =
                parent -> create_id_uniqueness_policy(PortableServer::UNIQUE_ID);
            policies[4] = parent ->
                          create_request_processing_policy(PortableServer::USE_SERVANT_MANAGER);
            policies[5] = parent ->
                          create_implicit_activation_policy(PortableServer::NO_IMPLICIT_ACTIVATION);

            m_objects_poa = m_poa -> create_POA("ResourcePOA", poaManager, policies);

            //
            // Create and install servant activator
            //
            ResourceServantActivator* servant_activator = new ResourceServantActivator();
            PortableServer::ServantBase_var serv = servant_activator;
            PortableServer::ServantActivator_var the_activator
            = servant_activator -> _this();
            m_objects_poa -> set_servant_manager(the_activator);
        }

        virtual ~Factory() throw()
        {
            //std::cerr << "~Factory()" << std::endl;
        }

        void shutdown() throw()
        {
            //std::cerr << "Factory::shutdown(etherealize_objects = true,\n"
            //          << "                  wait_for_completion = true)" << std::endl;

            m_objects_poa->destroy(true, true);
        }

        virtual PortableServer::POA_ptr _default_POA()
        {
            return PortableServer::POA::_duplicate(m_poa.in());
        }

        virtual ResFac::Resource_ptr create_resource(const char* resource_name)
        throw(CORBA::SystemException)
        {
            PortableServer::ObjectId_var oid
            = PortableServer::string_to_ObjectId(resource_name);
            PortableServer::Servant servant = new Resource_impl(m_objects_poa.in());

            m_objects_poa->activate_object_with_id (oid.in(), servant);

            CORBA::Object_var obj = m_objects_poa -> id_to_reference (oid);

            ResFac::Resource_var res = ResFac::Resource::_narrow (obj);

            return res._retn();
        }

    private:
        CORBA::ORB_var m_orb;
        PortableServer::POA_var m_poa;
        PortableServer::POA_var m_objects_poa;
}

; // End class Factory

int
run(CORBA::ORB_ptr orb, int & argc, char** argv)
{
    //
    // Resolve the Root POA
    //
    CORBA::Object_var obj = orb -> resolve_initial_references("RootPOA");
    PortableServer::POA_var root = PortableServer::POA::_narrow(obj.in());
    PortableServer::POAManager_var poaManager = root -> the_POAManager();

    //
    // Initialize the server so that incoming requests are
    // handled from this point on
    //
    poaManager -> activate();

    Factory * fac_impl = new Factory(orb, root.in());
    PortableServer::ServantBase_var serv = fac_impl;

    ResFac::ResourceFactory_var fac = fac_impl->_this();

    ResFac::Resource_var res = fac->create_resource("Resource 1");

    res->start();

    for (int i = 0; i < 100000;++i)
    {
        // tempo
    }

    res->stop();

#ifdef FORCE_POA_DESTROY

    fac_impl->shutdown();
#endif

    for (int i = 0; i < 100000;++i)
    {
        // tempo
    }


    return SUCCESS;
}

int main(int argc, char* argv[])
{

    int status = SUCCESS;
    CORBA::ORB_var orb;

    try
    {
        orb = CORBA::ORB_init(argc, argv);
        status = run(orb, argc, argv);
    }
    catch (const CORBA::Exception& ex)
    {
        status = FAILURE;
    }

    if (!CORBA::is_nil(orb))
    {
        try
        {
            orb -> destroy();
        }
        catch (const CORBA::Exception& ex)
        {
            status = FAILURE;
        }
    }


    if (status == 0)
        std::cout << "Ok" << std::endl;
    else
        std::cout << "Failed" << std::endl;


    return status;
}

