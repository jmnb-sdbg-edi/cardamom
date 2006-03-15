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


package cdmw.orbsupport;

public class CORBASystemExceptionWrapper extends cdmw.common.Exception {

    private org.omg.CORBA.SystemException systemException;

    public CORBASystemExceptionWrapper(
        org.omg.CORBA.SystemException se, String reason) {

        super( SAFE, se.toString() + " \n" + reason );
        this.systemException = se;

    }
    
    public CORBASystemExceptionWrapper(org.omg.CORBA.SystemException se) {

        super( SAFE, se.toString());
        this.systemException = se;

    }
            
    public void raiseSystemException() throws org.omg.CORBA.SystemException {
        throw systemException;
    }
    
    public org.omg.CORBA.SystemException getSystemException() {
        return systemException;
    }
    
    public static CORBASystemExceptionWrapper wrapSystemException(
        org.omg.CORBA.SystemException se, String reason) {
    
        return new CORBASystemExceptionWrapper(se, reason);
    
    }

}

