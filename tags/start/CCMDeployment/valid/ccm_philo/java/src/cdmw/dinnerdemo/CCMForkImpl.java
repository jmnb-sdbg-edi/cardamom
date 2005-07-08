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

package cdmw.dinnerdemo;

import cdmw.common.Assert;

//
// IDL:acme.com/Dinner/CCM_Fork:1.0
//
public class CCMForkImpl extends org.omg.CORBA.LocalObject
    implements com.acme.Dinner.CCM_Fork {

    private boolean inuse = false;
    
    public CCMForkImpl() {}
    
    //
    // IDL:acme.com/Dinner/Fork/get:1.0
    //
    public void get() throws com.acme.Dinner.InUse {
        if (this.inuse) {
            throw new com.acme.Dinner.InUse();
        }
        this.inuse = true;
    }

    //
    // IDL:acme.com/Dinner/Fork/release:1.0
    //
    public void release() {
        Assert.check(inuse);
        this.inuse = false;
    }

}

