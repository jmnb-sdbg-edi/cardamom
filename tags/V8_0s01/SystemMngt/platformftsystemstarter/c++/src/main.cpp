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
#include <sstream>

#include "orbsupport/CORBA.hpp"
#include "orbsupport/OrbSupport.hpp"
#include "orbsupport/StrategyList.hpp"
#include "ossupport/OS.hpp"
#include "common/Options.hpp"
#include "common/String.hpp"

#include "Foundation/ossupport/OS.hpp"

#include "platformftsystemstarter/StarterConfigurationMngr.hpp"

#include "SystemMngt/idllib/CdmwPlatformMngtDaemon.stub.hpp"

void print_syntax(char* exe)
{
    std::cout << "Syntax is :" << std::endl << exe;
    std::cout << " | " << Cdmw::Common::Options::PROCESS_XML_FILE_OPTION << "=XMLConfigFile";
    std::cout << " [" << Cdmw::Common::Options::VALIDATE_XML_OPTION << "] ]";
    std::cout << std::endl;
}


int main(int argc, char* argv[])
{
    using namespace Cdmw::PlatformMngt;
    using namespace Cdmw::OsSupport;
    using namespace Cdmw::OrbSupport;
    using namespace Cdmw::Common;
    
    // the return code 
    int ret_code = 0;

    // checks the arguments
    std::string xml_conf;
    bool validate = false;
   
    if (argc > 1)
    {
        // checks the xml configuration file option
        std::string xml_conf_option =
            OS::get_option_value( argc, argv, Options::PROCESS_XML_FILE_OPTION);

        if (xml_conf_option != "no")
        {
            if (xml_conf_option != "yes")
            {
                if (OS::file_exists(xml_conf_option))
                {
                    xml_conf = xml_conf_option;  		

                    // checks the optional validate option
                    std::string validateXML_option =
                        OS::get_option_value (argc, argv, Cdmw::Common::Options::VALIDATE_XML_OPTION);

                    if (validateXML_option != "no")
                    {
                        validate = true;
                    }
                }
                else
                {
                    std::cerr << "Platform system starter: XML configuration file not found" << std::endl;
                    ::exit(EXIT_FAILURE);
                }                
            }
            else
            {
                std::cerr << "Platform system starter: XML configuration file not specified" << std::endl;
                ::exit(EXIT_FAILURE);
            }
        }
    }


    // The default configuration
    StarterConfigurationMngr conf;

    CORBA::ORB_var orb;

    try
    {
        // If an XML configuration file has been transmit, we use it
        if (!xml_conf.empty())
        {
            conf.parse(xml_conf, validate);
	}

	// If process name undefined
        if (!strcmp(conf.getProcessName(), ""))
        {
            std::cerr << "Platform system starter: process name missing in configuration file" << std::endl;
            ::exit(EXIT_FAILURE);
	}
	    
	// If process exe undefined
	if (!strcmp(conf.getProcessExecPath(), ""))
        {
            std::cerr << "Platform system starter: process exec path missing in configuration file" << std::endl;
            ::exit(EXIT_FAILURE);
	}

        // initialises the ORB
        orb = CORBA::ORB_init (argc, argv);

        // get daemon reference
        std::ostringstream daemonCorbaloc;

        std::string hostName = conf.getDaemonHostString();
        daemonCorbaloc 
            << "corbaloc::" << hostName
            <<  ":" << conf.getDaemonPortString()
            <<  "/" << CdmwPlatformMngt::DAEMON_NAME;

        CORBA::Object_var obj =
            orb->string_to_object (daemonCorbaloc.str().c_str());
            
        if (CORBA::is_nil (obj.in()))
        {
            throw CdmwPlatformMngt::HostNotFound();
        }
        
        
        CdmwPlatformMngt::Daemon_var daemon =
            CdmwPlatformMngt::Daemon::_narrow (obj.in());
            
        if (CORBA::is_nil (daemon.in()))
        {
            throw CdmwPlatformMngt::HostNotFound();
        }
        
        
        // Build the process arguments from xml file and starter arguments 
        std::string pgm_args = conf.getProcessArgs();
        
        if (argc > 1)
        {
            int iArg = 1;
	        
            // We scan all starter arguments to find the process options
            while (iArg < argc) 
            {    
                std::string arg = argv[iArg];
	    
	            // We check if in the current argument we do not have the starter option
                if (arg.find(Options::PROCESS_XML_FILE_OPTION) == std::string::npos &&
                    arg.find(Options::VALIDATE_XML_OPTION) == std::string::npos)
                {
                    // the option is for the process to start
                
                    pgm_args += " ";
                    pgm_args += arg;                
	            }
	    
	            iArg++;
            }
        }
        
        std::cout << std::endl;
        std::cout << "start process : " << std::endl;
        std::cout << "      name      :" << conf.getProcessName() << std::endl;
        std::cout << "      exec      :" << conf.getProcessExecPath() << std::endl;
        std::cout << "      port      :" << conf.getProcessPortString() << std::endl;
        std::cout << "      arguments :" << pgm_args.c_str() << std::endl;
        
        // build the ProcessInfo data for the process to start
        CdmwPlatformMngt::ProcessInfo process_info;
        
        process_info.process_exe = 
           CORBA::string_dup (conf.getProcessExecPath());
        process_info.process_port = ::atoi (conf.getProcessPortString());
        process_info.working_directory = 
           CORBA::string_dup (conf.getProcessWorkDir());
        process_info.process_args = CORBA::string_dup (pgm_args.c_str());
        process_info.exit_autorestart_info.default_exit_action = 
           CdmwPlatformMngt::NO_ACTION;
        process_info.exit_autorestart_info.exit_actions.length(0);
        process_info.exit_autorestart_info.max_nbr_of_restart_attempt = 0;
        process_info.configuration_data.run_as_root = false;
        process_info.configuration_data.generate_core = true;
        
        // request the daemon to create the process
        daemon->create_process (conf.getProcessName(), process_info);
    }
    catch (const CORBA::COMM_FAILURE& ex)
    {
        std::cerr << "Platform system starter: CORBA::COMM_FAILURE raised: "
                  << ex
                  << std::endl;
        ret_code = EXIT_FAILURE;
	}
    catch (const StarterConfigurationException& ex)
    {
        std::cerr << "Platform system starter: ConfigurationException raised: "
                  << ex
                  << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch (const CdmwPlatformMngt::ProcessAlreadyExists& ex)
    {
        std::cerr << "Platform system starter: ProcessAlreadyExists raised: "
                  << ex
                  << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch (const CdmwPlatformMngt::InvalidInfo& ex)
    {
        std::cerr << "Platform system starter: InvalidInfo raised: "
                  << ex
                  << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch (const CORBA::SystemException& ex)
    {
        std::cerr << "Platform system starter: CORBA SystemException raised: "
                  << ex
                  << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch (const CORBA::Exception& ex)
    {
        std::cerr << "Platform system starter: CORBA Exception raised: "
                  << ex
                  << std::endl;
        ret_code = EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "Platform system starter: Unexpected Exception raised"
                  << std::endl;
        ret_code = EXIT_FAILURE;
    }
    
    if (!CORBA::is_nil(orb.in()))
    {
        // stops the ORB
        orb->destroy();
    }

    return ret_code;
}
