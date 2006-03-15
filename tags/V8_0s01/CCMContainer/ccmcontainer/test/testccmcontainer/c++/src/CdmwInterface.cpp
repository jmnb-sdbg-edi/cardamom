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


#include "Foundation/orbsupport/CORBA.hpp"
#include "ConfAndPlug/cdmwinit/CdmwInterface.hpp"

#include "CCMContainer/ccmcontainer/InitUtils.hpp"
#include "CCMContainer/ccmcontainer/HomeAllocatorRegistry.hpp"
#include "testccmcontainer/CdmwHelloHome_impl.hpp"



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
    Cdmw::CCM::Container::InitUtils::register_valuetype_factories(orb);
    // Register HomeAllocator
    typedef HomeAllocator<TestCdmwCCM::CdmwHelloHome_impl> CdmwHelloHome_Allocator;
    HomeAllocatorRegistry::Register("TestCdmwCCM.CdmwHelloHome_impl",
                                    &CdmwHelloHome_Allocator::TheAllocator);
}

// CDMW Factory objects will be deactivated at ORB shutdown.
void
CDMW_cleanup(CORBA::ORB_ptr orb)
    throw ()
{
    using namespace Cdmw::CCM::Container;
    // UnRegister HomeAllocator
    HomeAllocatorRegistry::Unregister("TestCdmwCCM.CdmwHelloHome_impl");
}


}; // end of namespace
}; // end of namespace



