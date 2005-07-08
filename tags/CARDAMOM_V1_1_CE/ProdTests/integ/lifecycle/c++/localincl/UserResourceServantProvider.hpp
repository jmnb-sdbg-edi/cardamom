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

#ifndef INCL_RESOURCES_USER_RESOURCE_SERVANT_PROVIDER_HPP
#define INCL_RESOURCES_USER_RESOURCE_SERVANT_PROVIDER_HPP

#include "lifecycle/ResourceServantProvider.hpp"
#include "LifeCycle/lifecycle/ServantProviderAllocator.hpp"
//#include "orbsupport/OrbSupport.hpp"
#include <map>
#include <string>

namespace Cdmw
{
namespace Resources
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
class UserResourceServantProvider : public ResourceServantProvider
{
    friend class Cdmw::LifeCycle::ServantProviderAllocator<UserResourceServantProvider>;
   public:
        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~UserResourceServantProvider()
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
            return false;
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
        UserResourceServantProvider(CORBA::ORB_ptr                              orb,
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
        UserResourceServantProvider()
                throw();

        // Hide copy constructor/assignment operator
        UserResourceServantProvider(const UserResourceServantProvider& rhs)
                throw();
        
        UserResourceServantProvider&
        operator=(const UserResourceServantProvider& rhs)
                throw();

        struct ObjInfo
        {
            CF::ResourceNumType     m_type;
            PortableServer::Servant m_serv;

            ObjInfo()
                throw();

            ObjInfo(CF::ResourceNumType     type,
                    PortableServer::Servant serv)
                throw();
            ObjInfo(const ObjInfo & oi)
                throw();
            
            ObjInfo & 
            operator=(const ObjInfo & rhs)
                throw();
        };
        typedef std::map<std::string,ObjInfo> ActiveObjectMap;
        /**
        *[Attribute description]
        */ 
        CORBA::ORB_var                              m_orb;
        PortableServer::POA_var                     m_poa;
        const Cdmw::LifeCycle::ObjectDeactivator & m_object_deactivator;
        ActiveObjectMap                             m_aom; // Active Object Map

}; // End class UserResourceServantProvider

}; // End namespace Resources


// 
namespace LifeCycle
{

template <>
ServantProviderBase*
ServantProviderAllocator<Cdmw::Resources::UserResourceServantProvider>::allocate
                    (CORBA::ORB_ptr                  orb,
                     PortableServer::POA_ptr         adapter,
                     const ObjectDeactivator &       deactivator,
                     int & argc, char** argv)
    throw(std::bad_alloc,
      CORBA::SystemException)
{
    return new Cdmw::Resources::UserResourceServantProvider(orb, adapter, deactivator);
}

#if CDMW_ORB_VDR == orbacus && CDMW_ORB_VER < 410

// This is to avoid the ORBACUS Bug waiting for the V 4.1 version.
template <>
void 
ServantProviderAllocator<Cdmw::Resources::UserResourceServantProvider>::dispose(ServantProviderBase * provider) 
    throw () 
{ 
    // FIXME: This causes a memory leak. It will be corrected with ORBacus/C++ v 4.1
    //    delete provider;
}

#endif

typedef Cdmw::LifeCycle::ServantProviderAllocator<Cdmw::Resources::UserResourceServantProvider> USPAllocator;

};

}; // End namespace Cdmw
#endif // INCL_RESOURCES_USER_RESOURCE_SERVANT_PROVIDER_HPP

