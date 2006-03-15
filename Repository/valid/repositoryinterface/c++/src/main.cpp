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
#include <string>

#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/common/Options.hpp>

#include "repositoryinterface/RepositoryInterfaceTest.hpp"


const int SUCCESS = 0;
const int FAILURE = 1;

const char* NAMING_REPOS_URL_OPTION = "--namingRepos-url";


void usage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "RepositoryInterfaceTest options" << std::endl;
    std::cout << "Options:\n" << std::endl;
    std::cout << "--help                     Display this information." 
              << std::endl;
    std::cout << NAMING_REPOS_URL_OPTION 
              << "=<URL>  URL of the CDMW Naming and Repository service.\n" 
              << std::endl;
}



int main(int argc, char* argv[]) {
    
    //
    // help argument
    //
    if((argc > 0) && (strcmp(argv[0], "--help") == 0))   {    
        usage();
        return SUCCESS;
    }

    //
    // get Naming & Repository URL from arguments
    //
    std::string namingReposURL = 
        Cdmw::OsSupport::OS::get_option_value(argc, argv, NAMING_REPOS_URL_OPTION);

    if (namingReposURL == "no") 
    {
        std::cout << "naming and repository url undefined" << std::endl;
        usage();
        return FAILURE;
    }

    Cdmw::NamingAndRepository::RepositoryInterfaceTest test(argc, argv);
    try {
        test.init(namingReposURL.c_str());
        test.start();
    } catch (const CORBA::SystemException& e) {
        std::cerr << "Exception while retrieving Repository reference: " << e << std::endl;
        test.destroyORB();
        return FAILURE;
    }

    test.destroyORB();
    return SUCCESS;

}



