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


#include "test3/NamingContextReaderCleaner.hpp"

#include <exception>
#include <iostream>


void exceptionHandler(void)
{
	std::cout << "Unexpected Exception : Test failed" << std::endl;
}



int main(int argc, char* argv[])
{

    std::set_unexpected(&exceptionHandler);

    // Initialize the ORB
    CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);

    // Get the root POA
    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var rootPOA = PortableServer::POA::_narrow(obj.in());

    PortableServer::POAManager_var POAMgr = rootPOA->the_POAManager();
    POAMgr->activate();

    NamingContextReaderCleaner test("Test2 - ReaderCleaner", rootPOA.in(), orb.in());

    test.start();

    orb->destroy();

	return 0;
}


