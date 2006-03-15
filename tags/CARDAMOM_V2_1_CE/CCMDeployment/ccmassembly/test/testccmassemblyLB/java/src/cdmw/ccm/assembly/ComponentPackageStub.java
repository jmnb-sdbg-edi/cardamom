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

import com.thalesgroup.CdmwDeployment.LocationCreationException;
import com.thalesgroup.CdmwDeployment.FileNotReferenced;



class ComponentPackageStub
    extends com.thalesgroup.CdmwDeployment._ComponentPackagePOA
{
    
    private static final String DATA_DIR = "./";
    


    private String name;
    private String version;


    public ComponentPackageStub(String name, String version) {
        this.name = name;
        this.version = version;
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
    get_name()
    {
        return name;
    }
    

    public String
    get_version()
    {
        return version;
    }    

    public String
    get_CSD_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               LocationCreationException
    {
        java.io.File csd = new java.io.File(DATA_DIR + name + version + 
                                            "/" + name + ".csd");
        return csd.getAbsolutePath();
    }
    

    public String
    get_CCD_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        java.io.File ccd = new java.io.File(DATA_DIR + name + version + 
                                            "/" +  name + ".ccd");
        return ccd.getAbsolutePath();
    }
    

    public String
    get_code_location(String implUUID)
        throws org.omg.Components.Deployment.UnknownImplId,
               org.omg.Components.Deployment.InstallationFailure,
               LocationCreationException
    {
        throw new org.omg.CORBA.NO_IMPLEMENT();
    }
    

    public String
    get_component_CPF_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        String cpfLocation = DATA_DIR + name + version + "/" + name + ".cpf";
        java.io.File cpf = new java.io.File(cpfLocation);
        if (cpf.exists()) {
            return cpf.getAbsolutePath();
        } else {
            throw new FileNotReferenced();
        }
    }
    

    public String
    get_home_CPF_location()
        throws org.omg.Components.Deployment.InstallationFailure,
               FileNotReferenced,
               LocationCreationException
    {
        String cpfLocation = DATA_DIR + name + version + "/" + name + "Home.cpf";
        java.io.File cpf = new java.io.File(cpfLocation);
        if (cpf.exists()) {
            return cpf.getAbsolutePath();
        } else {
            throw new FileNotReferenced();
        }
    }
    
    
    
}

