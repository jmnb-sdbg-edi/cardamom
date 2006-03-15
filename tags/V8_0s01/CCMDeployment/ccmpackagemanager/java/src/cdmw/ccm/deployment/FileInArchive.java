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


package cdmw.ccm.deployment;

import org.omg.Components.Deployment.InstallationFailure;
import com.thalesgroup.CdmwDeployment.READ_WRITE_ERROR;
import com.thalesgroup.CdmwDeployment.FILE_ALREADY_INSTALLED;
import com.thalesgroup.CdmwDeployment.FILE_NOT_INSTALLED;

import cdmw.ccm.deployment.FileUtilities;

/**
 * This class represents an fileinarchive reference in a package descriptor.
 *
 */
public class FileInArchive extends FileReference {

    /**
     * The entry in archive.
     */
    private String archiveEntry;

    /**
     * The archive in which the file is.
     */
    private Archive archive;

    /**
     * Constructor.
     *
     * @param archiveEntry the entry in archive.
     * @param archive the archive in which the file is.
     */
    public FileInArchive(String archiveEntry,
                         Archive archive)
    {
        cdmw.common.Assert.check(archiveEntry != null);
        this.archiveEntry = archiveEntry;

        cdmw.common.Assert.check(archive != null);
        this.archive = archive;

        this.actualLocation = null;
    }

    /**
     * Returns the corresponding entry in archive.
     *
     * @return the entry in archive.
     */
    public String getArchiveEntry() {
        return archiveEntry;
    }

    /**
     * Returns the Archive in which the file is.
     *
     * @param the archive in which the file is.
     */
    public Archive getArchive() {
        return archive;
    }

    /**
     * Install the file in archive into the specified directory.
     *
     * @param  directory  the directory into which the file will be installed.
     *
     * @throws  InstallationFailure 
     *          READ_WRITE_ERROR: raised if specified directory is not a 
     *          directory, if the file could not be extracted or written.<br>
     *          FILE_ALREADY_INSTALLED: raised if file is already installed.<br>
     *          FILE_NOT_INSTALLED: raised if archive in which the file is 
     *          has not been installed.
     */
    public void installInto(java.io.File directory)
        throws InstallationFailure
    {
        try {
            FileUtilities.checkDirectory(directory);
        } catch (java.io.IOException e) {
            throw new InstallationFailure(e.getMessage(),
                                          READ_WRITE_ERROR.value);
        }

        java.io.File newFile = new java.io.File(
            directory, archiveEntry.replace('/', java.io.File.separatorChar));
        installAs(newFile);
    }

    /**
     * Install the file in archive as the speficied newLocation.
     *
     * @param  newLocation  the new path of the installed file.
     *
     * @throws  InstallationFailure 
     *          READ_WRITE_ERROR: raised if the file could not 
     *          be extracted or written.<br>
     *          FILE_ALREADY_INSTALLED: raised if file is already installed.<br>
     *          FILE_NOT_INSTALLED: raised if archive in which the file is 
     *          has not been installed.
     */
    public void installAs(java.io.File newLocation)
        throws InstallationFailure
    {
        // Check if link if already installed
        if (isInstalled()) {
            throw new InstallationFailure(
                "FileInArchive " + archiveEntry +
                " is already installed at " + actualLocation.getPath(),
                FILE_ALREADY_INSTALLED.value);
        }

        // Check if target file already exist
        if (newLocation.exists()) {
            throw new InstallationFailure(
                newLocation + " already exists.",
                READ_WRITE_ERROR.value);
        }

        // Check if archive is installed
        if (!archive.isInstalled()) {
            throw new InstallationFailure(
                archive.toString() + " needs to be installed first.",
                FILE_NOT_INSTALLED.value);
        }

        // Check if archive is a zip
        if (!cdmw.ccm.deployment.FileUtilities.isZip(archive.getActualLocation())) {
            throw new InstallationFailure(
                archive.getActualLocation() + " is not a zip file. ",
                READ_WRITE_ERROR.value);
        }

        // Extract file
        try {
            cdmw.ccm.deployment.FileUtilities.unzipFileAs(archiveEntry,
                                                              archive.getActualLocation(),
                                                              newLocation.getAbsolutePath());
            actualLocation = newLocation;
        } catch (java.io.IOException e) {
            throw new InstallationFailure(
                "Extraction of " + archiveEntry +
                " from " + archive.getActualLocation() + " failed." +
                " (" + e.getMessage() + ")",
                READ_WRITE_ERROR.value);
        }
    }


    /**
     * Constructs a string representation of this FileInArchive.
     * The string is a representation of the equivalent 
     * <fileinarchive> XML element.
     *
     * @return a string representation of this object.
     */
    public String toString() {
        return "<fileinarchive name=\"" + archiveEntry + "\">" +
               archive.toString() + "</fileinarchive>";
    }

}


