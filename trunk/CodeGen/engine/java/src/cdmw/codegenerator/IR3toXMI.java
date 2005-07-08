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


package cdmw.codegenerator;

/**
 * This class is called by the ir3_xmi shell script.
 */
public class IR3toXMI
    extends org.objectweb.ccm.CORBA.Application {
    
    /**
     * Default constructor.
     */
    public IR3toXMI() {
    }

    /**
     * The main bootstrap method.
     *
     * @param args The command line arguments.
     */
    public static void main(java.lang.String[] args) {
        IR3toXMI ir3 = new IR3toXMI();
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

        // Create an OpenCCM XMI Compiler
        XMICompiler compiler = new XMICompiler(
            new IR3XMIOptionsManager(org.objectweb.ccm.scripts.Version.label, "ir3_xmi"),
            org.objectweb.ccm.scripts.Version.label);

        // Run the compiler.
        boolean ok = compiler.run(args);
        if (!ok) {
            return -1;
        }

        // Set the default filename.
        String filename = "stdout";
        String xmidtd = "Xmi.dtd";

        // Create an options manager.
        IR3XMIOptionsManager manager = compiler.getManager();

        // Deals with the options.
        if (manager.isSet("-o")) {
            filename = manager.get("-o")[0];
        }

        if (manager.isSet("-dtd")) {
            xmidtd = manager.get("-dtd")[0];
        }

        // Start an IDL3 Repository
        org.objectweb.ccm.IDL3.Repository rep = 
            new org.objectweb.ccm.IDL3.Repository(repositoryRef.as_IDL3_repository());

        try
        {
            cdmw.codegenerator.IDL3_XMI mi = 
                new cdmw.codegenerator.IDL3_XMI(rep);

            mi.generate(manager.getObjectName(),
                        filename,
                        xmidtd);
        }
        catch(fr.lifl.cim.jidlscript.lang.JISError ex)
        {
            System.err.println(ex.exception.toString());
            return 1;
        }

        return 0;
    }
}


