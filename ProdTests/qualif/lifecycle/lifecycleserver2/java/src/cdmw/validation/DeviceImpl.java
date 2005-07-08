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

package cdmw.resources;

import cdmw.lifecycle.ObjectDeactivator;

import com.thalesgroup.CF.DevicePOA;
import com.thalesgroup.CF.DevicePackage.UsageType;

import com.thalesgroup.CF.ResourcePackage.StartError;
import com.thalesgroup.CF.ResourcePackage.StopError;

import org.omg.PortableServer.POA;

/**
 * This class provides an implementation of the Device interface.
 *
 */
public class DeviceImpl extends DevicePOA {

    /**
     * POA reference
     */
    private POA poa;

    /**
     * Deactivator of the device
     */
    private ObjectDeactivator objectDeactivator;

    /**
     * State of the device
     */
    private UsageType state;

    /**
     * Location of the device
     */
    private String location;

    /**
     * Constructor
     *
     * @param poa
     * @param deactivator
     * @param state
     * @param location
     */
    public DeviceImpl(POA poa, ObjectDeactivator deactivator,
        UsageType state, String location) {

        this.poa = poa;
        this.objectDeactivator = deactivator;
        this.state = state;
          this.location = location;

    }

    /**
     * Returns the default POA.
     *
     * @return Default POA.
     */
    public POA _default_POA() {
        return poa;
    }

    //
    // IDL:thalesgroup.com/CF/Device/start:1.0
    //
    /**
     * Transition from IDLE to ACTIVE.
     *
     * @exception StartError Error during start
     */
    public void start() throws StartError {
        if (state.equals(UsageType.IDLE)) {
            state = UsageType.ACTIVE;
        } else if ( state.equals(UsageType.ACTIVE) ) {
            throw new StartError("Device active!");
        } else {
            throw new StartError("Device busy!");
        }
    }

    //
    // IDL:thalesgroup.com/CF/Device/stop:1.0
    //
    /**
     * Transition from ACTIVE to IDLE deactivates current object.
     *
     * @exception StopError Error during stop
     */
    public void stop() throws StopError {
        if (state.equals(UsageType.IDLE)) {
            throw new StopError("Device not active!");
        } else if (state.equals(UsageType.ACTIVE)) {
            state = UsageType.IDLE;
            try {
                objectDeactivator.deactivateObject();
            } catch (Exception e) {}
        } else {
            throw new StopError("Device busy!");
        }
    }

    //
    // IDL:thalesgroup.com/CF/Device/state:1.0
    //
    /**
     * Return the state of the device.
     *
     * @return The state of the device.
     */
    public UsageType state() {
        return state;
    }

    //
    // IDL:thalesgroup.com/CF/Device/label:1.0
    //
    /**
     * Returns the label of the device.
     *
     * @return The label of the device
     */
    public String label() {
        return location;
    }

}

