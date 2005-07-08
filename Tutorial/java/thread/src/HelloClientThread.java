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

    private org.omg.CORBA.ORB orb;
    private com.thalesgroup.CdmwNamingAndRepository.Repository repository;
    private String applicationName;
    private String processName;
    private com.thalesgroup.Hello.HelloInterface helloInterface;
    private boolean exitRequired;
    
    
    public HelloClientThread(org.omg.CORBA.ORB orb,
                             com.thalesgroup.CdmwNamingAndRepository.Repository repository,
                             String applicationName,
                             String processName)
    {
        this.orb = orb;
        this.repository = repository;
        this.applicationName = applicationName;
        this.processName = processName;
        this.helloInterface = null;
        this.exitRequired = false;
      
        try  {      
      
            // ========================================================
            // Get the Default Root context from Repository
            // ========================================================
            org.omg.CORBA.Object obj2 = 
                repository.resolve_root_context (
                    com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);
            org.omg.CosNaming.NamingContextExt rootCtx = 
                org.omg.CosNaming.NamingContextExtHelper.narrow(obj2);
                                   
                                   
            // ========================================================
            // Get the Factory Finder from Default Root Context
            // ========================================================
            org.omg.CORBA.Object obj3 = rootCtx.resolve_str (
                com.thalesgroup.CdmwNamingAndRepository.FACTORY_FINDER.value);
            com.thalesgroup.CdmwLifeCycle.FactoryFinder factoryFinder = 
                com.thalesgroup.CdmwLifeCycle.FactoryFinderHelper.narrow(obj3);      
        
            // ======================================================
            // find the HelloInterface factory object from repository
            // ======================================================
            com.thalesgroup.CdmwLifeCycle.FactoryBase factory = 
                factoryFinder.find_factory (
                    "Hello.Domain/Hello.Factories/HelloEntity.factory");
                            
            com.thalesgroup.HelloEntityFactoryModule.HelloEntityFactory helloFactory = 
                 com.thalesgroup.HelloEntityFactoryModule.HelloEntityFactoryHelper.narrow(factory);
    
            // ===================================================
            // get the HelloInterface proxy from the factory
            // ===================================================
            helloInterface = helloFactory.create_HelloInterface ("HelloInterface_001",
                                                                 "Hello_001");      
            
            if (helloInterface == null) {
                System.out.println ("could not narrow object to type HelloInterface");
                return;
            }            
        
        } catch (org.omg.CORBA.UserException ex) {
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
            // Call displayHello method of HelloInterface object
            // ===================================================
            if (helloInterface != null) {
                helloInterface.display_hello();
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

    }
       
}
   
    




