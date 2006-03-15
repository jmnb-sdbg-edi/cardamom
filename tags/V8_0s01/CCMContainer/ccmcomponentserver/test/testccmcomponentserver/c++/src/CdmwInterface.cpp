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


#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"
#include "CCMContainer/ccmcontainer/ConfigValue_impl.hpp"
#include "CCMContainer/ccmcontainer/HomeAllocatorRegistry.hpp"

#include "Foundation/orbsupport/CORBA.hpp"


//----------------------------------------------------------------------------

namespace Cdmw 
{ 
namespace CdmwInit 
{ 



void 
CDMW_init(CORBA::ORB_ptr orb, int & argc, char** argv, ProcessControl* ctrl) 
    throw (CORBA::SystemException)
{
    using namespace Cdmw::CCM::Container;
    // Register ConfigValue Valuetype  factory
#if CDMW_ORB_VDR == tao && CDMW_ORB_VER < 14
    CORBA_ValueFactoryBase_var valfac =
#else
    CORBA::ValueFactoryBase_var valfac =
#endif
        orb->register_value_factory("IDL:omg.org/Components/ConfigValue:1.0",
                                    new ConfigValueFactory());

}

// CDMW Factory objects will be deactivated at ORB shutdown.
void
CDMW_cleanup(CORBA::ORB_ptr orb)
    throw ()
{
}


}; // end of namespace
}; // end of namespace



