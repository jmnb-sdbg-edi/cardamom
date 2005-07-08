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

/**
 * Implementation of the DocumentDescription value factory
 *
 */
public class DocumentDescriptionFactoryImpl
    implements com.thalesgroup.CdmwXML.DocumentDescriptionValueFactory {
    
    /**
     * Is called by the ORB runtime while in the process 
     * of unmarshaling a value type.
     *
     * @param in an InputStream object from which the value will be read
     * @return The value read off of the Inputstream.
     */
    public java.io.Serializable
        read_value(org.omg.CORBA_2_3.portable.InputStream in) {
            java.io.Serializable desc = new DocumentDescriptionImpl();
            return in.read_value(desc);
    }
    
    /**
     * Creates a DocumentDescription valuetype.
     *
     * @param reference Basic reference to the DOM Document.
     * @param doctype Doctype of the Document.
     * @return The new DocumentDescription valuetype.
     */
    public com.thalesgroup.CdmwXML.DocumentDescription create(
        com.thalesgroup.CdmwXML.Document reference, short[] doctype) {
        return new DocumentDescriptionImpl(reference, doctype);
    }
    
    
}

