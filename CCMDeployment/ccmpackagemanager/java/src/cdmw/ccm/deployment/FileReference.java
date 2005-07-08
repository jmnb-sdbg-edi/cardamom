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


package cdmw.ccm.deployment;

import cdmw.ccm.deployment.FileUtilities;

import org.omg.Components.Deployment.InstallationFailure;
import com.thalesgroup.CdmwDeployment.FILE_ALREADY_INSTALLED;
import com.thalesgroup.CdmwDeployment.FILE_NOT_INSTALLED;

/**
 * This abstract class represents a file reference in a package descriptor.
 * This is the base class for all representations of <link>, <codebase> and 
 * <fileinarchive> references.<br>
 * FileReference defines abstract methods for installation of the referenced file.
 */
public abstract class FileReference {

    /**
     * The location of the installed file.
     */
    protected java.io.File actualLocation;


    /**
     * Returns the location of the installed file.
     *
     * @return  the location of the installed file.
     *
     * @throws  InstallationFailure 
     *          FILE_NOT_INSTALLED: raised if the file is not installed.
     */
    public String getActualLocation()
        throws InstallationFailure
    {
        if (! isInstalled()) {
            throw new InstallationFailure(
                "File '" + toString() + "' not installed.",
                FILE_NOT_INSTALLED.value);
        }
        return actualLocation.getAbsolutePath();
    }

    /**
     * Set the location of the installed file.
     *
     * @param  actualLocation  the location of the installed file.
     *
     * @throws  InstallationFailure
     *          FILE_ALREADY_INSTALLED: raised if the file was already installed.
     *          FILE_NOT_INSTALLED: raised if the specified location doesn't 
     *          correspond to an existing file.
     */
    public void setActualLocation(java.io.File actualLocation)
        throws InstallationFailure
    {
        if (isInstalled()) {
            throw new InstallationFailure(
                "File already installed at " +
                 actualLocation.toString(),
                 FILE_ALREADY_INSTALLED.value);
        } else {
            if (!actualLocation.exists()) {
                throw new InstallationFailure(
                    "File '" + actualLocation.toString() + "' doesn't exist.",
                    FILE_NOT_INSTALLED.value);
            } else {
                this.actualLocation = actualLocation;
            }
        }
    }

    /**
     * Returns the basename of the installed file.
     *
     * @return  the basename  of the installed file.
     * 
     * @throws  InstallationFailure
     *          FILE_NOT_INSTALLED: raised if the file is not installed.
     */
    public String getActualBasename()
        throws InstallationFailure
    {
        if (! isInstalled()) {
            throw new InstallationFailure(
                "File '" + toString() + "' not installed.",
                FILE_NOT_INSTALLED.value);
        }
        return actualLocation.getName();
    }


    /**
     * Returns true if the referenced file is installed.
     *
     * @return  <code>true</code> if the file is installed.
     */
    public boolean isInstalled() {
        return (actualLocation != null);
    }


    /**
     * Installs the referenced file into the specified directory.
     *
     * @param  directory  the directory into which the file should be installed.
     *
     * @throws  InstallationFailure  
     *          raised if an error occurs durring installation.
     */
    public void installInto(String directory)
        throws InstallationFailure
    {
        installInto(new java.io.File(directory));
    }

    /**
     * Installs the referenced file into the specified directory.
     *
     * @param  directory  the directory into which the file should be installed.
     *
     * @throws  InstallationFailure 
     *          raised if an error occurs durring installation.
     */
    public abstract void installInto(java.io.File directory)
        throws InstallationFailure;


    /**
     * Installs the referenced file at the specified location.
     *
     * @param  newLocation  the path of the installed file.
     *
     * @throws  InstallationFailure 
     *          raised if an error occurs durring installation.
     */
    public void installAs(String newLocation)
        throws InstallationFailure
    {
        installAs(new java.io.File(newLocation));
    }

    /**
     * Installs the referenced file at the specified location.
     *
     * @param  newLocation  the path of the installed file.
     *
     * @throws  InstallationFailure 
     *          raised if an error occurs durring installation.
     */
    public abstract void installAs(java.io.File newLocation)
        throws InstallationFailure;


    /**
     * Constructs a string representation of this FileReference.
     *
     * @return a string representation of this object.
     */    
    public abstract String toString();

    /**
     * Moves the installed file as the specified location.
     *
     * @param  newLocation  the location of the moved file.
     *
     * @throws  IOException
     *          raised if the move failed
     * @throws  InstallationFailure
     *          FILE_NOT_INSTALLED: raised if file is not installed.
     */    
    public void moveFileTo(String newLocation) 
        throws java.io.IOException,
               InstallationFailure
    {
        moveFileTo(new java.io.File(newLocation));
    }


    /**
     * Moves the installed file as the specified location.
     *
     * @param  newLocation  the location of the moved file.
     *
     * @throws  IOException
     *          raised if the move failed
     * @throws  InstallationFailure
     *          FILE_NOT_INSTALLED: raised if file is not installed.
     */    
    public void moveFileTo(java.io.File newLocation) 
        throws java.io.IOException,
               InstallationFailure
    {
        // move file only if it is installed
        if (!isInstalled()) {
            throw new InstallationFailure(
                "File '" + toString() + "' not installed.",
                FILE_NOT_INSTALLED.value);                
        }
        
        // if new location is a directory, move file into directory
        if (newLocation.isDirectory()) {
            newLocation = new java.io.File(newLocation, actualLocation.getName());
        }

        // try to move file
        FileUtilities.moveFileTo(actualLocation, newLocation);

        // if renaming success, update actualLocation
        actualLocation = newLocation;
    }       
        
        
    /**
     * Deletes the installed file.
     *
     * @return  <code>true</code> if the file was successfull deleted, 
     *          <code>false</code> otherwise.
     */    
    public boolean delete() {
        boolean ret = false;

        // TODO: how to guarantee that a file is
        // not used or will not be used elsewhere ?

        if (isInstalled()) {
            ret = actualLocation.delete();
            actualLocation = null;
        }
        return ret;
    }



}



