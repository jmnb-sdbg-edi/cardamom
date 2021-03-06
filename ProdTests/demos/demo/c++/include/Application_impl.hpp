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


#ifndef INCL_ENTITY_APPLICATION_IMPL_HPP 
#define INCL_ENTITY_APPLICATION_IMPL_HPP 
#include "Foundation/orbsupport/CORBA.hpp"
#include "LifeCycle/lifecycle/LifeCycleSingleObject.hpp"
#include "CF.skel.hpp"
#include "Foundation/ossupport/OS.hpp"
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
class Application_impl :
        public virtual POA_CF::Application,
        public Cdmw::LifeCycle::LifeCycleSingleObject,
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
        Application_impl(const Cdmw::LifeCycle::ObjectDeactivator & obj_d,
                         const std::string &                         application_name)
            throw(CORBA::SystemException);

        /**
        * Purpose:
        * <p> [ Destructor description if necessary]
        * 
        */ 
        virtual 
        ~Application_impl()
                throw();
    
        /**
        * Purpose:
        * <p>
        * 
        *@param
        *@return
        *@exception
        */ 
        //
        // IDL:thalesgroup.com/CF/Application/start:1.0
        //
        // starts process "m_name"
        virtual void start()
            throw(CF::Application::StartError,
                CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CF/Application/stop:1.0
        //
        // kills already started process and deactivates current object
        virtual void stop()
            throw(CF::Application::StopError,
          CORBA::SystemException);

        //
        // IDL:thalesgroup.com/CF/Application/name:1.0
        //
        virtual char* name()
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
        Application_impl()
                throw(CORBA::SystemException);

        // Hide copy constructor/assignment operator
        Application_impl(const Application_impl& rhs)
                throw();
        
        Application_impl&
        operator=(const Application_impl& rhs)
                throw();
        /**
        *[Attribute description]
        */
        std::string                     m_name;
        bool                            m_started;
        Cdmw::OsSupport::OS::ProcessId m_pid;
}; // End class Application_impl 

}; // End namespace Resources
}; // End namespace Cdmw
#endif // INCL_ENTITY_APPLICATION_IMPL_HPP 

