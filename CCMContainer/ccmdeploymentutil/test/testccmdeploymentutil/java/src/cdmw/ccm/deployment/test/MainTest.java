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


package cdmw.ccm.deployment.test;

import cdmw.testutils.Testable;
import cdmw.orbsupport.StrategyList;
import cdmw.xmlutils.XMLHelper;

import com.thalesgroup.CdmwXML.XMLParser;
import com.thalesgroup.CdmwXML.XMLParserHelper;

/**
 * This class only calls the other tests for ccmdeploymentutil library:
 *  - Requirement
 *  - RequirementList
 *  - CsdReader
 *  - CcdReader
 *  - CadReader
 *  - CpfReader
 *  - FileUtilities
 */
public class MainTest {

    public static final int POA_THREAD_POOL_SIZE = 10;

    public static final String OB_CONFIG_FILE = "./obtest.conf";

    public static void main(String[] args) {
        String ftpDirPath = null;
        // get path of FTP dir if specified
        if (args.length == 1)
            ftpDirPath = args[0];

        org.omg.CORBA.ORB orb = null;
        try {
            StrategyList orbStrategies = new StrategyList();
            orbStrategies.addORBThreaded();
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);

            //String[] serverArgs = { "-ORBconfig", OB_CONFIG_FILE };
            String[] serverArgs = {
                "-ORBInitRef", "XMLService=corbaloc::localhost:5060/XMLService",
                "-ORBInitRef", "InterfaceRepository=corbaloc:iiop:localhost:21000/DefaultRepository"
            };
            orb = cdmw.orbsupport.ORBSupport.ORBInit(serverArgs, orbStrategies);

            XMLHelper.registerValueFactories(orb);
            
            XMLParser parser = XMLParserHelper.narrow(
                orb.resolve_initial_references("XMLService"));

            org.omg.PortableServer.POA rootPOA =
                org.omg.PortableServer.POAHelper.narrow(
                    orb.resolve_initial_references("RootPOA"));
            rootPOA.the_POAManager().activate();

            RequirementTest reqTest = new RequirementTest();
            reqTest.start();
            RequirementListTest reqListTest = new RequirementListTest();
            reqListTest.start();

            CsdReaderTest csdTest = new CsdReaderTest(parser);
            csdTest.start();
            CcdReaderTest ccdTest = new CcdReaderTest(parser);
            ccdTest.start();
            CadReaderTest cadTest = new CadReaderTest(parser);
            cadTest.start();
            CpfReaderTest cpfTest = new CpfReaderTest(parser, orb);
            cpfTest.start();

            ConfigValuesTest configValuesTest = new ConfigValuesTest(orb);
            configValuesTest.start();
            
            FileUtilitiesTest fileUtilTest = new FileUtilitiesTest(ftpDirPath);
            fileUtilTest.start();

        } catch(Exception e) {
            e.printStackTrace();
        }

        // destroy ORB
        if (orb != null) {
            orb.shutdown(false);
            orb.destroy();
        }

    }


}



