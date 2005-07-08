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


package cdmw.eventadmin.test;

import cdmw.testutils.Testable;

import com.thalesgroup.CdmwEventAdmin.EventChannelProfileManager;
import com.thalesgroup.CdmwEventAdmin.EventChannelProperty;

public class EventChannelProfileManagerTest extends Testable {

    public static final String CHANNEL_1_NAME = "CHANNEL 1";
    public static final String CHANNEL_2_NAME = "CHANNEL/2";
    public static final String CHANNEL_3_NAME = "CHANNEL 3";

    public static final String TYPE_NAME = "TYPE";
    public static final String UNTYPED_FLAG = "Untyped";
    public static final String TYPED_FLAG = "Typed";

    public static final String URL_NAME = "URL";
    public static final String URL_UNTYPED 
        = "corbaloc::amidala:2098/DefaultEventChannelFactory";
    public static final String URL_TYPED 
        = "corbaloc::localhost:2098/DefaultTypedEventChannelFactory";

    public static final String[] XML_CHANNEL_NAME = {
        "TestEventChannel1",
        "TestEventChannel2",
        "TestEventChannel3"
    };

    public static final int[] XML_CHANNEL_NB_PROPERTY = {2, 2, 1};

    public static final String[] XML_CHANNEL_TYPE_FLAG = {
        "Typed",
        "Untyped",
        "Typed"
    };

    public static final String[] XML_URL = {
        URL_TYPED,
        URL_UNTYPED,
        ""
    };

    private EventChannelProfileManager profileManager;

