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

    public ClientExecutorImpl() {
        this.sessionContext = null;
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
            cdmw.namingandrepository.NamingInterface niDefRoot =
                new cdmw.namingandrepository.NamingInterface(defRootContext);
            
            
            
            
            // ========================================================
            // get server component from name service
            // ======================================================== 
            System.out.println("Get the server reference...");
            org.omg.CORBA.Object objref1 = niDefRoot.resolve ("MyNaming/Tutorial/HelloServer");
    
            com.acme.Hello.Server helloServer = 
                com.acme.Hello.ServerHelper.narrow(objref1);
                 
            // ========================================================
            // get client component from name service
            // ========================================================
            System.out.println("Get the client reference..."); 
            org.omg.CORBA.Object objref2 = niDefRoot.resolve ("MyNaming/Tutorial/HelloClient");
    
            com.acme.Hello.Client helloClient = 
                com.acme.Hello.ClientHelper.narrow(objref2);
            
            
            
            // ========================================================
            // Get the server event sink
            // ========================================================
            System.out.println("Get the server event sink...");
            com.acme.Hello.HelloInfoConsumer consumer = 
                helloServer.get_consumer_received_info();
            
            // ========================================================
            // Subscribe to server consumer
            // ========================================================
            System.out.println("Subscribe to the server...");
            org.omg.Components.Cookie cookie = 
                helloClient.subscribe_info_to_send (consumer);
            
        
            // ===============================================================
            // push the hello info event to the server using the "info" port
            // ===============================================================    
            System.out.println();
            System.out.println(">>>> Push the hello info event ...");
        
            // create the hello info event
            com.acme.Hello.HelloInfo ev = new HelloInfoImpl();
            ev.text = ">>>>--- Hello from client ---<<<<";
        
            // then push it
            sessionContext.push_info_to_send(ev);
            
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
    }
    
    
    //
    // ccm_remove function : it destroys the component (call by the container)
    //
    public void ccm_remove() {
        System.out.println("===> ccm_remove called!");
    }
  
    
}


