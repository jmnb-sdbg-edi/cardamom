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


package cdmw.namingandrepository.test;

import cdmw.testutils.Testable;
import cdmw.namingandrepository.RepositoryInterface;
import cdmw.namingandrepository.NamingInterface;

import com.thalesgroup.CdmwNamingAndRepository.Repository;
import com.thalesgroup.CdmwNamingAndRepository.RepositoryHelper;
import com.thalesgroup.CdmwNamingAndRepository.InvalidName;
import com.thalesgroup.CdmwNamingAndRepository.NoNameDomain;

import org.omg.CosNaming.NamingContext;
import org.omg.CORBA.SystemException;


public class RepositoryTest extends Testable {

    private NamingContext context;

    public static final String REPOSITORY_BINDING_NAME
        = "CDMW.I/repository.simulated";
    public static final String DEFAULT_DOMAIN_NAME
        = "MyDefault.system/MyDefault.domain";


    public RepositoryTest(NamingContext nc) {
        this.out = System.out;
        this.name = "RepositoryTest";
        this.context = nc;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(7);

        // This tests RepositoryInterface class
        //-------------------------------------------------
        // Testing string to name conversion
        //-------------------------------------------------

        // Get reference to the repository interface
        Repository rep = null;
        try {
            NamingInterface ni = new NamingInterface(context);
            org.omg.CORBA.Object obj = ni.resolve(REPOSITORY_BINDING_NAME);
            rep = RepositoryHelper.narrow(obj);
            if (rep==null) {
                println("Couldn't retrieve reference of the Repository");
                fail();
                return;
            }
        } catch (Exception e) {
            println("Couldn't retrieve reference of the Repository");
            e.printStackTrace();
            fail();
            return;
        }

        // Init of the RepositoryInterface
        println("Init of the RepositoryInterface.");
        try {
            RepositoryInterface.init(DEFAULT_DOMAIN_NAME,rep);
            succeed();
        } catch (NoNameDomain nnd) {
            fail();
            return;
        } catch (InvalidName in) {
            fail();
            return;
        } catch (SystemException e) {
            fail();
            return;
        }


        //    MyDefault.system/MyDefault.domain/dom1.1/dom2.2
        //    MyDefault.system/MyDefault.domain/dom2.1
        //    Another.system/Another.domain/domX.Y
        println("Test get_domain_naming_interface()");

        try {
            NamingInterface ni
                = RepositoryInterface.getDomainNamingInterface("");
            String input = "dom1.1/dom2.2";
            org.omg.CORBA.Object obj = ni.resolve(input);
            check(obj!=null);
            input = "MyDefault.system/MyDefault.domain";
            ni = RepositoryInterface.getDomainNamingInterface(input);
            input = "dom2.1";
            obj = ni.resolve(input);
            check(obj!=null);
            input = "Another.system/Another.domain";
            ni = RepositoryInterface.getDomainNamingInterface(input);
            input = "domX.Y";
            obj = ni.resolve(input);
            check(obj!=null);
        } catch (Exception e) {
            fail();
            return;
        }

        println("Test get_domain_context()");

        try {
            NamingContext nc = RepositoryInterface.getDomainContext("");
            NamingInterface ni = new NamingInterface(nc);
            String input = "dom1.1/dom2.2";
            org.omg.CORBA.Object obj = ni.resolve(input);
            check(obj!=null);
            input = "MyDefault.system/MyDefault.domain";
            nc = RepositoryInterface.getDomainContext(input);
            ni = new NamingInterface(nc);
            input = "dom2.1";
            obj = ni.resolve(input);
            check(obj!=null);
            input = "Another.system/Another.domain";
            nc = RepositoryInterface.getDomainContext(input);
            ni = new NamingInterface(nc);
            input = "domX.Y";
            obj = ni.resolve(input);
            check(obj!=null);
        } catch (Exception e) {
            fail();
            return;
        }

    }

}

