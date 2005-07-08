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


class DisplayFacetExecutorImpl
    extends org.omg.CORBA.LocalObject
    implements com.acme.Hello.CCM_Display 
{
    
    private com.acme.Hello.CCM_Server_Context serverSessionContext;
    
    
    public DisplayFacetExecutorImpl() {
        serverSessionContext = null;
    }
    
    
    public void setServerSessionContext(com.acme.Hello.CCM_Server_Context serverCtx) {
        serverSessionContext = serverCtx;
    }
    
    
    
    //
    // IDL:acme.com/Hello/CCM_Display/display_hello:1.0
    //
    public void display_hello() {
        System.out.println();
        System.out.println("<------------------------------>");
        System.out.println("<############ HELLO ###########>");
        System.out.println("<------------------------------>");
        System.out.println();

        try {
            // ===============================================================
            // push the hello info event to the client using the "info" port
            // ===============================================================    
            System.out.println();
            System.out.println(">>>> Push the hello info event ...");
        
            // create the hello info event
            com.acme.Hello.HelloInfo ev = new HelloInfoImpl();
            ev.text = ">>>>--- Hello has been received ---<<<<";
        
            // then push it
            serverSessionContext.push_info_to_send(ev);
       
        } catch (Exception e) {
             System.out.println("Unexpected exception raised!\n" +
                                e.toString());
        }
    }
    
}


