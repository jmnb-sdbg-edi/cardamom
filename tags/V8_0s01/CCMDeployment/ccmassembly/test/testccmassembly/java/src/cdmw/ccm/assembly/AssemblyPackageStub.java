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


package cdmw.ccm.assembly;

import com.thalesgroup.CdmwDeployment._ComponentPackage;
import com.thalesgroup.CdmwDeployment.LocationCreationException;
import com.thalesgroup.CdmwDeployment.FileNotReferenced;


class AssemblyPackageStub
    extends com.thalesgroup.CdmwDeployment._AssemblyPackagePOA
{
    
    private static final String DATA_DIR = "./";
    private static final String CAD_NAME = "cad.cad";


    private String uuid;
    private java.util.Map components;
    
    
    
    public AssemblyPackageStub(String uuid) {
        this.uuid = uuid;
        components = new java.util.Hashtable();
    }
    
    
    public void addComponentPackage(String componentfile_id,
                                    _ComponentPackage comp)
    {
        components.put(componentfile_id, comp);
    }
    
 
    public String
    get_link_location(String link_href)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public String
    get_fileinarchive_location(String name,
                               String archive_link)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }

    public String
    get_codebase_location(String filename)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }
                   

    public String
    get_uuid()
    {
        return uuid;
    }


    public String
    get_CAD_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               LocationCreationException
    {
        // find .cad file in assembly 'uuid' directory
        java.io.File dir = new java.io.File(DATA_DIR + uuid);
        if (!dir.isDirectory()) {
            throw new org.omg.Components.Deployment.InstallationFailure();
        }
        java.io.File[] cadFiles = 
            dir.listFiles(
                new java.io.FilenameFilter() {
                    public boolean accept(java.io.File dir, String name) {
                        return name.endsWith(".cad");
                    }
                }
            );
        
        if (cadFiles.length != 1) {
            throw new org.omg.Components.Deployment.InstallationFailure();
        }
        
        java.io.File cad = cadFiles[0];
        return cad.getAbsolutePath();
    }
    
    
    public _ComponentPackage
    get_component_package(String componentfile_id)
        throws FileNotReferenced
    {
        return (_ComponentPackage) components.get(componentfile_id);
    }

    public String
    get_home_CPF_location(String homeplacement_id)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        String cpfLocation = DATA_DIR + uuid + "/" + homeplacement_id + "Home.cpf";
        java.io.File cpf = new java.io.File(cpfLocation);
        if (cpf.exists()) {
            return cpf.getAbsolutePath();
        } else {
            throw new FileNotReferenced();
        }
    }

    public String
    get_component_CPF_location(String homeplacement_id)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        String cpfLocation = DATA_DIR + uuid + "/" + homeplacement_id + ".cpf";
        java.io.File cpf = new java.io.File(cpfLocation);
        if (cpf.exists()) {
            return cpf.getAbsolutePath();
        } else {
            throw new FileNotReferenced();
        }
    }
    

    public String
    get_instantiation_CPF_location(String componentinstantiation_id)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        String cpfLocation = DATA_DIR + uuid + "/" + componentinstantiation_id + ".cpf";
        java.io.File cpf = new java.io.File(cpfLocation);
        if (cpf.exists()) {
            return cpf.getAbsolutePath();
        } else {
            throw new FileNotReferenced();
        }
    }

    public String
    get_componentgroup_CPF_location(String componentgroup_id)
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        String cpfLocation = DATA_DIR + uuid + "/" + componentgroup_id + ".cpf";
        java.io.File cpf = new java.io.File(cpfLocation);
        if (cpf.exists()) {
            return cpf.getAbsolutePath();
        } else {
            throw new FileNotReferenced();
        }
    }
}

