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


#ifndef INCL_VALIDATION_USER_ANONYMOUS_RESOURCE_SERVANT_PROVIDER_HPP
#define INCL_VALIDATION_USER_ANONYMOUS_RESOURCE_SERVANT_PROVIDER_HPP

#include "ResourceServantProvider.hpp"
#include "LifeCycle/lifecycle/ServantProviderAllocator.hpp"

#include <map>
#include <string>

namespace Cdmw
{
namespace Validation
{

/**
*Purpose:
*<p>    [class description]
*
*Features:
*<p> [Thread safety, exception safety, performance]
*    [if no pertinent write none ]
*
*@see  [if necessary] 
*@link [if necessary] 
*
*/
class UserAnonymousResourceServantProvider : public Cdmw::Resources::ResourceServantProvider
{
    friend class Cdmw::LifeCycle::ServantProviderAllocator<UserAnonymousResourceServantProvider>;
   public:
        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~UserAnonymousResourceServantProvider()
                throw();

        /**
        * Purpose:
        * <p>This method acts as a hand shaking between the Servant Provider
        * and the generated Factory implementation. It indicates whether
        * a Servant provided by this Servant Provider may incarnate multiple
        * objects (with ObjectIds provided by this Servant Provider)
        *
        *@return <B>true</B> if a servant may incarnate multiple objects, 
        * <B>false</B> otherwise.
        */
        virtual bool may_incarnate_multiple_objects() const throw()
        {
            return true;
        }
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        virtual PortableServer::ObjectId * 
        create_resource(CF::ResourceNumType   resource_number,
                        const CF::Properties& qualifiers)
            throw(CF::InvalidResourceNumber,
                  CF::InvalidProperties,
                  CF::CreationError);
        //
        virtual PortableServer::ObjectId * 
        create_device(const char* label)
            throw(CF::Device::InvalidLocation);
        //
        virtual PortableServer::ObjectId * 
        create_application(const char* name)
            throw(CF::CreateApplicationError);
        //
        PortableServer::Servant 
        lookup_servant(const PortableServer::ObjectId& oid) 
            throw(PortableServer::ForwardRequest,
                  CORBA::SystemException);
        void 
        release_servant(const PortableServer::ObjectId& oid,
                        PortableServer::POA_ptr         poa,
                        PortableServer::Servant         servant) 
            throw(CORBA::SystemException);
        // returns the repository id of the object with oid
        std::string
        get_supported_interface(const PortableServer::ObjectId& oid) const
            throw(CORBA::OBJECT_NOT_EXIST);
        
    protected:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        UserAnonymousResourceServantProvider(CORBA::ORB_ptr                              orb,
                                             PortableServer::POA_ptr                     poa,
                                             const Cdmw::LifeCycle::ObjectDeactivator & deactivator)
            throw(CORBA::SystemException);

    private:
        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        UserAnonymousResourceServantProvider()
                throw();

        // Hide copy constructor/assignment operator
        UserAnonymousResourceServantProvider(const UserAnonymousResourceServantProvider& rhs)
                throw();
        
        UserAnonymousResourceServantProvider&
        operator=(const UserAnonymousResourceServantProvider& rhs)
                throw();

        /**
        *[Attribute description]
        */ 
        CORBA::ORB_var                              m_orb;
        PortableServer::POA_var                     m_poa;
        const Cdmw::LifeCycle::ObjectDeactivator & m_object_deactivator;

}; // End class UserAnonymousResourceServantProvider

}; // End namespace Validation


// 
namespace LifeCycle
{

template <>
ServantProviderBase*
ServantProviderAllocator<Cdmw::Validation::UserAnonymousResourceServantProvider>::allocate
                    (CORBA::ORB_ptr                  orb,
                     PortableServer::POA_ptr         adapter,
                     const ObjectDeactivator &       deactivator,
                     int & argc, char** argv)
    throw(std::bad_alloc,
          CORBA::SystemException)
{
    return new Cdmw::Validation::UserAnonymousResourceServantProvider(orb, adapter, deactivator);
}

typedef Cdmw::LifeCycle::ServantProviderAllocator<Cdmw::Validation::UserAnonymousResourceServantProvider> USPAllocator;

};

}; // End namespace Cdmw
#endif // INCL_VALIDATION_USER_ANONYMOUS_RESOURCE_SERVANT_PROVIDER_HPP

