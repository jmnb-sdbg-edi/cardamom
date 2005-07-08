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


#include <fstream>
#include <string>

#include <Foundation/common/Options.hpp>
#include <Foundation/ossupport/OS.hpp>
#include <Foundation/orbsupport/CORBA.hpp>
#include <Foundation/orbsupport/OrbSupport.hpp>
#include <Foundation/orbsupport/StrategyList.hpp>
#include <LoadBalancing/idllib/CdmwLBGroupManager.stub.hpp>
#include <LoadBalancing/lbinit/LBServiceInit.hpp>
#include <LoadBalancing/idllib/PortableGroup.stub.hpp>


namespace
{
    const int SUCCESS = 0;
    const int FAILURE = 1;
} // end of anonymous namespace


/**
 * Displays the usage.
 *
 * @param os the output stream
 * @param app_name the application name
 */
void
usage(std::ostream & os, const std::string &app_name)
{
    os << "usage:\n"
       << app_name << " OPTIONS\n"
       << "\n"
       << "options:\n"
       << "--help         display the usage\n"
       << "--iogr=FILE    IOGR file to update\n"
       << std:: endl;
}


/**
 * This application gets a reference to a LB object group from FILE,
 * requests the LB group manager a newer version of it and saves it
 * back into the file. The LB group manager may return the same
 * reference.
 */
int main(int argc, char* argv[])
{
    try {
        // parse the command-line arguments

        // --help: displays the usage
        if (strcmp(argv[1], "--help") == 0) {
            usage(std::cout, argv[0]);
            return SUCCESS;
        }

        // --iogr: IOGR file to update
        std::string iogrfile =
            Cdmw::OsSupport::OS::get_option_value(argc, argv, "iogr");
        if ((iogrfile == "no") || (iogrfile == "yes")) {
            std::cerr << "--iogr is a mandatory option" << std::endl;
            usage(std::cerr, argv[0]);
            return FAILURE;
        }

        // init LB service
        // Cdmw::LB::LBServiceInit::Init(argc, argv);

        // init the ORB
        Cdmw::OrbSupport::StrategyList strategyList;
        strategyList.add_OrbThreaded();
        strategyList.add_PoaThreadPerConnection();

        CORBA::ORB_var orb =
            Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, strategyList);

        // get the LB Group Manager
        CORBA::Object_var lbgroup_mgr_obj
            = orb->resolve_initial_references("LBGroupManager");

        CdmwLB::LBGroupManager_var lbgroup_mgr;
        lbgroup_mgr = CdmwLB::LBGroupManager::_narrow(lbgroup_mgr_obj.in());

        // read the IOGR from FILE
        std::string s_iogr;
        std::ifstream infile(iogrfile.c_str());
        infile >> s_iogr;
        infile.close();
        CORBA::Object_var iogr;
        iogr = orb->string_to_object(s_iogr.c_str());

        // get the new version of the reference
        ::PortableGroup::ObjectGroup_var new_iogr;
        new_iogr = lbgroup_mgr->get_object_group_ref(iogr.in());

	CORBA::String_var s_new_iogr = orb->object_to_string(new_iogr.in());

        // save the IOGR to FILE
        std::ofstream outfile(iogrfile.c_str());
        outfile << s_new_iogr.in();
        outfile.close();
    } catch(const CORBA::SystemException& e) {
        std::cout<<"Unexpected exception :" << e << std::endl;
    } catch (...) {
      std::cout << " ANOTHER TRAN BUG !!! " << std::endl;
    }
}
