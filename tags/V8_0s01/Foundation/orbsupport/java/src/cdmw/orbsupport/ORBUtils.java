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


package cdmw.orbsupport;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

/**
 * This class defines ORB properties and it
 * provides a set of methods to manage ORB services
 *
 *
 *
 */
public class ORBUtils {

  private static final ORBUtilsImpl orbUtils= new ORBUtilsImpl();
  
  
  	/**
     * Command line option to define a specific port
     * for the Root POA Manager (ORB Master).
     */
    public static final String PORT_OPTION = orbUtils.PORT_OPTION;

    /**
     * Command line option to define a specific port
     * for the Naming Service (ORB Master).
     */
    public static final String NAMING_PORT_OPTION = orbUtils.NAMING_PORT_OPTION;
  
  	/**
     * Command line option to define a specific port
     * for the Root POA Manager (ORB Slave).
     */
    public static final String PORT_OPTION_SLAVE = orbUtils.PORT_OPTION_SLAVE;

    /**
     * Command line option to define a specific port
     * for the Naming Service (ORB Slave).
     */
    public static final String NAMING_PORT_OPTION_SLAVE = orbUtils.NAMING_PORT_OPTION_SLAVE;
  
  	/**
     * Start the underlying ORB specific Naming Service.
     *
     * @return The Naming Service process as a ThreadedProcess object.
     */
     public static ThreadedProcess startNamingService()
        throws java.io.IOException {
		return OS.createJavaProcess(orbUtils.JAVA_NAMING_SERVICE);
	}

    /**
     * Start the underlying ORB specific Naming Service on a specific port.
     *
     * @param port The specific port on which the service must listen
     * @return The Naming Service process as a ThreadedProcess object.
     */
    public static ThreadedProcess startNamingService(String port)
        throws java.io.IOException {
		return OS.createJavaProcess(
			orbUtils.JAVA_NAMING_SERVICE + " " + orbUtils.PORT_OPTION + port);
    }
    
    /**
     * Start the underlying ORB specific Naming Service in a separate thread.
     *
     * @param args Command line arguments.
     */
    public static void startNamingService(String[] args) {
	    Thread naming = new ORBUtilsImpl.NamingServiceThread(args);
	    naming.start();
    }

    /**
     * Start the underlying ORB specific Property Service.
     *
     * @return The Property Service process as a ThreadedProcess object.
     */
    public static ThreadedProcess startPropertyService()
        throws java.io.IOException {
		return OS.createJavaProcess(orbUtils.JAVA_PROPERTY_SERVICE);
    }

    /**
     * Start the underlying ORB specific Property Service on a specific port.
     *
     * @param port The specific port on which the service must listen
     * @return The Property Service process as a ThreadedProcess object.
     */
    public static ThreadedProcess startPropertyService(String port)
       throws java.io.IOException{
	    return OS.createJavaProcess(
			orbUtils.JAVA_PROPERTY_SERVICE + " " + orbUtils.PROPERTY_PORT_OPTION + port);	
    }
    
    /**
     * Start the underlying ORB specific Property Service in a separate thread.
     *
     * @param args Command line arguments.
     */
    public static void startPropertyService(String[] args) {
	Thread property = new ORBUtilsImpl.PropertyServiceThread(args);
	property.start();
    }

    /**
     * Start the underlying ORB specific Event Service.
     *
     * @return The Event Service process as a ThreadedProcess object.
     */
    public static ThreadedProcess startEventService()
        throws java.io.IOException {
		return OS.createJavaProcess(orbUtils.JAVA_EVENT_SERVICE);
    }

    /**
     * Start the underlying ORB specific Event Service with specific options.
     *
     * @param options It specifics the options for the service
     * @return The Event Service process as a ThreadedProcess object.
     */
    public static ThreadedProcess startEventService(String options)
        throws java.io.IOException {
		return OS.createJavaProcess(orbUtils.JAVA_EVENT_SERVICE + " " + options);
    }
  
    /**
     * Start the underlying ORB specific Event Service in a separate thread.
     *
     * @param args Command line arguments.
     */
    public static void startEventService(String[] args) {
	Thread event = new ORBUtilsImpl.EventServiceThread(args);
	event.start();
    }

    /**
     * Set the underlying ORB specific Java properties.
     *
     * @return The new Properties object with the set values.
     */
       public static java.util.Properties getProperties() {
        java.util.Properties props = System.getProperties();
        props.put("org.omg.CORBA.ORBClass", orbUtils.ORB_CLASS);
        props.put("org.omg.CORBA.ORBSingletonClass", orbUtils.ORB_SINGLETON_CLASS);
        return props;
    }
    
    
  

}

