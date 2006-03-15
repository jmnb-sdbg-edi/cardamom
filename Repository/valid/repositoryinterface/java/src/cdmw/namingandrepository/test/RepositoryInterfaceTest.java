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


package cdmw.namingandrepository.test;

import cdmw.namingandrepository.RepositoryInterface;
import cdmw.commonsvcs.naming.NamingInterface;



public class RepositoryInterfaceTest 
    extends cdmw.testutils.Testable
{

    private static final int POA_THREAD_POOL_SIZE = 2;
    private static final String NAMING_REPOS_URL_OPTION = "--namingRepos-url";
    
    private static final String DEFAULT_NAME_DOMAIN_INVALID = "%$.*\\'/é|..,\n;¤µ€";
    private static final String DEFAULT_NAME_DOMAIN_UNKNOWN = "UndefinedNameDomain";
    private static final String DEFAULT_NAME_DOMAIN         = "DefaultNameDomain";
    private static final String SUB_NAME_DOMAIN             = "SubNameDomain";
    private static final String OTHER_NAME_DOMAIN           = "AnotherNameDomain";
    
    
    private org.omg.CORBA.ORB orb = null;
    private com.thalesgroup.CdmwNamingAndRepository.Repository repository = null;
    
    
    
    RepositoryInterfaceTest(String[] args)
        throws org.omg.CORBA.SystemException
    {
        this.name = "Test RepositoryInterface";
        this.out = System.out;

        // ===================================================
        // Initialize the ORB
        // ===================================================        
        cdmw.orbsupport.StrategyList orbStrategies = 
            new cdmw.orbsupport.StrategyList();
        orbStrategies.addORBThreaded();
        try {
            orbStrategies.addPOAThreadPool(POA_THREAD_POOL_SIZE);
        } catch (cdmw.common.BadParameterException equals) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        
        this.orb = cdmw.orbsupport.ORBSupport.ORBInit(args, orbStrategies);

        // ===================================================
        // Get the root POA 
        // ===================================================
        
        org.omg.CORBA.Object obj = null;
        try {
            obj = orb.resolve_initial_references("RootPOA");
        } catch (org.omg.CORBA.ORBPackage.InvalidName e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
        org.omg.PortableServer.POA rootPOA = 
            org.omg.PortableServer.POAHelper.narrow(obj);
        
        // ===================================================
        // Activate the root POA manager
        // ===================================================
        org.omg.PortableServer.POAManager poaManager = rootPOA.the_POAManager();
        try {
            poaManager.activate();
        } catch (org.omg.PortableServer.POAManagerPackage.AdapterInactive e) {
            // shouldn't happen
            cdmw.common.Assert.check(false);
        }
    }
    
    
    public void init(String namingReposURL)
        throws org.omg.CORBA.SystemException
    {
        // ===================================================
        // Get the naming and repository
        //     from the input URL
        // ===================================================
        org.omg.CORBA.Object obj = orb.string_to_object(namingReposURL);
        this.repository = com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper.narrow(obj);            
    }
    
    
    public void destroyORB() {
        repository = null;
        if (orb != null) {
            try {
                orb.destroy();
                orb = null;
            } catch (org.omg.CORBA.SystemException ex) {
                System.err.println(ex.toString());
                ex.printStackTrace(System.err);
            }
        }
    }
    

    public void doTests() {
        // set number of requested successfull tests
        setNbOfRequestedTestOK(9);
        
        println("Try RepositoryInterface.init with invalid domain name");
        try {
            RepositoryInterface.init(DEFAULT_NAME_DOMAIN_INVALID, repository);
            fail("expected InvalidName exception bas not been thrown");
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName e) {
            succeed("expected InvalidName exception catched");
        } catch (Exception e) {
            fail("Unexpected exception: " + e.toString());
            e.printStackTrace(out);
        }
        
        println("Try RepositoryInterface.init with unwkown domain name");
        try {
            RepositoryInterface.init(DEFAULT_NAME_DOMAIN_UNKNOWN, repository);
            fail("expected NoNameDomain exception bas not been thrown");
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain e) {
            succeed("expected NoNameDomain exception catched");
        } catch (Exception e) {
            fail("Unexpected exception: " + e.toString());
            e.printStackTrace(out);
        }
        
        println("RepositoryInterface.init with valid domain name");
        try {
            RepositoryInterface.init(DEFAULT_NAME_DOMAIN, repository);
            succeed("RepositoryInterface successfully initialised");
        } catch (Exception e) {
            fail("Unexpected exception: " + e.toString());
            e.printStackTrace(out);
        }
        
        println("Test getRepository()");
        if (repository == RepositoryInterface.getRepository())
            succeed();
        else
            fail();
        

        println("Get NamingInterface for default name domain");
        try {
            NamingInterface ni = RepositoryInterface.getDomainNamingInterface("");
            try {
                // check that ni correspond to default name domain
                org.omg.CORBA.Object obj = ni.resolve(SUB_NAME_DOMAIN);
                succeed();
            } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                fail("NamingInterface doesn't correspond to default name domain");
                e.printStackTrace(out);
            }
        } catch (Exception e) {
            fail("Cannot retrieve default name domain: " + e.toString());
            e.printStackTrace(out);
        }


        println("Get NamingContext for default name domain");
        try {
            org.omg.CosNaming.NamingContext nc = RepositoryInterface.getDomainContext("");
            try {
                // check that nc correspond to default name domain
                org.omg.CosNaming.NameComponent[] name = 
                    cdmw.commonsvcs.naming.NameParser.toName(SUB_NAME_DOMAIN);
                org.omg.CORBA.Object obj = nc.resolve(name);
                succeed();
            } catch (org.omg.CosNaming.NamingContextPackage.NotFound e) {
                fail("NamingContext doesn't correspond to default name domain");
                e.printStackTrace(out);
            }
        } catch (Exception e) {
            fail("Cannot retrieve default name domain: " + e.toString());
            e.printStackTrace(out);
        }


        println("Get default name domain");
        try {
            com.thalesgroup.CdmwNamingAndRepository.NameDomain nd = 
                RepositoryInterface.getDomain("");
            try {
                // check that nd correspond to default name domain
                com.thalesgroup.CdmwNamingAndRepository.NameDomain subnd = 
                    nd.resolve_sub_domain(SUB_NAME_DOMAIN);
                succeed();
            } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain e) {
                fail("NameDomain doesn't correspond to default name domain");
                e.printStackTrace(out);
            }
        } catch (Exception e) {
            fail("Cannot retrieve default name domain: " + e.toString());
            e.printStackTrace(out);
        }

        println("Try to get an unkown name domain");
        try {
            com.thalesgroup.CdmwNamingAndRepository.NameDomain nd = 
                RepositoryInterface.getDomain(DEFAULT_NAME_DOMAIN_UNKNOWN);
            fail("expected NoNameDomain exception has not been thrown");
        } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain e) {
            succeed("expected NoNameDomain exception catched");
        } catch (com.thalesgroup.CdmwNamingAndRepository.InvalidName e) {
            fail("Name " + DEFAULT_NAME_DOMAIN_UNKNOWN + " is an invalid name");
            e.printStackTrace(out);
        }
        
        println("Try to get another name domain");
        try {
            com.thalesgroup.CdmwNamingAndRepository.NameDomain nd = 
                RepositoryInterface.getDomain(OTHER_NAME_DOMAIN);
            try {
                // check that nd doesn't correspond to default name domain
                com.thalesgroup.CdmwNamingAndRepository.NameDomain subnd = 
                    nd.resolve_sub_domain(SUB_NAME_DOMAIN);
                fail("returned NameDomain correspond to default name domain");
            } catch (com.thalesgroup.CdmwNamingAndRepository.NoNameDomain e) {
                succeed();
            }
        } catch (Exception e) {
            fail("Cannot retrieve other name domain: " + e.toString());
            e.printStackTrace(out);
        }
        
        
    
        // ================================================================
        // Release all static object references held by RepositoryInterface
        // ================================================================
        RepositoryInterface.finish();
    }
    
    
    
    
    
    private static void usage() {
        System.out.println("Usage:\n");
        System.out.println("RepositoryInterfaceTest options\n");
        System.out.println("Options:\n");
        System.out.println();
        System.out.println("--help                     Display this information.\n");
        System.out.println();
        System.out.println(NAMING_REPOS_URL_OPTION + "=<URL>  URL of the CDMW Naming and Repository service.\n");
        System.out.println();
    }
    

    
    public static void main(String[] args) {
        
        //
        // help argument
        //
        if(args.length > 0 && args[0].equals("--help"))	{    
            usage();
            return;
        }

        //
        // get Naming & Repository URL from arguments
        //
        String namingReposURL = 
             cdmw.ossupport.OS.getOptionValue (args, NAMING_REPOS_URL_OPTION);
             
        if (namingReposURL.equals("no")) 
        {
            System.out.println("naming and repository url undefined");
            usage();
            return;
        }

        RepositoryInterfaceTest test = new RepositoryInterfaceTest(args);
        try {
            test.init(namingReposURL);
            test.start();
        } catch (org.omg.CORBA.SystemException e) {
            System.err.println("Exception while retrieving Repository reference: " + e.toString());
            e.printStackTrace(System.err);
        }
        
        test.destroyORB();
    
    }
    
}
