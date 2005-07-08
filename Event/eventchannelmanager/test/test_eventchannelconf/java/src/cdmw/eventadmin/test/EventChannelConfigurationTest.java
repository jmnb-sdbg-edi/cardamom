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
import cdmw.common.InternalErrorException;

import cdmw.eventadmin.EventChannelConfiguration;
import cdmw.eventadmin.ConfigurationException;

import com.thalesgroup.CdmwEventAdmin.EventChannelProperty;

public class EventChannelConfigurationTest extends Testable {

    private EventChannelConfiguration conf;
    private String xmlFile;
    private boolean doValidate;

    public EventChannelConfigurationTest(String xmlFilename, 
                                            boolean doValidate) {
    
        name = "EventChannelConfigurationTest";
        out = System.out;
        conf = null;
        xmlFile = xmlFilename;
        doValidate = doValidate;
    
    }

    public void doTests() {
        
        // set number of requested successfull tests
        setNbOfRequestedTestOK(3);
    
        // This tests EventChannelConfiguration class
        //-------------------------------------------------

        boolean testOk = false;

        try {
            println("Instantiate an EventChannelConfiguration object");

            try {
                conf = EventChannelConfiguration.
                    readConfiguration(xmlFile, doValidate);
                if (conf!=null) {
                    testOk = true;
                }
            } catch (ConfigurationException e) {
                // test failed!
                println("ConfigurationException raised");
                println(e.what());
            } catch (InternalErrorException e) {
                // test failed!
                println("InternalErrorException raised");
            }
         
            check(testOk);

            println("Get number of ChannelProfile");
            testOk = false;
            int nbProfile = 0;
            
            try {
                nbProfile = conf.getNbEventChannelProfile();
                System.out.println("nbProfile = " + nbProfile);
                testOk = true;
            } catch (Exception e) {
                // test failed!
            }
            check(testOk);

            println("Get each channel name and properties");
            testOk = false;
            try {
                String name;
                java.util.Vector properties = null;

                for (int i = 0; i < nbProfile; i++) {
                    name = conf.getNextEventChannelName(i);
                    System.out.println("Channel " + i + " = " +name);
                    println(name);

                    properties = conf.getEventChannelProfileValue(name);
                    if (properties!=null) {
                        java.util.Enumeration e = properties.elements();
                        System.out.println("Properties number: " 
                            + properties.size());
                        EventChannelProperty property;
                        for( ; e.hasMoreElements() ; ) {
                            property = (EventChannelProperty) e.nextElement();
                            println(property.name);
                            println(property.value);
                        }
                    } else {
                        println("No Properties!");
                    }
                }
                testOk = true;
            } catch (Exception e) {}
            check(testOk);
            
        } catch (Exception e) {
            System.out.println("ERROR: Unexpected exception");
        }

    }

}


