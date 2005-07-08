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


#ifndef INCL_DEMO_FT_HELLO_ACTIVATION_HANDLER_HPP 
#define INCL_DEMO_FT_HELLO_ACTIVATION_HANDLER_HPP 

#include <string>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/common/Assert.hpp>
#include <FaultTolerance/idllib/CdmwFTActivation.skel.hpp>

#include "HelloInterface_impl.hpp"
#include "RecoveryMechanism.hpp"


// Activation Handler to be called by Cdmw FT when Hello Object becomes primary. 

class HelloActivationHandler: 
    virtual public CdmwFT::Location::ActivationHandler,
    virtual public Cdmw::OrbSupport::RefCountLocalObject
{

public:

    // Constructor
    HelloActivationHandler(const std::string& name,
                           HelloInterface_impl* hello_impl,
                           RecoveryMechanism* recovery_mechanism):
        m_name (name),
        m_hello_impl (hello_impl),
        m_recovery_mechanism (recovery_mechanism),
        m_is_active (false)
    { 
        CDMW_ASSERT(m_hello_impl != NULL);
        m_hello_impl->_add_ref();
        CDMW_ASSERT(m_recovery_mechanism != NULL);
        m_recovery_mechanism->_add_ref();
    };
    
    // Descrutor
    ~HelloActivationHandler() 
    {
        m_hello_impl->_remove_ref();
        m_recovery_mechanism->_remove_ref();
    };
    
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/activate()
    virtual void activate()
        throw (CORBA::SystemException);
    
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/deactivate()
    virtual void deactivate()
        throw (CORBA::SystemException);

    // returns true if it has been activated
    virtual bool is_active() {
        return m_is_active;
    };
    
    // returns the Hello servant
    virtual HelloInterface_impl* getHelloServant() {
        return m_hello_impl;
    };
    
        
private:

    // The application name.
    std::string m_name;

    // The Hello servant
    HelloInterface_impl* m_hello_impl;

    // the Recovery Mechanism
    RecoveryMechanism* m_recovery_mechanism;
    
    // active flag
    bool m_is_active;
};


#endif // INCL_DEMO_FT_HELLO_ACTIVATION_HANDLER_HPP


