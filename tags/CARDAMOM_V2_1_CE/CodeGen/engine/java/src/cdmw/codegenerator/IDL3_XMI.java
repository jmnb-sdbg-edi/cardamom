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
 * This class generates XMI from IDL3.
 */
public class IDL3_XMI
    extends org.objectweb.ccm.visitorIDL3.common.Generator {

    /**
     * Name of the file to generate.
     */
    private java.lang.String filename_;

    /**
     * XMI DTD.
     */
    private java.lang.String xmidtd_;

    /**
     * Constructor.
     *
     * @param repository The Interface Repository.
     *
     * @exception fr.lifl.cim.jidlscript.lang.JISError
     */
    public IDL3_XMI(org.objectweb.ccm.IDL3.Repository repository)
        throws fr.lifl.cim.jidlscript.lang.JISError {
        
        super(repository);

        // Create a printer.
        XMIPrinter printer = new XMIPrinter(this, "    ");

        // Use this printer.
        setPrinter(printer);
    }

    /**
     * Setters.
     */

    /**
     * This method sets the XMI DTD.
     *
     * @param xmidtd The XMI DTD.
     */
    public void setDTD(java.lang.String xmidtd) {
        xmidtd_ = xmidtd;
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
     * Getters.
     */

    /**
     * This method returns the XMI DTD.
     */
    public java.lang.String getDTD() {
        return xmidtd_;
    }

    /**
     * This method returns the filename.
     */
    public java.lang.String getFilename() {
        return filename_;
    }

    /**
     * This method visits the Interface Repository
     * and generates the file.
     *
     * @param name The IR Object to visit.
     * @param filename The name of the file to generate.
     * @param xmidtd The XMI DTD.
     */
    public void generate(java.lang.String name,
                         java.lang.String filename,
                         java.lang.String xmidtd) {
                         
        setDTD(xmidtd);
        setFilename(filename);
        super.visitObj(name);
    }
}


