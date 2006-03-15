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


package cdmw.lifecycle.test;

import cdmw.testutils.Testable;
import cdmw.lifecycle.EntityCleanupNotifier;

import com.thalesgroup.CdmwNamingAndRepository.NameDomain;

import org.omg.CORBA.ORB;

import org.omg.CosPropertyService.PropertySet;
import org.omg.CosPropertyService.PropertySetHelper;
import org.omg.CosPropertyService.PropertySetDef;
import org.omg.CosPropertyService.PropertySetDefFactory;
import org.omg.CosPropertyService.InvalidPropertyName;


public class EntityCleanupNotifierTest extends Testable {

    public static final String PROPERTY_NAME = "EntityData";

    public static final String OBJECT_1_NAME = "OBJECT  1";
    public static final String OBJECT_2_NAME = "OBJECT  2";
    public static final String OBJECT_3_NAME = "OBJECT  3";

    private PropertySetDefFactory factory;

    private ORB orb;

    public EntityCleanupNotifierTest(PropertySetDefFactory fac,ORB orb) {
        this.name = "EntityCleanupNotifierTest";
        this.out = System.out;
        this.factory = fac;
        this.orb = orb;
    }

    public void doTests() {
    	
    	// set number of requested successfull tests
		setNbOfRequestedTestOK(14);

        // This tests EntityCleanupNotifier class
        //-------------------------------------------------
        println("Testing EntityCleanupNotifier Persistence");

        try {

            // Create a PropertySet
            PropertySetDef setdef = factory.create_propertysetdef();
            PropertySet set = PropertySetHelper.unchecked_narrow(setdef);

            println("Creating an EntityCleanupNotifier object");
            EntityCleanupNotifier ecn = new EntityCleanupNotifier(null);

            println("First init()");
            check(ecn.init(set,PROPERTY_NAME));

            println("Testing that Property is not defined");
            check(!set.is_property_defined(PROPERTY_NAME));

            println("Testing that Property is not defined");
            check(ecn.entries() == 0);

            // Create 2 Entries
            println("Inserting two entries");
            byte[] oid = OBJECT_1_NAME.getBytes();
            ecn.insertEntry(oid,OBJECT_1_NAME);
            oid = OBJECT_2_NAME.getBytes();
            ecn.insertEntry(oid,OBJECT_2_NAME);

            println("Checking that entries are inserted.");
            check(ecn.entries() == 2);

            // Dump entries
            ecn.dump(System.out);

            check(ecn.finish(orb));
            println("State persisted.");

            println("Checking that Property is defined.");
            check(set.is_property_defined(PROPERTY_NAME));

            // State is persisted. Retreive it!

            println("Creating an EntityCleanupNotifier object");
            ecn = new EntityCleanupNotifier(null);

            check(ecn.entries() == 0);

            println("Retrieving previous state");
            check(ecn.init(set,PROPERTY_NAME));

            println("Checking number of entries");
            check(ecn.entries() == 2);

            // Dump entries
            ecn.dump(System.out);

            // Add 1 entry
            println("Adding another entry");
            oid = OBJECT_3_NAME.getBytes();
            ecn.insertEntry(oid,OBJECT_3_NAME);

            println("Checking number of entries.");
            check(ecn.entries() == 3);

            // Dump entries
            ecn.dump(System.out);

            check(ecn.finish(orb));
            println("New state persisted.");

            // State is persisted. Retreive it again!

            println("Creating an EntityCleanupNotifier object");
            ecn = new EntityCleanupNotifier(null);

            check(ecn.entries() == 0);

            println("Retrieving previous state");
            check(ecn.init(set,PROPERTY_NAME));

            println("Checking number of entries");
            check(ecn.entries() == 3);

            // Dump entries
            ecn.dump(System.out);

        } catch (InvalidPropertyName ipn) {
            println("ERROR: " + ipn.getMessage());
        }

    }

}

