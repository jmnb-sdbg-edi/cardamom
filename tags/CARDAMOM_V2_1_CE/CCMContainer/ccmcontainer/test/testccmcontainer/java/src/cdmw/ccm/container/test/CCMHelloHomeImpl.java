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


package cdmw.ccm.container.test;

public class CCMHelloHomeImpl
    extends org.omg.CORBA.LocalObject
    implements com.thalesgroup.HelloModule.CCM_HelloHome {

    /**
     *  Constructor
     */
    public CCMHelloHomeImpl() {
        System.out.println("CCMHelloHomeImpl constructor created.");
    }

    //
    // IDL:thalesgroup.com/HelloModule/CCM_HelloHomeImplicit/create:1.0
    //
    public org.omg.Components.EnterpriseComponent create() {
        System.out.println("CCMHelloHomeImpl.create() called.");
        return null;
    }

    public static
        org.omg.Components.HomeExecutorBase createCCMHelloHomeImpl() {
        return new CCMHelloHomeImpl();
    }

}