    public EventChannelProfileManagerTest(EventChannelProfileManager manager) {
        name= "EventChannelProfileManagerTest";
        out = System.out;
        this.profileManager = manager;
    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(12);
    
        // This tests TestEventChannelProfileManager class
        //-------------------------------------------------

        boolean testOk = true;

        println("Listing existing event channel profile");
        String[] eventChannelNames;
        try {
            eventChannelNames = profileManager.managed_event_channels();
            int nbNames = eventChannelNames.length;

            if (nbNames == XML_CHANNEL_NAME.length) {
                for (int i=0; i<nbNames; i++) {
                    String name = eventChannelNames[i];
                    if (!name.equals(XML_CHANNEL_NAME[i])) {
                        // get out the loop, test failed!
                        println("Profile[" + i + "]: " + name
                                    + ", instead of " + XML_CHANNEL_NAME[i]);
                        testOk = false;
                        break;
                    }
                }
            } else {
                println("XML file contains " + nbNames 
                            + " channel profiles, waiting " + XML_CHANNEL_NAME.length);
                testOk = false;
            }
        } catch(Exception e) {
             // test failed!
             println("Exception: " + e.getMessage());
             testOk = false;
        }
        check(testOk);

        // Check now the EventChannelProfileManager correctly read the XML file
        println("Find profile defined in XML file and check it");
        testOk = false;
        for (int i=0; i<XML_CHANNEL_NAME.length; i++) {
            try {
                EventChannelProperty[] returnProfile
                    = profileManager.find_profile(XML_CHANNEL_NAME[i]);

                if (returnProfile.length == XML_CHANNEL_NB_PROPERTY[i]) {
                    testOk = true;
                    String name = returnProfile[0].name;
                    if ( !name.equals(TYPE_NAME) ) {
                        println("Profile for " + XML_CHANNEL_NAME[i]);
                        println(": Prop1 Name is " + name 
                                    + " instead of " + TYPE_NAME);
                        testOk = false;
                        break;
                    }

                    String value = returnProfile[0].value;
                    if ( !value.equals(XML_CHANNEL_TYPE_FLAG[i]) ) {
                        println("Profile for " + XML_CHANNEL_NAME[i]);
                        println(": Prop1 Value is " + value + " instead of " 
                            + XML_CHANNEL_TYPE_FLAG[i]);
                        testOk = false;
                        break;
                    }

                    if (XML_CHANNEL_NB_PROPERTY[i] > 1) {
                        name = returnProfile[1].name;
                        if ( !name.equals(URL_NAME) ) {
                            println("Profile for " + XML_CHANNEL_NAME[i]);
                            println(": Prop2 Name is " + name 
                                + " instead of " + URL_NAME);
                            testOk = false;
                            break;
                        }
                        
                        value = returnProfile[1].value;
                        if ( !value.equals(XML_URL[i]) ) {
                            println("Profile for " + XML_CHANNEL_NAME[i]);
                            println(": Prop2 Value is " + value 
                                        + " instead of " + XML_URL[i]);
                            testOk = false;
                            break;
                        }
                    }
                }
            } catch(Exception e) {
                // test failed! get out loop!
                println("Exception: " + e.getMessage());
                testOk = false;
                break; 
            }
        }  // end for loop
        check(testOk);

        println("Creating a new profile");
        testOk = false;
        EventChannelProperty[] profile = new EventChannelProperty[2];
        profile[0] = new EventChannelProperty(TYPE_NAME,TYPED_FLAG);
        profile[1] = new EventChannelProperty(URL_NAME,URL_TYPED);

        try {
            profileManager.create_profile(CHANNEL_1_NAME, profile);
            testOk = true;
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }
        check(testOk);
        

        println("Creating an existant profile, raising AlreadyExistant exception");
        testOk = false;
        try {
            profileManager.create_profile(CHANNEL_1_NAME, profile);
        } catch(com.thalesgroup.CdmwEventAdmin.AlreadyExistant ae) {
            testOk = true;
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }
        check(testOk);

        println("Creating a profile with an invalid channel name, raise InvalidName exception");
        testOk = false;
        try {
            profileManager.create_profile(CHANNEL_2_NAME, profile);
        } catch(com.thalesgroup.CdmwEventAdmin.InvalidName in) {
            testOk = true;
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }
        check(testOk);

        println("Remove the created profile");
        testOk = false;
        try {
            profileManager.remove_profile(CHANNEL_1_NAME);
            testOk = true;
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }        
        check(testOk);

        println("Try to remove it again! Raise NotExist exception.");
        testOk = false;
        try {
            profileManager.remove_profile(CHANNEL_1_NAME);

            // No exception raised, test failed
            println("Unexpected successfull remove operation");
        } catch (com.thalesgroup.CdmwEventAdmin.NotExist ne) {
            testOk = true;
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }
        check(testOk);

        println("Update inexistant profile!");
        testOk = false;
        try {
            profileManager.update_profile(CHANNEL_1_NAME, profile);
        } catch (com.thalesgroup.CdmwEventAdmin.NotExist ne) {
            testOk = true;
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }
        check(testOk);

        println("Create this profile again!");
        testOk = false;
        try {
            profileManager.create_profile(CHANNEL_1_NAME, profile);
            testOk = true;
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }
        check(testOk);
        
        println("Update this profile");
        testOk = false;
        profile = new EventChannelProperty[1];
        profile[0] = new EventChannelProperty(TYPE_NAME,UNTYPED_FLAG);
        try {
            profileManager.update_profile(CHANNEL_1_NAME, profile);
            testOk = true;
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }
        check(testOk);
        
        println("Find a profile");
        testOk = false;
        try {
            EventChannelProperty[] returnProfile
                = profileManager.find_profile(CHANNEL_1_NAME);

            if (returnProfile.length == profile.length) {

                testOk = true;
                for (int j=0; j<profile.length; j++) {
                    String name = returnProfile[j].name;
                    String refName = profile[j].name;
                    if (!name.equals(refName)) {
                        testOk = false;
                        break;
                    }

                    String value = returnProfile[j].value;
                    String refValue = profile[j].value;
                    if (!value.equals(refValue)) {
                        testOk = false;
                        break;
                    }
                }
            }
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }
        check(testOk);
        
        println("Find an inexistant profile");
        testOk = false;
        try {
            profileManager.find_profile(CHANNEL_3_NAME);
            // no exception raised, error
            println("Unexpected profile found!");
        } catch (com.thalesgroup.CdmwEventAdmin.NotExist ne) {
            println("Exception: " + ne.getMessage());
            testOk = true;
        } catch(Exception e) {
            // test failed!
            println("Exception: " + e.getMessage());
        }
        check(testOk);

    }
    
    public void littleCheck() {
        String[] eventChannelNames;
        try {
            eventChannelNames = profileManager.managed_event_channels();
            out.println("List of the existing event channel profile:");
            for(int i = 0; i<eventChannelNames.length; i++) {
                out.print("Channel name: "+eventChannelNames[i]);
                EventChannelProperty[] returnProfile
                    = profileManager.find_profile(eventChannelNames[i]);
                out.println(" Number of properties: " + returnProfile.length);
            }
        } catch(Exception e) {
            e.printStackTrace();
        }
    
    }

}


