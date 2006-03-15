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


package cdmw.cdmwinit.test;

import cdmw.testutils.Testable;
import cdmw.common.BadParameterException;

import cdmw.cdmwinit.CdmwInitConfiguration;

public class InitConfigurationTest extends Testable {

    public InitConfigurationTest() {
        this.out = System.out;
        this.name = "Init Configuration Test";
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(13);

        try {

            CdmwInitConfiguration init =
                CdmwInitConfiguration.readConfiguration("Process_Gen.xml",true);

            println("- getServiceAttributeValue -");

            String res = init.getServiceAttributeValue("performance", "result-file-name");
            println("performance result-file-name = " + res );
            succeed();

            try {
                println("- testing getServiceAttributeValue -");
                res = init.getServiceAttributeValue("xxxx", "result-file-name");
                fail();
            }
            catch(BadParameterException bpe) {
                println(bpe.what());
                succeed();
            }

            try {
                println("- testing getServiceAttributeValue -");
                res = init.getServiceAttributeValue("performance", "xxx");
                fail();
            }
            catch(BadParameterException bpe) {
                println(bpe.what());
                succeed();
            }


            println("- getFrameworkAttributeValue -");

            res = init.getFrameworkAttributeValue("first.framework", 0, "domain-name");
            println("domain-name = " + res);
            succeed();

            res = init.getFrameworkAttributeValue("first.framework", 0, "full-factory-name");
            println("full-factory-name = " + res);
            succeed();

            try {
                println("- testing getFrameworkAttributeValue -");
                res = init.getFrameworkAttributeValue("xxxx", 0, "domain-name");
                fail();
            } catch(BadParameterException bpe) {
                println(bpe.what());
                succeed();
            }

            try {
                println("- testing getFrameworkAttributeValue -");
                res = init.getFrameworkAttributeValue("first.framework", 0, "xxxxx");
                fail();
            } catch(BadParameterException bpe) {
                println(bpe.what());
                succeed();
            }

            try {
                println("- testing getFrameworkAttributeValue -");
                res = init.getFrameworkAttributeValue("first.framework", 3, "domain-name");
                fail();
            } catch(BadParameterException bpe) {
                println(bpe.what());
                succeed();
            }


            println("- getFrameworkMultithreadAttribute -");

            String[] results = init.getFrameworkMultithreadAttribute("first.framework", 0);
            String threadingPolicy = results[0];
            String threadingAttribute = results[1];
            String threadingAttributeValue = results[2];
            if (threadingPolicy == "serialize" && threadingAttribute.length()==0 && threadingAttributeValue.length()==0) {
                println("EntityResourceFactoryImpl 1 : ");
                println(threadingPolicy);
                succeed();
            }

            println("- getFrameworkMultithreadAttribute -");
            results = init.getFrameworkMultithreadAttribute("first.framework", 1);
            threadingPolicy = results[0];
            threadingAttribute = results[1];
            threadingAttributeValue = results[2];
            if (threadingPolicy == "thread-per-request" && threadingAttribute.length()==0 && threadingAttributeValue.length()==0) {
                println("EntityResourceFactoryImpl 2 : ");
                println(threadingPolicy);
                succeed();
            }

            println("- getFrameworkMultithreadAttribute -");
            results = init.getFrameworkMultithreadAttribute("first.framework", 2);
            threadingPolicy = results[0];
            threadingAttribute = results[1];
            threadingAttributeValue = results[2];
            if (threadingPolicy == "thread-pool" && threadingAttribute == "thread-size" && threadingAttributeValue == "5") {
                println("EntityResourceFactoryImpl 3 : ");
                println(threadingPolicy);
                println(threadingAttribute);
                println(threadingAttributeValue);
                succeed();
            }

            try {
                println("- testing getFrameworkMultithreadAttribute -");
                results = init.getFrameworkMultithreadAttribute("xxxx", 0);
                threadingPolicy = results[0];
                threadingAttribute = results[1];
                threadingAttributeValue = results[2];
                fail();
            } catch(BadParameterException bpe) {
                println(bpe.what());
                succeed();
            }

            try {
                println("- testing getFrameworkMultithreadAttribute -");
                results = init.getFrameworkMultithreadAttribute("first.framework", 3);
                threadingPolicy = results[0];
                threadingAttribute = results[1];
                threadingAttributeValue = results[2];
                fail();
            } catch(BadParameterException bpe) {
                println(bpe.what());
                succeed();
            }

            try {
                println("- testing getFrameworkMultithreadAttribute -");
                results = init.getFrameworkMultithreadAttribute("second.framework", 0);
                threadingPolicy = results[0];
                threadingAttribute = results[1];
                threadingAttributeValue = results[2];
                fail();
            } catch(BadParameterException bpe) {
                println(bpe.what());
                succeed();
            }

        }
        catch(Exception e) {
            e.printStackTrace();
            fail();
        }

    }

}

