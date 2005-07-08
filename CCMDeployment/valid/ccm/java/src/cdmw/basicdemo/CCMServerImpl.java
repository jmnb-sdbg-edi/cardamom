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

package cdmw.basicdemo;

//
// IDL:thalesgroup.com/BasicDemo/CCM_Server:1.0
//
public class CCMServerImpl extends CCMDisplayImpl
    implements com.acme.BasicDemo.CCM_Server {
    
    private int indice = 0;

    private String name = "";
    
    private com.acme.BasicDemo.CCM_Display display = new CCMDisplayImpl();

    public CCMServerImpl() {}

    //
    // IDL:thalesgroup.com/BasicDemo/CCM_Server/get_for_clients:1.0
    //
    public com.acme.BasicDemo.CCM_Display get_for_clients() {
        return this.display;
    }

    //
    // IDL:thalesgroup.com/BasicDemo/CCM_Server_Executor/name:1.0
    //
    public String name() {
        // TODO: Implementation
        return this.name;
    }

    public void name(String name) {
        this.name = name;
    }

    //
    // IDL:thalesgroup.com/BasicDemo/Registration/register:1.0
    //
    public String register() {
        // TODO: Implementation
        //return a string with a register number - ex: "Register 5"
        this.indice++;
        return "Register " + this.indice;
    }
    
}

