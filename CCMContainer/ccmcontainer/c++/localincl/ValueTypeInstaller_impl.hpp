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


#ifndef INCL_CDMW_CCM_CONTAINER_VALUE_TYPE_INSTALLER_IMPL_HPP
#define INCL_CDMW_CCM_CONTAINER_VALUE_TYPE_INSTALLER_IMPL_HPP


#include <Foundation/orbsupport/CORBA.hpp>
#include "CCMContainer/idllib/CdmwCcmContainer.skel.hpp"
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include "Foundation/osthreads/Mutex.hpp"
#include "Foundation/ossupport/OS.hpp"
#include <string>
#include <map>


namespace Cdmw
{

namespace CCM
{
namespace Container
{
    /**
     * Implementation of the Session2Context internal interface for use with
     * Session container type.
     */
    class ValueTypeInstaller_impl :
        public CdmwCcmContainer::ValueTypeInstaller,
        public Cdmw::OrbSupport::RefCountLocalObject
    {
    public:
        /**
        * Purpose:
        * <p> Constructor.
        *
        *@param orb A reference to the ORB pseudo-object
        *
        *@exception CORBA::SystemException TODO:
        */
        ValueTypeInstaller_impl(CORBA::ORB_ptr orb)
            throw (CORBA::SystemException);
        
        /**
        * Purpose:
        * <p> Destructor
        *
        */
        ~ValueTypeInstaller_impl()
            throw ();
        
        //
        // IDL:thalesgroup.com/CdmwCcmContainer/ValueTypeInstaller/install_valuetype_factory:1.0
        //
        virtual
        CdmwCcmContainer::ValueTypeRegistration_ptr
        install_valuetype_factory(const char* repid,
                                  const char* location,
                                  const char* entrypoint)
            throw (CORBA::SystemException);

        bool unregister_valuetype_factory(const std::string & repid);
        
       
    private:

        struct RegistrationInfo
        {
            RegistrationInfo(CdmwCcmContainer::ValueTypeRegistration_ptr registration,
                 Cdmw::OsSupport::OS::SharedObjectHandle*    shlib ,
                             bool                                         release)
                throw()
                : m_registration(registration),
                  m_shlib(shlib),
                  m_release(release)
            {
            }
            
      /**
       * NOTA: Notice the use of a _ptr instead of a _var !
       */   
      CdmwCcmContainer::ValueTypeRegistration_ptr m_registration;
      Cdmw::OsSupport::OS::SharedObjectHandle*    m_shlib;
      bool                                         m_release;
        };
        
        typedef  std::map<std::string,RegistrationInfo> RegistrationMap;
        /**
         *[Attribute description]
         */ 
        CORBA::ORB_var           m_orb;
        Cdmw::OsSupport::Mutex  m_mutex;
        RegistrationMap          m_registrations;
        
       

        
        ValueTypeInstaller_impl();
        ValueTypeInstaller_impl(const ValueTypeInstaller_impl&);
    };
}; // End namespace Container
}; // End namespace CCM
}; // End namespace Cdmw

#endif // INCL_CDMW_CCM_CONTAINER_VALUE_TYPE_INSTALLER_IMPL_HPP

