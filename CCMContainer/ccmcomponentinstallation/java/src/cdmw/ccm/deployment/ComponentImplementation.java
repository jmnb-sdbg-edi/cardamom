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
import org.omg.Components.RemoveFailure;

import com.thalesgroup.CdmwDeployment.DELETE_ERROR;
import com.thalesgroup.CdmwDeployment.READ_WRITE_ERROR;


class ComponentImplementation
    implements java.io.Serializable
{


    private String implUUID;
    private java.io.File installDir;

    private java.util.Map installedLocations;
    private java.io.File code;
    private java.util.Map valuetypeFactories;


    public ComponentImplementation(String implUUID,
                                   java.io.File installDir)
    {
        this.implUUID = implUUID;
        this.installDir = installDir;
        installedLocations = new java.util.Hashtable();
        code = null;
        valuetypeFactories = new java.util.Hashtable();
    }

    public  String getUUID() {
        return implUUID;
    }


    public void installCode(String codeLocation)
        throws InstallationFailure
    {
        // check if code library has already been installed
        if (installedLocations.containsKey(codeLocation)) {
            code =
                (java.io.File) installedLocations.get(codeLocation);
        } else {
            // install code file
            code = installFile(codeLocation);
        }
    }


    public String getCodeLocation() {
        cdmw.common.Assert.check(code != null);
        return code.getAbsolutePath();
    }


    public void installValuetypeFactory(String factoryLocation,
                                        String repid)
        throws InstallationFailure
    {
        java.io.File factoryFile;

        // check if factory library has already been installed
        if (installedLocations.containsKey(factoryLocation)) {
            factoryFile =
                (java.io.File) installedLocations.get(factoryLocation);

        } else {
            // install factory
            factoryFile = installFile(factoryLocation);
        }

        // add factory to valuetypeFactories
        valuetypeFactories.put(repid, factoryFile);
    }


    public String getValuetypeFactoryLocation(String repid) 
        throws com.thalesgroup.CdmwDeployment.UnknownValuetype
    {
        java.io.File factoryFile = (java.io.File) valuetypeFactories.get(repid);
        if (factoryFile == null) {
            throw new com.thalesgroup.CdmwDeployment.UnknownValuetype(
                "No valuetype factory for " + repid + 
                " in component implementation " + implUUID);
        }
        return factoryFile.getAbsolutePath() ;
    }



    public void installDependency(String dependencyLocation)
        throws InstallationFailure
    {
        // check if dependency file has already been installed
        if (!installedLocations.containsKey(dependencyLocation)) {
            // install dependency file
            java.io.File dependency = installFile(dependencyLocation);
        }        
    }


    public void remove()
        throws org.omg.Components.RemoveFailure
    {
        // remove all implementation files
        try {
            FileUtilities.removeDirectoryRecursively(installDir);
            
        } catch (java.io.IOException e) {
            throw new org.omg.Components.RemoveFailure(
                e.getMessage(),
                DELETE_ERROR.value);
        }

        // empty lists
        installedLocations = new java.util.Hashtable();
        code = null;
        valuetypeFactories = new java.util.Hashtable();
    }



    private java.io.File installFile(String location)
        throws InstallationFailure
    {
        // get basename of file
        String name = location.replace('\\', '/');
        int index = name.lastIndexOf('/');
        name = name.substring(index);

        // dowload location
        java.io.File destination = new java.io.File(installDir, name);
        try {
            FileUtilities.downloadFile(location,
                                       destination.getAbsolutePath());
        } catch (cdmw.common.BadParameterException e) {
            throw new InstallationFailure(
                "Cannot access location : " + location,
                READ_WRITE_ERROR.value);
        } catch (java.io.IOException e) {
            throw new InstallationFailure(
                "Error while downloading " + location,
                READ_WRITE_ERROR.value);
        }

        // add File to list of installed locations
        installedLocations.put(location, destination);

        return destination;
    }


    void print() {
        System.out.println("implementation " + implUUID);
        System.out.println("  code : " + code.toString());
        System.out.println("  valuetype factories :");

        java.util.Iterator it = valuetypeFactories.entrySet().iterator();
        while (it.hasNext()) {
            java.util.Map.Entry e = (java.util.Map.Entry) it.next();
            String repid = (String) e.getKey();
            java.io.File factory = (java.io.File) e.getValue();

            System.out.println("     " + repid + " : " + factory.toString());
        }

        System.out.println("");
    }


}



