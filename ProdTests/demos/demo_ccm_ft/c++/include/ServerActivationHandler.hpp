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


#ifndef INCL_DEMO_FT_SERVER_ACTIVATION_HANDLER_HPP 
#define INCL_DEMO_FT_SERVER_ACTIVATION_HANDLER_HPP 

#include <string>
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/RefCountLocalObject.hpp"
#include "Foundation/common/Assert.hpp"
#include "FaultTolerance/idllib/CdmwFTActivation.skel.hpp"

// Activation Handler to be called by Cdmw FT when Hello Object becomes primary. 

class ServerActivationHandler: 
    virtual public CdmwFT::Location::ActivationHandler,
    virtual public Cdmw::OrbSupport::RefCountLocalObject
{

public:

    // Constructor
    ServerActivationHandler()
    { 
    };
    
    // Descrutor
    ~ServerActivationHandler() 
    {
    };
    
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/activate()
    virtual void activate()
        throw (CORBA::SystemException)
	{
	}
    
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/deactivate()
    virtual void deactivate()
        throw (CORBA::SystemException)
	{
	}
    
        
private:
};


#endif // INCL_DEMO_FT_SERVER_ACTIVATION_HANDLER_HPP


