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
    
    com.acme.Hello.CCM_Client_Context sessionContext;
    
    private ClientActivationThread activationThread;
    private boolean isActive;
    

    public ClientExecutorImpl() {
        this.sessionContext = null;
        this.isActive = false;
        this.activationThread = new ClientActivationThread(this);
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

        // if thread not yet started, start it
        if (! isActive)
        {
            System.out.println("===> start activation thread!");
            
            isActive = true;
          
            activationThread.start();
        }
    }
    
    
    //
    // ccm_passivate function : it makes stop the component (call by the container)
    //
    public void ccm_passivate() {
        System.out.println("===> ccm_passivate called!");
        
       if (isActive) {
           // thread has been started so stop it then unlock mutex
           isActive = false;
    
           // join the thread
           try {
               activationThread.join();
               
               System.out.println("===> thread has join");
           } catch (Exception e) {
               System.out.println("Unexpected exception raised!\n" +
                                  e.toString());
           }
        }
    }
    
    
    //
    // ccm_remove function : it destroys the component (call by the container)
    //
    public void ccm_remove() {
        System.out.println("===> ccm_remove called!");
    }
  

    //
    // IDL:thalesgroup.com/Hello/CCM_Server/push_received_info:1.0
    //
    public void push_received_info(com.acme.Hello.HelloInfo event) {
        System.out.println("===> push_received_info called!");
        
        event.display_text();
    }
    
}


