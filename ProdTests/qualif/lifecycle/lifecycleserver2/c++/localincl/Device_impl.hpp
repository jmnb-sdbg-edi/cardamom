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

#ifndef INCL_RESOURCES_DEVICE_IMPL_HPP 
#define INCL_RESOURCES_DEVICE_IMPL_HPP 
#include "Foundation/orbsupport/CORBA.hpp"
#include "LifeCycle/lifecycle/ObjectDeactivator.hpp"
#include "lifecycleserver2/CF.skel.hpp"
#include "Foundation/orbsupport/RefCountServantBase.hpp"


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
class Device_impl :
        public virtual POA_CF::Device,
        public virtual Cdmw::OrbSupport::RefCountServantBase
{

    public:

        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        Device_impl(PortableServer::POA_ptr                     poa,
                    const Cdmw::LifeCycle::ObjectDeactivator & deactivator,
                    CF::Device::UsageType                       state,
                    const std::string &                         location)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~Device_impl()
                throw();

        virtual PortableServer::POA_ptr _default_POA() {
            return PortableServer::POA::_duplicate(m_poa.in());
        }
    
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        //
        // IDL:thalesgroup.com/CF/Device/start:1.0
        //
        // Transition from IDLE to ACTIVE
        virtual void start()
            throw(CF::Device::StartError,
                CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CF/Device/stop:1.0
        //
        // Transition from ACTIVE to IDLE deactivates current object
        virtual void stop()
            throw(CF::Device::StopError,
          CORBA::SystemException);
  
        //
        // IDL:thalesgroup.com/CF/Device/state:1.0
        //
        virtual CF::Device::UsageType state()
            throw(CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CF/Device/label:1.0
        //
        virtual CF::Device::Location label()
            throw(CORBA::SystemException);
        /**
        *[Attribute description]
        */ 


    private:
        /**
        * Purpose:
        * <p> [Constructor description if necessary]
        * 
        *@param
        *@exception
        */ 
        Device_impl()
                throw(CORBA::SystemException);

        // Hide copy constructor/assignment operator
        Device_impl(const Device_impl& rhs)
                throw();
        
        Device_impl&
        operator=(const Device_impl& rhs)
                throw();
        /**
        *[Attribute description]
        */         
        PortableServer::POA_var                       m_poa;
        const Cdmw::LifeCycle::ObjectDeactivator &   m_object_deactivator;
        CF::Device::UsageType                         m_state;
        std::string                                   m_location;
}; // End class Device_impl 

}; // End namespace Resources
}; // End namespace Cdmw
#endif // INCL_RESOURCES_DEVICE_IMPL_HPP 

