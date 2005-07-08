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

    private static final String COMPLETE_CHANNEL_NAME = "EVENT_CHANNELS/EventChannel1";
    
    private org.omg.CORBA.ORB orb;
    private String applicationName;
    private String processName;
    private org.omg.CosEventComm.PushConsumer  pushConsumer;
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
                cdmw.event.EventChannel eventchannel =
                    new cdmw.event.EventChannel(COMPLETE_CHANNEL_NAME,
                                                 nc);

                try {
                    pushConsumer = eventchannel.getPushConsumer(null);
                } catch (org.omg.CORBA.SystemException ex) {
                    System.err.println("impossible to get a push consumer on " + COMPLETE_CHANNEL_NAME );
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
   
    // Note: This stopThread() operation is equivalent 
    //       to the HelloClientThread::stop() C++ operation.
    //       (stop() operation is final in java.lang.Thread class)
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
                    org.omg.CORBA.Any data = orb.create_any();
                    data.insert_string("<<**** HELLO ****>>");
    
                    pushConsumer.push (data);
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
        
        } catch (org.omg.CORBA.UserException ex) {
            System.out.println("Unexpected exception in thread loop");
            ex.printStackTrace();
        } catch (org.omg.CORBA.SystemException ex) {
            System.out.println("Unexpected exception in thread loop");
            ex.printStackTrace();
        }
        
    }
       
}
   
    




