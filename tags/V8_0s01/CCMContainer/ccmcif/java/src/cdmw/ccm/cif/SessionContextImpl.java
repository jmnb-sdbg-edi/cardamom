/* ===================================================================== */
/*
 * This file is part of CARDAMOM (R) which is jointly developed by THALES
 * and SELEX-SI. It is derivative work based on PERCO Copyright (C) THALES
 * 2000-2003. All rights reserved.
 * 
 * Copyright (C) THALES 2004-2005. All rights reserved
 * 
 * CARDAMOM is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * CARDAMOM is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public
 * License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with CARDAMOM; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/* ===================================================================== */


package cdmw.ccm.cif;

import cdmw.orbsupport.ExceptionMinorCodes;

/**
 * This class implements the Components::SessionContext interface.
 * It will be inherited by all session components context implementation.
 *
 */
//
// IDL:omg.org/Components/SessionContext:1.0
//
public class SessionContextImpl extends CCMContextImpl 
    implements org.omg.Components.SessionContext {

    /**
     * session context given by the container
     */
    private org.omg.Components.SessionContext sessionContext;

    public SessionContextImpl(org.omg.Components.CCMContext ctx) {
        super(ctx);
        this.sessionContext = 
            org.omg.Components.SessionContextHelper.narrow(ctx);

        if (this.sessionContext == null) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMWrongContextType, 
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }
    }

    //
    // IDL:omg.org/Components/SessionContext/get_CCM_object:1.0
    //
    public org.omg.CORBA.Object get_CCM_object() 
        throws org.omg.Components.IllegalState {
        return this.sessionContext.get_CCM_object();
    }

}

