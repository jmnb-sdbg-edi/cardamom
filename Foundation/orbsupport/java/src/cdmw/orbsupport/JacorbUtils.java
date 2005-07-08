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


package cdmw.orbsupport;

import cdmw.ossupport.OS;
import cdmw.ossupport.ThreadedProcess;

 class ORBUtilsImpl {

    /**
     * Command line option to define a specific port
     * for the Root POA Manager (ORB Master).
     */
    public static final String PORT_OPTION = "-p=";

    /**
     * Command line option to define a specific port
     * for the Naming Service (ORB Master).
     */
    public static final String NAMING_PORT_OPTION = "-ORBInitRef.NameService=";
    
    /**
     * Command line option to define a specific port
     * for the Root POA Manager (ORB Slave).
     */
    public static final String PORT_OPTION_SLAVE = " -ORBEndpoint iiop://localhost:";

    /**
     * Command line option to define a specific port
     * for the Naming Service (ORB Slave).
     */
    public static final String NAMING_PORT_OPTION_SLAVE = "-ORBInitRef NameService=";

     /**
     * Command line option to define a specific port
     * for the Property Service.
     */
    public static final String PROPERTY_PORT_OPTION = "--PropPort=";
   
    /**
     * Naming Service Java class name for this ORB.
     */
     
    public static final String JAVA_NAMING_SERVICE
      //  = "org.jacorb.naming.NameServer";
		= "cdmw.orbsupport.JacorbNamingService";
    /**
     * Property Service Java class name for this ORB.
     */
    public static final String JAVA_PROPERTY_SERVICE
        = "cdmw.orbsupport.JacorbPropertyService";

    /**
     * Event Service Java class name for this ORB.
     */
   public static final String JAVA_EVENT_SERVICE
        = "cdmw.orbsupport.JacORBEventService";

    /**
     * Java class name for this ORB.
     */
    public static final String ORB_CLASS
        = "org.jacorb.orb.ORB";

    /**
     * Java singleton class name for this ORB.
     */
    public static final String ORB_SINGLETON_CLASS
        = "org.jacorb.orb.ORBSingleton";

     
    /**
     * Thread used to start the Orbacus Naming Service.
     */
    public static class NamingServiceThread extends Thread {
    
        /**
         * Command line arguments
         */
        private String[] args;

        public NamingServiceThread(String[] args) {
            this.args = args;
        }
    
        public void run() {
            org.jacorb.naming.NameServer.main(this.args);
        }
    
    }
    
    /**
     * Thread used to start the Orbacus Property Service.
     */
  public static class PropertyServiceThread extends Thread {
    
        /**
         * Command line arguments
         */
        private String[] args;
        
        public PropertyServiceThread(String[] args) {
            this.args = args;
        }
        
        public void run() {
            cdmw.orbsupport.JacorbPropertyService.main(this.args);
        }
        
    }
    
    /**
     * Thread used to start the Orbacus Event Service.
     */
   public static class EventServiceThread extends Thread {
    
        /**
         * Command line arguments
         */
        private String[] args;
    
        public EventServiceThread(String[] args) {
            this.args = args;
        }        
        
        public void run() {
          //  com.ooc.CosEvent.Server.main(this.args);
        }
        
    }

}

