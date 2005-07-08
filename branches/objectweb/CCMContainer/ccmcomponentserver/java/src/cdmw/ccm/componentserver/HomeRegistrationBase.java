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


package cdmw.ccm.componentserver;

import cdmw.orbsupport.ExceptionMinorCodes;

/**
 * The HomeRegistration acts as a singleton factory for the creation
 * of Container objects and is used by an Assembly object
 * during the deployment process.
 *
 */
public abstract class HomeRegistrationBase extends org.omg.CORBA.LocalObject
    implements org.omg.Components.HomeRegistration {

    private java.util.Hashtable registeredHomes = new java.util.Hashtable();

    //
    // IDL:omg.org/Components/HomeRegistration/register_home:1.0
    //
    public void register_home(org.omg.Components.CCMHome homeRef,
                               String homeName) {

        if ( bind(homeRef, homeName) ) {
            registeredHomes.put(homeName, homeRef);
        }

    }

    //
    // IDL:omg.org/Components/HomeRegistration/unregister_home:1.0
    //
    public void unregister_home(org.omg.Components.CCMHome homeRef) {

        java.util.Enumeration keys = registeredHomes.keys();
        for( ; keys.hasMoreElements(); ) {
            String homeName = (String) keys.nextElement();
            org.omg.Components.CCMHome home =
                (org.omg.Components.CCMHome) registeredHomes.get(homeName);
            if (homeRef._is_equivalent(home)) {
                unbind(homeName);
                registeredHomes.remove(homeName);
                return;
            }
        }
        throw new org.omg.CORBA.BAD_PARAM(
            ExceptionMinorCodes.BAD_PARAMHomeNotRegistered,
            org.omg.CORBA.CompletionStatus.COMPLETED_NO);

    }

    protected abstract boolean
        bind(org.omg.Components.CCMHome homeRef, String homeName);

    protected abstract boolean unbind(String homeName);

}

