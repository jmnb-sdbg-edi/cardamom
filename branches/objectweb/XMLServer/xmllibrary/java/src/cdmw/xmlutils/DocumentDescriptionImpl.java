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


package cdmw.xmlutils;

public class DocumentDescriptionImpl 
    extends com.thalesgroup.CdmwXML.DocumentDescription {

    /**
     * Default constructor.
     */    
    public DocumentDescriptionImpl() {}

    /**
     * Constructor
     *
     * @param reference Basic reference to the DOM Document.
     * @param doctype Name of the DTD; 
     * i.e., the name immediately following the DOCTYPE keyword.
     */
    public DocumentDescriptionImpl(
        com.thalesgroup.CdmwXML.Document reference, short[] doctype) {
        this.s_reference = reference;
        this.s_doctype = doctype;
    }

    //
    // IDL:thalesgroup.com/CdmwXML/DocumentDescription/doctype:1.0
    //
    /**
     * Returns the name of the doctype.
     *
     * @return The name of the doctype.
     */
    public short[] doctype() {
        return this.s_doctype;
    }

    //
    // IDL:thalesgroup.com/CdmwXML/NodeDescription/type:1.0
    //
    /**
     * Returns the type of the node.
     *
     * @return The type of the node.
     */
    public short type() {
        return com.thalesgroup.CdmwXML.DOCUMENT_NODE.value;
    }

    //
    // IDL:thalesgroup.com/CdmwXML/NodeDescription/reference:1.0
    //
    /**
     * Returns the reference to the DOM node.
     *
     * @return The reference to the DOM node.
     */
    public com.thalesgroup.CdmwXML.Node reference() {
        return this.s_reference;
    }

}

