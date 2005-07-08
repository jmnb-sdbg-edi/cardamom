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
 * This class takes a template file, visits the IR and generates a file.
 */
public class CdmwIDL2Printer
    extends org.objectweb.ccm.visitorIDL3.idl.IDL3Printer {

    /**
     * Constructor.
     *
     * @param gen The generator class to use.
     * @param call_char The character to use to do a JAVA method call.
     * @param comment_char The character to use to start a comment.
     * @param indent The indentation to use.
     * @param filename The name of the file to generate.
     *
     * @exception fr.lifl.cim.jidlscript.lang.JISError
     */
    public CdmwIDL2Printer(org.objectweb.ccm.visitorIDL3.common.Generator gen,
                            char call_char,
                            char comment_char,
                            java.lang.String indent,
                            java.lang.String filename)
        throws fr.lifl.cim.jidlscript.lang.JISError {

        super(gen, call_char, comment_char, indent);

        // Load the template file.
        super.loadFile(filename);

        // Set the "self" alias in root scope on this object.
        super.setAlias("self", this);
    }

    /**
     * This method generates : #include "IFR_3_0.idl".
     */
    public java.lang.String printCORBAImport() {
        super.printImmediately("#include \"IFR_3_0.idl\"\\n");
        
        return "";
    }

    /**
     * This method generates : #include "Components.idl".
     */
    public java.lang.String printComponentsImport() {
        super.printImmediately("#include \"Components.idl\"\\n");

        return "";
    }

    /**
     * This method generates : #include "TypeCode.idl".
     */
    public java.lang.String printTypeCodeImport() {
        super.printImmediately("#include \"TypeCode.idl\"\\n");

        return "";
    }

    /**
     * This method tells the name of the script to use when
     * one want to generate CDMW IDL2.
     */
    public java.lang.String which() {
        return "ir3_cdmw_idl2";
    }

    /**
     * This method generates the #include instructions.
     */
    public java.lang.String imported() {
        java.lang.String[] objs = ((IDL3_CdmwIDL2) getGenerator()).getIncluded();
        java.lang.String res = "";
        
        for (int i = 0; i < objs.length; i++)
            res = res + "#include \"" + objs[i] + "\"\\n";

        res = res + "#include \"CdmwCcmCif.idl\"\\n";
        // res = res + "#include <CosEventComm.idl>\\n";

        if (!res.equals(""))
            res = res + "\\n";

        return res;
    }

    /**
     * This method generates the #include instructions.
     */
    public java.lang.String imported(org.objectweb.ccm.util.Vector v) {
        java.lang.String res = "";
        for (java.util.Enumeration e = v.elements(); e.hasMoreElements();) {
            java.util.StringTokenizer st = 
                new java.util.StringTokenizer((java.lang.String) e.nextElement());
            if (st.countTokens() == 2) {
                // skip the first token
                st.nextToken();
                res += "#include <" + st.nextToken() + ">\\n";
            }
        }

        if (!res.equals(""))
            res = res + "\\n";

        return res;
    }

    /**
     * This method generates the prefix to use for any CDMW IDL2.
     */
    public java.lang.String cdmwPragmaPrefix() {
        super.printImmediately("#pragma prefix \"thalesgroup.com\"\\n");
        return "";
    }

    /**
     * This method adds the prefix "Cdmw_" to a CORBA module name.
     *
     * @param decl The declaration to visit.
     */
    public java.lang.String
    addCdmwPrefix(org.objectweb.ccm.IDL3.Declaration decl) {
        if (decl.getParent().getDeclKind() == 
            org.objectweb.ccm.IDL3.DeclarationKind._dk_repository) {
            return "Cdmw_";
        }

        return "";
    }

    /**
     * This method returns the list of properties files.
     */
    public java.lang.String[] getPropertyFilesList() {
        return (((IDL3_CdmwIDL2) getGenerator()).getPropertyFiles());
    }
}


