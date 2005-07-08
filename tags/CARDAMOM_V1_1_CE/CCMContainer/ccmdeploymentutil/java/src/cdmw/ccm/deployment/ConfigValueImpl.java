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

import org.omg.CORBA.Any;
import org.omg.Components.ConfigValue;

public class ConfigValueImpl extends ConfigValue
    implements java.io.Serializable {

    public ConfigValueImpl() {}

    public ConfigValueImpl(String n, Any val) {
        this.name = n;
        this.value = val;
    }


    public boolean equals(Object o) {
        if (o == null) 
            return false;

        if (this == o)
            return true;

        if (!(o instanceof ConfigValue))
            return false;

        ConfigValue c = (ConfigValue) o;
        
        if (name == null) {
            if (c.name != null)
                return false;
        } else {
            if (! name.equals(c.name))
                return false;
        }
        
        if (value == null)
            return (c.value == null);
        else
            return (value.equal(c.value));
    }
    

    private void writeObject(java.io.ObjectOutputStream out)
        throws java.io.IOException {
        System.out.println("ConfigValue.writeObject");
        out.writeObject(name);
        out.writeObject(value);
    }

    private void readObject(java.io.ObjectInputStream in)
        throws java.io.IOException, ClassNotFoundException {
        name = (String) in.readObject();
        value = (Any) in.readObject();
    }

}

