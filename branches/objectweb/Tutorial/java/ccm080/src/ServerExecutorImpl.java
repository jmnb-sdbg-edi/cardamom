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


class ServerExecutorImpl
    extends org.omg.CORBA.LocalObject
    implements com.thalesgroup.UserHello.User_Server 
{
    
    private com.acme.Hello.CCM_Server_Context sessionContext; 

    private String name;
    
    
    public ServerExecutorImpl() {
        this.sessionContext = null;
        this.name = "";
    }
    
    
    //
    // set_session_context function : it fixes the session context of the component (call by the container)
    //
    public void set_session_context(org.omg.Components.SessionContext ctx) {
        System.out.println("===> set_session_context called!");
        
        sessionContext = com.acme.Hello.CCM_Server_ContextHelper.narrow(ctx);
    }

    
    //
    // IDL:thalesgroup.com/Hello/CCM_Client_Executor/name:1.0
    //
    public String name() {
        return name;
    }

    public void name(String a) {
        this.name = a;
    }

    
    //
    // ccm_activate function : it actives the component (call by the container)
    //
    public void ccm_activate() {
        System.out.println("===> ccm_activate called!");

        try {
            System.out.println();
            System.out.println(">>>> server name : " + name());
            System.out.println();
            
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


