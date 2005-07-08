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
 * This class generates CDMW-specific IDL2 from IDL3.
 */
public class IDL3_CdmwIDL2
    extends org.objectweb.ccm.visitorIDL3.common.Generator {

    /**
     * Name of the file to generate.
     */
    private java.lang.String filename_;
    
    /**
     * List of included IDLs.
     */
    private java.lang.String[] included_;

    /**
     * List of properties files.
     */
    private java.lang.String[] propfiles_;

    /**
     * Constructor.
     *
     * @param repository The Interface Repository.
     *
     * @exception fr.lifl.cim.jidlscript.lang.JISError
     */
    public IDL3_CdmwIDL2(org.objectweb.ccm.IDL3.Repository repository)
        throws fr.lifl.cim.jidlscript.lang.JISError {
        
        super(repository);
        
        // Create a printer
        CdmwIDL2Printer printer = new 
            CdmwIDL2Printer(this, '$', '#', "    ", "cdmwidl2.template");

        // Use this printer
        setPrinter(printer);
    }

    /**
     * Setters.
     */

    /**
     * This method sets the included IDLs.
     *
     * @param included List of included IDLs.
     */
    public void setIncluded(java.lang.String[] included) {
        included_ = included;
    }

    /**
     * This method sets the name of the file to generate.
     *
     * @param filename The name of the file to generate.
     */
    public void setFilename(java.lang.String filename) {
        filename_ = filename;
    }

    /**
     * This method sets the properties files.
     *
     * @param propfiles List of properties files.
     */
    public void setPropertyFiles(java.lang.String[] propfiles) {
        propfiles_ = propfiles;
    }

    /**
     * Getters.
     */

    /**
     * This method returns the included IDLs.
     */
    public java.lang.String[] getIncluded() {
        return included_;
    }

    /**
     * This method returns the name of the file to generate.
     */
    public java.lang.String getFilename() {
        return filename_;
    }

    /**
     * This method returns the properties files.
     */
    public java.lang.String[] getPropertyFiles() {
        return propfiles_;
    }

    /**
     * This method visits the Interface Repository
     * and generates the file.
     *
     * @param name The IR Object to visit.
     * @param filename The name of the file to generate.
     * @param included The list of included IDLs.
     * @param propfiles The list of properties files.
     */
    public void generate(java.lang.String name,
                         java.lang.String filename,
                         java.lang.String[] included,
                         java.lang.String[] propfiles) {
                         
        setIncluded(included);
        setFilename(filename);
        setPropertyFiles(propfiles);
        super.visitObj(name);
    }
}


