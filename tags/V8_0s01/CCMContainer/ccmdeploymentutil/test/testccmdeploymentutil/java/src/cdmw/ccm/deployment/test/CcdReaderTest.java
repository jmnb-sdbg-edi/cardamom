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


package cdmw.ccm.deployment.test;

import com.thalesgroup.CdmwXML.XMLParser;

import com.thalesgroup.CdmwDeployment.ComponentKindValue;
import com.thalesgroup.CdmwDeployment.ServantLifetimeValue;
import com.thalesgroup.CdmwDeployment.ThreadingPolicyValue;

import cdmw.ccm.deployment.CcdReader;


public class CcdReaderTest extends cdmw.testutils.Testable {

    private static final String CCD_FILENAME
        = new java.io.File("./CcdReaderTest.ccd").getAbsolutePath();

    private static final String HOME_REPID = "IDL:CcdReaderTest/HomeTest:1.0";

    private static final String COMP_REPID = "IDL:CcdReaderTest/ComponentTest:1.0";

    private static final ComponentKindValue COMP_KIND =
        ComponentKindValue.SESSION;

    private static final ServantLifetimeValue SERV_LIFETIME =
        ServantLifetimeValue.CONTAINER_LIFETIME;

    private static final ThreadingPolicyValue THREAD_POLICY =
        ThreadingPolicyValue.MULTITHREAD;

    private static final String HOME_SERVANT = "CcdReaderTest.HomeTest_impl";

    private static final boolean CONFIG_COMPLETE = true;


    private static XMLParser parser = null;


    public CcdReaderTest(XMLParser parser)
    {
        this.parser = parser;
        this.out = System.out;
        this.name = "CcdReader tests";
    }


    public void doTests()
    {
        // set number of requested successfull tests
        setNbOfRequestedTestOK(8);
        
        //
        // create CcdReader
        //
        println("Test creation of CcdReader for " + CCD_FILENAME);
        CcdReader ccdReader;
        try {
            ccdReader = new CcdReader(parser, CCD_FILENAME);
            succeed();
        } catch (com.thalesgroup.CdmwXML.XMLParserPackage.ParsingException e) {
            fail("Error while parsing CCD file \"" + CCD_FILENAME + "\":\n" +
                 e.reason);
            return;
        }

        //
        // test getHomeRepositoryId()
        //
        println("Test CcdReader.getHomeRepositoryId()");
        String homeRepid = ccdReader.getHomeRepositoryId();
        if (HOME_REPID.equals(homeRepid)) {
            succeed();
        } else {
            fail("Home RepositoryId found is " + homeRepid + " instead of " + HOME_REPID);
        }

        //
        // test getComponentRepositoryId()
        //
        println("Test CcdReader.getComponentRepositoryId()");
        String compRepid = ccdReader.getComponentRepositoryId();
        if (COMP_REPID.equals(compRepid)) {
            succeed();
        } else {
            fail("Component RepositoryId found is " + compRepid + " instead of " + COMP_REPID);
        }

        //
        // test getComponentKind()
        //
        println("Test CcdReader.getComponentKind()");
        ComponentKindValue compKind = ccdReader.getComponentKind();
        if (COMP_KIND.equals(compKind)) {
            succeed();
        } else {
            fail("Component kind found is " + compKindToString(compKind) +
                " instead of " + compKindToString(COMP_KIND));
        }

        //
        // test getServantLifetime()
        //
        println("Test CcdReader.getServantLifetime()");
        ServantLifetimeValue servLifetime = ccdReader.getServantLifetime();
        if (SERV_LIFETIME.equals(servLifetime)) {
            succeed();
        } else {
            fail("Servant lifetime found is " +servLifetimeToString(servLifetime) +
                " instead of " + servLifetimeToString(SERV_LIFETIME));
        }

        //
        // test getThreadingPolicy()
        //
        println("Test CcdReader.getThreadingPolicy()");
        ThreadingPolicyValue threadPolicy = ccdReader.getThreadingPolicy();
        if (THREAD_POLICY.equals(threadPolicy)) {
            succeed();
        } else {
            fail("Threading policy found is " + threadPolicyToString(threadPolicy) +
                " instead of " + threadPolicyToString(THREAD_POLICY));
        }

        //
        // test getHomeServantClassname()
        //
        println("Test CcdReader.getHomeServantClassname()");
        String homeServant = ccdReader.getHomeServantClassname(homeRepid);
        if (HOME_SERVANT.equals(homeServant)) {
            succeed();
        } else {
            fail("Home servant classname found is " + homeServant +
                 " instead of " + HOME_SERVANT);
        }

        //
        // test getConfigurationcomplete()
        //
        println("Test CcdReader.getConfigurationcomplete()");
        boolean confComplete = ccdReader.getConfigurationcomplete();
        if (CONFIG_COMPLETE == confComplete) {
            succeed();
        } else {
            fail("Configuration complete found is " + confComplete +
                 " instead of " + CONFIG_COMPLETE);
        }
    }


    private String compKindToString(ComponentKindValue compKind) {
        if (compKind == null) {
            return null;
        }

        if (compKind.equals(ComponentKindValue.SERVICE)) {
            return "service";
        } else if (compKind.equals(ComponentKindValue.SESSION)) {
            return "session";
        } else if (compKind.equals(ComponentKindValue.PROCESS)) {
            return "process";
        } else if (compKind.equals(ComponentKindValue.ENTITY)) {
            return "entity";
        } else {
            // shouldn't happen
            cdmw.common.Assert.check(false);
            return null;     // for compiler
        }
    }

    private String servLifetimeToString(ServantLifetimeValue servLifetime) {
        if (servLifetime == null) {
            return null;
        }

        if (servLifetime.equals(ServantLifetimeValue.COMPONENT_LIFETIME)) {
            return "component";
        } else if (servLifetime.equals(ServantLifetimeValue.METHOD_LIFETIME)) {
            return "method";
        } else if (servLifetime.equals(ServantLifetimeValue.TRANSACTION_LIFETIME)) {
            return "transaction";
        } else if (servLifetime.equals(ServantLifetimeValue.CONTAINER_LIFETIME)) {
            return "container";
        } else {
            // shouldn't happen
            cdmw.common.Assert.check(false);
            return null;     // for compiler
        }
    }


    private String threadPolicyToString(ThreadingPolicyValue threadPolicy) {
        if (threadPolicy == null) {
            return null;
        }

        if (threadPolicy.equals(ThreadingPolicyValue.SERIALIZE)) {
            return "serialize";
        } else if (threadPolicy.equals(ThreadingPolicyValue.MULTITHREAD)) {
            return "mutithread";
        } else {
            // shouldn't happen
            cdmw.common.Assert.check(false);
            return null;     // for compiler
        }
    }

}

