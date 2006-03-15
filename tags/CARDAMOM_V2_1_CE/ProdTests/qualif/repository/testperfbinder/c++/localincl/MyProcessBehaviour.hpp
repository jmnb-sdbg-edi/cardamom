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

#ifndef INCL_MY_PROCESS_BEHAVIOUR_HPP 
#define INCL_MY_PROCESS_BEHAVIOUR_HPP 

#include "SystemMngt/platforminterface/PlatformInterface.hpp"
#include <ConfAndPlug/cdmwinit/ProcessControl.hpp>
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/orbsupport/CORBA.hpp"

#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include <Foundation/commonsvcs/naming/NamingInterface.hpp>
#include "Repository/repositoryinterface/RepositoryInterface.hpp"
#include "Repository/idllib/CdmwNamingAndRepository.stub.hpp"
#include "testperfbinder/TestHello_impl.hpp"
#include "testperfbinder/TestRepositoryBinder.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
/**
* Purpose:
* <p>
* The abstract process behaviour.
*/
//class MyProcessBehaviour : public Cdmw::PlatformMngt::ProcessBehaviour
class MyProcessBehaviour : public Cdmw::CdmwInit::ProcessControl
{

public:

    /**
    * Purpose:
    * <p> The constructor.
    */ 
    MyProcessBehaviour  (CORBA::ORB_ptr orb, PortableServer::POA_ptr rootPOA, std::string bind_number)
        : m_bind_number(bind_number)
    {
        m_orb = CORBA::ORB::_duplicate(orb);
        m_rootPOA =  PortableServer::POA::_duplicate(rootPOA);
    }


    /**
    * Purpose:
    * <p> The destructor.
    */ 
    virtual ~MyProcessBehaviour() throw ()
    {
     
    }

    
    /**
    * Purpose:
    * <p>
    * the behaviour for the
    * IDL:thalesgroup.com/CdmwPlatformMngt/Process/nb_steps:1.0
    * attribute
    */
    virtual CORBA::ULong nb_steps() throw(CORBA::SystemException)
    {
        return 1;
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/get_service:1.0
	* operation
	*/
    virtual CORBA::Object_ptr get_service() throw(CORBA::SystemException)
    {
        return CORBA::Object::_nil();
    }
    
    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/initialise:1.0
	* operation
	*/
    virtual void on_initialise(const CdmwPlatformMngtBase::StartupKind& startup_kind)
        throw( CORBA::SystemException)
    {

    }



    
    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/run:1.0
	* operation
	*/
    virtual void on_run()
    throw( CORBA::SystemException,
           CdmwPlatformMngt::ProcessDelegate::NotReadyToRun )
    {    

        // Do test
        m_test = new Cdmw::TestRepositoryBinder (m_orb.in(), m_rootPOA.in(), m_bind_number);
        m_test->start();

    }

    /**
	* Purpose:
	* <p>
	* the behaviour for the
	* IDL:thalesgroup.com/CdmwPlatformMngt/Process/stop:1.0
	* operation
	*/
    virtual void on_stop() throw(CORBA::SystemException)
    {
    }


    
private:

    /**
    * The ORB.
    */
    CORBA::ORB_var m_orb;

    /**
     * The poa
     */
    PortableServer::POA_var m_rootPOA;

    /**
     * bind_number
     */
    std::string m_bind_number;
    
    /**
     * test
     */
    Cdmw::TestRepositoryBinder* m_test;
    
};


#endif // INCL_MY_PROCESS_BEHAVIOUR_HPP

