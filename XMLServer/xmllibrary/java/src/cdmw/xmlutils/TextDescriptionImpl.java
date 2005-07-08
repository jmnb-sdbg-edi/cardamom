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

public class TextDescriptionImpl 
    extends com.thalesgroup.CdmwXML.TextDescription {
    
    /**
     * Default constructor.
     */
    public TextDescriptionImpl() {}

    /**
     * Constructor
     *
     * @param reference Basic reference to the DOM Text.
     * @param data Value of the data of the Text.
     */
    public TextDescriptionImpl(
        com.thalesgroup.CdmwXML.Text reference, short[] data) {
        this.s_reference = reference;
        this.s_data = data;
    }

    //
    // IDL:thalesgroup.com/CdmwXML/TextDescription/data:1.0
    //
    /**
     * Returns the value of the data of the Text.
     *
     * @return The value of the data of the Text.
     */
    public short[] data() {
        return this.s_data;
    }

    /**
     * Sets the value of the data of the Text.
     *
     * @param val New value of the data.
     */
    public void data(short[] val) {
        this.s_data = val;
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
        return com.thalesgroup.CdmwXML.TEXT_NODE.value;
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

