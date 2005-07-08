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


#ifndef INCL_DATABASE_ACTIVATION_HANDLER_HPP 
#define INCL_DATABASE_ACTIVATION_HANDLER_HPP 

// Cdmw Files
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/RefCountLocalObject.hpp>
#include <Foundation/common/Assert.hpp>
#include <FaultTolerance/idllib/CdmwFTActivation.skel.hpp>

// Demo Files
#include "Database_impl.hpp"


// Activation Handler to be called by Cdmw FT when Database Object becomes primary. 

class DatabaseActivationHandler: 
    virtual public CdmwFT::Location::ActivationHandler,
    virtual public Cdmw::OrbSupport::RefCountLocalObject
{

public:

    // Constructor
    DatabaseActivationHandler(const std::string& name,
                              Database_impl* database_impl):
        m_name (name),
        m_database_impl (database_impl),
        m_is_active (false)
    { 
        CDMW_ASSERT(m_database_impl != NULL);
        m_database_impl->_add_ref();
    };
    
    // Descrutor
    ~DatabaseActivationHandler() 
    {
        m_database_impl->_remove_ref();
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
    
    // returns the Database servant
    virtual Database_impl* getDatabaseServant() {
        return m_database_impl;
    };
    
        
private:

    // The application name.
    std::string m_name;

    // The Database servant
    Database_impl* m_database_impl;

    // active flag
    bool m_is_active;
};


#endif // INCL_DATABASE_ACTIVATION_HANDLER_HPP


