/* ===================================================================== */
/*
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
*/
/* ===================================================================== */


#include <string>
#include "Foundation/ossupport/OS.hpp"
#include "Foundation/common/String.hpp"
#include "Foundation/common/Assert.hpp"
#include "Foundation/orbsupport/CORBA.hpp"
#include "Foundation/orbsupport/OrbSupport.hpp"

#include "XMLServer/xmllibrary/XMLHelper.hpp"
#include "XMLServer/idllib/CdmwXML.stub.hpp"
#include "testxmllibraryclient/TestXMLLibrary.hpp"

namespace {
	const int POA_THREAD_POOL_SIZE = 5;

    const int SUCCESS = 0;
    const int FAILURE = 1;
    
    #if (CDMW_ORB_VDR == orbacus)
        const std::string PREFIX = "relfile:";
    #elif (CDMW_ORB_VDR == tao)
        const std::string PREFIX = "file://";
    #endif
    const std::string XML_SERVER_DIR = "./";
    const std::string XML_SERVER_STANDALONE_DIR = "./";
    const std::string XML_SERVER_IOR_FILE = "xml_server.ior";

    const std::string STANDALONE_OPTION      = "--Standalone";
};


int
run(CORBA::ORB_ptr orb, int argc, char* argv[])
{
	using namespace Cdmw::XMLUtils;
	using namespace Cdmw::Common;
        using namespace Cdmw::OsSupport;

	// Obtain the server's object reference from a file
        std::string standalone = OS::get_option_value (argc, argv, STANDALONE_OPTION);
	std::string objStr;
	if (standalone == "no")
	{
          objStr = PREFIX + XML_SERVER_DIR + XML_SERVER_IOR_FILE;
        }
        else
	{
          objStr = PREFIX + XML_SERVER_STANDALONE_DIR + XML_SERVER_IOR_FILE;
        }
        CORBA::Object_var obj = orb -> string_to_object( objStr.c_str() );
	if(CORBA::is_nil(obj.in()))
	{      
		std::cerr << argv[0] << ": cannot read IOR" << std::endl;
		return FAILURE;
	}

	CdmwXML::XMLParser_var parser = CdmwXML::XMLParser::_narrow(obj.in());
    CDMW_ASSERT(!CORBA::is_nil(parser.in()));
    
    // Testing TestXMLLibrary
    TestXMLLibrary xmlLibraryTest(parser.in());
    xmlLibraryTest.start();
    
    return SUCCESS;
}

int main(int argc, char* argv[])
{
	int status = SUCCESS;
    CORBA::ORB_var orb;

    try
    {
    	Cdmw::OrbSupport::StrategyList orb_strategies;
		orb_strategies.add_OrbThreaded();
        orb_strategies.add_PoaThreadPool(POA_THREAD_POOL_SIZE);
    
		orb = Cdmw::OrbSupport::OrbSupport::ORB_init(argc, argv, orb_strategies);
        Cdmw::XMLUtils::XMLHelper::register_value_factories(orb.in());
        
		status = run(orb.in(), argc, argv);
    }
    catch(const CORBA::Exception& ex)
    {
		std::cerr << ex << std::endl;
		status = FAILURE;
    }

    if(!CORBA::is_nil(orb.in()))
    {
		try
		{
		  Cdmw::OrbSupport::OrbSupport::ORB_cleanup(orb.in());
		  orb->destroy();
		}
		catch(const CORBA::Exception& ex)
		{
			std::cerr << ex << std::endl;
			status = FAILURE;
		}
    }
    
    return status;   
}


