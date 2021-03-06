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


package cdmw.resources;

import cdmw.lifecycle.ServantProviderBase;

import com.thalesgroup.CF.DataType;
import com.thalesgroup.CF.InvalidResourceNumber;
import com.thalesgroup.CF.InvalidProperties;
import com.thalesgroup.CF.CreationError;
import com.thalesgroup.CF.CreateApplicationError;
import com.thalesgroup.CF.DevicePackage.InvalidLocation;

/**
 * This class provides abstract methods to be implemented by the user.
 *
 */
public abstract class ResourceServantProvider extends ServantProviderBase {

    /**
     * IDs
     */
    private final static String[] ids = {
        "IDL:omg.org/CosLifeCycle/LifeCycleObject:1.0",
        "IDL:thalesgroup.com/CF/Resource:1.0",
        "IDL:thalesgroup.com/CF/Device:1.0",
        "IDL:thalesgroup.com/CF/Application:1.0"
    };

    /**
     * Method createResource.
     * @param resourceNumber
     * @param qualifiers
     * @return byte[]
     * @throws InvalidResourceNumber
     * @throws InvalidProperties
     * @throws CreationError
     */
    public abstract byte[] createResource(
        short resourceNumber,
        DataType[] qualifiers)
        throws InvalidResourceNumber, InvalidProperties, CreationError;

    /**
     * Method createDevice.
     * @param label
     * @return byte[]
     * @throws InvalidLocation
     */
    public abstract byte[] createDevice(String label) throws InvalidLocation;

    /**
     * Method createApplication.
     * @param name
     * @return byte[]
     * @throws CreateApplicationError
     */
    public abstract byte[] createApplication(String name)
        throws CreateApplicationError;

    /**
     * Returns the list od IDs
     *
     * @return The list of IDs as a string table
     */
    protected String[] getIds() {
        return ids;
    }

    /**
     * Returns the number od IDs
     *
     * @return The number of IDs
     */
    protected int getNumberOfIds() {
        return ids.length;
    }

}

