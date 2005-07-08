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
import cdmw.lifecycle.LifeCycleSingleObject;
import cdmw.ossupport.OS;

import com.thalesgroup.CF.ApplicationOperations;
import com.thalesgroup.CF.ResourcePackage.StartError;
import com.thalesgroup.CF.ResourcePackage.StopError;

import org.omg.CORBA.SystemException;
import org.omg.CosLifeCycle.LifeCycleObject;
import org.omg.CosLifeCycle.FactoryFinder;
import org.omg.CosLifeCycle.NVP;

import org.omg.CosLifeCycle.NoFactory;
import org.omg.CosLifeCycle.NotCopyable;
import org.omg.CosLifeCycle.NotMovable;
import org.omg.CosLifeCycle.InvalidCriteria;
import org.omg.CosLifeCycle.CannotMeetCriteria;
import org.omg.CosLifeCycle.NotRemovable;

/**
 * This class provides an implementation of the Application interface.
 *
 */
public class ApplicationImpl extends LifeCycleSingleObject 
    implements ApplicationOperations {

    /**
     * Name of the application
     */
    private String name;

    /**
     * State of the application
     */
    private boolean started;

    /**
     * Process reference
     */
    private Process p;

    /**
     * Constructor
     *
     * @param objD Object deactivator
     * @param applicationName Name of the application
     */
    public ApplicationImpl(ObjectDeactivator objD, String applicationName) {

        super(objD);
        this.name = applicationName;
        this.started = false;
        this.p = null;

    }

    //
    // IDL:thalesgroup.com/CF/Application/start:1.0
    //
    /**
     * Starts process.
     *
     * @exception StartError Error during start
     */
    public void start() throws StartError {

        if (started) {
            throw new StartError("Application already started!");
        } else {
            try {
                //p = OS.createProcess(name,null,null);
                //System.out.println("createProcess(" + name + ")");
                System.out.println("Starting <" + name + ">");
                started = true;
            } catch (Exception e) {
                throw new StartError("Could not start " + name + "!");
            }
        }

    }

    //
    // IDL:thalesgroup.com/CF/Application/stop:1.0
    //
    /**
     * Kills already started process and deactivates current object.
     *
     * @exception StopError Error during stop
     */
    public void stop() throws StopError {
        if (started) {
            try {
                //OS.killProcess(p);
                //System.out.println("killProcess " + name + ".");
                System.out.println("Stopping <" + name + ">");
                started = false;
                deactivateObject();
            } catch (Exception e) {
                throw new StopError("Could not stop " + name + "!");
            }
        } else {
            throw new StopError("Application not started!");
        }
    }

    //
    // IDL:thalesgroup.com/CF/Application/name:1.0
    //
    /**
     * Returns the name of the application.
     *
     * @return Name of the application.
     */
    public String name() {
        return name;
    }

}

