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


package cdmw.ccm.container;

import cdmw.common.Assert;
import cdmw.orbsupport.ExceptionMinorCodes;

/**
 * Implementation of the Session2Context internal interface for use with
 * Session container type.
 */
public class ValueTypeRegistrationImpl
    extends org.omg.CORBA.LocalObject
    implements com.thalesgroup.CdmwCcmContainer.ValueTypeRegistration {

    private com.thalesgroup.CdmwCcmContainer.ValueTypeInstaller installer;

    private String repid;

    /**
     * Constructor.
     *
     * @param orb A reference to the ORB pseudo-object
     */
    public ValueTypeRegistrationImpl(
        com.thalesgroup.CdmwCcmContainer.ValueTypeInstaller installer,
        String repid) {

        this.installer = installer;
        this.repid = repid;
        Assert.check(installer != null);
        try {
            ValueTypeInstallerImpl imp = (ValueTypeInstallerImpl) installer;
        } catch(ClassCastException cce) {
            throw new org.omg.CORBA.BAD_PARAM(
                ExceptionMinorCodes.BAD_PARAMWrongValuetypeInstallerType,
                org.omg.CORBA.CompletionStatus.COMPLETED_NO);
        }

    }

    //
    // IDL:thalesgroup.com/CdmwCcmContainer/ValueTypeFactoryRegistration/unregister:1.0
    //
    public void unregister() {
        // done in the destructor in C++
        try {
            ValueTypeInstallerImpl imp = (ValueTypeInstallerImpl) installer;
            imp.unregister_valuetype_factory(repid);
        } catch(ClassCastException cce) {}
    }

}

