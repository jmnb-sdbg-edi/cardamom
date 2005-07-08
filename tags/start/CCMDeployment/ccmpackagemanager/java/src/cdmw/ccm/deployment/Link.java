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

import org.omg.Components.Deployment.InstallationFailure;
import com.thalesgroup.CdmwDeployment.READ_WRITE_ERROR;
import com.thalesgroup.CdmwDeployment.FILE_ALREADY_INSTALLED;

import cdmw.ccm.deployment.FileUtilities;


public class Link extends FileReference {


    protected java.net.URL sourceLink;


    public Link(String sourceLink)
        throws cdmw.common.BadParameterException
    {
        this.sourceLink = FileUtilities.toURL(sourceLink);
        this.actualLocation = null;
    }

    public String getSourceLink() {
        return sourceLink.toString();
    }

    public void installInto(java.io.File directory)
        throws InstallationFailure
    {
        try {
            FileUtilities.checkDirectory(directory);
        } catch (java.io.IOException e) {
            throw new InstallationFailure(e.getMessage(),
                                          READ_WRITE_ERROR.value);
        }

        // retrieve name of the file denoted by the sourceLink.
        java.io.File urlPath = new java.io.File(sourceLink.getPath());
        installAs(new java.io.File(directory, urlPath.getName()));
    }


    public void installAs(java.io.File newLocation)
        throws InstallationFailure
    {
        // Check if link if already installed
        if (isInstalled()) {
            throw new InstallationFailure(
                "Link " + sourceLink.toString() +
                " is already installed at " + actualLocation.getPath(),
                FILE_ALREADY_INSTALLED.value);
        }

        // Check if target file already extists
        if (newLocation.exists()) {
            throw new InstallationFailure(
                newLocation.toString() + " already exists.",
                READ_WRITE_ERROR.value);
        }

        // Download link
        try {
            cdmw.ccm.deployment.FileUtilities.downloadFile(sourceLink,
                                                               newLocation.getAbsolutePath());
            actualLocation = newLocation;
            
        } catch (cdmw.common.BadParameterException e) {
            // error in sourceLink
            throw new InstallationFailure(
                "Download of " + sourceLink.toString() + " failed :\n" +
                e.getMessage(),
                READ_WRITE_ERROR.value);
                
        } catch (java.io.IOException e) {
            // error durring download
            throw new InstallationFailure(
                "Download of " + sourceLink.toString() + " to " + 
                newLocation.getPath() + " failed :\n" + e.getMessage(),
                READ_WRITE_ERROR.value);
        }
    }


    public String toString() {
        return "<link href=\"" + sourceLink.toString() + "\">";
    }



}



