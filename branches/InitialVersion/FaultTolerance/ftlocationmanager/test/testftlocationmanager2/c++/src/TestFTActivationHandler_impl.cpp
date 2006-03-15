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

#include <iostream>

#include "testftlocationmanager2/TestFTActivationHandler_impl.hpp"

namespace Cdmw {
namespace FT {

    TestFTActivationHandler_impl::TestFTActivationHandler_impl()
        : m_activation (false)
        {}
    TestFTActivationHandler_impl::~TestFTActivationHandler_impl()
        {}
    
    //
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/activate:1.0
    //
    void TestFTActivationHandler_impl::activate()
        throw(CORBA::SystemException)
    {
        std::cout<<"TestFTActivationHandler_impl::activate()"<<std::endl;
        m_activation = true;
        
    }
    

    //
    // IDL:thalesgroup.com/CdmwFT/Location/ActivationHandler/deactivate:1.0
    //
    void TestFTActivationHandler_impl::deactivate()
        throw(CORBA::SystemException)
    {
        std::cout<<"TestFTActivationHandler_impl::deactivate()"<<std::endl;
        m_activation = false;
        
    }


} // End of namespace FT
} // End of namespace Cdmw

