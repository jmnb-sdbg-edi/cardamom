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

import cdmw.ccm.deployment.ConfigValueImpl;
import cdmw.ccm.deployment.ConfigValues;



public class ConfigValuesTest extends cdmw.testutils.Testable {

    private static final String STRING_CONFIG_NAME = "StringConfig";
    private static final String STRING_CONFIG_VALUE = "A String";

    private static final String VALUETYPE_CONFIG_NAME = "A Valuetype";
    
    private org.omg.CORBA.ORB orb;
    
    
    
    public ConfigValuesTest(org.omg.CORBA.ORB orb) {
        this.out = System.out;
        this.name = "ConfigValues tests";
        this.orb = orb;
    }
    
    
    public void doTests() {
    
        // set number of requested successfull tests
        setNbOfRequestedTestOK(5);
        
        // test ConfigValueImpl.equals()
        println("test ConfigValueImpl.equals()");
        
        org.omg.CORBA.Any any1 = orb.create_any();
        any1.insert_string(STRING_CONFIG_VALUE);
        ConfigValueImpl configVal1 = new ConfigValueImpl(STRING_CONFIG_NAME, any1);
        
        org.omg.CORBA.Any any2 = orb.create_any();
        any2.insert_string(STRING_CONFIG_VALUE);
        ConfigValueImpl configVal2 = new ConfigValueImpl(STRING_CONFIG_NAME, any2);
        
        if (configVal1.equals(configVal2)) {
            succeed();
        } else {
            fail();
        }
        
        org.omg.CORBA.Any any3 = orb.create_any();
        org.omg.Components.ConfigValueHelper.insert(any3, configVal1);
        ConfigValueImpl configVal3 = new ConfigValueImpl(VALUETYPE_CONFIG_NAME, any3);
        
        org.omg.CORBA.Any any4 = orb.create_any();
        org.omg.Components.ConfigValueHelper.insert(any4, configVal2);
        ConfigValueImpl configVal4 = new ConfigValueImpl(VALUETYPE_CONFIG_NAME, any4);
        if (configVal3.equals(configVal4)) {
	    succeed();
        } else {
            fail();
        }
        
        // test ConfigValues.equals
        println("test ConfigValues.equals()");
        ConfigValues config1 = new ConfigValues();
        ConfigValues config2 = new ConfigValues();

        config1.setConfigValue(configVal1);
        if (config1.equals(config2)) {
            fail();
        } else {
            succeed();
        }
        
        config2.setConfigValue(configVal4);
        config2.setConfigValue(configVal2);
        if (config1.equals(config2)) {
            fail();
        } else {
            succeed();
        }
        
        config1.setConfigValue(configVal3);
        if (config1.equals(config2)) {
            succeed();
        } else {
            fail();
        }
        
    
    
    }
    
    
    

}

