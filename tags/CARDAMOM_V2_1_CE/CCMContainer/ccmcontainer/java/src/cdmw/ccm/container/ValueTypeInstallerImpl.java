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


package cdmw.ccm.container;

import cdmw.ossupport.JarClassLoader;

import com.thalesgroup.CdmwCcmContainer.ValueTypeRegistration;

import org.omg.CORBA.portable.ValueFactory;
import org.omg.Components.Deployment.ImplEntryPointNotFound;

/**
 * Implementation of the Session2Context internal interface for use with
 * Session container type.
 */
public class ValueTypeInstallerImpl
    extends org.omg.CORBA.LocalObject
    implements com.thalesgroup.CdmwCcmContainer.ValueTypeInstaller {

    private final class RegistrationInfo {

        public ValueTypeRegistration registration;
        public boolean release;

        public RegistrationInfo(ValueTypeRegistration reg, boolean release) {
            this.registration = reg;
            this.release = release;
        }

    };

    private java.util.HashMap registrations = new java.util.HashMap();

    private org.omg.CORBA_2_3.ORB orb;

    /**
     * Constructor.
     *
     * @param orb A reference to the ORB pseudo-object
     */
    public ValueTypeInstallerImpl(org.omg.CORBA.ORB orb) {
        this.orb = (org.omg.CORBA_2_3.ORB) orb;
    }

    //
    // IDL:thalesgroup.com/CdmwCcmContainer/ValueTypeInstaller/install_valuetype_factory:1.0
    //
    public synchronized ValueTypeRegistration install_valuetype_factory(
        String repid,
        String location,
        String entrypoint) {

        ValueTypeRegistration reg = null;
        RegistrationInfo regInfo = (RegistrationInfo) registrations.get(repid);
        if (regInfo != null) {
            // Already registered through the installer.
            // Return existing registration object.
            reg = regInfo.registration;
        } else {
            // Register the valuetype factory within the ORB.
            ValueFactory oldValuetypeFac = this.orb.lookup_value_factory(repid);

            if (oldValuetypeFac == null) {

                // Was not already registered outside of the installer
                // Leave previous valuetype factory and create a registration object
                // with no unregistration if registered outside of the installer.
                java.lang.reflect.Method factoryMethod = null;
                try {
                    // get the classname and the method name
                    int ind = entrypoint.lastIndexOf('.');
                    String classname = entrypoint.substring(0, ind);
                    String methodname = entrypoint.substring(ind + 1);
                    // load the class
                    Class factory = loadClass(classname, location);
                    // get the method object
                    java.lang.reflect.Method[] methods = factory.getMethods();
                    for(int i=0; i<methods.length; i++) {
                        if (methods[i].getName().equals(methodname)) {
                            factoryMethod = methods[i];
                            break;
                        }
                    }
                } catch(IndexOutOfBoundsException ioobe) {
                    throw new org.omg.CORBA.BAD_PARAM(ioobe.toString());
                } catch(LinkageError le) {
                    throw new org.omg.CORBA.BAD_PARAM(le.toString());
                } catch(ClassNotFoundException cnfe) {
                    throw new org.omg.CORBA.BAD_PARAM(cnfe.toString());
                }

                // Get Valuetype Factory
                ValueFactory valuetypeFac = null;
                try {
                    valuetypeFac =
                        (ValueFactory) factoryMethod.invoke(null, null);
                } catch(IllegalAccessException iae) {
                    throw new org.omg.CORBA.BAD_PARAM(iae.toString());
                } catch(IllegalArgumentException iae) {
                    throw new org.omg.CORBA.BAD_PARAM(iae.toString());
                } catch(java.lang.reflect.InvocationTargetException ite) {
                    throw new org.omg.CORBA.BAD_PARAM(ite.toString());
                }

                if (valuetypeFac == null) {
                    throw new org.omg.CORBA.BAD_PARAM(
                        com.thalesgroup.CdmwDeployment.UNSUPPORTED_COMPONENT_TYPE.value,
                        org.omg.CORBA.CompletionStatus.COMPLETED_NO);
                }

                this.orb.register_value_factory(repid, valuetypeFac);
            }
            RegistrationInfo info = new RegistrationInfo(
                new ValueTypeRegistrationImpl(this, repid), true);
            reg = info.registration;
            registrations.put(repid, info);
        }
        return reg;

    }

    public synchronized boolean unregister_valuetype_factory(String repid) {

        boolean found = false;
        RegistrationInfo regInfo = (RegistrationInfo) registrations.get(repid);
        if (regInfo != null) {
            if (regInfo.release) {
                this.orb.unregister_value_factory(repid);
            }
            registrations.remove(repid);
            found = true;
        }
        return found;

    }
    
    /**
     * Loads the class with the given name. If the classname is not found 
     * in the current classpath, then the location param is used to try 
     * to load it from the specified jar file.
     * 
     * @param classname The name of the class to load
     * @param location Optional pathname to an external jar file
     * @return The resulting Class object
     * @exception ClassNotFoundException if the classname is not found
     */
    private synchronized Class loadClass(String classname, String location) 
        throws ClassNotFoundException {
        
        try {
            // try to find the class in the current classpath
            return Class.forName(classname);
        } catch(ClassNotFoundException cnfe) {
            // class was not found, so we try to find it at the given location
            System.out.println("Loading " + classname + " from jar file " + location);
            JarClassLoader jarLoader = new JarClassLoader(location);
            return jarLoader.loadClass(classname);
        }
        
    }

}

