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


package cdmw.ccm.deployment;

import org.omg.Components.ConfigValue;


public class ConfigValues implements Cloneable {

    private java.util.Hashtable config;


    public ConfigValues() {
        config = new java.util.Hashtable();
    }

    public ConfigValues(ConfigValue[] config) {
        this();
        setConfigValues(config);
    }

    public synchronized int size() {
        return config.size();
    }

    public synchronized ConfigValue setConfigValue(ConfigValue c) {
        if (c == null) 
            throw new NullPointerException("Cannot add null ConfigValue");
            
        return (ConfigValue) config.put(c.name, c);
    }

    public synchronized void setConfigValues(ConfigValue[] config) {
        for(int i=0; i<config.length; i++) {
            this.config.put(config[i].name, config[i]);
        }
    }

    public synchronized ConfigValue getConfigValue(String name) {
        return (ConfigValue) config.get(name);
    }

    public synchronized ConfigValue[] getConfigValueArray() {
        ConfigValue[] conf = new ConfigValue[config.size()];
        java.util.Iterator it = config.values().iterator();
        int i = 0;
        while (it.hasNext()) {
            conf[i] = (ConfigValue) it.next();
            i++;
        }

        return conf;
    }

    public synchronized boolean equals (Object o) {
        if (o == null)
            return false;
            
        if (this == o) 
            return true;

        if (!(o instanceof ConfigValues))
            return false;
            
        ConfigValues c = (ConfigValues) o;
                        
        return config.equals(c.config);
            
    }
    
    
    public synchronized Object clone() {
        try {
            ConfigValues c = (ConfigValues) super.clone();
            c.config = (java.util.Hashtable) config.clone();
            return c;
        } catch (CloneNotSupportedException e) { 
            // this shouldn't happen, since super is cloneable
            // and config attribute is cloneable
            e.printStackTrace(System.err);
            throw new InternalError();
        }
    }
    
}


