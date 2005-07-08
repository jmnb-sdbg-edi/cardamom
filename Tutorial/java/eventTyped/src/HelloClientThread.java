/* =========================================================================== *
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
 * =========================================================================== */


package cdmw.tutorial;


class HelloClientThread
    extends java.lang.Thread
{

    private static final String COMPLETE_TYPED_CHANNEL_NAME = "EVENT_CHANNELS/TypedEventChannel1";
    private static final String INTERFACE_REP = com.thalesgroup.TypedEchoHelper.id();
    
    
    /**
    * The orb reference.
    */
    private org.omg.CORBA.ORB orb;

    /**
    * The application name.
    */
    private String applicationName;

    /**
    * The process name.
    */
    private String processName;

    /**
    * The push consumer reference 
    */
    private com.thalesgroup.TypedEcho pushConsumer;

    /**
    * The flag to break the thread loop.
    */
    private boolean exitRequired;
    
    
    public HelloClientThread(org.omg.CORBA.ORB orb,
                             String applicationName,
                             String processName)
    {
        this.orb = orb;
        this.applicationName = applicationName;
        this.processName = processName;
        this.pushConsumer = null;
        this.exitRequired = false;
      
        try  {      
      
            // =======================================================
            // Get NamingInterface object from RepositoryInterface
            // =======================================================
            cdmw.namingandrepository.NamingInterface ni = 
                cdmw.namingandrepository.RepositoryInterface.getDomainNamingInterface (
                    cdmw.common.Locations.CDMW_SERVICES_NAME_DOMAIN);
                                         
            // =====================================    
            // Get Event service naming context
            // =====================================
            org.omg.CORBA.Object obj = ni.resolve("EVENT");
            org.omg.CosNaming.NamingContext nc =
                org.omg.CosNaming.NamingContextHelper.narrow(obj);
             
            // =====================================================  
            // Get event channel of server and its push consummer 
            // =====================================================
            if (nc == null) {
                System.err.println("NamingContext is a nil object reference");                      
            } else {       
                cdmw.event.TypedEventChannel typedeventchannel =
                    new cdmw.event.TypedEventChannel(COMPLETE_TYPED_CHANNEL_NAME,
                                                      nc);

                try {
                    org.omg.CORBA.Object obj2 = typedeventchannel.getTypedPushConsumer(INTERFACE_REP, null);
                    pushConsumer = com.thalesgroup.TypedEchoHelper.narrow(obj2);
                } catch (org.omg.CORBA.SystemException ex) {
                    System.err.println("impossible to get a push consumer on " + COMPLETE_TYPED_CHANNEL_NAME );
                }
           }
        
        } catch (org.omg.CORBA.UserException ex) {
            System.err.println(ex.toString());
            ex.printStackTrace();        
        } catch (org.omg.CORBA.SystemException ex) {
            System.err.println(ex.toString());
            ex.printStackTrace();        
        }

    }
   
    // Note: stop() operation is final in Thread class.
    //       This stopThread() operation is equivalent 
    //       to the HelloClientThread::stop() C++ operation
    public void stopThread() {
        exitRequired = true;
    }
                             
   
    public void run() {
        try {
            System.out.println("Hello Client Thread Started");
            
            String baseMsg = "process <" + applicationName + "/" + processName + ">" ;
            
            // ===================================================
            // trace a message (use CDMW_TRACE in Cdmw thread)
            // ===================================================
            String msg = baseMsg + "Client Thread started";  
                                           
            cdmw.trace.Tracer.trace("TUTOR", 
                                     (short) 1, 
                                     msg);
            
            // ====================================================
            // thread loop
            // ====================================================
            
            while (!exitRequired) 
            {
                // ===================================================
                // Push event in Push Consumer
                // ===================================================
                if (pushConsumer != null) {   
                    pushConsumer.echo_string("<<**** HELLO ****>>");
                }
            
            
                // ===================================================
                // set thread sleeping
                // ===================================================
                try { 
                    cdmw.ossupport.OS.sleep (10000);
                } catch (java.lang.InterruptedException e) {
                    // ignore
                }
            } 
        
        } catch (org.omg.CORBA.SystemException ex) {
            System.out.println("Unexpected exception in thread loop");
            ex.printStackTrace();
        }
        
    }
       
}
   
    




