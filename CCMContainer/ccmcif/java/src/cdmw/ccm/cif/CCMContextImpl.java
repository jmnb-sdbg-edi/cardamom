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


package cdmw.ccm.cif;

/**
 * This class implement the org.omg.Components.CCMContext interface.
 * It will be inherited by all components context implementation.
 *
 */
public class CCMContextImpl
    extends org.omg.CORBA.LocalObject
    implements org.omg.Components.CCMContext {

    /*
     * container context
     */
    private org.omg.Components.CCMContext context;

    public CCMContextImpl(org.omg.Components.CCMContext ctx) {
        this.context = ctx;
    }

    //
    // IDL:omg.org/Components/CCMContext/get_caller_principal:1.0
    //
    public org.omg.SecurityLevel2.Credentials get_caller_principal() {
        return this.context.get_caller_principal();
    }

    //
    // IDL:omg.org/Components/CCMContext/get_CCM_home:1.0
    //
    public org.omg.Components.CCMHome get_CCM_home() {
        return this.context.get_CCM_home();
    }

    //
    // IDL:omg.org/Components/CCMContext/get_rollback_only:1.0
    //
    public boolean get_rollback_only() 
        throws org.omg.Components.IllegalState {
        return this.context.get_rollback_only();
    }

    //
    // IDL:omg.org/Components/CCMContext/get_user_transaction:1.0
    //
    public org.omg.Components.Transaction.UserTransaction 
        get_user_transaction() throws org.omg.Components.IllegalState {
        return this.context.get_user_transaction();
    }

    //
    // IDL:omg.org/Components/CCMContext/is_caller_in_role:1.0
    //
    public boolean is_caller_in_role(String role) {
        return this.context.is_caller_in_role(role);
    }

    //
    // IDL:omg.org/Components/CCMContext/set_rollback_only:1.0
    //
    public void set_rollback_only() 
        throws org.omg.Components.IllegalState {
        this.context.set_rollback_only();
    }

}

