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


package cdmw.xmlutils;

/**
 * Implementation of the CDATASectionDescription value factory
 *
 */
public class CDATASectionDescriptionFactoryImpl
    implements com.thalesgroup.CdmwXML.CDATASectionDescriptionValueFactory {
    
    /**
     * Is called by the ORB runtime while in the process 
     * of unmarshaling a value type.
     *
     * @param in an InputStream object from which the value will be read
     * @return The value read off of the Inputstream.
     */
    public java.io.Serializable
        read_value(org.omg.CORBA_2_3.portable.InputStream in) {
            java.io.Serializable desc = new CDATASectionDescriptionImpl();
            return in.read_value(desc);
    }
    
    /**
     * Creates a CDATASectionDescription valuetype.
     *
     * @param reference Basic reference to the DOM CDATASection.
     * @param data Value of the data of the CDATASection.
     * @return The new CDATASectionDescription valuetype.
     */
    public com.thalesgroup.CdmwXML.CDATASectionDescription create(
        com.thalesgroup.CdmwXML.CDATASection reference, short[] data) {
        return new CDATASectionDescriptionImpl(reference, data);
    }
    
    
}

