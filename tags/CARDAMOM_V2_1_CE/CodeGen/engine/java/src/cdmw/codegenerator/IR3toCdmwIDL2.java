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


package cdmw.codegenerator;

/**
 * This class is called by the ir3_cdmwidl2 shell script.
 */
public class IR3toCdmwIDL2
    extends org.objectweb.ccm.CORBA.Application {

    /**
     * Default constructor.
     */
    public IR3toCdmwIDL2() {
    }

    /**
     * The main bootstrap method.
     *
     * @param args The command line arguments.
     */
    public static void main(java.lang.String[] args) {
        IR3toCdmwIDL2 ir3 = new IR3toCdmwIDL2();
        ir3.start(args);
    }

    /**
     * Overridden org.objectweb.ccm.CORBA.Application methods.
     */

    /**
     * This method runs the application.
     *
     * @param args The command line arguments.
     */
    public int run(java.lang.String[] args) {
        // Obtain the Interface Repository.
        org.objectweb.ccm.ComponentRepository repositoryRef =
            org.objectweb.ccm.ComponentRepositoryHelper.narrow(
                org.objectweb.ccm.CORBA.TheInterfaceRepository.getRepository());

        // Create an OMG IDL Compiler.
        org.objectweb.ccm.scripts.IDL3Compiler compiler = 
            new org.objectweb.ccm.scripts.IDL3Compiler();

        // Run the compiler.
        boolean ok = compiler.run(args);
        if (!ok) {
            return -1;
        }

        // Set the default filename.
        String filename = "stdout";

        // Create an options manager.
        org.objectweb.ccm.scripts.IDL3OptionsManager manager = 
            compiler.getManager();
        
        // Deals with the options.
        if (manager.isSet("-o")) {
            filename = manager.get("-o")[0];
        }

        String[] includes = new String[0];
        if (manager.isSet("-i"))
            includes = manager.get("-i");

        java.lang.String[] propfiles = new java.lang.String[0];
        if (manager.isSet("-f"))
            propfiles = manager.get("-f");

        java.io.File f = null;
        
        if (!filename.equals("stdout")) {
            java.util.Vector v = new java.util.Vector();
            for (int i = 0; i < includes.length; i++) {
                v.add(includes[i]);
            }
            
            f = new java.io.File(filename);
            v.add(f.getName());
            includes = (java.lang.String[]) v.toArray(new String[0]);
        }
      
        // Start an IDL3 Repository
        org.objectweb.ccm.IDL3.Repository rep = null;

        try {
            java.lang.String cdmwPrefix = "Cdmw_";
      
            // IDL3
            rep = new org.objectweb.ccm.IDL3.Repository(repositoryRef.as_IDL3_repository());
      
            cdmw.codegenerator.IDL3_CdmwIDL2 mi_cdmw = 
                new cdmw.codegenerator.IDL3_CdmwIDL2(rep);

            if (filename.equals("stdout")) {
                mi_cdmw.generate(manager.getObjectName(),
                                  filename,
                                  includes,
                                  propfiles);
            } else {
                if (f.getParent() == null) {
                    mi_cdmw.generate(manager.getObjectName(),
                                      cdmwPrefix + f.getName(),
                                      includes,
                                      propfiles);
                } else {
                    mi_cdmw.generate(manager.getObjectName(),
                                      f.getParent() + java.io.File.separator + cdmwPrefix + f.getName(),
                                      includes,
                                      propfiles);
                }
            }

            // IDL2
            rep = new org.objectweb.ccm.IDL3.Repository(repositoryRef.as_IDL2_repository());

            org.objectweb.ccm.visitorIDL3.idl.IDL3_IDL2 mi = 
                new org.objectweb.ccm.visitorIDL3.idl.IDL3_IDL2(rep);

            mi.generate(manager.getObjectName(),
                        filename,
                        new String[0],
                        propfiles);
        } catch(fr.lifl.cim.jidlscript.lang.JISError ex) {
            System.err.println(ex.exception.toString());
            return 1;
        }

        return 0;
    }
}


