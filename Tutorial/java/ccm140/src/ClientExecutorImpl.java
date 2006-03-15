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

package Hello;


class ClientExecutorImpl
    extends org.omg.CORBA.LocalObject
    implements com.thalesgroup.UserHello.User_Client 
{
    
    private com.acme.Hello.CCM_Client_Context sessionContext; 
    
    private com.acme.Hello.Client helloClient;
    
    private org.omg.Components.Cookie cookie;
    

    public ClientExecutorImpl() {
        this.sessionContext = null;
        this.helloClient = null;
        this.cookie = null;
    }
    
    
    
    //
    // set_session_context function : it fixes the session context of the component (call by the container)
    //
    public void set_session_context(org.omg.Components.SessionContext ctx) {
        System.out.println("===> set_session_context called!");
        
        sessionContext = com.acme.Hello.CCM_Client_ContextHelper.narrow(ctx);
    }
    
    
    //
    // ccm_activate function : it actives the component (call by the container)
    //
    public void ccm_activate() {
        System.out.println("===> ccm_activate called!");

        try {
            // ========================================================
            // Get the naming and repository
            // ========================================================
            System.out.println("Get the repository...");
            com.thalesgroup.CdmwNamingAndRepository.Repository repository = 
                cdmw.namingandrepository.RepositoryInterface.getRepository();
                       
                       
            // ======================================================  
            // get the default root context
            // ======================================================
            org.omg.CosNaming.NamingContext defRootContext =
                repository.resolve_root_context(
                    com.thalesgroup.CdmwNamingAndRepository.DEFAULT_ROOT_CONTEXT.value);

            // ======================================================                      
            // create a naming interface on default root context
            // ====================================================== 
            cdmw.commonsvcs.naming.NamingInterface niDefRoot =
                new cdmw.commonsvcs.naming.NamingInterface(defRootContext);
            
            
            // ========================================================
            // get server home from name service
            // ======================================================== 
            System.out.println("Get the server home reference...");
            org.omg.CORBA.Object objref1 = 
                niDefRoot.resolve ("MyNaming/Tutorial/HelloServerHome");

            com.acme.Hello.ServerHome helloServerHome = 
                com.acme.Hello.ServerHomeHelper.narrow(objref1);
                 
            // ========================================================
            // get client component from name service
            // ========================================================
            System.out.println("Get the client reference..."); 
            org.omg.CORBA.Object objref2 = niDefRoot.resolve ("MyNaming/Tutorial/HelloClient");
    
            helloClient = com.acme.Hello.ClientHelper.narrow(objref2);
            
	    
	    
	    
            // ========================================================
            // create a server instance using server home
            // ======================================================== 
            System.out.println("Create the server...");
            com.acme.Hello.Server helloServer = helloServerHome.create();
            
            
            // ========================================================
            // Get the server event sink
            // ========================================================
            System.out.println("Get the server event sink...");
            com.acme.Hello.StatusInfoConsumer consumer = 
                helloServer.get_consumer_info();
            
            // ========================================================
            // Subscribe to server consumer
            // ========================================================
            System.out.println("Subscribe to the server...");
            org.omg.Components.Cookie cookie = 
                helloClient.subscribe_info (consumer);
            
        
            // ===============================================================
            // push the status info event to the server using the "info" port
            // ===============================================================    
            System.out.println();
            System.out.println(">>>> Push the status info event ...");
        
            // create the status info event
            com.acme.Hello.StatusInfo ev = new StatusInfoImpl();
            ev.text = ">>>>--- Hello from client ---<<<<";
        
            // then push it
            sessionContext.push_info(ev);
            
        } catch (org.omg.CORBA.UserException e) {
             System.out.println("CORBA User Exception : \n" +
                                e.toString());
                                
        } catch (org.omg.CORBA.SystemException e) {
             System.out.println("CORBA System Exception : \n" +
                                e.toString());
                                
        } catch (Exception e) {
             System.out.println("Unexpected exception raised!\n" +
                                e.toString());
        }
    }
    
    
    //
    // ccm_passivate function : it makes stop the component (call by the container)
    //
    public void ccm_passivate() {
        System.out.println("===> ccm_passivate called!");

        try { 
            // ========================================================
            // Unsubscribe the server consumer
            // ========================================================
            System.out.println("Unsubscribe the server...");
            helloClient.unsubscribe_info(cookie);
            
        } catch (Exception e) {
             System.out.println("Unexpected exception raised!\n" +
                                e.toString());
        }
    }
    
    
    //
    // ccm_remove function : it destroys the component (call by the container)
    //
    public void ccm_remove() {
        System.out.println("===> ccm_remove called!");
    }
  
    
}


